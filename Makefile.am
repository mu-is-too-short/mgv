# @(#)$Mu: mgv/Makefile.am 1.118 2001/03/14 13:13:25 $
##
## Makefile.am
##	automake file for mgv.
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

ACLOCAL_AMFLAGS = -I config

bin_PROGRAMS = mgv
mgv_SOURCES  = \
	Ghostview.c app.c auto.c center.c dsc.c error.c \
	file.c help.c log.c magstep.c main.c mark.c mark_action.c \
	mark_range.c media.c mgv.c mgv_create.c options.c orientation.c \
	page.c page_control.c print.c scroll.c strings.c toolbar.c \
	track.c uiutil.c unmangle.c view.c zoom.c \
	_ad.c
mgv_libs = -lwlib @XM_LIBS@ \
	@EDITRES_LIBS@ @XT_LIBS@ @XPM_LIBS@ @XP_LIBS@ @XEXT_LIBS@ \
	@X11_LIBS@ @X_PRE_LIBS@
mgv_LDADD = _bld.o -Lwlib $(mgv_libs)

mgvresdir = $(pkgdatadir)/$(VERSION)
mgvres_DATA = MGv.gz

##
## Make sure this is kept in sync with the _bld.c rule below.
##
mgv_DEPENDENCIES = _bld.o wlib/libwlib.a

BUILD_SOURCE = _ad.c _bld.c MGv

CLEANFILES = .been_warned MGv.gz
DISTCLEANFILES = _ad.c _bld.c MGv

INCLUDES = -I. -Iwlib

SUBDIRS = doc wlib lib

_ad.c: MGv
	tools/ad2c -a bld_fallbacks MGv > _ad.c

MGv: MGv.ad
	sed -e 's;__DATADIR__;$(datadir)/mgv/$(VERSION);g' -e 's;__VERSION__;$(VERSION);g' < MGv.ad > MGv || (rm -f MGv; exit 1)

MGv.gz: MGv
	gzip -9 < $< > $@

##
## Maintainer only things.
##
if MU_MAINTAINER

