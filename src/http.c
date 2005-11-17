/*
 * http.c : GeeXboX uShare Web Server handler.
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <upnp/upnp.h>
#include <upnp/upnptools.h>

#include "services.h"
#include "cds.h"
#include "cms.h"
#include "metadata.h"
#include "http.h"
#include "minmax.h"

struct web_file_t {
  char *fullpath;
  size_t pos;
  enum {
    FILE_LOCAL,
    FILE_MEMORY,
  } type;
  union {
    struct {
      int fd;
      struct upnp_entry_t *entry;
    } local;
    struct {
      char *contents;
      size_t len;
    } memory;
  } detail;
};

static int
http_get_info (const char *filename, struct File_Info *info)
{
  extern struct upnp_entry_t *root_entry;
  struct upnp_entry_t *entry = NULL;
  struct stat st;
  int upnp_id = 0;

  if (!filename || !info)
    return -1;

  print_info ("http_get_info, filename : %s\n", filename);

  if (!strcmp (filename, CDS_LOCATION))
  {
    info->file_length = CDS_DESCRIPTION_LEN;
    info->last_modified = 0;
    info->is_directory = 0;
    info->is_readable = 1;
    info->content_type = ixmlCloneDOMString (SERVICE_CONTENT_TYPE);
    return 0;
  }

  if (!strcmp (filename, CMS_LOCATION))
  {
    info->file_length = CMS_DESCRIPTION_LEN;
    info->last_modified = 0;
    info->is_directory = 0;
    info->is_readable = 1;
    info->content_type = ixmlCloneDOMString (SERVICE_CONTENT_TYPE);
    return 0;
  }

  upnp_id = atoi (strrchr (filename, '/') + 1);
  entry = upnp_get_entry (root_entry, upnp_id);
  if (!entry)
    return -1;

  if (!entry->fullpath)
    return -1;

  if (stat (entry->fullpath, &st) < 0)
    return -1;

  if (access (entry->fullpath, R_OK) < 0)
  {
    if (errno != EACCES)
      return -1;
    info->is_readable = 0;
  }
  else
    info->is_readable = 1;

  /* file exist and can be read */
  info->file_length = st.st_size;
  info->last_modified = st.st_mtime;
  info->is_directory = S_ISDIR (st.st_mode);
  info->content_type = strdup ("");

  return 0;
}

static UpnpWebFileHandle
http_open (const char *filename, enum UpnpOpenFileMode mode)
{
  extern struct upnp_entry_t *root_entry;
  struct upnp_entry_t *entry = NULL;
  struct web_file_t *file;
  int fd, upnp_id = 0;

  if (!filename)
    return NULL;

  print_info ("http_open, filename : %s\n", filename);

  if (mode != UPNP_READ)
    return NULL;

  if (!strcmp (filename, CDS_LOCATION))
  {
    file = malloc (sizeof (struct web_file_t));
    file->fullpath = strdup (CDS_LOCATION);
    file->pos = 0;
    file->type = FILE_MEMORY;
    file->detail.memory.contents = strdup (CDS_DESCRIPTION);
    file->detail.memory.len = CDS_DESCRIPTION_LEN;
    return ((UpnpWebFileHandle) file);
  }

  if (!strcmp (filename, CMS_LOCATION))
  {
    file = malloc (sizeof (struct web_file_t));
    file->fullpath = strdup (CMS_LOCATION);
    file->pos = 0;
    file->type = FILE_MEMORY;
    file->detail.memory.contents = strdup (CMS_DESCRIPTION);
    file->detail.memory.len = CMS_DESCRIPTION_LEN;
    return ((UpnpWebFileHandle) file);
  }

  upnp_id = atoi (strrchr (filename, '/') + 1);
  entry = upnp_get_entry (root_entry, upnp_id);
  if (!entry)
    return NULL;

  if (!entry->fullpath)
    return NULL;

  print_info ("Fullpath : %s\n", entry->fullpath);

  fd = open (entry->fullpath, O_RDONLY | O_NONBLOCK | O_SYNC | O_NDELAY);
  if (fd < 0)
    return NULL;

  file = malloc (sizeof (struct web_file_t));
  file->fullpath = strdup (entry->fullpath);
  file->pos = 0;
  file->type = FILE_LOCAL;
  file->detail.local.entry = entry;
  file->detail.local.fd = fd;

  return ((UpnpWebFileHandle) file);
}

