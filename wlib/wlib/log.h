/*
 * log.h
 *	Standard wlib program log.
 *
 * Copyright (C) 1996 Matthew D. Francey
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   Authors:	Matthew D. Francey
 *		Eric A. Howe (mu@trends.net)
 */
#ifndef	WLIB_LOG_H
#define	WLIB_LOG_H
MU_HID(wlib_log_h, "$Mu: mgv/wlib/wlib/log.h 1.1 2001/03/09 04:16:28 $")

#include <stdarg.h>

typedef struct WLLOG_s *WLLOG;

CDECLS_BEGIN
extern WLLOG wl_log_open(Widget shell);
extern WLLOG wl_log_close(WLLOG);
extern void  wl_log_show(WLLOG lg, int show);
extern void  wl_log(WLLOG lg, char *fmt, ...);
extern void  wl_vlog(WLLOG lg, char *fmt, va_list);
CDECLS_END

#endif
