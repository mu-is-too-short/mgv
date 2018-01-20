/*
 * WlSplashScreen.h
 *	Public header for the WlSplashScreen widget.
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
#ifndef WLIB_WLSPLASHSCREEN_H
#define WLIB_WLSPLASHSCREEN_H
MU_HID(wlib_wlsplashscreen_h, "$Mu: mgv/wlib/wlib/WlSplashScreen.h 1.2 2001/03/17 18:17:04 $")

/**
 ** NEVER REALIZE ONE OF THESE!
 **
 ** Just create one with XtAppCreateShell ASAP and destroy it when
 ** the real actions starts.  This widget subclasses applicationShell
 ** because we want to be able to create one without having to worry
 ** about a parent widget of any kind.  All this widget does is create
 ** a window (via raw Xlib) with a pixmap or bitmap in it.
 **
 ** This widget is, to some extent, an abuse of Xt but so is XmDisplay
 ** (and sometimes I think the whole damn OSF is an abuse of Xt) so
 ** we've got some kind of precedent.
 **/

#define	XwlNsplashPixmap	"wlSplashPixmap"
#define	XwlCSplashPixmap	"WlSplashPixmap"

#define	XwlNsplashBitmap	"wlSplashBitmap"
#define	XwlCSplashBitmap	"WlSplashBitmap"

#define	XwlNshowSplashScreen	"wlShowSplashScreen"
#define	XwlCShowSplashScreen	"WlShowSplashScreen"

#define	XwlNuseMonoSplash	"wlUseMonoSplash"
#define	XwlCUseMonoSplash	"WlUseMonoSplash"

#define XwlNbitMapData	"wlBitMapData"
#define XwlCBitMapData	"WlBitMapData"
#define XwlRBytes	"WlBytes"

#define XwlNbitMapWidth	"wlBitMapWidth"
#define XwlCBitMapWidth	"WlBitMapWidth"

#define XwlNbitMapHeight	"wlBitMapHeight"
#define XwlCBitMapHeight	"WlBitMapHeight"

#define XwlNpixMapData	"wlPixMapData"
#define XwlCPixMapData	"WlPixMapData"

extern WidgetClass wlSplashScreenWidgetClass;

#define XwlIsSplashScreen(a) (XtIsSubclass(a, wlSplashScreenWidgetClass))

#endif