##
## This requires GNU make but I don't care since this is the maintainer
## section.
##
mu_ads = $(wildcard ad/*.ad wlib/wlib/*.ad)

res.m4: doc/mgv.sgml-in
	tools/mkresm4 --prefix=mgv --output=res.m4 $<

MGv.ad: MGv.ad.in res.m4 $(mu_ads)
	m4 -P -D__CLS__=MGv -I. -Iwlib res.m4 MGv.ad.in > MGv.ad \
		|| (rm -f MGv.ad; exit 1)

endif

##
## We really want to use $(mgv_DEPENDENCIES) here as well but, alas,
## that will yield a circular dependency because _bld.o is in
## $(mgv_DEPENDENCIES).  Hence, we duplicate parts of $(mgv_DEPENDENCIES)
## here.
##
_bld.c: $(mgv_OBJECTS) wlib/libwlib.a Makefile
	tools/mkver '$(VERSION)' '$(CC)' '$(CFLAGS)' '$(DEFS)' '$(LDFLAGS)' \
		'$(mgv_libs)' '$(bindir)' '$(datadir)' \
		> _bld.c || (rm -f _bld.c; exit 1)

MAINTAINERCLEANFILES = Makefile.in MGv.ad res.m4 \
	.been-warned configure aclocal.m4

all: .been-warned
.been-warned:
	@echo
	@echo "**********************************************************"
	@cat RELEASE_NOTES
	@echo "**********************************************************"
	@touch .been-warned

##
## Semi-automatically generated dependencies.  See tools/mudep for things.
## Only local files are listed here (this makes things system-neutral).
## This is the only decent way I could think of to get deps with automake
## and autoconf.  This mudep trick works with Linux and the standard XFree86
## makedepend, YMMV anywhere else.
##
## --STUPID-MARKER--
Ghostview.o: mine/Ghostview.h mine/GhostviewP.h wlib/wlib/app.h
Ghostview.o: wlib/wlib/display.h wlib/wlib/rcs.h wlib/wlib/sanity.h
Ghostview.o: wlib/wlib/stdlib.h wlib/wlib/strdefs.h wlib/wlib/util.h
Ghostview.o: wlib/wlib/wacros.h wlib/wlib/wlib.h
app.o: mine/Ghostview.h mine/app.h mine/dsc.h mine/error.h mine/log.h
app.o: mine/mark.h mine/mgv.h mine/opaque.h mine/page.h mine/scroll.h
app.o: mine/strdefs.h mine/strings.h mine/zoom.h wlib/wlib/WlAppShell.h
app.o: wlib/wlib/app.h wlib/wlib/build.h wlib/wlib/callbacks.h
app.o: wlib/wlib/display.h wlib/wlib/help.h wlib/wlib/icons.h wlib/wlib/log.h
app.o: wlib/wlib/msg.h wlib/wlib/rcs.h wlib/wlib/resource.h wlib/wlib/sanity.h
app.o: wlib/wlib/sens.h wlib/wlib/stdlib.h wlib/wlib/strdefs.h
app.o: wlib/wlib/strings.h wlib/wlib/util.h wlib/wlib/wlib.h
auto.o: mine/Ghostview.h mine/auto.h mine/dsc.h mine/mgv.h mine/opaque.h
auto.o: wlib/wlib/app.h wlib/wlib/rcs.h wlib/wlib/sanity.h wlib/wlib/sens.h
auto.o: wlib/wlib/strdefs.h wlib/wlib/wlib.h
center.o: mine/Ghostview.h mine/center.h mine/dsc.h mine/mgv.h mine/opaque.h
center.o: wlib/wlib/app.h wlib/wlib/rcs.h wlib/wlib/sanity.h
center.o: wlib/wlib/scrollbar.h wlib/wlib/sens.h wlib/wlib/strdefs.h
center.o: wlib/wlib/wlib.h
dsc.o: mine/dsc.h wlib/wlib/rcs.h wlib/wlib/stdlib.h wlib/wlib/util.h
error.o: mine/error.h mine/log.h mine/strdefs.h mine/strings.h wlib/wlib/app.h
error.o: wlib/wlib/msg.h wlib/wlib/rcs.h wlib/wlib/strdefs.h
error.o: wlib/wlib/strings.h wlib/wlib/wlib.h
file.o: mine/Ghostview.h mine/app.h mine/dsc.h mine/error.h mine/file.h
file.o: mine/mgv.h mine/opaque.h mine/page_control.h mine/sens.h mine/strdefs.h
file.o: mine/strings.h mine/uiutil.h wlib/wlib/WlAppShell.h wlib/wlib/app.h
file.o: wlib/wlib/callbacks.h wlib/wlib/display.h wlib/wlib/help.h
file.o: wlib/wlib/process.h wlib/wlib/rcs.h wlib/wlib/sanity.h wlib/wlib/sens.h
file.o: wlib/wlib/stdlib.h wlib/wlib/strdefs.h wlib/wlib/strings.h
file.o: wlib/wlib/typesanity.h wlib/wlib/wlib.h
help.o: mine/Ghostview.h mine/dsc.h mine/help.h mine/mgv.h mine/opaque.h
help.o: wlib/wlib/about.h wlib/wlib/app.h wlib/wlib/build.h wlib/wlib/icons.h
help.o: wlib/wlib/rcs.h wlib/wlib/sanity.h wlib/wlib/sens.h wlib/wlib/strdefs.h
help.o: wlib/wlib/wlib.h
log.o: mine/Ghostview.h mine/app.h mine/dsc.h mine/log.h mine/mgv.h
log.o: mine/opaque.h mine/sens.h mine/strdefs.h mine/strings.h wlib/wlib/app.h
log.o: wlib/wlib/rcs.h wlib/wlib/sanity.h wlib/wlib/sens.h wlib/wlib/strdefs.h
log.o: wlib/wlib/strings.h wlib/wlib/wlib.h
magstep.o: mine/Ghostview.h mine/app.h mine/dsc.h mine/magstep.h mine/mgv.h
magstep.o: mine/opaque.h wlib/wlib/app.h wlib/wlib/callbacks.h wlib/wlib/help.h
magstep.o: wlib/wlib/rcs.h wlib/wlib/sanity.h wlib/wlib/sens.h
magstep.o: wlib/wlib/stdlib.h wlib/wlib/strdefs.h wlib/wlib/typesanity.h
magstep.o: wlib/wlib/wlib.h
main.o: lib/log-icon.xbm lib/log-icon.xpm lib/log-mask.xbm lib/mgv-icon.xbm
main.o: lib/mgv-icon.xpm lib/mgv-mask.xbm lib/splash.xbm lib/splash.xpm
main.o: mine/Ghostview.h mine/app.h mine/dsc.h mine/help.h mine/mgv.h
main.o: mine/opaque.h mine/page_control.h mine/strdefs.h mine/strings.h
main.o: wlib/wlib/WlAppShell.h wlib/wlib/WlSplashScreen.h wlib/wlib/WlToolBar.h
main.o: wlib/wlib/app.h wlib/wlib/build.h wlib/wlib/help.h wlib/wlib/icons.h
main.o: wlib/wlib/msg.h wlib/wlib/options.h wlib/wlib/rcs.h wlib/wlib/sanity.h
main.o: wlib/wlib/sens.h wlib/wlib/stdlib.h wlib/wlib/strdefs.h
main.o: wlib/wlib/strings.h wlib/wlib/wlib.h
mark.o: mine/Ghostview.h mine/dsc.h mine/mark.h mine/mgv.h mine/opaque.h
mark.o: mine/uiutil.h wlib/wlib/rcs.h wlib/wlib/sanity.h wlib/wlib/sens.h
mark_action.o: mine/Ghostview.h mine/dsc.h mine/mark.h mine/mgv.h mine/opaque.h
mark_action.o: mine/uiutil.h wlib/wlib/app.h wlib/wlib/display.h
mark_action.o: wlib/wlib/rcs.h wlib/wlib/sanity.h wlib/wlib/sens.h
mark_action.o: wlib/wlib/stdlib.h wlib/wlib/strdefs.h wlib/wlib/util.h
mark_action.o: wlib/wlib/wlib.h
mark_range.o: mine/Ghostview.h mine/dsc.h mine/error.h mine/mark.h mine/mgv.h
mark_range.o: mine/opaque.h mine/uiutil.h wlib/wlib/app.h wlib/wlib/callbacks.h
mark_range.o: wlib/wlib/help.h wlib/wlib/rcs.h wlib/wlib/sanity.h
mark_range.o: wlib/wlib/sens.h wlib/wlib/stdlib.h wlib/wlib/strdefs.h
mark_range.o: wlib/wlib/wlib.h
media.o: mine/Ghostview.h mine/dsc.h mine/media.h mine/mgv.h mine/opaque.h
media.o: wlib/wlib/rcs.h wlib/wlib/sanity.h wlib/wlib/sens.h
media.o: wlib/wlib/typesanity.h
mgv.o: mine/Ghostview.h mine/app.h mine/auto.h mine/dsc.h mine/log.h mine/mgv.h
mgv.o: mine/opaque.h mine/page_control.h mine/sens.h mine/strdefs.h
mgv.o: mine/strings.h mine/uiutil.h mine/unmangle.h mine/zoom.h wlib/wlib/app.h
mgv.o: wlib/wlib/display.h wlib/wlib/rcs.h wlib/wlib/sanity.h
mgv.o: wlib/wlib/scrollbar.h wlib/wlib/sens.h wlib/wlib/stdlib.h
mgv.o: wlib/wlib/strdefs.h wlib/wlib/strings.h wlib/wlib/wlib.h
mgv_create.o: mine/Ghostview.h mine/app.h mine/auto.h mine/center.h mine/dsc.h
mgv_create.o: mine/error.h mine/file.h mine/help.h mine/log.h mine/magstep.h
mgv_create.o: mine/mark.h mine/media.h mine/mgv.h mine/opaque.h mine/options.h
mgv_create.o: mine/orientation.h mine/page.h mine/print.h mine/sens.h
mgv_create.o: mine/toolbar.h mine/track.h mine/view.h mine/zoom.h
mgv_create.o: wlib/wlib/app.h wlib/wlib/callbacks.h wlib/wlib/display.h
mgv_create.o: wlib/wlib/help.h wlib/wlib/icons.h wlib/wlib/rcs.h
mgv_create.o: wlib/wlib/sanity.h wlib/wlib/sens.h wlib/wlib/stdlib.h
mgv_create.o: wlib/wlib/strdefs.h wlib/wlib/wlib.h
options.o: mine/Ghostview.h mine/app.h mine/dsc.h mine/mgv.h mine/opaque.h
options.o: mine/options.h wlib/wlib/app.h wlib/wlib/rcs.h wlib/wlib/sanity.h
options.o: wlib/wlib/sens.h wlib/wlib/typesanity.h
orientation.o: mine/Ghostview.h mine/dsc.h mine/mgv.h mine/opaque.h
orientation.o: mine/orientation.h wlib/wlib/rcs.h wlib/wlib/sanity.h
orientation.o: wlib/wlib/sens.h wlib/wlib/typesanity.h
page.o: mine/Ghostview.h mine/app.h mine/auto.h mine/center.h mine/dsc.h
page.o: mine/log.h mine/mgv.h mine/opaque.h mine/page.h mine/page_control.h
page.o: mine/sens.h mine/uiutil.h wlib/wlib/app.h wlib/wlib/rcs.h
page.o: wlib/wlib/sanity.h wlib/wlib/sens.h wlib/wlib/strdefs.h
page.o: wlib/wlib/typesanity.h wlib/wlib/wlib.h
page_control.o: mine/Ghostview.h mine/dsc.h mine/mgv.h mine/opaque.h
page_control.o: mine/page_control.h mine/sens.h wlib/wlib/rcs.h
page_control.o: wlib/wlib/sanity.h wlib/wlib/sens.h wlib/wlib/stdlib.h
print.o: mine/Ghostview.h mine/app.h mine/dsc.h mine/error.h mine/log.h
print.o: mine/mgv.h mine/opaque.h mine/print.h mine/sens.h mine/strdefs.h
print.o: mine/strings.h mine/uiutil.h wlib/wlib/app.h wlib/wlib/callbacks.h
print.o: wlib/wlib/help.h wlib/wlib/process.h wlib/wlib/rcs.h
print.o: wlib/wlib/sanity.h wlib/wlib/sens.h wlib/wlib/stdlib.h
print.o: wlib/wlib/strdefs.h wlib/wlib/strings.h wlib/wlib/util.h
print.o: wlib/wlib/wlib.h
scroll.o: mine/Ghostview.h mine/center.h mine/dsc.h mine/error.h mine/mgv.h
scroll.o: mine/opaque.h mine/page_control.h mine/scroll.h mine/sens.h
scroll.o: mine/strdefs.h mine/strings.h mine/uiutil.h wlib/wlib/app.h
scroll.o: wlib/wlib/display.h wlib/wlib/rcs.h wlib/wlib/sanity.h
scroll.o: wlib/wlib/scrollbar.h wlib/wlib/sens.h wlib/wlib/stdlib.h
scroll.o: wlib/wlib/strdefs.h wlib/wlib/strings.h wlib/wlib/util.h
scroll.o: wlib/wlib/wlib.h
strings.o: mine/strdefs.h mine/strings.h wlib/wlib/app.h wlib/wlib/rcs.h
strings.o: wlib/wlib/strdefs.h wlib/wlib/strings.h wlib/wlib/wlib.h
toolbar.o: lib/toolbar/fileOpen-i.xbm lib/toolbar/fileOpen-i.xpm
toolbar.o: lib/toolbar/fileOpen.xbm lib/toolbar/fileOpen.xpm
toolbar.o: lib/toolbar/filePrint-i.xbm lib/toolbar/filePrint-i.xpm
toolbar.o: lib/toolbar/filePrint.xbm lib/toolbar/filePrint.xpm
toolbar.o: lib/toolbar/helpContext-i.xbm lib/toolbar/helpContext-i.xpm
toolbar.o: lib/toolbar/helpContext.xbm lib/toolbar/helpContext.xpm
toolbar.o: lib/toolbar/magstepDecr-i.xbm lib/toolbar/magstepDecr-i.xpm
toolbar.o: lib/toolbar/magstepDecr.xbm lib/toolbar/magstepDecr.xpm
toolbar.o: lib/toolbar/magstepIncr-i.xbm lib/toolbar/magstepIncr-i.xpm
toolbar.o: lib/toolbar/magstepIncr.xbm lib/toolbar/magstepIncr.xpm
toolbar.o: lib/toolbar/mark-i.xbm lib/toolbar/mark-i.xpm lib/toolbar/mark.xbm
toolbar.o: lib/toolbar/mark.xpm lib/toolbar/markAll-i.xbm
toolbar.o: lib/toolbar/markAll-i.xpm lib/toolbar/markAll.xbm
toolbar.o: lib/toolbar/markAll.xpm lib/toolbar/markEven-i.xbm
toolbar.o: lib/toolbar/markEven-i.xpm lib/toolbar/markEven.xbm
toolbar.o: lib/toolbar/markEven.xpm lib/toolbar/markInvert-i.xbm
toolbar.o: lib/toolbar/markInvert-i.xpm lib/toolbar/markInvert.xbm
toolbar.o: lib/toolbar/markInvert.xpm lib/toolbar/markNone-i.xbm
toolbar.o: lib/toolbar/markNone-i.xpm lib/toolbar/markNone.xbm
toolbar.o: lib/toolbar/markNone.xpm lib/toolbar/markOdd-i.xbm
toolbar.o: lib/toolbar/markOdd-i.xpm lib/toolbar/markOdd.xbm
toolbar.o: lib/toolbar/markOdd.xpm lib/toolbar/pageNext-i.xbm
toolbar.o: lib/toolbar/pageNext-i.xpm lib/toolbar/pageNext.xbm
toolbar.o: lib/toolbar/pageNext.xpm lib/toolbar/pagePrev-i.xbm
toolbar.o: lib/toolbar/pagePrev-i.xpm lib/toolbar/pagePrev.xbm
toolbar.o: lib/toolbar/pagePrev.xpm mine/toolbar.h wlib/wlib/WlToolBar.h
toolbar.o: wlib/wlib/rcs.h
track.o: mine/Ghostview.h mine/dsc.h mine/mgv.h mine/opaque.h mine/strdefs.h
track.o: mine/strings.h mine/track.h wlib/wlib/rcs.h wlib/wlib/sanity.h
track.o: wlib/wlib/sens.h wlib/wlib/strings.h
uiutil.o: mine/Ghostview.h mine/app.h mine/dsc.h mine/mgv.h mine/opaque.h
uiutil.o: mine/sens.h mine/uiutil.h wlib/wlib/app.h wlib/wlib/rcs.h
uiutil.o: wlib/wlib/sanity.h wlib/wlib/sens.h wlib/wlib/stdlib.h
uiutil.o: wlib/wlib/strdefs.h wlib/wlib/wlib.h
unmangle.o: mine/Ghostview.h mine/app.h mine/dsc.h mine/error.h mine/mgv.h
unmangle.o: mine/opaque.h mine/strdefs.h mine/strings.h mine/unmangle.h
unmangle.o: wlib/wlib/app.h wlib/wlib/rcs.h wlib/wlib/sanity.h wlib/wlib/sens.h
unmangle.o: wlib/wlib/stdlib.h wlib/wlib/strdefs.h wlib/wlib/strings.h
unmangle.o: wlib/wlib/wlib.h
view.o: mine/Ghostview.h mine/app.h mine/dsc.h mine/log.h mine/mgv.h
view.o: mine/opaque.h mine/strdefs.h mine/strings.h mine/view.h mine/zoom.h
view.o: wlib/wlib/app.h wlib/wlib/rcs.h wlib/wlib/sanity.h wlib/wlib/sens.h
view.o: wlib/wlib/strdefs.h wlib/wlib/strings.h wlib/wlib/typesanity.h
view.o: wlib/wlib/wlib.h
zoom.o: mine/Ghostview.h mine/app.h mine/dsc.h mine/error.h mine/log.h
zoom.o: mine/mgv.h mine/opaque.h mine/sens.h mine/strdefs.h mine/strings.h
zoom.o: mine/zoom.h wlib/wlib/app.h wlib/wlib/display.h wlib/wlib/rcs.h
zoom.o: wlib/wlib/sanity.h wlib/wlib/sens.h wlib/wlib/stdlib.h
zoom.o: wlib/wlib/strdefs.h wlib/wlib/strings.h wlib/wlib/util.h
zoom.o: wlib/wlib/wlib.h
