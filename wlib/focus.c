/*
 * focus.c
 *	Focus initialization utilities.  We can't get away with
 *	XmNinitialFocus since it only gets used once and it doesn't
 *	deal with *blah things when coming from resource files (sigh).
 *
 * Copyright (C) 1996  Matthew D. Francey
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   Authors:	Matthew D. Francey
 *		Eric A. Howe (mu@trends.net)
 */
#include <wlib/rcs.h>
MU_ID("$Mu: mgv/wlib/focus.c 1.1 2001/03/09 04:16:27 $")

#include <Xm/Xm.h>

#include <wlib/wlib.h>
#include <wlib/resource.h>

/*
 * A pox on XmNinitialFocus! I curse its worthlessness!
 */
Widget
wl_focus(Widget w)
{
	char *s;

	s = wl_res_string(w, XwlNinitialFocus, XwlCInitialFocus, NULL);
	if(s == NULL || (w = wl_find1(w, s)) == NULL)
		return NULL;
	return XmProcessTraversal(w, XmTRAVERSE_CURRENT) ? w : NULL;
}
