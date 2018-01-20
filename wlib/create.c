/*
 * create.c
 *	Widget tree creation functions for wlib.
 *
 * Copyright (C) 1996  Matthew D. Francey
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
#include	<wlib/rcs.h>
MU_ID("$Mu: mgv/wlib/create.c 1.3 2001/03/17 17:59:01 $")

#include <errno.h>
#include <Xm/XmAll.h>

#include <wlib/stdlib.h>
#include <wlib/wlib.h>
#include <wlib/callbacks.h>
#include <wlib/help.h>
#include <wlib/wlibP.h>
#include <wlib/sanity.h>
#include <wlib/typesanity.h>

/*
 * local constants
 */
#define	WL_CLASS	0x0001	/* raw WidgetClass	*/
#define	WL_CONV		0x0002	/* convenience function	*/
#define	WL_TYPEMASK	0x000f
#define	WL_DIALOG	0x0010

/*
 * These map "class" names to the appropriate construction technique.
 * Exactly one of `cls' and `func' should be non-NULL.
 */
typedef struct {
	char       *name;
	WidgetClass cls;
	WL_CONVFUNC func;
	int         type;
} N2C;
static N2C *classmap;
static int n_classmap;

/*
 * Initial values for the classmap.
 * I'm too clever to sort this my hand so we copy the whole mess
 * and let qsort do what it does.  We could use two classmaps--built-in
 * and application defined--but that's more bother than we need, the
 * memory cost of copying `initn2c' really isn't anything to worry about.
 */
static struct {
	char        *name;
	WidgetClass *cls;
	WL_CONVFUNC  func;
	int          type;
} initn2c[] = {
	/*
	 * normal old widgets, someday I'll get around to adding
	 * the Dt and Xm 2.0 widgets to this list, for now, you'll
	 * have to add Dt/Xm2.0 ones with wl_addclass() if you
	 * want to use them
	 */
	{wlArrowB,     &xmArrowButtonWidgetClass,      NULL, 0},
	{wlBulletinB,  &xmBulletinBoardWidgetClass,    NULL, 0},
	{wlCascadeB,   &xmCascadeButtonWidgetClass,    NULL, 0},
	{wlCommand,    &xmCommandWidgetClass,          NULL, 0},
	{wlDialogS,    &xmDialogShellWidgetClass,      NULL, 0},
	{wlDrawingA,   &xmDrawingAreaWidgetClass,      NULL, 0},
	{wlDrawnB,     &xmDrawnButtonWidgetClass,      NULL, 0},
	{wlFileSB,     &xmFileSelectionBoxWidgetClass, NULL, 0},
	{wlForm,       &xmFormWidgetClass,             NULL, 0},
	{wlFrame,      &xmFrameWidgetClass,            NULL, 0},
	{wlLabel,      &xmLabelWidgetClass,            NULL, 0},
	{wlList,       &xmListWidgetClass,             NULL, 0},
	{wlMainW,      &xmMainWindowWidgetClass,       NULL, 0},
	{wlMenuShell,  &xmMenuShellWidgetClass,        NULL, 0},
	{wlMessageB,   &xmMessageBoxWidgetClass,       NULL, 0},
	{wlPanedW,     &xmPanedWindowWidgetClass,      NULL, 0},
	{wlPushB,      &xmPushButtonWidgetClass,       NULL, 0},
	{wlRowColumn,  &xmRowColumnWidgetClass,        NULL, 0},
	{wlScale,      &xmScaleWidgetClass,            NULL, 0},
	{wlScrollBar,  &xmScrollBarWidgetClass,        NULL, 0},
	{wlScrolledW,  &xmScrolledWindowWidgetClass,   NULL, 0},
	{wlSelectionB, &xmSelectionBoxWidgetClass,     NULL, 0},
	{wlSeparator,  &xmSeparatorWidgetClass,        NULL, 0},
	{wlText,       &xmTextWidgetClass,             NULL, 0},
	{wlTextField,  &xmTextFieldWidgetClass,        NULL, 0},
	{wlToggleB,    &xmToggleButtonWidgetClass,     NULL, 0},

	/*
	 * convenience functions for dialogs
	 */
	{wlErrorDialog,       NULL, XmCreateErrorDialog,         WL_DIALOG},
	{wlFileSBDialog,      NULL, XmCreateFileSelectionDialog, WL_DIALOG},
	{wlFormDialog,        NULL, XmCreateFormDialog,          WL_DIALOG},
	{wlInformationDialog, NULL, XmCreateInformationDialog,   WL_DIALOG},
	{wlMessageDialog,     NULL, XmCreateMessageDialog,       WL_DIALOG},
	{wlPromptDialog,      NULL, XmCreatePromptDialog,        WL_DIALOG},
	{wlQuestionDialog,    NULL, XmCreateQuestionDialog,      WL_DIALOG},
	{wlSelectionDialog,   NULL, XmCreateSelectionDialog,     WL_DIALOG},
	{wlTemplateDialog,    NULL, XmCreateTemplateDialog,      WL_DIALOG},
	{wlWarningDialog,     NULL, XmCreateWarningDialog,       WL_DIALOG},
	{wlWorkingDialog,     NULL, XmCreateWorkingDialog,       WL_DIALOG},

	/*
	 * convenience functions for widgets
	 */
	{wlMenuBar,      NULL, XmCreateMenuBar,      0},
	{wlOptionMenu,   NULL, XmCreateOptionMenu,   0},
	{wlPopupMenu,    NULL, XmCreatePopupMenu,    0},
	{wlPulldownMenu, NULL, XmCreatePulldownMenu, 0},
	{wlRadioBox,     NULL, XmCreateRadioBox,     0},
	{wlScrolledList, NULL, XmCreateScrolledList, 0},
	{wlScrolledText, NULL, XmCreateScrolledText, 0}
};
#define	N_INIT	(sizeof(initn2c)/sizeof(initn2c[0]))

