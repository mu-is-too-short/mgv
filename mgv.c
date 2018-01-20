/*
 * mgv.c
 *	Top half (the bottom half is Ghostview.c) of the functionality of mgv.
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
MU_ID("$Mu: mgv/mgv.c 1.209 2001/04/09 17:58:19 $")

#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <Xm/List.h>
#include <Xm/ToggleB.h>
#include <Xm/ScrolledW.h>
#include <Xm/ScrollBar.h>

#include <wlib/stdlib.h>
#include <wlib/wlib.h>
#include <wlib/scrollbar.h>
#include <wlib/sanity.h>
#include <wlib/display.h>
#include <mine/mgv.h>
#include <mine/app.h>
#include <mine/unmangle.h>
#include <mine/log.h>
#include <mine/sens.h>
#include <mine/strings.h>
#include <mine/zoom.h>
#include <mine/auto.h>
#include <mine/uiutil.h>
#include <mine/page_control.h>

static void
window_title(MGV *m)
{
	char     *buf;
	Arg      a[2];
	Cardinal n;

	if(m->file != NULL) {
		buf = wl_calloc(64 + strlen(m->file), 1);
		sprintf(buf, "MGv(%d-%d): %s", (int)getpid(),m->serial,m->file);
	}
	else {
		buf = wl_calloc(64, 1);
		sprintf(buf, "MGv(%d-%d)", (int)getpid(), m->serial);
	}

	n = 0;
	XtSetArg(a[n], XmNtitle,    buf);	++n;
	XtSetArg(a[n], XmNiconName, buf);	++n;
	wlSetValues(wl_top(m->main), a, n);
	wl_free(buf);
}

/*
 * I tried using XmScrollVisible() for this but that would get confused
 * at times so I'll just do it by hand (the problem only appears to happen
 * when the horizontal scrollbar was unmanaged, the vertical scrollbar
 * would get reset but the window wouldn't actually scroll, the scrollbars
 * are always managed these days but this works so I'm leaving it alone).
 */
static void
resetsbs(Widget swin, int which)
{
	Widget w;
	WLSBV  v;

	if(which & MgvRESETSCROLL_H) {
		memset((void *)&v, '\0', sizeof(v));
		w = wl_getsbvalues(swin, "*HorScrollBar", &v);
		v.value = v.min;
		wl_setsbvalues(w, &v, True);
	}
	if(which & MgvRESETSCROLL_V) {
		memset((void *)&v, '\0', sizeof(v));
		w = wl_getsbvalues(swin, "*VertScrollBar", &v);
		v.value = v.min;
		wl_setsbvalues(w, &v, True);
	}
}

/*
 * Close current file.  There are some sequence issues here so you'd
 * be advised to watch how the other code closes things before trying
 * to do it yourself.  The open/close code needs a little bit of clean
 * up.
 */
void
mgv_close(MGV *m, int kludge, int reset_kludge)
{
	/*
	 * Reset the widgetry.  If you don't reset the scrollbars here,
	 * the scrolled window widget can get really confused (seems to
	 * only happen when you've scrolled to the bottom with some files).
	 *
	 * However, if you try to reset the scrollbars while the scrolled
	 * window is dying (generally this means you're closing the window)
	 * _and_ the scrollbars are not in their home positions, then
	 * some older versions of Motif (Motif 1.2.3 based on personal
	 * experience and bug reports, 2.0.1 and 1.2.5 appear to deal with
	 * this just fine but I have had problem reports with 1.2.5) will be
	 * naughty and dump core.
	 */
	mgv_zoom_killall(m);
	if(!reset_kludge)
		resetsbs(m->sw, m->bits & MgvRESETSCROLL);
	XmListDeleteAllItems(m->pagelist);
	if(m->orientation != NULL && !kludge) {
		XmToggleButtonSetState(m->orientation, False, False);
		m->orientation = NULL;
	}
	if(m->media != NULL)
		XmToggleButtonSetState(m->media, False, False);
	m->media = NULL;
	wl_unmanage(m->fs_save);
	wl_unmanage(m->docinfo);
	wl_unmanage(m->printdlg);

	GhostviewDisableInterpreter(m->ghost);

	if(m->fp != NULL)
		fclose(m->fp);
	m->realfile = wl_free(m->realfile);
	m->file     = wl_free(m->file);
	m->savefile = wl_free(m->savefile);
	m->marked   = wl_free(m->marked);
	m->dsc      = dsc_free(m->dsc);
	m->fp       = NULL;
	m->page     = 0;
	m->pageq    = -1;
	m->pagedone = FALSE;
	m->n_marked = 0;
	m->pgstack  = mgv_pgstack_free(m->pgstack);
	m->sens    &= ~(MgvSALL & ~MgvSLOG);
	if(!kludge)
		m->bits &= ~MgvFORCEDORIENT;

	window_title(m);
}

