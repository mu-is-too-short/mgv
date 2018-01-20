/*
 * init.c
 *	Application initialization functions.  This file is part of wlib.
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
MU_ID("$Mu: mgv/wlib/init.c 1.1 2001/03/09 04:16:27 $")

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <Xm/RepType.h>

#include <wlib/stdlib.h>
#include <wlib/wlib.h>
#include <wlib/help.h>
#include <wlib/helpP.h>
#include <wlib/wlibP.h>
#include <wlib/resource.h>
#include <wlib/appP.h>

static void
help_action(Widget w, XEvent *ev, String *argv, Cardinal *argc)
{
	XmAnyCallbackStruct cbs;

	USEUP(argv); USEUP(argc);

	memset((void *)&cbs, '\0', sizeof(cbs));
	cbs.reason = XmCR_HELP;
	cbs.event  = ev;
	wl_help(w, NULL, &cbs);
}

static Boolean
string2percent(Display *dpy, XrmValue *a, Cardinal *n, XrmValue *from,
						XrmValue *to, XtPointer *data)
{
	int    alloced = FALSE;
	double value;
	char  *p;

	USEUP(data); USEUP(a);

	if(*n != 0) {
		XtWarningMsg("wrongParameters", "cvtStringToPercentage",
				"XtToolkitError",
				"String to Percentage needs no extra arguments",
				NULL, NULL);
	}
	if(from->addr == NULL)
		return False;
	if(to->addr != NULL && to->size < sizeof(int)) {
		to->size = sizeof(int);
		return False;
	}
	if(to->addr == NULL) {
		to->addr = wl_malloc(sizeof(int));
		to->size = sizeof(int);
		alloced  = TRUE;
	}

	/*
	 * Convert and check error conditions.
	 * We use an int to store the result since pushing floats
	 * and doubles through the resources is kinda' brutal on the
	 * Alpha (and possibly the UltraSPARC); hence, the scaling
	 * factor of 1e3 instead of 1e2.
	 */
	value = strtod((char *)from->addr, &p);
	if(value !=  HUGE_VAL
	&& value != -HUGE_VAL
	&& p     !=  from->addr) {
		if(*p == '%')
			value /= 100.0;
		*(int *)(to->addr) = (int)(value * 1000.0);
		return True;
	}

	if(alloced)
		to->addr = wl_free(to->addr);
	XtDisplayStringConversionWarning(dpy,(char *)from->addr,XwlRPercentage);
	return False;
}

static char *
percent2string(char *p)
{
	char buf[1024];
	double value = *(int *)p / 10.0;
	sprintf(buf, "%g%%", value);
	return XtNewString(buf);
}

static XtActionsRec actions[] = {
	{"wlHelp", help_action},
};

WLAPP
wl_init(int *argc, char **argv, String cls, XrmOptionDescList opt,
						Cardinal nopt, char **fallbacks)
{
	Display	    *dpy;
	XrmDatabase  db;
	WLAPP        app;
	XtAppContext ac;

	/*
	 * Start help things now, we want to make sure it is all set
	 * up before we start fiddling with the resource database.
	 */
	wlp_help_init();

	/*
	 * On the version of Sinix that I had access to (5.42)
	 * tear off menus caused a core dump when they were torn
	 * off (this even happens with the Motif sample programs)
	 * but we'll install the type converter anyway:
	 *
	 *	DO NOT SHOOT YOURSELF IN THE HEAD, it hurts.
	 *
	 * I could use #ifdef's (ack! ack! ptooi!) but that's just gross,
	 * I could also attempt some uname(2) magic but that's too fragile
	 * so I'll just assume a certain intelligence level in the users
	 * (dangerous perhaps but if you assume that someone is an idiot,
	 * they will often rise to your expectations).
	 */
	XtSetLanguageProc(NULL, NULL, NULL);
	XmRepTypeInstallTearOffModelConverter();
	XtToolkitInitialize();
	XtSetTypeConverter(XmRString, XwlRPercentage, string2percent, NULL, 0,
							XtCacheNone, NULL);
	wl_res_addfmt(XwlRPercentage, percent2string);
	if((ac = XtCreateApplicationContext()) == NULL) {
		fprintf(stderr, "wl_init(): could not create AppContext\n");
		return NULL;
	}
	XtAppAddActions(ac, actions, XtNumber(actions)); 

	if(fallbacks != NULL)
		XtAppSetFallbackResources(ac, fallbacks);

	/*
	 * We want to parse the command-line after we have a display
	 * so we need some tricks here.  There is a problem:
	 * XrmParseCommand doesn't seem to know about the standard toolkit
	 * switches (such as -xrm); XtOpenDisplay seems to be calling
	 * XrmParseCommand twice:  once with the application resource list
	 * and again with an Xt internal resource list.  This isn't a really
	 * big problem though, the XrmOptionKind enum provides the
	 * XrmoptionResArg option type which appears to exist just to make
	 * -xrm function.  The obvious hack here is to pass a NULL application
	 * switch list to XtOpenDisplay and let it use its internal list
	 * and then call XrmParseCommand when we're ready.  I should have
	 * thought of this right away but I didn't notice a problem until
	 * -xrm stopped working.
	 */
	dpy = XtOpenDisplay(ac, NULL, NULL, cls, NULL, 0, argc, argv);
	if(dpy == NULL) {
		fprintf(stderr, "wl_init(): could not open display.\n");
		return NULL;
	}

	if((app = wl_app_create(dpy, cls)) == NULL)
		return NULL;

	/*
	 * XtOpenDisplay will take care of the current Display database,
	 * the application defaults file, the fallbacks, and the standard
	 * toolkit switches.  After all of that is out of the way, we want to
	 * load the rc files--user settings and then the settings from the
	 * last execution--and then deal with the application switches on the
	 * command line.
	 *
	 * Also, XrmMergeDatabases will destroy the source database
	 * so we don't have to worry about destroying what `wl_res_load'
	 * gives us (wl_res_load never returns NULL BTW, maybe an empty
	 * database but never NULL).
	 */
	db = XtDatabase(dpy);
	XrmMergeDatabases(wl_res_load(app->user), &db);
	XrmMergeDatabases(wl_res_load(app->rc),   &db);
	XrmParseCommand(&db, opt, nopt, cls, argc, argv);

	return app;
}
