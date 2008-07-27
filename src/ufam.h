/*
 * ufam.h : GeeXboX uShare file alterative monitor headers
 * Originally developped for the GeeXboX project.
 * Copyright (C) 2005-2007 Alexis Saettler <asbin@asbin.org>
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

#ifndef _UFAM_H_
#define _UFAM_H_

#ifdef HAVE_FAM

#include <fam.h>
#include <pthread.h>
#include <stdbool.h>

#include "ushare.h"
#include "metadata.h"

typedef struct ufam_s {
  FAMConnection fc;

  pthread_t thread;
  pthread_mutex_t startstop_lock;
  pthread_cond_t stop_cond;
  pthread_mutex_t add_monitor_lock;
  bool stop;
} ufam_t;

typdef struct ufam_entry_s {
  ufam_t *ufam;
  struct upnp_entry_t *entry;
  FAMRequest fr;
} ufam_entry_t;

ufam_t *ufam_init (void);
void ufam_free (ufam_t *ufam);

void ufam_start (ushare_t *ut);
void ufam_stop (ufam_t *ufam);

ufam_entry_t *ufam_add_monitor (ufam_t *ufam, struct upnp_entry_t *entry);
void ufam_remove_monitor (ufam_entry_t *ufam_entry);

#endif /* HAVE_FAM */

#endif /* _UFAM_H_ */