static int
http_read (UpnpWebFileHandle fh, char *buf, size_t buflen)
{
  struct web_file_t *file = (struct web_file_t *) fh;
  ssize_t len = -1;

  print_info ("http_read\n");

  if (!file)
    return -1;

  switch (file->type)
  {
  case FILE_LOCAL:
    print_info ("Read local file.\n");
    len = read (file->detail.local.fd, buf, buflen);
    break;
  case FILE_MEMORY:
    print_info ("Read file from memory.\n");
    len = MIN (buflen, file->detail.memory.len - file->pos);
    memcpy (buf, file->detail.memory.contents + file->pos, len);
    break;
  default:
    print_info ("Unknown file type.\n");
    break;
  }

  if (len >= 0)
    file->pos += len;

  print_info ("Read %d bytes.\n", len);

  return len;
}

static int
http_write (UpnpWebFileHandle fh __attribute__((unused)),
            char *buf __attribute__((unused)),
            size_t buflen __attribute__((unused)))
{
  print_info ("http write\n");

  return 0;
}

static int
http_seek (UpnpWebFileHandle fh, long offset, int origin)
{
  struct web_file_t *file = (struct web_file_t *) fh;
  long newpos = -1;

  print_info ("http_seek\n");

  if (!file)
    return -1;

  switch (origin)
  {
  case SEEK_SET:
    print_info ("Attempting to seek to %ld (was at %d) in %s\n",
                offset, file->pos, file->fullpath);
    newpos = offset;
    break;
  case SEEK_CUR:
    print_info ("Attempting to seek by %ld from %d in %s\n",
                offset, file->pos, file->fullpath);
    newpos = file->pos + offset;
    break;
  case SEEK_END:
    print_info ("Attempting to seek by %ld from end (was at %d) in %s\n",
                offset, file->pos, file->fullpath);

    if (file->type == FILE_LOCAL)
    {
      struct stat sb;
      if (stat (file->fullpath, &sb) < 0)
      {
        print_info ("%s: cannot stat: %s\n",
                    file->fullpath, strerror (errno));
        return -1;
      }
      newpos = sb.st_size + offset;
    }
    else if (file->type == FILE_MEMORY)
      newpos = file->detail.memory.len + offset;
    break;
  }

  switch (file->type)
  {
  case FILE_LOCAL:
    /* Just make sure we cannot seek before start of file. */
    if (newpos < 0)
    {
      print_info ("%s: cannot seek: %s\n", file->fullpath, strerror (EINVAL));
      return -1;
    }

    /* Don't seek with origin as specified above, as file may have
       changed in size since our last stat. */
    if (lseek (file->detail.local.fd, newpos, SEEK_SET) == -1)
    {
      print_info ("%s: cannot seek: %s\n", file->fullpath, strerror (errno));
      return -1;
    }
    break;
  case FILE_MEMORY:
    if (newpos < 0 || newpos > (long) file->detail.memory.len)
    {
      print_info ("%s: cannot seek: %s\n", file->fullpath, strerror (EINVAL));
      return -1;
    }
    break;
  }

  file->pos = newpos;

  return 0;
}

static int
http_close (UpnpWebFileHandle fh)
{
  struct web_file_t *file = (struct web_file_t *) fh;

  print_info ("http_close\n");

  if (!file)
    return -1;

  switch (file->type)
  {
  case FILE_LOCAL:
    close (file->detail.local.fd);
    break;
  case FILE_MEMORY:
    /* no close operation */
    if (file->detail.memory.contents)
      free (file->detail.memory.contents);
    break;
  default:
    print_info ("Unknown file type.\n");
    break;
  }

  if (file->fullpath)
    free (file->fullpath);
  free (file);

  return 0;
}

struct UpnpVirtualDirCallbacks virtual_dir_callbacks =
  {
    http_get_info,
    http_open,
    http_read,
    http_write,
    http_seek,
    http_close
  };
