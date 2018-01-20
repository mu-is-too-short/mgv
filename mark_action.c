/*
 * mark_action.c
 *	Page marking action for mgv.
 *
 * Copyright (C) 1998 Eric A. Howe
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
 *   Authors:  Eric A. Howe (mu@trends.net)
 */
#include <wlib/rcs.h>
MU_ID("$Mu: mgv/mark_action.c 1.6 2001/03/18 20:06:27 $")

#include <Xm/List.h>

#include <wlib/wlib.h>
#include <wlib/display.h>
#include <wlib/sanity.h>
#include <wlib/stdlib.h>
#include <wlib/util.h>
#include <mine/mgv.h>
#include <mine/mark.h>
#include <mine/uiutil.h>

/*
 * We keep the MGVMC details hidden 'cuz no one else needs to know
 * about it.
 */
struct MGVMC_s {
	int       last;		/* last page fiddled with	*/
	Dimension height;	/* last visible height		*/
	int       top;		/* first visible item		*/
	int       count;	/* visible item count		*/
};

static MGVMC
mc_alloc(Widget w, int page)
{
	MGVMC    mc = wl_malloc(sizeof(struct MGVMC_s));
	Arg      a[2];
	Cardinal n;

	n = 0;
	XtSetArg(a[n], XmNtopItemPosition,  &mc->top);		++n;
	XtSetArg(a[n], XmNvisibleItemCount, &mc->count);	++n;
	wlGetValues(w, a, n);

	/*
	 * We really want the clip window height here but this should
	 * be close enough.
	 */
	n = 0;
	XtSetArg(a[n], XmNheight, &mc->height);	++n;
	wlGetValues(XtParent(w), a, n);

	mc->last = page;

	return mc;
}

static MGVMC
mc_free(MGVMC mc)
{
	return wl_free(mc);
}

/*
 * page marking action
 */
void
mgv_action_mark(Widget w, XEvent *ev, String *argv, Cardinal *argc)
{
	XButtonEvent *bev = (XButtonEvent *)ev;
	MGV  *m = mgv_userdata(w);
	int   page;
	char *arg;

	/*
	 * sanity
	 */
	if(w != m->pagelist
	|| m->dsc        == NULL
	|| m->dsc->pages == NULL) {
		XBell(wl_display(w), 100);
		return;
	}

	page = XmListYToPos(w, bev->y) - 1;

	/*
	 * The zero argument form of mgvMark just toggles the status of
	 * whatever page the pointer is on.
	 */
	if(*argc == 0) {
		mgv_mark(m, page, !m->marked[page], TRUE);
		return;
	}

	arg = wl_lstrdup(*argv);
	if(strcmp(arg, "begin") == 0) {
		wl_assert(m->mc == NULL);
		m->mc = mc_alloc(w, page);
		mgv_mark(m, page, !m->marked[page], TRUE);
	}
	else if(strcmp(arg, "end") == 0) {
		wl_assert(m->mc != NULL);
		m->mc = mc_free(m->mc);
	}
	else if(strcmp(arg, "extend") == 0) {
		MGVMC mc = m->mc;
		wl_assert(mc != NULL);

		/*
		 * Check the event vertical position against the visible
		 * area of the list.  If the mouse is outside the visible
		 * area, we will scroll the list by one position in the
		 * appropriate direction.
		 */
		if(bev->y          >  mc->height
		&& m->dsc->n_pages != page
		&& m->dsc->n_pages >= mc->top + mc->count) {
			++page;
			++mc->top;
			XmListSetPos(w, mc->top);
		}
		else if(bev->y  <  0
		     && page    != 0
		     && mc->top != 0) {
			--page;
			--mc->top;
			XmListSetPos(w, mc->top);
		}

		if(page > mc->last) {
			mgv_mark(m, page, !m->marked[page], TRUE);
			mc->last = page;
		}
		else if(page < mc->last) {
			mgv_mark(m, mc->last, !m->marked[mc->last], TRUE);
			mc->last = page;
		}
	}

	wl_free(arg);
}
