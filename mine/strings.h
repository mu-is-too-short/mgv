/*
 * strings.h
 *	message string stuff
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
#ifndef	MGV_STRINGS_H
#define	MGV_STRINGS_H
MU_HID(strings_h, "$Mu: mgv/mine/strings.h 1.25 2001/03/14 13:34:57 $")

#include <wlib/strings.h>
#include <mine/strdefs.h>

/*
 * the MgvS<x> entry in this list corresponds to the MgvNstring<x> resource
 */
#define MgvSbadArgument		MgvNstringBadArgument
#define MgvScantAllocPixmap	MgvNstringCantAllocPixmap
#define MgvScantDecompress	MgvNstringCantDecompress
#define MgvScantHideMenu	MgvNstringCantHideMenu
#define MgvScantOpen		MgvNstringCantOpen
#define MgvScantPrint		MgvNstringCantPrint
#define MgvSerror		MgvNstringError
#define MgvSfileExists		MgvNstringFileExists
#define MgvSinterpDone		MgvNstringInterpDone
#define MgvSinterpFailed	MgvNstringInterpFailed
#define MgvSmessage		MgvNstringMessage
#define MgvSnoCoordinates	MgvNstringNoCoordinates
#define MgvSnoPrintCommand	MgvNstringNoPrintCommand
#define MgvSpageFinished	MgvNstringPageFinished
#define MgvSunknownSwitch	MgvNstringUnknownSwitch
#define MgvSPS			MgvNstringPS
#define MgvSEPS			MgvNstringEPS
#define MgvSPDF			MgvNstringPDF
#define MgvSunknownType		MgvNstringUnknownType
#define MgvSunknownMedia	MgvNstringUnknownMedia
#define MgvSunknown		MgvNstringUnknown
#define	MgvSwarning		MgvNstringWarning
#define	MgvSerror		MgvNstringError
#define	MgvScantSaveRes		MgvNstringCantSaveRes
#define	MgvScontClosing		MgvNstringContClosing

CDECLS_BEGIN
extern void mgv_strings_init(void);
CDECLS_END

#endif
