/*
 * msg.h
 *	Error, warning, and message conveniences for wlib.
 *
 * Copyright (C) 1998 Eric A. Howe
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
#ifndef	WLIB_MSG_H
#define	WLIB_MSG_H
MU_HID(wlib_msg_h, "$Mu: mgv/wlib/wlib/msg.h 1.2 2001/03/12 13:47:27 $")

#include <stdarg.h>
#include <X11/Intrinsic.h>

CDECLS_BEGIN
typedef void (*WL_STDERR)(void *, char *);

extern Widget wl_error(Widget, char *, ...);
extern Widget wl_warn(Widget, char *, ...);
extern Widget wl_msg(Widget, char *, ...);
CDECLS_END

#endif
