/*
 * sens.c
 *	Widget sensitivity handling for wlib.
 *
 * Copyright (C) 1996  Eric A. Howe
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
MU_ID("$Mu: mgv/wlib/sens.c 1.1 2001/03/09 04:16:28 $")

#include <X11/Intrinsic.h>

#include <wlib/stdlib.h>
#include <wlib/wlib.h>
#include <wlib/sens.h>

typedef struct {
	char  *name;
	Widget w;
	int    primary, secondary;
} SENS;

struct WLSN_s {
	Widget root;
	SENS  *sens;
	int    n_sens;
};

void
wl_sens(WLSN s, int mask)
{
	int	i;
	Boolean	b;

	if(s == NULL)
		return;

	for(i = 0; i < s->n_sens; ++i) {
		if(s->sens[i].w == NULL
		&& (s->sens[i].w = wl_find1(s->root, s->sens[i].name)) == NULL)
			continue;
		b = False;
		if((s->sens[i].primary & mask) == s->sens[i].primary)
			b = True;
		else if(s->sens[i].secondary != 0
		    && (s->sens[i].secondary & mask) == s->sens[i].secondary)
			b = True;
		if(XtIsSensitive(s->sens[i].w) != b)
			XtSetSensitive(s->sens[i].w, b);
	}
}

WLSN
wl_sens_free(WLSN s)
{
	int i;

	if(s == NULL)
		return s;
	for(i = 0; i < s->n_sens; ++i)
		wl_free(s->sens[i].name);
	wl_free((XtPointer)s->sens);
	wl_free((XtPointer)s);
	return NULL;
}

WLSN
wl_sens_create(Widget root, WLSND *sd, int n_sd)
{
	WLSN s;
	int  i;

	if((s = wl_calloc(sizeof(struct WLSN_s), 1)) == NULL
	|| (s->sens = wl_calloc(sizeof(SENS), n_sd)) == NULL)
		return wl_sens_free(s);

	s->root   = wl_top(root);
	s->n_sens = n_sd;

	for(i = 0; i < n_sd; ++i) {
		s->sens[i].name      = XtNewString(sd[i].name);
		s->sens[i].w         = NULL;
		s->sens[i].primary   = sd[i].primary;
		s->sens[i].secondary = sd[i].secondary;
	}

	return s;
}
