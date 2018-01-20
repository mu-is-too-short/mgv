/*
 * wacros.h
 *	Convenient macros for widget hacking.
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
#ifndef WLIB_WACROS_H
#define WLIB_WACROS_H
MU_HID(wlib_wacros_h, "$Mu: mgv/wlib/wlib/wacros.h 1.1 2001/03/09 04:16:28 $")

/**
 ** Core
 **/
#define	core_background(w) \
	((w)->core.background_pixel)
#define	core_border_width(w) \
	((w)->core.border_width)
#define	core_bpixmap(w) \
	((w)->core.background_pixmap)
#define	core_colormap(w) \
	((w)->core.colormap)
#define	core_depth(w) \
	((w)->core.depth)
#define	core_height(w) \
	((w)->core.height)
#define	core_width(w) \
	((w)->core.width)

/**
 ** Composite
 **/
#define	comp_children(w) \
	(((CompositeRec *)w)->composite.children)
#define	comp_num_children(w) \
	((int)((CompositeRec *)w)->composite.num_children)

/**
 ** XmManager
 **/
#define man_traversal_on(w) \
	(((XmManagerRec *)w)->manager.traversal_on)

#endif
