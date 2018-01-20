/*
 * text.c
 *	Convenience functions for XmText and XmTextField.
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
 *   Authors:	Eric A. Howe (mu@trends.net)
 */
#include <wlib/rcs.h>
MU_ID("$Mu: mgv/wlib/text.c 1.1 2001/03/09 04:16:28 $")

#include <string.h>
#include <X11/Intrinsic.h>
#include <Xm/XmStrDefs.h>

#include <wlib/text.h>

int
wl_text_size(Widget text)
{
	Arg    a;
	String s = NULL;

	XtSetArg(a, XmNvalue, &s);
	XtGetValues(text, &a, 1);
	return s == NULL ? 0 : strlen(s);
}

Boolean
wl_text_isempty(Widget text)
{
	Arg    a;
	String s = NULL;

	XtSetArg(a, XmNvalue, &s);
	XtGetValues(text, &a, 1);
	return s == NULL || *s == '\0';
}
