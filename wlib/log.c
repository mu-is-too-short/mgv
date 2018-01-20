/*
 * log.c
 *	Log dialogs.  This file is part of wlib.
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
MU_ID("$Mu: mgv/wlib/log.c 1.4 2001/03/19 19:31:41 $")

#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <Xm/Text.h>
#include <Xm/FileSB.h>

#include <wlib/stdlib.h>
#include <wlib/wlib.h>
#include <wlib/text.h>
#include <wlib/msg.h>
#include <wlib/callbacks.h>
#include <wlib/help.h>
#include <wlib/wlibP.h>
#include <wlib/typesanity.h>
#include <wlib/log.h>
#include <wlib/sanity.h>
#include <wlib/strings.h>

/*
 * user data for the log
 */
#define	LOG_MAGIC1	0x4c4f4700	/* 'L' 'O' 'G' */
#define	LOG_MAGIC2	0x00474f4c	/* 'G' 'O' 'L' */
struct WLLOG_s {
	long   magic1;
	Widget shell;
	Widget form;
	Widget text;
	Widget fs;
	Widget save;
	int    hidden;
	long   magic2;
};

static int
log_ok(WLLOG lg)
{
	DIEIF(lg         == NULL);
	DIEIF(lg->magic1 != LOG_MAGIC1);
	DIEIF(lg->magic2 != LOG_MAGIC2);
	DIEIF(lg->shell  == NULL);
	DIEIF(lg->form   == NULL);
	DIEIF(lg->text   == NULL);
	DIEIF(lg->save   == NULL);
	return TRUE;
}

/*
 * log->clear
 */
static void
clear(Widget w, XtPointer closure, XtPointer call)
{
	WLLOG lg = (WLLOG)closure;

	USEUP(w); USEUP(call);
	wl_assert(log_ok(lg));

	XmTextSetString(lg->text, "");
	XmTextSetCursorPosition(lg->text, (XmTextPosition)0);
	XmTextSetInsertionPosition(lg->text, (XmTextPosition)0);
	XmTextShowPosition(lg->text, (XmTextPosition)0);

	XtSetSensitive(lg->save, False);
}

/*
 * log->hide
 */
static void
hide(Widget w, XtPointer closure, XtPointer call)
{
	WLLOG lg = (WLLOG)closure;

	USEUP(w); USEUP(call);
	wl_assert(log_ok(lg));

	lg->hidden = TRUE;
	XtPopdown(lg->shell);
}

/*
 * log->fs->ok
 */
static void
save_ok(Widget w, XtPointer closure, XtPointer call)
{
	XmFSS *cbs = (XmFSS *)call;
	WLLOG  lg  = (WLLOG)closure;
	char  *file, *value;
	FILE  *fp;
	wl_assert(log_ok(lg));

	file = NULL;

	if(!XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &file)) {
		wl_error(w, "%s: %s", XtName(w), wl_s(w, WlpScantGet));
		return;
	}

	value = XmTextGetString(lg->text);

	if((fp = fopen(file, "w")) == NULL
	|| fwrite(value, 1, strlen(value), fp) != strlen(value)) {
		wl_error(w, "%s: %s", file, strerror(errno));
		unlink(file);
	}
	else {
		XtUnmanageChild(w);
		XtSetSensitive(lg->save, False);
	}

	if(fp != NULL)
		fclose(fp);
	wl_free((XtPointer)value);
}

static WLW log_save[] = {
	{"logSave", wlFileSBDialog, 0, NULL},
	{NULL,      NULL,           0, NULL}
};
static WLC cbs[] = {
	{"*logSave", XmNcancelCallback, NULL, wl_dismiss},
	{"*logSave", XmNokCallback,     NULL, save_ok},
	{NULL,       NULL,              NULL, NULL}
};

/*
 * log->save
 */
static void
save(Widget w, XtPointer closure, XtPointer call)
{
	WLLOG lg = (WLLOG)closure;

	USEUP(call);
	wl_assert(log_ok(lg));

	if(lg->fs == NULL) {
		if((lg->fs = wl_create(XtParent(w), &log_save[0])) == NULL)
			return;
		wl_callback(XtParent(lg->fs), &cbs[0], (XtPointer)lg);
	}
	else {
		XmFileSelectionDoSearch(lg->fs, NULL);
	}
	XtManageChild(lg->fs);
	XtPopup(XtParent(lg->fs), XtGrabNone);
}

static void
log_destroy(Widget w, XtPointer closure, XtPointer call)
{
	WLLOG lg = (WLLOG)closure;

	USEUP(w); USEUP(call);
	wl_assert(log_ok(lg));

	wl_free((void *)lg);
}

