/*
 * http.c : GeeXboX uShare Web Server handler.
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
#include "trace.h"
#include "presentation.h"

struct web_file_t {
  char *fullpath;
  size_t pos;
  enum {
    FILE_LOCAL,
    FILE_MEMORY
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
  extern struct ushare_t *ut;
  struct upnp_entry_t *entry = NULL;
  struct stat st;
  int upnp_id = 0;

  if (!filename || !info)
    return -1;

  log_verbose ("http_get_info, filename : %s\n", filename);

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

  if (ut->use_presentation && !strcmp (filename, USHARE_PRESENTATION_PAGE))
  {
    if (build_presentation_page (ut) < 0)
      return -1;
    
    info->file_length = ut->presentation->len;
    info->last_modified = 0;
    info->is_directory = 0;
    info->is_readable = 1;
    info->content_type = ixmlCloneDOMString (PRESENTATION_PAGE_CONTENT_TYPE);
    return 0;
  }

  if (ut->use_presentation && !strncmp (filename, USHARE_CGI, strlen (USHARE_CGI)))
  {
    if (process_cgi (ut, (char *) (filename + strlen (USHARE_CGI) + 1)) < 0)
      return -1;
    
    info->file_length = ut->presentation->len;
    info->last_modified = 0;
    info->is_directory = 0;
    info->is_readable = 1;
    info->content_type = ixmlCloneDOMString (PRESENTATION_PAGE_CONTENT_TYPE);
    return 0;
  }

  upnp_id = atoi (strrchr (filename, '/') + 1);
  entry = upnp_get_entry (ut->root_entry, upnp_id);
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
  extern struct ushare_t *ut;
  struct upnp_entry_t *entry = NULL;
  struct web_file_t *file;
  int fd, upnp_id = 0;

  if (!filename)
    return NULL;

  log_verbose ("http_open, filename : %s\n", filename);

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

  if (ut->use_presentation && ( !strcmp (filename, USHARE_PRESENTATION_PAGE)
      || !strncmp (filename, USHARE_CGI, strlen (USHARE_CGI))))
  {
    file = malloc (sizeof (struct web_file_t));
    file->fullpath = strdup (USHARE_PRESENTATION_PAGE);
    file->pos = 0;
    file->type = FILE_MEMORY;
    file->detail.memory.contents = strdup (ut->presentation->buf);
    file->detail.memory.len = ut->presentation->len;
    return ((UpnpWebFileHandle) file);
  }

  upnp_id = atoi (strrchr (filename, '/') + 1);
  entry = upnp_get_entry (ut->root_entry, upnp_id);
  if (!entry)
    return NULL;

  if (!entry->fullpath)
    return NULL;

  log_verbose ("Fullpath : %s\n", entry->fullpath);

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

  log_verbose ("http_read\n");

  if (!file)
    return -1;

  switch (file->type)
  {
  case FILE_LOCAL:
    log_verbose ("Read local file.\n");
    len = read (file->detail.local.fd, buf, buflen);
    break;
  case FILE_MEMORY:
    log_verbose ("Read file from memory.\n");
    len = (size_t) MIN (buflen, file->detail.memory.len - file->pos);
    memcpy (buf, file->detail.memory.contents + file->pos, (size_t) len);
    break;
  default:
    log_verbose ("Unknown file type.\n");
    break;
  }

  if (len >= 0)
    file->pos += len;

  log_verbose ("Read %d bytes.\n", len);

  return len;
}

static int
http_write (UpnpWebFileHandle fh __attribute__((unused)),
            char *buf __attribute__((unused)),
            size_t buflen __attribute__((unused)))
{
  log_verbose ("http write\n");

  return 0;
}

static int
http_seek (UpnpWebFileHandle fh, long offset, int origin)
{
  struct web_file_t *file = (struct web_file_t *) fh;
  long newpos = -1;

  log_verbose ("http_seek\n");

  if (!file)
    return -1;

  switch (origin)
  {
  case SEEK_SET:
    log_verbose ("Attempting to seek to %ld (was at %d) in %s\n",
                offset, file->pos, file->fullpath);
    newpos = offset;
    break;
  case SEEK_CUR:
    log_verbose ("Attempting to seek by %ld from %d in %s\n",
                offset, file->pos, file->fullpath);
    newpos = file->pos + offset;
    break;
  case SEEK_END:
    log_verbose ("Attempting to seek by %ld from end (was at %d) in %s\n",
                offset, file->pos, file->fullpath);

    if (file->type == FILE_LOCAL)
    {
      struct stat sb;
      if (stat (file->fullpath, &sb) < 0)
      {
        log_verbose ("%s: cannot stat: %s\n",
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
      log_verbose ("%s: cannot seek: %s\n", file->fullpath, strerror (EINVAL));
      return -1;
    }

    /* Don't seek with origin as specified above, as file may have
       changed in size since our last stat. */
    if (lseek (file->detail.local.fd, newpos, SEEK_SET) == -1)
    {
      log_verbose ("%s: cannot seek: %s\n", file->fullpath, strerror (errno));
      return -1;
    }
    break;
  case FILE_MEMORY:
    if (newpos < 0 || newpos > (long) file->detail.memory.len)
    {
      log_verbose ("%s: cannot seek: %s\n", file->fullpath, strerror (EINVAL));
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

  log_verbose ("http_close\n");

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
    log_verbose ("Unknown file type.\n");
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
