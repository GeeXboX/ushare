/*
 * ushare.c : GeeXboX uShare UPnP Media Server.
 * Originally developped for the GeeXboX project.
 * Parts of the code are originated from GMediaServer from Oskar Liljeblad.
 * Copyright (C) 2005 Benjamin Zores <ben@geexbox.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <upnp/upnp.h>
#include <upnp/upnptools.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#error "Missing config.h file : run configure again"
#endif

#include "ushare.h"
#include "services.h"
#include "http.h"
#include "metadata.h"
#include "util_iconv.h"

#if HAVE_SETLOCALE && ENABLE_NLS
# include <locale.h>
#endif

#include "gettext.h"
#define _(string) gettext (string)

static UpnpDevice_Handle dev;
static char *deviceUDN;

static void
handle_action_request (struct Upnp_Action_Request *request)
{
  struct service_t *service;
  struct service_action_t *action;
#ifdef DEBUG
  char val[256];
  int ip;
#endif /* DEBUG */

  if (!request)
    return;

  if (request->ErrCode != UPNP_E_SUCCESS)
    return;

  if (strcmp (request->DevUDN + 5, deviceUDN))
    return;

#ifdef DEBUG
  ip = request->CtrlPtIPAddr.s_addr;
  ip = ntohl (ip);
  sprintf (val, "%d.%d.%d.%d",
           (ip >> 24) & 0xFF, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF);

  printf ("ServiceID : %s\n", request->ServiceID);
  printf ("actionName : %s\n", request->ActionName);
  printf ("CtrlPtIP : %s\n", val);
#endif /* DEBUG */

  if (find_service_action (request, &service, &action))
    {
      struct action_event_t event;

      event.request = request;
      event.status = true;
      event.service = service;

      if (action->function (&event) && event.status)
        request->ErrCode = UPNP_E_SUCCESS;

      return;
    }

  if (service) /* Invalid Action name */
    strcpy (request->ErrStr, "Unknown Service Action");
  else /* Invalid Service name */
    strcpy (request->ErrStr, "Unknown Service ID");

  request->ActionResult = NULL;
  request->ErrCode = UPNP_SOAP_E_INVALID_ACTION;
}

static int
device_callback_event_handler (Upnp_EventType type, void *event,
                               void *cookie __attribute__((unused)))
{
  switch (type)
    {
    case UPNP_CONTROL_ACTION_REQUEST:
      handle_action_request ((struct Upnp_Action_Request *) event);
      break;
    case UPNP_CONTROL_ACTION_COMPLETE:
    case UPNP_EVENT_SUBSCRIPTION_REQUEST:
    case UPNP_CONTROL_GET_VAR_REQUEST:
      break;
    default:
      break;
    }

  return 0;
}

int
finish_upnp (void)
{
  printf (_("Stopping UPnP Service ...\n"));
  UpnpUnRegisterRootDevice (dev);
  UpnpFinish ();

  if (deviceUDN)
    free (deviceUDN);

  return UPNP_E_SUCCESS;
}

int
init_upnp (char *name, char *udn, char *ip)
{
  char *description = NULL;
  int len, res;

  if (!name || !udn || !ip)
    return -1;

  deviceUDN = strdup (udn);
  if (!deviceUDN)
    return -1;

  len = strlen (UPNP_DESCRIPTION) + strlen (name) + strlen (udn) + 1;
  description = (char *) malloc (len * sizeof (char));
  memset (description, 0, len);
  sprintf (description, UPNP_DESCRIPTION, name, udn);

  printf (_("Initializing UPnP subsystem ...\n"));
  res = UpnpInit (ip, 0);
  if (res != UPNP_E_SUCCESS)
  {
    printf (_("Cannot initialize UPnP subsystem\n"));
    return -1;
  }

  printf (_("UPnP MediaServer listening on %s:%d\n"),
          UpnpGetServerIpAddress (), UpnpGetServerPort());

  UpnpEnableWebserver (TRUE);

  res = UpnpSetVirtualDirCallbacks (&virtual_dir_callbacks);
  if (res != UPNP_E_SUCCESS)
  {
    printf (_("Cannot set virtual directory callbacks\n"));
    free (description);
    return -1;
  }

  res = UpnpAddVirtualDir (VIRTUAL_DIR);
  if (res != UPNP_E_SUCCESS)
  {
    printf (_("Cannot add virtual directory for web server\n"));
    free (description);
    return -1;
  }

  res = UpnpRegisterRootDevice2 (UPNPREG_BUF_DESC, description, 0, 1,
                                 device_callback_event_handler, NULL, &dev);
  if (res != UPNP_E_SUCCESS)
  {
    printf (_("Cannot register UPnP device\n"));
    free (description);
    return -1;
  }

  res = UpnpUnRegisterRootDevice (dev);
  if (res != UPNP_E_SUCCESS)
  {
    printf (_("Cannot unregister UPnP device\n"));
    free (description);
    return -1;
  }

  res = UpnpRegisterRootDevice2 (UPNPREG_BUF_DESC, description, 0, 1,
                                 device_callback_event_handler, NULL, &dev);
  if (res != UPNP_E_SUCCESS)
  {
    printf (_("Cannot register UPnP device\n"));
    free (description);
    return -1;
  }

  printf (_("Sending UPnP advertisement for device ...\n"));
  UpnpSendAdvertisement (dev, 1800);

  printf (_("Listening for control point connections ...\n"));

  if (description)
    free (description);

  return 0;
}

