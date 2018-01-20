/*
 * mgv.h
 *	Main header for mgv.
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
#ifndef	MGV_MGV_H
#define	MGV_MGV_H
MU_HID(mgv_h, "$Mu: mgv/mine/mgv.h 1.176 2001/03/14 13:34:57 $")

#include <stdio.h>
#include <sys/types.h>
#include <X11/Intrinsic.h>

#include <wlib/sens.h>
#include <mine/Ghostview.h>
#include <mine/dsc.h>
#include <mine/opaque.h>

/*
 * Upper and lower bounds on the magstep menu radio buttons.
 * These should match the magstep menu so you'll have to update
 * the resource file and the menu creation code in mgv_create.c
 * if you change these.
 */
#define	MGV_MAGMAX	+5
#define	MGV_MAGMIN	-5

/*
 * misc flags for MGV.bits
 */
#define	MgvALIASEPS	(0x1 << 0x00)
#define	MgvFORCEDORIENT	(0x1 << 0x01)
#define	MgvMENUDOWN	(0x1 << 0x02)
#define	MgvZOOMDRAWN	(0x1 << 0x03)
#define	MgvMAGICSCROLL	(0x1 << 0x04)
#define	MgvSMOOTH	(0x1 << 0x05)
#define	MgvSMARTSIZE	(0x1 << 0x06)

#define	MgvRESETSCROLL_H	(0x1 << 0x07)
#define	MgvRESETSCROLL_V	(0x1 << 0x08)
#define	MgvRESETSCROLL		(MgvRESETSCROLL_H | MgvRESETSCROLL_V)

/*
 * Things that can be hidden, these bits also go in MGV.bits.
 * These bits are set if the corresponding UI item is hidden.
 */
#define	MgvMENUBAR	(0x1 << 0x09)
#define	MgvSTATUSLINE	(0x1 << 0x0a)
#define	MgvTOOLBAR	(0x1 << 0x0b)
#define	MgvPAGELIST	(0x1 << 0x0c)

/*
 * This structure defines one instance of mgv.  All of the functions
 * exist soley to support and maintain instances of this struct.
 *
 * Pointers to these things get tossed around as void* a lot
 * so we'll go for a little paranoia and attach some magic numbers.
 *
 * Unless otherwise noted, all pointers in these structs are allocated.
 */
