/*
 * metadata.c : GeeXboX uShare CDS Metadata DB.
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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <upnp/upnp.h>
#include <upnp/upnptools.h>

#include "mime.h"
#include "metadata.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#error "Missing config.h file : run configure again"
#endif

#ifdef ENABLE_NLS
# define _(string) gettext (string)
# include "gettext.h"
#else
# define _(string) string
#endif

struct upnp_entry_t *root_entry = NULL;
int nr_entries = 0;

static char *
getExtension (char *filename)
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
getUpnpClass (char *filename)
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
getUpnpProtocol (char *filename)
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

static int
is_valid_extension (char *filename)
{
  extern struct mime_type_t MIME_Type_List[];
  struct mime_type_t *list;
  char *extension = NULL;

  if (!filename)
    return 0;

  extension = getExtension (filename);
  if (!extension)
    return 0;

  list = MIME_Type_List;
  while (list->extension)
  {
    if (!strcasecmp ((*list).extension, extension))
    {
      free (extension);
      return 1;
    }
    *list++;
  }

  free (extension);
  
  return 0;
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
upnp_entry_new (char *name, char *fullpath,
                struct upnp_entry_t *parent, int size, int dir)
{
  struct upnp_entry_t *entry = NULL;
  char *x, *title = NULL;

  if (!name)
    return NULL;
  
  entry = (struct upnp_entry_t *) malloc (sizeof (struct upnp_entry_t));

  entry->id = nr_entries++;
  entry->fullpath = fullpath ? strdup (fullpath) : NULL;
  entry->parent = parent;
  entry->child_count =  dir ? 0 : -1;

  entry->childs = (struct upnp_entry_t **)
    malloc (sizeof (struct upnp_entry_t *));
  *(entry->childs) = NULL;

  if (!dir) /* item */
    {
      entry->class = getUpnpClass (name);
      entry->protocol = getUpnpProtocol (name);
      entry->url = (char *) malloc (1024 * sizeof (char));
      sprintf (entry->url, "http://%s:%d%s/%d",
               UpnpGetServerIpAddress (), UpnpGetServerPort(),
               VIRTUAL_DIR, entry->id);  
    }
  else /* container */
    {
      entry->class = strdup ("object.container");
      entry->protocol = NULL;
      entry->url = NULL;
    }

  title = strdup (name);
  x = strrchr (title, '.');
  if (x)  /* avoid displaying file extension */
    title[strlen (title) - strlen (x)] = '\0';
  entry->title = strdup (title);;
  free (title);

  entry->size = size;
  entry->fd = -1;

#ifdef DEBUG
  if (entry->id && entry->url)
    printf ("Entry->URL (%d): %s\n", entry->id, entry->url);
#endif /* DEBUG */

  return entry;
}

static void
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
metadata_add_file (struct upnp_entry_t *entry, char *file, char *name)
{
  struct stat st;

  if (!entry || !file || !name)
    return;
  
  if (stat (file, &st) < 0)
    return;
  
  if (is_valid_extension (file))
  {
    struct upnp_entry_t *child = NULL;

    child = upnp_entry_new (name, file, entry, st.st_size, 0);
    if (child)
      upnp_entry_add_child (entry, child);
  }
}

static void
metadata_add_container (struct upnp_entry_t *entry, char *container)
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

#ifdef DEBUG      
      printf ("%s\n", fullpath);
#endif /* DEBUG */
      
      if (stat (fullpath, &st) < 0)
      {
        free (namelist[i]);
        free (fullpath);
        continue;
      }

      if (S_ISDIR (st.st_mode))
      {
        struct upnp_entry_t *child = NULL;

        child = upnp_entry_new (namelist[i]->d_name,
                                fullpath, entry, 0, 1);
        metadata_add_container (child, fullpath);
        upnp_entry_add_child (entry, child);
      }
      else
        metadata_add_file (entry, fullpath, namelist[i]->d_name);

      free (namelist[i]);
      free (fullpath);
    }
    free (namelist);
  }
}

void
free_metadata_list (void)
{
  if (root_entry)
    upnp_entry_free (root_entry);
  root_entry = NULL;
  nr_entries = 0;
}

void
build_metadata_list (char *contentdir)
{
  printf (_("Building Metadata List ...\n"));
  printf (_("Looking for files in content directory : %s\n"), contentdir);

  /* build root entry */
  if (!root_entry)
    root_entry = upnp_entry_new ("root", NULL, NULL, -1, 1);

  /* add files from content directory */
  metadata_add_container (root_entry, contentdir);

  printf (_("Found %d files and subdirectories.\n"), nr_entries);
}
