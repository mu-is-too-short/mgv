/*
 * app.c
 *	Top level application resource handling
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
MU_ID("$Mu: mgv/app.c 1.131 2001/03/18 19:37:58 $")

#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <Xm/Xm.h>
#include <Xm/AtomMgr.h>
#include <Xm/Protocols.h>
#include <Xm/ToggleB.h>

#include <wlib/WlAppShell.h>
#include <wlib/stdlib.h>
#include <wlib/wlib.h>
#include <wlib/build.h>
#include <wlib/sanity.h>
#include <wlib/resource.h>
#include <wlib/callbacks.h>
#include <wlib/help.h>
#include <wlib/app.h>
#include <wlib/log.h>
#include <wlib/msg.h>
#include <wlib/util.h>
#include <wlib/display.h>
#include <wlib/icons.h>
#include <mine/mgv.h>
#include <mine/app.h>
#include <mine/strdefs.h>
#include <mine/strings.h>
#include <mine/page.h>
#include <mine/mark.h>
#include <mine/zoom.h>
#include <mine/scroll.h>
#include <mine/log.h>
#include <mine/error.h>

/*
 * Figure out a default printer name.
 *
 * LPDEST is for SYSV/lp based systems and PRINTER is for BSD/lpr
 * based systems but we'll check both.
 */
static String
defprinter(void)
{
	char	*e;

	if((e = getenv("LPDEST"))  == NULL
	&& (e = getenv("PRINTER")) == NULL)
		e = "ps";
	return e;
}

/*
 * probable print commands
 */
static struct {
	char *command;
	char *switches;
} printcmds[] = {
	{"lp",   " -d %s"},
	{"lpr",  " -P%s"},
	{"rlpr", " -P%s"},
	{NULL,   NULL}
};
#define N_CMD	(4 + 7)
#define	N_PRINTCMDS (int)(sizeof(printcmds)/sizeof(printcmds[0]))

/*
 * Default PATH setting.  I find the notion of not having a PATH a little
 * strange but I suppose it is possible.  Since we're only looking for
 * the print spooler command, this list should suffice.
 */
#define	PATH	"/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/sbin"

/*
 * Figure out a default print command.
 */
static void
defprintcmd(Widget w, int offset, XrmValue *v)
{
	char *buf, *s, *p;
	int   i, done;

	USEUP(offset); USEUP(w);

	if((s = getenv("PATH")) == NULL)
		s = PATH;
	p   = XtNewString(s);
	buf = wl_malloc(strlen(s) + N_CMD + 1);

	v->addr = NULL;
	v->size = 0;
	done    = FALSE;
	for(s = strtok(p, ":"); !done && s != NULL; s = strtok(NULL, ":")) {
		for(i = 0; i < N_PRINTCMDS; ++i) {
			sprintf(buf, "%s/%s", s, printcmds[i].command);
			if(access(buf, X_OK) != 0)
				continue;
			strcat(buf, printcmds[i].switches);
			done = TRUE;
			break;
		}
	}

	wl_free(p);
	s = defprinter();
	p = wl_calloc(strlen(buf) + strlen(s) + 1, 1);
	sprintf(p, buf, s);

	v->addr = p;
	v->size = strlen(p);
	wl_free(buf);
}

/*
 * We check for pdftops (from xpdf) before pdf2ps (from gs 5.10) because
 * pdftops is much faster.
 */
static char *pdfconv[] = {
	"pdftops",
	"pdf2ps",
};
#define	N_PDFCONV (int)(sizeof(pdfconv)/sizeof(pdfconv[0]))