/*
 * Initialize the menu toggles, etc...
 */
static void
init(MGV *m)
{
	char  *name;
	char   media[512];
	int    i;
	Widget w;

	/*
	 * set media (this isn't quite right, see DSC)
	 */
	if(m->dsc              != NULL
	&& m->dsc->type        != DSC_EPS
	&& m->dsc->media       != NULL
	&& m->dsc->media->name != NULL) {
		strcpy(media, "*menuBar*media_pd*");
		name = m->dsc->media->name;
		for(i = 0; dsc_media[i].name != NULL; ++i) {
			if(strcmp(name, dsc_media[i].name) != 0)
				continue;
			strcat(media, dsc_media[i].name);
			break;
		}
		if(dsc_media[i].name != NULL) {
			w = wl_find1(m->main, media);
			XmToggleButtonSetState(w, True, False);
			m->media = w;
			mgv_media(m, i);
		}
	}
	else if(m->dsc != NULL
	     && m->dsc->bbox[DSC_URX] > m->dsc->bbox[DSC_LLX]
	     && m->dsc->bbox[DSC_URY] > m->dsc->bbox[DSC_LLY]) {
		mgv_media(m, MgvUSEBBOX);
	}

	/*
	 * set orientation
	 */
	if(!(m->bits & MgvFORCEDORIENT)) {
		GvOrient orient = GvOrientPortrait;

		name   = "*menuBar*orientation_pd*portrait";
		if(m->dsc != NULL
		&& m->dsc->orientation == DSC_LANDSCAPE) {
			name   = "*menuBar*orientation_pd*landscape";
			orient = GvOrientLandscape;
		}
		w = wl_find1(m->main, name);
		XmToggleButtonSetState(w, True, False);
		m->orientation = w;
		mgv_orient(m, orient, FALSE);
	}

	/*
	 * and the antialiasing
	 */
	w = wl_find1(m->main, "*menuBar*options_pd*antiAlias");
	wl_assert(w != NULL);
	XmToggleButtonSetState(w, GhostviewGetAntiAlias(m->ghost), False);
}

/*
 * Really open a file.  Hmm, this gettin' a little grotty me thinks.
 */