char *
create_udn (char *interface)
{
  int sock;
  struct ifreq ifr;
  char *buf;
  unsigned char *ptr;

  if (!interface)
    return NULL;

  /* determine UDN according to MAC address */
  sock = socket (AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
  {
    perror ("socket");
    exit (-1);
  }

  strcpy (ifr.ifr_name, interface);
  strcpy (ifr.ifr_hwaddr.sa_data, "");

  if (ioctl (sock, SIOCGIFHWADDR, &ifr) < 0)
  {
    perror ("ioctl");
    exit (-1);
  }

  buf = (char *) malloc (64 * sizeof (char));
  memset (buf, 0, 64);
  ptr = (unsigned char *) ifr.ifr_hwaddr.sa_data;

  snprintf (buf, 64, "%02X:%02X:%02X:%02X:%02X:%02X",
            (ptr[0] & 0377), (ptr[1] & 0377), (ptr[2] & 0377),
            (ptr[3] & 0377), (ptr[4] & 0377), (ptr[5] & 0377));

  return buf;
}

char *
get_iface_address (char *interface)
{
  int sock, ip;
  struct ifreq ifr;
  char *val;

  if (!interface)
    return NULL;

  /* determine UDN according to MAC address */
  sock = socket (AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
  {
    perror ("socket");
    exit (-1);
  }

  strcpy (ifr.ifr_name, interface);
  ifr.ifr_addr.sa_family = AF_INET;

  if (ioctl (sock, SIOCGIFADDR, &ifr) < 0)
  {
    perror ("ioctl");
    exit (-1);
  }

  val = (char *) malloc (16);
  ip = ((struct sockaddr_in *) &ifr.ifr_addr)->sin_addr.s_addr;
  ip = ntohl (ip);
  sprintf (val, "%d.%d.%d.%d",
           (ip >> 24) & 0xFF, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF);

  return val;
}

static void
UPnPBreak (int s __attribute__ ((unused)))
{
  finish_upnp ();
  free_metadata_list ();
  finish_iconv();

  exit (0);
}

static void
display_headers (void)
{
  printf (_("%s (version %s), a lightweight UPnP Media Server.\n"),
          PACKAGE_NAME, VERSION);
  printf (_("Benjamin Zores (C) 2005, for GeeXboX Team.\n"));
  printf (_("See http://ushare.geexbox.org/ for updates.\n"));
}

static void
display_usage (void)
{
  display_headers ();
  printf (_("\nOptions :\n"));
  printf (_("   -n, --name :\t\tSet UPnP Friendly Name (default is 'uShare')\n"));
  printf (_("   -i, --interface :\tSet Network Interface (default is 'eth0')\n"));
  printf (_("   -c, --content :\tSet the content directory to be shared (default is './')\n"));
  printf (_("   -v, --version :\tDisplay the version of uShare and exit.\n"));
  printf (_("   -h, --help :\t\tDisplay this help\n"));

  exit (0);
}

void
setup_i18n(void)
{
#if HAVE_SETLOCALE && ENABLE_NLS
  setlocale (LC_ALL, "");
#endif
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);
}

int
main (int argc, char **argv)
{
  char *udn = NULL, *ip = NULL;
  char *name = NULL, *interface = NULL;
  char *content = NULL;
  int c,index;
  char short_options[] = "vhn:i:c:";
  struct option long_options [] = {
    {"version", no_argument, 0, 'v' },
    {"help", no_argument, 0, 'h' },
    {"name", required_argument, 0, 'n' },
    {"interface", required_argument, 0, 'i' },
    {"content", required_argument, 0, 'c' },
    {0, 0, 0, 0 }
  };

  setup_i18n();
  setup_iconv();

  /* command line argument processing */
  while (1)
    {
      c = getopt_long(argc, argv, short_options, long_options, &index);

      if (c == EOF)
        break;

      switch (c)
        {
        case 0:
          /* opt = long_options[index].name; */
          break;

        case '?':
        case ':':
        case 'h':
          display_usage ();
          return 0;

        case 'v':
          display_headers ();
          return 0;

        case 'n':
          if (!optarg)
            return -1;
          name = strdup (optarg);
          break;

        case 'i':
          if (!optarg)
            return -1;
          interface = strdup (optarg);
          break;

        case 'c':
          if (!optarg)
            return -1;
          content = strdup (optarg);
          break;

        default:
          return -1;
        }
    }

  if (!content)
    content = strdup ("./");
  if (!name)
    name = strdup ("uShare");
  if (!interface)
    interface = strdup ("eth0");

  udn = create_udn (interface);
  if (!udn)
  {
    free (content);
    free (name);
    free (interface);
    return -1;
  }

  ip = get_iface_address (interface);
  if (!ip)
  {
    free (content);
    free (name);
    free (interface);
    free (udn);
    return -1;
  }

  signal (SIGINT, UPnPBreak);

  display_headers ();
  if (init_upnp (name, udn, ip) < 0)
  {
    finish_upnp ();
    free (content);
    free (name);
    free (interface);
    free (udn);
    free (ip);
    return -1;
  }
  free (name);
  free (interface);
  free (udn);
  free (ip);

  build_metadata_list (content);
  free (content);

  while (1)
    sleep (1000000);

  /* it should never be executed */
  return 0;
}
