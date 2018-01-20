/*
 * sens.h
 *	Widget sensitivity handling for wlib.
 *
 * Copyright (C) 1996  Eric A. Howe
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
 *   Authors:	Matthew D. Francey
 *		Eric A. Howe (mu@trends.net)
 */
#ifndef	WLIB_SENS_H
#define	WLIB_SENS_H
MU_HID(wlib_sens_h, "$Mu: mgv/wlib/wlib/sens.h 1.1 2001/03/09 04:16:28 $")

typedef struct {
	char *name;
	int   primary, secondary;
} WLSND;

typedef struct WLSN_s *WLSN;

CDECLS_BEGIN
extern WLSN wl_sens_create(Widget, WLSND *, int);
extern WLSN wl_sens_free(WLSN);
extern void wl_sens(WLSN, int);
CDECLS_END

#endif
