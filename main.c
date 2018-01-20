/*
 * main.c
 *	Initialization and entry to mgv.
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
/*
 * application initialization
 */
#include <wlib/rcs.h>
MU_ID("$Mu: mgv/main.c 1.225 2001/03/18 20:06:27 $")

#include <signal.h>
#include <unistd.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <Xm/Xm.h>

#include <wlib/stdlib.h>
#include <wlib/wlib.h>
#include <wlib/options.h>
#include <wlib/msg.h>
#include <wlib/build.h>
#include <wlib/icons.h>
#include <wlib/WlAppShell.h>
#include <wlib/WlSplashScreen.h>
#include <wlib/WlToolBar.h>
#include <wlib/sanity.h>
#include <wlib/help.h>
#include <mine/mgv.h>
#include <mine/app.h>
#include <mine/help.h>
#include <mine/strdefs.h>
#include <mine/strings.h>
#include <mine/page_control.h>

#include <lib/log-icon.xbm>
#include <lib/log-icon.xpm>
#include <lib/log-mask.xbm>
#include <lib/mgv-icon.xbm>
#include <lib/mgv-icon.xpm>
#include <lib/mgv-mask.xbm>

#include <lib/splash.xbm>
#include <lib/splash.xpm>

#define	MS(x)	"*" x
#define	GS(x)	"MGv*Ghostview." x

