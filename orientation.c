/*
 * orientation.c
 *	Orientation menu callbacks for mgv.
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
MU_ID("$Mu: mgv/orientation.c 1.33 1998/11/11 05:59:53 $")

#include <Xm/ToggleB.h>

#include <wlib/typesanity.h>
#include <wlib/sanity.h>
#include <mine/mgv.h>
#include <mine/orientation.h>

/*
 * orientation->*
 */
static struct {
	char    *name;
	GvOrient orientation;
} orients[] = {
	{"portrait",	GvOrientPortrait},
	{"landscape",	GvOrientLandscape},
	{"upsideDown",	GvOrientUpsideDown},
	{"seascape",	GvOrientSeascape},
};
#define	N_ORIENTS	(int)(sizeof(orients)/sizeof(orients[0]))

void
mgv_orientation(Widget w, XtPointer closure, XtPointer call)
{
	XmTBS *cbs = (XmTBS *)call;
	MGV   *m   = (MGV *)closure;
	char  *name;
	int    i;

	wl_assert(MgvOK(m));

	/*
	 * ignore unsetting
	 */
	if(!cbs->set)
		return;

	for(i = 0, name = XtName(w); i < N_ORIENTS; ++i) {
		if(strcmp(name, orients[i].name) == 0) {
			mgv_orient(m, orients[i].orientation, TRUE);
			m->orientation = w;
			mgv_show(m, m->page);
			return;
		}
	}

	wl_assert("mgv_orientation(): unknown widget!" != NULL);
	return;
}
