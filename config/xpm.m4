dnl
dnl xpm.m4
dnl	local automake/autoconf/m4 fun for finding xpm.
dnl	These macros are taken from the DDD configure stuff.
dnl
dnl Copyright (C) 1998 Eric A. Howe
dnl
dnl This program is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 2 of the License, or
dnl (at your option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program; if not, write to the Free Software
dnl Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
dnl
dnl   Authors:	Eric A. Howe (mu@trends.net)
dnl

AC_OUTPUT_COMMANDS([# @(#)$Mu: mgv/config/xpm.m4 1.1 2001/03/09 04:17:22 $])

dnl
dnl MU_FIND_XPM
dnl ---------------
dnl
dnl Find Xpm libraries and headers.
dnl Put Xpm include directory in xpm_includes,
dnl put Xpm library directory in xpm_libraries,
dnl and add appropriate flags to X_CFLAGS and X_LIBS.
dnl
dnl Borrowed from DDD.
dnl
AC_DEFUN(MU_FIND_XPM, [
	AC_REQUIRE([AC_PATH_XTRA])
	xpm_includes=
	xpm_libraries=
	AC_ARG_WITH(xpm, [  --without-xpm                    do not use the Xpm library])

	dnl
	dnl Treat --without-xpm like
	dnl --without-xpm-includes --without-xpm-libraries.
	dnl
	if test "$with_xpm" = "no"; then
		xpm_includes=no
		xpm_libraries=no
	fi
	AC_ARG_WITH(xpm-includes, [  --with-xpm-includes=DIR          Xpm include files are in DIR], xpm_includes="$withval")
	AC_ARG_WITH(xpm-libraries, [  --with-xpm-libraries=DIR         Xpm libraries are in DIR], xpm_libraries="$withval")

	AC_MSG_CHECKING(for Xpm)

	##
	## Search the include files.  Note that XPM can come in <X11/xpm.h> (as
	## in X11R6) or in <xpm.h> if installed locally.
	##
	if test "$xpm_includes" = ""; then
		AC_CACHE_VAL(mu_cv_xpm_includes, [
			mu_xpm_save_LIBS="$LIBS"
			mu_xpm_save_CFLAGS="$CFLAGS"
			mu_xpm_save_CPPFLAGS="$CPPFLAGS"
			mu_xpm_save_LDFLAGS="$LDFLAGS"

			LIBS="$X_PRE_LIBS -lXpm -lXt -lX11 $X_EXTRA_LIBS $LIBS"
			CFLAGS="$X_CFLAGS $CFLAGS"
			CPPFLAGS="$X_CFLAGS $CPPFLAGS"
			LDFLAGS="$X_LIBS $LDFLAGS"

			AC_TRY_COMPILE([
				#include <X11/xpm.h>
			], [int a;], [
				mu_cv_xpm_includes=
			], [
				for dir in "$x_includes" \
						"${prefix}/include" \
						/usr/include \
						/usr/local/include \
						/usr/include/Motif* \
						/usr/include/X11* \
						/usr/dt/include \
						/usr/openwin/include \
						/usr/dt/*/include \
						/opt/*/include \
						"${prefix}"/*/include \
						/usr/*/include \
						/usr/local/*/include \
						"${prefix}"/include/* \
						/usr/include/* \
						/usr/local/include/*; do
					if test -f "$dir/X11/xpm.h" \
					|| test -f "$dir/xpm.h"; then
						mu_cv_xpm_includes="$dir"
						break
					fi
				done
			])

			LIBS="$mu_xpm_save_LIBS"
			CFLAGS="$mu_xpm_save_CFLAGS"
			CPPFLAGS="$mu_xpm_save_CPPFLAGS"
			LDFLAGS="$mu_xpm_save_LDFLAGS"
		])
		xpm_includes="$mu_cv_xpm_includes"
	fi

	##
	## And now for the libraries.
	##

	if test "$xpm_libraries" = ""; then
		AC_CACHE_VAL(mu_cv_xpm_libraries, [
			mu_xpm_save_LIBS="$LIBS"
			mu_xpm_save_CFLAGS="$CFLAGS"
			mu_xpm_save_CPPFLAGS="$CPPFLAGS"
			mu_xpm_save_LDFLAGS="$LDFLAGS"

			LIBS="$X_PRE_LIBS -lXpm -lXt -lX11 $X_EXTRA_LIBS $LIBS"
			CFLAGS="$X_CFLAGS $CFLAGS"
			CPPFLAGS="$X_CFLAGS $CPPFLAGS"
			LDFLAGS="$X_LIBS $LDFLAGS"

			##
			## We use XtToolkitInitialize() here since it takes no
			## arguments and thus also works with a C++ compiler.
			##
			AC_TRY_LINK([
				#include <X11/Intrinsic.h>
				#include <X11/xpm.h>
			], [XtToolkitInitialize();], [
				mu_cv_xpm_libraries=
			], [
				for dir in "$x_libraries" \
						"${prefix}/lib" \
						/usr/lib \
						/usr/local/lib \
						/usr/lib/Motif* \
						/usr/lib/X11* \
						/usr/dt/lib \
						/usr/openwin/lib \
						/usr/dt/*/lib \
						/opt/*/lib \
						"${prefix}"/*/lib \
						/usr/*/lib \
						/usr/local/*/lib \
						"${prefix}"/lib/* \
						/usr/lib/* \
						/usr/local/lib/*; do
					if test -d "$dir" \
					&& test "`ls $dir/libXpm.* 2>/dev/null`" != ""; then
						mu_cv_xpm_libraries="$dir"
						break
					fi
				done
			])

			LIBS="$mu_xpm_save_LIBS"
			CFLAGS="$mu_xpm_save_CFLAGS"
			CPPFLAGS="$mu_xpm_save_CPPFLAGS"
			LDFLAGS="$mu_xpm_save_LDFLAGS"
		])
		xpm_libraries="$mu_cv_xpm_libraries"
	fi

	##
	## Add Xpm definitions to X flags
	##

	if test "$xpm_includes" != "" \
	&& test "$xpm_includes" != "$x_includes" \
	&& test "$xpm_includes" != "no"; then
		X_CFLAGS="-I$xpm_includes $X_CFLAGS"
	fi

	if test "$xpm_libraries" != "" \
	&& test "$xpm_libraries" != "$x_libraries" \
	&& test "$xpm_libraries" != "no"; then
		case "$X_LIBS" in
		*-R\ *) mu_xpm_R="-R $xpm_libraries";;
		*-R*)   mu_xpm_R="-R$xpm_libraries";;
		*)      mu_xpm_R="";;
		esac
		X_LIBS="-L$xpm_libraries $mu_xpm_R $X_LIBS"
	fi

	xpm_libraries_result="$xpm_libraries"
	xpm_includes_result="$xpm_includes"

	if test "$xpm_libraries_result" = ""; then
		xpm_libraries_result="in default path"
	elif test "$xpm_libraries_result" = "no"; then
		xpm_libraries_result="(none)"
	fi

	if test "$xpm_includes_result" = ""; then
		xpm_includes_result="in default path"
	elif test "$xpm_includes_result" = "no"; then
		xpm_includes_result="(none)"
	fi

	AC_MSG_RESULT([libraries $xpm_libraries_result, headers $xpm_includes_result])
])dnl
