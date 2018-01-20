/*
 * helpP.h
 *	Internal help system support for wlib.
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
 *   Authors:	Matthew D. Francey
 *		Eric A. Howe (mu@trends.net)
 */
#ifndef	WLIB_HELPP_H
#define	WLIB_HELPP_H
MU_HID(wlib_helpp_h, "$Mu: mgv/wlib/wlib/helpP.h 1.2 2001/03/09 16:22:52 $")

typedef struct {
	String chapter;
	String section;
} WLP_HELP;

typedef Boolean (*WLP_HELPFN)(Widget, WLP_HELP *);
typedef void    (*WLP_HELPSHUT)(void);

CDECLS_BEGIN
extern void wlp_help_html_init(void);
extern void wlp_help_remote_init(void);

extern void wlp_help_init(void);
extern void wlp_help_shutdown(void);

extern void wlp_help_add(char *, WLP_HELPFN, WLP_HELPSHUT);
extern void wlp_help_del(char *);

extern char *wlp_help_browser(Widget);
extern char *wlp_help_prefix(Widget);
extern char *wlp_help_url(Widget, char *, char *);
CDECLS_END

#endif
