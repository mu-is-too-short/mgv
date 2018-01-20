/*
 * help-remote.c
 *	Help support for netscape's -remote protocol.
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
MU_ID("$Mu: mgv/wlib/help-remote.c 1.1 2001/03/09 04:16:27 $")

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <X11/Xlib.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>

#include <wlib/stdlib.h>
#include <wlib/wlib.h>
#include <wlib/strings.h>
#include <wlib/msg.h>
#include <wlib/wlibP.h>
#include <wlib/sanity.h>
#include <wlib/helpP.h>
#include <wlib/display.h>

/*
 * X error handler for catching BadWindow
 */
static int           badwindow      = FALSE;
static XErrorHandler default_xerror = NULL;
static int
badwin_catcher(Display *dpy, XErrorEvent *ev)
{
	badwindow = (ev->error_code == BadWindow);
	return badwindow ? 0 : default_xerror(dpy, ev);
}

/*
 * XmuClientWindow replacement.  Many people don't have Xmu
 * for some reason.
 */
static Window
recurse(Display *dpy, Window win, Atom wm_state)
{
	Atom           actual_type = None;
	int            format;
	unsigned long  n_items, after;
	unsigned char *prop;
	Window         root, found, parent;
	Window        *kids;
	unsigned int   n_kids, i;

	if(!XQueryTree(dpy, win, &root, &parent, &kids, &n_kids))
		return None;
	for(i = 0, found = None; found == None && i < n_kids; ++i) {
		XGetWindowProperty(dpy, kids[i], wm_state, 0, 0, False,
				AnyPropertyType, &actual_type, &format,
				&n_items, &after, &prop);
		if(actual_type != None)
			found = kids[i];
	}

	for(i = 0; found == None && i < n_kids; ++i)
		found = recurse(dpy, kids[i], wm_state);

	if(kids != NULL)
		XFree((void *)kids);
	return found;
}

static Window
client_window(Display *dpy, Window win)
{
	Atom           wm_state, actual_type;
	int            format;
	unsigned long  n_items, after;
	unsigned char *prop;
	Window         kid;

	wm_state = actual_type = None;

	if((wm_state = XInternAtom(dpy, "WM_STATE", True)) == None)
		return win;
	XGetWindowProperty(dpy, win, wm_state, 0, 0, False, AnyPropertyType,
			&actual_type, &format, &n_items, &after, &prop);
	if(actual_type != None)
		return win;
	return (kid = recurse(dpy, win, wm_state)) == None ? win : kid;
}

static Window
winping(Display *dpy, Window win, char *name, char *classname)
{
	XClassHint hints = {NULL, NULL};
	int        status;

	if(win == None)
		return None;

	/*
	 * we have to watch for BadWindow errors in here
	 */
	XSync(dpy, False);
	default_xerror = XSetErrorHandler(badwin_catcher);
	win = client_window(dpy, win);
	XSync(dpy, False);
	if(win == None || badwindow) {
		win = None;
		goto Hell;
	}
	status = XGetClassHint(dpy, win, &hints);
	XSync(dpy, False);
	if(status == 0 || badwindow) {
		win = None;
		goto Hell;
	}
	if(strcmp(name, hints.res_name)       != 0
	|| strcmp(classname, hints.res_class) != 0)
		win = None;
	XFree(hints.res_name);
	XFree(hints.res_class);
Hell:
	XSetErrorHandler(default_xerror);
	badwindow = FALSE;
	return win;
}

static Window
winfind(Display *dpy, char *name, char *cls)
{
	Window   root, win, *kids;
	unsigned n_kids, i;

	root = RootWindow(dpy, DefaultScreen(dpy));
	if(XQueryTree(dpy, root, &win, &win, &kids, &n_kids) == 0)
		return None;
	for(i = 0, win = None; win == None && i < n_kids; ++i)
		win = winping(dpy, kids[i], name, cls);
	XFree(kids);

	return win;
}

static pid_t
launch(char **argv)
{
	pid_t brat;

	brat = fork();
	switch(brat) {
	case 0:
		execvp(*argv, argv);
		fprintf(stderr, "could not execute %s: %s\n",
						*argv, strerror(errno));
		exit(EXIT_FAILURE);
		break;
	case -1:
		break;
	default:
		break;
	}
	return brat;
}

/*
 * HTML help with netscape's -remote thing
 *
 * This is a little tricky.  The first time we make a help window we want to
 * either start netscape ourselves and keep using it or we want to make
 * a new netscape window and keep using that.  The problem is figuring out
 * which window is the new netscape window that 'openURL(...,new-window)'
 * made for us.
 */
static Boolean
remote(Widget w, WLP_HELP *h)
{
#	define	OPENNEWURL	"openURL(%s,new-window)"
#	define	OPENURL		"openURL(%s)"
static	Window	ns = None;
	Window  win;
	Display	*dpy;
	char	*display, *url, *argv[8], id[64];
	char    *act = NULL;
	int	n, wait_for_it = TRUE;
	pid_t	brat;

	dpy = wl_display(w);
	n = 0;
	argv[n++] = wlp_help_browser(w);
	if((display = wl_display_string(w)) != NULL) {
		argv[n++] = "-display";
		argv[n++] = display;
	}

	url = wlp_help_url(w, h->chapter, h->section);
	if(ns != None
	&& winping(dpy, ns, "Navigator", "Netscape") == ns) {
		/*
		 * Cool, we can still use our old window (or so we think
		 * as we wallow in monkey arrogance).
		 */
		sprintf(id, "%ld", (long)ns);
		act = wl_calloc(strlen(url) + sizeof(OPENURL), 1);
		sprintf(act, OPENURL, url);
		argv[n++] = "-id";
		argv[n++] = &id[0];
		argv[n++] = "-remote";
		argv[n++] = act;
	}
	else if((win = winfind(dpy, "Navigator", "Netscape")) != None) {
		/*
		 * Okay, netscape is up and running so we need to get
		 * our fine selves a new window to do with what we will.
		 */
		ns = None;
		sprintf(id, "%ld", (long)win);
		act = wl_calloc(strlen(url) + sizeof(OPENNEWURL), 1);
		sprintf(act, OPENNEWURL, url);
		argv[n++] = "-id";
		argv[n++] = &id[0];
		argv[n++] = "-remote";
		argv[n++] = act;
	}
	else {
		/*
		 * Launch netscape ourselves.
		 * This is the only time we don't want to wait for the
		 * child process, the other two branches just make a temporary
		 * process (via "netscape -remote") but this branch will
		 * make a "real" process.
		 */
		ns = None;
		wait_for_it = FALSE;
		argv[n++] = url;
	}
	argv[n++] = NULL;
	ARGSANITY(argv, n);

	/*
	 * We don't want to use wlp_launch unless we're actually going to
	 * make a window, if we make that mistake we'll just sit and spin
	 * for ever because wlp_launch will never return, oh well, "Bob"
	 * will tilt the luck plane our way.
	 */
	if(ns == None)
		ns = wlp_launch(display, argv, &brat);
	else
		brat = launch(argv);
	if(wait_for_it)
		waitpid(brat, NULL, 0);

	wl_free(url);
	wl_free(act);

	return True;
#	undef	OPENURL
#	undef	OPENNEWURL
}

void
wlp_help_remote_init(void)
{
	wlp_help_add("html-remote", remote, NULL);
	wlp_help_add("netscape",    remote, NULL);
	wlp_help_add("remote",      remote, NULL);
}
