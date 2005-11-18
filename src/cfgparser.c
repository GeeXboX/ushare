/*
 * cfgparser.c : GeeXboX uShare config file parser.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cfgparser.h"
#include "ushare.h"

#define USHARE_DIR_DELIM ","

static int
ignore_line (const char *line)
{
  int i;
  size_t len;

  /* commented line */
  if (line[0] == '#' )
    return 1;

  len = strlen (line);

  for (i = 0 ; i < (int) len ; i++ )
    if (line[i] != ' ' && line[i] != '\t' && line[i] != '\n')
      return 0;

  return 1;
}

static char *
strdup_trim (const char *s)
{
  size_t begin, end;
  char *r = NULL;

  if (!s)
    return NULL;
  
  end = strlen (s) - 1;

  for (begin = 0 ; begin < end ; begin++)
    if (s[begin] != ' ' && s[begin] != '\t' && s[begin] != '"')
      break;

  for (; begin < end ; end--)
    if (s[end] != ' ' && s[end] != '\t' && s[end] != '"' && s[end] != '\n')
      break;

  r = strndup (s + begin, end - begin + 1);

  return r;
}

ushare_config *
ushare_config_new (void)
{
  ushare_config *config = (ushare_config *) malloc (sizeof (ushare_config));
  config->name = strdup (DEFAULT_USHARE_NAME);
  config->interface = strdup (DEFAULT_USHARE_IFACE);
  config->content = NULL;

  return config;
}

void
ushare_config_free (ushare_config *c)
{
  if (!c)
    return;
  
  if (c->name)
    free (c->name);
  if (c->interface)
    free (c->interface);
  if (c->content)
    free_content (c->content);

  free (c);
}

void
config_set_name (ushare_config *config, const char *name)
{
  if (!config || !name)
    return;

  if (config->name)
  {
    free (config->name);
    config->name = NULL;
  }

  config->name = strdup_trim (name);
}

void
config_set_interface (ushare_config *config, const char *iface)
{
  if (!config || !iface)
    return;

  if (config->interface)
  {
    free (config->interface);
    config->interface = NULL;
  }

  config->interface = strdup_trim (iface);
}

void
config_add_contentdir (ushare_config *config, const char *dir)
{
  if (!config || !dir)
    return;
  
  config->content = add_content (config->content, dir);
}

void
config_set_contentdir (ushare_config *config, content_list *content)
{
  if (!config || !content)
    return;
  
  config->content = content;
}

int
parse_config_file (ushare_config *config, const char *file)
{
  FILE *conffile;
  char *line = NULL;
  int line_number = 0;
  size_t size = 0, len;
  ssize_t read;
  char *s = NULL, *token = NULL;

  if (!file)
    return -1;
  
  if (!config)
    return -1;
 
  conffile = fopen (file, "r");
  if (!conffile)
    return -1;

  while ((read = getline (&line, &size, conffile)) != -1)
  {
    line_number++;
    len = strlen (line);

    if (ignore_line (line))
      continue;

    if (line[len-1] == '\n')
      line[len-1] = '\0';

    while (line[0] == ' ' || line[0] == '	')
      line++;

    if (!strncmp (line, USHARE_NAME, strlen (USHARE_NAME)))
    {
      s = strchr (line, '=') + 1;
      if (s && s[0] != '\0')
      {
        config_set_name (config, s);
        print_info ("[config] uShare Name: %s\n", config->name);
      }
    }
    else if (!strncmp (line, USHARE_IFACE, strlen (USHARE_IFACE)))
    {
      s = strchr (line, '=') + 1;
      if (s && s[0] != '\0')
      {
        config_set_interface (config, s);
        print_info ("[config] uShare Interface name: %s\n",
                    config->interface);
      }
    }
    else if (!strncmp (line, USHARED_DIR, strlen (USHARED_DIR)))
    {
      s = strchr (line, '=') + 1;
      if (s && s[0] != '\0')
      {
        s = strdup_trim (s);
        print_info ("[config] uShare Shared directories: %s\n", s);

        token = strtok (s, USHARE_DIR_DELIM);
        while (token)
        {
          config_add_contentdir (config, token);
          token = strtok (NULL, USHARE_DIR_DELIM);
        }
        print_info("\n");
      }
    }
  }

  fclose (conffile);
  
  if (s)
    free (s);
  if (line)
    free(line);

  return 0;
}
