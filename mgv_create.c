/*
 * mgv_create.c
 *	Primary widget tree creator for mgv.
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
MU_ID("$Mu: mgv/mgv_create.c 1.212 2001/03/19 09:28:49 $")

#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <X11/Intrinsic.h>
#include <Xm/ToggleB.h>
#include <Xm/AtomMgr.h>
#include <Xm/Protocols.h>

#include <wlib/stdlib.h>
#include <wlib/wlib.h>
#include <wlib/sanity.h>
#include <wlib/icons.h>
#include <wlib/callbacks.h>
#include <wlib/help.h>
#include <wlib/display.h>
#include <mine/mgv.h>
#include <mine/app.h>
#include <mine/file.h>
#include <mine/error.h>
#include <mine/help.h>
#include <mine/log.h>
#include <mine/magstep.h>
#include <mine/media.h>
#include <mine/options.h>
#include <mine/orientation.h>
#include <mine/page.h>
#include <mine/mark.h>
#include <mine/print.h>
#include <mine/sens.h>
#include <mine/track.h>
#include <mine/view.h>
#include <mine/zoom.h>
#include <mine/auto.h>
#include <mine/center.h>
#include <mine/toolbar.h>

#if defined(ABS)
#	undef ABS
#endif
#define ABS(x) ((x) < 0 ? -(x) : (x))

static WLW menu_file[] = {
	{"open",   wlPushB,     WL_STATUS, NULL},
	{"reopen", wlPushB,     WL_STATUS, NULL},
	{"save",   wlPushB,     WL_STATUS, NULL},
	{"print",  wlPushB,     WL_STATUS, NULL},
	{"info",   wlPushB,     WL_STATUS, NULL},
	{"sep",    wlSeparator, 0,         NULL},
	{"close",  wlPushB,     WL_STATUS, NULL},
	{"exit",   wlPushB,     WL_STATUS, NULL},
	{NULL,     NULL,         0,         NULL}
};

static WLW menu_size[] = {
	{"size",   wlToggleB, WL_STATUS, NULL},
	{"height", wlToggleB, WL_STATUS, NULL},
	{"width",  wlToggleB, WL_STATUS, NULL},
	{"none",   wlToggleB, WL_STATUS, NULL},
	{NULL,     NULL,      0,         NULL}
};

static WLW menu_page[] = {
	{"next",       wlPushB,     WL_STATUS, NULL},
	{"prev",       wlPushB,     WL_STATUS, NULL},
	{"sep1",       wlSeparator, 0,         NULL},
	{"center",     wlToggleB,   WL_STATUS, NULL},
	{"redisplay",  wlPushB,     WL_STATUS, NULL},
	{"sep2",       wlSeparator, 0,         NULL},
	{"showLabels", wlToggleB,   WL_STATUS, NULL},
	{"showPgNums", wlToggleB,   WL_STATUS, NULL},
	{"sep3",       wlSeparator, 0,         NULL},
	{"size",       wlCascadeB,  0,         menu_size},
	{NULL,         NULL,        0,         NULL}
};

static WLW menu_mark[] = {
	{"mark",      wlPushB,     WL_STATUS, NULL},
	{"markAll",   wlPushB,     WL_STATUS, NULL},
	{"markNone",  wlPushB,     WL_STATUS, NULL},
	{"even",      wlPushB,     WL_STATUS, NULL},
	{"odd",       wlPushB,     WL_STATUS, NULL},
	{"invert",    wlPushB,     WL_STATUS, NULL},
	{"range",     wlPushB,     WL_STATUS, NULL},
	{NULL,        NULL,        0,         NULL}
};

static WLW menu_magstep[] = {
	{"decrease", wlPushB,     WL_STATUS, NULL},
	{"-5",       wlToggleB,   WL_STATUS, NULL},
	{"-4",       wlToggleB,   WL_STATUS, NULL},
	{"-3",       wlToggleB,   WL_STATUS, NULL},
	{"-2",       wlToggleB,   WL_STATUS, NULL},
	{"-1",       wlToggleB,   WL_STATUS, NULL},
	{"0",        wlToggleB,   WL_STATUS, NULL},
	{"1",        wlToggleB,   WL_STATUS, NULL},
	{"2",        wlToggleB,   WL_STATUS, NULL},
	{"3",        wlToggleB,   WL_STATUS, NULL},
	{"4",        wlToggleB,   WL_STATUS, NULL},
	{"5",        wlToggleB,   WL_STATUS, NULL},
	{"increase", wlPushB,     WL_STATUS, NULL},
	{"sep",      wlSeparator, 0,         NULL},
	{"set",      wlPushB,     WL_STATUS, NULL},
	{NULL,       NULL,        0,         NULL}
};

static WLW menu_orientation[] = {
	{"portrait",   wlToggleB, WL_STATUS, NULL},
	{"landscape",  wlToggleB, WL_STATUS, NULL},
	{"upsideDown", wlToggleB, WL_STATUS, NULL},
	{"seascape",   wlToggleB, WL_STATUS, NULL},
	{NULL,         NULL,      0,         NULL}
};


static WLW menu_media_us[] = {
	{"letter",      wlToggleB, WL_STATUS, NULL},
	{"lettersmall", wlToggleB, WL_STATUS, NULL},
	{"halfletter",  wlToggleB, WL_STATUS, NULL},
	{"tabloid",     wlToggleB, WL_STATUS, NULL},
	{"ledger",      wlToggleB, WL_STATUS, NULL},
	{"legal",       wlToggleB, WL_STATUS, NULL},
	{"statement",   wlToggleB, WL_STATUS, NULL},
	{"executive",   wlToggleB, WL_STATUS, NULL},
	{"foolscap",    wlToggleB, WL_STATUS, NULL},
	{"folio",       wlToggleB, WL_STATUS, NULL},
	{"quarto",      wlToggleB, WL_STATUS, NULL},
	{"10x14",       wlToggleB, WL_STATUS, NULL},
	{"11x17",       wlToggleB, WL_STATUS, NULL},
	{"archA",       wlToggleB, WL_STATUS, NULL},
	{"archB",       wlToggleB, WL_STATUS, NULL},
	{"archC",       wlToggleB, WL_STATUS, NULL},
	{"archD",       wlToggleB, WL_STATUS, NULL},
	{"archE",       wlToggleB, WL_STATUS, NULL},
	{NULL,          NULL,      0,         NULL}
};
static WLW menu_media_a[] = {
	{"a0",          wlToggleB, WL_STATUS, NULL},
	{"a1",          wlToggleB, WL_STATUS, NULL},
	{"a2",          wlToggleB, WL_STATUS, NULL},
	{"a3",          wlToggleB, WL_STATUS, NULL},
	{"a4",          wlToggleB, WL_STATUS, NULL},
	{"a4small",     wlToggleB, WL_STATUS, NULL},
	{"a5",          wlToggleB, WL_STATUS, NULL},
	{"a6",          wlToggleB, WL_STATUS, NULL},
	{"a7",          wlToggleB, WL_STATUS, NULL},
	{"a8",          wlToggleB, WL_STATUS, NULL},
	{"a9",          wlToggleB, WL_STATUS, NULL},
	{"a10",         wlToggleB, WL_STATUS, NULL},
	{NULL,          NULL,      0,         NULL}
};
static WLW menu_media_b[] = {
	{"b0",          wlToggleB, WL_STATUS, NULL},
	{"b1",          wlToggleB, WL_STATUS, NULL},
	{"b2",          wlToggleB, WL_STATUS, NULL},
	{"b3",          wlToggleB, WL_STATUS, NULL},
	{"b4",          wlToggleB, WL_STATUS, NULL},
	{"b5",          wlToggleB, WL_STATUS, NULL},
	{NULL,          NULL,      0,         NULL}
};
static WLW menu_media_c[] = {
	{"c0",          wlToggleB, WL_STATUS, NULL},
	{"c1",          wlToggleB, WL_STATUS, NULL},
	{"c2",          wlToggleB, WL_STATUS, NULL},
	{"c3",          wlToggleB, WL_STATUS, NULL},
	{"c4",          wlToggleB, WL_STATUS, NULL},
	{"c5",          wlToggleB, WL_STATUS, NULL},
	{"c6",          wlToggleB, WL_STATUS, NULL},
	{NULL,          NULL,      0,         NULL}
};

static WLW menu_media[] = {
	{"mediaUS",     wlCascadeB, 0,         menu_media_us},
	{"mediaA",      wlCascadeB, 0,         menu_media_a},
	{"mediaB",      wlCascadeB, 0,         menu_media_b},
	{"mediaC",      wlCascadeB, 0,         menu_media_c},
	{NULL,          NULL,       0,         NULL}
};

static WLW menu_options[] = {
	{"antiAlias",    wlToggleB, WL_STATUS, NULL},
	{"magicScroll",  wlToggleB, WL_STATUS, NULL},
	{"smoothScroll", wlToggleB, WL_STATUS, NULL},
	{"smartSizing",  wlToggleB, WL_STATUS, NULL},
	{"warnings",     wlToggleB, WL_STATUS, NULL},
	{NULL,           NULL,      0,         NULL}
};

/* almost same as the popup menu */
static WLW menu_view[] = {
	{"hideLog",     wlToggleB,   WL_STATUS, NULL},
	{"hideList",    wlToggleB,   WL_STATUS, NULL},
	{"hideStatus",  wlToggleB,   WL_STATUS, NULL},
	{"hideMenu",    wlToggleB,   WL_STATUS, NULL},
	{"hideToolBar", wlToggleB,   WL_STATUS, NULL},
	{"sep",         wlSeparator, 0,         NULL},
	{"closeZoom",   wlPushB,     WL_STATUS, NULL},
	{NULL,          NULL,        0,         NULL}
};

