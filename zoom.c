/*
 * zoom.c
 *	Zooming callbacks for mgv.
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
MU_ID("$Mu: mgv/zoom.c 1.135 2001/03/18 20:06:27 $")

#include <X11/Xlib.h>
#include <Xm/RowColumn.h>
#include <Xm/AtomMgr.h>
#include <Xm/Protocols.h>

#include <wlib/stdlib.h>
#include <wlib/wlib.h>
#include <wlib/sanity.h>
#include <wlib/util.h>
#include <wlib/display.h>
#include <mine/mgv.h>
#include <mine/strings.h>
#include <mine/zoom.h>
#include <mine/log.h>
#include <mine/app.h>
#include <mine/sens.h>
#include <mine/error.h>

/*
 * one zoomer
 */
#define	ZOOM_MAGIC1	0x5a4f4f4d	/* 'Z' 'O' 'O' 'M' */
#define ZOOM_MAGIC2	0x4d4f4f5a	/* 'M' 'O' 'O' 'Z' */
struct MGVZ_s {
	long   magic1;
	Widget shell;		
	Widget zoom;		/* the form	*/
	MGVZ   next, prev;
	long   magic2;
};

static int
zoom_ok(MGVZ z)
{
	DIEIF(z         == NULL);
	DIEIF(z->magic1 != ZOOM_MAGIC1);
	DIEIF(z->magic2 != ZOOM_MAGIC2);
	DIEIF(z->shell  == NULL);
	DIEIF(z->zoom   == NULL);
	return TRUE;
}

/*
 * convert a rectangle to a position and size taking care to do the
 * subtractions in the right order
 */
static void
geom(int x1, int y1, int x2, int y2, int *x, int *y, unsigned *w, unsigned *h)
{
	if(x1 < x2)
		*x = x1, *w = x2 - x1;
	else
		*x = x2, *w = x1 - x2;
	if(y1 < y2)
		*y = y1, *h = y2 - y1;
	else
		*y = y2, *h = y1 - y2;
}

static void
clip(int *x, int *y, int xwant, int ywant, Dimension w, Dimension h)
{
	*x = (xwant < 0) ? 0 : xwant;
	*y = (ywant < 0) ? 0 : ywant;
	*x = (*x    > w) ? w : *x; 
	*y = (*y    > h) ? h : *y; 
}

void
mgv_action_zoom(Widget w, XEvent *ev, String *argv, Cardinal *argc)
{
	MGV		*m;
	Widget		mainwin;
	int		x, y;
	unsigned	wd, ht;
	Dimension	gw, gh;
	char		*action = NULL;

	if(ev == NULL || *argc < 1)
		return;

	x = y = wd = ht = 0;

	/*
	 * Get the mgv which is the XmNuserData of the top level form.
	 */
	for(mainwin = XtParent(w); mainwin != NULL; mainwin = XtParent(mainwin))
		if(strcmp(XtName(mainwin), "mainWindow") == 0
		|| strcmp(XtName(mainwin), "zoomer")     == 0)
			break;
	wl_assert(mainwin != NULL);

	XtVaGetValues(mainwin, XmNuserData, &m, NULL);
	wl_assert(MgvZOK(m));

	/*
	 * sanity
	 */
	if(w != m->ghost || m->realfile == NULL || m->file == NULL) {
		XBell(wl_display(mainwin), 100);
		return;
	}

	/*
	 * Set up the GC.
	 * We don't try to establish the graphics context until the
	 * first time through:  we need a realized widget for the GC
	 * and we really can't be certain about when we'll have such
	 * a widget, we do know, however, that we will (or at least
	 * should) have a realized widget by the time we get here
	 * for the first time.
	 */
	if(m->reticule_gc == None) {
		MGVR     *r = mgv_app_res(m->main);
		XGCValues gcv;
		int       mask;

		memset((void *)&gcv, '\0', sizeof(gcv));
		mask = 0;

		gcv.foreground = r->reticule_fg;
		gcv.line_width = r->reticule_width;
		gcv.function   = GXxor;
		mask = GCForeground | GCLineWidth | GCFunction;
		m->reticule_gc = XtGetGC(m->ghost, mask, &gcv);
	}

	/*
	 * parse the action
	 */
	action = wl_lstrdup(argv[0]);
	if(strcmp(action, "begin") == 0) {
		m->startx = ev->xbutton.x;
		m->starty = ev->xbutton.y;
		m->bits  &= ~MgvZOOMDRAWN;
	}
	else if(strcmp(action, "extend") == 0) {
		if(m->bits & MgvZOOMDRAWN) {
			geom(m->startx, m->starty, m->curx, m->cury,
					&x, &y, &wd, &ht);
			XDrawRectangle(wl_display(m->ghost),
					XtWindow(m->ghost), m->reticule_gc,
					x, y, wd, ht);
		}
		m->bits |= MgvZOOMDRAWN;
		XtVaGetValues(m->ghost, XmNwidth, &gw, XmNheight, &gh, NULL);
		clip(&m->curx, &m->cury, ev->xmotion.x, ev->xmotion.y, gw, gh);
		geom(m->startx, m->starty, m->curx, m->cury, &x, &y, &wd, &ht);
		XDrawRectangle(wl_display(m->ghost), XtWindow(m->ghost),
					m->reticule_gc, x, y, wd, ht);
	}
	else if(strcmp(action, "end") == 0) {
		if(!(m->bits & MgvZOOMDRAWN))
			return;
		geom(m->startx, m->starty, m->curx, m->cury, &x, &y, &wd, &ht);
		XDrawRectangle(wl_display(m->ghost), XtWindow(m->ghost),
					m->reticule_gc, x, y, wd, ht);
		XtVaGetValues(m->ghost, XmNwidth, &gw, XmNheight, &gh, NULL);
		clip(&m->curx, &m->cury, ev->xmotion.x, ev->xmotion.y, gw, gh);
		geom(m->startx, m->starty, m->curx, m->cury, &x, &y, &wd, &ht);
		XDrawRectangle(wl_display(m->ghost), XtWindow(m->ghost),
					m->reticule_gc, x, y, wd, ht);

		/*
		 * popup the menu
		 */
		XtSetSensitive(m->zoom_menu, True);
		XmMenuPosition(m->zoom_menu, &ev->xbutton);
		XtManageChild(m->zoom_menu);
	}
	else {
		mgv_warn(m->ghost, wl_s(w, MgvSbadArgument), "mgvZoom",argv[0]);
	}

	wl_free(action);
}

