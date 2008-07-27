/*
 * ufam.c : GeeXboX uShare file alterative monitor
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


#ifdef HAVE_FAM

#include <stdio.h>
#include <stdlib.h>
#include <fam.h>
#include <sys/select.h>
#include <pthread.h>
#include <stdbool.h>

#include "ushare.h"
#include "metadata.h"
#include "gettext.h"
#include "trace.h"
#include "mime.h"
#include "ufam.h"


/**
 * ufam_entry_new : return a malloc'd ufam_entry_t struct
 */
ufam_entry_t *
ufam_entry_new (ufam_t *ufam, struct upnp_entry_t *entry)
{
  ufam_entry_t *ufam_entry = NULL;

  ufam_entry = malloc (sizeof (ufam_entry_t));
  if (!ufam_entry)
    return NULL;

  ufam_entry->ufam = ufam;
  ufam_entry->entry = entry;

  return ufam_entry;
}

/**
 * ufam_entry_free: destroy ufam_entry
 */
void
ufam_entry_free (ufam_entry_t *ufam_entry)
{
  if (!ufam_entry)
    return;

  free (ufam_entry);
}


/**
 * ufam_thread: new thread to monitor changes in FAM Events
 *  @arg: is a struct ushare_t* var
 */
static void *
ufam_thread (void *arg)
{
  int rc;
  FAMEvent fe;
  struct upnp_entry_t *entry;
  ushare_t *ut = (ushare_t *) arg;

  while (true)
  {
    pthread_mutex_lock (&ut->ufam->startstop_lock);
    if (ut->ufam->stop)
    {
      pthread_cond_signal (&ut->ufam->stop_cond);
      pthread_mutex_unlock (&ut->ufam->startstop_lock);
      break;
    }
    pthread_mutex_unlock (&ut->ufam->startstop_lock);

    rc = FAMPending(&ut->ufam->fc);
    if (rc == -1)
      perror("FAMPending");

    if (rc > 0)
    {
      if (FAMNextEvent(&ut->ufam->fc, &fe) < 0)
      {
        perror("FAMNextEvent");
        exit(1);
      }
      switch ((int)fe.code)
      {
        case FAMChanged:
        case FAMDeleted:
        case FAMCreated:
        case FAMMoved:
          entry = (struct upnp_entry_t *) fe.userdata;
          if (entry)
            log_verbose(_("ufam - dir %s has changed\n"), entry->fullpath);
          /* TODO : rebuild metadat_list for this dir instead of rebuild from scratch */
          free_metadata_list (ut);
          build_metadata_list (ut);
          break;
      }
    }
  }

  pthread_exit (NULL);
  return NULL;
}


/**
 * ufam_init: initialize a new FAM instance
 */
ufam_t *
ufam_init (void)
{
  ufam_t *ufam = NULL;

  ufam = malloc (sizeof (ufam_t));
  if (!ufam)
    return NULL;

  pthread_mutex_init (&ufam->startstop_lock, NULL);
  pthread_cond_init (&ufam->stop_cond, NULL);
  pthread_mutex_init (&ufam->add_monitor_lock, NULL);
  ufam->stop = false;

  pthread_mutex_lock (&ufam->startstop_lock);

  if ((FAMOpen2(&ufam->fc,"ushare")) < 0)
  {
    perror("fam");
    exit(1);
  }

  pthread_mutex_unlock (&ufam->startstop_lock);

  return ufam;
}


/**
 * ufam_start: start the FAM instance - launch the new thread
 */
void
ufam_start (ushare_t *ut)
{

  pthread_mutex_lock (&ut->ufam->startstop_lock);

  if (pthread_create (&ut->ufam->thread, NULL, ufam_thread, (void*) ut))
  {
    perror ("Failed to create thread");
    pthread_mutex_unlock (&ut->ufam->startstop_lock);
    return;
  }

  pthread_mutex_unlock (&ut->ufam->startstop_lock);
}

/**
 * ufam_stop: stop the FAM instance and wait thread to finish
 */
void
ufam_stop (ufam_t *ufam)
{
  if (!ufam)
    return;

  pthread_mutex_lock (&ufam->startstop_lock);
  ufam->stop = true;
  pthread_cond_wait (&ufam->stop_cond, &ufam->startstop_lock);

  pthread_join (ufam->thread, NULL);

  pthread_mutex_unlock (&ufam->startstop_lock);
}

/**
 * ufam_free: free and close the FAM instance
 */
void
ufam_free (ufam_t *ufam)
{
  if (!ufam)
    return;

  pthread_cond_destroy (&ufam->stop_cond);
  pthread_mutex_destroy (&ufam->add_monitor_lock);
  pthread_mutex_destroy (&ufam->startstop_lock);

  if (&ufam->fc)
    FAMClose(&ufam->fc);

  free (ufam);
}


/**
 * ufam_add_monitor: add a new monitor for this entry
 *  note: should only be a Directory to monitor
 * TODO: check that entry is a directory
 */
ufam_entry_t *
ufam_add_monitor (ufam_t *ufam, struct upnp_entry_t *entry)
{
  ufam_entry_t *ufam_entry = NULL;

  if (!entry->fullpath)
    return NULL;

  //log_verbose("ufam - new monitor for %s\n", entry->fullpath);
  pthread_mutex_lock (&ufam->add_monitor_lock);

  ufam_entry = ufam_entry_new (ufam, entry);

  if (FAMMonitorDirectory(&ufam->fc, entry->fullpath, &ufam_entry->fr , (void*) entry ) < 0)
  {
    perror("FAMMonitor failed");
    exit(1);
  }

  pthread_mutex_unlock (&ufam->add_monitor_lock);

  return ufam_entry;
}

/**
 * ufam_remove_monitor: remove this entry from the FAM instance
 */
void
ufam_remove_monitor (ufam_entry_t *ufam_entry)
{
  if (!ufam_entry)
    return;

  pthread_mutex_lock (&ufam_entry->ufam->add_monitor_lock);
  FAMCancelMonitor(&ufam_entry->ufam->fc, &ufam_entry->fr);
  pthread_mutex_unlock (&ufam_entry->ufam->add_monitor_lock);

  ufam_entry_free (ufam_entry);
}

#endif /* HAVE_FAM */
