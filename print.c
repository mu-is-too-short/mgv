/*
 * print.c
 *	Printing support functions for mgv.
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
MU_ID("$Mu: mgv/print.c 1.105 2001/03/11 18:18:05 $")

#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <Xm/MessageB.h>
#include <Xm/ToggleB.h>

#include <wlib/stdlib.h>
#include <wlib/wlib.h>
#include <wlib/sanity.h>
#include <wlib/callbacks.h>
#include <wlib/help.h>
#include <wlib/util.h>
#include <wlib/process.h>
#include <mine/mgv.h>
#include <mine/app.h>
#include <mine/print.h>
#include <mine/log.h>
#include <mine/sens.h>
#include <mine/strings.h>
#include <mine/uiutil.h>
#include <mine/error.h>

/*
 * the command section is a little form so that we can get
 * the label and the text field to align in a pleasing fashion
 * (I could use the caption widget from Xbae but then everyone
 * would need to have the Xbae widgets kicking around)
 */
static WLW cmd_form[] = {
	{"cmd",     wlLabel,     0, NULL},
	{"cmdText", wlTextField, 0, NULL},
	{NULL,      NULL,        0, NULL}
};

/*
 * the widgets for the print dialog
 */
static WLW print_form[] = {
	{"cmdForm",    wlForm,      WL_MANAGE, cmd_form},
	{"markedOnly", wlToggleB,   0,         NULL},
	{"unmark",     wlToggleB,   0,         NULL},
	{"apply",      wlPushB,     0,         NULL},
	{"dismiss",    wlPushB,     0,         NULL},
	{"help",       wlPushB,     0,         NULL},
	{"sep",        wlSeparator, 0,         NULL},
	{NULL,         NULL,        0,         NULL}
};

/*
 * the print dialog itself
 */
static WLW printdlg[] = {
	{"printDialog", wlFormDialog, 0, print_form},
	{NULL,         NULL,         0,         NULL}
};

/*
 * Popup a message to show the status, if any, of the print job.
 */
static void
print_out(XtPointer closure, int *fd, XtInputId *id)
{
	MGV *m = (MGV *)closure;

	wl_assert(MgvOK(m));

	if(m->child_out == NULL)
		m->child_out = XmCreateInformationDialog(m->main, "childStdout",
								NULL, 0);
	if(!wl_child_msg(m->child_out, *fd)) {
		XtRemoveInput(*id);
		close(*fd);
	}
}

/*
 * Popup a message to show the errors, if any, of the print job.
 */
static void
print_err(XtPointer closure, int *fd, XtInputId *id)
{
	MGV *m = (MGV *)closure;

	wl_assert(MgvOK(m));

	if(m->child_err == NULL)
		m->child_err = XmCreateErrorDialog(m->main, "childStderr",
								NULL, 0);
	if(!wl_child_msg(m->child_err, *fd)) {
		XtRemoveInput(*id);
		close(*fd);
	}
}


/*
 * print->apply
 */
static void
print_apply(Widget w, XtPointer closure, XtPointer call)
{
	struct sigaction sa, osa;
	XtInputId out_id, err_id;
	MGV    *m = (MGV *)closure;
	MGVR   *r;
	FILE   *fp;
	String  cmd;
	Boolean marked, unmark;
	int     i, status;
	WLPIPE  pk;

	USEUP(call);
	wl_assert(MgvOK(m));

	r = mgv_app_res(m->main);

	cmd     = NULL;
	marked  =
	unmark  = False;
	XtVaGetValues(wl_find1(m->printdlg, "*cmdText"),
		XmNvalue,	&cmd,
		NULL);

	/*
	 * sanity checks
	 */
	if(cmd == NULL || *cmd == '\0') {
		mgv_error(m->printdlg, "%s", wl_s(w, MgvSnoPrintCommand));
		return;
	}

	/*
	 * check the toggles (both toggles should always have the
	 * same sensitivity)
	 */
	w = wl_find1(m->printdlg, "*markedOnly");
	if(XtIsSensitive(w)) {
		marked = XmToggleButtonGetState(w);
		w      = wl_find1(m->printdlg, "*unmark");
		unmark = marked ? XmToggleButtonGetState(w) : False;
	}

	if(strcmp(r->printcmd, cmd) != 0) {
		wl_free(r->printcmd);
		r->printcmd  = XtNewString(cmd);
		wl_free(*r->printargv);
		wl_free(r->printargv);
		r->printargv = wl_stoargv(cmd);
	}

	memset((void *)&pk,  '\0', sizeof(pk));
	memset((void *)&sa,  '\0', sizeof(sa));
	memset((void *)&osa, '\0', sizeof(osa));
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGPIPE, &sa, &osa);
	pk.flags = WL_PALL;
	if((status = wl_popen(&pk, *r->printargv, r->printargv)) != 0) {
		mgv_error(m->printdlg, "%s %s: %s", wl_s(w, MgvScantPrint),
						r->printcmd, strerror(status));
		sigaction(SIGPIPE, &osa, &sa);
		return;
	}
	out_id = XtAppAddInput(XtWidgetToApplicationContext(m->main),
					pk.out, (XtPointer)XtInputReadMask,
					print_out, (XtPointer)m);
	err_id = XtAppAddInput(XtWidgetToApplicationContext(m->main),
					pk.err, (XtPointer)XtInputReadMask,
					print_err, (XtPointer)m);
	mgv_write(m, fp = fdopen(pk.in, "w"), marked);
	fclose(fp);
	sigaction(SIGPIPE, &osa, &sa);

	if(unmark && m->dsc != NULL) {
		for(i = 0; i < m->dsc->n_pages; ++i)
			mgv_mark(m, i, FALSE, FALSE);
		mgv_list_update(m, -1);
	}
	wl_dlg_popdown(m->printdlg);
}

/*
 * and all the callbacks
 */
static WLC print_cb[] = {
	{"*apply",   XmNactivateCallback, NULL, print_apply},
	{"*dismiss", XmNactivateCallback, NULL, wl_dismiss},
	{"*help",    XmNactivateCallback, NULL, wl_help},
	{NULL,       NULL,                NULL, NULL}
};

/*
 * Print
 */
void
mgv_print(Widget w, XtPointer closure, XtPointer call)
{
	MGV  *m = (MGV *)closure;
	MGVR *r = mgv_app_res(m->main);

	USEUP(w); USEUP(call);
	wl_assert(MgvOK(m));

	if(m->printdlg == NULL) {
		if((m->printdlg = wl_create(m->main, printdlg)) == NULL)
			return;
		wl_callback(m->printdlg, &print_cb[0], (XtPointer)m);
	}

	if(r->printcmd != NULL)
		XtVaSetValues(wl_find1(m->printdlg, "*cmdText"),
			XmNvalue,	r->printcmd,
			NULL);

	wl_sens(m->senscache, m->sens);

	wl_dlg_popup(m->printdlg);
}
