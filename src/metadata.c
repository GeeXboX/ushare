/*
 * metadata.c : GeeXboX uShare CDS Metadata DB.
 * Originally developped for the GeeXboX project.
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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>

#include <upnp/upnp.h>
#include <upnp/upnptools.h>

#include "mime.h"
#include "metadata.h"
#include "util_iconv.h"
#include "content.h"
#include "gettext.h"
#include "trace.h"

#define TITLE_UNKNOWN "unknown"

static char *
getExtension (const char *filename)
{
  char *delimiter =".";
  char *str, *token, *extension;

  if (!filename)
    return NULL;

  str = strdup (filename);
  token = strtok (str, delimiter);
  extension = strdup (token);

  while (token)
  {
    token = strtok (NULL, delimiter);
    if (token)
    {
      if (extension)
        free (extension);
      extension = strdup (token);
    }
  }

  free (str);

  return extension;
}

static char *
getUpnpClass (const char *filename)
{
  extern struct mime_type_t MIME_Type_List[];
  struct mime_type_t *list;
  char *extension = NULL;

  if (!filename)
    return NULL;

  extension = getExtension (filename);
  if (!extension)
    return NULL;

  list = MIME_Type_List;

  while (list->extension)
  {
    if (!strcasecmp ((*list).extension, extension))
    {
      free (extension);
      return strdup ((*list).class);
    }
    *list++;
  }

  free (extension);

  return NULL;
}

static char *
getUpnpProtocol (const char *filename)
{
  extern struct mime_type_t MIME_Type_List[];
  struct mime_type_t *list;
  char *extension;

  if (!filename)
    return NULL;

  extension = getExtension (filename);
  if (!extension)
    return NULL;

  list = MIME_Type_List;

  while (list->extension)
  {
    if (!strcasecmp ((*list).extension, extension))
    {
      free (extension);
      return strdup ((*list).protocol);
    }
    *list++;
  }

  free (extension);

  return NULL;
}

static bool
is_valid_extension (const char *filename)
{
  extern struct mime_type_t MIME_Type_List[];
  struct mime_type_t *list;
  char *extension = NULL;

  if (!filename)
    return false;

  extension = getExtension (filename);
  if (!extension)
    return false;

  list = MIME_Type_List;
  while (list->extension)
  {
    if (!strcasecmp ((*list).extension, extension))
    {
      free (extension);
      return true;
    }
    *list++;
  }

  free (extension);

  return false;
}

static int
get_list_length (void *list)
{
  void **l = list;
  int n = 0;

  while (*l++)
    n++;

  return n;
}

static struct upnp_entry_t *
upnp_entry_new (struct ushare_t *ut, const char *name, const char *fullpath,
                struct upnp_entry_t *parent, int size, int dir)
{
  struct upnp_entry_t *entry = NULL;
  char *title = NULL;

  if (!name)
    return NULL;

  entry = (struct upnp_entry_t *) malloc (sizeof (struct upnp_entry_t));

  entry->id = ut->nr_entries++;
  entry->fullpath = fullpath ? strdup (fullpath) : NULL;
  entry->parent = parent;
  entry->child_count =  dir ? 0 : -1;
  entry->title = NULL;

  entry->childs = (struct upnp_entry_t **)
    malloc (sizeof (struct upnp_entry_t *));
  *(entry->childs) = NULL;

  if (!dir) /* item */
    {
      entry->class = getUpnpClass (name);
      entry->protocol = getUpnpProtocol (name);
      entry->url = (char *) malloc (1024 * sizeof (char));
      sprintf (entry->url, "http://%s:%d%s/%d",
               UpnpGetServerIpAddress (), ut->port,
               VIRTUAL_DIR, entry->id);
    }
  else /* container */
    {
      entry->class = strdup ("object.container");
      entry->protocol = NULL;
      entry->url = NULL;
    }

  title = iconv_convert (name);
  if (title)
  {
    if (!dir)
    {
      char *x = NULL;
      x = strrchr (title, '.');
      if (x)  /* avoid displaying file extension */
        *x = '\0';
    }
    entry->title = title;

    if (!strcmp (title, "")) /* DIDL dc:title can't be empty */
    {
      free (entry->title);
      entry->title = strdup (TITLE_UNKNOWN);
    }
  }
  else
  {
    upnp_entry_free (entry);
    return NULL;
  }

  entry->size = size;
  entry->fd = -1;

  if (entry->id && entry->url)
    log_verbose ("Entry->URL (%d): %s\n", entry->id, entry->url);

  return entry;
}

