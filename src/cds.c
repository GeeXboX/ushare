/*
 * cds.c : GeeXboX uShare Content Directory Service
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
#include "ushare.h"
#include "services.h"
#include "metadata.h"
#include "mime.h"
#include "buffer.h"
#include "minmax.h"

/* Represent the CDS GetSearchCapabilities action. */
#define SERVICE_CDS_ACTION_SEARCH_CAPS "GetSearchCapabilities"

/* Represent the CDS GetSortCapabilities action. */
#define SERVICE_CDS_ACTION_SORT_CAPS "GetSortCapabilities"

/* Represent the CDS GetSystemUpdateID action. */
#define SERVICE_CDS_ACTION_UPDATE_ID "GetSystemUpdateID"

/* Represent the CDS Browse action. */
#define SERVICE_CDS_ACTION_BROWSE "Browse"

/* Represent the CDS SearchCaps argument. */
#define SERVICE_CDS_ARG_SEARCH_CAPS "SearchCaps"

/* Represent the CDS SortCaps argument. */
#define SERVICE_CDS_ARG_SORT_CAPS "SortCaps"

/* Represent the CDS UpdateId argument. */
#define SERVICE_CDS_ARG_UPDATE_ID "Id"

/* Represent the CDS StartingIndex argument. */
#define SERVICE_CDS_ARG_START_INDEX "StartingIndex"

/* Represent the CDS RequestedCount argument. */
#define SERVICE_CDS_ARG_REQUEST_COUNT "RequestedCount"

/* Represent the CDS ObjectID argument. */
#define SERVICE_CDS_ARG_OBJECT_ID "ObjectID"

/* Represent the CDS Filter argument. */
#define SERVICE_CDS_ARG_FILTER "Filter"

/* Represent the CDS BrowseFlag argument. */
#define SERVICE_CDS_ARG_BROWSE_FLAG "BrowseFlag"

/* Represent the CDS SortCriteria argument. */
#define SERVICE_CDS_ARG_SORT_CRIT "SortCriteria"

/* Represent the CDS Root Object ID argument. */
#define SERVICE_CDS_ROOT_OBJECT_ID "0"

/* Represent the CDS DIDL Message Metadata Browse flag argument. */
#define SERVICE_CDS_BROWSE_METADATA "BrowseMetadata"

/* Represent the CDS DIDL Message DirectChildren Browse flag argument. */
#define SERVICE_CDS_BROWSE_CHILDREN "BrowseDirectChildren"

/* Represent the CDS DIDL Message Result argument. */
#define SERVICE_CDS_DIDL_RESULT "Result"

/* Represent the CDS DIDL Message NumberReturned argument. */
#define SERVICE_CDS_DIDL_NUM_RETURNED "NumberReturned"

/* Represent the CDS DIDL Message TotalMatches argument. */
#define SERVICE_CDS_DIDL_TOTAL_MATCH "TotalMatches"

/* Represent the CDS DIDL Message UpdateID argument. */
#define SERVICE_CDS_DIDL_UPDATE_ID "UpdateID"

/* DIDL parameters */
/* Represent the CDS DIDL Message Header Namespace. */
#define DIDL_NAMESPACE \
    "xmlns=\"urn:schemas-upnp-org:metadata-1-0/DIDL-Lite/\" " \
    "xmlns:dc=\"http://purl.org/dc/elements/1.1/\" " \
    "xmlns:upnp=\"urn:schemas-upnp-org:metadata-1-0/upnp/\""

/* Represent the CDS DIDL Message Header Tag. */
#define DIDL_LITE "DIDL-Lite"

/* Represent the CDS DIDL Message Item value. */
#define DIDL_ITEM "item"

/* Represent the CDS DIDL Message Item ID value. */
#define DIDL_ITEM_ID "id"

/* Represent the CDS DIDL Message Item Parent ID value. */
#define DIDL_ITEM_PARENT_ID "parentID"

/* Represent the CDS DIDL Message Item Restricted value. */
#define DIDL_ITEM_RESTRICTED "restricted"

/* Represent the CDS DIDL Message Item UPnP Class value. */
#define DIDL_ITEM_CLASS "upnp:class"

/* Represent the CDS DIDL Message Item Title value. */
#define DIDL_ITEM_TITLE "dc:title"

/* Represent the CDS DIDL Message Item Resource value. */
#define DIDL_RES "res"

/* Represent the CDS DIDL Message Item Protocol Info value. */
#define DIDL_RES_INFO "protocolInfo"

/* Represent the CDS DIDL Message Item Resource Size value. */
#define DIDL_RES_SIZE "size"

/* Represent the CDS DIDL Message Container value. */
#define DIDL_CONTAINER "container"

/* Represent the CDS DIDL Message Container ID value. */
#define DIDL_CONTAINER_ID "id"

/* Represent the CDS DIDL Message Container Parent ID value. */
#define DIDL_CONTAINER_PARENT_ID "parentID"

/* Represent the CDS DIDL Message Container number of children value. */
#define DIDL_CONTAINER_CHILDS "childCount"