static int
openit(MGV *m, int no_dsc)
{
	Widget alias;

	if(m->fp == NULL)
		return 0;
	rewind(m->fp);
	mgv_zoom_killall(m);
	XmListDeleteAllItems(m->pagelist);
	m->dsc    = dsc_free(m->dsc);
	m->marked = wl_free(m->marked);

	if(!no_dsc
	&& (m->dsc = dsc_scan(m->fp)) != NULL
	&& m->dsc->type    != DSC_EPS
	&& m->dsc->n_pages == 0) {
		dsc_free(m->dsc);
		m->dsc = NULL;
	}
	m->page     = 0;
	m->pageq    = -1;
	m->pagedone = TRUE;

	if(m->dsc != NULL
	&& m->dsc->n_pages > 0)
		m->marked = wl_calloc(m->dsc->n_pages, sizeof(int));

	/*
	 * reset the ghostview widget and force the default media
	 */
	mgv_media(m, m->default_media);
	GhostviewDisableInterpreter(m->ghost);
	XtVaSetValues(m->ghost,
		GvNfilename,	(m->dsc == NULL || m->dsc->pages == NULL)
				? m->realfile
				: NULL,
		NULL);
	alias = wl_find1(m->main, "*menuBar*options_pd*antiAlias");
	wl_assert(alias != NULL);
	if(!(m->bits & MgvALIASEPS)
	&& m->dsc       != NULL
	&& m->dsc->type == DSC_EPS) {
		XtVaSetValues(m->ghost, GvNantialias, False, NULL);
		XmToggleButtonSetState(alias, False, False);
	}
	else {
		XtVaSetValues(m->ghost,
			GvNantialias,	m->defalias,
			NULL);
		XmToggleButtonSetState(alias, m->defalias, False);
	}

	/*
	 * then set sensible values for the UI widgetry
	 */
	mgv_list_update(m, -1);

	/*
	 * this is a trick to force reorganization of the main form
	 * so that the scrolled list can get a little wider if it
	 * needs to.
	 */
	if(XtIsManaged(XtParent(m->pagelist))) {
		XtUnmanageChild(XtParent(m->pagelist));
		XtManageChild(XtParent(m->pagelist));
	}

	if(m->dsc != NULL)
		m->sens |= MgvSDOC;
	else
		m->sens &= ~MgvSDOC;
	if(m->dsc != NULL && m->dsc->n_pages > 1) {
		m->sens |= MgvSPAGES;
		XmListSelectPos(m->pagelist, 1, False);
	}
	else {
		m->sens &= ~MgvSPAGES;
	}

	/*
	 * we might have a one page file so this setting might not
	 * be correct, however, the log callbacks will correct the
	 * situation when it gets a "Done" message from the
	 * Ghostview widget
	 */
	m->sens |= MgvSNOTLAST;
	init(m);
	wl_sens(m->senscache, m->sens);

	m->pgstack = mgv_pgstack_alloc(m);
	if(m->dsc != NULL) {
		mgv_pgstack_prefill(m->pgstack, m->dsc->n_pages);
		XmProcessTraversal(m->pagelist, XmTRAVERSE_CURRENT);
	}

	return 0;
}

/*
 * Open and scan stdin.
 */
int
mgv_open_stdin(MGV *m)
{
	int try_dsc;

	if(mgv_app_res(m->main)->copy_stdin) {
		m->fp   = mgv_tcopy(m, &m->realfile, stdin);
		m->file = XtNewString("stdin");
		try_dsc = TRUE;
	}
	else {
		m->fp       = stdin;
		m->realfile = XtNewString("-");
		m->file     = XtNewString("stdin");
		try_dsc     = FALSE;
	}
	openit(m, !try_dsc);
	window_title(m);
	m->sens |= MgvSOPEN;
	wl_sens(m->senscache, m->sens);

	return 0;
}

/*
 * open and scan a file
 */
int
mgv_open(MGV *m, char *file)
{
	FILE	*fp;
	char	*realfile, *myfile;
	int	status;

	realfile =
	myfile   = NULL;
	status   = 0;

	wl_assert(file != NULL || m->file != NULL);
	myfile   = file == NULL ? XtNewString(m->file) : file;
	realfile = XtNewString(myfile);
	if((myfile = mgv_unmangle(m, myfile)) == NULL
	|| (fp     = fopen(myfile, "r"))      == NULL) {
		status = errno;
		goto Hell;
	}

	/*
	 * Make sure we're all closed up before its too late.
	 * There's no point in closing if we're not open
	 * though (in fact, if we do close when we're not open
	 * the orientation switches will not stick).
	 */
	if(m->file != NULL)
		mgv_close(m,file == NULL && (m->bits & MgvFORCEDORIENT), FALSE);

	m->fp       = fp;
	m->realfile = myfile;
	m->file     = realfile;
	openit(m, FALSE);
	window_title(m);
	m->sens |= MgvSOPEN | MgvSCANREOPEN;
	wl_sens(m->senscache, m->sens);

	return 0;
Hell:
	wl_free(myfile);
	wl_free(realfile);

	return status;
}