static void
defpdfconv(Widget w, int offset, XrmValue *v)
{
	char *buf, *s, *p;
	int   i, done;

	USEUP(offset); USEUP(w);

	if((s = getenv("PATH")) == NULL)
		s = PATH;
	p   = XtNewString(s);
	buf = wl_malloc(strlen(s) + 64);

	v->addr = NULL;
	v->size = 0;
	done    = FALSE;
	for(s = strtok(p, ":"); !done && s != NULL; s = strtok(NULL, ":")) {
		for(i = 0; i < N_PDFCONV; ++i) {
			sprintf(buf, "%s/%s", s, pdfconv[i]);
			if(access(buf, X_OK) != 0)
				continue;
			done = TRUE;
			break;
		}
	}

	/*
	 * We'll turn empty strings to NULLs later on.
	 */
	wl_free(p);
	v->addr = done ? XtNewString(buf) : "";
	v->size = strlen((char *)v->addr);
	wl_free(buf);
}

/*
 * Application resources.  Some of the resources need to have a realized
 * widget to get the correct colormap and what-not so we have to do this
 * in two passes.
 */
#define	OFFSET(f)	XtOffsetOf(MGVR, f)
static XtResource res_pass1[] = {
	{
		MgvNantialiasEPS, MgvCAntialiasEPS,
		XtRBoolean, sizeof(Boolean), OFFSET(antialias_eps),
		XtRImmediate, (XtPointer)True
	}, {
		MgvNautoHeight, MgvCAutoHeight,
		XtRBoolean, sizeof(Boolean), OFFSET(auto_height),
		XtRImmediate, (XtPointer)False
	}, {
		MgvNautoWidth, MgvCAutoWidth,
		XtRBoolean, sizeof(Boolean), OFFSET(auto_width),
		XtRImmediate, (XtPointer)True
	}, {
		MgvNbunzip, MgvCBunzip,
		XtRString, sizeof(String), OFFSET(bunzip),
		XtRString, (XtPointer)"bunzip2"
	}, {
		MgvNbzip, MgvCBzip,
		XtRString, sizeof(String), OFFSET(bzip),
		XtRString, (XtPointer)"bzip2 -9"
	}, {
		MgvNcenter, MgvCCenter,
		XtRBoolean, sizeof(Boolean), OFFSET(center),
		XtRImmediate, (XtPointer)False
	}, {
		MgvNcompress, MgvCCompress,
		XtRString, sizeof(String), OFFSET(compress),
		XtRString, (XtPointer)"compress"
	}, {
		MgvNconfirmExit, MgvCConfirmExit,
		XtRBoolean, sizeof(Boolean), OFFSET(confirm_exit),
		XtRImmediate, (XtPointer)True
	}, {
		MgvNcopyStdin, MgvCCopyStdin,
		XtRBoolean, sizeof(Boolean), OFFSET(copy_stdin),
		XtRImmediate, (XtPointer)True
	}, {
		MgvNdecompress, MgvCDecompress,
		XtRString, sizeof(String), OFFSET(decompress),
		XtRString, (XtPointer)"gzip --stdout --decompress"
	}, {
		MgvNdefaultPageMedia, MgvCDefaultPageMedia,
		MgvRPageMedia, sizeof(int), OFFSET(default_page_media),
		XtRString, (XtPointer)"Letter"
	}, {
		MgvNgunzip, MgvCGunzip,
		XtRString, sizeof(String), OFFSET(gunzip),
		XtRString, (XtPointer)"gzip --stdout --decompress"
	}, {
		MgvNgzip, MgvCGzip,
		XtRString, sizeof(String), OFFSET(gzip),
		XtRString, (XtPointer)"gzip -9"
	}, {
		MgvNhideLog, MgvCHideLog,
		XtRBoolean, sizeof(Boolean), OFFSET(hide_log),
		XtRString, (XtPointer)"True"
	}, {
		MgvNhideMenuBar, MgvCHideMenuBar,
		XtRBoolean, sizeof(Boolean), OFFSET(hide_menu),
		XtRImmediate, (XtPointer)False
	}, {
		MgvNhideStatusLine, MgvCHideStatusLine,
		XtRBoolean, sizeof(Boolean), OFFSET(hide_status),
		XtRImmediate, (XtPointer)False
	}, {
		MgvNhideToolBar, MgvCHideToolBar,
		XtRBoolean, sizeof(Boolean), OFFSET(hide_toolbar),
		XtRImmediate, (XtPointer)False
	}, {
		MgvNhidePageList, MgvCHidePageList,
		XtRBoolean, sizeof(Boolean), OFFSET(hide_pglist),
		XtRImmediate, (XtPointer)False
	}, {
		MgvNkillChildrenAtExit, MgvCKillChildrenAtExit,
		XtRBoolean, sizeof(Boolean), OFFSET(kill_kids),
		XtRImmediate, (XtPointer)True
	}, {
		MgvNmagicResistance, MgvCMagicResistance,
		XtRInt, sizeof(int), OFFSET(magic_resistance),
		XtRImmediate, (XtPointer)2
	}, {
		MgvNmagicScrolling, MgvCMagicScrolling,
		XtRBoolean, sizeof(Boolean), OFFSET(magic_scroll),
		XtRImmediate, (XtPointer)True
	}, {
		MgvNmagstep, MgvCMagstep,
		XtRInt, sizeof(int), OFFSET(magstep),
		XtRImmediate, (XtPointer)0
	}, {
		MgvNpdfConverter, MgvCPdfConverter,
		XtRString, sizeof(String), OFFSET(pdfconv),
		XtRCallProc, (XtPointer)defpdfconv
	}, {
		MgvNprintCommand, MgvCPrintCommand,
		XtRString, sizeof(String), OFFSET(printcmd),
		XtRCallProc, (XtPointer)defprintcmd
	}, {
		MgvNreticuleLineWidth, MgvCReticuleLineWidth,
		XtRInt, sizeof(int), OFFSET(reticule_width),
		XtRImmediate, (XtPointer)2
	}, {
		MgvNresetScrollBars, MgvCResetScrollBars,
		MgvRResetScrollBars, sizeof(int), OFFSET(reset_scroll),
		XtRImmediate, (XtPointer)MgvRESETSCROLL
	}, {
		MgvNscrollPercentage, MgvCScrollPercentage,
		XwlRPercentage, sizeof(int), OFFSET(scroll_percentage),
		XtRImmediate, (XtPointer)90
	}, {
		MgvNshowLabels, MgvCShowLabels,
		XtRBoolean, sizeof(Boolean), OFFSET(show_labels),
		XtRImmediate, (XtPointer)False
	}, {
		MgvNshowPageNumbers, MgvCShowPageNumbers,
		XtRBoolean, sizeof(Boolean), OFFSET(show_pgnums),
		XtRImmediate, (XtPointer)True,
	}, {
		MgvNsmartSizing, MgvCSmartSizing,
		XtRBoolean, sizeof(Boolean), OFFSET(smart_sizing),
		XtRImmediate, (XtPointer)False
	}, {
		MgvNsmoothScrolling, MgvCSmoothScrolling,
		XtRBoolean, sizeof(Boolean), OFFSET(smooth_scroll),
		XtRImmediate, (XtPointer)True
	}, {
		MgvNsmoothness, MgvCSmoothness,
		XtRInt, sizeof(int), OFFSET(smoothness),
		XtRImmediate, (XtPointer)25
	}, {
		MgvNsuppressWarnings, MgvCSuppressWarnings,
		XtRBoolean, sizeof(Boolean), OFFSET(suppress_warnings),
		XtRImmediate, (XtPointer)False
	}, {
		MgvNwmWidth, MgvCWmWidth,
		XtRInt, sizeof(int), OFFSET(wm_width),
		XtRImmediate, (XtPointer)14
	}, {
		MgvNwmHeight, MgvCWmHeight,
		XtRInt, sizeof(int), OFFSET(wm_height),
		XtRImmediate, (XtPointer)32
	}
};
static XtResource res_pass2[] = {
	{
		MgvNreticuleForeground, MgvCReticuleForeground,
		XtRPixel, sizeof(Pixel), OFFSET(reticule_fg),
		XtRString, (XtPointer)"red"
	}
};
#undef OFFSET

