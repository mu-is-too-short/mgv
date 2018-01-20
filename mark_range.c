/*
 * mark_range.c
 *	Mark range dialog for mgv.
 *
 * Copyright (C) 1999  Eric A. Howe
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
 *   Authors:	Eric A. Howe (mu@trends.net)
 */
#include <wlib/rcs.h>
MU_ID("$Mu: mgv/mark_range.c 1.7 2001/03/17 17:54:48 $")

#include <Xm/Xm.h>
#include <Xm/TextF.h>

#include <wlib/wlib.h>
#include <wlib/sanity.h>
#include <wlib/callbacks.h>
#include <wlib/help.h>
#include <wlib/stdlib.h>
#include <mine/mgv.h>
#include <mine/mark.h>
#include <mine/error.h>
#include <mine/uiutil.h>

static WLW range_rc[] = {
	{"fromLabel", wlLabel,     0, NULL},
	{"fromText",  wlTextField, 0, NULL},
	{"toLabel",   wlLabel,     0, NULL},
	{"toText",    wlTextField, 0, NULL},
	{"hackLabel", wlLabel,     0, NULL},
	{NULL,        NULL,        0, NULL}
};
static WLW range_kids[] = {
	{"rc",         wlRowColumn, WL_MANAGE, range_rc},
	{"apply",      wlPushB,     0,         NULL},
	{"dismiss",    wlPushB,     0,         NULL},
	{"sep",        wlSeparator, 0,         NULL},
	{NULL,         NULL,        0,         NULL}
};
static WLW mark_range_dlg[] = {
	{"markRangeDialog", wlFormDialog, 0, range_kids},
	{NULL,              NULL,         0, NULL}
};

static void
mark_range(Widget w, XtPointer closure, XtPointer call)
{
	MGV  *m = (MGV *)closure;
	char *s;
	int   first, last, i;

	USEUP(call);
	wl_assert(MgvOK(m));
	wl_assert(m->dsc != NULL);

	w = wl_dlg(w);
	first = atoi(s = XmTextFieldGetString(wl_find1(w, "*fromText"))) - 1;
	XtFree(s);
	last  = atoi(s = XmTextFieldGetString(wl_find1(w, "*toText")))  - 1;
	XtFree(s);

	if(first <  0    || last < 0
	|| first >= last || last > m->dsc->n_pages) {
		mgv_error(w, "Invalid page range:\n%d to %d",
				first + 1, last + 1);
		return;
	}

	for(i = first; i <= last; ++i)
		mgv_mark(m, i, TRUE, FALSE);
	mgv_list_update(m, -1);

	wl_dlg_popdown(w);
}

static WLC mark_range_cb[] = {
	{"*apply",   XmNactivateCallback, NULL, mark_range},
	{"*dismiss", XmNactivateCallback, NULL, wl_dismiss},
	{"*help",    XmNactivateCallback, NULL, wl_help},
	{NULL,       NULL,                NULL, NULL}
};

void
mgv_mark_range(Widget w, XtPointer closure, XtPointer call)
{
	MGV *m = (MGV *)closure;

	USEUP(w); USEUP(call);
	wl_assert(MgvOK(m));

	if(m->mark_range == NULL) {
		m->mark_range = wl_create(m->main, mark_range_dlg);
		wl_callback(m->mark_range, &mark_range_cb[0], (XtPointer)m);
	}
	wl_dlg_popup(m->mark_range);

}
