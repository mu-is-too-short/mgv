/*
 * wlib.h
 *	Interface to wlib.
 *
 * Copyright (C) 1996 Matthew D. Francey
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
#ifndef	WLIB_H
#define	WLIB_H
MU_HID(wlib_h, "$Mu: mgv/wlib/wlib/wlib.h 1.2 2001/03/11 18:24:10 $")

#include <X11/Intrinsic.h>

#include <wlib/app.h>
#include <wlib/strdefs.h>

/*
 * widget creation convenience function
 */
CDECLS_BEGIN
typedef Widget (*WL_CONVFUNC)(Widget, String, ArgList, Cardinal);
CDECLS_END

/*
 * callback lists
 */
typedef struct {
	char		*name;
	char		*call;
	XtPointer	closure;
	void		(*func)(Widget, XtPointer, XtPointer);
} WLC;

/*
 * Widget lists.
 * In most cases, the "type" will be the name of the widget's public
 * header (without the .h), for dialogs, use "ErrorDialog", "FileSBDialog",...
 * If you've added a widget, then you already know what "type" to use.
 *
 * For convenience (and some extra compile time checking), constants
 * for the standard Motif 1.2 classes are provided in wlib/strdefs.h.
 */
#define	WL_MANAGE	(1 << 0)
#define	WL_UNMANAGE	(1 << 1)
#define	WL_STATUS	(1 << 2)
typedef struct wl_widget_s WLW;
struct wl_widget_s {
	char *name;
	char *type;
	int   flags;	/* WL_MANAGE, WL_UNMANAGE, WL_STATUS */
	WLW  *kids;
};

#define	wlArrowB		"ArrowB"
#define	wlBulletinB		"BulletinB"
#define	wlCascadeB		"CascadeB"
#define	wlCommand		"Command"
#define	wlDialogS		"DialogS"
#define	wlDrawingA		"DrawingA"
#define	wlDrawnB		"DrawnB"
#define	wlFileSB		"FileSB"
#define	wlForm			"Form"
#define	wlFrame			"Frame"
#define	wlLabel			"Label"
#define	wlList			"List"
#define	wlMainW			"MainW"
#define	wlMenuShell		"MenuShell"
#define	wlMessageB		"MessageB"
#define	wlPanedW		"PanedW"
#define	wlPushB			"PushB"
#define	wlRowColumn		"RowColumn"
#define	wlScale			"Scale"
#define	wlScrollBar		"ScrollBar"
#define	wlScrolledW		"ScrolledW"
#define	wlSelectionB		"SelectionB"
#define	wlSeparator		"Separator"
#define	wlText			"Text"
#define	wlTextField		"TextField"
#define	wlToggleB		"ToggleB"
#define	wlErrorDialog		"ErrorDialog"
#define	wlFileSBDialog		"FileSBDialog"
#define	wlFormDialog		"FormDialog"
#define	wlInformationDialog	"InformationDialog"
#define	wlMessageDialog		"MessageDialog"
#define	wlPromptDialog		"PromptDialog"
#define	wlQuestionDialog	"QuestionDialog"
#define	wlSelectionDialog	"SelectionDialog"
#define	wlTemplateDialog	"TemplateDialog"
#define	wlWarningDialog		"WarningDialog"
#define	wlWorkingDialog		"WorkingDialog"
#define	wlMenuBar		"MenuBar"
#define	wlOptionMenu		"OptionMenu"
#define	wlPopupMenu		"PopupMenu"
#define	wlPulldownMenu		"PulldownMenu"
#define	wlRadioBox		"RadioBox"
#define	wlScrolledList		"ScrolledList"
#define	wlScrolledText		"ScrolledText"

/*
 * These currently only exist for completeness.
 */
#define	wl_find1	XtNameToWidget
#define	wl_destroy	XtDestroyWidget

#define wl_unmanage(w) {if(w != NULL) XtUnmanageChild(w);}

/*
 * prototypes
 * Functions that return ints generally return zero for success
 * and an errno value for failure
 */
CDECLS_BEGIN
extern WLAPP wl_init(int *, char **, String, XrmOptionDescList, Cardinal, char **);

extern int    wl_addclass(char *, WidgetClass, WL_CONVFUNC, int);
extern int    wl_appnames(Widget, char **, char **);
extern char  *wl_appname(Widget);
extern char  *wl_appclass(Widget);
extern Widget wl_create(Widget, WLW *);
extern void   wl_callback(Widget, WLC *, XtPointer);
extern void   wl_find(Widget, ...);
extern Widget wl_findup(Widget, char *);
extern Widget wl_top(Widget);
extern Widget wl_shell(Widget);
extern Widget wl_dlg(Widget);
extern Widget wl_dlgshell(Widget);
extern void   wl_shutdown(void);
extern Widget wl_focus(Widget);

extern char *wl_version(void);

extern int wl_chkver(Widget);

extern void wl_dlg_popup(Widget);
extern void wl_dlg_popdown(Widget);
extern void wl_popup_menu(Widget, XtPointer, XEvent *, Boolean *);

CDECLS_END

#endif
