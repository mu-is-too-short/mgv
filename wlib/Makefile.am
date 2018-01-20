# @(#)$Mu: mgv/wlib/Makefile.am 1.4 2001/03/12 13:47:27 $
##
## Makefile.am
##	automake file for wlib.
##
## Copyright (C) 1998 Eric A. Howe
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
##
##   Authors:	Eric A. Howe (mu@trends.net)
##

MAINTAINERCLEANFILES = Makefile.in

noinst_LIBRARIES = libwlib.a
libwlib_a_SOURCES = \
	WlAppShell.c WlSplashScreen.c WlToolBar.c \
	about.c app.c appclass.c assert.c callback.c chkver.c create.c \
	dismiss.c dlg.c error.c fgbg.c find.c findup.c focus.c \
	free_pixmap.c help-html.c help-remote.c \
	help.c icons.c init.c log.c popdown_dialog.c popup_dialog.c \
	popup_menu.c process.c res.c scrollbar.c sens.c shell.c shutdown.c \
	status.c strings.c text.c togglesync.c top.c util.c \
	version.c xbm.c xpm.c xtoolwait.c

INCLUDES = -I.

##
## Semi-automatically generated dependencies.  See tools/mudep for things.
## Only local files are listed here (this makes things system-neutral).
## This is the only decent way I could think of to get deps with automake
## and autoconf.  This mudep trick works with Linux and the standard XFree86
## makedepend, YMMV anywhere else.
##
## --STUPID-MARKER--
WlAppShell.o: wlib/WlAppShell.h wlib/WlAppShellP.h wlib/app.h wlib/display.h
WlAppShell.o: wlib/rcs.h wlib/stdlib.h wlib/strdefs.h wlib/util.h wlib/wacros.h
WlAppShell.o: wlib/wlib.h
WlSplashScreen.o: wlib/WlSplashScreen.h wlib/WlSplashScreenP.h wlib/app.h
WlSplashScreen.o: wlib/display.h wlib/rcs.h wlib/sanity.h wlib/strdefs.h
WlSplashScreen.o: wlib/wacros.h wlib/wlib.h wlib/wlibP.h
WlToolBar.o: wlib/WlToolBar-iunknown.xbm wlib/WlToolBar-iunknown.xpm
WlToolBar.o: wlib/WlToolBar-unknown.xbm wlib/WlToolBar-unknown.xpm
WlToolBar.o: wlib/WlToolBar.h wlib/WlToolBarP.h wlib/app.h wlib/display.h
WlToolBar.o: wlib/rcs.h wlib/sanity.h wlib/stdlib.h wlib/strdefs.h
WlToolBar.o: wlib/typesanity.h wlib/wacros.h wlib/wlib.h wlib/wlibP.h
about.o: wlib/about.h wlib/app.h wlib/callbacks.h wlib/display.h wlib/icons.h
about.o: wlib/rcs.h wlib/sanity.h wlib/stdlib.h wlib/strdefs.h wlib/wlib.h
app.o: wlib/app.h wlib/appP.h wlib/rcs.h wlib/resource.h wlib/sanity.h
app.o: wlib/stdlib.h wlib/strdefs.h wlib/util.h wlib/wlib.h
appclass.o: wlib/app.h wlib/display.h wlib/rcs.h wlib/strdefs.h wlib/wlib.h
assert.o: wlib/app.h wlib/rcs.h wlib/sanity.h wlib/stdlib.h wlib/strdefs.h
assert.o: wlib/wlib.h
callback.o: wlib/app.h wlib/rcs.h wlib/strdefs.h wlib/wlib.h
chkver.o: wlib/app.h wlib/build.h wlib/msg.h wlib/rcs.h wlib/resource.h
chkver.o: wlib/strdefs.h wlib/strings.h wlib/wlib.h wlib/wlibP.h
create.o: wlib/app.h wlib/callbacks.h wlib/help.h wlib/rcs.h wlib/sanity.h
create.o: wlib/stdlib.h wlib/strdefs.h wlib/typesanity.h wlib/wlib.h
create.o: wlib/wlibP.h
dismiss.o: wlib/app.h wlib/callbacks.h wlib/rcs.h wlib/strdefs.h wlib/wlib.h
dlg.o: wlib/app.h wlib/rcs.h wlib/strdefs.h wlib/wlib.h
error.o: wlib/msg.h wlib/rcs.h wlib/sanity.h
fgbg.o: wlib/rcs.h wlib/sanity.h wlib/wlibP.h
find.o: wlib/app.h wlib/rcs.h wlib/strdefs.h wlib/wlib.h
findup.o: wlib/app.h wlib/rcs.h wlib/strdefs.h wlib/wlib.h
focus.o: wlib/app.h wlib/rcs.h wlib/resource.h wlib/strdefs.h wlib/wlib.h
free_pixmap.o: wlib/rcs.h wlib/sanity.h
help-html.o: wlib/app.h wlib/display.h wlib/helpP.h wlib/msg.h wlib/rcs.h
help-html.o: wlib/sanity.h wlib/stdlib.h wlib/strdefs.h wlib/strings.h
help-html.o: wlib/wlib.h wlib/wlibP.h
help-remote.o: wlib/app.h wlib/display.h wlib/helpP.h wlib/msg.h wlib/rcs.h
help-remote.o: wlib/sanity.h wlib/stdlib.h wlib/strdefs.h wlib/strings.h
help-remote.o: wlib/wlib.h wlib/wlibP.h
help.o: wlib/app.h wlib/display.h wlib/help.h wlib/helpP.h wlib/process.h
help.o: wlib/rcs.h wlib/resource.h wlib/stdlib.h wlib/strdefs.h wlib/util.h
help.o: wlib/wlib.h wlib/wlibP.h
icons.o: wlib/app.h wlib/display.h wlib/icons.h wlib/rcs.h wlib/resource.h
icons.o: wlib/sanity.h wlib/stdlib.h wlib/strdefs.h wlib/wlib.h wlib/wlibP.h
init.o: wlib/app.h wlib/appP.h wlib/help.h wlib/helpP.h wlib/rcs.h
init.o: wlib/resource.h wlib/stdlib.h wlib/strdefs.h wlib/wlib.h wlib/wlibP.h
log.o: wlib/app.h wlib/callbacks.h wlib/help.h wlib/log.h wlib/msg.h wlib/rcs.h
log.o: wlib/sanity.h wlib/stdlib.h wlib/strdefs.h wlib/strings.h wlib/text.h
log.o: wlib/typesanity.h wlib/wlib.h wlib/wlibP.h
popdown_dialog.o: wlib/app.h wlib/rcs.h wlib/resource.h wlib/strdefs.h
popdown_dialog.o: wlib/wlib.h
popup_dialog.o: wlib/app.h wlib/rcs.h wlib/strdefs.h wlib/wlib.h
popup_menu.o: wlib/app.h wlib/rcs.h wlib/strdefs.h wlib/wlib.h
process.o: wlib/app.h wlib/process.h wlib/rcs.h wlib/stdlib.h wlib/strdefs.h
process.o: wlib/wlib.h
res.o: wlib/build.h wlib/rcs.h wlib/resource.h wlib/stdlib.h wlib/wlibP.h
scrollbar.o: wlib/app.h wlib/rcs.h wlib/sanity.h wlib/scrollbar.h
scrollbar.o: wlib/strdefs.h wlib/wlib.h
sens.o: wlib/app.h wlib/rcs.h wlib/sens.h wlib/stdlib.h wlib/strdefs.h
sens.o: wlib/wlib.h
shell.o: wlib/app.h wlib/rcs.h wlib/strdefs.h wlib/wlib.h
shutdown.o: wlib/app.h wlib/help.h wlib/helpP.h wlib/rcs.h wlib/strdefs.h
shutdown.o: wlib/wlib.h wlib/wlibP.h
status.o: wlib/callbacks.h wlib/rcs.h wlib/stdlib.h wlib/strdefs.h wlib/wlibP.h
strings.o: wlib/app.h wlib/rcs.h wlib/resource.h wlib/stdlib.h wlib/strdefs.h
strings.o: wlib/strings.h wlib/wlib.h wlib/wlibP.h
text.o: wlib/rcs.h wlib/text.h
togglesync.o: wlib/app.h wlib/callbacks.h wlib/rcs.h wlib/sanity.h
togglesync.o: wlib/strdefs.h wlib/typesanity.h wlib/wlib.h
top.o: wlib/app.h wlib/rcs.h wlib/strdefs.h wlib/wlib.h
util.o: wlib/rcs.h wlib/stdlib.h wlib/util.h
version.o: wlib/app.h wlib/build.h wlib/rcs.h wlib/stdlib.h wlib/strdefs.h
version.o: wlib/wlib.h
xbm.o: wlib/app.h wlib/display.h wlib/rcs.h wlib/sanity.h wlib/strdefs.h
xbm.o: wlib/wlib.h wlib/wlibP.h
xpm.o: wlib/app.h wlib/display.h wlib/rcs.h wlib/sanity.h wlib/strdefs.h
xpm.o: wlib/wlib.h wlib/wlibP.h
xtoolwait.o: wlib/rcs.h wlib/wlibP.h
