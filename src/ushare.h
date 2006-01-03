/*
 * ushare.h : GeeXboX uShare UPnP Media Server header.
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

#ifndef _USHARE_H_
#define _USHARE_H_

#include <upnp/upnp.h>
#include <upnp/upnptools.h>
#include <stdbool.h>

#include "content.h"
#include "buffer.h"

#define VIRTUAL_DIR "/web"

#define UPNP_DESCRIPTION \
"<?xml version=\"1.0\" encoding=\"utf-8\"?>" \
"<root xmlns=\"urn:schemas-upnp-org:device-1-0\">" \
"  <specVersion>" \
"    <major>1</major>" \
"    <minor>0</minor>" \
"  </specVersion>" \
"  <device>" \
"    <deviceType>urn:schemas-upnp-org:device:MediaServer:1</deviceType>" \
"    <friendlyName>%s</friendlyName>" \
"    <manufacturer>GeeXboX Team</manufacturer>" \
"    <manufacturerURL>http://ushare.geexbox.org/</manufacturerURL>" \
"    <modelDescription>GeeXboX uShare : UPnP Media Server</modelDescription>" \
"    <modelName>uShare</modelName>" \
"    <modelNumber>001</modelNumber>" \
"    <modelURL>http://ushare.geexbox.org/</modelURL>" \
"    <serialNumber>GEEXBOX-USHARE-01</serialNumber>" \
"    <UDN>uuid:%s</UDN>" \
"    <serviceList>" \
"      <service>" \
"        <serviceType>urn:schemas-upnp-org:service:ConnectionManager:1</serviceType>" \
"        <serviceId>urn:upnp-org:serviceId:ConnectionManager</serviceId>" \
"        <SCPDURL>/web/cms.xml</SCPDURL>" \
"        <controlURL>/web/cms_control</controlURL>" \
"        <eventSubURL>/web/cms_event</eventSubURL>" \
"      </service>" \
"      <service>" \
"        <serviceType>urn:schemas-upnp-org:service:ContentDirectory:1</serviceType>" \
"        <serviceId>urn:upnp-org:serviceId:ContentDirectory</serviceId>" \
"        <SCPDURL>/web/cds.xml</SCPDURL>" \
"        <controlURL>/web/cds_control</controlURL>" \
"        <eventSubURL>/web/cds_event</eventSubURL>" \
"      </service>" \
"    </serviceList>" \
"    <presentationURL>/web/ushare.html</presentationURL>" \
"  </device>" \
"</root>"

struct ushare_t {
  char *name;
  char *interface;
  content_list *contentlist;
  struct upnp_entry_t *root_entry;
  int nr_entries;
  int init;
  UpnpDevice_Handle dev;
  char *udn;
  char *ip;
  unsigned short port;
  struct buffer_t *presentation;
  bool use_presentation;
  bool verbose;
  bool daemon;
};

struct action_event_t {
  struct Upnp_Action_Request *request;
  bool status;
  struct service_t *service;
};

inline void display_headers (void);

#endif /* _USHARE_H_ */