/*
 * qsort/bsearch comparison function for the classmap array
 */
static int
classmap_cmp(const void *p1, const void *p2)
{
	return strcmp(((N2C *)p1)->name, ((N2C *)p2)->name);
}

static int
mkmap(N2C *entry, char *name, WidgetClass cls, WL_CONVFUNC func, int type)
{
	if((entry->name = wl_malloc(strlen(name) + 1)) == NULL)
		return FALSE;
	strcpy(entry->name, name);
	entry->type |= type;
	if(cls != NULL) {
		entry->type |= WL_CLASS;
		entry->cls   = cls;
	}
	else {
		entry->type |= WL_CONV;
		entry->func  = func;
	}
	return TRUE;
}

void
wlp_create_shutdown(void)
{
	for(--n_classmap; n_classmap >= 0; --n_classmap)
		wl_free((void *)classmap[n_classmap].name);
	classmap   = wl_free((void *)classmap);
	n_classmap = 0;
}

static int
classmap_create(void)
{
	int         i;
	WidgetClass cls;

	if(classmap != NULL)
		return TRUE;

	if((classmap = wl_calloc(N_INIT, sizeof(N2C))) == NULL)
		return FALSE;
	n_classmap = N_INIT;
	for(i = 0; i < n_classmap; ++i) {
		cls = initn2c[i].cls != NULL ? *initn2c[i].cls : NULL;
		if(!mkmap(&classmap[i], initn2c[i].name, cls, initn2c[i].func,
							initn2c[i].type)) {
			wlp_create_shutdown();
			return FALSE;
		}
	}
	qsort((void *)classmap, n_classmap, sizeof(N2C), classmap_cmp);

	return TRUE;
}