void
mgv_refresh(MGV *m)
{
	DSC	*dsc = m->dsc;
	FILE	*fp  = m->fp;
	Widget	w    = m->ghost;

	if(dsc == NULL || dsc->pages == NULL)
		return;
	GhostviewSendPS(w, fp, dsc->prolog.begin, dsc->prolog.len, False);
	GhostviewSendPS(w, fp, dsc->setup.begin,  dsc->setup.len,  False);
	GhostviewSendPS(w, fp, dsc->pages[m->page].sect.begin,
					dsc->pages[m->page].sect.len, False);
}

void
mgv_show(MGV *m, int page)
{
	m->pageq = page;
	if(GhostviewIsInterpreterRunning(m->ghost))
		GhostviewDisableInterpreter(m->ghost);
	m->pagedone = TRUE;
}

void
mgv_showq(MGV *m)
{
	DSC	*dsc;
	int	o;
	int	page;

	/*
	 * block insanity
	 */
	if(m           == NULL
	|| m->ghost    == NULL
	|| m->realfile == NULL
	|| m->file     == NULL
	|| m->pageq    <  0)
		return;

	page = m->pageq;

	/*
	 * range check the page number
	 */
	if((dsc = m->dsc) != NULL) {
		if(page >= dsc->n_pages)
			page = dsc->n_pages - 1;
		if(page < 0)
			page = 0;
	}
	m->page     = page;
	m->pageq    = -1;
	m->pagedone = FALSE;

	resetsbs(m->sw, m->bits & MgvRESETSCROLL);

	/*
	 * new page orientation
	 */
	if(dsc != NULL && !(m->bits & MgvFORCEDORIENT)) {
		o = -1;
		if(dsc->pages != NULL
		&& dsc->pages[page].orientation != DSC_NONE)
			o = dsc->pages[page].orientation;
		else if(dsc->deforient != DSC_NONE)
			o = dsc->deforient;
		else if(dsc->orientation != DSC_NONE)
			o = dsc->orientation;

		switch(o) {
		case DSC_PORTRAIT:
			mgv_orient(m, GvOrientPortrait, FALSE);
			break;
		case DSC_LANDSCAPE:
			mgv_orient(m, GvOrientLandscape, FALSE);
			break;
		default:
			break;
		}
	}

	/*
	 * non DSC file -- just render the next page
	 */
	if(dsc == NULL || dsc->pages == NULL) {
		if(!GhostviewIsInterpreterRunning(m->ghost))
			GhostviewEnableInterpreter(m->ghost);
		else if(GhostviewIsInterpreterReady(m->ghost))
			GhostviewNextPage(m->ghost);
		else
			XBell(wl_display(m->ghost), 100);
		m->autosizer(m);
		m->center(m);
		return;
	}

	/*
	 * adjust various widgets accordingly
	 */
	XmListSelectPos(m->pagelist, m->page + 1, False);
	if(m->page > 0)
		m->sens |= MgvSNOTFIRST;
	else
		m->sens &= ~MgvSNOTFIRST;
	if(m->page < dsc->n_pages - 1)
		m->sens |= MgvSNOTLAST;
	else
		m->sens &= ~MgvSNOTLAST;
	wl_sens(m->senscache, m->sens);

	/*
	 * then draw it
	 */
	if(!GhostviewIsInterpreterReady(m->ghost)) {
		GhostviewEnableInterpreter(m->ghost);
		GhostviewSendPS(m->ghost, m->fp, dsc->prolog.begin,
						dsc->prolog.len, False);
		GhostviewSendPS(m->ghost, m->fp, dsc->setup.begin,
						dsc->setup.len, False);
	}
	else {
		GhostviewNextPage(m->ghost);
	}
	GhostviewSendPS(m->ghost, m->fp, dsc->pages[m->page].sect.begin,
					dsc->pages[m->page].sect.len, False);

	/*
	 * And autosizing.  I could have a NULL check here instead of
	 * using the mgv_auto_noop() thing but I think empty functions
	 * are cleaner than a bunch of NULL checks.
	 */
	m->autosizer(m);
	m->center(m);
}

/*
 * assign page orientation
 */
