/*
 * callbacks.h
 *	Standard callback functions for wlib.
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
#ifndef	WLIB_CALLBACKS_H
#define	WLIB_CALLBACKS_H
MU_HID(wlib_callbacks_h, "$Mu: mgv/wlib/wlib/callbacks.h 1.1 2001/03/09 04:16:28 $")

CDECLS_BEGIN
extern void wl_dismiss(Widget, XtPointer, XtPointer);
extern void wl_toggle_sync(Widget, XtPointer, XtPointer);
extern void wl_status_arm(Widget, XtPointer, XtPointer);
extern void wl_status_disarm(Widget, XtPointer, XtPointer);
extern void wl_status_erase(Widget, XtPointer, XtPointer);
CDECLS_END

#endif
