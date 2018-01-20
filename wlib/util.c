/*
 * util.c
 *	Miscellaneous utility functions for wlib.
 *
 * Copyright (C) 1998 Eric A. Howe
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
 *		Eric A. Howe (mu@trends.va)
 */
#include <wlib/rcs.h>
MU_ID("$Mu: mgv/wlib/util.c 1.1 2001/03/09 04:16:28 $")

#include <ctype.h>
#include <unistd.h>
#include <X11/Intrinsic.h>

#include <wlib/stdlib.h>
#include <wlib/util.h>

/*
 * Create a copy of 'src' that is all lower case.  The caller
 * should free the returned string with wl_free.
 */
char *
wl_lstrdup(char *src)
{
	return wl_stolower(XtNewString(src));
}

/*
 * Down-case 's' in place.
 */
char *
wl_stolower(char *s)
{
	char *ret;
	for(ret = s; *s != '\0'; ++s)
		*s = tolower((int)*s);
	return ret;
}

/*
 * This doesn't know about quoting or escaping, just white space.
 * You should wl_free argv[0] and argv when you're done with them.
 */
char **
wl_stoargv(char *s)
{
	char **argv;
	char *p, *q;
	int  i, n;

	q = p = strcpy(wl_calloc(strlen(s) + 1, 1), s);
	for(n = 1; *p != '\0'; ++p) {
		if(isspace(*p)) {
			*p = '\0';
			++n;
		}
	}

	argv = wl_calloc(n + 1, sizeof(char *));
	for(i = 0, p = q; i < n; ++i) {
		while(*p == '\0')
			++p;
		argv[i] = p;
		while(*p != '\0')
			++p;
	}

	return argv;
}

/*
 * Dynamic version of getcwd(3).  The returned string should be
 * freed with wl_free().
 */
char *
wl_getcwd(void)
{
	int   size = 32;
	char *buf  = wl_malloc(size);

	while(getcwd(buf, size - 1) == NULL)
		buf = wl_realloc(buf, size *= 2);

	return wl_realloc(buf, strlen(buf) + 1);
}
