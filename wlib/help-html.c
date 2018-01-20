/*
 * help-html.c
 *	Extra help support for HTML.
 *
 * Copyright (C) 1996  Matthew D. Francey
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
MU_ID("$Mu: mgv/wlib/help-html.c 1.1 2001/03/09 04:16:27 $")

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

#include <wlib/stdlib.h>
#include <wlib/wlib.h>
#include <wlib/wlibP.h>
#include <wlib/helpP.h>
#include <wlib/display.h>
#include <wlib/sanity.h>
#include <wlib/msg.h>
#include <wlib/strings.h>

/*
 * plain vanilla HTML help with no browser communication 
 */
static Boolean
html(Widget w, WLP_HELP *h)
{
	char   *display;
	char   *url, *argv[5];
	int     n;
	pid_t   brat;
	Boolean status = True;

	n = 0;
	argv[n++] = wlp_help_browser(w);
	if((display = wl_display_string(w)) != NULL) {
		argv[n++] = "-display";
		argv[n++] = display;
	}
	argv[n++] = url = wlp_help_url(w, h->chapter, h->section);
	argv[n++] = NULL;
	ARGSANITY(argv, n);

	brat = fork();
	switch(brat) {
	case 0:
		execvp(argv[0], argv);
		fprintf(stderr, "could not execute %s: %s\n", argv[0],
							strerror(errno));
		exit(EXIT_FAILURE);
		break;
	case -1:
		n = errno;
		wl_warn(wl_top(w), wl_s(w, WlpScantBrowse),
							argv[0], strerror(n));
		status = False;
		break;
	default:
		break;
	}

	wl_free(url);

	return status;
}

void
wlp_help_html_init(void)
{
	wlp_help_add("html", html, NULL);
}
