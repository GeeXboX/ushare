/*
 * http.c : GeeXboX uShare Web Server handler.
 * Originally developped for the GeeXboX project.
 * Parts of the code are originated from GMediaServer from Oskar Liljeblad.
 * Copyright (C) 2005-2007 Benjamin Zores <ben@geexbox.org>
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

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "metadata.h"
#include "minmax.h"
#include "trace.h"
#include "presentation.h"
#include "osdep.h"
#include "mime.h"

#define PROTOCOL_TYPE_PRE_SZ  11   /* for the str length of "http-get:*:" */
#define PROTOCOL_TYPE_SUFF_SZ 2    /* for the str length of ":*" */

typedef struct web_file_s {
  char *fullpath;
  off_t pos;
  char *contents;
  off_t len;
} web_file_t;

static inline void
set_info_file (dlna_http_file_info_t *info,
               const size_t length, const char *content_type)
{
  info->file_length   = length;
  info->content_type  = strdup (content_type);
}

static int
http_get_info (const char *filename, dlna_http_file_info_t *info)
{
  extern ushare_t *ut;
  
  if (!filename || !info)
    return 1;

  log_verbose ("http_get_info, filename : %s\n", filename);

  if (ut->use_presentation && !strcmp (filename, USHARE_PRESENTATION_PAGE))
  {
    if (build_presentation_page (ut) < 0)
      return 1;

    set_info_file (info, ut->presentation->len,
                   PRESENTATION_PAGE_CONTENT_TYPE);
    return 0;
  }

  if (ut->use_presentation &&
      !strncmp (filename, USHARE_CGI, strlen (USHARE_CGI)))
  {
    if (process_cgi (ut, (char *) (filename + strlen (USHARE_CGI) + 1)) < 0)
      return 1;

    set_info_file (info, ut->presentation->len,
                   PRESENTATION_PAGE_CONTENT_TYPE);
    return 0;
  }

  return 1;
}

static dlna_http_file_handler_t *
get_file_memory (const char *fullpath, const char *description,
                 const size_t length)
{
  dlna_http_file_handler_t *dhdl;
  web_file_t *file;

  file = malloc (sizeof (web_file_t));
  file->fullpath = strdup (fullpath);
  file->pos = 0;
  file->contents = strdup (description);
  file->len = length;

  dhdl                       = malloc (sizeof (dlna_http_file_handler_t));
  dhdl->external             = 1;
  dhdl->priv                 = file;
  
  return ((dlna_http_file_handler_t *) dhdl);
}

static dlna_http_file_handler_t *
http_open (const char *filename)
{
  extern ushare_t *ut;

  if (!filename)
    return NULL;

  log_verbose ("http_open, filename : %s\n", filename);

  if (ut->use_presentation && ( !strcmp (filename, USHARE_PRESENTATION_PAGE)
      || !strncmp (filename, USHARE_CGI, strlen (USHARE_CGI))))
    return get_file_memory (USHARE_PRESENTATION_PAGE, ut->presentation->buf,
                            ut->presentation->len);

  return NULL;
}

static int
http_read (void *hdl, char *buf, size_t buflen)
{
  web_file_t *file = (web_file_t *) hdl;
  ssize_t len = -1;

  log_verbose ("http_read\n");

  if (!file)
    return -1;

  len = (size_t) MIN (buflen, file->len - file->pos);
  memcpy (buf, file->contents + file->pos, (size_t) len);

  if (len >= 0)
    file->pos += len;

  log_verbose ("Read %zd bytes.\n", len);

  return len;
}

static int
http_seek (void *hdl, off_t offset, int origin)
{
  web_file_t *file = (web_file_t *) hdl;
  off_t newpos = -1;

  log_verbose ("http_seek\n");

  if (!file)
    return -1;

  switch (origin)
  {
  case SEEK_SET:
    log_verbose ("Attempting to seek to %lld (was at %lld) in %s\n",
                offset, file->pos, file->fullpath);
    newpos = offset;
    break;
  case SEEK_CUR:
    log_verbose ("Attempting to seek by %lld from %lld in %s\n",
                offset, file->pos, file->fullpath);
    newpos = file->pos + offset;
    break;
  case SEEK_END:
    log_verbose ("Attempting to seek by %lld from end (was at %lld) in %s\n",
                offset, file->pos, file->fullpath);

    newpos = file->len + offset;
    break;
  }

  if (newpos < 0 || newpos > file->len)
  {
    log_verbose ("%s: cannot seek: %s\n", file->fullpath, strerror (EINVAL));
    return -1;
  }

  file->pos = newpos;

  return 0;
}

static int
http_close (void *hdl)
{
  web_file_t *file = (web_file_t *) hdl;

  log_verbose ("http_close\n");

  if (!file)
    return -1;

  if (file->contents)
    free (file->contents);

  if (file->fullpath)
    free (file->fullpath);
  free (file);

  return 0;
}

dlna_http_callback_t ushare_http_callbacks = {
  http_get_info,
  http_open,
  http_read,
  NULL,
  http_seek,
  http_close
};
