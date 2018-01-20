/*
 * app.c
 *	Application config utilities.
 *
 * Copyright (C) 1998 Eric A. Howe
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
 *   Authors:	Eric A. Howe (mu@trends.net)
 */
#include <wlib/rcs.h>
MU_ID("$Mu: mgv/wlib/app.c 1.2 2001/03/19 19:31:41 $")

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include <wlib/stdlib.h>
#include <wlib/app.h>
#include <wlib/appP.h>
#include <wlib/wlib.h>
#include <wlib/resource.h>
#include <wlib/strdefs.h>
#include <wlib/sanity.h>
#include <wlib/util.h>

Display *
wl_app_dpy(WLAPP app)
{
	return app->dpy;
}

char *
wl_app_resfile(WLAPP app)
{
	return app->rc;
}

char *
wl_app_userresfile(WLAPP app)
{
	return app->user;
}

char *
wl_app_dotdir(WLAPP app)
{
	return app->dir;
}

char *
wl_app_name(WLAPP app)
{
	return app->cls;
}

static char *
cls2dir(char *cls)
{
	struct passwd *pwd = getpwuid(getuid());
	int   size;
	char *name, *p, *q;

	size = strlen(pwd->pw_dir) + 1 + 1 + strlen(cls) + 1;
	name = wl_calloc(size, 1);

	sprintf(name, "%s/.", pwd->pw_dir);
	p = name + strlen(pwd->pw_dir) + 2;
	for(q = cls; *q != '\0'; ++q, ++p)
		*p = tolower((int)*q);

	return name;
}

static char *
dir2res(char *dir)
{
	int   size = strlen(dir) + sizeof("/resources");
	char *s    = wl_calloc(size, 1);
	return strcat(strcpy(s, dir), "/resources");
}

static char *
dir2user(char *dir)
{
	int   size = strlen(dir) + sizeof("/user");
	char *s    = wl_calloc(size, 1);
	return strcat(strcpy(s, dir), "/user");
}

/*
 * All of the permission bits.
 * The sticky bit is non-portable so we'll just act like it doesn't
 * exist, it isn't important to us anyway.
 */
#define	MASK (				\
	  S_ISUID | S_ISGID 		\
	| S_IRUSR | S_IWUSR | S_IXUSR	\
	| S_IRGRP | S_IWGRP | S_IXGRP	\
	| S_IROTH | S_IWOTH | S_IXOTH	\
)

/*
 * All of the permission bits that we want set on the config directory.
 */
#define	MODE (S_IRUSR | S_IWUSR | S_IXUSR)

static int
makedir(char *dir)
{
	struct stat st;
	mode_t      mode = 0;

	memset((void *)&st, '\0', sizeof(st));
	if(stat(dir, &st) == 0) {
		if(!S_ISDIR(st.st_mode))
			return ENOTDIR;
		mode = st.st_mode & MASK;
	}
	else if(mkdir(dir, MODE) != 0) {
		return errno;
	}

	/*
	 * If mode is still zero then we made the directory so we
	 * need to stat again to get the permission bits.  There is
	 * a chance that the directory already had a zero mode but
	 * I'll consider that rare enough that it isn't worth the
	 * hassle, an initial zero mode just means that we call
	 * stat(2) one more time than we need to, who cares?
	 */
	if(mode == 0) {
		if(stat(dir, &st) != 0)
			return errno;
		mode = st.st_mode & MASK;
	}

	if((mode & MODE) != MODE
	&& chmod(dir, mode | MODE) != 0)
		return errno;

	return 0;
}

WLAPP
wl_app_create(Display *dpy, char *cls)
{
	WLAPP app = wl_calloc(sizeof(struct WLAPP_s), 1);

	app->dpy  = dpy;
	app->cls  = XtNewString(cls);
	app->dir  = cls2dir(app->cls);
	app->rc   = dir2res(app->dir);
	app->user = dir2user(app->dir);

	if(makedir(app->dir) != 0)
		return wl_app_destroy(app);

	return app;
}

