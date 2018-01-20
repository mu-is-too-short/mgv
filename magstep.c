/*
 * magstep.c
 *	Magstep menu callbacks for mgv.
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
MU_ID("$Mu: mgv/magstep.c 1.62 2001/03/11 18:18:05 $")

#include <Xm/ToggleB.h>
#include <Xm/TextF.h>

#include <wlib/stdlib.h>
#include <wlib/typesanity.h>
#include <wlib/wlib.h>
#include <wlib/sanity.h>
#include <wlib/callbacks.h>
#include <wlib/help.h>
#include <mine/mgv.h>
#include <mine/app.h>
#include <mine/magstep.h>

/*
 * Set magstep and synchronize the widgets.
 *
 * If you are looking for a way to prevent high magsteps from griding your
 * machine into the ground, this is the place for the limits.  All of the
 * magstep setting goes through here.
 */
int
mgv_ui_magnify(MGV *m, int magstep)
{
	int   different;
	char  s[64];
	MGVR *r = mgv_app_res(m->main);

	different = mgv_magnify(m, r->magstep = magstep);

	/*
	 * If the magstep is in the range of the radio buttons, we
	 * need to turn the correct one on and the old one off.
	 */
	if(magstep >= MGV_MAGMIN && magstep <= MGV_MAGMAX) {
		sprintf(s, "*menuBar*magstep_pd*%d", magstep);
		if(m->magstep != NULL)
			XmToggleButtonSetState(m->magstep, False, False);
		m->magstep = wl_find1(m->main, s);
		XmToggleButtonSetState(m->magstep, True, False);
	}
	else {
		if(m->magstep != NULL)
			XmToggleButtonSetState(m->magstep, False, False);
		m->magstep = NULL;
	}

	sprintf(s, "%+d", magstep);

	/*
	 * Update the status line thingy.
	 */
	XtVaSetValues(wl_find1(m->main, "*statusLine*magstepFrame*value"),
		XtVaTypedArg,	XmNlabelString,
				XtRString, s,
				strlen(s) + 1,
		NULL);

	/*
	 * Update the magstepdlg->set dialog (if it exists).
	 */
	if(m->magstepdlg != NULL) {
		Arg a;

		XtSetArg(a, XmNvalue, s);
		XtSetValues(wl_find1(m->magstepdlg, "*text"), &a, 1);
	}

	return different;
}

/*
 * Magstep->increase
 */
void
mgv_magstep_increase(Widget w, XtPointer closure, XtPointer call)
{
	MGV *m = (MGV *)closure;

	USEUP(w); USEUP(call);
	wl_assert(MgvOK(m));

	if(mgv_ui_magnify(m, m->magnification + 1))
		mgv_show(m, m->page);
}

/*
 * Magstep->decrease
 */
void
mgv_magstep_decrease(Widget w, XtPointer closure, XtPointer call)
{
	MGV *m = (MGV *)closure;

	USEUP(w); USEUP(call);
	wl_assert(MgvOK(m));

	if(mgv_ui_magnify(m, m->magnification - 1))
		mgv_show(m, m->page);
}

/*
 * magstep->set
 */
static WLW inner_form[] = {
	{"label", wlLabel,     0, NULL},
	{"decr",  wlArrowB,    0, NULL},
	{"incr",  wlArrowB,    0, NULL},
	{"text",  wlTextField, 0, NULL},
	{NULL,    NULL,        0, NULL}
};
static WLW magstep_form[] = {
	{"form",    wlForm,      WL_MANAGE, inner_form},
	{"apply",   wlPushB,     0,         NULL},
	{"dismiss", wlPushB,     0,         NULL},
	{"help",    wlPushB,     0,         NULL},
	{"sep",     wlSeparator, 0,         NULL},
	{NULL,      NULL,        0,         NULL}
};
static WLW magstep_dlg[] = {
	{"magstepDialog", wlFormDialog, 0, magstep_form},
	{NULL,            NULL,         0, NULL}
};

