/*
 * dismiss.c
 *	Standard dismiss callback for dialogs.
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
#include	<wlib/rcs.h>
MU_ID("$Mu: mgv/wlib/dismiss.c 1.1 2001/03/09 04:16:27 $")

#include <wlib/wlib.h>
#include <wlib/callbacks.h>

/*
 * Unmanage the dialog box which holds "w".
 */
void
wl_dismiss(Widget w, XtPointer closure, XtPointer call)
{
	USEUP(call); USEUP(closure);
	XtUnmanageChild(wl_dlg(w));
}
