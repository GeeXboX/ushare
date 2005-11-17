/* intutil.c - Integer utility functions.
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

#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

#include "intutil.h"

#define INT_STR_FUNC(n,t,m) \
    char * \
    n(t value) \
    { \
	sprintf(intstr, "%" m, value); \
	return intstr; \
    }

/* Why 23 characters? */
/* 2^64-1 in octal is 22 chars + null byte = 23 */
static char intstr[23];

INT_STR_FUNC(uint32_str, uint32_t, PRIu32)
INT_STR_FUNC(int32_str, int32_t, PRIi32)

bool
parse_int32 (const char *instr, int32_t *outint)
{
  int32_t value = 0;

  if (!instr || !outint)
    return false;

  if (*instr == '-')
  {
    if (instr[1] == '\0') /* FIXME: this should be done on all!!! */
      return false;

    for (instr++; *instr != '\0'; instr++)
    {
      int8_t c = *instr - '0';

      if (c < 0 || c > 9)
        return false;

      if (value < INT32_MIN/10L
          || (value == INT32_MIN/10L && c > -(INT32_MIN%10L)))
        return false;
      value = value*10L - c;
    }
  }
  else
  {
    if (*instr == '\0') /* FIXME: this should be done on all!!! */
      return false;

    for (; *instr != '\0'; instr++)
    {
      int8_t c = *instr - '0';

      if (c < 0 || c > 9)
        return false;

      if (value > INT32_MAX/10L
          || (value == INT32_MAX/10L && c > INT32_MAX%10L))
        return false;
      value = value*10L + c;
    }
  }
  *outint = value;

  return true;
}

bool
parse_uint32 (const char *instr, uint32_t *outint)
{
  uint32_t value = 0;

  if (!instr || !outint)
    return false;

  for (; *instr != '\0'; instr++)
  {
    int8_t c = *instr - '0';

    if (c < 0 || c > 9)
      return false;

    if (value > UINT32_MAX/10L
        || (value == UINT32_MAX/10L && c > (int8_t) UINT32_MAX % 10))
      return false;
    value = value*10L + c;
  }
  *outint = value;

  return true;
}
