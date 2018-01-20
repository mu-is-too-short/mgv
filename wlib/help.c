/*
 * help.c
 *	Help support functions.  This file is part of wlib.
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
MU_ID("$Mu: mgv/wlib/help.c 1.2 2001/03/09 16:22:50 $")

#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <X11/cursorfont.h>
#include <Xm/Xm.h>

#include <wlib/stdlib.h>
#include <wlib/wlib.h>
#include <wlib/help.h>
#include <wlib/wlibP.h>
#include <wlib/helpP.h>
#include <wlib/resource.h>
#include <wlib/util.h>
#include <wlib/display.h>
#include <wlib/process.h>

typedef struct {
	char        *name;
	WLP_HELPFN   handler;
	WLP_HELPSHUT shut;
	int          dying;
} HELPENT;

static HELPENT *helptypes   = NULL;
static int      n_helptypes = 0;

typedef struct {
	WLP_HELPFN handler;
	String     chapter;
	String     section;
} HELP;

/*
 * string->help function type converter
 */
static Boolean
string2help(Display *dpy, XrmValue *a, Cardinal *n, XrmValue *from,
						XrmValue *to, XtPointer *data)
{
	int   i;
	char *lower;

	USEUP(dpy); USEUP(a); USEUP(n); USEUP(data);

	/*
	 * Sanity.
	 */
	if(*n != 0) {
		XtWarningMsg("wrongParameters", "cvsStringToHelpType",
			"XtToolkitError",
			"String to HelpType conversion needs no extra arguments",
			NULL, NULL);
	}

	if(from->addr == NULL)
		return False;
	if(to->addr != NULL && to->size < sizeof(XtPointer)) {
		to->size = sizeof(XtPointer);
		return False;
	}
	if(to->addr == NULL) {
		to->addr = wl_malloc(sizeof(XtPointer));
		to->size = sizeof(XtPointer);
	}

	lower = wl_lstrdup((char *)from->addr);
	for(i = 0; i < n_helptypes; ++i) {
		if(strcmp(lower, helptypes[i].name) != 0)
			continue;
		*(XtPointer *)to->addr = (XtPointer)helptypes[i].handler;
		break;
	}

	/*
	 * Not found, use the default.
	 * This should agree with how `help_default' behaves.
	 */
	if(i == n_helptypes) {
		*(XtPointer *)to->addr = (XtPointer)helptypes[i - 1].handler;
		XtDisplayStringConversionWarning(dpy, (String)from->addr,
								XwlRHelpType);
	}

	wl_free(lower);
	return True;
}

/*
 * Get the default help handler.
 * This just grabs the last entry in the help list.  We assume that
 * `wlp_help_init' initializes things in the right way and that
 * `wlp_help_add' just appends new entries.  We go for the last entry
 * in case the outside world has added a help handler; of course, this
 * assumes that `wlp_help_init' will be called before the outside handlers
 * are added.
 */
static void
help_default(Widget w, int offset, XrmValue *v)
{
static	XtPointer p;
	USEUP(w); USEUP(offset);

	/*
	 * Oh well, not much we can do here.
	 */
	if(n_helptypes == 0) {
		v->addr = NULL;
		v->size = 0;
		return;
	}

	p = (XtPointer)helptypes[n_helptypes - 1].handler;
	v->addr = (XtPointer)&p;
	v->size = sizeof(WLP_HELPFN);
}

void
wlp_help_init(void)
{
	/*
	 * Initialize the standard help handlers.
	 * We use this order so that `help_default' can just pick the
	 * last entry as the "preferred" default help system.
	 */
	wlp_help_html_init();
	wlp_help_remote_init();

	XtSetTypeConverter(XtRString, XwlRHelpType, string2help, NULL,
							0, XtCacheNone, NULL);
}

static void
help_clear(HELPENT *h)
{
	if(h == NULL)
		return;
	wl_free(h->name);
	memset((void *)h, '\0', sizeof(HELPENT));
}

void
wlp_help_shutdown(void)
{
	int i;
	for(i = 0; i < n_helptypes; ++i) {
		helptypes[i].dying = TRUE;
		if(helptypes[i].shut != NULL)
			helptypes[i].shut();
		help_clear(&helptypes[i]);
	}
	helptypes   = wl_free((XtPointer)helptypes);
	n_helptypes = 0;
}

void
wlp_help_add(char *name, WLP_HELPFN handler, WLP_HELPSHUT shut)
{
	int   i;
	char *s;

	s = wl_lstrdup(name);
	for(i = 0; i < n_helptypes; ++i) {
		if(strcmp(s, helptypes[i].name) == 0) {
			helptypes[i].handler = handler;
			helptypes[i].shut    = shut;
			wl_free(s);
			return;
		}
	}

	helptypes = wl_realloc(helptypes, (n_helptypes + 1)*sizeof(HELPENT));
	helptypes[n_helptypes].name    = s;
	helptypes[n_helptypes].handler = handler;
	helptypes[n_helptypes].shut    = shut;
	helptypes[n_helptypes].dying   = FALSE;
	++n_helptypes;
}