static WLW menu_help[] = {
	{"contents", wlPushB,     WL_STATUS, NULL},
	{"context",  wlPushB,     WL_STATUS, NULL},
	{"appres",   wlPushB,     WL_STATUS, NULL},
	{"cmdline",  wlPushB,     WL_STATUS, NULL},
	{"sep",      wlSeparator, 0,         NULL},
	{"changelog",wlPushB,     WL_STATUS, NULL},
	{"about",    wlPushB,     WL_STATUS, NULL},
	{NULL,       NULL,        0,         NULL}
};

static WLW menu_bar[] = {
	{"file",        wlCascadeB, 0, menu_file},
	{"page",        wlCascadeB, 0, menu_page},
	{"view",        wlCascadeB, 0, menu_view},
	{"mark",        wlCascadeB, 0, menu_mark},
	{"magstep",     wlCascadeB, 0, menu_magstep},
	{"orientation", wlCascadeB, 0, menu_orientation},
	{"media",       wlCascadeB, 0, menu_media},
	{"options",     wlCascadeB, 0, menu_options},
	{"help",        wlCascadeB, 0, menu_help},
	{NULL,          NULL,       0,         NULL}
};

static WLW view_window[] = {
	{"mgv", "Ghostview", 0, NULL},
	{NULL,  NULL,        0, NULL}
};