void
mgv_orient(MGV *m, GvOrient orient, int force)
{
	GvOrient current = GhostviewGetOrientation(m->ghost);

	/*
	 * if we don't reset the visible region, the scrolled
	 * window will get all confused
	 */
	if(current != orient)
		resetsbs(m->sw, MgvRESETSCROLL);
	if(force)
		m->bits |= MgvFORCEDORIENT;
	else
		m->bits &= ~MgvFORCEDORIENT;
	GhostviewDisableInterpreter(m->ghost);
	XtVaSetValues(m->ghost, XtNorientation, orient, NULL);
}

/*
 * Assign page magnification.  The factor of 1.2 was taken from ghostview
 * and ghostview took it from TeX, I don't know the significance of this
 * value (it is approximately ln(10)/2 but that's the closest thing I can
 * figure out, perhaps I should ask the magical constant oracle (I've got
 * the URL somewhere) to meditate upon this affair, I could even be so
 * daring and bold as to read the TeX source code).
 */
static double
magnify(double dpi, int mag)
{
	for(; mag > 0; dpi *= 1.2, --mag)
		;
	for(; mag < 0; dpi /= 1.2, ++mag)
		;
	return dpi;
}

int
mgv_magnify(MGV *m, int mag)
{
	int xdpi, ydpi;

	if(mag == m->magnification)
		return FALSE;

	m->magnification = mag;
	GhostviewDisableInterpreter(m->ghost);
	xdpi = (int)magnify((double)m->default_xdpi, mag);
	ydpi = (int)magnify((double)m->default_ydpi, mag);
	XtVaSetValues(m->ghost,
		GvNxdpi,	xdpi,
		GvNydpi,	ydpi,
		NULL);

	return TRUE;
}

void
mgv_mark(MGV *m, int page, int mark, int update)
{
	if(m == NULL
	|| m->dsc == NULL
	|| m->dsc->pages == NULL
	|| m->marked[page] == mark)
		return;
	m->marked[page] = mark;

	m->n_marked += (mark ? 1 : -1);
	if(m->n_marked > 0)
		m->sens |= MgvSMARKED;
	else
		m->sens &= ~MgvSMARKED;

	if(update)
		mgv_list_update(m, page + 1);
}

/*
 * set the media
 */
void
mgv_media(MGV *m, int media)
{
	int      o_llx, o_lly, o_urx, o_ury;
	int      n_llx, n_lly, n_urx, n_ury;
	Arg      a[4];
	Cardinal n;

	if(media != MgvUSEBBOX) {
		n_llx = 0;
		n_lly = 0;
		n_urx = dsc_media[media].width;
		n_ury = dsc_media[media].height;
	}
	else {
		n_llx = m->dsc->bbox[DSC_LLX];
		n_lly = m->dsc->bbox[DSC_LLY];
		n_urx = m->dsc->bbox[DSC_URX];
		n_ury = m->dsc->bbox[DSC_URY];
	}

	n = 0;
	XtSetArg(a[n], GvNllx, &o_llx);	++n;
	XtSetArg(a[n], GvNlly, &o_lly);	++n;
	XtSetArg(a[n], GvNurx, &o_urx);	++n;
	XtSetArg(a[n], GvNury, &o_ury);	++n;
	wlGetValues(m->ghost, a, n);

	/*
	 * if nothing has changed, then don't bother
	 */
	if(n_llx == o_llx && n_lly == o_lly
	&& n_urx == o_urx && n_ury == o_ury)
		return;

	n = 0;
	XtSetArg(a[n], GvNllx, n_llx);	++n;
	XtSetArg(a[n], GvNlly, n_lly);	++n;
	XtSetArg(a[n], GvNurx, n_urx);	++n;
	XtSetArg(a[n], GvNury, n_ury);	++n;
	wlSetValues(m->ghost, a, n);
}

/*
 * copy part of a PS file and fix the "%%Pages:" comment
 */
