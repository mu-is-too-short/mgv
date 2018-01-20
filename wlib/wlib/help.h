/*
 * help.h
 *	Help system support for wlib.
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
#ifndef	WLIB_HELP_H
#define	WLIB_HELP_H
MU_HID(wlib_help_h, "$Mu: mgv/wlib/wlib/help.h 1.1 2001/03/09 04:16:28 $")

#define WL_ICON_HELP "wlHelp"

CDECLS_BEGIN
extern void wl_help(Widget, XtPointer, XtPointer);
extern void wl_help_context(Widget, XtPointer, XtPointer);
extern int  wl_help_load(Widget, char *, char *);
CDECLS_END

#endif
