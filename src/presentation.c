/*
 * presentation.c : GeeXboX uShare UPnP Presentation Page.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#error "Missing config.h file : run configure again"
#endif

#if HAVE_LANGINFO_CODESET
# include <langinfo.h>
#endif

#include "metadata.h"
#include "content.h"
#include "buffer.h"
#include "presentation.h"
#include "gettext.h"
#include "util_iconv.h"

#define CGI_ACTION "action="
#define CGI_ACTION_ADD "add"
#define CGI_ACTION_DEL "del"
#define CGI_ACTION_REFRESH "refresh"
#define CGI_PATH "path"
#define CGI_SHARE "share"

int
process_cgi (struct ushare_t *ut, char *cgiargs)
{
  char *action = NULL;
  int refresh = 0;

  if (!ut || !cgiargs)
    return -1;

  if (strncmp (cgiargs, CGI_ACTION, strlen (CGI_ACTION)))
    return -1;

  action = strdup ((char *) cgiargs + strlen (CGI_ACTION));
  strtok ((char *) action, "&");

  if (!strcmp (action, CGI_ACTION_ADD))
  {
    char *path = NULL;
    path = strdup (cgiargs + strlen (CGI_ACTION) + strlen (action) + 1);

    if (path && !strncmp (path, CGI_PATH"=", strlen (CGI_PATH) + 1))
    {
      ut->contentlist = add_content (ut->contentlist,
                                     path + strlen (CGI_PATH) + 1);
      refresh = 1;
      free (path);
    }
  }
  else if (!strcmp (action, CGI_ACTION_DEL))
  {
    char *shares,*share;
    int num, shift=0;

    shares = strdup (cgiargs + strlen (CGI_ACTION) + strlen (action) + 1);
    for (share = strtok (shares, "&"); share ; share = strtok (NULL, "&"))
    {
      if (sscanf (share, CGI_SHARE"[%d]=on", &num) < 0)
        continue;
      ut->contentlist = del_content (ut->contentlist, num - shift++);
    }

    refresh = 1;
    free (shares);
  }
  else if (!strcmp (action, CGI_ACTION_REFRESH))
    refresh = 1;

  if (refresh && ut->contentlist)
  {
    free_metadata_list (ut);
    build_metadata_list (ut);
  }

  if (ut->presentation)
    buffer_free (ut->presentation);
  ut->presentation = buffer_new ();

  buffer_append (ut->presentation, "<html>");
  buffer_append (ut->presentation, "<head>");
  buffer_appendf (ut->presentation, "<title>%s</title>",
                  _("uShare Information Page"));
  buffer_append (ut->presentation,
                 "<meta http-equiv=\"pragma\" content=\"no-cache\">");
  buffer_append (ut->presentation,
                 "<meta http-equiv=\"expires\" content=\"1970-01-01\"/>");
  buffer_append (ut->presentation,
                 "<meta http-equiv=\"refresh\" content=\"0; URL=/web/ushare.html\"/>");
  buffer_append (ut->presentation, "</head>");
  buffer_append (ut->presentation, "</html>");

  if (action)
    free (action);

  return 0;
}

int
build_presentation_page (struct ushare_t *ut)
{
  int i;
  char *mycodeset = NULL;

  if (!ut)
    return -1;

  if (ut->presentation)
    buffer_free (ut->presentation);
  ut->presentation = buffer_new ();

#if HAVE_LANGINFO_CODESET
  mycodeset = nl_langinfo (CODESET);
#endif
  if (!mycodeset)
    mycodeset = UTF8;

  buffer_append (ut->presentation, "<html>");
  buffer_append (ut->presentation, "<head>");
  buffer_appendf (ut->presentation, "<title>%s</title>",
                 _("uShare Information Page"));
  buffer_appendf (ut->presentation,
                  "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=%s\"/>",
                  mycodeset);
  buffer_append (ut->presentation,
                 "<meta http-equiv=\"pragma\" content=\"no-cache\"/>");
  buffer_append (ut->presentation,
                 "<meta http-equiv=\"expires\" content=\"1970-01-01\"/>");
  buffer_append (ut->presentation, "</head>");
  buffer_append (ut->presentation, "<body>");
  buffer_append (ut->presentation, "<h1 align=\"center\">");
  buffer_appendf (ut->presentation, "<tt>%s</tt><br/>",
                  _("uShare UPnP A/V Media Server"));
  buffer_append (ut->presentation, _("Information Page"));
  buffer_append (ut->presentation, "</h1>");
  buffer_append (ut->presentation, "<br/>");

  buffer_append (ut->presentation, "<center>");
  buffer_append (ut->presentation, "<tr width=\"500\">");
  buffer_appendf (ut->presentation, "<b>%s :</b> %s<br/>",
                  _("Version"), VERSION);
  buffer_append (ut->presentation, "</tr>");
  buffer_appendf (ut->presentation, "<b>%s :</b> %s<br/>",
                  _("Device UDN"), ut->udn);
  buffer_appendf (ut->presentation, "<b>%s :</b> %d<br/>",
                  _("Number of shared files and directories"), ut->nr_entries);
  buffer_append (ut->presentation, "<br/>");

  buffer_appendf (ut->presentation,
                  "<form method=\"get\" action=\"%s\">", USHARE_CGI);
  buffer_appendf (ut->presentation,
                  "<input type=\"hidden\" name=\"action\" value=\"%s\"/>",
                  CGI_ACTION_DEL);
  for (i = 0 ; i < ut->contentlist->count ; i++)
  {
    buffer_appendf (ut->presentation, "<b>%s #%d :</b>", _("Share"), i + 1);
    buffer_appendf (ut->presentation,
                    "<input type=\"checkbox\" name=\""CGI_SHARE"[%d]\"/>", i);
    buffer_appendf (ut->presentation, "%s<br/>", ut->contentlist->content[i]);
  }
  buffer_appendf (ut->presentation,
                 "<input type=\"submit\" value=\"%s\"/>", _("unShare!"));
  buffer_append (ut->presentation, "</form>");
  buffer_append (ut->presentation, "<br/>");

  buffer_appendf (ut->presentation,
                  "<form method=\"get\" action=\"%s\">", USHARE_CGI);
  buffer_append (ut->presentation, _("Add a new share :  "));
  buffer_appendf (ut->presentation,
                  "<input type=\"hidden\" name=\"action\" value=\"%s\"/>",
                  CGI_ACTION_ADD);
  buffer_append (ut->presentation, "<input type=\"text\" name=\""CGI_PATH"\"/>");
  buffer_appendf (ut->presentation,
                  "<input type=\"submit\" value=\"%s\"/>", _("Share!"));
  buffer_append (ut->presentation, "</form>");

  buffer_append (ut->presentation, "<br/>");

  buffer_appendf (ut->presentation,
                  "<form method=\"get\" action=\"%s\">", USHARE_CGI);
  buffer_appendf (ut->presentation,
                  "<input type=\"hidden\" name=\"action\" value=\"%s\"/>",
                  CGI_ACTION_REFRESH);
  buffer_appendf (ut->presentation, "<input type=\"submit\" value=\"%s\"/>",
                  _("Refresh Shares ..."));
  buffer_append (ut->presentation, "</form>");
  buffer_append (ut->presentation, "</center>");

  buffer_append (ut->presentation, "</body>");
  buffer_append (ut->presentation, "</html>");

  return 0;
}
