/*
 * content.c : GeeXboX uShare content list
 * Originally developped for the GeeXboX project.
 * Copyright (C) 2005 Alexis Saettler <asbin@asbin.org>
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

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "content.h"

content_list *
add_content(content_list *list, const char *item)
{
  if (!list)
  {
    list = (content_list*) malloc (sizeof(content_list));
    list->content = NULL;
    list->count = 0;
  }
  if (item)
  {
    list->count++;
    list->content = (char**) realloc (list->content, list->count * sizeof(char*));
    if (!list->content)
    {
      perror ("error realloc");
      exit (2);
    }
    list->content[list->count-1] = strdup (item);
  }
  return list;
}

void
free_content(content_list *list)
{
  int i;
  if (list)
  {
    if (list->content)
    {
      for (i=0 ; i < list->count ; i++)
      {
        if (list->content[i])
          free (list->content[i]);
      }
      free (list->content);
    }
    free (list);
  }
}