static void
copy_and_fix(FILE *in, FILE *out, long begin, long end, int pages,
							int check_atend)
{
	long	here;
	int	atend, written;
	char	*cmt, *s;

	here    = begin;
	written =
	atend   = FALSE;
	while((cmt = dsc_cpuntil(in, out, here, end, "%%Pages:")) != NULL) {
		here = ftell(in);
		if(written || (check_atend && atend)) {
			wl_free(cmt);
			continue;
		}

		s = cmt + sizeof("%%Pages:");
		while(*s != '\0' && isspace((int)*s & 0xff))
			++s;
		if(check_atend
		&& strncmp(s, "(atend)", sizeof("(atend)") - 1) == 0) {
			fputs(cmt, out);
			atend = TRUE;
		}
		else {
			s = cmt + sizeof("%%Pages:");
			while(*s != '\0' && isspace((int)*s & 0xff))	++s;
			while(*s != '\0' && isdigit((int)*s & 0xff))	++s;
			while(*s != '\0' && isspace((int)*s & 0xff))	++s;
			if(*s == '\0')
				fprintf(out, "%%%%Pages: %d\n", pages);
			else
				fprintf(out, "%%%%Pages: %d %s\n", pages, s);
			written = TRUE;
		}
		wl_free(cmt);
	}
}

static void
pgcp(FILE *in, FILE *out, MGV *m, int i, int page)
{
	char	*cmt;

	cmt = dsc_cpuntil(in, out, m->dsc->pages[i].sect.begin,
					m->dsc->pages[i].sect.end, "%%Page:");
	if(m->dsc->pages[i].label != NULL)
		fprintf(out, "%%%%Page: %s %d\n", m->dsc->pages[i].label, page);
	else
		fprintf(out, "%%%%Page: %d\n", page);
	wl_free(cmt);
	dsc_cp(in, out, -1, m->dsc->pages[i].sect.end);
}

static int
write_marked(MGV *m, FILE *out)
{
	FILE	*in;
	int	i, page;

	wl_assert(m->n_marked > 0);

	if((in = fopen(m->realfile, "r")) == NULL)
		return FALSE;

	/*
	 * copy the header section but watch out for "%%Pages:"
	 * comments since we have to correct them
	 */
	copy_and_fix(in, out, m->dsc->head.begin, m->dsc->head.end,
							m->n_marked, TRUE);

	/*
	 * copy the rest of the "header" stuff
	 */
	dsc_cp(in, out, m->dsc->preview.begin,  m->dsc->preview.end);
	dsc_cp(in, out, m->dsc->defaults.begin, m->dsc->defaults.end);
	dsc_cp(in, out, m->dsc->prolog.begin,   m->dsc->prolog.end);
	dsc_cp(in, out, m->dsc->setup.begin,    m->dsc->setup.end);

	/*
	 * copy the pages correcting the "%%Page:" comments as we go
	 */
	if(m->dsc->pageorder == DSC_DESCEND) {
		for(i = m->dsc->n_pages-1, page = m->n_marked; i >= 0; --i)
			if(m->marked[i])
				pgcp(in, out, m, i, page--);
	}
	else {
		for(i = 0, page = 1; i < m->dsc->n_pages; ++i)
			if(m->marked[i])
				pgcp(in, out, m, i, page++);
	}


	/*
	 * copy the trailer and once again, we are on the look-out
	 * for "%%Pages:" comments
	 */
	copy_and_fix(in, out, m->dsc->trailer.begin, m->dsc->trailer.end,
							m->n_marked, FALSE);

	fclose(in);

	return TRUE;
}

/*
 * dump pages to a FILE *
 */
int
mgv_write(MGV *m, FILE *out, int marked)
{
	FILE	*in;
	char	buf[1024];
	size_t	n;

	if(marked)
		return write_marked(m, out);

	/*
	 * if we're not considering marked pages, then just
	 * copy the whole damn file to out
	 */
	if((in = fopen(m->realfile, "r")) == NULL)
		return FALSE;
	while((n = fread((void *)&buf[0], 1, sizeof(buf), in)) != 0)
		fwrite((void *)&buf[0], 1, n, out);
	fclose(in);

	return TRUE;
}