#define	MgvMAGIC1	0x4d475600L	/* 'M' 'G' 'V' */
#define	MgvMAGIC2	0x0056474dL	/* 'V' 'G' 'M' */
#define	MgvTZOOM	(1 << 1)
#define	MgvTFULL	(MgvTZOOM | (1 << 2))
typedef struct MGV_s {
	long secret_handshake1;

	int  type;	/* MgvTZOOM or MgvTFULL			*/
	int  serial;	/* just an arbitrary but unique number	*/

	struct MGV_s	*parent;

	/*
	 * Main window components.
	 * We only cache the widgets that get a lot of use, we get less used
	 * parts as needed from wl_find1/XtNameToWidget.
	 */
	Widget main;		/* main form			*/
	Widget sw;		/* the scrolled window		*/
	Widget pagelist;	/* the page list		*/
	Widget menubar;		/* the menu bar			*/
	Widget statusline;	/* the main status line form	*/
	Widget toolbar;		/* the toolbar			*/
	Widget ghost;		/* the Ghostview widget		*/
	Widget orientation;	/* current orientation toggle	*/
	Widget media;		/* current media toggle		*/
	Widget popup;		/* the popup menu		*/
	Widget clip;		/* clip window to sw		*/
	Widget magstep;		/* current magstep toggle	*/
	Widget tracker;		/* the position tracker label	*/

	/*
	 * Dialog cache.
	 * Dialogs are generally created when they are first needed
	 * and then managed/unmanaged for future use.
	 */
	Widget fs_open;		/* file selector dialog (open)	*/
	Widget fs_save;		/* file selector dialog (save)	*/
	Widget save_query;	/* exists? overwrite		*/
	Widget about;		/* about dialog			*/
	Widget printdlg;	/* print dialog			*/
	Widget child_err;	/* child stderr message box	*/
	Widget child_out;	/* child stdout message box	*/
	Widget magstepdlg;	/* set magstep dialog		*/
	Widget docinfo;		/* document info dialog		*/
	Widget mark_range;	/* the range marking dialog	*/

	/*
	 * The open file.
	 * The `realfile' is the name of the file that we're actually
	 * working with, `file' is the one that came from the user;
	 * they will be the same only when the user has specified a plain
	 * PostScript file (i.e. not compressed and not a convertable
	 * PDF file).
	 *
	 * The `fp' will be connected to `realfile'.
	 *
	 * The `dsc' may be NULL so watch out.  The `marked' array will have
	 * `dsc->n_pages' elements or, if `dsc' is NULL, it will be NULL.
	 */
	FILE *fp;
	char *realfile, *file;
	DSC  *dsc;
	int  *marked;

	/*
	 * The `page' field is the page index that is currently on
	 * the screen or being put on the screen, the `pageq' field
	 * is the page queue.  The queue isn't really a queue since
	 * it only has one entry but we only care about the last entry
	 * any way so it doesn't matter.  The `pageq' is used in combination
	 * with a work procedure (see app.c:worker) to avoid filling the
	 * input buffer for gs with a bunch of garbage.  We want to do this
	 * do avoid a backlog when someone presses `PageUp' or `PageDown'
	 * a lot, this trick basically protects us from the rendering lag
	 * of the gs child process.
	 *
	 * The `pageq' will be negative when it is empty.
	 *
	 * The `pagedone' flag is generally set by the message callback
	 * to indicate the `page' has been fully rendered.
	 */
	int page;
	int pageq;
	int pagedone;

	/*
	 * misc
	 */
	int      magnification;
	int      default_xdpi, default_ydpi;
	int      default_media;
	int      n_marked;
	char    *savefile;	/* name we're attempting to save to */
	long     bits;
	Pixmap   icon, mask;
	Boolean  defalias, labels, pgnums;

	/*
	 * Sensitivity things.
	 */
	int  sens;
	WLSN senscache;

	/*
	 * Magic/smooth scrolling support.
	 */
	int   last_magic;
	int   magic_resistance;
	int   smoothness;
	float scrollfactor;

	/*
	 * Page number stack.
	 */
	MGVP pgstack;

	/*
	 * Window sizing function.  This function handles the automatic
	 * size/width/height stuff.  This should never be NULL.
	 */
	void (*autosizer)(struct MGV_s *);

	/*
	 * Page centering/positioning function.
	 * This will never be NULL but it may be a no-op.
	 */
	void (*center)(struct MGV_s *);

	/*
	 * The root_width and root_height values are adjusted to compenstate
	 * for the wmWidth and wmHeight resources; life would be a little
	 * more pleasant if we could ask someone how large the window
	 * decorations are but the required incantations (I have tried
	 * many) are beyond me at the moment.  The root_width and root_height
	 * values are only used to limit the size of the window during
	 * auto-sizing and, technically, that is your window manager's job
	 * so I'm not going to worry too much about it.
	 */
	int root_width, root_height;

	/*
	 * Zooming and panning.  The startx, starty, curx, and cury
	 * fields are used to track the current cursor position by
	 * both the zooming and the panning code; since you can't
	 * do both at the same time, this should be safe.
	 */
	int    startx, starty;
	int    curx,   cury;
	GC     reticule_gc;
	Widget zoom_menu;
	MGVZ   zoomers;
	int    n_zooms;

	/*
	 * Marking context for the mgvMark action.  This will only be
	 * non-null between mgvMark(begin) and mgvMark(end) action calls.
	 */
	MGVMC mc;

	long secret_handshake2;
} MGV;

#define	MgvISZOOM(m)	((m) != NULL && ((m)->type & ~MgvTZOOM) == 0)

#define	MgvOK(m)	mgv_ok((m), FALSE)
#define	MgvZOK(m)	mgv_ok((m), TRUE)

/*
 * this is used as the media parameter to mgv_media() if you
 * want to use the bounding box in the ps file
 */
#define	MgvUSEBBOX	-1

CDECLS_BEGIN
extern MGV  *mgv_alloc(char *);
extern void  mgv_close(MGV *, int, int);
extern MGV  *mgv_create(Widget, char *);
extern MGV  *mgv_free(MGV *);
extern char *mgv_label(MGV *, int);
extern int   mgv_magnify(MGV *, int);
extern void  mgv_mark(MGV *, int, int, int);
extern void  mgv_media(MGV *, int);
extern int   mgv_ok(MGV *, int);
extern int   mgv_open(MGV *, char *);
extern int   mgv_open_stdin(MGV *);
extern void  mgv_orient(MGV *, GvOrient, int);
extern void  mgv_refresh(MGV *);
extern void  mgv_show(MGV *, int);
extern void  mgv_showq(MGV *);
extern MGV  *mgv_top(MGV *);
extern int   mgv_write(MGV *, FILE *, int);
extern MGV  *mgv_zdup(MGV *);
CDECLS_END

#endif
