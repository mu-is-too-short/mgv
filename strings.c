/*
 * strings.c
 *	String handling for mgv (mostly for i18n/l10n stuff which no one
 *	has bothered to do yet).  All of the strings are loaded and cached
 *	on first use.
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
MU_ID("$Mu: mgv/strings.c 1.30 2001/03/14 13:34:53 $")

#include <X11/StringDefs.h>

#include <wlib/wlib.h>
#include <mine/strings.h>
#include <mine/strdefs.h>

static WLS strings[] = {
	{
		MgvNstringBadArgument,
		"%s:  unknown argument '%s' to action"
	}, {
		MgvNstringCantAllocPixmap,
		"backing pixmap could not be allocated"
	}, {
		MgvNstringCantDecompress,
		"Cannot decompress"
	}, {
		MgvNstringCantHideMenu,
		"Menu bar won't let itself be hidden."
	}, {
		MgvNstringCantOpen,
		"Could not open"
	}, {
		MgvNstringCantPrint,
		"Print command failed:"
	}, {
		MgvNstringError,
		"Error"
	}, {
		MgvNstringFileExists,
		"File exists, overwrite?"
	}, {
		MgvNstringInterpDone,
		"Interpreter done."
	}, {
		MgvNstringInterpFailed,
		"GhostScript Interpreter failure."
	}, {
		MgvNstringMessage,
		"Message"
	}, {
		MgvNstringNoCoordinates,
		"(?, ?)"
	}, {
		MgvNstringNoPrintCommand,
		"No print command given"
	}, {
		MgvNstringPageFinished,
		"Page finished."
	}, {
		MgvNstringUnknownSwitch,
		"Unknown switch(es): %signoring."
	}, {
		MgvNstringPS,
		"PostScript (PS)"
	}, {
		MgvNstringEPS,
		"Encapsulated PostScript (EPS)"
	}, {
		MgvNstringPDF,
		"Portable Document Format (PDF)"
	}, {
		MgvNstringUnknownType,
		"Unknown Document Type"
	}, {
		MgvNstringUnknownMedia,
		"Unknown Media"
	}, {
		MgvNstringUnknown,
		"Unknown message."
	}, {
		MgvNstringWarning,
		"Warning"
	}, {
		MgvNstringError,
		"Error"
	}, {
		MgvNstringCantSaveRes,
		"Cannot save resource file ~/.mgv/resources"
	}, {
		MgvNstringContClosing,
		"Continue closing?"
	},
};
#define	N_STRINGS	(int)(sizeof(strings)/sizeof(strings[0]))

void
mgv_strings_init(void)
{
	wl_sadd(strings, N_STRINGS);
}
