dnl
dnl cc.m4
dnl	autoconf m4 fun for working around debug/optimize things in
dnl	AC_PROG_CC.
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

AC_OUTPUT_COMMANDS([# @(#)$Mu: mgv/config/cc.m4 1.1 2001/03/09 04:17:22 $])

dnl
dnl AC_PROG_CC replacement.
dnl
dnl The -DMU_AUTOKLUDGE business is an interesting study in kludging around
dnl code that is trying to be smarter than it really is.  If CFLAGS doesn't
dnl have anything in it, AC_PROG_CC will put things in it for us; in
dnl particular, the dreaded debug flag, -g, will appear under various
dnl circumstances.  I don't want debug information it be around by default,
dnl I want optimizations turned on and debug turned off unless someone asks
dnl for different settings via --disable-optimize and/or --enable-debug.
dnl The problem is that I don't know what compiler optimization flags to
dnl use until the compiler is known; hence, the debug/optimization settings
dnl cannot be dealt with until AC_PROG_CC has done its thing.  To keep
dnl AC_PROG_CC from polluting CFLAGS with debug settings we have to make sure
dnl that something is in CFLAGS before AC_PROG_CC executes.  I have chosen
dnl to slip a -DMU_AUTOKLUDGE into CFLAGS since it should be completely
dnl harmless and it is easy to take out later.  This whole thing is just
dnl a way to let me use -O2, -xO2, or plain -O depending on what works.
dnl
dnl This macro runs AC_PROG_CC (with a little trick) and AC_ISC_POSIX.
dnl After that, we check for --enable-debug and --enable-optimize switches
dnl (and their --disable counterparts of course) to pick up debug and/or
dnl optimization flags for the C compiler.
dnl
AC_DEFUN(MU_PROG_CC, [
	CFLAGS="${CFLAGS:--DMU_AUTOKLUDGE}"
	AC_PROG_CC
	CFLAGS=`echo $CFLAGS | sed 's/-DMU_AUTOKLUDGE//'`

	dnl
	dnl We must call AC_ISC_POSIX before AC_TRY_COMPILE or we
	dnl get some warnings.
	dnl
	AC_ISC_POSIX

	dnl
	dnl Check for debug options.
	dnl
	mu_debug=${DEBUGFLAG:--g}
	AC_ARG_ENABLE(debug,
	[  --enable-debug          turn on debugging [default=no]], [
		if test "x${enable_debug}" != "xyes"; then
			mu_debug=""
		fi
	], [
		mu_debug=""
	])
	if test "x${mu_debug}" != "x"; then
		dnl
		dnl We could end up with multiple -g's in here but I really
		dnl don't care, if the compiler ends up caring then too bad,
		dnl so there!
		dnl
		CFLAGS="$mu_debug $CFLAGS"
	fi

	dnl
	dnl Check for various optimization settings.
	dnl
	mu_opt="yes"
	AC_ARG_ENABLE(optimize,
	[  --enable-optimize       turn on optimization [default=yes]],
		if test "x$enable_optimize" = "xno"; then
			mu_opt="no"
		fi
	)
	if test "x${mu_opt}" != "xyes"; then
		OPTFLAG=""
		mu_opt_works="no"
	elif test "x${OPTFLAG}" != "x"; then
		dnl
		dnl They've specified a specific optimization flag so we'll be
		dnl nice and use it (besides, the docs say we will and matching
		dnl the docs is generally a good idea).
		dnl
		CFLAGS="$OPTFLAG $CFLAGS"
		mu_opt_works="yes"
	else
		dnl
		dnl We try -O2 before -xO2 since -O2 will usually work and gcc
		dnl seems to just warn that "language O2 is unknown" with -xO2
		dnl but the compile doesn't fail, sigh.  Hopefully Sun's
		dnl compiler will fail with -O2 so that we can pick up -xO2.
		dnl
		mu_cflags="$CFLAGS"
		mu_opt_works="no"
		for OPTFLAG in -O2 -xO2 -O ""; do
			if test $mu_opt_works != "yes"; then
				CFLAGS="$OPTFLAG $mu_cflags"
				AC_TRY_COMPILE([],[],
					mu_opt_works=yes,
					mu_opt_works=no
				)
			fi
		done
	fi

	dnl
	dnl If we've got an optimization flag but no debug flag, then
	dnl we want to strip the final binaries.
	dnl
	if test "x${mu_opt_works}" != "x"; then
		if test "x${mu_debug}" = "x"; then
			LDFLAGS="${STRIPFLAG:--s} $LDFLAGS"
		fi
	fi
])