static WLW menu_zoom[] = {
	{"15", wlPushB, 0, NULL},
	{"20", wlPushB, 0, NULL},
	{"30", wlPushB, 0, NULL},
	{"40", wlPushB, 0, NULL},
	{"50", wlPushB, 0, NULL},
	{NULL, NULL,    0, NULL}
};

static WLW magstep_form[] = {
	{"value", wlPushB,     0,           NULL},
	{"text",  wlTextField, WL_UNMANAGE, NULL},
	{"label", wlLabel,     0,           NULL},
	{NULL,    NULL,        0,           NULL}
};
static WLW magstep_frame[] = {
	{"form", wlForm, WL_MANAGE, magstep_form},
	{NULL,   NULL,   0,         NULL}
};

static WLW tracker_frame[] = {
	{"tracker", wlLabel, 0, NULL},
	{NULL,      NULL,    0, NULL}
};

static WLW status_frame[] = {
	{"wlStatusLabel", wlLabel, 0, NULL},
	{NULL,            NULL,    0, NULL}
};

static WLW status_form[] = {
	{"magstepFrame", wlFrame, 0, magstep_frame},
	{"trackerFrame", wlFrame, 0, tracker_frame},
	{"statusFrame",  wlFrame, 0, status_frame},
	{NULL,           NULL,    0, NULL}
};

static WLW tool_bar[] = {
	{"fileOpen",    wlPushB, WL_STATUS, NULL},
	{"filePrint",   wlPushB, WL_STATUS, NULL},

	{"mark",        wlPushB, WL_STATUS, NULL},
	{"markAll",     wlPushB, WL_STATUS, NULL},
	{"markNone",    wlPushB, WL_STATUS, NULL},
	{"markInvert",  wlPushB, WL_STATUS, NULL},
	{"markEven",    wlPushB, WL_STATUS, NULL},
	{"markOdd",     wlPushB, WL_STATUS, NULL},

	{"magstepIncr", wlPushB, WL_STATUS, NULL},
	{"magstepDecr", wlPushB, WL_STATUS, NULL},

	{"helpContext", wlPushB, WL_STATUS, NULL},

	{"pagePrev",    wlPushB, WL_STATUS, NULL},
	{"pageNext",    wlPushB, WL_STATUS, NULL},

	{NULL,          NULL,    0,         NULL}
};

static WLW main_window[] = {
	{"statusLine", wlForm,         WL_MANAGE, status_form},
	{"menuBar",    wlMenuBar,      WL_MANAGE, menu_bar},
	{"toolBar",    "WlToolBar",    WL_MANAGE, tool_bar},
	{"list",       wlScrolledList, WL_MANAGE, NULL},
	{"view",       wlScrolledW,    0,         view_window},
	{"zoomMenu",   wlPopupMenu,    0,         menu_zoom},
	{NULL,         NULL,           0,         NULL}
};

static WLW main_widget[] = {
	{"mainWindow", wlForm, 0, main_window},
	{NULL,         NULL,   0, NULL}
};

/*
 * Almost the same as menu_view[].
 */
static WLW menu_popup_kids[] = {
	{"back",        wlPushB,     WL_STATUS, NULL},
	{"forward",     wlPushB,     WL_STATUS, NULL},
	{"sep1",        wlSeparator, 0,         NULL},
	{"hideLog",     wlToggleB,   WL_STATUS, NULL},
	{"hideList",    wlToggleB,   WL_STATUS, NULL},
	{"hideStatus",  wlToggleB,   WL_STATUS, NULL},
	{"hideMenu",    wlToggleB,   WL_STATUS, NULL},
	{"hideToolBar", wlToggleB,   WL_STATUS, NULL},
	{"sep2",        wlSeparator, 0,         NULL},
	{"closeZoom",   wlPushB,     WL_STATUS, NULL},
	{NULL,          NULL,        0,         NULL}
};

static WLW menu_popup[] = {
	{"popup", wlPopupMenu, 0, menu_popup_kids},
	{NULL,    NULL,        0, NULL}
};