static XrmOptionDescRec options[] = {
	/*
	 * MGv resources
	 */
	WL_OT("-aliaseps",        MS(MgvNantialiasEPS)),
	WL_OT("-autoheight",      MS(MgvNautoHeight)),
	WL_OT("-autowidth",       MS(MgvNautoWidth)),
	WL_OS("-bunzip",          MS(MgvNbunzip)),
	WL_OS("-bzip",            MS(MgvNbzip)),
	WL_OT("-center",          MS(MgvNcenter)),
	WL_OS("-compress",        MS(MgvNcompress)),
	WL_OT("-confirmexit",     MS(MgvNconfirmExit)),
	WL_OT("-copystdin",       MS(MgvNcopyStdin)),
	WL_OS("-decompress",      MS(MgvNdecompress)),
	WL_OS("-gunzip",          MS(MgvNgunzip)),
	WL_OS("-gzip",            MS(MgvNgzip)),
	WL_OT("-killkids",        MS(MgvNkillChildrenAtExit)),
	WL_OT("-labels",          MS(MgvNshowLabels)),
	WL_OF("-log",             MS(MgvNhideLog)),
	WL_OS("-magicresistance", MS(MgvNmagicResistance)),
	WL_OT("-magicscroll",     MS(MgvNmagicScrolling)),
	WL_OS("-magstep",         MS(MgvNmagstep)),
	WL_OS("-media",           MS(MgvNdefaultPageMedia)),
	WL_OF("-menubar",         MS(MgvNhideMenuBar)),
	WL_OF("-noaliaseps",      MS(MgvNantialiasEPS)),
	WL_OF("-noautoheight",    MS(MgvNautoHeight)),
	WL_OF("-noautowidth",     MS(MgvNautoWidth)),
	WL_OF("-nocenter",        MS(MgvNcenter)),
	WL_OF("-noconfirmexit",   MS(MgvNconfirmExit)),
	WL_OF("-nocopystdin",     MS(MgvNcopyStdin)),
	WL_OF("-nokillkids",      MS(MgvNkillChildrenAtExit)),
	WL_OF("-nolabels",        MS(MgvNshowLabels)),
	WL_OT("-nolog",           MS(MgvNhideLog)),
	WL_OT("-nomagicscroll",   MS(MgvNmagicScrolling)),
	WL_OT("-nomenubar",       MS(MgvNhideMenuBar)),
	WL_OT("-nopagelist",      MS(MgvNhidePageList)),
	WL_OF("-nopgnums",        MS(MgvNshowPageNumbers)),
	WL_OF("-nosmartsizing",   MS(MgvNsmartSizing)),
	WL_OF("-nosmooth",        MS(MgvNsmoothScrolling)),
	WL_OT("-nostatus",        MS(MgvNhideStatusLine)),
	WL_OF("-nosuppress",      MS(MgvNsuppressWarnings)),
	WL_OT("-notoolbar",       MS(MgvNhideToolBar)),
	WL_OF("-pagelist",        MS(MgvNhidePageList)),
	WL_OS("-paper",           MS(MgvNdefaultPageMedia)),
	WL_OS("-pdf",             MS(MgvNpdfConverter)),
	WL_OT("-pgnums",          MS(MgvNshowPageNumbers)),
	WL_OS("-printcommand",    MS(MgvNprintCommand)),
	WL_OV("-resetboth",       MS(MgvNresetScrollBars), "both"),
	WL_OV("-resethor",        MS(MgvNresetScrollBars), "hor"),
	WL_OV("-resetnone",       MS(MgvNresetScrollBars), "none"),
	WL_OV("-resetver",        MS(MgvNresetScrollBars), "ver"),
	WL_OS("-reticulefg",      MS(MgvNreticuleForeground)),
	WL_OS("-reticulewidth",   MS(MgvNreticuleLineWidth)),
	WL_OS("-scrollpercent",   MS(MgvNscrollPercentage)),
	WL_OT("-smartsizing",     MS(MgvNsmartSizing)),
	WL_OT("-smooth",          MS(MgvNsmoothScrolling)),
	WL_OS("-smoothness",      MS(MgvNsmoothness)),
	WL_OF("-status",          MS(MgvNhideStatusLine)),
	WL_OT("-suppress",        MS(MgvNsuppressWarnings)),
	WL_OF("-toolbar",         MS(MgvNhideToolBar)),
	WL_OS("-wmheight",        MS(MgvNwmHeight)),
	WL_OS("-wmwidth",         MS(MgvNwmWidth)),
	WL_OS("-zoomfg",          MS(MgvNreticuleForeground)),
	WL_OS("-zoomwidth",       MS(MgvNreticuleLineWidth)),

	/*
	 * wlib resources
	 */
	WL_OS("-browser",     MS(XwlNhelpBrowser)),
	WL_OT("-coloricon",   MS(XwlNuseColorIcon)),
	WL_OS("-helptype",    MS(XwlNhelpType)),
	WL_OT("-installicon", MS(XwlNinstallIcon)),
	WL_OF("-monoicon",    MS(XwlNuseColorIcon)),
	WL_OF("-nocoloricon", MS(XwlNuseColorIcon)),
	WL_OF("-noicon",      MS(XwlNinstallIcon)),
	WL_OS("-prefix",      MS(XwlNhelpPrefix)),
	WL_OS("-temp",        MS(XwlNtempDir)),
	WL_OT("-asserts",     MS(XwlNenableAssertions)),
	WL_OF("-noasserts",   MS(XwlNenableAssertions)),

	/*
	 * WlAppShell
	 */
	WL_OS("-depth",       MS(XwlNdepth)),
	WL_OT("-editres",     MS(XwlNuseEditRes)),
	WL_OT("-install",     MS(XwlNusePrivateColormap)),
	WL_OF("-noeditres",   MS(XwlNuseEditRes)),
	WL_OT("-noinstall",   MS(XwlNusePrivateColormap)),
	WL_OF("-noprivatecm", MS(XwlNusePrivateColormap)),
	WL_OT("-privatecm",   MS(XwlNusePrivateColormap)),
	WL_OS("-visual",      MS(XwlNvisualClass)),
	WL_OS("-visualid",    MS(XwlNvisualID)),

	/*
	 * WlSplashScreen
	 */
	WL_OF("-colorsplash", MS(XwlNuseMonoSplash)),
	WL_OT("-monosplash",  MS(XwlNuseMonoSplash)),
	WL_OF("-nosplash",    MS(XwlNshowSplashScreen)),
	WL_OT("-splash",      MS(XwlNshowSplashScreen)),
	WL_OS("-splashxbm",   MS(XwlNsplashBitmap)),
	WL_OS("-splashxpm",   MS(XwlNsplashPixmap)),

	/*
	 * WlToolBar
	 */
	WL_OT("-colortoolbar", MS(XwlNuseColorIcons)),
	WL_OT("-flatten",      MS(XwlNflattenButtons)),
	WL_OF("-monotoolbar",  MS(XwlNuseColorIcons)),
	WL_OF("-noflatten",    MS(XwlNflattenButtons)),
	WL_OF("-notooltips",   MS(XwlNshowToolTips)),
	WL_OT("-tooltips",     MS(XwlNshowToolTips)),

	/*
	 * Ghostview resources
	 */
	WL_OT("-antialias",   GS(GvNantialias)),
	WL_OS("-arguments",   GS(GvNarguments)),
	WL_OS("-gslocale",    GS(GvNgsLocale)),
	WL_OS("-interpreter", GS(GvNinterpreter)),
	WL_OV("-landscape",   GS(GvNorientation), "landscape"),
	WL_OF("-noantialias", GS(GvNantialias)),
	WL_OF("-noquiet",     GS(GvNquiet)),
	WL_OF("-nosafer",     GS(GvNsafer)),
	WL_OV("-portrait",    GS(GvNorientation), "portrait"),
	WL_OS("-psfg",        GS(GvNpostScriptForeground)),
	WL_OS("-psbg",        GS(GvNpostScriptBackground)),
	WL_OT("-quiet",       GS(GvNquiet)),
	WL_OT("-safer",       GS(GvNsafer)),
	WL_OV("-seascape",    GS(GvNorientation), "seascape"),
	WL_OV("-upsidedown",  GS(GvNorientation), "upsidedown"),
	WL_OS("-xdpi",        GS(GvNxdpi)),
	WL_OS("-ydpi",        GS(GvNydpi)),
};
#undef GS
#undef MS

