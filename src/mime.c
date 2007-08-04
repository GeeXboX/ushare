/*
 * mime.c : GeeXboX uShare media file MIME-type association.
 * Originally developped for the GeeXboX project.
 * Ref : http://freedesktop.org/wiki/Standards_2fshared_2dmime_2dinfo_2dspec
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#error "Missing config.h file : run configure again"
#endif

#include <stdlib.h>
#include <string.h>

#include "mime.h"
#include "dlna.h"
#include "ushare.h"

#define UPNP_VIDEO "object.item.videoItem"
#define UPNP_AUDIO "object.item.audioItem.musicTrack"
#define UPNP_PHOTO "object.item.imageItem.photo"
#define UPNP_PLAYLIST "object.item.playlistItem"
#define UPNP_TEXT "object.item.textItem"

const struct mime_type_t MIME_Type_List[] = {
  /* Video files */
  { "asf",   "MPEG4_P2_ASF_SP_G726",     UPNP_VIDEO, "http-get:*:video/x-ms-asf:"},
  { "avc",   "AVC_MP4_BL_CIF15_AAC_520", UPNP_VIDEO, "http-get:*:video/x-msvideo:"},
  { "avi",   "MPEG4_P2_TS_SP_MPEG1_L3",  UPNP_VIDEO, "http-get:*:video/x-msvideo:"},
  { "dv",    "MPEG_PS_NTSC",             UPNP_VIDEO, "http-get:*:video/x-dv:"},
  { "divx",  "MPEG4_P2_TS_SP_MPEG1_L3",  UPNP_VIDEO, "http-get:*:video/x-msvideo:"},
  { "wmv",   "WMVMED_BASE",              UPNP_VIDEO, "http-get:*:video/x-ms-wmv:"},
  { "mjpg",  "MPEG1",                    UPNP_VIDEO, "http-get:*:video/x-motion-jpeg:"},
  { "mjpeg", "MPEG1",                    UPNP_VIDEO, "http-get:*:video/x-motion-jpeg:"},
  { "mpeg",  "MPEG_PS_NTSC",             UPNP_VIDEO, "http-get:*:video/mpeg:"},
  { "mpg",   "MPEG_PS_NTSC",             UPNP_VIDEO, "http-get:*:video/mpeg:"},
  { "mpe",   "MPEG_PS_NTSC",             UPNP_VIDEO, "http-get:*:video/mpeg:"},
  { "mp2p",  "MPEG_PS_NTSC",             UPNP_VIDEO, "http-get:*:video/mp2p:"},
  { "vob",   "MPEG_PS_NTSC",             UPNP_VIDEO, "http-get:*:video/mp2p:"},
  { "mp2t",  "MPEG_TS_SD_NA",            UPNP_VIDEO, "http-get:*:video/mp2t:"},
  { "m1v",   "MPEG1",                    UPNP_VIDEO, "http-get:*:video/mpeg:"},
  { "m2v",   "MPEG_PS_NTSC",             UPNP_VIDEO, "http-get:*:video/mpeg2:"},
  { "mpg2",  "MPEG_PS_NTSC",             UPNP_VIDEO, "http-get:*:video/mpeg2:"},
  { "mpeg2", "MPEG_PS_NTSC",             UPNP_VIDEO, "http-get:*:video/mpeg2:"},
  { "m4v",   "MPEG4_P2_TS_ASP_MPEG1_L3", UPNP_VIDEO, "http-get:*:video/mp4:"},
  { "m4p",   "MPEG4_P2_TS_ASP_MPEG1_L3", UPNP_VIDEO, "http-get:*:video/mp4:"},
  { "mp4ps", "MPEG4_P2_TS_ASP_MPEG1_L3", UPNP_VIDEO, "http-get:*:video/x-nerodigital-ps:"},
  { "ts",    "MPEG_TS_SD_NA",            UPNP_VIDEO, "http-get:*:video/mpeg2:"},
  { "ogm",   NULL,                       UPNP_VIDEO, "http-get:*:video/mpeg:"},
  { "mkv",   "MPEG4_P2_TS_ASP_MPEG1_L3", UPNP_VIDEO, "http-get:*:video/mpeg:"},
  { "rmvb",  NULL,                       UPNP_VIDEO, "http-get:*:video/mpeg:"},
  { "mov",   "AVC_TS_MP_HD_AAC_MULT5",   UPNP_VIDEO, "http-get:*:video/quicktime:"},
  { "qt",    NULL,                       UPNP_VIDEO, "http-get:*:video/quicktime:"},
  { "bin",   NULL,                       UPNP_VIDEO, "http-get:*:video/mpeg2:"},
  { "iso",   NULL,                       UPNP_VIDEO, "http-get:*:video/mpeg2:"},

  /* Audio files */
  { "3gp",  "AMR_3GPP",   UPNP_AUDIO, "http-get:*:audio/3gpp:"},
  { "aac",  "AAC_ADTS",   UPNP_AUDIO, "http-get:*:audio/x-aac:"},
  { "ac3",  "AC3",        UPNP_AUDIO, "http-get:*:audio/x-ac3:"},
  { "aif",  NULL,         UPNP_AUDIO, "http-get:*:audio/aiff:"},
  { "aiff", NULL,         UPNP_AUDIO, "http-get:*:audio/aiff:"},
  { "at3p", "ATRAC3plus", UPNP_AUDIO, "http-get:*:audio/x-atrac3:"},
  { "au",   NULL,         UPNP_AUDIO, "http-get:*:audio/basic:"},
  { "snd",  NULL,         UPNP_AUDIO, "http-get:*:audio/basic:"},
  { "dts",  NULL,         UPNP_AUDIO, "http-get:*:audio/x-dts:"},
  { "rmi",  NULL,         UPNP_AUDIO, "http-get:*:audio/midi:"},
  { "mid",  NULL,         UPNP_AUDIO, "http-get:*:audio/midi:"},
  { "mp1",  "MP3",        UPNP_AUDIO, "http-get:*:audio/mp1:"},
  { "mp2",  "MP3",        UPNP_AUDIO, "http-get:*:audio/mp2:"},
  { "mp3",  "MP3",        UPNP_AUDIO, "http-get:*:audio/mpeg:"},
  { "mp4",  "AAC_ISO_320",UPNP_AUDIO, "http-get:*:audio/mp4:"},
  { "m4a",  "AAC_ISO_320",UPNP_AUDIO, "http-get:*:audio/mp4:"},
  { "ogg",  NULL,         UPNP_AUDIO, "http-get:*:audio/x-ogg:"},
  { "wav",  NULL,         UPNP_AUDIO, "http-get:*:audio/wav:"},
  { "pcm",  "LPCM",       UPNP_AUDIO, "http-get:*:audio/l16:"},
  { "lpcm", "LPCM",       UPNP_AUDIO, "http-get:*:audio/l16:"},
  { "l16",  "LPCM",       UPNP_AUDIO, "http-get:*:audio/l16:"},
  { "wma",  "WMABASE",    UPNP_AUDIO, "http-get:*:audio/x-ms-wma:"},
  { "mka",  NULL,         UPNP_AUDIO, "http-get:*:audio/mpeg:"},
  { "ra",   NULL,         UPNP_AUDIO, "http-get:*:audio/x-pn-realaudio:"},
  { "rm",   NULL,         UPNP_AUDIO, "http-get:*:audio/x-pn-realaudio:"},
  { "ram",  NULL,         UPNP_AUDIO, "http-get:*:audio/x-pn-realaudio:"},

  /* Images files */
  { "bmp",  NULL,      UPNP_PHOTO, "http-get:*:image/bmp:"},
  { "ico",  NULL,      UPNP_PHOTO, "http-get:*:image/x-icon:"},
  { "gif",  NULL,      UPNP_PHOTO, "http-get:*:image/gif:"},
  { "jpeg", "JPEG_SM", UPNP_PHOTO, "http-get:*:image/jpeg:"},
  { "jpg",  "JPEG_SM", UPNP_PHOTO, "http-get:*:image/jpeg:"},
  { "jpe",  "JPEG_SM", UPNP_PHOTO, "http-get:*:image/jpeg:"},
  { "pcd",  NULL,      UPNP_PHOTO, "http-get:*:image/x-ms-bmp:"},
  { "png",  "PNG_LRG", UPNP_PHOTO, "http-get:*:image/png:"},
  { "pnm",  NULL,      UPNP_PHOTO, "http-get:*:image/x-portable-anymap:"},
  { "ppm",  NULL,      UPNP_PHOTO, "http-get:*:image/x-portable-pixmap:"},
  { "qti",  NULL,      UPNP_PHOTO, "http-get:*:image/x-quicktime:"},
  { "qtf",  NULL,      UPNP_PHOTO, "http-get:*:image/x-quicktime:"},
  { "qtif", NULL,      UPNP_PHOTO, "http-get:*:image/x-quicktime:"},
  { "tif",  NULL,      UPNP_PHOTO, "http-get:*:image/tiff:"},
  { "tiff", NULL,      UPNP_PHOTO, "http-get:*:image/tiff:"},

  /* Playlist files */
  { "pls", NULL, UPNP_PLAYLIST, "http-get:*:audio/x-scpls:"},
  { "m3u", NULL, UPNP_PLAYLIST, "http-get:*:audio/mpegurl:"},
  { "asx", NULL, UPNP_PLAYLIST, "http-get:*:video/x-ms-asf:"},

  /* Subtitle Text files */
  { "srt", NULL, UPNP_TEXT, "http-get:*:text/srt:"}, /* SubRip */
  { "ssa", NULL, UPNP_TEXT, "http-get:*:text/ssa:"}, /* SubStation Alpha */
  { "stl", NULL, UPNP_TEXT, "http-get:*:text/srt:"}, /* Spruce */
  { "psb", NULL, UPNP_TEXT, "http-get:*:text/psb:"}, /* PowerDivX */
  { "pjs", NULL, UPNP_TEXT, "http-get:*:text/pjs:"}, /* Phoenix Japanim */
  { "sub", NULL, UPNP_TEXT, "http-get:*:text/sub:"}, /* MicroDVD */
  { "idx", NULL, UPNP_TEXT, "http-get:*:text/idx:"}, /* VOBsub */
  { "dks", NULL, UPNP_TEXT, "http-get:*:text/dks:"}, /* DKS */
  { "scr", NULL, UPNP_TEXT, "http-get:*:text/scr:"}, /* MACsub */
  { "tts", NULL, UPNP_TEXT, "http-get:*:text/tts:"}, /* TurboTitler */
  { "vsf", NULL, UPNP_TEXT, "http-get:*:text/vsf:"}, /* ViPlay */
  { "zeg", NULL, UPNP_TEXT, "http-get:*:text/zeg:"}, /* ZeroG */
  { "mpl", NULL, UPNP_TEXT, "http-get:*:text/mpl:"}, /* MPL */

  /* Miscellaneous text files */
  { "bup", NULL, UPNP_TEXT, "http-get:*:text/bup:"}, /* DVD backup */
  { "ifo", NULL, UPNP_TEXT, "http-get:*:text/ifo:"}, /* DVD information */

  { NULL, NULL, NULL, NULL}
};

char *mime_get_protocol (struct mime_type_t *mime)
{
  extern struct ushare_t *ut;
  char protocol[512];
  
  if (!mime)
    return NULL;

  sprintf (protocol, mime->mime_protocol);
  
  if (ut->dlna && mime->dlna_type)
  {
    char dlna_info[448];
    sprintf (dlna_info, "%s=%s;%s=%s;%s=%s;%s=%s;%s=%s",
             DLNA_ORG_PS, DLNA_ORG_PS_VAL,
             DLNA_ORG_CI, DLNA_ORG_CI_VAL,
             DLNA_ORG_OP, DLNA_ORG_OP_VAL,
             DLNA_ORG_PN, mime->dlna_type,
             DLNA_ORG_FLAGS, DLNA_ORG_FLAGS_VAL);
    strcat (protocol, dlna_info);
  }
  else
    strcat (protocol, "*");

  return strdup (protocol);
}
