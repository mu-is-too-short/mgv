/*
 * dlg.c
 *	Function to find the nearest dialog shell of a widget.
 *
 * Copyright (C) 1998 Eric A. Howe
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
MU_ID("$Mu: mgv/wlib/dlg.c 1.1 2001/03/09 04:16:27 $")

#include <Xm/DialogS.h>

#include <wlib/wlib.h>

Widget
wl_dlgshell(Widget w)
{
	for(; !XmIsDialogShell(w); w = XtParent(w))
		;
	return w;
}

Widget
wl_dlg(Widget w)
{
	for(; XtParent(w) != NULL; w = XtParent(w))
		if(XmIsDialogShell(XtParent(w)))
			return w;
	return NULL;
}