/*
 * The switches don't change that much so keeping these two guys up to
 * date by hand isn't that much bother.
 */
static char *switch_longhelp[] = {
"-aliaseps, -noaliaseps",
	"\tAnti-aliasing graphic images will often make the images look",
	"\tworse and increase the rendering time significantly; hence,",
	"\tthese switches are offered to give you finer control over when",
	"\tanti-aliasing is used.  These switches set the \"antialiasEPS\"",
	"\tapplication resource.",
"-antialias, -noantialias",
	"\tEnable or disable anti-aliasing within Ghostscript; by default,",
	"\tanti-aliasing is enabled.  If anti-aliasing is enable, then the",
	"\tx11alpha device is used and Ghostscript will be run with the",
	"\t-dNOPLATFONTS switch; if anti-aliasing is disabled, then the x11",
	"\tdevice will be used.  This switch sets the \"antialias\"",
	"\tresource for Ghostview widgets.",
"-arguments args",
	"\tSpecify additional arguments for Ghostscript; this switch sets",
	"\tthe \"arguments\" resource.",
"-autoheight, -noautoheight",
	"\tAttempt to make the main window tall enough to accommodate a",
	"\twhole page.  This switch sets the \"autoHeight\" resource.",
"-autowidth, -noautowidth",
	"\tAttempt to make the main window wide enough to accommodate a",
	"\twhole page.  This switch sets the \"autoWidth\" resource.",
"-browser htmlbrowser",
	"\tSpecify the HTML browser to use for the on-line help; this",
	"\tswitch sets the \"helpBrowser\" resource.",
"-bzip cmd, -bunzip cmd",
	"\tSpecify the command to compress or decompress bzip2ed files; the",
	"\tcommands should read from the standard input and write to the",
	"\tstandard output.  These switches set the \"bzip\" and \"bunzip\"",
	"\tresources.",
"-center, -nocenter",
	"\tEnable or disable automatic page centering.  This switch sets",
	"\tthe \"center\" resource.",
"-color, -grayscale, -greyscale, -monochrome",
	"\tForce color/grayscale/monochrome display; these switches do not",
	"\thave anything to do with the visual, depth, or colormap that mgv",
	"\tuses, you will want to use the -visual, -visualid, -depth, and",
	"\t-privatecm switches for that.  These switches set the palette",
	"\tresource.",
"-coloricon, -monoicon, -nocoloricon",
	"\tUse or don't use a color icon (if available).  These switches",
	"\tset the \"wlUseColorIcon\" resource.",
"-colortoolbar, -monotoolbar",
	"\tUse color or monochrome icons in the tool bar.  These switches",
	"\tset the \"useColorIcons\" resource.",
"-compress cmd, -decompress cmd",
	"\tSpecify the command to compress or decompress compressed files;",
	"\tthe commands should read from the standard input and write to",
	"\tthe standard output.  These switches set the \"compress\" and",
	"\t\"decompress\" resources.",
"-confirmexit, -noconfirmexit",
	"\tRequire or don't require user confirmation when attempting to",
	"\texit the application with more than one top level viewing",
	"\twindow.  These switches set the \"confirmExit\" resource.",
"-copystdin, -nocopystdin",
	"\tCopy or don't copy files from the standard input to a temporary",
	"\tfile.  These switches set the \"copyStdin\" resource.",
"-depth n",
	"\tSpecify a non-default color depth.  This switch sets the",
	"\t\"wlDepth\" resource.",
"-dpi n, -resolution n",
	"\tSpecify the overall resolution; these switches set the",
	"\tResolution resource.",
"-editres, -noeditres",
	"\tInstall or don't install support for editres.  This switch sets",
	"\tthe \"wlUseEditRes\" resource.",
"-flatten, -noflatten",
	"\tFlatten or don't flatten the tool bar buttons.  These switches",
	"\tset the \"flattenButtons\" resource.",
"-gslocale locale",
	"\tForce the GHOSTVIEW window property to be written according to",
	"\tlocale.  This switch sets the \"gsLocale\" resource.",
"-gunzip cmd",
	"\tSpecify the command to compress or decompress gzipped files; the",
	"\tcommands should read from the standard input and write to the",
	"\tstandard output.  These switches set the \"gzip\" and \"gunzip\"",
	"\tresources.",
"-help, --help, -H, -?",
	"\tDisplay a short summary of the available command line switches.",
"-helptype type",
	"\tSpecify the help type for on-line help.  This switch sets the",
	"\t\"helpType\" resource to type.",
"-install, -noinstall, -privatecm, -noprivatecm",
	"\tInstall or don't install a private colormap.  Non-default visual",
	"\tand/or depth settings may cause this switch to be ignored.  This",
	"\tswitch sets the \"wlUsePrivateColormap\" resource.",
"-installicon, -noicon",
	"\tInstall or don't install an icon when the application starts up.",
	"\tThese switches set the \"wlInstallIcon\" resource.",
"-interpreter interp",
	"\tUse interp as the PostScript interpreter.  This switch is useful",
	"\tif you want to use an interpreter that is not in your PATH or is",
	"\tnot the first gs is your PATH.  This switch sets the",
	"\t\"interpreter\" resource.",
"-killkids, -nokillkids",
	"\tKill or don't kill child processes when mgv exits; this switch",
	"\tsets the \"killChildrenAtExit\" resource.",
"-labels, -nolabels",
	"\tShow or don't show the page labels in the page list; this switch",
	"\tsets the \"showLabels\" resource.",
"-log, -nolog",
	"\tShow or hide the Ghostscript log on start-up; this switch sets",
	"\tthe \"hideLog\" resource.",
"-longhelp",
	"\tDisplay a summary, with explanations, of the available command",
	"\tline switches.",
"-magicresistance n",
	"\tSpecify the number of attempts to scroll beyond the page",
	"\tboundaries before magical scrolling takes effect; this switch",
	"\tsets the \"magicResistance\" resource.",
"-magicscroll, -nomagicscroll",
	"\tAllow or disallow magical scrolling; this switch sets the",
	"\t\"magicScrolling\" resource.",
"-magstep m",
	"\tSet the initial magstep to m; this switch sets the \"magstep\"",
	"\tresource.",
"-media media, -paper media",
	"\tSpecify the default page media; this switch sets the",
	"\t\"defaultPageMedia\" resource.",
"-menubar, -nomenubar",
	"\tShow or hide the menu bar at start up; this switch sets the",
	"\t\"hideMenuBar\" resource.",
"-monosplash, -colorsplash",
	"\tUse the monochrome or the color splash screen.  These switches",
	"\tset the \"wlUseMonoSplash\" resource.",
"-pagelist, -nopagelist",
	"\tShow or hide the page list at start up; this switch sets the",
	"\t\"hidePageList\" resource.",
"-pgnums, -nopgnums",
	"\tShow or don't show the page numbers in the page list; this",
	"\tresource sets the \"showPageNumbers\" resource.",
"-portrait, -landscape, -seascape, -upsidedown",
	"\tSpecify the initial page orientation.",
"-prefix pfx",
	"\tSpecify the location of the files for the on-line help; this",
	"\tswitch sets the \"helpPrefix\" resource.",
"-printcommand cmd",
	"\tUse cmd as the default printer command; this switch sets the",
	"\t\"printCommand\" resource.",
"-quiet, -noquiet",
	"\tUse or don't use -dQUIET with Ghostscript; this switch sets the",
	"\tquiet resource.  The -dQUIET switch tells Ghostscript to",
	"\tsuppresses the normal startup messages.",
"-resetboth",
	"\tReset both the horizontal and vertical scrollbars when you move",
	"\tto a new page.  This switch sets the \"resetScrollBars\"",
	"\tresource to both.",
"-resethor",
	"\tReset the horizontal scrollbar when you move to a new page.",
	"\tThis switch sets the \"resetScrollBars\" resource to horizontal.",
"-resetnone",
	"\tDon't reset either scrollbar when you move to a new page.  This",
	"\tswitch sets the \"resetScrollBars\" resource to none.",
"-resetver",
	"\tReset the vertical scrollbar when you move to a new page.  This",
	"\tswitch sets the \"resetScrollBars\" resource to vertical.",
"-safer, -nosafer",
	"\tUse or don't use -dSAFER with Ghostscript; this switch sets the",
	"\tsafer resource.  When run with the -dSAFER switch, Ghostscript",
	"\tdisables the deletefile and renamefile operators and can only",
	"\topen files with read-only access.",
"-scrollpercent percentage",
	"\tScroll the screen by percentage of the window size when",
	"\tscrolling with the keyboard.  This switch sets the",
	"\t\"scrollPercentage\" resource.",
"-smartsizing, -nosmartsizing",
	"\tEnable or disable smart sizing.  This switch sets the",
	"\t\"smartSizing\" resource.",
"-smooth, -nosmooth",
	"\tEnable or disable smooth scrolling.  This switch sets the",
	"\t\"smoothScrolling\" resource.",
"-smoothness n",
	"\tSpecify how smooth smooth scrolling should be.  This switch sets",
	"\tthe \"smoothness\" resource to n.",
"-splash, -nosplash",
	"\tShow or don't show the splash screen.",
"-splashxbm filename",
	"\tSpecify the splash screen XBM file.  This switch sets the",
	"\t\"wlSplashBitmap\" resource.",
"-splashxpm filename",
	"\tSpecify the splash screen XPM file.  This switch sets the",
	"\t\"wlSplashPixmap\" resource.",
"-status, -nostatus",
	"\tShow or hide the status line at start up; this switch sets the",
	"\t\"hideStatusLine\" resource.",
"-temp dir",
	"\tSpecify the temporary directory; this switch sets the",
	"\t\"tempDir\" resource.",
"-toolbar, -notoolbar",
	"\tShow or hide the tool bar at start up; this switch sets the",
	"\t\"hideToolBar\" resource.",
"-tooltips, -notooltips",
	"\tShow or don't show tool tips for the tool bar buttons.  These",
	"\tswitches set the \"showToolTips\" resource.",
"-version, --version",
	"\tDisplay lots of version and build information about mgv.  The",
	"\tdisplayed information is the same as that in the \"Build Options",
	"\tDialog\".",
"-visual visualClass",
	"\tSpecify a non-default visual.  This switch sets the",
	"\t\"wlVisualClass\" resource.",
"-visualid visualID",
	"\tSpecify a non-default visual by visual id (see xdpyinfo for a",
	"\tlist of visual ids).  This switch sets the \"wlVisualID\"",
	"\tresource.",
"-wmwidth width, -wmheight height",
	"\tSet the total width or height of the window manager decorations.",
	"\tThese switches set the \"wmWidth\" and \"wmHeight\" resources.",
"-xdpi n, -ypdi n",
	"\tSpecify the horizontal and vertical resolutions; these switches",
	"\tset the xdpi and ydpi resources.",
"-zoomfg color, -reticulefg color",
	"\tSpecify the zoom reticule color; these switches set the",
	"\t\"reticuleForeground\" resource.",
"-zoomwidth n, -reticulewidth n",
	"\tSpecify the zoom reticule width; these switches set the",
	"\t\"reticuleLineWidth\" resource.",
(char *)0
};

