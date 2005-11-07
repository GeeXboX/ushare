/* strbuf.c - The string buffer data-structure.
 *
 * Copyright (C) 2004, 2005 Oskar Liljeblad
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
 *
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "minmax.h"
#include "strbuf.h"

#define DEFAULT_STRBUF_CAPACITY 16

#define SWAP_INT32(a,b) { int32_t _t = (a); (a) = (b); (b) = _t; }

static int32_t
normalize_strbuf_pos (struct strbuf_t *sb, int32_t pos)
{
  if (!sb)
    return 0;

  if (pos >= (int32_t) sb->len)
    return sb->len;
  if (pos >= 0)
    return pos;
  pos += 1 + sb->len;
  if (pos >= 0)
    return pos;
  return 0;
}

static int32_t
normalize_str_pos (const char *str, int32_t pos)
{
  if (!str)
    return 0;

  if (pos >= 0)
    return strnlen (str, pos);
  pos += 1 + strlen (str);
  if (pos >= 0)
    return pos;

  return 0;
}

char *
strbuf_buffer (struct strbuf_t *sb)
{
  if (!sb)
    return NULL;

  return sb->buf;
}

struct strbuf_t *
strbuf_new (void)
{
  return strbuf_new_with_capacity (DEFAULT_STRBUF_CAPACITY);
}

struct strbuf_t *
strbuf_new_with_capacity (uint32_t capacity)
{
  struct strbuf_t *sb = NULL;

  sb = malloc (sizeof (struct strbuf_t));
  sb->len = 0;
  sb->capacity = capacity;
  sb->buf = malloc (sb->capacity * sizeof (char));
  if (sb->capacity > 0)
    sb->buf[0] = '\0';

  return sb;
}

void
strbuf_free (struct strbuf_t *sb)
{
  if (!sb)
    return;

  if (sb->buf)
    free (sb->buf);
  free (sb);
}

void
strbuf_replace_data_n (struct strbuf_t *sb, int32_t sp, int32_t ep,
                       uint32_t times, const void *mem, uint32_t len)
{
  uint32_t addlen, dellen;

  if (!sb)
    return;

  sp = normalize_strbuf_pos (sb, sp);
  ep = normalize_strbuf_pos (sb, ep);

  if (sp > ep)
    SWAP_INT32 (sp, ep);

  addlen = len * times;
  dellen = ep - sp;

  if (addlen != dellen)
  {
    strbuf_ensure_capacity (sb, sb->len + 1 - dellen + addlen);
    memmove (sb->buf + sp + addlen, sb->buf+ep, sb->len + 1 - ep);
    sb->len += addlen - dellen;
  }

  if (addlen > 0)
    for (; times > 0; times--)
    {
      memcpy (sb->buf + sp, mem, len);
      sp += len;
    }
}

void
strbuf_replace_substring_n (struct strbuf_t *sb, int32_t sp, int32_t ep,
                            uint32_t times, const char *substr,
                            int32_t subsp, int32_t subep)
{
  if (!sb || !substr)
    return;

  subsp = normalize_str_pos (substr, subsp);
  subep = normalize_str_pos (substr, subep);

  if (subsp > subep)
    SWAP_INT32 (subsp, subep);

  strbuf_replace_data_n (sb, sp, ep, times, substr + subsp, subep - subsp);
}

int
strbuf_replacef_n (struct strbuf_t *sb, int32_t sp, int32_t ep,
                   uint32_t times, const char *fmt, ...)
{
  va_list ap;
  int len;

  va_start (ap, fmt);
  len = strbuf_vreplacef_n (sb, sp, ep, times, fmt, ap);
  va_end (ap);

  return len;
}

int
strbuf_vreplacef_n (struct strbuf_t *sb, int32_t sp, int32_t ep,
                    uint32_t times, const char *fmt, va_list ap)
{
  char *str;
  int len;

  if (!sb || !fmt)
    return -1;

  sp = normalize_strbuf_pos (sb, sp);
  ep = normalize_strbuf_pos (sb, ep);
  if (sp > ep)
    SWAP_INT32 (sp, ep);

  len = vasprintf (&str, fmt, ap);

  strbuf_replace_substring_n (sb, sp, ep, times, str, 0, len);
  free (str);

  return len;
}

void
strbuf_ensure_capacity (struct strbuf_t *sb, uint32_t min_capacity)
{
  if (!sb)
    return;

  if (min_capacity > sb->capacity)
  {
    sb->capacity = MAX (min_capacity, sb->len * 2 + 2); /* XXX: MAX -> max */
    sb->buf = realloc (sb->buf, sb->capacity * sizeof (char));
    if (sb->len == 0)
      sb->buf[0] = '\0';
  }
}
