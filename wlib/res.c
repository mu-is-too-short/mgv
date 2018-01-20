/*
 * res.c
 *	Resource munging utilities.
 *	of wlib.
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
MU_ID("$Mu: mgv/wlib/res.c 1.2 2001/03/19 19:31:41 $")

#include <string.h>
#include <time.h>
#include <errno.h>
#include <Xm/XmStrDefs.h>

#include <wlib/stdlib.h>
#include <wlib/resource.h>
#include <wlib/build.h>
#include <wlib/wlibP.h>

/*
 * Fascinating what you can learn from the Xt source!
 * We'll just assume that XtGetApplicationResources has already been
 * called on the resource list, I can't think of any really good way
 * to check the status of an XtResource (resource_offset seems like
 * the best thing to look at but I'll just rely on the human).
 */
void
wl_res_decompile(XtResource *r, Cardinal n)
{
#	define	Q2S(s)	XrmQuarkToString((XrmQuark)(s))
	Cardinal i;
	for(i = 0; i < n; ++i, ++r) {
		r->resource_name   = Q2S(r->resource_name);
		r->resource_class  = Q2S(r->resource_class);
		r->resource_type   = Q2S(r->resource_type);
		r->resource_offset = -r->resource_offset - 1;
	}
#	undef	Q2S
}

static char *
fmt_string(char *p)
{
	return XtNewString(*(char **)p);
}

static char *
fmt_int(char *p)
{
	char buf[1024];
	sprintf(buf, "%d", *(int *)p);
	return XtNewString(buf);
}

static char *
fmt_boolean(char *p)
{
	return *(Boolean *)p == True
		? XtNewString("true")
		: XtNewString("false");
}

static char *
fmt_unknown(char *p)
{
	USEUP(p);
	return XtNewString("??????????");
}

typedef struct {
	char  *type;
	WLFMT  fmt;
} CONVERTER;

static CONVERTER *conv;
static int n_conv;

static CONVERTER builtin[] = {
	{XmRString,  fmt_string},
	{XmRInt,     fmt_int},
	{XmRBoolean, fmt_boolean},
};
#define	N_BUILTIN (int)(sizeof(builtin)/sizeof(builtin[0]))

static WLFMT
formatter(char *type)
{
	int i;

	for(i = 0; i < n_conv; ++i)
		if(strcmp(type, conv[i].type) == 0)
			return conv[i].fmt;
	for(i = 0; i < N_BUILTIN; ++i)
		if(strcmp(type, builtin[i].type) == 0)
			return builtin[i].fmt;
	return fmt_unknown;
}

static char *
now(int utc)
{
	time_t    t = time(NULL);
	char     *s = NULL;
	size_t    n;
	char     *fmt;
	size_t    used;
	struct tm tm;

	if(utc) {
		tm  = *gmtime(&t);
		fmt = "%Y-%m-%d %H:%M:%S";
	}
	else {
		tm  = *localtime(&t);
		fmt = "%c";
	}
	n = 2*strlen(fmt);

	do {
		n   *= 2;
		s    = wl_realloc(s, n);
		used = strftime(s, n, fmt, &tm);
	} while(used == 0);

	return s;
}

int
wl_res_open(FILE **fpr, char *name, char **header, int n_header)
{
	int   i;
	FILE *fp;
	char *utc   = NULL;
	char *local = NULL;

	if((fp = fopen(name, "w")) == NULL)
		return errno;

	for(i = 0; i < n_header; ++i)
		fprintf(fp, "!# %s\n", header[i]);
	utc   = now(TRUE);
	local = now(FALSE);
	fprintf(fp, "!# Last written: %s UTC (%s local)\n", utc, local);
	fprintf(fp, "!# App version : %s\n", bld_verstring);
	fprintf(fp, "!#\n");

	wl_free(utc);
	wl_free(local);

	*fpr = fp;

	return 0;
}

FILE *
wl_res_close(FILE *fp)
{
	if(fp != NULL)
		fclose(fp);
	return NULL;
}

void
wl_res_store(FILE *fp, char *cls, char *p, XtPointer b, XtResource *r, int n)
{
	int   i;
	char *v, *s;

	if(cls == NULL)
		cls = "";
	if(p == NULL)
		p = "*";

	for(i = 0; i < n; ++i, ++r) {
		v = (char *)b + r->resource_offset;
		if(*(void **)v == NULL
		&& strcmp(r->resource_type, XmRString) == 0)
			continue;
		s = formatter(r->resource_type)(v);
		fprintf(fp, "%s%s%s: %s\n", cls, p, r->resource_name, s);
		wl_free(s);
	}
}