int
wl_addclass(char *type, WidgetClass cls, WL_CONVFUNC func, int dlg)
{
	N2C *tmp, e;
	int  i;

	if(classmap == NULL && !classmap_create())
		return ENOMEM;
	e.name = type;
	tmp = bsearch((void *)&e, (void *)classmap, n_classmap, sizeof(N2C),
								classmap_cmp);
	if(tmp != NULL)
		return EEXIST;

	memset((void *)&e, '\0', sizeof(e));
	if(!mkmap(&e, type, cls, func, dlg ? WL_DIALOG : 0)) {
		wl_free((void *)e.name);
		return ENOMEM;
	}
	tmp = wl_realloc(classmap, (n_classmap + 1)*sizeof(N2C));
	if(tmp == NULL)
		return ENOMEM;
	classmap = tmp;
	++n_classmap;

	for(i = 0; i < n_classmap - 1; ++i)
		if(strcmp(e.name, classmap[i].name) < 0)
			break;
	memmove((void *)&classmap[i + 1], (void *)&classmap[i],
					(n_classmap - i - 1) * sizeof(N2C));
	classmap[i] = e;

	return 0;
}

/*
 * set defaultButton and/or cancelButton
 */
typedef struct {
	String def;
	String cancel;
} DRES;
static void
buttons(Widget w)
{
	Widget   k;
	DRES     d;
	Arg      a[2];
	Cardinal n;
	XtResource r[] = {
		{
			XwlNdefaultButton, XwlCDefaultButton,
			XtRString, sizeof(String), XtOffsetOf(DRES, def),
			XtRImmediate, NULL
		}, {
			XwlNcancelButton, XwlCCancelButton,
			XtRString, sizeof(String), XtOffsetOf(DRES, cancel),
			XtRImmediate, NULL
		}
	};

	memset((void *)&d, '\0', sizeof(d));
	XtGetApplicationResources(w, (XtPointer)&d, &r[0], XtNumber(r),
								NULL, 0);

	n = 0;
	if(d.def != NULL && (k = wl_find1(w, d.def)) != NULL) {
		XtSetArg(a[n], XmNdefaultButton, k);	++n;
	}
	if(d.cancel != NULL && (k = wl_find1(w, d.cancel)) != NULL) {
		XtSetArg(a[n], XmNcancelButton, k);	++n;
	}
	wlSetValues(w, a, n);
}

#define is_button(w) (WlIsPushButton(w) || WlIsToggleButton(w))

static void
grok_flags(Widget w, int flags)
{
	Widget status;

	if((flags & WL_STATUS) != 0
	&& is_button(w)
	&& (status = wl_find1(wl_top(w), "*wlStatusLabel")) != NULL) {
		XtAddCallback(w, XmNarmCallback,
					wl_status_arm, (XtPointer)status);
		XtAddCallback(w, XmNdisarmCallback,
					wl_status_disarm, (XtPointer)status);
		XtAddCallback(w, XmNdestroyCallback,
					wl_status_erase, NULL);
	}

	if(flags & WL_MANAGE)
		XtManageChild(w);
}

/*
 * Error recovery of this function could be improved.
 */
