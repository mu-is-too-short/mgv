dnl
dnl motif.m4
dnl	local automake/autoconf/m4 fun for motif.
dnl	This was taken from DDD.
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

AC_OUTPUT_COMMANDS([# @(#)$Mu: mgv/config/motif.m4 1.1 2001/03/09 04:17:22 $])

dnl
dnl MU_FIND_MOTIF
dnl --------------
dnl
dnl Find Motif libraries and headers
dnl Put Motif include directory in motif_includes,
dnl put Motif library directory in motif_libraries,
dnl and add appropriate flags to X_CFLAGS and X_LIBS.
dnl
dnl Borrowed from DDD.
dnl
AC_DEFUN(MU_FIND_MOTIF, [
	AC_REQUIRE([AC_PATH_XTRA])
	motif_includes=
	motif_libraries=
	AC_ARG_WITH(motif-includes, [  --with-motif-includes=DIR        Motif include files are in DIR], motif_includes="$withval")
	AC_ARG_WITH(motif-libraries, [  --with-motif-libraries=DIR       Motif libraries are in DIR], motif_libraries="$withval")

	AC_MSG_CHECKING(for Motif)

	##
	## Headers first.
	##

	if test "$motif_includes" = ""; then
		AC_CACHE_VAL(mu_cv_motif_includes, [
			mu_motif_save_LIBS="$LIBS"
			mu_motif_save_CFLAGS="$CFLAGS"
			mu_motif_save_CPPFLAGS="$CPPFLAGS"
			mu_motif_save_LDFLAGS="$LDFLAGS"

			LIBS="$X_PRE_LIBS -lXm -lXt -lX11 $X_EXTRA_LIBS $LIBS"
			CFLAGS="$X_CFLAGS $CFLAGS"
			CPPFLAGS="$X_CFLAGS $CPPFLAGS"
			LDFLAGS="$X_LIBS $LDFLAGS"

			AC_TRY_COMPILE([#include <Xm/Xm.h>], [int a;], [
				##
				## Headers are in the standard include path.
				##
				mu_cv_motif_includes=
			], [
				##
				## Xm/Xm.h is not in the standard search path.
				## Locate it and put its directory in
				## `motif_includes'
				##
				## /usr/include/Motif* are used on HP-UX
				## /usr/include/X11* are used on HP-UX
				## /usr/dt is used on CDE systems
				## /usr/openwin is used on Solaris
				## Other directories are just guesses.
				##
				for dir in "$x_includes" \
						"${prefix}/include" \
						/usr/include \
						/usr/local/include \
						/usr/include/Motif* \
						/usr/include/X11* \
						/usr/dt/include \
						/usr/openwin/include \ \
						/usr/dt/*/include \
						/opt/*/include \
						"${prefix}"/*/include \
						/usr/*/include \
						/usr/local/*/include \
						"${prefix}"/include/* \
						/usr/include/* \
						/usr/local/include/*; do
					if test -f "$dir/Xm/Xm.h"; then
						mu_cv_motif_includes="$dir"
						break
					fi
				done
				if test "$mu_cv_motif_includes" = ""; then
					mu_cv_motif_includes=no
				fi
			])

			LIBS="$mu_motif_save_LIBS"
			CFLAGS="$mu_motif_save_CFLAGS"
			CPPFLAGS="$mu_motif_save_CPPFLAGS"
			LDFLAGS="$mu_motif_save_LDFLAGS"
		])
		motif_includes="$mu_cv_motif_includes"
	fi

	##
	## And now for the libraries.
	##
	if test "$motif_libraries" = ""; then
		AC_CACHE_VAL(mu_cv_motif_libraries, [
			mu_motif_save_LIBS="$LIBS"
			mu_motif_save_CFLAGS="$CFLAGS"
			mu_motif_save_CPPFLAGS="$CPPFLAGS"
			mu_motif_save_LDFLAGS="$LDFLAGS"

			LIBS="$X_PRE_LIBS -lXm -lXt -lX11 $X_EXTRA_LIBS $LIBS"
			CFLAGS="$X_CFLAGS $CFLAGS"
			CPPFLAGS="$X_CFLAGS $CPPFLAGS"
			LDFLAGS="$X_LIBS $LDFLAGS"

			##
			## We use XmRepTypeInstallTearOffModelConverter() here
			## since it takes no arguments and thus also works
			## with a C++ compiler.
			##
			AC_TRY_LINK([
				#include <X11/Intrinsic.h>
				#include <Xm/Xm.h>
			], [XmRepTypeInstallTearOffModelConverter();], [
				##
				## libXm is in the standard search path.
				##
				mu_cv_motif_libraries=
			], [
				##
				## libXm.a is not in the standard search path.
				## Locate it and put its directory in
				## `motif_libraries'
				##
				## /usr/lib/Motif* are used on HP-UX
				## /usr/lib/X11* are used on HP-UX
				## /usr/dt is used on CDE systems
				## /usr/lesstif is sometimes used with LessTif
				## /usr/openwin is used on Solaris
				## Other directories are just guesses.
				##
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
						/usr/lesstif*/lib \
						/usr/lib/Lesstif* \
						"${prefix}"/*/lib \
						/usr/*/lib \
						/usr/local/*/lib \
						"${prefix}"/lib/* \
						/usr/lib/* \
						/usr/local/lib/*; do
					if test -d "$dir" \
					&& test "`ls $dir/libXm.* 2>/dev/null`" != ""; then
						mu_cv_motif_libraries="$dir"
						break
					fi
				done
				if test "$mu_cv_motif_libraries" = ""; then
					mu_cv_motif_libraries=no
				fi
			])
			LIBS="$mu_motif_save_LIBS"
			CFLAGS="$mu_motif_save_CFLAGS"
			CPPFLAGS="$mu_motif_save_CPPFLAGS"
			LDFLAGS="$mu_motif_save_LDFLAGS"
		])
		motif_libraries="$mu_cv_motif_libraries"
	fi

	##
	## Add Motif definitions to X flags.
	##

	if test "$motif_includes" != "" \
	&& test "$motif_includes" != "$x_includes" \
	&& test "$motif_includes" != "no"; then
		X_CFLAGS="-I$motif_includes $X_CFLAGS"
	fi

	if test "$motif_libraries" != "" \
	&& test "$motif_libraries" != "$x_libraries" \
	&& test "$motif_libraries" != "no"; then
		case "$X_LIBS" in
		*-R\ *)	mu_motif_R="-R $motif_libraries";;
		*-R*)	mu_motif_R="-R$motif_libraries";;
		*)	mu_motif_R="";;
		esac
		X_LIBS="-L$motif_libraries $mu_motif_R $X_LIBS"
	fi

	motif_libraries_result="$motif_libraries"
	motif_includes_result="$motif_includes"

	if test "$motif_libraries_result" = ""; then
		motif_libraries_result="in default path"
	elif test "$motif_libraries_result" = "no"; then
		motif_libraries_result="(none)"
	fi
	if test "$motif_includes_result" = ""; then
		motif_includes_result="in default path"
	elif test "$motif_includes_result" = "no"; then
		motif_includes_result="(none)"
	fi
	AC_MSG_RESULT([libraries $motif_libraries_result, headers $motif_includes_result])
])dnl
