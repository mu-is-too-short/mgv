/*
 * chkver.c
 *	The wl_chkver() function.
 *
 * Copyright (C) 1997 Eric A. Howe
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
#include <wlib/rcs.h>
MU_ID("$Mu: mgv/wlib/chkver.c 1.1 2001/03/09 04:16:27 $")

#include <string.h>
#include <X11/StringDefs.h>

#include <wlib/wlib.h>
#include <wlib/wlibP.h>
#include <wlib/strings.h>
#include <wlib/msg.h>
#include <wlib/build.h>
#include <wlib/resource.h>

/*
 * You should make sure applicationVersion is set to whatever bld_verstring
 * ends up being if you're going to use this function; the build frame work
 * all my stuff uses takes care of these details with very little help.
 */
int
wl_chkver(Widget w)
{
	char *version;

	w = wl_top(w);
	version = wl_res_string(w, XwlNapplicationVersion,
						XwlCApplicationVersion, NULL);
	if(strcmp(bld_verstring, version) == 0)
		return TRUE;
	wl_warn(w, wl_s(w, WlpSbadVersion), version, bld_verstring);
	return FALSE;
}
