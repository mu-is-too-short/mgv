/*
 * wlibP.h
 *	Private wlib routines.
 *
 * Copyright (C) 1996  Matthew D. Francey
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
#ifndef	WLIBP_H
#define	WLIBP_H
MU_HID(wlibP_h, "$Mu: mgv/wlib/wlib/wlibP.h 1.1 2001/03/09 04:16:28 $")

#include <stdio.h>

#include <X11/Intrinsic.h>

/*
 * Internal String resources.
 */
#define	WlpScantGet		XwlNstringCantGetFile
#define	WlpScantBrowse		XwlNstringCantBrowse
#define	WlpSbadVersion		XwlNstringBadVersion
#define	WlpScantLoadHelp	XwlNstringCantLoadHelp

CDECLS_BEGIN

/*
 * Xpm/Xbm wrappers.
 */
extern Pixmap wlp_xpm_from_file(Widget, Display *, Pixel, char *);
extern Pixmap wlp_xpm_from_data(Widget, Display *, Pixel, char **);

extern Pixmap wlp_xbm_from_file(Widget, Display *, char *);
extern Pixmap wlp_xbm_from_data(Widget, Display *, Pixel, Pixel, char *, int, int);

/*
 * misc
 */
extern void   wlp_fgbg(Widget, Pixel *, Pixel *);
extern void   wlp_create_shutdown(void);
extern void   wlp_icons_shutdown(void);
extern void   wlp_res_shutdown(void);
extern void   wlp_str_shutdown(void);
extern void   wlp_status_shutdown(void);
extern Window wlp_launch(char *, char **, pid_t *);

CDECLS_END

#endif
