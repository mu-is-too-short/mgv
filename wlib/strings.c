/*
 * strings.c
 *	string handling for wlib (mostly for language fun)
 *
 * Copyright (C) 1997 Eric A. Howe
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
#include <wlib/rcs.h>
MU_ID("$Mu: mgv/wlib/strings.c 1.1 2001/03/09 04:16:28 $")

#include <ctype.h>
#include <stdio.h>
#include <X11/StringDefs.h>

#include <wlib/stdlib.h>
#include <wlib/strings.h>
#include <wlib/strdefs.h>
#include <wlib/wlib.h>
#include <wlib/wlibP.h>
#include <wlib/resource.h>

typedef struct {
	String	value;
	String	rname;
	String	rclass;
	String	rdefault;
} STRING;

/*
 * This list should be kept sorted on STRING.r.resource_name.
 */
static STRING builtins[] = {
	{
		NULL,
		XwlNstringBadVersion, XwlCStringBadVersion,
		"Bad app-defaults version: wanted '%s', found '%s'"
	}, {
		NULL,
		XwlNstringCantBrowse, XwlCStringCantBrowse,
		"Could not launch browser %s: %s"
	}, {
		NULL,
		XwlNstringCantGetFile, XwlCStringCantGetFile,
		"could not get file name from file selector"
	}, {
		NULL,
		XwlNstringCantLoadHelp, XwlCStringCantLoadHelp,
		"Could not load help file:\nurl     = '%s'\nchapter = '%s'\nsection = '%s'\n"
	}
};
#define	N_BUILTINS	(int)(sizeof(builtins)/sizeof(builtins[0]))

static STRING *strings;
static int n_strings;

static int
cmp(const void *p1, const void *p2)
{
	STRING *s1 = (STRING *)p1;
	STRING *s2 = (STRING *)p2;
	return strcmp(s1->rname, s2->rname);
}

static STRING *
find(String name, STRING *b, int n_b)
{
	STRING t;

	if(b == NULL)
		return NULL;

	t.rname = name;
	return (STRING *)bsearch((void *)&t, (void *)b, n_b,sizeof(STRING),cmp);
}

String
wl_s(Widget w, String name)
{
	STRING *s;

	if((s = find(name, builtins, N_BUILTINS)) == NULL
	&& (s = find(name, strings,  n_strings))  == NULL)
		return name;

	if(s->value == NULL)
		s->value = wl_res_string(wl_top(w), s->rname, s->rclass,
								s->rdefault);
	return s->value;
}

void
wl_sadd(WLS *s, int n_s)
{
	int first = n_strings;
	int i;

	n_strings += n_s;
	strings    = wl_realloc(strings, n_strings*sizeof(STRING));

	for(i = 0; i < n_s; ++i) {
		strings[first + i].value    = NULL;
		strings[first + i].rdefault = XtNewString(s[i].def);
		strings[first + i].rname    = XtNewString(s[i].name);
		strings[first + i].rclass   = XtNewString(s[i].name);
		*strings[first + i].rclass  = tolower((int)*s[i].name);
	}

	qsort((void *)strings, n_strings, sizeof(STRING), cmp);
}

void
wlp_str_shutdown(void)
{
	for(--n_strings; n_strings >= 0; --n_strings) {
		wl_free(strings[n_strings].rname);
		wl_free(strings[n_strings].rclass);
		wl_free(strings[n_strings].rdefault);
	}
	strings = wl_free((XtPointer)strings);
}