static char *switch_help[] = {
	"[-help]",
	"[-longhelp]",
	"[--help]",
	"[--version]",
	"[-?]",
	"[-H]",
	"[-[no]aliaseps]",
	"[-[no]antialias]",
	"[-[no]autoheight]",
	"[-[no]autowidth]",
	"[-[no]center]",
	"[-[no]confirmexit]",
	"[-[no]copystdin]",
	"[-[no]editres]",
	"[-[no]flatten]",
	"[-[no]install]",
	"[-[no]killkids]",
	"[-[no]labels]",
	"[-[no]log]",
	"[-[no]magicscroll]",
	"[-[no]menubar]",
	"[-[no]pagelist]",
	"[-[no]pgnums]",
	"[-[no]quiet]",
	"[-[no]safer]",
	"[-[no]smartsizing]",
	"[-[no]smooth]",
	"[-[no]splash]",
	"[-[no]status]",
	"[-[no]toolbar]",
	"[-[no]tooltips]",
	"[-arguments args]",
	"[-browser htmlbrowser]",
	"[-bunzip cmd]",
	"[-bzip cmd]",
	"[-color]",
	"[-coloricon]",
	"[-colorsplash]",
	"[-colortoolbar]",
	"[-compress cmd]",
	"[-decompress cmd]",
	"[-depth n]",
	"[-dpi n]",
	"[-grayscale]",
	"[-greyscale]",
	"[-gslocale locale]",
	"[-gunzip cmd]",
	"[-helptype type]",
	"[-installicon]",
	"[-interpreter interp]",
	"[-landscape]",
	"[-magicresistance n]",
	"[-magstep m]",
	"[-media media]",
	"[-monochrome]",
	"[-monoicon]",
	"[-monosplash]",
	"[-monotoolbar]",
	"[-nocoloricon]",
	"[-noicon]",
	"[-paper media]",
	"[-portrait]",
	"[-prefix pfx]",
	"[-printcommand cmd]",
	"[-resetboth]",
	"[-resethor]",
	"[-resetnone]",
	"[-resetver]",
	"[-resolution n]",
	"[-reticulefg color]",
	"[-reticulewidth n]",
	"[-scrollpercent percentage]",
	"[-seascape]",
	"[-smoothness n]",
	"[-splashxbm filename]",
	"[-splashxpm filename]",
	"[-temp dir]",
	"[-upsidedown]",
	"[-version]",
	"[-visual visualClass]",
	"[-visualid visualID]",
	"[-wmheight height]",
	"[-wmwidth width]",
	"[-xdpi n]",
	"[-ypdi n]",
	"[-zoomfg color]",
	"[-zoomwidth n]",
	(char *)0
};

