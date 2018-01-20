/*
 * log.c
 *	Error and message logging functions.
 *	Most of this is just a preprocessor of sorts for the log things
 *	in app.c.
 *
 * Copyright (C) 1996  Eric A. Howe
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
MU_ID("$Mu: mgv/log.c 1.76 1998/11/11 05:59:52 $")

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <wlib/wlib.h>
#include <wlib/sanity.h>
#include <mine/mgv.h>
#include <mine/app.h>
#include <mine/log.h>
#include <mine/sens.h>
#include <mine/strings.h>

/*
 * this gets errors (GvNoutputCallback)
 */
void
mgv_log_output(Widget w, XtPointer closure, XtPointer call)
{
	MGV *m = (MGV *)closure;

	USEUP(w);
	wl_assert(MgvZOK(m));

	mgv_log_show(True);
	mgv_log("%s: %s", m->file, (char *)call);
}

/*
 * the ghostviewWidgetClass GvNmessageCallback
 */
void
mgv_log_message(Widget w, XtPointer closure, XtPointer call)
{
	char *msg = (char *)call, buf[1024];
	MGV  *m   = (MGV *)closure;
	char *type, *err;
	int   error;

	wl_assert(MgvZOK(m));
	m = mgv_top(m);

	/*
	 * I suppose we could use quarks here but there really
	 * isn't any point since this doesn't get hit very much.
	 */
	error = FALSE;
	if(strcmp(msg, "Refresh") == 0) {
		mgv_refresh(m);
		return;
	}
	else if(strcmp(msg, "Page") == 0) {
		type = MgvSmessage;
		err  = MgvSpageFinished;
		m->pagedone = TRUE;
	}
	else if(strcmp(msg, "Done") == 0) {
		/*
		 * this means that we are out of pages
		 */
		type = MgvSmessage;
		err  = MgvSinterpDone;
		m->sens &= ~MgvSNOTLAST;
		wl_sens(m->senscache, m->sens);
	}
	else if(strcmp(msg, "Failed") == 0) {
		type  = MgvSerror;
		err   = MgvSinterpFailed;
		error = TRUE;
	}
	else if(strcmp(msg, "BadAlloc") == 0) {
		type  = MgvSerror;
		err   = MgvScantAllocPixmap;
		error = TRUE;
	}
	else {
		type  = MgvSerror;
		err   = MgvSunknown;
		error = TRUE;
	}
	sprintf(&buf[0], "%s: %s: %s\n", wl_s(w, type), msg, wl_s(w, err));
	if(error)
		mgv_log_show(TRUE);
	mgv_log("%s: %s", m->file, buf);
}