static WLC cb[] = {
	{"*menuBar*file_pd*open", XmNactivateCallback,
		NULL, mgv_file_open},
	{"*toolBar*fileOpen", XmNactivateCallback,
		NULL, mgv_file_open},
	{"*menuBar*file_pd*reopen", XmNactivateCallback,
		NULL, mgv_file_reopen},
	{"*menuBar*file_pd*save", XmNactivateCallback,
		NULL, mgv_file_save},
	{"*menuBar*file_pd*print", XmNactivateCallback,
		NULL, mgv_print},
	{"*toolBar*filePrint", XmNactivateCallback,
		NULL, mgv_print},
	{"*menuBar*file_pd*info", XmNactivateCallback,
		NULL, mgv_file_info},
	{"*menuBar*file_pd*close", XmNactivateCallback,
		NULL, mgv_app_close},
	{"*menuBar*file_pd*exit", XmNactivateCallback,
		NULL, mgv_app_exit},

	{"*menuBar*page_pd*next", XmNactivateCallback,
		NULL, mgv_page_next},
	{"*toolBar*pageNext", XmNactivateCallback,
		NULL, mgv_page_next},
	{"*menuBar*page_pd*prev", XmNactivateCallback,
		NULL, mgv_page_prev},
	{"*toolBar*pagePrev", XmNactivateCallback,
		NULL, mgv_page_prev},
	{"*menuBar*page_pd*center", XmNvalueChangedCallback,
		NULL, mgv_page_center},
	{"*menuBar*page_pd*redisplay", XmNactivateCallback,
		NULL, mgv_page_redisplay},
	{"*menuBar*page_pd*showLabels", XmNvalueChangedCallback,
		NULL, mgv_page_labels},
	{"*menuBar*page_pd*showPgNums", XmNvalueChangedCallback,
		NULL, mgv_page_labels},

	{"*menuBar*size_pd*size", XmNvalueChangedCallback,
		NULL, mgv_page_size},
	{"*menuBar*size_pd*height", XmNvalueChangedCallback,
		NULL, mgv_page_size},
	{"*menuBar*size_pd*width", XmNvalueChangedCallback,
		NULL, mgv_page_size},
	{"*menuBar*size_pd*none", XmNvalueChangedCallback,
		NULL, mgv_page_size},

	{"*menuBar*mark_pd*mark", XmNactivateCallback,
		NULL, mgv_mark_mark},
	{"*toolBar*mark", XmNactivateCallback,
		NULL, mgv_mark_mark},
	{"*menuBar*mark_pd*markAll", XmNactivateCallback,
		NULL, mgv_mark_markall},
	{"*toolBar*markAll", XmNactivateCallback,
		NULL, mgv_mark_markall},
	{"*menuBar*mark_pd*markNone", XmNactivateCallback,
		NULL, mgv_mark_unmarkall},
	{"*toolBar*markNone", XmNactivateCallback,
		NULL, mgv_mark_unmarkall},
	{"*menuBar*mark_pd*even", XmNactivateCallback,
		NULL, mgv_mark_even},
	{"*toolBar*markEven", XmNactivateCallback,
		NULL, mgv_mark_even},
	{"*menuBar*mark_pd*odd", XmNactivateCallback,
		NULL, mgv_mark_odd},
	{"*toolBar*markOdd", XmNactivateCallback,
		NULL, mgv_mark_odd},
	{"*menuBar*mark_pd*invert", XmNactivateCallback,
		NULL, mgv_mark_invert},
	{"*toolBar*markInvert", XmNactivateCallback,
		NULL, mgv_mark_invert},
	{"*menuBar*mark_pd*range", XmNactivateCallback,
		NULL, mgv_mark_range},

	{"*menuBar*magstep_pd*decrease", XmNactivateCallback,
		NULL, mgv_magstep_decrease},
	{"*toolBar*magstepDecr", XmNactivateCallback,
		NULL, mgv_magstep_decrease},
	{"*menuBar*magstep_pd*-5", XmNvalueChangedCallback,
		NULL, mgv_magstep_n},
	{"*menuBar*magstep_pd*-4", XmNvalueChangedCallback,
		NULL, mgv_magstep_n},
	{"*menuBar*magstep_pd*-3", XmNvalueChangedCallback,
		NULL, mgv_magstep_n},
	{"*menuBar*magstep_pd*-2", XmNvalueChangedCallback,
		NULL, mgv_magstep_n},
	{"*menuBar*magstep_pd*-1", XmNvalueChangedCallback,
		NULL, mgv_magstep_n},
	{"*menuBar*magstep_pd*0", XmNvalueChangedCallback,
		NULL, mgv_magstep_n},
	{"*menuBar*magstep_pd*1", XmNvalueChangedCallback,
		NULL, mgv_magstep_n},
	{"*menuBar*magstep_pd*2", XmNvalueChangedCallback,
		NULL, mgv_magstep_n},
	{"*menuBar*magstep_pd*3", XmNvalueChangedCallback,
		NULL, mgv_magstep_n},
	{"*menuBar*magstep_pd*4", XmNvalueChangedCallback,
		NULL, mgv_magstep_n},
	{"*menuBar*magstep_pd*5", XmNvalueChangedCallback,
		NULL, mgv_magstep_n},
	{"*menuBar*magstep_pd*increase", XmNactivateCallback,
		NULL, mgv_magstep_increase},
	{"*toolBar*magstepIncr", XmNactivateCallback,
		NULL, mgv_magstep_increase},
	{"*menuBar*magstep_pd*set", XmNactivateCallback,
		NULL, mgv_magstep_set},

	{"*menuBar*orientation_pd*portrait", XmNvalueChangedCallback,
		NULL, mgv_orientation},
	{"*menuBar*orientation_pd*landscape", XmNvalueChangedCallback,
		NULL, mgv_orientation},
	{"*menuBar*orientation_pd*upsideDown", XmNvalueChangedCallback,
		NULL, mgv_orientation},
	{"*menuBar*orientation_pd*seascape", XmNvalueChangedCallback,
		NULL, mgv_orientation},

	{"*menuBar*media_pd*letter", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*lettersmall", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*halfletter", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*tabloid", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*ledger", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*legal", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*statement", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*executive", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*foolscap", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*folio", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*quarto", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*10x14", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*11x17", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*archA", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*archB", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*archC", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*archD", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*archE", XmNvalueChangedCallback,
		NULL, mgv_media_cb},

	{"*menuBar*media_pd*a0", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*a1", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*a2", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*a3", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*a4", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*a4small", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*a5", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*a6", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*a7", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*a8", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*a9", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*a10", XmNvalueChangedCallback,
		NULL, mgv_media_cb},

	{"*menuBar*media_pd*b0", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*b1", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*b2", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*b3", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*b4", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*b5", XmNvalueChangedCallback,
		NULL, mgv_media_cb},

	{"*menuBar*media_pd*c0", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*c1", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*c2", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*c3", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*c4", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*c5", XmNvalueChangedCallback,
		NULL, mgv_media_cb},
	{"*menuBar*media_pd*c6", XmNvalueChangedCallback,
		NULL, mgv_media_cb},

	{"*menuBar*options_pd*antiAlias", XmNvalueChangedCallback,
		NULL, mgv_options_antialias},
	{"*menuBar*options_pd*magicScroll", XmNvalueChangedCallback,
		NULL, mgv_options_bits},
	{"*menuBar*options_pd*smoothScroll", XmNvalueChangedCallback,
		NULL, mgv_options_bits},
	{"*menuBar*options_pd*smartSizing", XmNvalueChangedCallback,
		NULL, mgv_options_bits},
	{"*menuBar*options_pd*warnings", XmNvalueChangedCallback,
		NULL, mgv_options_warnings},

	{"*menuBar*help_pd*contents", XmNactivateCallback,
		NULL, wl_help},
	{"*menuBar*help_pd*context", XmNactivateCallback,
		NULL, wl_help_context},
	{"*toolBar*helpContext", XmNactivateCallback,
		NULL, wl_help_context},
	{"*menuBar*help_pd*appres", XmNactivateCallback,
		NULL, wl_help},
	{"*menuBar*help_pd*cmdline", XmNactivateCallback,
		NULL, wl_help},
	{"*menuBar*help_pd*changelog", XmNactivateCallback,
		NULL, wl_help},
	{"*menuBar*help_pd*about", XmNactivateCallback,
		NULL, mgv_help_about},

	/*
	 * The view and popup menus attach callbacks to each other
	 * for synchronization; the closure argument should be the
	 * _other_ widget name (with path).
	 */

	{"*menuBar*view_pd*hideLog", XmNvalueChangedCallback,
		NULL, mgv_view_log},
	{"*menuBar*view_pd*hideList", XmNvalueChangedCallback,
		NULL, mgv_view_page},
	{"*menuBar*view_pd*hideList", XmNvalueChangedCallback,
		"*popup*hideList", wl_toggle_sync},
	{"*menuBar*view_pd*hideStatus", XmNvalueChangedCallback,
		NULL, mgv_view_status},
	{"*menuBar*view_pd*hideStatus", XmNvalueChangedCallback,
		"*popup*hideStatus", wl_toggle_sync},
	{"*menuBar*view_pd*hideMenu", XmNvalueChangedCallback,
		NULL, mgv_view_menu},
	{"*menuBar*view_pd*hideMenu", XmNvalueChangedCallback,
		"*popup*hideMenu", wl_toggle_sync},
	{"*menuBar*view_pd*hideToolBar", XmNvalueChangedCallback,
		NULL, mgv_view_toolbar},
	{"*menuBar*view_pd*hideToolBar", XmNvalueChangedCallback,
		"*popup*hideToolBar", wl_toggle_sync},
	{"*menuBar*view_pd*closeZoom", XmNactivateCallback,
		NULL, mgv_view_zoom},

	{"*popup*back", XmNactivateCallback,
		NULL, mgv_page_backforward},
	{"*popup*forward", XmNactivateCallback,
		NULL, mgv_page_backforward},
	{"*popup*hideLog", XmNvalueChangedCallback,
		NULL, mgv_view_log},
	{"*popup*hideList", XmNvalueChangedCallback,
		NULL, mgv_view_page},
	{"*popup*hideList", XmNvalueChangedCallback,
		"*menuBar*view_pd*hideList", wl_toggle_sync},
	{"*popup*hideStatus", XmNvalueChangedCallback,
		NULL, mgv_view_status},
	{"*popup*hideStatus", XmNvalueChangedCallback,
		"*menuBar*view_pd*hideStatus", wl_toggle_sync},
	{"*popup*hideMenu", XmNvalueChangedCallback,
		NULL, mgv_view_menu},
	{"*popup*hideMenu", XmNvalueChangedCallback,
		"*menuBar*view_pd*hideMenu", wl_toggle_sync},
	{"*popup*hideToolBar", XmNvalueChangedCallback,
		NULL, mgv_view_toolbar},
	{"*popup*hideToolBar", XmNvalueChangedCallback,
		"*menuBar*view_pd*hideToolBar", wl_toggle_sync},
	{"*popup*closeZoom", XmNactivateCallback,
		NULL, mgv_view_zoom},

	{"*view*mgv", GvNoutputCallback,	NULL, mgv_log_output},
	{"*view*mgv", GvNmessageCallback,	NULL, mgv_log_message},
	{"*view*mgv", GvNnotifyCallback,	NULL, mgv_track},

	{"*list", XmNbrowseSelectionCallback,	NULL, mgv_page_goto},

	{"*zoomMenu*15", XmNactivateCallback,	NULL, mgv_zoom},
	{"*zoomMenu*20", XmNactivateCallback,	NULL, mgv_zoom},
	{"*zoomMenu*30", XmNactivateCallback,	NULL, mgv_zoom},
	{"*zoomMenu*40", XmNactivateCallback,	NULL, mgv_zoom},
	{"*zoomMenu*50", XmNactivateCallback,	NULL, mgv_zoom},
	{"*zoomMenu",    XmNunmapCallback,	NULL, mgv_zoom_unmap},

	{"*mainWindow", XmNhelpCallback,	NULL, wl_help},

	{"*statusLine*magstepFrame*value", XmNactivateCallback,
		NULL, mgv_magstep_edit},
	{"*statusLine*magstepFrame*text", XmNactivateCallback,
		NULL, mgv_magstep_text},
	{"*statusLine*magstepFrame*text", XmNlosingFocusCallback,
		NULL, mgv_magstep_text},

	{NULL, NULL, NULL, NULL}
};

