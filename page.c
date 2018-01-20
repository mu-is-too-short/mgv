/*
 * page.c
 *	Page menu callbacks for mgv.
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
MU_ID("$Mu: mgv/page.c 1.109 1998/11/23 06:58:30 $")

#include <Xm/List.h>
#include <Xm/ToggleB.h>
#include <Xm/ScrollBar.h>

#include <wlib/wlib.h>
#include <wlib/sanity.h>
#include <wlib/typesanity.h>
#include <mine/mgv.h>
#include <mine/app.h>
#include <mine/page.h>
#include <mine/log.h>
#include <mine/sens.h>
#include <mine/auto.h>
#include <mine/center.h>
#include <mine/uiutil.h>
#include <mine/page_control.h>

/*
 * popup->back, popup->forward
 */
void
mgv_page_backforward(Widget w, XtPointer closure, XtPointer call)
{
	MGV *m = (MGV *)closure;

	USEUP(call);

	if(*XtName(w) == 'b')
		mgv_pgstack_back(m->pgstack);
	else
		mgv_pgstack_forward(m->pgstack);
}

/*
 * page->next
 */
void
mgv_page_next(Widget w, XtPointer closure, XtPointer call)
{
	MGV *m = (MGV *)closure;

	USEUP(w); USEUP(call);
	wl_assert(MgvOK(m));

	mgv_pgstack_next(m->pgstack);
}

/*
 * page->prev
 */
void
mgv_page_prev(Widget w, XtPointer closure, XtPointer call)
{
	MGV *m = (MGV *)closure;

	USEUP(w); USEUP(call);
	wl_assert(MgvOK(m));

	mgv_pgstack_prev(m->pgstack);
}

/*
 * page list callback
 */
void
mgv_page_goto(Widget w, XtPointer closure, XtPointer call)
{
	XmLSS *cbs = (XmLSS *)call;
	MGV   *m   = (MGV *)closure;

	USEUP(w);
	wl_assert(MgvOK(m));

	if(m->page == cbs->item_position - 1)
		return;

	/*
	 * Jeez, XmList is one-based (probably so you can use position
	 * zero to indicate the end of the list when using the
	 * XmList.*() convenience functions, -1 would probably be a better
	 * value to indicate "the end"), how silly in a C environment
	 * (even windoze got this one right!).
	 */
	mgv_pgstack_goto(m->pgstack, cbs->item_position - 1);
}

/*
 * show/hide the page labels/numbers
 */
void
mgv_page_labels(Widget w, XtPointer closure, XtPointer call)
{
	XmTBS *cbs = (XmTBS *)call;
	MGV   *m   = (MGV *)closure;

	wl_assert(MgvOK(m));

	if(m->dsc == NULL
	|| m->dsc->pages == NULL)
		return;
	if(strcmp(XtName(w), "showLabels") == 0)
		m->labels = cbs->set;
	else
		m->pgnums = cbs->set;

	if(!m->labels && !m->pgnums) {
		w = wl_find1(m->main, "*popup*hideList");
		XmToggleButtonSetState(w, True, True);
		return;
	}
	if(!m->labels != !m->pgnums
	&& !XtIsManaged(XtParent(m->pagelist))) {
		w = wl_find1(m->main, "*popup*hideList");
		wl_assert(w != NULL);
		XmToggleButtonSetState(w, False, True);
	}

	mgv_list_update(m, -1);

	/*
	 * There doesn't seem to be any simple way to retain
	 * the scroll position.  If you replace in the loop rather
	 * than rebuilding the list, the list won't resize properly
	 * when you the labels go away (i.e. it won't shrink).
	 */
	XmListSetBottomPos(m->pagelist, m->page + 1);

	/*
	 * And the sizing trick (ugh!).
	 * I don't know why I reset the attachment between the
	 * unmanage/manage calls, it is not needed with Motif 2.0.1 but
	 * maybe I needed it for 1.2.  This code is a couple years old
	 * now so I'm just going to leave it alone since it isn't
	 * hurting anything.
	 */
	w = XtParent(m->pagelist);
	if(!XtIsManaged(w))
		return;
	XtUnmanageChild(w);
	XtVaSetValues(w, XmNrightAttachment, XmATTACH_FORM, NULL);
	XtManageChild(w);
}

/*
 * page->center
 */
void
mgv_page_center(Widget w, XtPointer closure, XtPointer call)
{
	MGV   *m   = (MGV *)closure;
	XmTBS *cbs = (XmTBS *)call;

	USEUP(w);
	wl_assert(MgvOK(m));

	m->center = cbs->set ? mgv_center_center : mgv_center_noop;
	mgv_app_res(m->main)->center = cbs->set;
	m->center(m);
}

/*
 * page->redisplay
 */
void
mgv_page_redisplay(Widget w, XtPointer closure, XtPointer call)
{
	MGV *m = (MGV *)closure;

	USEUP(w); USEUP(call);
	wl_assert(MgvOK(m));

	mgv_show(m, m->page);
}

/*
 * page->auto size
 */
void
mgv_page_size(Widget w, XtPointer closure, XtPointer call)
{
	MGV   *m   = (MGV *)closure;
	XmTBS *cbs = (XmTBS *)call;
	MGVR  *res = mgv_app_res(m->main);

	wl_assert(MgvOK(m));

	if(!cbs->set)
		return;

	/*
	 * "What's in a name...", a bloody hell of a lot I'd say.
	 */
	switch(*XtName(w)) {
	case 's':
		m->autosizer     = mgv_auto_size;
		res->auto_height = True;
		res->auto_width  = True;
		break;
	case 'h':
		m->autosizer     = mgv_auto_height;
		res->auto_height = True;
		res->auto_width  = False;
		break;
	case 'w':
		m->autosizer     = mgv_auto_width;
		res->auto_height = False;
		res->auto_width  = True;
		break;
	case 'n':
		m->autosizer     = mgv_auto_noop;
		res->auto_height = False;
		res->auto_width  = False;
		break;
	default:
		wl_assert("unknown toggle!" != NULL);
		break;
	}
	m->autosizer(m);
}
