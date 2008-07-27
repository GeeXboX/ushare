/*
 * ushare.h : GeeXboX uShare UPnP Media Server header.
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

#ifndef _USHARE_H_
#define _USHARE_H_

#include <stdbool.h>
#include <pthread.h>

#include <dlna.h>

#include "content.h"
#include "buffer.h"

#define VIRTUAL_DIR "/web"
#define DEFAULT_UUID "898f9738-d930-4db4-a3cf"

#define UPNP_MAX_CONTENT_LENGTH 4096

typedef struct ushare_s {
  char *name;
  char *interface;
  char *model_name;
  content_list_t *contentlist;
  int init;
  char *udn;
  unsigned short port;
  unsigned short telnet_port;
  buffer_t *presentation;
  bool use_presentation;
  bool use_telnet;
  dlna_t *dlna;
  dlna_org_flags_t dlna_flags;
  dlna_capability_mode_t caps;
  bool verbose;
  bool daemon;
  bool override_iconv_err;
  char *cfg_file;
  pthread_mutex_t termination_mutex;
  pthread_cond_t termination_cond;
#ifdef HAVE_FAM
  ufam_t *ufam;
#endif /* HAVE_FAM */
} ushare_t;

inline void display_headers (void);

#endif /* _USHARE_H_ */
