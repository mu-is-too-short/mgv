/*
 * about.c
 *	Standard about box convenience function.
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
MU_ID("$Mu: mgv/wlib/about.c 1.3 2001/03/11 18:24:10 $")

#include <string.h>
#include <Xm/Text.h>

#include <wlib/stdlib.h>
#include <wlib/wlib.h>
#include <wlib/callbacks.h>
#include <wlib/sanity.h>
#include <wlib/about.h>
#include <wlib/display.h>
#include <wlib/icons.h>

static WLC bldopts_cb[] = {
	{"*dismiss", XmNactivateCallback, NULL, wl_dismiss},
	{NULL,       NULL,                NULL, NULL},
};
static WLW bldopts_kids[] = {
	{"dismiss", wlPushB,        0,         NULL},
	{"sep",     wlSeparator,    0,         NULL},
	{"text",    wlScrolledText, WL_MANAGE, NULL},
	{NULL,      NULL,           0,         NULL}
};
static WLW bldopts[] = {
	{"bldoptsDialog", wlFormDialog, 0, bldopts_kids},
	{NULL,            NULL,         0, NULL},
};

#define	ABOUT_MAGIC1	0x41424f55
#define	ABOUT_MAGIC2	0x554f4221
typedef struct {
	long    magic1;
	Widget  about;
	Widget  bldopts;
	char *(*version)(void);
	long    magic2;
} ABOUT;

static int
about_ok(ABOUT *a)
{
	DIEIF(a          == NULL);
	DIEIF(a->magic1  != ABOUT_MAGIC1);
	DIEIF(a->magic2  != ABOUT_MAGIC2);
	DIEIF(a->version == NULL);
	DIEIF(a->about   == NULL);
	return TRUE;
}

/*
 * help->about->options
 */
static void
about_options(Widget w, XtPointer closure, XtPointer call)
{
	ABOUT   *about = (ABOUT *)closure;
	String   buf;
	Widget   text;

	USEUP(w); USEUP(call);
	wl_assert(about_ok(about));

	if(about->bldopts != NULL) {
		XtManageChild(about->bldopts);
		XtPopup(XtParent(about->bldopts), XtGrabNone);
		return;
	}

	if((about->bldopts = wl_create(about->about, bldopts)) == NULL)
		return;

	wl_find(about->bldopts, &text, "*textSW*text", NULL);
	XmTextInsert(text, 0, buf = about->version());
	XmTextSetCursorPosition(text, 0);
	wl_free(buf);

	wl_callback(about->bldopts, &bldopts_cb[0], (XtPointer)about);
	XtManageChild(about->bldopts);
	XtPopup(XtParent(about->bldopts), XtGrabNone);
}

static void
pixmap_free(Widget w, XtPointer closure, XtPointer call)
{
	Pixmap	pix;
	Arg     a;

	USEUP(closure); USEUP(call);

	pix = None;
	XtSetArg(a, XmNlabelPixmap, &pix);
	XtGetValues(w, &a, 1);
	wl_free_pixmap(wl_display(w), pix);
}

static void
death(Widget w, XtPointer closure, XtPointer call)
{
	ABOUT	*about = (ABOUT *)closure;

	USEUP(w); USEUP(call);
	wl_assert(about_ok(about));

	wl_free((void *)about);
}

/*
 * the about dialog
 */
static WLC about_cb[] = {
	{"*options",     XmNactivateCallback, NULL, about_options},
	{"*dismiss",     XmNactivateCallback, NULL, wl_dismiss},
	{"*icon",        XmNdestroyCallback,  NULL, pixmap_free},
	{"*aboutDialog", XmNdestroyCallback,  NULL, death},
	{NULL,           NULL,                NULL, NULL}
};
static WLW aboutform[] = {
	{"icon",   wlLabel, 0, NULL},
	{"header", wlLabel, 0, NULL},
	{NULL,     NULL,    0, NULL}
};
static WLW about_kids[] = {
	{"options", wlPushB,        0,         NULL},
	{"dismiss", wlPushB,        0,         NULL},
	{"sep",     wlSeparator,    0,         NULL},
	{"form",    wlForm,         WL_MANAGE, aboutform},
	{"footer",  wlLabel,        0,         NULL},
	{"text",    wlScrolledText, WL_MANAGE, NULL},
	{NULL,      NULL,           0,         NULL}
};
static WLW about_dlg[] = {
	{"aboutDialog", wlFormDialog, 0, about_kids},
	{NULL,         NULL,          0, NULL}
};

/*
 * help->about
 */
Widget
wl_about(Widget parent, char *header, char *footer, char **cpright,
		Pixmap (*mkpix)(char *, Widget), char *(*version)(void))
{
	ABOUT   *about;
	Widget   text, icon;
	char   **s;
	Pixmap   pix;

	if((about = wl_calloc(1, sizeof(ABOUT))) == NULL
	|| (about->about = wl_create(parent, about_dlg)) == NULL) {
		wl_free((void *)about);
		return NULL;
	}
	about->magic1  = ABOUT_MAGIC1;
	about->magic2  = ABOUT_MAGIC2;
	about->version = version == NULL ? wl_version : version;

	wl_find(about->about,
		&text,	"*textSW*text",
		&icon,	"*icon",
		NULL);

	XtVaSetValues(wl_find1(about->about, "*header"),
		XtVaTypedArg,	XmNlabelString,
				XmRString, header,
				strlen(header) + 1,
		NULL);
	XtVaSetValues(wl_find1(about->about, "*footer"),
		XtVaTypedArg,	XmNlabelString,
				XmRString, footer,
				strlen(footer) + 1,
		NULL);

	for(s = cpright; s != NULL && *s != NULL; ++s)
		XmTextInsert(text, XmTextGetLastPosition(text), *s);
	XmTextSetCursorPosition(text, 0);

	wl_callback(XtParent(about->about), &about_cb[0], (XtPointer)about);

	if(mkpix != NULL
	&& (pix = mkpix(WL_ICON_MAIN, parent)) != None) {
		Arg      a[2];
		Cardinal n;

		n = 0;
		XtSetArg(a[n], XmNlabelType,   XmPIXMAP);	++n;
		XtSetArg(a[n], XmNlabelPixmap, pix);		++n;
		wlSetValues(icon, a, n);
	}

	wl_assert(about_ok(about));
	return about->about;
}
