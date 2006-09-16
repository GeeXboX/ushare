/*
 * cfgparser.c : GeeXboX uShare config file parser.
 * Originally developped for the GeeXboX project.
 * Copyright (C) 2005-2006 Alexis Saettler <asbin@asbin.org>
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#error "Missing config.h file : run configure again"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdbool.h>
#include <limits.h>

#include "gettext.h"
#include "cfgparser.h"
#include "ushare.h"
#include "trace.h"

#define USHARE_DIR_DELIM ","

static bool
ignore_line (const char *line)
{
  int i;
  size_t len;

  /* commented line */
  if (line[0] == '#' )
    return true;

  len = strlen (line);

  for (i = 0 ; i < (int) len ; i++ )
    if (line[i] != ' ' && line[i] != '\t' && line[i] != '\n')
      return false;

  return true;
}

static char *
strdup_trim (const char *s)
{
  size_t begin, end;
  char *r = NULL;

  if (!s)
    return NULL;

  end = strlen (s) - 1;

  for (begin = 0 ; begin < end ; begin++)
    if (s[begin] != ' ' && s[begin] != '\t' && s[begin] != '"')
      break;

  for (; begin < end ; end--)
    if (s[end] != ' ' && s[end] != '\t' && s[end] != '"' && s[end] != '\n')
      break;

  r = strndup (s + begin, end - begin + 1);

  return r;
}

static void
ushare_set_name (struct ushare_t *ut, const char *name)
{
  if (!ut || !name)
    return;

  if (ut->name)
  {
    free (ut->name);
    ut->name = NULL;
  }

  ut->name = strdup_trim (name);
}

static void
ushare_set_interface (struct ushare_t *ut, const char *iface)
{
  if (!ut || !iface)
    return;

  if (ut->interface)
  {
    free (ut->interface);
    ut->interface = NULL;
  }

  ut->interface = strdup_trim (iface);
}

static void
ushare_add_contentdir (struct ushare_t *ut, const char *dir)
{
  if (!ut || !dir)
    return;

  ut->contentlist = add_content (ut->contentlist, dir);
}

static void
ushare_set_dir (struct ushare_t *ut, const char *dirlist)
{
  char *x = NULL, *token = NULL;
  char *m_buffer = NULL, *buffer;

  if (!ut || !dirlist)
    return;

  x = strdup_trim (dirlist);
  if (x)
  {
    m_buffer = (char*) malloc (strlen (x) * sizeof (char));
    if (m_buffer)
    {
      buffer = m_buffer;
      token = strtok_r (x, USHARE_DIR_DELIM, &buffer);
      while (token)
      {
        ushare_add_contentdir (ut, token);
        token = strtok_r (NULL, USHARE_DIR_DELIM, &buffer);
      }
      free (m_buffer);
    }
    free (x);
  }
}

static void
ushare_set_port (struct ushare_t *ut, const char *port)
{
  if (!ut || !port)
    return;

  ut->port = atoi (port);
  if (ut->port <= 49152)
  {
    fprintf (stderr,
             _("Warning: port doesn't fit IANA port assignements.\n"));

    fprintf (stderr, _("Warning: Only Dynamic or Private Ports can be used "
                       "(from 49152 through 65535)\n"));
    ut->port = 0;
  }
}

static u_configline_t configline[] = {
  {USHARE_NAME, ushare_set_name},
  {USHARE_IFACE, ushare_set_interface},
  {USHARE_PORT, ushare_set_port},
  {USHARE_DIR, ushare_set_dir},
  {NULL, NULL},
};

static void
parse_config_line (struct ushare_t *ut, const char *line,
                   u_configline_t *configline)
{
  char *s = NULL;

  s = strchr (line, '=');
  if (s && s[1] != '\0')
  {
    int i;
    for (i=0 ; configline[i].name ; i++)
    {
      if (!strncmp (line, configline[i].name, strlen (configline[i].name)))
      {
        configline[i].set_var (ut, s + 1);
        break;
      }
    }
  }
}

int
parse_config_file (struct ushare_t *ut)
{
  char filename[PATH_MAX];
  FILE *conffile;
  char *line = NULL;
  size_t size = 0;
  ssize_t read;

  if (!ut)
    return -1;

  snprintf (filename, PATH_MAX, "%s/%s", SYSCONFDIR, USHARE_CONFIG_FILE);

  conffile = fopen (filename, "r");
  if (!conffile)
    return -1;

  while ((read = getline (&line, &size, conffile)) != -1)
  {
    if (ignore_line (line))
      continue;

    if (line[read-1] == '\n')
      line[read-1] = '\0';

    while (line[0] == ' ' || line[0] == '\t')
      line++;

    parse_config_line (ut, line, configline);
  }

  fclose (conffile);

  if (line)
    free (line);

  return 0;
}

inline static void
display_usage (void)
{
  display_headers ();
  printf ("\n");
  printf (_("Usage: ushare [-n name] [-i interface] [-p port] [-c directory] [[-c directory]...]\n"));
  printf (_("Options:\n"));
  printf (_(" -n, --name=NAME\tSet UPnP Friendly Name (default is '%s')\n"),
          DEFAULT_USHARE_NAME);
  printf (_(" -i, --interface=IFACE\tUse IFACE Network Interface (default is '%s')\n"),
          DEFAULT_USHARE_IFACE);
  printf (_(" -p, --port=PORT\tForces the HTTP server to run on PORT\n"));
  printf (_(" -c, --content=DIR\tShare the content of DIR directory\n"));
  printf (_(" -w, --no-web\t\tDisable the control web page (enabled by default)\n"));
  printf (_(" -v, --verbose\t\tSet verbose display\n"));
  printf (_(" -D, --daemon\t\tRun as a daemon\n"));
  printf (_(" -V, --version\t\tDisplay the version of uShare and exit\n"));
  printf (_(" -h, --help\t\tDisplay this help\n"));
}

int
parse_command_line (struct ushare_t *ut, int argc, char **argv)
{
  int c, index;
  char short_options[] = "VhvDwn:i:p:c:";
  struct option long_options [] = {
    {"version", no_argument, 0, 'V' },
    {"help", no_argument, 0, 'h' },
    {"verbose", no_argument, 0, 'v' },
    {"daemon", no_argument, 0, 'D' },
    {"name", required_argument, 0, 'n' },
    {"interface", required_argument, 0, 'i' },
    {"port", required_argument, 0, 'p' },
    {"content", required_argument, 0, 'c' },
    {"no-web", no_argument, 0, 'w' },
    {0, 0, 0, 0 }
  };

  /* command line argument processing */
  while (true)
  {
    c = getopt_long (argc, argv, short_options, long_options, &index);

    if (c == EOF)
      break;

    switch (c)
    {
    case 0:
      /* opt = long_options[index].name; */
      break;

    case '?':
    case 'h':
      display_usage ();
      return -1;

    case 'V':
      display_headers ();
      return -1;

    case 'v':
      ut->verbose = true;
      break;

    case 'D':
      ut->daemon = true;
      break;

    case 'n':
      ushare_set_name (ut, optarg);
      break;

    case 'i':
      ushare_set_interface (ut, optarg);
      break;

    case 'p':
      ushare_set_port (ut, optarg);
      break;

    case 'c':
      ushare_add_contentdir (ut, optarg);
      break;

    case 'w':
      ut->use_presentation = false;
      break;

    default:
      break;
    }
  }

  return 0;
}