/*
 * These are only used for writing settings to disk.
 */
typedef struct {
	String  arguments;
	String  interpreter;
	Boolean quiet;
	Boolean safer;
	Boolean antialias;
} GVGOOP;
#define	OFFSET(field)	XtOffsetOf(GVGOOP, field)
static XtResource gv_res[] =
{
	{
		GvNarguments, GvCArguments,
		XtRString, sizeof(String), OFFSET(arguments),
		XtRString, NULL
	}, {
		GvNinterpreter, GvCInterpreter,
		XtRString, sizeof(String), OFFSET(interpreter),
		XtRString, "gs"
	}, {
		GvNquiet, GvCQuiet,
		XtRBoolean, sizeof(Boolean), OFFSET(quiet),
		XtRImmediate, (XtPointer)True
	}, {
		GvNsafer, GvCSafer,
		XtRBoolean, sizeof(Boolean), OFFSET(safer),
		XtRImmediate, (XtPointer)True
	}, {
		GvNantialias, GvCAntialias,
		XtRBoolean, sizeof(Boolean), OFFSET(antialias),
		XtRImmediate, (XtPointer)True
	}
};
#undef	OFFSET

/*
 * resource converter for MgvRPageMedia
 */
static Boolean
string2media(Display *dpy, XrmValue *a, Cardinal *n, XrmValue *from,
						XrmValue *to, XtPointer *data)
{
	int   alloced = FALSE;
	char *lower  = NULL;
	int   i;

	USEUP(a); USEUP(n); USEUP(data);

	/*
	 * Initial sanity checks.
	 */
	if(*n != 0) {
		XtWarningMsg("wrongParameters", "cvtStringToMedia",
			"XtToolkitError",
			"String to Media conversion needs no extra arguments",
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
		alloced = TRUE;
	}

	lower = wl_lstrdup((String)from->addr);
	for(i = 0; dsc_media[i].name != NULL; ++i) {
		if(strcmp(lower, dsc_media[i].name) != 0)
			continue;
		*(int *)(to->addr) = i;
		wl_free(lower);
		return True;
	}

	if(alloced)
		to->addr = wl_free(to->addr);
	wl_free(lower);
	XtDisplayStringConversionWarning(dpy, (char *)from->addr,MgvRPageMedia);
	return False;
}

static char *
media2string(char *p)
{
	return XtNewString(dsc_media[*(int *)p].name);
}

static struct {
	char *s;
	int   len;
	int   bits;
} resetsb[] = {
	{"both",       4, MgvRESETSCROLL},
	{"all",        3, MgvRESETSCROLL},
	{"true",       4, MgvRESETSCROLL},
	{"none",       4, 0},
	{"false",      5, 0},
	{"horizontal", 3, MgvRESETSCROLL_H},
	{"vertical",   3, MgvRESETSCROLL_V},
};
#define	N_RESETSB	(int)(sizeof(resetsb)/sizeof(resetsb[0]))

/*
 * resource converter for MgvRResetScrollBars
 */
static Boolean
string2resetsb(Display *dpy, XrmValue *a, Cardinal *n, XrmValue *from,
						XrmValue *to, XtPointer *data)
{
	Boolean	alloced = False;
	char	*lower  = NULL;
	int	i, bits;

	USEUP(dpy); USEUP(a); USEUP(data);

	/*
	 * Initial sanity checks.
	 */
	if(*n != 0) {
		XtWarningMsg("wrongParameters", "cvtStringToResetScrollBars",
			"XtToolkitError",
			"String to ResetScrollBars conversion needs no extra arguments",
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
		alloced = TRUE;
	}

	lower = wl_lstrdup((String)from->addr);
	for(i = 0, bits = -1; i < N_RESETSB && bits == -1; ++i)
		if(strncmp(lower, resetsb[i].s, resetsb[i].len) == 0)
			bits = resetsb[i].bits;
	wl_free(lower);

	if(bits == -1) {
		if(alloced)
			to->addr = wl_free(to->addr);
		return False;
	}
	*(int *)(to->addr) = bits;

	return True;
}

static char *
resetsb2string(char *p)
{
	int bits = *(int *)p;
	int i;

	for(i = 0; i < N_RESETSB; ++i)
		if(bits == resetsb[i].bits)
			return XtNewString(resetsb[i].s);
	return XtNewString("none");
}

/*
 * a doubly linked list makes deletion easier
 */
typedef struct MGVL_s {
	MGV		*m;
	struct MGVL_s	*next, *prev;
} MGVL;
static struct {
	/*
	 * Pass zero is uninitialized, pass one is when we get most of
	 * the resources (everything that doesn't need a realized (ie a
	 * window, colormap, etc...) widget), and pass two is when we get
	 * the rest of the resources.
	 */
	int	pass;

	/*
	 * We use a work proc to process page changes, everything that
	 * changes the page just queues up a request (in a queue of length
	 * one) and the work proc will deal with it when the backend is
	 * ready.
	 */
	XtWorkProcId wp;

	int	nextserial;
	MGVR	res;
	int	n_mgvs;
	Widget	reaper;
	MGVL	*mgvs;

	WLLOG log;

	WLAPP a;
} app;

static XtActionsRec actions[] = {
	{"mgvMark",	mgv_action_mark},
	{"mgvScroll",	mgv_action_scroll},
	{"mgvZoom",	mgv_action_zoom},
	{"mgvPan",	mgv_action_pan}
};

static void
hidelog(Widget w, XtPointer closure, XtPointer call)
{
	USEUP(w); USEUP(closure); USEUP(call);
	mgv_log_show(False);
}

static void
log_create(Widget w)
{
	WLC cb[] = {
		{"*log*hide", XmNactivateCallback, NULL, hidelog},
		{NULL,        NULL,                NULL, NULL}
	};
	Widget   shell;
	Arg      a[3];
	Cardinal n;
	Atom     del;
	char    *title;

	title = wl_res_string(wl_top(w), MgvNlogTitle, MgvCLogTitle,
							"MGv:  Message Log");
	shell = XtAppCreateShell(NULL, "MGv", wlAppShellWidgetClass,
							wl_display(w), NULL, 0);
	app.log = wl_log_open(shell);
	wl_callback(shell, &cb[0], NULL);
	wl_icons("log", shell, NULL, NULL);

	n = 0;
	XtSetArg(a[n], XmNtitle,          title);		++n;
	XtSetArg(a[n], XmNiconName,       title);		++n;
	XtSetArg(a[n], XmNdeleteResponse, XmDO_NOTHING);	++n;
	wlSetValues(shell, a, n);
	del = XmInternAtom(wl_display(shell), "WM_DELETE_WINDOW", False);
	XmAddWMProtocolCallback(shell, del, hidelog, NULL);

	wl_log_show(app.log, !app.res.hide_log);
}

static Boolean
worker(XtPointer closure)
{
	MGVL *p;
	MGV  *m;

	USEUP(closure);

	for(p = app.mgvs; p != NULL; p = p->next) {
		m = p->m;
		if(m->pageq < 0)
			continue;
		if(m->pagedone)
			mgv_showq(m);
	}

	return False;
}

static void
xt_warnings(String msg)
{
	if(app.res.suppress_warnings)
		return;

	/*
	 * We could just use the default handler (as returned by
	 * `XtAppSetWarningHandler') but that's just going to send things
	 * to `stderr' anyway; by doing it ourselves, we get a bit more
	 * control over the format.
	 */
	mgv_log("Xt Warning: %s", msg);
}

static void
app_init(Widget w)
{
	if(app.pass >= 2)
		return;

	wl_assert(w != NULL);
	if(app.pass < 1) {
		XtAppContext ac = XtWidgetToApplicationContext(w);

		XtSetTypeConverter(XmRString, MgvRPageMedia,
				string2media, NULL, 0, XtCacheNone, NULL);
		XtSetTypeConverter(XmRString, MgvRResetScrollBars,
				string2resetsb, NULL, 0, XtCacheNone, NULL);

		wl_res_addfmt(MgvRPageMedia, media2string);
		wl_res_addfmt(MgvRResetScrollBars, resetsb2string);

		wl_res_get(wl_top(w), &app.res, &res_pass1[0],
					XtNumber(res_pass1), NULL, 0);
		XtAppAddActions(ac, actions, XtNumber(actions));
		wl_chkver(w);
		app.nextserial = 1;
		++app.pass;

		/*
		 * Sanity checking.
		 */
		if(app.res.smoothness < 0)
			app.res.smoothness *= -1;
		else if(app.res.smoothness == 0)
			app.res.smoothness = 1;
		if(app.res.magic_resistance < 0)
			app.res.magic_resistance *= -1;
		if(app.res.printcmd == NULL)
			app.res.printcmd = "";

		/*
		 * We may want to change these sometime so we'll make
		 * copies that we know we can free.
		 */
		app.res.bunzip     = XtNewString(app.res.bunzip);
		app.res.bzip       = XtNewString(app.res.bzip);
		app.res.decompress = XtNewString(app.res.decompress);
		app.res.compress   = XtNewString(app.res.compress);
		app.res.gunzip     = XtNewString(app.res.gunzip);
		app.res.gzip       = XtNewString(app.res.gzip);
		app.res.printcmd   = XtNewString(app.res.printcmd);
		app.res.printargv  = wl_stoargv(app.res.printcmd);

		if(strlen(app.res.pdfconv) == 0)
			app.res.pdfconv = NULL;
		else
			app.res.pdfconv = XtNewString(app.res.pdfconv);

		app.wp = XtAppAddWorkProc(ac, worker, NULL);

		log_create(w);

		/*
		 * We don't want to fiddle with the warning handlers until
		 * we have a log and we can't have a log until a couple of
		 * things have already been dealt with.  Hence, a couple
		 * of warnings may slip through.
		 */
		XtAppSetWarningHandler(ac, xt_warnings);
	}
	else if(XtIsRealized(w)) {
		wl_res_get(wl_top(w), &app.res, &res_pass2[0],
						XtNumber(res_pass2), NULL, 0);
		++app.pass;
	}
}

static int
app_ok(void)
{
	DIEIF(app.pass       >  2);
	DIEIF(app.n_mgvs     <= 0);
	DIEIF(app.mgvs       == NULL);
	DIEIF(app.nextserial <= 0);
	return TRUE;
}

static char *rchead[] = {
	"",
	"This file contains various user-definable resources for mgv.",
	"Feel free to change any of these settings; however, if you",
	"add settings for resources that are not already here, they",
	"will be lost when mgv is next run, if you want to set resources",
	"that do not appear in this file then you should put them in the",
	"\"user\" file in this directory.",
	"",
};
#define	N_RCHEAD	(int)(sizeof(rchead)/sizeof(rchead[0]))

static void
close_part2(Widget w, XtPointer closure, XtPointer call)
{
	MGV     *m = (MGV *)closure;
	MGVL    *e;
	Arg      a[2];
	Cardinal n;

	USEUP(w); USEUP(call);
	wl_assert(app_ok());
	wl_assert(MgvOK(m));

	for(e = app.mgvs; e != NULL && e->m != m; e = e->next)
		;
	wl_assert(e != NULL);
	if(e->prev != NULL)
		e->prev->next = e->next;
	else
		app.mgvs = e->next;
	if(e->next != NULL)
		e->next->prev = e->prev;

	XtUnmanageChild(m->main);
	mgv_close(m, FALSE, TRUE);

	n = 0;
	XtSetArg(a[n], XmNiconPixmap, None);	++n;
	XtSetArg(a[n], XmNiconMask,   None);	++n;
	wlSetValues(wl_top(m->main), a, n);

	wl_free_pixmap(wl_display(m->main), m->icon);
	wl_free_pixmap(wl_display(m->main), m->mask);
	wl_free((void *)e);

	XtDestroyWidget(wl_top(m->main));
	mgv_free(m);

	if(--app.n_mgvs == 0) {
		if(app.res.kill_kids) {
			struct sigaction sa;
			memset((void *)&sa, '\0', sizeof(sa));
			sa.sa_handler = SIG_IGN;
			sigemptyset(&sa.sa_mask);
			sigaction(SIGTERM, &sa, NULL);
			kill(-getpgrp(), SIGTERM);
		}

		/*
		 * Remember that we made copies of these in the
		 * resource initialization phase.
		 */
		wl_free(app.res.bunzip);
		wl_free(app.res.bzip);
		wl_free(app.res.decompress);
		wl_free(app.res.compress);
		wl_free(app.res.gunzip);
		wl_free(app.res.gzip);
		wl_free(app.res.pdfconv);
		wl_free(app.res.printcmd);
		wl_free(*app.res.printargv);
		wl_free(app.res.printargv);

		app.log = wl_log_close(app.log);
		app.a   = wl_app_destroy(app.a);
		wl_shutdown();

		exit(EXIT_SUCCESS);
	}
}

static WLW close_problem[] = {
	{"closeProblem", wlQuestionDialog, 0, NULL},
	{NULL,           NULL,             0, NULL,}
};
static WLC close_problem_cb[] = {
	{"*closeProblem", XmNunmapCallback,  NULL, wl_dismiss},
	{"*closeProblem", XmNcancelCallback, NULL, wl_dismiss},
	{"*closeProblem", XmNhelpCallback,   NULL, wl_help},
	{"*closeProblem", XmNokCallback,     NULL, close_part2},
	{NULL,           NULL,             0, NULL,}
};

void
mgv_app_close(Widget w, XtPointer closure, XtPointer call)
{
	MGV     *m = (MGV *)closure;
	Arg      a[5];
	Cardinal n;
	GVGOOP   g;
	int      status;

	USEUP(call);
	wl_assert(app_ok());
	wl_assert(MgvOK(m));

	memset((void *)&g, '\0', sizeof(g));
	n = 0;
	XtSetArg(a[n], GvNarguments,	&g.arguments);		++n;
	XtSetArg(a[n], GvNinterpreter,	&g.interpreter);	++n;
	XtSetArg(a[n], GvNquiet,	&g.quiet);		++n;
	XtSetArg(a[n], GvNsafer,	&g.safer);		++n;
	XtSetArg(a[n], GvNantialias,	&g.antialias);		++n;
	wlGetValues(m->ghost, a, n);

	if((status = wl_app_res_open(app.a, rchead, N_RCHEAD)) == 0) {
		wl_app_res_store(app.a, "*", &app.res,
					res_pass1, XtNumber(res_pass1));
		wl_app_res_store(app.a, "*mgv.", &g,
					gv_res, XtNumber(gv_res));
		wl_app_res_close(app.a);
		close_part2(NULL, m, NULL);
	}
	else {
		char buf[1024];

		sprintf(buf, "%s\n        %s: %s\n%s",
			wl_s(m->main, MgvScantSaveRes),
			wl_app_resfile(app.a), strerror(status),
			wl_s(m->main, MgvScontClosing));
		w = wl_create(wl_top(m->main), close_problem);
		wl_callback(XtParent(w), &close_problem_cb[0], m);
		XtVaSetValues(w,
			XtVaTypedArg,	XmNmessageString,
					XtRString, buf,
					strlen(buf) + 1,
			NULL);
		wl_dlg_popup(w);
	}
}

static void
app_death(Widget w, XtPointer closure, XtPointer call)
{
	MGVL	*e, *next;

	USEUP(w); USEUP(closure); USEUP(call);
	wl_assert(app_ok());

	/*
	 * The final destroy will exit() without any help.
	 * This is safe, the links we care about won't be damaged
	 * by mgv_app_close.  Also, mgv_app_close only uses the
	 * `closure' parameter so we don't care about the other two.
	 */
	for(e = app.mgvs; e != NULL; e = next) {
		next = e->next;
		mgv_app_close(NULL, e->m, NULL);
	}
}

static void
confirm_destroy(Widget w, XtPointer closure, XtPointer call)
{
	USEUP(w); USEUP(closure); USEUP(call);
	app.reaper = NULL;
}

static WLW exit_query[] = {
	{"reallyExit", wlQuestionDialog, 0, NULL},
	{NULL,         NULL,             0, NULL}
};
static WLC exit_query_cb[] = {
	{"*reallyExit", XmNunmapCallback,   NULL, wl_dismiss},
	{"*reallyExit", XmNcancelCallback,  NULL, wl_dismiss},
	{"*reallyExit", XmNhelpCallback,    NULL, wl_help},
	{"*reallyExit", XmNokCallback,      NULL, app_death},
	{"*reallyExit", XmNdestroyCallback, NULL, confirm_destroy},
	{NULL,          NULL,               NULL, NULL}
};

void
mgv_app_exit(Widget w, XtPointer closure, XtPointer call)
{
	wl_assert(app_ok());
	if(app.n_mgvs <= 1 || !app.res.confirm_exit) {
		app_death(w, closure, call);
		return;
	}

	if(app.reaper == NULL) {
		app.reaper = wl_create(wl_top(w), exit_query);
		wl_callback(XtParent(app.reaper), &exit_query_cb[0], NULL);
	}
	wl_dlg_popup(app.reaper);
}

MGVR *
mgv_app_res(Widget w)
{
	app_init(w);
	return &app.res;
}

MGV *
mgv_app_add(MGV *m)
{
	MGVL	*e;

	app_init(m->main);

	if((e = wl_calloc(1, sizeof(MGVL))) == NULL)
		return NULL;
	e->m = m;
	if((e->next = app.mgvs) != NULL)
		e->next->prev = e;
	app.mgvs = e;
	++app.n_mgvs;
	return m;
}

int
mgv_app_nextserial(void)
{
	return app.nextserial++;
}

void
mgv_log_show(Boolean show)
{
	MGVL  *e;
	Widget w;

	if(app.log == NULL)
		return;

	app.res.hide_log = !show;
	for(e = app.mgvs; e != NULL; e = e->next) {
		w = wl_find1(e->m->popup, "*hideLog");
		XmToggleButtonSetState(w, app.res.hide_log, False);
		w = wl_find1(e->m->main, "*menuBar*view_pd*hideLog");
		XmToggleButtonSetState(w, app.res.hide_log, False);
	}
	wl_log_show(app.log, show);
}

void
mgv_log(char *fmt, ...)
{
	va_list ap;

	if(app.log == NULL)
		return;

	va_start(ap, fmt);
	wl_vlog(app.log, fmt, ap);
	va_end(ap);
}

void
mgv_app_set(WLAPP a)
{
	app.a = a;
}

char *
mgv_app_tmpdir(void)
{
	/*
	 * The first entry in the MGVL list is always there and wl_app_tmpdir()
	 * only needs a widget to check an application resource so it doesn't
	 * matter which widget we use.
	 */
	return wl_app_tmpdir(app.a, app.mgvs->m->main);
}

WLAPP
mgv_app_get(void)
{
	return app.a;
}