static int
helpem_long(char *me, int ret)
{
	int	i;

	printf("%s [options] [files...]\n", me);
	for(i = 0; switch_longhelp[i] != NULL; ++i)
		printf("%s\n", switch_longhelp[i]);
	return ret;
}

static int
helpem(char *me, int ret)
{
#define	LINELEN	75
	int	i, len, slen;

	printf("%s [options] [files...]", me);
	for(i = 0, len = LINELEN + 100; switch_help[i] != NULL; ++i) {
		slen = strlen(switch_help[i]);
		if(len + slen + 1 > LINELEN) {
			printf("\n\t");
			len = 8;
		}
		printf("%s ", switch_help[i]);
		len += slen + 1;
	}
	printf("\n");
	return ret;
#undef	LINELEN
}

static int
version(void)
{
	char *buf;

	printf("%s\n\n", buf = wl_version());
	printf("Copyright (c) 1996-2001 Eric A. Howe, Matthew D. Francey\n");
	printf("This is free software with ABSOLUTELY NO WARRANTY.  ");
	printf("See COPYING for details.\n");
	wl_free(buf);
	return EXIT_SUCCESS;
}

/*
 * POSIX doesn't say that fileno() is safe in a signal handler
 * so we grab the file descriptor of stderr (POSIX doesn't appear
 * to force it be 2 even though we all think it is).  Also, we'll
 * be redirecting stderr later on so we want our own copy that isn't
 * really stderr.
 *
 * Okay, okay, we're only using this in a signal handler for SIGSEGV
 * and friends so who cares about safety?  I do, if I'm going to screw up,
 * I'd like to do it properly.
 */
