# @(#)$Mu: mgv/lib/Makefile.am 1.17 2001/03/09 16:22:43 $
##
## Makefile.am
##	automake file for mgv/lib.
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
CLEANFILES = splash-imags.tar.gz icon-images.tar.gz

SUBDIRS = toolbar

mgvmiscdir = $(pkgdatadir)/$(VERSION)
mgvmisc_DATA = \
	README \
	splash-images.tar.gz \
	icon-images.tar.gz

mgv_icons = \
	mgv-icon.xbm mgv-icon.xpm mgv-mask.xbm \
	log-icon.xbm log-icon.xpm log-mask.xbm

splash-images.tar.gz: splash.xbm splash.xpm
	tar -cf - splash.xbm splash.xpm | gzip -9 > $@

icon-images.tar.gz: $(mgv_icons)
	tar -cf - $(mgv_icons) | gzip -9 > icon-images.tar.gz