static WLSND sens[] = {
	{"*menuBar*file_pd*reopen",	MgvSOPEN | MgvSCANREOPEN, 0},
	{"*menuBar*file_pd*print",	MgvSOPEN, 0},
	{"*toolBar*filePrint",		MgvSOPEN, 0},
	{"*menuBar*file_pd*save",	MgvSOPEN, 0},
	{"*menuBar*file_pd*info",	MgvSOPENDOC, 0},
	{"*fileSave*saveUnmark",	MgvSMARKED, 0},
	{"*fileSave*saveMarked",	MgvSMARKED, 0},
	{"*menuBar*page_pd*prev",	MgvSOPENDOC | MgvSNOTFIRST, 0},
	{"*toolBar*pagePrev",		MgvSOPENDOC | MgvSNOTFIRST, 0},
	{"*menuBar*page_pd*next",	MgvSOPENDOC | MgvSNOTLAST,
					MgvSOPEN    | MgvSNOTLAST},
	{"*toolBar*pageNext",		MgvSOPENDOC | MgvSNOTLAST,
					MgvSOPEN    | MgvSNOTLAST},
	{"*menuBar*page_pd*center",	MgvSOPEN, 0},
	{"*menuBar*page_pd*redisplay",	MgvSOPENDOC, 0},
	{"*menuBar*page_pd*showLabels",	MgvSOPENDOC, 0},
	{"*menuBar*page_pd*showPgNums",	MgvSOPENDOC, 0},
	{"*menuBar*mark",		MgvSOPENDOC, 0},
	{"*menuBar*mark_pd*mark",	MgvSOPENDOC | MgvSPAGES, 0},
	{"*toolBar*mark",		MgvSOPENDOC | MgvSPAGES, 0},
	{"*menuBar*mark_pd*markAll",	MgvSOPENDOC | MgvSPAGES, 0},
	{"*toolBar*markAll",		MgvSOPENDOC | MgvSPAGES, 0},
	{"*menuBar*mark_pd*markNone",	MgvSOPENDOC | MgvSPAGES | MgvSMARKED, 0},
	{"*toolBar*markNone",		MgvSOPENDOC | MgvSPAGES | MgvSMARKED, 0},
	{"*menuBar*mark_pd*even",	MgvSOPENDOC | MgvSPAGES, 0},
	{"*toolBar*markEven",		MgvSOPENDOC | MgvSPAGES, 0},
	{"*menuBar*mark_pd*odd",	MgvSOPENDOC | MgvSPAGES, 0},
	{"*toolBar*markOdd",		MgvSOPENDOC | MgvSPAGES, 0},
	{"*menuBar*mark_pd*invert",	MgvSOPENDOC | MgvSPAGES, 0},
	{"*toolBar*markInvert",		MgvSOPENDOC | MgvSPAGES, 0},
	{"*toolBar*markPages",		MgvSOPENDOC | MgvSPAGES, 0},
	{"*menuBar*mark_pd*markPages",	MgvSOPENDOC | MgvSPAGES, 0},
	{"*menuBar*mark_pd*range",	MgvSOPENDOC | MgvSPAGES, 0},
	{"*menuBar*magstep",		MgvSOPEN, 0},
	{"*toolBar*magstepIncr",	MgvSOPEN, 0},
	{"*toolBar*magstepDecr",	MgvSOPEN, 0},
	{"*menuBar*orientation",	MgvSOPEN, 0},
	{"*menuBar*media",		MgvSOPEN, 0},
	{"*printDialog*markedOnly",	MgvSMARKED, 0},
	{"*printDialog*unmark",		MgvSMARKED, 0},
	{"*listSW", 			MgvSPAGES, 0},
	{"*menuBar*view_pd*closeZoom",	MgvSGOTZOOMS, 0},
	{"*popup*back",			MgvSCANBACK    | MgvSOPENDOC, 0},
	{"*popup*forward",		MgvSCANFORWARD | MgvSOPENDOC, 0},
	{"*popup*closeZoom",		MgvSGOTZOOMS, 0},
};
#define	N_SENS	(sizeof(sens)/sizeof(sens[0]))

