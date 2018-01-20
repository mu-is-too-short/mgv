# @(#)$Mu: mgv/lib/toolbar/Makefile.am 1.13 1998/11/22 13:41:35 $
##
## Makefile.am
##	automake file for mgv/lib/toolbar.
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
CLEANFILES = toolbar-images.tar.gz

mu_bitmaps = \
	fileOpen.xbm	fileOpen-i.xbm		\
	filePrint.xbm	filePrint-i.xbm		\
	helpContext.xbm	helpContext-i.xbm	\
	magstepDecr.xbm	magstepDecr-i.xbm	\
	magstepIncr.xbm	magstepIncr-i.xbm	\
	mark.xbm	mark-i.xbm		\
	markAll.xbm	markAll-i.xbm		\
	markEven.xbm	markEven-i.xbm		\
	markInvert.xbm	markInvert-i.xbm	\
	markNone.xbm	markNone-i.xbm		\
	markOdd.xbm	markOdd-i.xbm		\
	pageNext.xbm	pageNext-i.xbm		\
	pagePrev.xbm	pagePrev-i.xbm

mu_pixmaps = \
	fileOpen.xpm	fileOpen-i.xpm		\
	filePrint.xpm	filePrint-i.xpm		\
	helpContext.xpm	helpContext-i.xpm	\
	magstepDecr.xpm	magstepDecr-i.xpm	\
	magstepIncr.xpm	magstepIncr-i.xpm	\
	mark.xpm	mark-i.xpm		\
	markAll.xpm	markAll-i.xpm		\
	markEven.xpm	markEven-i.xpm		\
	markInvert.xpm	markInvert-i.xpm	\
	markNone.xpm	markNone-i.xpm		\
	markOdd.xpm	markOdd-i.xpm		\
	pageNext.xpm	pageNext-i.xpm		\
	pagePrev.xpm	pagePrev-i.xpm

toolbardir = $(pkgdatadir)/$(VERSION)
toolbar_DATA = toolbar-images.tar.gz

toolbar-images.tar.gz: $(mu_bitmaps) $(mu_pixmaps)
	tar cf - $(mu_bitmaps) $(mu_pixmaps) | gzip -9 > $@