static WLW menu_zoom[] = {
	{"15", wlPushB, 0, NULL},
	{"20", wlPushB, 0, NULL},
	{"30", wlPushB, 0, NULL},
	{"40", wlPushB, 0, NULL},
	{"50", wlPushB, 0, NULL},
	{NULL, NULL,    0, NULL}
};
static WLW zoom_brats[] = {
	{"dismiss",  wlPushB,     0, NULL},
	{"sep",      wlSeparator, 0, NULL},
	{"view",     "Ghostview", 0, NULL},
	{"zoomMenu", wlPopupMenu, 0, menu_zoom},
	{NULL,       NULL,        0, NULL},
};
static WLW zoom_window[] = {
	{"zoomer", wlFormDialog, 0, zoom_brats},
	{NULL,     NULL,         0, NULL},
};

static void
zoom_add(MGV *top, Widget zoom)
{
	MGVZ z;

	if((z = wl_malloc(sizeof(struct MGVZ_s))) == NULL)
		return;
	z->magic1 = ZOOM_MAGIC1;
	z->magic2 = ZOOM_MAGIC2;
	z->shell  = XtParent(zoom);
	z->zoom   = zoom;
	z->prev   = NULL;
	z->next   = top->zoomers;
	if(top->zoomers != NULL)
		top->zoomers->prev = z;
	top->zoomers = z;
	if(++top->n_zooms == 1)
		wl_sens(top->senscache, top->sens |= MgvSGOTZOOMS);
	wl_assert(zoom_ok(z));
	wl_assert(zoom_ok(top->zoomers));
}

static void
zoom_del(MGV *top, Widget shell)
{
	MGVZ z;
	MGV *m;

	for(z = top->zoomers; z != NULL && z->shell != shell; z = z->next)
		;
	wl_assert(z != NULL);
	if(z->prev != NULL)
		z->prev->next = z->next;
	else
		top->zoomers = z->next;
	if(z->next != NULL)
		z->next->prev = z->prev;
	XtVaGetValues(z->zoom, XmNuserData, &m, NULL);
	wl_assert(MgvZOK(m));
	wl_assert(top->zoomers == NULL || zoom_ok(top->zoomers));
	wl_assert(zoom_ok(z));

	mgv_free(m);
	XtDestroyWidget(z->zoom);
	wl_free(z);
	if(--top->n_zooms == 0)
		wl_sens(top->senscache, top->sens &= ~MgvSGOTZOOMS);
}