/* Represent the CDS DIDL Message Container Restricted value. */
#define DIDL_CONTAINER_RESTRICTED "restricted"

/* Represent the CDS DIDL Message Container Searchable value. */
#define DIDL_CONTAINER_SEARCH "searchable"

/* Represent the CDS DIDL Message Container UPnP Class value. */
#define DIDL_CONTAINER_CLASS "upnp:class"

/* Represent the CDS DIDL Message Container Title value. */
#define DIDL_CONTAINER_TITLE "dc:title"

/* UPnP ContentDirectory Service actions */
static bool
cds_get_search_capabilities (struct action_event_t *event)
{
  upnp_add_response (event, SERVICE_CDS_ARG_SEARCH_CAPS, "");

  return event->status;
}

static bool
cds_get_sort_capabilities (struct action_event_t *event)
{
  upnp_add_response (event, SERVICE_CDS_ARG_SORT_CAPS, "");

  return event->status;
}

static bool
cds_get_system_update_id (struct action_event_t *event)
{
  upnp_add_response (event, SERVICE_CDS_ARG_UPDATE_ID,
                     SERVICE_CDS_ROOT_OBJECT_ID);

  return event->status;
}

static void
didl_add_header (struct buffer_t *out)
{
  buffer_appendf (out, "<%s %s>", DIDL_LITE, DIDL_NAMESPACE);
}

static void
didl_add_footer (struct buffer_t *out)
{
  buffer_appendf (out, "</%s>", DIDL_LITE);
}

static void
didl_add_tag (struct buffer_t *out, char *tag, char *value)
{
  if (value)
    buffer_appendf (out, "<%s>%s</%s>", tag, value, tag);
}

static void
didl_add_param (struct buffer_t *out, char *param, char *value)
{
  if (value)
    buffer_appendf (out, " %s=\"%s\"", param, value);
}

static void
didl_add_value (struct buffer_t *out, char *param, int value)
{
  buffer_appendf (out, " %s=\"%d\"", param, value);
}

static void
didl_add_item (struct buffer_t *out, int item_id,
               int parent_id, char *restricted, char *class, char *title,
               char *protocol_info, int size, char *url)
{
  buffer_appendf (out, "<%s", DIDL_ITEM);
  didl_add_value (out, DIDL_ITEM_ID, item_id);
  didl_add_value (out, DIDL_ITEM_PARENT_ID, parent_id);
  didl_add_param (out, DIDL_ITEM_RESTRICTED, restricted);
  buffer_append (out, ">");

  didl_add_tag (out, DIDL_ITEM_CLASS, class);
  didl_add_tag (out, DIDL_ITEM_TITLE, title);

  buffer_appendf (out, "<%s", DIDL_RES);
  didl_add_param (out, DIDL_RES_INFO, protocol_info);
  if (size >= 0)
    didl_add_value (out, DIDL_RES_SIZE, size);
  buffer_append (out, ">");
  if (url)
    buffer_append (out, url);
  buffer_appendf (out, "</%s>", DIDL_RES);
  buffer_appendf (out, "</%s>", DIDL_ITEM);
}

static void
didl_add_container (struct buffer_t *out, int id, int parent_id,
                    int child_count, char *restricted, char *searchable,
                    char *title, char *class)
{
  buffer_appendf (out, "<%s", DIDL_CONTAINER);

  didl_add_value (out, DIDL_CONTAINER_ID, id);
  didl_add_value (out, DIDL_CONTAINER_PARENT_ID, parent_id);
  if (child_count >= 0)
    didl_add_value (out, DIDL_CONTAINER_CHILDS, child_count);
  didl_add_param (out, DIDL_CONTAINER_RESTRICTED, restricted);
  didl_add_param (out, DIDL_CONTAINER_SEARCH, searchable);
  buffer_append (out, ">");

  didl_add_tag (out, DIDL_CONTAINER_CLASS, class);
  didl_add_tag (out, DIDL_CONTAINER_TITLE, title);

  buffer_appendf (out, "</%s>", DIDL_CONTAINER);
}

static int
cds_browse_metadata (struct action_event_t *event, struct buffer_t *out,
                     int index, int count, struct upnp_entry_t *entry)
{
  int result_count = 0, c = 0;

  if (!entry)
    return -1;

  if (entry->child_count == -1) /* item : file */
  {
    didl_add_header (out);
    didl_add_item (out, entry->id, entry->parent
                   ? entry->parent->id : -1, "0", entry->class,
                   entry->title, entry->protocol, -1, entry->url);
    didl_add_footer (out);

    for (c = index; c < MIN (index + count, entry->child_count); c++)
      result_count++;

    upnp_add_response (event, SERVICE_CDS_DIDL_RESULT, out->buf);
    upnp_add_response (event, SERVICE_CDS_DIDL_NUM_RETURNED, "1");
    upnp_add_response (event, SERVICE_CDS_DIDL_TOTAL_MATCH, "1");
  }
  else  /* container : directory */
  {
    char tmp[32];
    didl_add_header (out);
    didl_add_container (out, entry->id, entry->parent
                        ? entry->parent->id : -1, entry->child_count,
                        "true", "true", entry->title, entry->class);
    didl_add_footer (out);

    for (c = index; c < MIN (index + count, entry->child_count); c++)
      result_count++;

    upnp_add_response (event, SERVICE_CDS_DIDL_RESULT, out->buf);
    sprintf (tmp, "%d", result_count);
    upnp_add_response (event, SERVICE_CDS_DIDL_NUM_RETURNED, tmp);
    
    sprintf (tmp, "%d", entry->child_count);
    upnp_add_response (event, SERVICE_CDS_DIDL_TOTAL_MATCH, tmp);
  }

  return result_count;
}

