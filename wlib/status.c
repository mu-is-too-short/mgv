/*
 * status.c
 *	Status line callbacks for wlib.
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
 */
#include <wlib/rcs.h>
MU_ID("$Mu: mgv/wlib/status.c 1.1 2001/03/09 04:16:28 $")

#include <string.h>
#include <Xm/Xm.h>

#include <wlib/stdlib.h>
#include <wlib/callbacks.h>
#include <wlib/wlibP.h>
#include <wlib/strdefs.h>

typedef struct {
	XmString   s;
	XtResource r;
} STRING;

/*
 * An empty XmString, this is initialized when wl_status_disarm is
 * first called.
 */
static XmString empty;
static Arg      empty_arg;

typedef struct {
	Widget   w;
	XmString x;
} CACHEE;

static struct {
	CACHEE *c;
	int     n;
} cache;

static int
cache_cmp(const void *p1, const void *p2)
{
	long a = (long)((CACHEE *)p1)->w;
	long b = (long)((CACHEE *)p2)->w;
	if(a < b)
		return -1;
	if(a > b)
		return 1;
	return 0;
}

static XmString
cache_insert(Widget w, XmString x)
{
	int i;

	cache.c = wl_realloc(cache.c, (cache.n + 1) * sizeof(CACHEE));
	for(i = 0; i < cache.n; ++i) {
		if(cache.c[i].w < w) {
			memmove((void *)&cache.c[i + 1], (void *)&cache.c[i],
					(cache.n - i) * sizeof(CACHEE));
			break;
		}
	}
	cache.c[i].w = w;
	cache.c[i].x = x;
	++cache.n;

	return x;
}

static XmString
cache_find(Widget w)
{
	CACHEE t, *e;

	t.w = w;
	e = (CACHEE *)bsearch((void *)&t, (void *)cache.c, cache.n,
						sizeof(CACHEE), cache_cmp);
	return e == NULL ? NULL : e->x;
}

static XmString
cache_get(Widget w)
{
	STRING msg = {
		NULL, {
			XwlNstatusMessage, XwlCStatusMessage,
			XmRXmString, sizeof(XmString), XtOffsetOf(STRING, s),
			XmRString, (XtPointer)""
		}
	};

	if((msg.s = cache_find(w)) != NULL)
		return msg.s;
	XtGetApplicationResources(w, &msg, &msg.r, 1, NULL, 0);
	return cache_insert(w, msg.s);
}

static void
cache_free(void)
{
	wl_free((XtPointer)cache.c);
	memset((void *)&cache, '\0', sizeof(cache));
}

static void
cache_erase(Widget w)
{
	int i;

	for(i = 0; i < cache.n; ++i)
		if(cache.c[i].w == w)
			break;
	if(i == cache.n)
		return;
	if(i < cache.n - 1)
		memmove((void *)&cache.c[i], (void *)&cache.c[i + 1],
					(cache.n - i)*sizeof(CACHEE));
	--cache.n;
	cache.c = wl_realloc(cache.c, cache.n*sizeof(CACHEE));
}

void
wl_status_arm(Widget w, XtPointer closure, XtPointer call)
{
	Arg      a;
	XmString x;
	Widget   status = (Widget)closure;

	USEUP(call);

	x = cache_get(w);
	XtSetArg(a, XmNlabelString, x);
	XtSetValues(status, &a, 1);
}

void
wl_status_disarm(Widget w, XtPointer closure, XtPointer call)
{
	Widget status = (Widget)closure;

	USEUP(w); USEUP(call);

	if(empty == NULL) {
		empty = XmStringCreateLocalized("");
		XtSetArg(empty_arg, XmNlabelString, empty);
	}
	XtSetValues(status, &empty_arg, 1);
}

void
wl_status_erase(Widget w, XtPointer closure, XtPointer call)
{
	USEUP(closure); USEUP(call);
	cache_erase(w);
}

void
wlp_status_shutdown(void)
{
	cache_free();
	if(empty != NULL)
		XmStringFree(empty);
	empty = NULL;
	memset((void *)&empty_arg, '\0', sizeof(empty_arg));
}
