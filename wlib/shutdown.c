/*
 * shutdown.c
 *	wlib shutdown stuff.
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
MU_ID("$Mu: mgv/wlib/shutdown.c 1.1 2001/03/09 04:16:28 $")

#include <wlib/wlib.h>
#include <wlib/wlibP.h>
#include <wlib/help.h>
#include <wlib/helpP.h>

void
wl_shutdown(void)
{
	wlp_help_shutdown();
	wlp_create_shutdown();
	wlp_status_shutdown();
	wlp_res_shutdown();
	wlp_str_shutdown();
	wlp_icons_shutdown();
}