void
wlp_help_del(char *name)
{
	int   i;
	char *s;

	s = wl_lstrdup(name);
	for(i = 0; i < n_helptypes; ++i)
		if(strcmp(s, helptypes[i].name) != 0)
			continue;
	if(i == n_helptypes
	|| helptypes[i].dying)
		return;

	help_clear(&helptypes[i]);
	if(i < n_helptypes - 1)
		memmove((void *)&helptypes[i], (void *)&helptypes[i + 1],
					(n_helptypes - i)*sizeof(HELPENT));
	--n_helptypes;
	helptypes = wl_realloc(helptypes, n_helptypes*sizeof(HELPENT));
}

int
wl_help_load(Widget w, char *chapter, char *section)
{
	XtResource r = {
		XwlNhelpType, XwlCHelpType,
		XwlRHelpType, sizeof(WLP_HELPFN), XtOffsetOf(HELP, handler),
		XtRCallProc, (XtPointer)help_default
	};
	HELP     help;
	WLP_HELP h;

	if(chapter == NULL)
		return FALSE;

	memset((void *)&help, '\0', sizeof(help));
	XtGetApplicationResources(w, &help, &r, 1, NULL, 0);
	if(help.handler == NULL)
		return FALSE;
	h.chapter = chapter;
	h.section = section;

	return help.handler(w, &h);
}

/*
 * standard help callback
 */
void
wl_help(Widget w, XtPointer closure, XtPointer call)
{
	XtResource r[] = {
		{
			XwlNhelpChapter, XwlCHelpChapter,
			XtRString, sizeof(String), XtOffsetOf(HELP, chapter),
			XtRString, NULL
		}, {
			XwlNhelpSection, XwlCHelpSection,
			XtRString, sizeof(String), XtOffsetOf(HELP, section),
			XtRString, NULL
		}
	};
	HELP help;

	USEUP(call); USEUP(closure);

	for(; w != NULL; w = XtParent(w))
		if(XtHasCallbacks(w, XmNhelpCallback) == XtCallbackHasSome)
			break;
	if(w == NULL)
		return;

	memset((void *)&help, '\0', sizeof(help));
	XtGetApplicationResources(w, &help, &r[0], XtNumber(r), NULL, 0);
	if(help.chapter == NULL)
		return;

	wl_help_load(w, help.chapter, help.section);
}

void
wl_help_context(Widget w, XtPointer closure, XtPointer call)
{
	XmAnyCallbackStruct cbs;
	Cursor qa;
	XEvent ev;
	Widget top;

	USEUP(closure); USEUP(call);

	top = wl_top(w);
	memset((void *)&ev,  '\0', sizeof(ev));
	qa = XCreateFontCursor(wl_display(top), XC_question_arrow);
	w  = XmTrackingEvent(top, qa, False, &ev);
	while(w != NULL) {
		if(XtHasCallbacks(w, XmNhelpCallback) != XtCallbackHasSome) {
			w = XtParent(w);
			continue;
		}
		memset((void *)&cbs, '\0', sizeof(cbs));
		cbs.reason = XmCR_HELP;
		cbs.event  = &ev;
		XtCallCallbacks(w, XmNhelpCallback, (XtPointer)&cbs);
		w = NULL;
	}

	/*
	 * XmTrackingEvent() will take care of resetting the cursor
	 */
	XFreeCursor(wl_display(top), qa);
}

char *
wlp_help_browser(Widget w)
{
static	char *browser = NULL;

	if(browser == NULL)
		browser = wl_res_string(wl_top(w), XwlNhelpBrowser,
							XwlCHelpBrowser, "");
	return browser;
}

char *
wlp_help_prefix(Widget w)
{
static	char *help_prefix = NULL;

	if(help_prefix != NULL)
		return help_prefix;

	help_prefix = wl_res_string(wl_top(w), XwlNhelpPrefix,
							XwlCHelpPrefix, NULL);

	/*
	 * If the prefix is local to the file system, then we'll use it
	 * if it exists.
	 */
	if(strncmp(help_prefix, "file://", sizeof("file://") - 1) == 0) {
		struct stat st;
		char *dir = help_prefix + sizeof("file://") - 1;

		if(stat(dir, &st) != 0
		|| !S_ISDIR(st.st_mode))
			help_prefix = NULL;
	}
	else if(*help_prefix == '\0') {
		help_prefix = NULL;
	}

	return help_prefix;
}

char *
wlp_help_url(Widget w, char *chapter, char *section)
{
	char *prefix = wlp_help_prefix(w);
	char *url;
	size_t length;

	/*
	 * "<prefix><chapter>.html"
	 */
	length = strlen(prefix) + strlen(chapter) + 5 + 1;

	if(section == NULL) {
		url = wl_malloc(length);
		sprintf(url, "%s%s.html", prefix, chapter);
	}
	else {
		length += strlen(section) + 1;
		url     = wl_malloc(length);
		sprintf(url, "%s%s.html#%s", prefix, chapter, section);
	}

	return url;
}
