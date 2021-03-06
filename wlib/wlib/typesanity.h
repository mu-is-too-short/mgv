/*
 * typesanity.h
 *	finger preservers for Xt naming conventions.
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
#ifndef	WLIB_TYPESANITY_H
#define	WLIB_TYPESANITY_H
MU_HID(wlib_typesanity_h, "$Mu: mgv/wlib/wlib/typesanity.h 1.2 2001/03/17 18:17:04 $")

/*
 * I'm sorry but these names are just too damn much!
 */
typedef XmToggleButtonCallbackStruct     XmTBS;
typedef XmFileSelectionBoxCallbackStruct XmFSS;
typedef XmListCallbackStruct             XmLSS;

/*
 * Most of the time you don't care about the difference between
 * widgets and gadgets.  I really should be using XmIsArrowButton() and
 * friends inside the macro definitions but the XmIs* macros resolve to
 * XtIsSubclass() macros anyway.
 */
#define WlIsArrowButton(w) (				\
	   XtIsSubclass((w), xmArrowButtonWidgetClass)	\
	|| XtIsSubclass((w), xmArrowButtonGadgetClass)	\
)
#define WlIsCascadeButton(w) (					\
	   XtIsSubclass((w), xmCascadeButtonWidgetClass)	\
	|| XtIsSubclass((w), xmCascadeButtonGadgetClass)	\
)
#define WlIsLabel(w) (					\
	   XtIsSubclass((w), xmLabelWidgetClass)	\
	|| XtIsSubclass((w), xmLabelGadgetClass)	\
)
#define WlIsPushButton(w) (				\
	   XtIsSubclass((w), xmPushButtonWidgetClass)	\
	|| XtIsSubclass((w), xmPushButtonGadgetClass)	\
)
#define WlIsSeparator(w) (				\
	   XtIsSubclass((w), xmSeparatorWidgetClass)	\
	|| XtIsSubclass((w), xmSeparatorGadgetClass)	\
)
#define WlIsToggleButton(w) (				\
	   XtIsSubclass((w), xmToggleButtonWidgetClass)	\
	|| XtIsSubclass((w), xmToggleButtonGadgetClass)	\
)

#endif
