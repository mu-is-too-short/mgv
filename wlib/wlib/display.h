/*
 * display.h
 *	Display utilities.
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
 *   Authors:	Eric A. Howe (mu@trends.net)
 */
#ifndef	WLIB_DISPLAY_H
#define	WLIB_DISPLAY_H
MU_HID(wlib_display_h, "$Mu: mgv/wlib/wlib/display.h 1.1 2001/03/09 04:16:28 $")

#include <wlib/wlib.h>

#define wl_display(w)        XtDisplay(wl_top(w))
#define wl_display_string(w) DisplayString(wl_display(w))

#endif
