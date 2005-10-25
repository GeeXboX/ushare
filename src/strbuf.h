/* strbuf.h - The string buffer data-structure header.
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

#ifndef _STRBUF_H_
#define _STRBUF_H_

#include <stdint.h>
#include <stdarg.h>

struct strbuf_t {
  char *buf;
  uint32_t len;
  uint32_t capacity;
};

struct strbuf_t *strbuf_new (void);
struct strbuf_t *strbuf_new_with_capacity (uint32_t capacity);
void strbuf_free (struct strbuf_t *sb);

void strbuf_replace_substring_n (struct strbuf_t *sb, int32_t sp, int32_t ep,
                                 uint32_t times, const char *str,
                                 int32_t ssp, int32_t sep);
int strbuf_replacef_n (struct strbuf_t *sb, int32_t sp,
                       int32_t ep, uint32_t times, const char *fmt, ...)
     __attribute__ ((format (printf, 5, 6)));
int strbuf_vreplacef_n (struct strbuf_t *sb, int32_t sp, int32_t ep,
                        uint32_t times, const char *fmt, va_list ap)
     __attribute__ ((format (printf, 5, 0)));

char *strbuf_buffer (struct strbuf_t *sb);
void strbuf_ensure_capacity (struct strbuf_t *sb, uint32_t minimum_capacity);

#define strbuf_append(sb,str) strbuf_append_n(sb,1,str)
#define strbuf_appendf(sb,fmt...) strbuf_appendf_n(sb,1,fmt)
#define strbuf_append_n(sb,n,str) strbuf_replace_n(sb,-1,-1,n,str)
#define strbuf_appendf_n(sb,n,fmt...) strbuf_replacef_n(sb,-1,-1,n,fmt)
#define strbuf_prepend(sb,str) strbuf_prepend_n(sb,1,str)
#define strbuf_prepend_n(sb,n,str) strbuf_replace_n(sb,0,0,n,str)
#define strbuf_replace_n(sb,sp,ep,n,str) strbuf_replace_substring_n(sb,sp,ep,n,str,0,-1)

#endif /* _STRBUF_H_ */
