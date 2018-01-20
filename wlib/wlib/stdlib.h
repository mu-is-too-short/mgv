/*
 * stdlib.h
 *	stdlib.h wrapper.
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
#ifndef	WLIB_STDLIB_H
#define	WLIB_STDLIB_H
MU_HID(wlib_stdlib_h, "$Mu: mgv/wlib/wlib/stdlib.h 1.3 2001/03/17 18:17:04 $")

#include <stdlib.h>

#if !HAVE_PUTENV
	/*
	 * I'm just going to assume that you really do have `putenv'
	 * but some kind of insane _POSIX_SOURCE/_XOPEN_SOURCE macro
	 * nonsense in your system headers is making our life hard.
	 */
	extern int putenv(const char *);
#endif

/*
 * I'm just sick of freeing functions without return values.
 * I'm assuming that the Xt alloc functions will die if they
 * can't allocate the necessary memory.  AFAIK, this is "normal"
 * behavior.
 *
 * wl_free() exists for the first reason above, the others are just
 * for symmetry.
 */
#define	wl_free(x)	(XtFree((void *)(x)), NULL)
#define wl_malloc(x)	(void *)XtMalloc(x)
#define wl_realloc(x,y)	(void *)XtRealloc((void *)(x), (y))
#define	wl_calloc(x,y)	(void *)XtCalloc((x), (y))

#endif