MGV *
mgv_alloc(char *file)
{
	MGV	*m;

	if((m = wl_calloc(1, sizeof(MGV))) == NULL)
		return NULL;
	m->secret_handshake1 = MgvMAGIC1;
	m->secret_handshake2 = MgvMAGIC2;
	m->serial            = mgv_app_nextserial();
	m->autosizer         = mgv_auto_noop;
	if(file == NULL)
		return m;
	m->realfile = XtNewString(file);
	m->file     = XtNewString(file);
	m->pageq    = -1;
	m->pagedone = TRUE;

	return m;
}

/*
 * dup an mgv (for zooming)
 */
MGV *
mgv_zdup(MGV *in)
{
	MGV *m;

	if((m = mgv_alloc(in->realfile)) == NULL)
		return NULL;

	m->type   = MgvTZOOM;
	m->serial = in->serial;
	m->parent = mgv_top(in);
	m->main   = in->main;
	m->dsc    = in->dsc;
	m->page   = in->page;

	return m;
}

/*
 * Free an mgv (this only frees allocated memory;
 * Widget, FILE *, Pixmap, etc... types are your problem, you
 * should mgv_close() before this if you are killing a top level
 * window).
 *
 * You may think it odd that we return an MGV* but it is convenient:
 * you can do things like "m = mgv_free(m);" rather than
 * "mgv_free(m); m = NULL;".  I would be a little happier if all
 * destructors acted like this.
 */
MGV *
mgv_free(MGV *m)
{
	if(m == NULL)
		return NULL;

	wl_assert(m->fp == NULL);

	wl_assert(MgvZOK(m));
	wl_free(m->realfile);
	if(!MgvISZOOM(m)) {
		wl_assert(MgvOK(m));
		wl_free(m->savefile);
	}
	else {
		m->dsc = NULL;
	}

	/*
	 * sequence checks, zooms share the dsc with their
	 * main window
	 */
	wl_assert(m->dsc == NULL);
	m->pgstack   = mgv_pgstack_free(m->pgstack);
	m->senscache = wl_sens_free(m->senscache);
	wl_free(m);

	return NULL;
}

/*
 * Generate a page label.  The returned value should be freed with
 * wl_free when you're done with it.
 */
char *
mgv_label(MGV *m, int page)
{
	char *label = m->dsc->pages[page].label;
	char *buf   = NULL;

	switch(m->labels + 2 * m->pgnums) {
	case 1:
		buf = wl_calloc(strlen(label) + 3, 1);
		sprintf(buf, "(%s)", label);
		break;
	case 2:
		buf = wl_calloc(32, 1);
		sprintf(buf, "%d", page + 1);
		break;
	case 3:
		buf = wl_calloc(strlen(label) + 3 + 32, 1);
		sprintf(buf, "%d(%s)", page + 1, label);
		break;
	default:
		buf = wl_calloc(32, 1);
		sprintf(buf, "%d", page + 1);
		break;
	}

	return buf;
}

int
mgv_ok(MGV *m, int zoom)
{
	DIEIF(m                    == NULL);
	DIEIF(m->secret_handshake1 != MgvMAGIC1);
	DIEIF(m->secret_handshake2 != MgvMAGIC2);
	DIEIF((m->type & MgvTZOOM) == 0);
	DIEIF(m->serial            <= 0);
	DIEIF(m->ghost             == NULL);
	DIEIF(m->main              == NULL);
	DIEIF(m->zoom_menu         == NULL);
	if(zoom)
		return TRUE;

	DIEIF((m->type & MgvTFULL) == 0);
	DIEIF(m->autosizer         == NULL);
	DIEIF(m->clip              == NULL);
	DIEIF(m->pagelist          == NULL);
	DIEIF(m->sw                == NULL);
	DIEIF(m->tracker           == NULL);
	DIEIF(m->n_marked           < 0);
	DIEIF(m->root_height       <= 0);
	DIEIF(m->root_width        <= 0);
	DIEIF(m->smoothness        <= 0);

	return TRUE;
}

/*
 * Zoom MGV's and their main window MGV share a couple of things.
 */
MGV *
mgv_top(MGV *m)
{
	for(; m->parent != NULL; m = m->parent)
		;
	wl_assert(MgvOK(m));
	return m;
}
