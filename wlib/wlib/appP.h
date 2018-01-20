/*
 * appP.h
 *	Private things for app.c.
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
#ifndef	WLIB_APPP_H
#define	WLIB_APPP_H
MU_HID(wlib_appp_h, "$Mu: mgv/wlib/wlib/appP.h 1.1 2001/03/09 04:16:28 $")

#include <stdio.h>
#include <X11/Intrinsic.h>

#include <wlib/app.h>

struct WLAPP_s {
	Display *dpy;	/* the display					*/
	FILE    *fp;	/* the (maybe) open resources			*/
	char    *cls;	/* the application class name			*/
	char    *dir;	/* the application dot directory		*/
	char    *user;	/* the user edittable resource file name	*/
	char    *rc;	/* the resource file name			*/
	char    *tmp;	/* the temporary directory			*/
};

#endif