static void
magstep_apply(Widget w, XtPointer closure, XtPointer call)
{
	MGV   *m = (MGV *)closure;
	String s;
	Arg    a;

	USEUP(w); USEUP(call);
	wl_assert(MgvOK(m));

	XtSetArg(a, XmNvalue, &s);
	XtGetValues(wl_find1(m->magstepdlg, "*text"), &a, 1);

	if(mgv_ui_magnify(m, atoi(s)))
		mgv_show(m, m->page);
	wl_dlg_popdown(m->magstepdlg);
}

static void
magstep_updown(Widget w, XtPointer closure, XtPointer call)
{
	MGV   *m = (MGV *)closure;
	int    n;
	String s;
	char   buf[64];

	USEUP(call);
	wl_assert(MgvOK(m));

	switch(*XtName(w)) {
	case 'i':	n = +1;	break;
	case 'd':	n = -1; break;
	default:	wl_assert("arrow buttons are busted!" != NULL);	return;
	}
	w = wl_find1(m->magstepdlg, "*text");
	sprintf(buf, "%+d", atoi(s = XmTextFieldGetString(w)) + n);
	XtFree(s);
	XmTextFieldSetString(w, buf);
}

static WLC magstep_cb[] = {
	{"*incr",    XmNactivateCallback, NULL, magstep_updown},
	{"*decr",    XmNactivateCallback, NULL, magstep_updown},
	{"*apply",   XmNactivateCallback, NULL, magstep_apply},
	{"*dismiss", XmNactivateCallback, NULL, wl_dismiss},
	{"*help",    XmNactivateCallback, NULL, wl_help},
	{NULL,       NULL,                NULL, NULL}
};

void
mgv_magstep_set(Widget w, XtPointer closure, XtPointer call)
{
	MGV *m = (MGV *)closure;

	USEUP(w); USEUP(call);
	wl_assert(MgvOK(m));

	if(m->magstepdlg == NULL) {
		char	s[64];
		m->magstepdlg = wl_create(m->main, magstep_dlg);
		wl_callback(m->magstepdlg, &magstep_cb[0], (XtPointer)m);
		sprintf(s, "%+d", m->magnification);
		XmTextFieldSetString(wl_find1(m->magstepdlg, "*text"), s);
	}

	wl_dlg_popup(m->magstepdlg);
}

void
mgv_magstep_edit(Widget w, XtPointer closure, XtPointer call)
{
	MGV   *m = (MGV *)closure;
	Widget text;
	Arg    a;

	USEUP(w); USEUP(call);
	wl_assert(MgvOK(m));

	text = wl_find1(m->main, "*statusLine*magstepFrame*text");
	XtManageChild(text);

	XtSetArg(a, XmNrightWidget, text);
	XtSetValues(wl_find1(m->main, "*statusLine*magstepFrame*label"), &a, 1);

	XtUnmanageChild(wl_find1(m->main, "*statusLine*magstepFrame*value"));
	XmProcessTraversal(text, XmTRAVERSE_CURRENT);
}

void
mgv_magstep_text(Widget w, XtPointer closure, XtPointer call)
{
	MGV   *m = (MGV *)closure;
	Widget label, text, value;
	String s;
	Arg    a;

	USEUP(w);
	wl_assert(MgvOK(m));

	wl_find(m->main,
		&text,	"*statusLine*magstepFrame*text",
		&label,	"*statusLine*magstepFrame*label",
		&value,	"*statusLine*magstepFrame*value",
		NULL);
	XtUnmanageChild(text);
	XtManageChild(value);

	XtSetArg(a, XmNrightWidget, value);
	XtSetValues(label, &a, 1);

	if(((XmAnyCallbackStruct *)call)->reason == XmCR_ACTIVATE) {
		Arg a;

		XtSetArg(a, XmNvalue, &s);
		XtGetValues(text, &a, 1);

		if(mgv_ui_magnify(m, atoi(s)))
			mgv_show(m, m->page);
	}
}

void
mgv_magstep_n(Widget w, XtPointer closure, XtPointer call)
{
	MGV	*m   = (MGV *)closure;
	XmTBS	*cbs = (XmTBS *)call;
	int	magstep;

	wl_assert(MgvOK(m));

	if(!cbs->set)
		return;
	magstep = atoi(XtName(w));
	wl_assert(magstep >= MGV_MAGMIN && magstep <= MGV_MAGMAX);
	if(mgv_ui_magnify(m, magstep))
		mgv_show(m, m->page);
}
