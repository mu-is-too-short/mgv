/*
 * resource.h
 *	Misc convenience functions/macros for dealing with Xt resources.
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
#ifndef	WLIB_RESOURCE_H
#define	WLIB_RESOURCE_H
MU_HID(wlib_resource_h, "$Mu: mgv/wlib/wlib/resource.h 1.1 2001/03/09 04:16:28 $")

#include <stdio.h>
#include <X11/Intrinsic.h>

CDECLS_BEGIN
/*
 * These functions should allocate, via XtMalloc and friends, the return
 * value, the caller will free said memory with XtFree.
 */
typedef char *(*WLFMT)(char *);

extern void  wl_res_addfmt(char *, WLFMT);
extern void  wl_res_decompile(XtResource *, Cardinal);
extern int   wl_res_open(FILE **, char *, char **, int);
extern FILE *wl_res_close(FILE *);
extern void  wl_res_store(FILE *, char *, char *, XtPointer, XtResource *, int);
extern void  wl_res_get(Widget, void *, XtResource *, Cardinal, Arg *,Cardinal);

extern char   *wl_res_string(Widget, char *, char *, char *);
extern int     wl_res_int(Widget, char *, char *, int);
extern Boolean wl_res_boolean(Widget, char *, char *, Boolean);

extern XrmDatabase wl_res_load(char *);

CDECLS_END

#endif