void
upnp_entry_free (struct upnp_entry_t *entry)
{
  struct upnp_entry_t **childs;

  if (!entry)
    return;

  if (entry->fullpath)
    free (entry->fullpath);
  if (entry->class)
    free (entry->class);
  if (entry->protocol)
    free (entry->protocol);
  if (entry->title)
    free (entry->title);
  if (entry->url)
    free (entry->url);

  for (childs = entry->childs; *childs; *childs++)
    upnp_entry_free (*childs);
  free (entry->childs);

  free (entry);
}

static void
upnp_entry_add_child (struct upnp_entry_t *entry, struct upnp_entry_t *child)
{
  struct upnp_entry_t **childs;
  int n;

  if (!entry || !child)
    return;

  for (childs = entry->childs; *childs; *childs++)
    if (*childs == child)
      return;

  n = get_list_length ((void *) entry->childs) + 1;
  entry->childs = (struct upnp_entry_t **)
    realloc (entry->childs, (n + 1) * sizeof (*(entry->childs)));
  entry->childs[n] = NULL;
  entry->childs[n - 1] = child;
  entry->child_count++;
}

struct upnp_entry_t *
upnp_get_entry (struct upnp_entry_t *entry, int id)
{
  struct upnp_entry_t *result = NULL;
  struct upnp_entry_t **entries;

  if (!entry)
    return NULL;

  if (entry->id == id)
    return entry;

  for (entries = entry->childs; *entries; *entries++)
  {
    result = upnp_get_entry (*entries, id);
    if (result)
      return result;
  }

  return NULL;
}

static void
metadata_add_file (struct ushare_t *ut, struct upnp_entry_t *entry,
                   const char *file, const char *name)
{
  struct stat st;

  if (!entry || !file || !name)
    return;

  if (stat (file, &st) < 0)
    return;

  if (is_valid_extension (file))
  {
    struct upnp_entry_t *child = NULL;

    child = upnp_entry_new (ut, name, file, entry, st.st_size, 0);
    if (child)
      upnp_entry_add_child (entry, child);
  }
}

static void
metadata_add_container (struct ushare_t *ut,
                        struct upnp_entry_t *entry, const char *container)
{
  struct dirent **namelist;
  int n;

  if (!entry || !container)
    return;

  n = scandir (container, &namelist, 0, alphasort);
  if (n < 0)
    perror ("scandir");
  else
  {
    int i;

    for (i = 0; i < n; i++)
    {
      struct stat st;
      char *fullpath = NULL;

      if (namelist[i]->d_name[0] == '.')
      {
        free (namelist[i]);
        continue;
      }

      fullpath = (char *)
        malloc (strlen (container) + strlen (namelist[i]->d_name) + 2);
      sprintf (fullpath, "%s/%s", container, namelist[i]->d_name);

      log_verbose ("%s\n", fullpath);

      if (stat (fullpath, &st) < 0)
      {
        free (namelist[i]);
        free (fullpath);
        continue;
      }

      if (S_ISDIR (st.st_mode))
      {
        struct upnp_entry_t *child = NULL;

        child = upnp_entry_new (ut, namelist[i]->d_name,
                                fullpath, entry, 0, 1);
        metadata_add_container (ut, child, fullpath);
        upnp_entry_add_child (entry, child);
      }
      else
        metadata_add_file (ut, entry, fullpath, namelist[i]->d_name);

      free (namelist[i]);
      free (fullpath);
    }
    free (namelist);
  }
}

void
free_metadata_list (struct ushare_t *ut)
{
  ut->init = 0;
  if (ut->root_entry)
    upnp_entry_free (ut->root_entry);
  ut->root_entry = NULL;
  ut->nr_entries = 0;
}

void
build_metadata_list (struct ushare_t *ut)
{
  int i;
  log_info (_("Building Metadata List ...\n"));

  /* build root entry */
  if (!ut->root_entry)
    ut->root_entry = upnp_entry_new (ut, "root", NULL, NULL, -1, 1);

  /* add files from content directory */
  for (i=0 ; i < ut->contentlist->count ; i++)
  {
    struct upnp_entry_t *entry = NULL;
    char *title = NULL;
    int size = 0;

    log_info (_("Looking for files in content directory : %s\n"),
              ut->contentlist->content[i]);

    size = strlen (ut->contentlist->content[i]);
    if (ut->contentlist->content[i][size - 1] == '/')
      ut->contentlist->content[i][size - 1] = '\0';
    title = strrchr (ut->contentlist->content[i], '/');
    if (title)
      title++;
    else
    {
      /* directly use content directory name if no '/' before basename */
      title = ut->contentlist->content[i];
    }

    entry = upnp_entry_new (ut, title, ut->contentlist->content[i],
                            ut->root_entry, -1, 1);

    if (!entry)
      continue;
    upnp_entry_add_child (ut->root_entry, entry);
    metadata_add_container (ut, entry, ut->contentlist->content[i]);
  }

  log_info (_("Found %d files and subdirectories.\n"), ut->nr_entries);
  ut->init = 1;
}
