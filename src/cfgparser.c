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
#include <errno.h>

#include "cfgparser.h"
#include "ushare.h"

#define USHARED_DIR_DELIM ","

ushare_config *config;

int to_ignore(const char *line);
char *strdup_tr(const char *s);

static ushare_config *
ushare_config_new (void)
{
  ushare_config* config = (ushare_config*) malloc ( sizeof(ushare_config) );
  if ( !config )
  {
    perror("error malloc");
    exit (2);
  }
  config->name = NULL;
  config->interface = NULL;
  config->content = NULL;
  return config;
}

void
ushare_config_free (ushare_config *c)
{
  if (c)
  {
    if ( c->name )
      free (c->name);
    if ( c->interface )
      free (c->interface);
    if ( c->content )
      free_content (c->content);
    free (c);
  }
}

ushare_config*
config_add_name(ushare_config *config, const char *name)
{
  if (config)
    config->name = strdup_tr (name);
  return config;
}

char*
config_get_name(ushare_config *config)
{
  if (config)
    return config->name;
  return NULL;
}

ushare_config*
config_add_interface(ushare_config *config, const char *iface)
{
  if (config)
    config->interface = strdup_tr (iface);
  return config;
}

char*
config_get_interface(ushare_config *config)
{
  if (config)
    return config->interface;
  return NULL;
}

ushare_config*
config_add_contentdir(ushare_config *config, const char *dir)
{
  if (config)
    config->content = add_content ( config->content, dir );
  return config;
}

ushare_config*
config_set_contentdir(ushare_config *config, content_list *content)
{
  if (config)
    config->content = content;
  return config;
}

content_list*
config_get_contentdir(ushare_config *config)
{
  if (config)
    return config->content;
  return NULL;
}

int
parse_config_file(const char *file)
{
  if (!config)
    config = ushare_config_new ();
  return parse_file(config, file);
}

int
parse_file(ushare_config *config, const char *file)
{
  FILE *conffile;
  char *line = NULL;
  int line_number = 0;
  size_t size = 0, len;
  ssize_t read;
  char *s, *token;

  if (!config)
    exit (2);

  if (!file)
    conffile = fopen (DEFAULT_CONFFILE, "r");
  else
    conffile = fopen (file, "r");

  if (!conffile)
  {
    if ( errno == ENOENT )
    {
      /* file doesn't exist */
      return(-1);
    }
    else
    {
      perror("error fopen");
      exit(2);
    }
  }

  while ((read = getline(&line, &size, conffile)) != -1)
  {
    line_number++;
    len = strlen (line);

    if ( to_ignore (line) )
      continue;

    if (line[len-1] == '\n')
      line[len-1] = '\0';

    while ( line[0] == ' ' || line[0] == '	' )
      line++;

    if ( !strncmp( line, USHARE_NAME, strlen (USHARE_NAME)) )
    {
      s = strchr( line, '=') + 1;
      if (s[0] != '\0')
      {
        add_name (s);
        print_info("[config] uShare Name: %s\n", config->name);
      }
    }
    else if ( !strncmp( line, USHARE_IFACE, strlen (USHARE_IFACE)) )
    {
      s = strchr( line, '=') + 1;
      if (s[0] != '\0')
      {
        add_interface (s);
        print_info("[config] uShare Interface name: %s\n", config->interface);
      }
    }
    else if ( !strncmp( line, USHARED_DIR, strlen (USHARED_DIR)) )
    {
      s = strchr( line, '=') + 1;
      if (s[0] != '\0')
      {
        s = strdup_tr (s);
        print_info("[config] uShare Shared directories: %s\n",s);

        token = strtok( s, USHARED_DIR_DELIM );
        while (token)
        {
          add_contentdir ( token );
          token = strtok ( NULL, USHARED_DIR_DELIM );
        }

        free(s);
        print_info("\n");
      }
    }

  }

  if (line)
    free(line);

  fclose(conffile);

  return 0;
}

int
to_ignore(const char *line)
{
  int i;
  size_t len;

  /* commented line */
  if ( line[0] == '#' )
    return 1;

  len = strlen (line);

  for ( i = 0 ; i < (int) len ; i++ )
  {
    if ( line[i] != ' ' && line[i] != '	' && line[i] != '\n' )
      return 0;
  }

  return 1;
}

char*
strdup_tr(const char *s)
{
  size_t begin, end;
  char *r;
  end = strlen(s) - 1;

  for (begin = 0 ; begin < end ; begin++)
  {
    if ( s[begin] != ' ' && s[begin] != '\t' && s[begin] != '"' )
      break;
  }

  for (; begin < end ; end--)
  {
    if ( s[end] != ' ' && s[end] != '\t' && s[end] != '"' && s[end] != '\n' )
      break;
  }

  r = strndup(s+begin,end-begin+1);

  return(r);
}
