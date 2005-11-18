/*
 * config.c : GeeXboX uShare config file parser headers
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

#ifndef _CONFIG_PARSER_
#define _CONFIG_PARSER_

#include "content.h"

typedef struct ushare_config
{
  char *name;
  char *interface;
  content_list *content;
} ushare_config;


#define USHARE_NAME "USHARE_NAME"
#define USHARE_IFACE "USHARE_IFACE"
#define USHARED_DIR "USHARED_DIR"

#define DEFAULT_CONFFILE "/etc/ushare.conf"
#define DEFAULT_USHARE_NAME "uShare"
#define DEFAULT_USHARE_IFACE "eth0"


ushare_config *new_ushare_config( void );
void free_ushare_config( ushare_config *c);
int parse_config_file(const char *file);
int parse_file(ushare_config *config, const char *file);

ushare_config *config_add_name(ushare_config *config, const char *name);
char *config_get_name(ushare_config *config);
ushare_config *config_add_interface(ushare_config *config, const char *iface);
char *config_get_interface(ushare_config *config);
ushare_config *config_add_contentdir(ushare_config *config, const char *dir);
ushare_config *config_set_contentdir(ushare_config *config, content_list *content);
content_list *config_get_contentdir(ushare_config *config);

#define add_name(name) config_add_name(config,name)
#define get_name() config_get_name(config)
#define add_interface(interface) config_add_interface(config,interface)
#define get_interface() config_get_interface(config)
#define add_contentdir(dir) config_add_contentdir(config,dir)
#define set_contentdir(content) config_set_contentdir(config,content)
#define get_contentdir() config_get_contentdir(config)

extern ushare_config *config;

#endif