static void
zoom_dismiss(Widget w, XtPointer closure, XtPointer call)
{
	USEUP(call); USEUP(closure);
	zoom_del(mgv_top((MGV *)closure), wl_shell(w));
}

static WLC zoomcb[] = {
	{"*dismiss",     XmNactivateCallback, NULL, zoom_dismiss},
	{"*view",        GvNoutputCallback,   NULL, mgv_log_output},
	{"*view",        GvNmessageCallback,  NULL, mgv_log_message},
	{"*zoomMenu*15", XmNactivateCallback, NULL, mgv_zoom},
	{"*zoomMenu*20", XmNactivateCallback, NULL, mgv_zoom},
	{"*zoomMenu*30", XmNactivateCallback, NULL, mgv_zoom},
	{"*zoomMenu*40", XmNactivateCallback, NULL, mgv_zoom},
	{"*zoomMenu*50", XmNactivateCallback, NULL, mgv_zoom},
	{"*zoomMenu",    XmNunmapCallback,    NULL, mgv_zoom_unmap}, 
	{NULL,           NULL,                NULL, NULL}
};

/*
 * This is a trick.  It makes sense for the zoom windows to have
 * the main window as their parent but it also makes sense to have
 * them behave like their parent was the view widget that they are
 * zooming into when they popup (i.e. you want them to be centered
 * on the view they came from).  This isn't perfect but it will do.
 */
static void
popup_centered(Widget w, Widget c)
{
	Widget		p = XtParent(w);
	Position	cx, cy, px, py;
	Dimension	cw, ch, pw, ph;

	XtVaGetValues(c,
		XmNx,		&cx,	XmNy,		&cy,
		XmNwidth,	&cw,	XmNheight,	&ch,
		NULL);
	XtVaGetValues(p,
		XmNwidth,	&pw,	XmNheight,	&ph,
		NULL);

	px = (cx + cw/2 - pw/2) < 0 ? 0 : (cx + cw/2 - pw/2);
	py = (cy + ch/2 - ph/2) < 0 ? 0 : (cy + ch/2 - ph/2);
	XtVaSetValues(XtParent(w), XmNx, px, XmNy, py, NULL);
	XtVaSetValues(w, XmNmappedWhenManaged, True, NULL);
}

static void
zoomin(MGV *om, int factor, int x, int y, unsigned w, unsigned h)
{
	MGV     *m, *top;
	Widget   z;
	Arg      a[11];
	Cardinal n;
	int      xdpi, ydpi;
	FILE    *fp;
	int      llx, lly, urx, ury;
	Pixel    fg, bg; 
	Boolean  antialias;
	Atom     deletewin;
	GvOrient orient;
	GhostviewReturnStruct	gs;

	/*
	 * sanity check
	 */
	if(w == 0 || h == 0)
		return;

	XtVaGetValues(om->main, XmNuserData, &top, NULL);
	wl_assert(MgvOK(top));

	GhostviewPSBBox(om->ghost, x, y, x + w, y + h, &gs);

	/*
	 * set up the new DPI values
	 */
	llx  = gs.psx - gs.width  / 2;
	lly  = gs.psy - gs.height / 2;
	urx  = gs.psx + gs.width  / 2;
	ury  = gs.psy + gs.height / 2;
	xdpi = (gs.xdpi * (double)factor) / 10.0;
	ydpi = (gs.ydpi * (double)factor) / 10.0;

	/*
	 * Have the calculations driven us insane? They do that sometimes
	 * you know.
	 */
	if(llx >= urx || lly >= ury)
		return;

	/*
	 * build the widgets
	 */
	XtVaGetValues(om->ghost,
		XtNforeground,	&fg,
		XtNbackground,	&bg,
		GvNantialias,	&antialias,
		GvNorientation, &orient,
		NULL);
	z = wl_create(om->main, zoom_window);
	m = mgv_zdup(om);
	XtVaSetValues(z, XmNuserData, m, NULL);
	zoom_add(top, z);
	wl_callback(XtParent(z), &zoomcb[0], (XtPointer)m);
	wl_find(z,
		&m->ghost,	"*view",
		&m->zoom_menu,	"*zoomMenu",
		NULL);
	deletewin = XmInternAtom(wl_display(z), "WM_DELETE_WINDOW", False);
	XmAddWMProtocolCallback(XtParent(z), deletewin, zoom_dismiss, m);

	/*
	 * set up the new Ghostview widget
	 */
	n = 0;
	XtSetArg(a[n],	GvNllx,		llx);		++n;
	XtSetArg(a[n],	GvNlly,		lly);		++n;
	XtSetArg(a[n],	GvNurx,		urx);		++n;
	XtSetArg(a[n],	GvNury,		ury);		++n;
	XtSetArg(a[n],	GvNantialias,	antialias);	++n;
	XtSetArg(a[n],	XtNorientation,	orient);	++n;
	XtSetArg(a[n],	XtNforeground,	fg);		++n;
	XtSetArg(a[n],	XtNbackground,	bg);		++n;
	XtSetArg(a[n],	GvNxdpi,	xdpi);		++n;
	XtSetArg(a[n],	GvNydpi,	ydpi);		++n;
	wlSetValues(m->ghost, a, n);

	n = 0;
	XtSetArg(a[n], XmNuserData, m);	++n;
	wlSetValues(z, a, n);

	/*
	 * and life
	 */
	XtManageChild(z);
	popup_centered(z, MgvISZOOM(om) ? XtParent(om->ghost) : om->main);

	/*
	 * send in the PS
	 *
	 * If we don't have DSC then the best we can do is
	 * to give the new Ghostview widget the input filename
	 * and leave it at that.
	 */
	if(m->dsc == NULL || m->dsc->pages == NULL) {
		n = 0;
		XtSetArg(a[n], GvNfilename, m->realfile);	++n;
		wlSetValues(m->ghost, a, n);
		GhostviewInitialize(m->ghost);
		GhostviewEnableInterpreter(m->ghost);
		return;
	}

	GhostviewInitialize(m->ghost);
	GhostviewEnableInterpreter(m->ghost);
	if((fp = fopen(m->realfile, "r")) == NULL) {
		XtDestroyWidget(z);
		return;
	}
	GhostviewSendPS(m->ghost, fp, m->dsc->prolog.begin,
				m->dsc->prolog.len, False);
	GhostviewSendPS(m->ghost, fp, m->dsc->setup.begin,
				m->dsc->setup.len, False);
	GhostviewSendPS(m->ghost, fp, m->dsc->pages[m->page].sect.begin,
				m->dsc->pages[m->page].sect.len, True);
}

