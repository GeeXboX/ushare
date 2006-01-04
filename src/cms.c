/*
 * cms.c : GeeXboX uShare Connection Management Service.
 * Originally developped for the GeeXboX project.
 * Parts of the code are originated from GMediaServer from Oskar Liljeblad.
 * Copyright (C) 2005-2006 Benjamin Zores <ben@geexbox.org>
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

#include <upnp/upnp.h>
#include <upnp/upnptools.h>

#include "ushare.h"
#include "services.h"
#include "mime.h"

/* Represent the CMS GetProtocolInfo action. */
#define SERVICE_CMS_ACTION_PROT_INFO "GetProtocolInfo"

/* Represent the CMS GetCurrentConnectionIDs action. */
#define SERVICE_CMS_ACTION_CON_ID "GetCurrentConnectionIDs"

/* Represent the CMS GetCurrentConnectionInfo action. */
#define SERVICE_CMS_ACTION_CON_INFO "GetCurrentConnectionInfo"

/* Represent the CMS SOURCE argument. */
#define SERVICE_CMS_ARG_SOURCE "SOURCE"

/* Represent the CMS SINK argument. */
#define SERVICE_CMS_ARG_SINK "SINK"

/* Represent the CMS ConnectionIDs argument. */
#define SERVICE_CMS_ARG_CONNECTION_IDS "ConnectionIDs"

/* Represent the CMS ConnectionID argument. */
#define SERVICE_CMS_ARG_CONNECTION_ID "ConnectionID"

/* Represent the CMS RcsID argument. */
#define SERVICE_CMS_ARG_RCS_ID "RcsID"

/* Represent the CMS AVTransportID argument. */
#define SERVICE_CMS_ARG_TRANSPORT_ID "AVTransportID"

/* Represent the CMS ProtocolInfo argument. */
#define SERVICE_CMS_ARG_PROT_INFO "ProtocolInfo"

/* Represent the CMS PeerConnectionManager argument. */
#define SERVICE_CMS_ARG_PEER_CON_MANAGER "PeerConnectionManager"

/* Represent the CMS PeerConnectionID argument. */
#define SERVICE_CMS_ARG_PEER_CON_ID "PeerConnectionID"

/* Represent the CMS Direction argument. */
#define SERVICE_CMS_ARG_DIRECTION "Direction"

/* Represent the CMS Status argument. */
#define SERVICE_CMS_ARG_STATUS "Status"

/* Represent the CMS default connection ID value. */
#define SERVICE_CMS_DEFAULT_CON_ID "0"

/* Represent the CMS unknown connection ID value. */
#define SERVICE_CMS_UNKNOW_ID "-1"

/* Represent the CMS Output value. */
#define SERVICE_CMS_OUTPUT "Output"

/* Represent the CMS Success Status. */
#define SERVICE_CMS_STATUS_OK "OK"

static bool
cms_get_protocol_info (struct action_event_t *event)
{
  extern struct mime_type_t MIME_Type_List[];
  struct mime_type_t *list = MIME_Type_List;

  if (!event)
    return false;

  while (list->extension)
  {
    upnp_add_response (event, SERVICE_CMS_ARG_SOURCE, (*list).protocol);
    *list++;
  }

  upnp_add_response (event, SERVICE_CMS_ARG_SINK, "");

  return event->status;
}

static bool
cms_get_current_connection_ids (struct action_event_t *event)
{
  if (!event)
    return false;

  upnp_add_response (event, SERVICE_CMS_ARG_CONNECTION_IDS, "");

  return event->status;
}

static bool
cms_get_current_connection_info (struct action_event_t *event)
{
  extern struct mime_type_t MIME_Type_List[];
  struct mime_type_t *list = MIME_Type_List;

  if (!event)
    return false;

  upnp_add_response (event, SERVICE_CMS_ARG_CONNECTION_ID,
                     SERVICE_CMS_DEFAULT_CON_ID);
  upnp_add_response (event, SERVICE_CMS_ARG_RCS_ID, SERVICE_CMS_UNKNOW_ID);
  upnp_add_response (event, SERVICE_CMS_ARG_TRANSPORT_ID,
                     SERVICE_CMS_UNKNOW_ID);

  while (list->extension)
  {
    upnp_add_response (event, SERVICE_CMS_ARG_PROT_INFO, (*list).protocol);
    *list++;
  }

  upnp_add_response (event, SERVICE_CMS_ARG_PEER_CON_MANAGER, "");
  upnp_add_response (event, SERVICE_CMS_ARG_PEER_CON_ID,
                     SERVICE_CMS_UNKNOW_ID);
  upnp_add_response (event, SERVICE_CMS_ARG_DIRECTION, SERVICE_CMS_OUTPUT);
  upnp_add_response (event, SERVICE_CMS_ARG_STATUS, SERVICE_CMS_STATUS_OK);

  return event->status;
}

/* List of UPnP ConnectionManager Service actions */
struct service_action_t cms_service_actions[] = {
  { SERVICE_CMS_ACTION_PROT_INFO, cms_get_protocol_info },
  { SERVICE_CMS_ACTION_CON_ID, cms_get_current_connection_ids },
  { SERVICE_CMS_ACTION_CON_INFO, cms_get_current_connection_info },
  { NULL, NULL }
};