static int
cds_browse_directchildren (struct action_event_t *event,
                           struct buffer_t *out, int index,
                           int count, struct upnp_entry_t *entry)
{
  struct upnp_entry_t **childs;
  int s, result_count = 0;
  char tmp[32];

  if (entry->child_count == -1) /* item : file */
    return -1;

  didl_add_header (out);

  /* go to the child pointed out by index */
  childs = entry->childs;
  for (s = 0; s < index; s++)
    if (*childs)
      *childs++;

  for (; *childs; *childs++)
  {
    if (count == 0 || result_count < count)
      /* only fetch the requested count number or all entries if count = 0 */
    {
      if ((*childs)->child_count >= 0) /* container */
        didl_add_container (out, (*childs)->id, (*childs)->parent ?
                            (*childs)->parent->id : -1,
                            (*childs)->child_count, "true", NULL,
                            (*childs)->title, (*childs)->class);
      else /* item */
      {
        didl_add_item (out, (*childs)->id,
                       (*childs)->parent ? (*childs)->parent->id : -1,
                       "true", (*childs)->class, (*childs)->title,
                       (*childs)->protocol, (*childs)->size,
                       (*childs)->url);
      }
      result_count++;
    }
  }

  didl_add_footer (out);

  upnp_add_response (event, SERVICE_CDS_DIDL_RESULT, out->buf);
  sprintf (tmp, "%d", result_count);
  upnp_add_response (event, SERVICE_CDS_DIDL_NUM_RETURNED, tmp);
  sprintf (tmp, "%d", entry->child_count);
  upnp_add_response (event, SERVICE_CDS_DIDL_TOTAL_MATCH, tmp);

  return result_count;
}

static bool
cds_browse (struct action_event_t *event)
{
  extern struct ushare_t *ut;
  struct upnp_entry_t *entry = NULL;
  int result_count = 0, index, count, id, sort_criteria;
  char *flag = NULL;
  struct buffer_t *out = NULL;
  bool metadata;

  if (!event)
    return false;

  /* Check for status */
  if (!event->status)
    return false;

  /* check if metadatas have been well inited */
  if (!ut->init)
    return false;
  
  /* Retrieve Browse arguments */
  index = upnp_get_ui4 (event->request, SERVICE_CDS_ARG_START_INDEX);
  count = upnp_get_ui4 (event->request, SERVICE_CDS_ARG_REQUEST_COUNT);
  id = upnp_get_ui4 (event->request, SERVICE_CDS_ARG_OBJECT_ID);
  flag = upnp_get_string (event->request, SERVICE_CDS_ARG_BROWSE_FLAG);
  sort_criteria = upnp_get_ui4 (event->request, SERVICE_CDS_ARG_SORT_CRIT);

  if (!flag)
    return false;

  /* Check arguments validity */
  if (!strcmp (flag, SERVICE_CDS_BROWSE_METADATA))
    {
      if (index)
      {
        free (flag);
        return false;
      }
      metadata = true;
    }
  else if (!strcmp (flag, SERVICE_CDS_BROWSE_CHILDREN))
    metadata = false;
  else
  {
    free (flag);
    return false;
  }
  free (flag);

  entry = upnp_get_entry (ut->root_entry, id);
  if (!entry)
    return false;

  out = buffer_new ();
  if (!out)
    return false;

  if (metadata)
    result_count =
      cds_browse_metadata (event, out, index, count, entry);
  else
    result_count =
      cds_browse_directchildren (event, out, index, count, entry);

  if (result_count < 0)
    {
      buffer_free (out);
      return false;
    }

  buffer_free (out);
  upnp_add_response (event, SERVICE_CDS_DIDL_UPDATE_ID,
                     SERVICE_CDS_ROOT_OBJECT_ID);

  return event->status;
}

/* List of UPnP ContentDirectory Service actions */
struct service_action_t cds_service_actions[] = {
  { SERVICE_CDS_ACTION_SEARCH_CAPS, cds_get_search_capabilities },
  { SERVICE_CDS_ACTION_SORT_CAPS, cds_get_sort_capabilities },
  { SERVICE_CDS_ACTION_UPDATE_ID, cds_get_system_update_id },
  { SERVICE_CDS_ACTION_BROWSE, cds_browse },
  { NULL, NULL }
};
