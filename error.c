/*
 * error.c
 *	Error and warning message handlers.
 *
 * Copyright (C) 1998  Eric A. Howe
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
 *   Authors:	Eric A. Howe (mu@trends.net)
 */
#include <wlib/rcs.h>
MU_ID("$Mu: mgv/error.c 1.1 1998/09/14 21:30:18 $")

#include <stdio.h>

#include <wlib/wlib.h>
#include <wlib/msg.h>
#include <mine/error.h>
#include <mine/strings.h>
#include <mine/log.h>

void
mgv_warn(Widget w, char *fmt, ...)
{
	va_list ap;
	char    msg[2048];

	va_start(ap, fmt);
	vsprintf(msg, fmt, ap);
	va_end(ap);

	mgv_log("%s: %s", wl_s(w, MgvSwarning), msg);
	wl_warn(w, "%s", msg);
}

void
mgv_error(Widget w, char *fmt, ...)
{
	va_list ap;
	char    msg[2048];

	va_start(ap, fmt);
	vsprintf(msg, fmt, ap);
	va_end(ap);

	mgv_log("%s: %s", wl_s(w, MgvSerror), msg);
	wl_error(w, "%s", msg);
}