void
wl_res_addfmt(char *type, WLFMT fmt)
{
	int i;

	for(i = 0; i < n_conv; ++i) {
		if(strcmp(conv[i].type, type) != 0)
			continue;
		conv[i].fmt = fmt;
		return;
	}

	conv = wl_realloc(conv, (n_conv + 1) * sizeof(CONVERTER));
	conv[n_conv].type = XtNewString(type);
	conv[n_conv].fmt  = fmt;
	++n_conv;
}

XrmDatabase
wl_res_load(char *rc)
{
	XrmDatabase db = NULL;

	if(rc != NULL)
		db = XrmGetFileDatabase(rc);
	return db == NULL ? XrmGetStringDatabase("") : db;
}

void
wlp_res_shutdown(void)
{
	int i;

	for(i = 0; i < n_conv; ++i)
		wl_free(conv[i].type);
	conv   = wl_free((XtPointer)conv);
	n_conv = 0;
}

/*
 * XtGetApplicationResources does some funky things to the resource list
 * that you pass in.  Basically, all of the strings are "compiled" into a
 * non-string format that is, presumably, more efficient to work with.  I
 * haven't found any documentation about this but the Xt sources tell me
 * that it is true.  The problem is that I want to use the resource list
 * later on and I prefer to deal with the original string based form so
 * I am left with two options:
 *
 *	- decompile the resource list after Xt has had its way
 *	- wrap XtGetApplicationResources and copy the whole
 *	  resource list in that wrapper
 *
 * The second option is nicer because we don't have to rely on the
 * undocumented behavior of XtGetApplicationResources.  The overhead really
 * isn't a big deal because we only have to perform a shallow copy of the
 * resource list.  The first option is demonstrated in wl_res_decompile.
 *
 * If you're only grabbing a couple of resources then you're better off
 * making your own local copies and calling XtGetApplicationResources
 * directly; if your resource list is on the stack to begin with and you're
 * not dynamically allocating anything, then XtGetApplicationResources should
 * be used directly.
 */
void
wl_res_get(Widget w, void *base, XtResource *r, Cardinal n_r,
							Arg *a, Cardinal n_a)
{
	XtResource *copy = wl_calloc(sizeof(XtResource), n_r);

	memcpy((void *)copy, (void *)r, n_r*sizeof(XtResource));
	XtGetApplicationResources(w, base, copy, n_r, a, n_a);
	wl_free((XtPointer)copy);
}

/*
 * Simple convenience functions for getting a single value from the
 * resource database.
 */
typedef struct {
	Boolean b;
	int     i;
	String  s;
} BLECH;
char *
wl_res_string(Widget w, char *name, char *cls, char *def)
{
	XtResource r = {
		NULL, NULL,
		XmRString, sizeof(String), XtOffsetOf(BLECH, s),
		XmRString, NULL
	};
	BLECH b;
	r.resource_name  = name;
	r.resource_class = cls;
	r.default_addr   = (XtPointer)def;
	XtGetApplicationResources(w, &b, &r, 1, NULL, 0);
	return b.s;
}

int
wl_res_int(Widget w, char *name, char *cls, int def)
{
	XtResource r = {
		NULL, NULL,
		XmRInt, sizeof(int), XtOffsetOf(BLECH, i),
		XmRImmediate, NULL
	};
	BLECH b;
	r.resource_name  = name;
	r.resource_class = cls;
	r.default_addr   = (XtPointer)def;
	XtGetApplicationResources(w, &b, &r, 1, NULL, 0);
	return b.i;
}

Boolean
wl_res_boolean(Widget w, char *name, char *cls, Boolean def)
{
	XtResource r = {
		NULL, NULL,
		XmRBoolean, sizeof(Boolean), XtOffsetOf(BLECH, b),
		XmRImmediate, NULL
	};
	BLECH b;
	r.resource_name  = name;
	r.resource_class = cls;
	r.default_addr   = (XtPointer)(long)def;
	XtGetApplicationResources(w, &b, &r, 1, NULL, 0);
	return b.b;
}
