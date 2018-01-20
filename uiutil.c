/*
 * uiutil.c
 *	Miscellaneous UI utility functions for mgv.
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
 *   Authors:	Matthew D. Francey
 *		Eric A. Howe (mu@trends.va)
 */
#include <wlib/rcs.h>
MU_ID("$Mu: mgv/uiutil.c 1.11 1998/11/23 15:34:40 $")

#include <ctype.h>
#include <Xm/Xm.h>
#include <Xm/List.h>

#include <wlib/wlib.h>
#include <wlib/stdlib.h>
#include <wlib/sanity.h>
#include <mine/mgv.h>
#include <mine/app.h>
#include <mine/uiutil.h>
#include <mine/sens.h>

/*
 * Update the page list.
 * If 'page' is non-positive, then the whole list will be updated.
 */
void
mgv_list_update(MGV *m, int page)
{
	MGVR	*r = mgv_app_res(m->main);
	char	*font, *buf;
	int	i;
	XmString *strings;

	if(m->dsc == NULL
	|| m->dsc->n_pages <= 0)
		return;

	r->show_labels = m->labels ? True : False;
	r->show_pgnums = m->pgnums ? True : False;

	if(page > 0) {
		XmString x;
		font = m->marked[page - 1] ? "MARKED" : "UNMARKED";
		x = XmStringCreateLtoR(buf = mgv_label(m, page - 1), font);
		wl_free(buf);
		XmListReplaceItemsPos(m->pagelist, &x, 1, page);
		XmStringFree(x);
		wl_sens(m->senscache, m->sens);
		XmListSelectPos(m->pagelist, m->page + 1, False);
		return;
	}

	strings = wl_calloc(m->dsc->n_pages, sizeof(XmString));
	for(i = 0; i < m->dsc->n_pages; ++i) {
		font = m->marked[i] ? "MARKED" : "UNMARKED";
		strings[i] = XmStringCreateLtoR(buf = mgv_label(m, i), font);
		wl_free(buf);
	}
	XtVaSetValues(m->pagelist,
		XmNitems,	strings,
		XmNitemCount,	m->dsc->n_pages,
		NULL);
	if(m->page >= 0)
		XmListSelectPos(m->pagelist, m->page + 1, False);

	for(i = 0; i < m->dsc->n_pages; ++i)
		XmStringFree(strings[i]);
	wl_free(strings);
	wl_sens(m->senscache, m->sens);
}

MGV *
mgv_userdata(Widget w)
{
	MGV *m;
	Arg  a;

	/*
	 * The MGV is in the mainWindow user data for things that don't
	 * have a useful closure argument.
	 */
	w = wl_findup(w, "mainWindow");
	wl_assert(w != NULL);

	XtSetArg(a, XmNuserData, &m);
	XtGetValues(w, &a, 1);
	wl_assert(MgvOK(m));

	return m;
}