static	int	fd_stderr;

/*
 * Some people don't have SIGBUS but I don't want to put in more
 * than one ifdef to handle it.  Similar problems seem to affect SIGTRAP.
 * I use SIGUSR1 here because its the only value that is really safe to use.
 */
#if !defined(SIGBUS)
#	define	SIGBUS	SIGUSR1
#endif
#if !defined(SIGTRAP)
#	define	SIGTRAP	SIGUSR1
#endif

static void flaming_death(int);
#define	DEATH(sig)	{sig, flaming_death, #sig, sizeof(#sig)}
/*
 * the name field, and hence the n_name field, is only needed if
 * the handler is set to flaming_death
 */
static struct {
	int	sig;
	void	(*handler)(int);
	char	*name;
	int	n_name;
} sigs[] = {
	DEATH(SIGABRT),
	DEATH(SIGBUS),
	DEATH(SIGFPE),
	DEATH(SIGILL),
	DEATH(SIGSEGV),
	DEATH(SIGTRAP),

	{SIGCHLD, SIG_DFL, NULL, 0},
	{SIGPIPE, SIG_DFL, NULL, 0},
	{SIGTERM, SIG_DFL, NULL, 0},
};
#define	N_SIGS	(int)(sizeof(sigs)/sizeof(sigs[0]))

