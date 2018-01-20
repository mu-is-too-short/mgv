/*
 * assert.c
 *	assert(3) replacement which writes to stdout rather than stderr.
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
MU_ID("$Mu: mgv/wlib/assert.c 1.1 2001/03/09 04:16:27 $")

#include <stdio.h>

#include <wlib/stdlib.h>
#include <wlib/wlib.h>
#include <wlib/sanity.h>

/*
 * This global enables/disables wl_assert, see wlib/sanity.h for some
 * notes.  You will usually use the wl_enable_assert() function to
 * fiddle with this.
 */
int wlp_asserts_enabled = TRUE;

int
wl_abort(char *expr, char *file, int line, char *function)
{
	if(function != NULL)
		printf("%s:%d: %s: Assertion `%s' failed.\n",
						file, line, function, expr);
	else
		printf("%s:%d: Assertion `%s' failed.\n",
						file, line, expr);
	fflush(stdout);

	abort();
	return 0;	/* placate the blind idiot compiler gods */
}

int
wl_asserts_enabled(void)
{
	return wlp_asserts_enabled;
}

void
wl_assert_enable(int i)
{
	wlp_asserts_enabled = i;
}