/*
 * initialize various toggles
 */
#define	TOGGLE(top,name,bit) \
	if(bit) XmToggleButtonSetState(wl_find1(top, name), True, True);
#define	TOG_PGLIST	(1 << 0)
#define	TOG_MENU	(1 << 1)
#define	TOG_STAT	(1 << 2)
#define	TOG_MAGIC	(1 << 3)
#define	TOG_LOG		(1 << 4)
#define	TOG_SMOOTH	(1 << 5)
#define	TOG_TOOLBAR	(1 << 6)
static void
frobnicate(Widget mainwin, int mask)
{
	Widget	top  = wl_top(mainwin);

	wl_assert(XtWindow(wl_find1(mainwin, "*menuBar")) != None);
	TOGGLE(top, "*popup*hideLog",		mask & TOG_LOG);
	TOGGLE(top, "*popup*hideList",		mask & TOG_PGLIST);
	TOGGLE(top, "*popup*hideMenu",		mask & TOG_MENU);
	TOGGLE(top, "*popup*hideStatus",	mask & TOG_STAT);
	TOGGLE(top, "*popup*hideToolBar",	mask & TOG_TOOLBAR);
	TOGGLE(top, "*options_pd*magicScroll",	mask & TOG_MAGIC);
	TOGGLE(top, "*options_pd*smoothScroll",	mask & TOG_SMOOTH);
}
#undef HIDE