/*
 * Just because I screwed up doesn't mean I have to be
 * messy about it!
 */
static char swansong[] =
	"\tSomething in mgv broke!\n"
	"\tYou found a bug in mgv--I'm trying to die gracefully--if you\n"
	"\tcan reproduce the bug, please send a bug report to mu@trends.net\n";
static void
flaming_death(int sig)
{
	int	i;

	for(i = 0; i < N_SIGS; ++i)
		if(sigs[i].sig == sig)
			break;
	if(i != N_SIGS)
		write(fd_stderr, (void *)sigs[i].name, sigs[i].n_name);

	/*
	 * Remember:  only the C libraries know what the trailing nul
	 * character means.
	 */
	write(fd_stderr, (void *)swansong, sizeof(swansong) - 1);

	kill(-getpgrp(), SIGTERM);

	/*
	 * I stole this idea from ImageMagick, seems like a good idea.
	 */
	exit(sig);
}

static void
sig_init(void)
{
	struct sigaction sa;
	int	i;

	/*
	 * We want to lead our process group so that flaming_death
	 * will do the right thing.  All of this may seem a little
	 * excessive just to deal with SIGSEGV and SIGABRT but I'm
	 * sick of writting little awk one-liners to clean up the
	 * left over gs processes when I find a bug in mgv.
	 *
	 * Besides, I thought it was really cool when I trapped
	 * my first seg fault and died gracefully.
	 *
	 * I suppose I could keep a list of all the child processes
	 * that have been made (and maintain this list) but that is
	 * just a big pain in the ass; using process groups is just
	 * too cute and convenient a trick not to use it.
	 */
	setpgid(getpid(), getpid());

	fd_stderr = dup(fileno(stderr));

	memset((void *)&sa, '\0', sizeof(sa));
	sigemptyset(&sa.sa_mask);
	for(i = 0; i < N_SIGS; ++i) {
		if(sigs[i].sig == SIGUSR1)
			continue;
		sa.sa_handler = sigs[i].handler;
		sigaction(sigs[i].sig, &sa, NULL);
	}
}

static char **
extract_switches(int *ac, char **argv)
{
	int	i, j;
	int	argc      = *ac;
	char	**missing = NULL;
	int	n_missing = 0;

	for(i = 1; i < argc; ) {
		if(strcmp(argv[i], "--") == 0)
			break;
		if(*argv[i] != '-') {
			++i;
			continue;
		}

		missing = wl_realloc(missing, (n_missing + 1)*sizeof(char *));
		missing[n_missing++] = argv[i];

		for(j = i; j < argc; ++j)
			argv[j] = argv[j + 1];
		--argc;
	}
	*ac = argc;

	/*
	 * And terminate the list if necessary.
	 */
	if(missing != NULL) {
		missing = wl_realloc(missing, (n_missing + 1)*sizeof(char *));
		missing[n_missing++] = NULL;
	}

	return missing;
}

static void
report_badswitches(Widget w, char **unknown)
{
	int   i, size;
	char *msg;
	char *sep     = "\n    ";
	int   sepsize = strlen(sep);

	if(unknown == NULL)
		return;

	/*
	 * Spin through once to get the buffer size.
	 */
	for(i = size = 0; unknown[i] != NULL; ++i)
		size += strlen(unknown[i]) + sepsize;
	size += 3;
	msg = wl_calloc(size, 1);

	/*
	 * And again to fill things in.
	 */
	for(i = 0; unknown[i] != NULL; ++i)
		strcat(strcat(msg, sep), unknown[i]);
	strcat(msg, "\n");

	wl_warn(w, wl_s(w, MgvSunknownSwitch), msg);

	wl_free(unknown);
}

