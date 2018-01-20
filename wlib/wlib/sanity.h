/*
 * sanity.h
 *	Misc convenience functions/macros for sanity checks.
 *
 * Copyright (C) 1997 Eric A. Howe
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
#ifndef	WLIB_SANITY_H
#define	WLIB_SANITY_H
MU_HID(wlib_sanity_h, "$Mu: mgv/wlib/wlib/sanity.h 1.2 2001/03/17 18:17:04 $")

#include <stdio.h>

/*
 * stderr may be redirected to the UI in some apps so we can't use assert(3).
 * Hence, we use wl_assert() which is the same thing but writes the badness
 * message to stdout.  This is a kludge but at least a reasonable kludge.
 *
 * We only enable assertions of the global wl_enable_assert variable
 * is TRUE.  This variable is controlled by the wlEnableAssertions
 * resource and is initialized by wl_init().  The default value
 * is TRUE so asserts will be tripped before wl_init() is called.
 * You should use the wl_assert_* functions to fiddle with this.
 */
extern int wlp_asserts_enabled;
#if defined(NDEBUG)
#	define	wl_assert(x)	((void)0)
#else
#	if defined(__GNUC__)
#		define	WL_ASSERT_FUNCTION	__PRETTY_FUNCTION__
#	else
#		define	WL_ASSERT_FUNCTION	((void *)0)
#	endif
#	define	wl_assert(x) ((void) (			\
			(wlp_asserts_enabled && !(x))	\
			? wl_abort(#x, __FILE__, __LINE__, WL_ASSERT_FUNCTION) \
			: 0 \
	))
#endif

#define	ARGSANITY(a,n)	wl_assert((size_t)(n) <= sizeof(a)/sizeof((a)[0]))

/*
 * How many stack overruns do we have to see before people smarten up?
 * How many "Arg a[100];" declarations do we have to see before people
 * learn to count?
 */
#define wlGetValues(w,a,n) {		\
	ARGSANITY((a), (n));		\
	XtGetValues((w), (a), (n));	\
}
#define wlSetValues(w,a,n) {		\
	ARGSANITY((a), (n));		\
	XtSetValues((w), (a), (n));	\
}

/*
 * This is handy for assert(some_checks(x)) type things.
 * Note that we write to stdout since stderr is often redirected to
 * the user interface.
 */
#define	DIEIF(x)			\
	if(x) {				\
		printf(#x "\n");	\
		fflush(stdout);		\
		return FALSE;		\
	}

CDECLS_BEGIN
/*
 * This is the same as XFreePixmap() but it can deal with None pixmaps.
 * I got sick of all the silly "if this isn't dead, kill it" code, if
 * something is already dead, then killing it again should be very
 * easy so why should I have to check for life-signs all the time?
 * For convenience, this function always returns "None" so that you can do
 *
 *	p = wl_free_pixmap(dpy, p);
 *
 * instead of nonsense like this:
 *
 *	wl_free_pixmap(dpy, p);
 *	p = None;
 */
extern Pixmap wl_free_pixmap(Display *, Pixmap);

extern int  wl_abort(char *, char *, int, char *);
extern void wl_assert_enable(int);
extern int  wl_asserts_enabled(void);
CDECLS_END

#endif