MGV *
mgv_create(Widget parent, char *file)
{
	Widget	help, orientation;
	MGVR	*r;
	MGV	*m;
	Atom	deletewin;
	int	toggle, status;
	char	*sizer;
	Screen	*screen = DefaultScreenOfDisplay(wl_display(parent));
	GvOrient orient = GvOrientPortrait;

	r = mgv_app_res(parent);
	if((m = mgv_alloc(NULL)) == NULL)
		return NULL;
	m->type = MgvTFULL;

	/*
	 * store the relevant application resources and other low
	 * level things
	 */
	toggle = 0;
	m->default_media    = r->default_page_media;
	m->parent           = NULL;
	m->labels           = r->show_labels;
	m->pgnums           = r->show_pgnums;
	m->smoothness       = r->smoothness;
	m->magic_resistance = r->magic_resistance;
	m->scrollfactor     = r->scroll_percentage / 1000.0;
	m->bits            |= r->reset_scroll;
	if(r->antialias_eps)	m->bits |= MgvALIASEPS;
	if(r->smart_sizing)	m->bits |= MgvSMARTSIZE;
	if(r->magic_scroll)	m->bits |= MgvMAGICSCROLL, toggle |= TOG_MAGIC;
	if(r->smooth_scroll)	m->bits |= MgvSMOOTH,      toggle |= TOG_SMOOTH;
	if(r->hide_pglist)	toggle |= TOG_PGLIST;
	if(r->hide_menu)	toggle |= TOG_MENU;
	if(r->hide_status)	toggle |= TOG_STAT;
	if(r->hide_toolbar)	toggle |= TOG_TOOLBAR;
	if(r->hide_log)		toggle |= TOG_LOG;

	if(!m->labels && !m->pgnums)
		toggle |= TOG_PGLIST;

	/*
	 * build the widget tree
	 */
	m->main  = wl_create(parent, main_widget);
	m->popup = wl_create(m->main, menu_popup);
	if(mgv_app_add(m) == NULL) {
		mgv_free(m);
		return NULL;
	}
	wl_callback(parent, &cb[0], (XtPointer)m);
	XtVaSetValues(m->main, XmNuserData, (XtPointer)m, NULL);
	wl_find(m->main,
		&m->sw,		"view",
		&m->ghost,	"view*mgv",
		&m->pagelist,	"*list",
		&m->zoom_menu,	"*zoomMenu",
		&m->menubar,	"menuBar",
		&m->toolbar,	"toolBar",
		&m->statusline,	"statusLine",
		&m->tracker,    "*statusLine*tracker",
		&help,		"menuBar.help",
		NULL);
	XtVaGetValues(m->sw,      XmNclipWindow,     &m->clip, NULL);
	XtVaSetValues(m->menubar, XmNmenuHelpWidget, help,     NULL);

	mgv_toolbar_init(m->toolbar);

	/*
	 * defaults
	 */
	m->defalias     = GhostviewGetAntiAlias(m->ghost);
	m->default_xdpi = GhostviewGetXDPI(m->ghost);
	m->default_ydpi = GhostviewGetYDPI(m->ghost);

	XtSetSensitive(m->zoom_menu, False);

	m->root_width  = WidthOfScreen(screen)  - r->wm_width;
	m->root_height = HeightOfScreen(screen) - r->wm_height;

	wl_assert(MgvOK(m));

	switch(orient) {
	case GvOrientPortrait:
		orientation = NULL;
		break;
	case GvOrientLandscape:
		orientation = wl_find1(m->main, "*orientation_pd*landscape");
		break;
	case GvOrientSeascape:
		orientation = wl_find1(m->main, "*orientation_pd*seascape");
		break;
	case GvOrientUpsideDown:
		orientation = wl_find1(m->main, "*orientation_pd*upsideDown");
		break;
	default:
		/* not possible */
		wl_assert("Unknown orientation!" != NULL);
		orientation = NULL;
		break;
	}
	if(orientation != NULL)
		XmToggleButtonSetState(orientation, True, True);

	XtVaSetValues(parent, XmNdeleteResponse, XmDO_NOTHING, NULL);
	deletewin = XmInternAtom(wl_display(parent), "WM_DELETE_WINDOW", False);
	XmAddWMProtocolCallback(parent, deletewin, mgv_app_close, (XtPointer)m);

	/*
	 * open file
	 */
	status = 0;
	if(file != NULL)
		status = mgv_open(m, file);
	else if(!isatty(fileno(stdin)))
		status = mgv_open_stdin(m);
	else
		mgv_close(m, FALSE, FALSE);

	mgv_ui_magnify(m, r->magstep);

	m->senscache = wl_sens_create(wl_top(m->main), sens, N_SENS);
	wl_sens(m->senscache, m->sens);

	XmToggleButtonSetState(wl_find1(parent,"*menuBar*page_pd*showLabels"),
							m->labels, False);
	XmToggleButtonSetState(wl_find1(parent,"*menuBar*page_pd*showPgNums"),
							m->pgnums, False);
	wl_icons(WL_ICON_MAIN, parent, &m->icon, &m->mask);
	XtRealizeWidget(parent);
	XSync(wl_display(parent), False);
	frobnicate(m->main, toggle);

	/*
	 * And now we can take care of the auto-size requests.
	 * We can't let the toggle button set m->autosizer for us
	 * since the initial mgv_show() probably hasn't been called
	 * yet.
	 */
	switch((r->auto_width == True) + 2*(r->auto_height == True)) {
	case 0:
		sizer        = "*menuBar*size_pd*none";
		m->autosizer = mgv_auto_noop;
		break;
	case 1:
		sizer        = "*menuBar*size_pd*width";
		m->autosizer = mgv_auto_width;
		break;
	case 2:
		sizer        = "*menuBar*size_pd*height";
		m->autosizer = mgv_auto_height;
		break;
	case 3:
		sizer        = "*menuBar*size_pd*size";
		m->autosizer = mgv_auto_size;
		break;
	default:
		wl_assert("busted compiler!" != NULL);
		sizer = NULL;
		break;
	}
	XmToggleButtonSetState(wl_find1(m->main, sizer), True, False);
	XmToggleButtonSetState(wl_find1(m->main, "*options_pd*smartSizing"),
				(m->bits & MgvSMARTSIZE) ? True : False, False);
	XmToggleButtonSetState(wl_find1(m->main, "*options_pd*warnings"),
				r->suppress_warnings, False);

	/*
	 * Similarily for the page|center things.
	 */
	if(r->center) {
		m->center = mgv_center_center;
		XmToggleButtonSetState(wl_find1(m->main, "*page_pd*center"),
				True, False);
	}
	else {
		m->center = mgv_center_noop;
		XmToggleButtonSetState(wl_find1(m->main, "*page_pd*center"),
				False, False);
	}

	XtAddEventHandler(m->main, ButtonPressMask, False, wl_popup_menu,
							(XtPointer)m->popup);

	if(status != 0) {
		mgv_warn(m->main, "%s: %s", file, strerror(status));
		mgv_close(m, FALSE, FALSE);
	}

	m->autosizer(m);

	return m;
}
