/*
 * mime.c : GeeXboX uShare media file MIME-type association.
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

#include "mime.h"

struct mime_type_t MIME_Type_List[] = {
  /* Video files */
  { "asf", "object.item.videoItem.movie", "http-get:*:video/x-ms-asf:*"},
  { "avi", "object.item.videoItem.movie", "http-get:*:video/x-msvideo:*"},
  { "divx", "object.item.videoItem.movie", "http-get:*:video/x-msvideo:*"},
  { "wmv", "object.item.videoItem.movie", "http-get:*:video/x-ms-wmv:*"},
  { "mpeg", "object.item.videoItem.movie", "http-get:*:video/mpeg:*"},
  { "mpg", "object.item.videoItem.movie", "http-get:*:video/mpeg:*"},
  { "mpe", "object.item.videoItem.movie", "http-get:*:video/mpeg:*"},
  { "vob", "object.item.videoItem.movie", "http-get:*:video/mpeg:*"},
  { "m1v", "object.item.videoItem.movie", "http-get:*:video/mpeg:*"},
  { "m2v", "object.item.videoItem.movie", "http-get:*:video/mpeg:*"},
  { "m4v", "object.item.videoItem.movie", "http-get:*:video/mpeg:*"},
  { "ts", "object.item.videoItem.movie", "http-get:*:video/mpeg:*"},
  { "ogm", "object.item.videoItem.movie", "http-get:*:video/mpeg:*"},
  { "mkv", "object.item.videoItem.movie", "http-get:*:video/mpeg:*"},
  { "rmvb", "object.item.videoItem.movie", "http-get:*:video/mpeg:*"},
  { "mov", "object.item.videoItem.movie", "http-get:*:video/quicktime:*"},
  { "qt", "object.item.videoItem.movie", "http-get:*:video/quicktime:*"},

  /* Audio files */
  { "mp2", "object.item.audioItem.musicTrack", "http-get:*:audio/mpeg:*"},
  { "mp3", "object.item.audioItem.musicTrack", "http-get:*:audio/mpeg:*"},
  { "mp4", "object.item.audioItem.musicTrack", "http-get:*:audio/mpeg:*"},
  { "m4a", "object.item.audioItem.musicTrack", "http-get:*:audio/mpeg:*"},
  { "ogg", "object.item.audioItem.musicTrack",
    "http-get:*:audio/application/ogg:*"},
  { "wav", "object.item.audioItem.musicTrack", "http-get:*:audio/x-wav:*"},
  { "wma", "object.item.audioItem.musicTrack",
    "http-get:*:audio/audio/x-ms-wma:*"},
  { "mka", "object.item.audioItem.musicTrack", "http-get:*:audio/mpeg:*"},
  { "ra", "object.item.audioItem.musicTrack",
    "http-get:*:audio/x-pn-realaudio:*"},
  { "rm", "object.item.audioItem.musicTrack",
    "http-get:*:audio/x-pn-realaudio:*"},
  { "ram", "object.item.audioItem.musicTrack",
    "http-get:*:audio/x-pn-realaudio:*"},

  /* Images files */
  { "bmp", "object.item.imageItem.photo", "http-get:*:image/x-ms-bmp:*"},
  { "gif", "object.item.imageItem.photo", "http-get:*:image/gif:*"},
  { "jpeg", "object.item.imageItem.photo", "http-get:*:image/jpeg:*"},
  { "jpg", "object.item.imageItem.photo", "http-get:*:image/jpeg:*"},
  { "jpe", "object.item.imageItem.photo", "http-get:*:image/jpeg:*"},
  { "pcd", "object.item.imageItem.photo", "http-get:*:image/x-ms-bmp:*"},
  { "png", "object.item.imageItem.photo", "http-get:*:image/png:*"},
  { "pnm", "object.item.imageItem.photo",
    "http-get:*:image/x-portable-anymap:*"},
  { "ppm", "object.item.imageItem.photo",
    "http-get:*:image/x-portable-pixmap:*"},
  
  /* Playlist files */
  { "pls", "object.item.playlistItem", "http-get:*:audio/x-scpls:*"},
  { "m3u", "object.item.playlistItem", "http-get:*:audio/mpegurl:*"},
  { "asx", "object.item.playlistItem", "http-get:*:video/x-ms-asf:*"},

  { NULL, NULL, NULL}
};