static void
changed(Widget w, XtPointer closure, XtPointer call)
{
	WLLOG lg = (WLLOG)closure;

	USEUP(w); USEUP(call);
	wl_assert(log_ok(lg));

	if(wl_text_isempty(lg->text))
		XtSetSensitive(lg->save, False);
	else
		XtSetSensitive(lg->save, True);
}

/*
 * the log dialog itself
 */
static WLW log_form[] = {
	{"hide",  wlPushB,        0,         NULL},
	{"save",  wlPushB,        0,         NULL},
	{"clear", wlPushB,        0,         NULL},
	{"help",  wlPushB,        0,         NULL},
	{"sep",   wlSeparator,    0,         NULL},
	{"text",  wlScrolledText, WL_MANAGE, NULL},
	{NULL,    NULL,           0,         NULL}
};
static WLW log_dlg[] = {
	{"log", wlForm, WL_MANAGE, log_form},
	{NULL,  NULL,   0,         NULL}
};
static WLC log_cb[] = {
	{"*hide",  XmNactivateCallback,     NULL, hide},
	{"*save",  XmNactivateCallback,     NULL, save},
	{"*clear", XmNactivateCallback,     NULL, clear},
	{"*help",  XmNactivateCallback,     NULL, wl_help},
	{"*log",   XmNdestroyCallback,      NULL, log_destroy},
	{"*text",  XmNvalueChangedCallback, NULL, changed},
	{NULL,     NULL,                    NULL, NULL}
};

/*
 * create a log
 */
WLLOG
wl_log_open(Widget shell)
{
	WLLOG    lg;
	Widget   hide;
	Arg      a[2];
	Cardinal n;

	if((lg = wl_calloc(1, sizeof(struct WLLOG_s))) == NULL)
		return NULL;
	lg->magic1 = LOG_MAGIC1;
	lg->magic2 = LOG_MAGIC2;
	lg->shell  = shell;

	if((lg->form = wl_create(shell, &log_dlg[0])) == NULL)
		return wl_free((void *)lg);
	wl_callback(XtParent(lg->form), &log_cb[0], (XtPointer)lg);
	wl_find(lg->form,
		&hide,     "*hide",
		&lg->text, "*textSW*text",
		&lg->save, "*save",
		NULL);

	n = 0;
	XtSetArg(a[n], XmNdefaultButton, hide);	++n;
	XtSetArg(a[n], XmNcancelButton,  hide);	++n;
	wlSetValues(lg->form, a, n);

	XtSetSensitive(lg->save, False);

	wl_assert(log_ok(lg));
	return lg;
}

WLLOG
wl_log_close(WLLOG lg)
{
	if(lg == NULL)
		return NULL;
	wl_assert(log_ok(lg));
	XtDestroyWidget(lg->shell);
	return NULL;
}

/*
 * make log appear or disappear
 */
void
wl_log_show(WLLOG lg, int show)
{
	wl_assert(log_ok(lg));

	lg->hidden = !show;
	if(show) {
		XtManageChild(lg->form);
		XtPopup(lg->shell, XtGrabNone);
	}
	else {
		XtPopdown(lg->shell);
	}
}

/*
 * make an entry in a log
 */
void
wl_log(WLLOG lg, char *fmt, ...)
{
	va_list	ap;

	if(fmt == NULL)
		return;

	va_start(ap, fmt);
	wl_vlog(lg, fmt, ap);
	va_end(ap);
}

/*
 * make an entry in a log
 */
void
wl_vlog(WLLOG lg, char *fmt, va_list ap)
{
	char *s1, *s2;
	char  msg[2048];
	int   n;
	wl_assert(log_ok(lg));

	if(fmt == NULL)
		return;

	vsprintf(&msg[0], fmt, ap);

	s1 = XmTextGetString(lg->text);
	s2 = wl_realloc(s1, strlen(s1) + strlen(msg) + 1);
	if(s2 != NULL)
		strcat(s1 = s2, msg);
	XmTextSetString(lg->text, s1);
	XmTextSetCursorPosition(lg->text, (XmTextPosition)(n = strlen(s1)));
	XmTextSetInsertionPosition(lg->text, (XmTextPosition)n);
	XmTextShowPosition(lg->text, (XmTextPosition)n);
	wl_free((XtPointer)s1);

	if(lg->hidden)
		return;

	XtManageChild(lg->form);
	XtPopup(XtParent(lg->form), XtGrabNone);
}
