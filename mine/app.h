/*
 * app.h
 *	Application resource struff.
 *
 * Copyright (C) 1996  Eric A. Howe
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   Authors:	Matthew D. Francey
 *		Eric A. Howe (mu@trends.net)
 */
#ifndef	MGV_APP_H
#define	MGV_APP_H
MU_HID(app_h, "$Mu: mgv/mine/app.h 1.51 2001/03/12 13:47:27 $")

#include <wlib/app.h>

/*
 * application resources
 */
typedef struct {
	Boolean	antialias_eps;
	Boolean	auto_height;
	Boolean	auto_width;
	String	bunzip, bzip;
	Boolean	center;
	Boolean	confirm_exit;
	Boolean	copy_stdin;
	String	decompress, compress;
	int	default_page_media;
	String	gunzip, gzip;
	Boolean	hide_log;
	Boolean	hide_menu;
	Boolean	hide_status;
	Boolean	hide_toolbar;
	Boolean	hide_pglist;
	Boolean	kill_kids;
	int	magic_resistance;
	Boolean	magic_scroll;
	int	magstep;
	String	pdfconv;
	String	printcmd;
	int	reset_scroll;
	Pixel	reticule_fg;
	int	reticule_width;
	int	scroll_percentage;
	Boolean	show_labels;
	Boolean	show_pgnums;
	Boolean	smart_sizing;
	Boolean	smooth_scroll;
	int	smoothness;
	Boolean suppress_warnings;
	int	wm_width;
	int	wm_height;

	/*
	 * This just the argv form of printcmd, not really a resource.
	 */
	char **printargv;
} MGVR;

CDECLS_BEGIN
extern MGVR *mgv_app_res(Widget);
extern void  mgv_app_close(Widget, XtPointer, XtPointer);
extern void  mgv_app_exit(Widget, XtPointer, XtPointer);
extern MGV  *mgv_app_add(MGV *);
extern int   mgv_app_nextserial(void);
extern void  mgv_app_set(WLAPP);
extern WLAPP mgv_app_get(void);
extern char *mgv_app_tmpdir(void);
CDECLS_END

#endif