/*
 * zoom popup callback (all the buttons go through here)
 */
void
mgv_zoom(Widget w, XtPointer closure, XtPointer call)
{
	MGV     *m = (MGV *)closure;
	char    *name;
	int      x, y;
	unsigned wd, ht;

	USEUP(call);
	wl_assert(MgvZOK(m));

	name = XtName(w);

	/*
	 * figure out the "real" rectangle, the menu's unmap
	 * callback will clean up the reticule
	 */
	x = y = wd = ht = 0;
	geom(m->startx, m->starty, m->curx, m->cury, &x, &y, &wd, &ht);

	/*
	 * deal with it
	 */
	zoomin(m, atoi(name), x, y, wd, ht);
}

/*
 * the zoom menu has popped down so clean up the reticule
 */
void
mgv_zoom_unmap(Widget w, XtPointer closure, XtPointer call)
{
	MGV     *m = (MGV *)closure;
	int      x, y;
	unsigned wd, ht;

	USEUP(w); USEUP(call);
	wl_assert(MgvZOK(m));

	/*
	 * If the reticule was drawn at all, then clear the whole
	 * reticule rectangle to make sure the reticule gets completely
	 * cleaned up.  Originally, I was just drawing the reticule
	 * rectangle one last time (the graphics context for drawing
	 * the reticule is in xor mode BTW) but this tends to leave
	 * some pixels behind because the popup menu will obscure some
	 * (a single pixel in most cases but it can be more) of the
	 * reticule and this screws up the xor-ing.
	 *
	 * "Kill 'em all and let Goddess sort 'em out!"
	 */
	if(m->bits & MgvZOOMDRAWN) {
		int rw = mgv_app_res(m->main)->reticule_width;

		geom(m->startx, m->starty, m->curx, m->cury, &x, &y, &wd, &ht);
		XClearArea(wl_display(m->main), XtWindow(m->ghost),
			x - rw, y - rw, wd + 2*rw, ht + 2*rw, True);
	}
	m->bits &= ~MgvZOOMDRAWN;
	XtSetSensitive(m->zoom_menu, False);
}

void
mgv_zoom_killall(MGV *m)
{
	MGVZ z, next;

	/*
	 * Since the only thing that changes in *z->next when a zoomer
	 * is killed is the prev member, this is safe.
	 */
	for(z = m->zoomers; z != NULL; z = next) {
		next = z->next;
		zoom_del(m, z->shell);
	}
}
