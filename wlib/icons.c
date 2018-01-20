/*
 * icons.c
 *	Icon handling for wlib.  This file exists to cleanly isolate
 *	the code that depends on the Xpm library.
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
#include <wlib/rcs.h>
MU_ID("$Mu: mgv/wlib/icons.c 1.2 2001/03/19 19:31:41 $")

#include <limits.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <Xm/XmStrDefs.h>
#include <X11/Shell.h>

#include <wlib/stdlib.h>
#include <wlib/wlib.h>
#include <wlib/wlibP.h>
#include <wlib/icons.h>
#include <wlib/sanity.h>
#include <wlib/display.h>
#include <wlib/resource.h>

typedef struct {
	char *name;
	WLI   icon;
} ICON;
static ICON *icons   = NULL;
static int   n_icons = 0;

static WLI *
find(char *name, int alloc)
{
	int i;
	for(i = 0; i < n_icons; ++i)
		if(strcmp(icons[i].name, name) == 0)
			return &icons[i].icon;

	if(!alloc)
		return NULL;

	icons = wl_realloc(icons, (n_icons + 1)*sizeof(ICON));
	icons[n_icons].name = XtNewString(name);
	memset((void *)&icons[n_icons].icon, '\0', sizeof(WLI));

	return &icons[n_icons++].icon;
}

Pixmap
wl_icon(char *name, Widget w)
{
	Pixmap	  pix  = None;
	Display  *dpy  = wl_display(w);
	WLI      *icon = find(name, FALSE);
	Pixel     fg, bg;

	if(icon == NULL)
		return None;

	wlp_fgbg(w, &fg, &bg);

	if(icon->color != NULL
	&& (pix = wlp_xpm_from_data(w, dpy, bg, icon->color)) != None)
		return pix;

	return wlp_xbm_from_data(w, dpy, fg, bg, icon->bits,
						icon->width, icon->height);
}

int
wl_icons(char *name, Widget top, Pixmap *iconret, Pixmap *maskret)
{
	Display	 *dpy = wl_display(top);
	Pixmap    icon, mask;
	Pixel     fg, bg;
	Arg       a[2];
	Cardinal  n;
	Boolean   install, color;
	WLI      *icons;

	if(iconret != NULL)
		*iconret = None;
	if(maskret != NULL)
		*maskret = None;

	if((icons = find(name, FALSE)) == NULL
	|| icons->mask == NULL)
		return FALSE;

	install = wl_res_boolean(top, XwlNinstallIcon, XwlCInstallIcon, True);
	if(!install)
		return TRUE;

	color = wl_res_boolean(top, XwlNuseColorIcon, XwlCUseColorIcon, True);

	fg   = BlackPixel(dpy, DefaultScreen(dpy));
	bg   = WhitePixel(dpy, DefaultScreen(dpy));
	mask = wlp_xbm_from_data(NULL, dpy, fg, bg, icons->mask, icons->mwidth,
								icons->mheight);

	if(!color
	|| icons->color == NULL
	|| (icon = wlp_xpm_from_data(NULL, dpy, bg, icons->color)) == None)
		icon = wlp_xbm_from_data(NULL, dpy, fg, bg, icons->bits,
						icons->width, icons->height);
	if(icon == None) {
		wl_free_pixmap(dpy, mask);
		if(iconret != NULL)
			*iconret = None;
		if(maskret != NULL)
			*maskret = None;
		return FALSE;
	}

	n = 0;
	XtSetArg(a[n], XmNiconPixmap, icon);		++n;
	if(mask != None) {
		XtSetArg(a[n], XmNiconMask, mask);	++n;
	}
	wlSetValues(top, a, n);

	if(iconret != NULL)
		*iconret = icon;
	if(maskret != NULL)
		*maskret = mask;

	return TRUE;
}

int
wl_icons_install(char *name, WLI *i)
{
	WLI *icon = find(name, TRUE);
	*icon = *i;
	return TRUE;
}

void
wlp_icons_shutdown(void)
{
	int i;
	for(i = 0; i < n_icons; ++i)
		wl_free(icons[i].name);
	icons   = wl_free((XtPointer)icons);
	n_icons = 0;
}
