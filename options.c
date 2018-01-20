/*
 * options.c
 *	Options menu callbacks for mgv.
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
MU_ID("$Mu: mgv/options.c 1.34 1998/11/11 05:59:53 $")

#include <Xm/ToggleB.h>

#include <wlib/typesanity.h>
#include <wlib/sanity.h>
#include <mine/mgv.h>
#include <mine/app.h>
#include <mine/options.h>

/*
 * options->anti alias
 * If the this function gets called, then the user has requested an
 * anti-aliasing state so we take the new state as the default.
 */
void
mgv_options_antialias(Widget w, XtPointer closure, XtPointer call)
{
	XmTBS *cbs = (XmTBS *)call;
	MGV   *m   = (MGV *)closure;

	USEUP(w);
	wl_assert(MgvOK(m));

	GhostviewDisableInterpreter(m->ghost);
	XtVaSetValues(m->ghost, GvNantialias, cbs->set, NULL);
	m->defalias = cbs->set;
	mgv_show(m, m->page);
}

/*
 * options->(smart sizing|magic scrolling|smooth scrolling)
 */
void
mgv_options_bits(Widget w, XtPointer closure, XtPointer call)
{
	XmTBS	*cbs   = (XmTBS *)call;
	MGV	*m     = (MGV *)closure;
	MGVR	*r     = mgv_app_res(m->main);
	char	*name  = XtName(w);
	int	bit    = 0;
	int	redraw = FALSE;
	Boolean	*b     = NULL;

	wl_assert(MgvOK(m));

	if(strcmp(name, "magicScroll") == 0)
		bit = MgvMAGICSCROLL, redraw = FALSE, b = &r->magic_scroll;
	else if(strcmp(name, "smoothScroll") == 0)
		bit = MgvSMOOTH, redraw = FALSE, b = &r->smooth_scroll;
	else if(strcmp(name, "smartSizing") == 0)
		bit = MgvSMARTSIZE, redraw = TRUE, b = &r->smart_sizing;
	else
		wl_assert("Unknown entry in mgv_options_bits()!" != NULL);

	if(cbs->set)
		m->bits |=  bit, *b = True;
	else
		m->bits &= ~bit, *b = False;
	if(redraw)
		mgv_show(m, m->page);
}

/*
 * options->suppress warnings
 */
void
mgv_options_warnings(Widget w, XtPointer closure, XtPointer call)
{
	XmTBS *cbs = (XmTBS *)call;
	MGV   *m   = (MGV *)closure;

	USEUP(w);
	wl_assert(MgvOK(m));

	mgv_app_res(m->main)->suppress_warnings = cbs->set;
}
