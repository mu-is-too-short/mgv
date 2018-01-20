/*
 * window.c
 *	Window and popup menu callbacks for mgv.
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
MU_ID("$Mu: mgv/view.c 1.13 2001/03/11 18:47:30 $")

#include <Xm/ToggleB.h>

#include <wlib/wlib.h>
#include <wlib/typesanity.h>
#include <wlib/sanity.h>
#include <mine/mgv.h>
#include <mine/app.h>
#include <mine/strings.h>
#include <mine/view.h>
#include <mine/zoom.h>
#include <mine/log.h>

/*
 * window->hide log, popup->hide log
 */
void
mgv_view_log(Widget w, XtPointer closure, XtPointer call)
{
	USEUP(w); USEUP(closure); USEUP(call);
	mgv_log_show(!((XmTBS *)call)->set);
}

static void
layout(MGV *m)
{
	Arg      sw[6], pg[4], tb[2];
	Cardinal sw_n,  pg_n,  tb_n;
	Widget   top;

	sw_n = pg_n = tb_n = 0;
	top  = NULL;

	if(m->bits & MgvPAGELIST) {
		XtSetArg(sw[sw_n], XmNrightAttachment, XmATTACH_FORM);
		++sw_n;
	}
	else {
		XtManageChild(XtParent(m->pagelist));
		XtSetArg(sw[sw_n], XmNrightAttachment, XmATTACH_WIDGET);
		++sw_n;
		XtSetArg(sw[sw_n], XmNrightWidget, XtParent(m->pagelist));
		++sw_n;
	}

	if(m->bits & MgvSTATUSLINE) {
		XtSetArg(sw[sw_n], XmNbottomAttachment, XmATTACH_FORM);
		++sw_n;
		XtSetArg(pg[pg_n], XmNbottomAttachment, XmATTACH_FORM);
		++pg_n;
	}
	else {
		XtManageChild(m->statusline);
		XtSetArg(sw[sw_n], XmNbottomAttachment, XmATTACH_WIDGET);
		++sw_n;
		XtSetArg(sw[sw_n], XmNbottomWidget, m->statusline);
		++sw_n;
		XtSetArg(pg[pg_n], XmNbottomAttachment, XmATTACH_WIDGET);
		++pg_n;
		XtSetArg(pg[pg_n], XmNbottomWidget, m->statusline);
		++pg_n;
	}

	if(m->bits & MgvTOOLBAR) {
		top = NULL;
	}
	else {
		XtManageChild(m->toolbar);
		top = m->toolbar;
	}

	if(m->bits & MgvMENUBAR) {
		if(!(m->bits & MgvMENUDOWN)) {
			XtWidgetGeometry	req;

			/*
			 * Down on your knees rebel dog!
			 */
			memset((void *)&req, '\0', sizeof(req));
			req.request_mode = CWStackMode;
			req.stack_mode   = Below;
			XtMakeGeometryRequest(m->menubar, &req, NULL);
			m->bits |= MgvMENUDOWN;
		}

		if(top != NULL) {
			XtSetArg(tb[tb_n], XmNtopAttachment, XmATTACH_FORM);
			++tb_n;
		}
	}
	else {
		XtManageChild(m->menubar);
		if(top == NULL) {
			top = m->menubar;
		}
		else {
			XtSetArg(tb[tb_n], XmNtopAttachment, XmATTACH_WIDGET);
			++tb_n;
			XtSetArg(tb[tb_n], XmNtopWidget, m->menubar);
			++tb_n;
		}
	}

	if(top != NULL) {
		XtSetArg(sw[sw_n], XmNtopAttachment, XmATTACH_WIDGET);
		++sw_n;
		XtSetArg(sw[sw_n], XmNtopWidget, top);
		++sw_n;
		XtSetArg(pg[pg_n], XmNtopAttachment, XmATTACH_WIDGET);
		++pg_n;
		XtSetArg(pg[pg_n], XmNtopWidget, top);
		++pg_n;
	}
	else {
		XtSetArg(sw[sw_n], XmNtopAttachment, XmATTACH_FORM);
		++sw_n;
		XtSetArg(pg[pg_n], XmNtopAttachment, XmATTACH_FORM);
		++pg_n;
	}

	wlSetValues(m->sw, sw, sw_n);
	wlSetValues(XtParent(m->pagelist), pg, pg_n);
	wlSetValues(m->toolbar, tb, tb_n);

	if(m->bits & MgvPAGELIST)
		XtUnmanageChild(XtParent(m->pagelist));
	if(m->bits & MgvSTATUSLINE)
		XtUnmanageChild(m->statusline);
	if(m->bits & MgvTOOLBAR)
		XtUnmanageChild(m->toolbar);

	m->autosizer(m);
}

/*
 * hide/unhide the page list
 */
void
mgv_view_page(Widget w, XtPointer closure, XtPointer call)
{
	XmTBS *cbs = (XmTBS *)call;
	MGV   *m   = (MGV *)closure;
	MGVR  *r   = mgv_app_res(m->main);

	wl_assert(MgvOK(m));

	if(cbs->set) {
		r->hide_pglist = True;
		m->bits |= MgvPAGELIST;
	}
	else {
		if(!m->labels && !m->pgnums) {
			w = wl_find1(m->main, "*menuBar*page_pd*showPgNums");
			XmToggleButtonSetState(w, True, True);
		}
		r->hide_pglist = False;
		m->bits &= ~MgvPAGELIST;
	}

	layout(m);
}


/*
 * hide/unhide the status line
 */
void
mgv_view_status(Widget w, XtPointer closure, XtPointer call)
{
	XmTBS *cbs = (XmTBS *)call;
	MGV   *m   = (MGV *)closure;
	MGVR  *r   = mgv_app_res(m->main);

	USEUP(w);
	wl_assert(MgvOK(m));

	if(cbs->set) {
		r->hide_status = True;
		m->bits |= MgvSTATUSLINE;
	}
	else {
		r->hide_status = False;
		m->bits &= ~MgvSTATUSLINE;
	}

	layout(m);
}

/*
 * Hide/unhide the menu bar, we don't manage/unmanage because we
 * don't want to lose the accelerators.
 */
void
mgv_view_menu(Widget w, XtPointer closure, XtPointer call)
{
	XmTBS *cbs = (XmTBS *)call;
	MGV   *m   = (MGV *)closure;
	MGVR  *r   = mgv_app_res(m->main);

	USEUP(w);
	wl_assert(MgvOK(m));

	if(cbs->set) {
		r->hide_menu = True;
		m->bits |= MgvMENUBAR;
	}
	else {
		r->hide_menu = False;
		m->bits &= ~MgvMENUBAR;
	}

	layout(m);
}

/*
 * Hide/unhide the tool bar.
 */
void
mgv_view_toolbar(Widget w, XtPointer closure, XtPointer call)
{
	XmTBS *cbs = (XmTBS *)call;
	MGV   *m   = (MGV *)closure;
	MGVR  *r   = mgv_app_res(m->main);

	USEUP(w);

	if(cbs->set) {
		r->hide_toolbar = True;
		m->bits |= MgvTOOLBAR;
	}
	else {
		r->hide_toolbar = False;
		m->bits &= ~MgvTOOLBAR;
	}

	layout(m);
}

/*
 * window->close zooms, popup->close zooms
 */
void
mgv_view_zoom(Widget w, XtPointer closure, XtPointer call)
{
	MGV *m = (MGV *)closure;

	USEUP(w); USEUP(call);
	wl_assert(MgvOK(m));

	mgv_zoom_killall(m);
}
