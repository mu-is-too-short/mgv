/*
 * scrollbar.h
 *	Scrollbar utilities.
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
#ifndef	WLIB_SCROLLBAR_H
#define	WLIB_SCROLLBAR_H
MU_HID(wlib_scrollbar_h, "$Mu: mgv/wlib/wlib/scrollbar.h 1.1 2001/03/09 04:16:28 $")

#include <X11/Intrinsic.h>

/*
 * Values struct for wl_[gs]etsbvalues().  You can deal with most
 * of this stuff with XmScrollBar[GS]etValues() but you can't get/set
 * XmNminimum or XmNmaxiumum (and you'll almost always want the bounds
 * to ensure sanity on the slider position).
 */
typedef struct {
	int value;	/* slider position			*/
	int slider;	/* slider size				*/
	int incr;	/* increment/decrement amount		*/
	int pincr;	/* page increment/decrement amount	*/
	int max;	/* XmNmaximum				*/
	int min;	/* XmNminimum				*/
} WLSBV;

CDECLS_BEGIN
extern Widget wl_getsbvalues(Widget, char *, WLSBV *);
extern void   wl_setsbvalues(Widget, WLSBV *, Boolean);
CDECLS_END

#endif