static void
clear_tmp(char *tmp)
{
	DIR           *dir;
	struct dirent *d;
	char          *pwd;

	if(tmp == NULL
	|| (pwd = wl_getcwd()) == NULL
	|| chdir(tmp) != 0
	|| (dir = opendir(tmp)) == NULL)
		return;

	/*
	 * Some people are lazy with their 'struct dirent', they have
	 * something like this:
	 *
	 *	struct dirent { char d_name[1]; }
	 *
	 * rather than the more sensible
	 *
	 *	struct dirent { char *d_name; }
	 *
	 * so that they can allocate the dirent with just one malloc
	 * (false laziness! they could use just one malloc in either case!).
	 * This means that I can't just do some pointer fun to check for
	 * "." and "..", I have to use strcmp to avoid compiler warnings.
	 *
	 * I'm assuming that no one will put directories in the temp
	 * area, if such a thing does happen then I'll just switch to
	 * the lazy "system(rm -rf tmp)" approach.
	 */
	while((d = readdir(dir)) != NULL) {
		if(strcmp(d->d_name, ".")  == 0
		|| strcmp(d->d_name, "..") == 0)
			continue;
		remove(d->d_name);
	}

	closedir(dir);
	chdir(pwd);
	rmdir(tmp);
	wl_free(tmp);
	wl_free(pwd);
}

/*
 * We have to cache the temp dir in a static or we can't use
 * atexit() to clean it up.  We only need the atexit() thing
 * if wl_app_destroy doesn't get called but I'm sick of cleaning
 * up temp directories during debugging.
 */
static char *tmp_dir = NULL;

static void
exit_guy(void)
{
	if(tmp_dir != NULL)
		clear_tmp(tmp_dir);
	tmp_dir = NULL;
}

WLAPP
wl_app_destroy(WLAPP app)
{
	if(app == NULL)
		return NULL;
	wl_free(app->cls);
	wl_free(app->dir);
	wl_free(app->rc);
	wl_free(app->user);
	clear_tmp(app->tmp);
	tmp_dir = NULL;
	if(app->fp != NULL)
		fclose(app->fp);
	return NULL;
}

int
wl_app_res_open(WLAPP app, char **header, int n_header)
{
	return wl_res_open(&app->fp, app->rc, header, n_header);
}

void
wl_app_res_store(WLAPP app, char *p, XtPointer b, XtResource *r, int n_r)
{
	wl_res_store(app->fp, app->cls, p, b, r, n_r);
}

void
wl_app_res_close(WLAPP app)
{
	app->fp = wl_res_close(app->fp);
}

static char *
get_tmp_dir(WLAPP app, Widget w)
{
	struct stat st;
	char *dir;

	dir = wl_res_string(wl_top(w), XwlNtempDir, XwlCTempDir, NULL);

	/*
	 * Make sure dir is a useful directory for us.  If not, we'll
	 * just fallback to app->dir.
	 */
	memset((void *)&st, '\0', sizeof(st));
	if(dir == NULL
	|| stat(dir, &st) != 0
	|| !S_ISDIR(st.st_mode)
	|| access(dir, R_OK | W_OK | X_OK) != 0)
		dir = app->dir;

	return XtNewString(dir);
}

char *
wl_app_tmpdir(WLAPP app, Widget w)
{
	struct utsname uts;
	char *dir, *host;
	int   size;
	long  pid;

	if(app->tmp != NULL)
		return app->tmp;

	memset((void *)&uts, '\0', sizeof(uts));
	if(uname(&uts) != 0)
		host = XtNewString("unknown");
	else
		host = XtNewString(uts.nodename);
	dir  = get_tmp_dir(app, w);
	size = strlen(dir) + strlen(app->cls) + strlen(host) + 64;
	pid  = (long)getpid();

	app->tmp = wl_calloc(size, 1);
	sprintf(app->tmp, "%s/%s.tmp.%s.%ld", dir, app->cls, host, pid);
	if(makedir(app->tmp) != 0)
		app->tmp = wl_free(app->tmp);
	wl_free(dir);
	wl_free(host);

	/*
	 * Set up the atexit magic for some measure of crash clean up.
	 */
	tmp_dir = app->tmp;
	atexit(exit_guy);

	return app->tmp;
}
