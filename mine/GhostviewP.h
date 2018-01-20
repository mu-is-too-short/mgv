/*
 * GhostviewP.h -- Private header file for Ghostview widget.
 * Copyright (C) 1992  Timothy O. Theisen
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
 *   Author: Tim Theisen           Systems Programmer
 * Internet: tim@cs.wisc.edu       Department of Computer Sciences
 *     UUCP: uwvax!tim             University of Wisconsin-Madison
 *    Phone: (608)262-0438         1210 West Dayton Street
 *      FAX: (608)262-9777         Madison, WI   53706
 *
 * See Ghostview.c for additions by mu@trends.net.
 */
#ifndef GHOSTVIEWP_h
#define GHOSTVIEWP_h
MU_HID(ghostviewp_h, "$Mu: mgv/mine/GhostviewP.h 1.27 1998/11/25 16:49:53 $")

#include <mine/Ghostview.h>

typedef struct {
	XtPointer extension;
} GhostviewClassPart;

typedef struct _GhostviewClassRec {
	CoreClassPart      core_class;
	GhostviewClassPart ghostview_class;
} GhostviewClassRec;

extern GhostviewClassRec ghostviewClassRec;

/*
 * structure to describe section of file to send to ghostscript
 */
typedef struct GVFile_s GVFile;
struct GVFile_s {
	FILE    *fp;
	off_t    begin;
	unsigned len;
	Boolean  seek_needed;
	Boolean  close;
	GVFile  *next;
};

typedef struct {
	int       fd;	/* file descriptor, -1 if none	*/
	XtInputId id;
} GV_FD;

typedef struct {
	/**
	 ** resources
	 **/

	Pixel          foreground;
	Cursor         cursor, busy_cursor;
	XtCallbackList notify_callback, message_callback, output_callback;
	String         interpreter, arguments;
	String         filename;
	Boolean        quiet, safer;
	Boolean        use_bpixmap;
	GvOrient       orientation;
	int            xdpi, ydpi;
	int            llx, lly, urx, ury;
	int            left_margin, right_margin;
	int            bottom_margin, top_margin;
	String         gs_locale;
	Boolean        antialias;

	/**
	 ** private state
	 **/

	GC     gc;	/* GC used to clear window	*/
	Window mwin;	/* destination of gs messages	*/

	GVFile   *ps_input;		/* pointer it gs input queue	*/
	char     *input_buffer;		/* pointer to input buffer	*/
	unsigned  bytes_left;		/* bytes left in section	*/
	char     *input_buffer_ptr;	/* pointer into input buffer	*/
	unsigned  buffer_bytes_left;	/* bytes left in buffer		*/

	int   interpreter_pid;	/* pid of gs, -1 if none	*/
	GV_FD input;		/* stdin			*/
	GV_FD output;		/* stdout			*/
	GV_FD error;		/* stderr			*/

	Dimension gs_width, gs_height;

	Boolean disable_start;	/* whether to fork gs		*/
	Boolean busy;		/* Is gs busy drawing?		*/
	Boolean changed;	/* changes since last Setup()	*/

	Atom	ghostview;
	Atom	gv_colors;
	Atom	next, page, done;
} GhostviewPart;

typedef struct _GhostviewRec {
	CorePart      core;
	GhostviewPart ghostview;
} GhostviewRec;

#endif
