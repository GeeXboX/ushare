/*
 * metadata.h : GeeXboX uShare CDS Metadata DB header.
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

#ifndef _METADATA_H_
#define _METADATA_H_

#include <upnp/upnp.h>
#include <upnp/upnptools.h>
#include <stdbool.h>

#include "ushare.h"
#include "http.h"
#include "content.h"

struct upnp_entry_t {
  int id;
  char *fullpath;
  struct upnp_entry_t *parent;
  int child_count;
  struct upnp_entry_t **childs;
  char *class;
  char *protocol;
  char *title;
  char *url;
  int size;
  int fd;
};

void free_metadata_list (struct ushare_t *ut);
void build_metadata_list (struct ushare_t *ut);
struct upnp_entry_t * upnp_get_entry (struct upnp_entry_t *entry, int id);
void upnp_entry_free (struct upnp_entry_t *entry);

#endif /* _METADATA_H_ */
