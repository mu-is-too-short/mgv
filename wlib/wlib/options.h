/*
 * options.h
 *	XrmOptionDescRec utility macros.
 *
 * Copyright (C) 1998 Eric A. Howe
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   Authors:	Eric A. Howe (mu@trends.net)
 */
#ifndef	WLIB_OPTIONS_H
#define	WLIB_OPTIONS_H
MU_HID(wlib_options_h, "$Mu: mgv/wlib/wlib/options.h 1.1 2001/03/09 04:16:28 $")

#define WL_OF(o,r)	{o, r, XrmoptionNoArg,  "False"}
#define WL_OT(o,r)	{o, r, XrmoptionNoArg,  "True" }
#define WL_OS(o,r)	{o, r, XrmoptionSepArg, NULL   }
#define WL_OV(o,r,v)	{o, r, XrmoptionNoArg,  v      }

#endif
