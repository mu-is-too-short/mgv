/*
 * icons.h
 *	Icon handling for wlib.
 *
 * Copyright (C) 1997 Eric A. Howe
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
#ifndef	WLIB_ICONS_H
#define	WLIB_ICONS_H
MU_HID(wlib_icons_h, "$Mu: mgv/wlib/wlib/icons.h 1.1 2001/03/09 04:16:28 $")

#include <X11/Intrinsic.h>

#include <wlib/strdefs.h>

/*
 * Standard name for the main application.
 */
#define WL_ICON_MAIN "wlMain"

/*
 * We don't (currently) bother allocating any of these things
 * in wl_icon_init().
 */
typedef struct {
	/*
	 * Color icon, XPM format.
	 */
	char **color;

	/*
	 * Monochrome icon, XBM format.
	 */
	char *bits;
	int   width, height;

	/*
	 * Icon mask, XBM format.
	 */
	char *mask;
	int   mwidth, mheight;
} WLI;

CDECLS_BEGIN
extern int    wl_icons_install(char *, WLI *);
extern Pixmap wl_icon(char *, Widget);
extern int    wl_icons(char *, Widget, Pixmap *, Pixmap *);
CDECLS_END

#endif
