/*
 * content.h : GeeXboX uShare content list header
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

#ifndef _CONTENT_H_
#define _CONTENT_H_

typedef struct content_list_s {
  char **content;
  int count;
} content_list_t;

content_list_t *content_add (content_list_t *list, const char *item)
    __attribute__ ((malloc));
content_list_t *content_del (content_list_t *list, int n)
    __attribute__ ((nonnull));
void content_free (content_list_t *list)
    __attribute__ ((nonnull));

#endif