static void
nonx_options(char *me, char **argv)
{
	for(++argv; *argv != NULL; ++argv) {
		if(strcmp(*argv, "-help")  == 0
		|| strcmp(*argv, "--help") == 0
		|| strcmp(*argv, "-H")     == 0
		|| strcmp(*argv, "-?")     == 0)
			exit(helpem(me, EXIT_SUCCESS));
		if(strcmp(*argv, "-longhelp")  == 0
		|| strcmp(*argv, "--longhelp") == 0)
			exit(helpem_long(me, EXIT_SUCCESS));
		if(strcmp(*argv, "-version")  == 0
		|| strcmp(*argv, "--version") == 0)
			exit(version());
	}
}

static Widget
make_splash(Display *dpy)
{
	Arg      a[4];
	Cardinal n;

	n = 0;
	XtSetArg(a[n], XwlNpixMapData,   splash_xpm);		++n;
	XtSetArg(a[n], XwlNbitMapData,   splash_bits);		++n;
	XtSetArg(a[n], XwlNbitMapWidth,  splash_width);		++n;
	XtSetArg(a[n], XwlNbitMapHeight, splash_height);	++n;
	ARGSANITY(a, n);

	return XtAppCreateShell(NULL, "MGv", wlSplashScreenWidgetClass,dpy,a,n);
}

static WLI mgv_icons = {
	mgv_icon_xpm,
	(char *)mgv_icon_bits, mgv_icon_width, mgv_icon_height,
	(char *)mgv_mask_bits, mgv_mask_width, mgv_mask_height
};
static WLI log_icons = {
	log_icon_xpm,
	(char *)log_icon_bits, log_icon_width, log_icon_height,
	(char *)log_mask_bits, log_mask_width, log_mask_height
};

int
main(int argc, char **argv)
{
	MGV	*m;
	char	*me;
	Widget	top, splash;
	Display	*dpy;
	WLAPP   app;
	char	**unknown;

	/*
	 * prepare for the worst
	 */
	sig_init();

	if((me = strrchr(argv[0], '/')) == NULL)
		me = argv[0];
	else
		++me;

	/*
	 * Yank out -help and friends before anything else
	 * happens.  There's really no reason to crank up all of
	 * Xt just to show some command-line only information.
	 */
	nonx_options(me, argv);

	/*
	 * Crank Xt into life.
	 *
	 * We don't use the Xt convenience functions since we
	 * want several top level shells.
	 */
	app = wl_init(&argc, argv, "MGv", &options[0], XtNumber(options),
							&bld_fallbacks[0]);
	if(app == NULL) {
		fprintf(stderr, "mgv: could not initialize!\n");
		return EXIT_FAILURE;
	}
	dpy = wl_app_dpy(app);
	mgv_app_set(app);

	wl_addclass("Ghostview", ghostviewWidgetClass, NULL, FALSE);
	wl_addclass("WlToolBar", wlToolBarWidgetClass, NULL, FALSE);

	wl_icons_install(WL_ICON_MAIN, &mgv_icons);
	wl_icons_install("log",        &log_icons);

	unknown = extract_switches(&argc, argv);

	splash = make_splash(dpy);

	mgv_strings_init();

	/*
	 * Here we go...
	 */
	++argv;
	do {
		char *file = *argv;

		/*
		 * Fix up the standard "-" -> stdin naming.
		 */
		if(file != NULL
		&& strcmp(file, "-") == 0)
			file = NULL;

		top = XtAppCreateShell(NULL, "MGv", wlAppShellWidgetClass,
								dpy, NULL, 0);

		if((m = mgv_create(top, file)) == NULL) {
			fprintf(stderr, "%s: could not create top level window"
					"for %s.\n", me,
					file == NULL ? "(stdin)" : file);
			XtDestroyWidget(top);
			top = NULL;
		}
		else {
			mgv_pgstack_goto(m->pgstack, 0);
		}
	} while(*argv != NULL && *++argv != NULL);

	XtDestroyWidget(splash);

	if(top == NULL) {
		fprintf(stderr, "%s: could not create any top level windows\n"
							"\texitting.\n", me);
		exit(EXIT_FAILURE);
	}

	report_badswitches(top, unknown);

	XtAppMainLoop(XtWidgetToApplicationContext(top));

	/*
	 * Sacrifice for the angry compiler gods.
	 */
	return 0;
}