Widget
wl_create(Widget parent, WLW *tw)
{
	Widget   (*mk)(const char *, WidgetClass, Widget, ArgList, Cardinal);
	Arg      a;
	Cardinal n;
	Widget   w, ret;
	int      help;
	N2C      *ctor, srch;

	if(classmap == NULL && !classmap_create())
		return NULL;

	/*
	 * if the parent is a XmMENU_BAR or XmPULLDOWN_MENU
	 * all the kiddies get a XmNhelpCallback...
	 */
	help = 0;
	if(XmIsRowColumn(parent)) {
		unsigned char t;

		XtSetArg(a, XmNrowColumnType, &t);
		XtGetValues(parent, &a, 1);
		if(t == XmMENU_BAR
		|| t == XmMENU_OPTION
		|| t == XmMENU_POPUP
		|| t == XmMENU_PULLDOWN)
			++help;
	}

	/*
	 * run down the list of widgets to create
	 */
	ret = NULL;
	while(tw->name != NULL) {
		n = 0;

		/*
		 * create the widget
		 */
		memset((void *)&srch, '\0', sizeof(srch));
		srch.name = tw->type;
		ctor      = bsearch((void *)&srch, (void *)classmap, n_classmap,
						sizeof(N2C), classmap_cmp);
		if(ctor == NULL)
			return NULL;
		switch((ctor->type) & WL_TYPEMASK) {
		case WL_CLASS:
			mk = (tw->flags & WL_UNMANAGE)
				? XtCreateWidget
				: XtCreateManagedWidget;
			w = mk(tw->name, ctor->cls, parent, NULL, 0);
			break;
		case WL_CONV:
			if((ctor->type & WL_DIALOG)
			&& (tw->flags  & WL_UNMANAGE)) {
				XtSetArg(a, XmNautoUnmanage, False);	++n;
			}
			w = ctor->func(parent, tw->name, &a, n);
			break;
		default:
			wl_assert("wl_create(): impossible type mask" != NULL);
			return NULL;
		}
		if(w == NULL)
			return NULL;

		if(ctor->type & WL_DIALOG)
			++help;

		if(help)
			XtAddCallback(w, XmNhelpCallback, wl_help, NULL);
		if(ret == NULL)
			ret = w;

		/*
		 * kiddies?  if not, continue with next widget
		 */
		if(tw->kids == NULL) {
			grok_flags(w, tw->flags);
			++tw;
			continue;
		}

		/*
		 * Create the kiddies.
		 *
		 * WARNING:  Profanity this way comes.
		 *
		 * However, Motif's fucked menu system causes us to
		 * digress from simplicity.  The Motif menu system
		 * is surely the work of a twisted mind.  Sanity
		 * would suggest that the pulldown should be a
		 * child of "w" to reflect the visual hierarchy of
		 * the menus; however, Motif thinks it makes more
		 * sense to have only one XmMenuShell at each level;
		 * why? the wrong kinds of drugs perhaps.
		 *
		 * If we go ahead and snub our noses at this brain
		 * damage, it will work just fine--and give us a sane
		 * widget tree in the menu--except on a few Motif versions
		 * (2.0.0 and 1.2.3 on some systems) where it dumps core
		 * when destroying the menubar (no real surprises here
		 * though, you should see what Purify thinks of libXm).
		 * So, we do the ugly thing to calm the anger of the
		 * blind idiot Motif gods and their mad pipers.
		 *
		 * Not only does the menu organization not make any sense
		 * but it forces you to be extra careful with possible
		 * name conflicts.
		 *
		 * This menu system is fucking high stupidity.  I just
		 * can't rage against this nearly enough.
		 *
		 * Look the the menu tree in editres and you'll see how
		 * fucking stupid it looks!
		 *
		 * BTW, I'm not sure what should happen with option
		 * menus here, I don't use them so I've been too lazy
		 * to read up on the matter; for now I'm assuming that
		 * option menus require the same dumb-ass treatment as
		 * the other menus.
		 */
		n = 0;
		if(strcmp(tw->type, wlCascadeB)   == 0
		|| strcmp(tw->type, wlOptionMenu) == 0) {
			Widget	pd;
			char	s[1024];
			Arg	a;

			sprintf(s, "%s_pd", XtName(w));
			pd = XmCreatePulldownMenu(parent, s, NULL, 0);
			if(pd == NULL)
				return NULL;
			XtSetArg(a, XmNsubMenuId, pd);
			XtSetValues(w, &a, 1);
			w = pd;
		}
		if(wl_create(w, tw->kids) == NULL)
			return NULL;
		grok_flags(w, tw->flags);
		++tw;
	}

	/*
	 * We can't set defaultButton or cancelButton in the resource
	 * file since the manager's resources will get picked up before
	 * the children are born.  Hence, we kludge around the problem.
	 *
	 * We use XmIsManager rather than XmIsDailogShell(XtParent())
	 * in case someone wants default button type things in something
	 * that isn't inside a dialog shell.
	 */
	if(XmIsManager(ret))
		buttons(ret);

	return ret;
}
