/*
 * app.h
 *	Public things for app.c.
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
#ifndef	WLIB_APP_H
#define	WLIB_APP_H
MU_HID(wlib_app_h, "$Mu: mgv/wlib/wlib/app.h 1.1 2001/03/09 04:16:28 $")

typedef struct WLAPP_s *WLAPP;

CDECLS_BEGIN

extern Display *wl_app_dpy(WLAPP);
extern WLAPP    wl_app_create(Display *, char *);
extern WLAPP    wl_app_destroy(WLAPP);
extern int      wl_app_res_open(WLAPP, char **, int);
extern void     wl_app_res_store(WLAPP, char *, XtPointer, XtResource *, int);
extern void     wl_app_res_close(WLAPP);
extern char    *wl_app_dotdir(WLAPP);
extern char    *wl_app_tmpdir(WLAPP, Widget);
extern char    *wl_app_resfile(WLAPP);
extern char    *wl_app_userresfile(WLAPP);
extern char    *wl_app_name(WLAPP);

CDECLS_END

#endif
