/*
 * xtoolwait.c
 *	hacked up and stripped down version of xtoolwait 1.2
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
 *		Richard Huveneers (richard@hekkihek.hacom.nl)
 *
 * Xtoolwait has the following copyright:
 *
 *	Copyright (C) 1995  Richard Huveneers <richard@hekkihek.hacom.nl>
 *
 * and falls under the GPL.
 */
#include <wlib/rcs.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/Xmd.h>

#include <wlib/wlibP.h>

static void
child_death(int x)
{
	wait(&x);
}

static int
is_mapped(Display *dpy, Window window, Atom wm_state)
{
	Atom           type;
	int            format, status;
	unsigned long  n_items, bytes_after;
	unsigned char *prop;
	CARD32         state;

	status = XGetWindowProperty(dpy, window, wm_state, 0L, 1L, False,
			wm_state, &type, &format, &n_items,
			&bytes_after, &prop);
	if(status  != Success
	|| type    != wm_state
	|| n_items != 1
	|| format  != 32)
		return FALSE;

	state = *((CARD32 *)prop);
	return state == NormalState || state == IconicState;
}

static void
sig(int s, void (*handler)(int), struct sigaction *osa)
{
	struct sigaction sa;

	memset((void *)&sa, '\0', sizeof(sa));
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = handler;
	sigaction(s, &sa, osa);
}

Window
wlp_launch(char *display, char **argv, pid_t *pid)
{
	struct sigaction sig_child;
	Display *dpy = NULL;
	Window   wid = None;
	XEvent   ev;
	Atom     wm_state;
	pid_t    kid;

	/*
	 * We want our own connection to the X server to avoid mucking
	 * with Xt's event stream, damn I really love X sometimes.
	 */
	if((dpy = XOpenDisplay(display)) == NULL)
		return None;

	/*
	 * Make sure that the file descriptor is not passed to the client.
	 */
	if(fcntl(ConnectionNumber(dpy), F_SETFD, 1L) == -1)
		return None;

	/*
	 * We assume that the client is not smart enough to locate the virtual
	 * root window, if there is one, so it will create its main window on
	 * the default root window.
	 */
	XSelectInput(dpy, DefaultRootWindow(dpy), SubstructureNotifyMask);

	/*
	 * We assume that the window manager provides the WM_STATE property
	 * on top-level windows, as required by ICCCM 2.0.  If the window
	 * manager has not yet completed its initialisation, the WM_STATE
	 * atom might not exist, in which case we create it; this situation
	 * is highly unlikely in our case but...
	 */
	wm_state = XInternAtom(dpy, "WM_STATE", False);

	sig(SIGCHLD, child_death, &sig_child);

	kid = fork();
	switch(kid) {
	case -1:
		goto Hell;
		break;
	case 0:
		execvp(argv[0], argv);
		fprintf(stderr, "wlp_launch(%s): %s\n", argv[0],
							strerror(errno));
		exit(EXIT_FAILURE);
	default:
		if(pid != NULL)
			*pid = kid;
		break;
	}

	while(wid == None) {
		XNextEvent(dpy, &ev);
		switch(ev.type) {
		case CreateNotify:
			if(ev.xcreatewindow.send_event
			|| ev.xcreatewindow.override_redirect)
				break;
			XSelectInput(dpy, ev.xcreatewindow.window,
							PropertyChangeMask);
			break;
		case PropertyNotify:
			if(ev.xproperty.send_event
			|| ev.xproperty.atom != wm_state
			|| !is_mapped(dpy, ev.xproperty.window, wm_state))
				break;
			wid = ev.xproperty.window;
			break;
		default:
			break;
		}
	}

Hell:
	sigaction(SIGCHLD, &sig_child, NULL);

	if(dpy != NULL)
		XCloseDisplay(dpy);

	return wid;
}
