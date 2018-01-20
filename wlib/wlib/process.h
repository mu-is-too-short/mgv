/*
 * process.h
 *	Tools and such for dealing with child processes.
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
#ifndef	WLIB_PROCESS_H
#define	WLIB_PROCESS_H
MU_HID(wlib_process_h, "$Mu: mgv/wlib/wlib/process.h 1.2 2001/03/17 18:17:04 $")

#include <sys/types.h>

#define	WL_PNONE  0
#define	WL_PIN   (1 << 0)
#define	WL_PERR	 (1 << 1)
#define	WL_POUT	 (1 << 2)
#define	WL_PALL	 (WL_PIN | WL_POUT | WL_PERR)

typedef struct {
	int   in, out, err;
	pid_t pid;
	int   flags;
} WLPIPE;

CDECLS_BEGIN
extern int wl_popen(WLPIPE *, char *, char **);
extern int wl_child_msg(Widget, int);
CDECLS_END

#endif
