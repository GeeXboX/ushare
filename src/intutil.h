/* intutil.h - Integer utility functions header.
 *
 * Copyright (C) 2001, 2002, 2003, 2004, 2005  Oskar Liljeblad
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

#ifndef _INT_UTIL_H_
#define _INT_UTIL_H_

#include <stdbool.h>

char * uint32_str (uint32_t value);
char * int32_str (int32_t value);
bool parse_int32 (const char *instr, int32_t *outint);
bool parse_uint32 (const char *instr, uint32_t *outint);

#endif /* _INT_UTIL_H_ */
