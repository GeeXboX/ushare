/*
 * mime.c : GeeXboX uShare media file MIME-type association.
 * Originally developped for the GeeXboX project.
 * Ref : http://freedesktop.org/wiki/Standards_2fshared_2dmime_2dinfo_2dspec
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
  { "avi", "object.item.videoItem.movie", "http-get:*:video/avi:*"},
  { "dv", "object.item.videoItem.movie", "http-get:*:video/x-dv:*"},
  { "divx", "object.item.videoItem.movie", "http-get:*:video/x-msvideo:*"},
  { "wmv", "object.item.videoItem.movie", "http-get:*:video/x-ms-wmv:*"},
  { "mjpg", "object.item.videoItem.movie",
    "http-get:*:video/x-motion-jpeg:*"},
  { "mjpeg", "object.item.videoItem.movie",
    "http-get:*:video/x-motion-jpeg:*"},
  { "mpeg", "object.item.videoItem.movie", "http-get:*:video/mpeg:*"},
  { "mpg", "object.item.videoItem.movie", "http-get:*:video/mpeg:*"},
  { "mpe", "object.item.videoItem.movie", "http-get:*:video/mpeg:*"},
  { "mp2p", "object.item.videoItem.movie", "http-get:*:video/mp2p:*"},
  { "vob", "object.item.videoItem.movie", "http-get:*:video/mp2p:*"},
  { "mp2t", "object.item.videoItem.movie", "http-get:*:video/mp2t:*"},
  { "m1v", "object.item.videoItem.movie", "http-get:*:video/mpeg:*"},
  { "m2v", "object.item.videoItem.movie", "http-get:*:video/mpeg2:*"},
  { "mpg2", "object.item.videoItem.movie", "http-get:*:video/mpeg2:*"},
  { "mpeg2", "object.item.videoItem.movie", "http-get:*:video/mpeg2:*"},
  { "m4v", "object.item.videoItem.movie", "http-get:*:video/mp4:*"},
  { "m4p", "object.item.videoItem.movie", "http-get:*:video/mp4:*"},
  { "mp4ps", "object.item.videoItem.movie",
    "http-get:*:video/x-nerodigital-ps:*"},
  { "ts", "object.item.videoItem.movie", "http-get:*:video/mpeg2:*"},
  { "ogm", "object.item.videoItem.movie", "http-get:*:video/mpeg:*"},
  { "mkv", "object.item.videoItem.movie", "http-get:*:video/mpeg:*"},
  { "rmvb", "object.item.videoItem.movie", "http-get:*:video/mpeg:*"},
  { "mov", "object.item.videoItem.movie", "http-get:*:video/quicktime:*"},
  { "qt", "object.item.videoItem.movie", "http-get:*:video/quicktime:*"},

  /* Audio files */
  { "aac", "object.item.audioItem.musicTrack", "http-get:*:audio/x-aac:*"},
  { "ac3", "object.item.audioItem.musicTrack", "http-get:*:audio/x-ac3:*"},
  { "aif", "object.item.audioItem.musicTrack", "http-get:*:audio/aiff:*"},
  { "aiff", "object.item.audioItem.musicTrack", "http-get:*:audio/aiff:*"},
  { "at3p", "object.item.audioItem.musicTrack",
    "http-get:*:audio/x-atrac3:*"},
  { "au", "object.item.audioItem.musicTrack", "http-get:*:audio/basic:*"},
  { "snd", "object.item.audioItem.musicTrack", "http-get:*:audio/basic:*"},
  { "dts", "object.item.audioItem.musicTrack", "http-get:*:audio/x-dts:*"},
  { "rmi", "object.item.audioItem.musicTrack", "http-get:*:audio/midi:*"},
  { "mid", "object.item.audioItem.musicTrack", "http-get:*:audio/midi:*"},
  { "mp1", "object.item.audioItem.musicTrack", "http-get:*:audio/mp1:*"},
  { "mp2", "object.item.audioItem.musicTrack", "http-get:*:audio/mp2:*"},
  { "mp3", "object.item.audioItem.musicTrack", "http-get:*:audio/mpeg:*"},
  { "mp4", "object.item.audioItem.musicTrack", "http-get:*:audio/mp4:*"},
  { "m4a", "object.item.audioItem.musicTrack", "http-get:*:audio/mpeg:*"},
  { "ogg", "object.item.audioItem.musicTrack", "http-get:*:audio/x-ogg:*"},
  { "wav", "object.item.audioItem.musicTrack", "http-get:*:audio/wav:*"},
  { "pcm", "object.item.audioItem.musicTrack", "http-get:*:audio/l16:*"},
  { "lpcm", "object.item.audioItem.musicTrack", "http-get:*:audio/l16:*"},
  { "l16", "object.item.audioItem.musicTrack", "http-get:*:audio/l16:*"},
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
  { "bmp", "object.item.imageItem.photo", "http-get:*:image/bmp:*"},
  { "ico", "object.item.imageItem.photo", "http-get:*:image/x-icon:*"},
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
  { "qti", "object.item.imageItem.photo", "http-get:*:image/x-quicktime:*"},
  { "qtf", "object.item.imageItem.photo", "http-get:*:image/x-quicktime:*"},
  { "qtif", "object.item.imageItem.photo", "http-get:*:image/x-quicktime:*"},
  { "tif", "object.item.imageItem.photo", "http-get:*:image/tiff:*"},
  { "tiff", "object.item.imageItem.photo", "http-get:*:image/tiff:*"},

  /* Playlist files */
  { "pls", "object.item.playlistItem", "http-get:*:audio/x-scpls:*"},
  { "m3u", "object.item.playlistItem", "http-get:*:audio/mpegurl:*"},
  { "asx", "object.item.playlistItem", "http-get:*:video/x-ms-asf:*"},

  { NULL, NULL, NULL}
};
