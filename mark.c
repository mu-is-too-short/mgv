/*
 * mark.c
 *	Page marking callbacks for mgv.
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
MU_ID("$Mu: mgv/mark.c 1.38 1999/01/19 04:42:04 $")

#include <Xm/Xm.h>

#include <wlib/sanity.h>
#include <mine/mgv.h>
#include <mine/uiutil.h>
#include <mine/mark.h>

/*
 * mark->mark
 */
void
mgv_mark_mark(Widget w, XtPointer closure, XtPointer call)
{
	MGV *m = (MGV *)closure;

	USEUP(w); USEUP(call);
	wl_assert(MgvOK(m));

	mgv_mark(m, m->page, !m->marked[m->page], TRUE);
}

/*
 * mark->mark all
 */
void
mgv_mark_markall(Widget w, XtPointer closure, XtPointer call)
{
	MGV *m = (MGV *)closure;
	int  i, max;

	USEUP(w); USEUP(call);
	wl_assert(MgvOK(m));

	max = (m->dsc == NULL) ? 0 : m->dsc->n_pages;
	for(i = 0; i < max; ++i)
		mgv_mark(m, i, TRUE, FALSE);
	mgv_list_update(m, -1);
}

/*
 * mark->unmark all
 */
void
mgv_mark_unmarkall(Widget w, XtPointer closure, XtPointer call)
{
	MGV *m = (MGV *)closure;
	int  i, max;

	USEUP(w); USEUP(call);
	wl_assert(MgvOK(m));

	max = (m->dsc == NULL) ? 0 : m->dsc->n_pages;
	for(i = 0; i < max; ++i)
		mgv_mark(m, i, FALSE, FALSE);
	mgv_list_update(m, -1);
}

static void
mgv_mark_evenodd(MGV *m, int start)
{
	int	i, max;

	max   = (m->dsc == NULL) ? 0 : m->dsc->n_pages;
	for(i = start; i < max; i += 2)
		mgv_mark(m, i, TRUE, FALSE);
	mgv_list_update(m, -1);
}

/*
 * mark->odd
 */
void
mgv_mark_odd(Widget w, XtPointer closure, XtPointer call)
{
	MGV *m = (MGV *)closure;

	USEUP(w); USEUP(call);
	wl_assert(MgvOK(m));

	mgv_mark_evenodd(m, 0);
}

/*
 * mark->even
 */
void
mgv_mark_even(Widget w, XtPointer closure, XtPointer call)
{
	MGV *m = (MGV *)closure;

	USEUP(w); USEUP(call);
	wl_assert(MgvOK(m));

	mgv_mark_evenodd(m, 1);
}

/*
 * mark->invert
 */
void
mgv_mark_invert(Widget w, XtPointer closure, XtPointer call)
{
	MGV *m = (MGV *)closure;
	int  i, n;
	Arg  a;

	USEUP(w); USEUP(call);
	wl_assert(MgvOK(m));

	XtSetArg(a, XmNitemCount, &n);
	XtGetValues(m->pagelist, &a, 1);

	for(i = 0; i < n; ++i)
		mgv_mark(m, i, !m->marked[i], FALSE);
	mgv_list_update(m, -1);
}
