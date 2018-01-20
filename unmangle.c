/*
 * compressed.c
 *	Utility functions to deal with compressed/gzipped files.
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
#include <wlib/rcs.h>
MU_ID("$Mu: mgv/unmangle.c 1.44 1998/11/11 05:59:53 $")

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include <wlib/stdlib.h>
#include <wlib/wlib.h>
#include <wlib/sanity.h>
#include <mine/mgv.h>
#include <mine/error.h>
#include <mine/unmangle.h>
#include <mine/strings.h>
#include <mine/app.h>

/*
 * Magic numbers for various compressed files (from gzip.h in gzip 1.2.4
 * and bzip2.c from bzip2-0.1pl2).
 */
#define	FT_NORM		0
#define	FT_GZIP		(1 << 0)
#define	FT_BZIP		(1 << 1)
#define	FT_COMPRESS	(1 << 2)
#define	FT_PDF		(1 << 3)
#define	IS_COMPRESSED	(FT_GZIP | FT_BZIP | FT_COMPRESS)
static struct {
	char	*ext;
	int	n_ext;
	char	*magic;	/* this is really two useful bytes */
	int	type;
} magic[] = {
#define	BLECH(e,m,t) {e, sizeof(e) - 1, m, t}
	BLECH(".z",   "\037\036", FT_GZIP),	/* pack		*/
	BLECH(".gz",  "\037\213", FT_GZIP),	/* gzip		*/
	BLECH(".z",   "\037\236", FT_GZIP),	/* old gzip 	*/
	BLECH(".Z",   "\037\235", FT_COMPRESS),	/* compress	*/
	BLECH(".bz2", "BZ",       FT_BZIP),	/* bzip2	*/
#undef	BLECH
};
#define	N_MAGIC	(int)(sizeof(magic)/sizeof(magic[0]))

static int
filetype(char *file)
{
	int  fd, i, n, n_ext;
	char buf[2], *ext;
	int  pdfbit = 0;

	if(strstr(file, ".pdf") != NULL)
		pdfbit = FT_PDF;

	if((ext = strrchr(file, '.'))   == NULL
	|| (fd  = open(file, O_RDONLY)) == -1)
		return FT_NORM | pdfbit;
	n = read(fd, (void *)&buf[0], sizeof(buf));
	close(fd);
	if(n != sizeof(buf))
		return FT_NORM | pdfbit;

	n_ext = strlen(ext);
	for(i = 0; i < N_MAGIC; ++i) {
		if(n_ext != magic[i].n_ext
		|| strcmp(magic[i].ext, ext) != 0)
			continue;
		if(buf[0] == magic[i].magic[0]
		&& buf[1] == magic[i].magic[1])
			return magic[i].type | pdfbit;
	}
	return FT_NORM | pdfbit;
}

static void
showmsg(MGV *m, char *file, char *tmp)
{
	FILE       *fp;
	char       *buf;
	struct stat st;

	if(tmp == NULL || *tmp == '\0')
		return;

	memset((void *)&st, '\0', sizeof(st));
	if(stat(tmp, &st) < 0)
		return;

	fp = fopen(tmp, "r");
	remove(tmp);
	if(fp == NULL
	|| (buf = wl_calloc(st.st_size + 1, 1)) == NULL)
		return;

	fread((void *)buf, 1, st.st_size, fp);
	fclose(fp);

	if(*buf != '\0')
		mgv_warn(m->main, "%s %s:\n%s",
				wl_s(m->main, MgvScantDecompress), file, buf);
	wl_free(buf);
}

/*
 * Uncompress a file and return the new file name, if the file doesn't
 * appear to be compressed or the uncompression fails, the original
 * file name will be returned.  In all cases, the return value is
 * allocated with XtNewString().
 *
 * We use "system()" in here to avoid the hassle of converting the
 * decompression commands from strings to argv's but this means that
 * some kludging is needed to grab the error output.  This would be
 * "cleaner" (in some sense) if pipe/dup2/fork/exec/XtAppAddInput
 * were used instead but sometimes laziness is good.
 *
 * This is kinda' sick, busting this into two separate functions should
 * heal some of the sickness.
 */
char *
mgv_unmangle(MGV *m, char *file)
{
#	define  CMD_FMT "%s <%s >%s 2>%s"
#	define	PDF_FMT	"%s %s %s 2>%s"
	char   *cmd, *out, *tmp, *s, *tempdir;
	int     t, cmd_size;
	MGVR   *r = mgv_app_res(m->main);

	tempdir = mgv_app_tmpdir();

	if((t = filetype(file)) == FT_NORM)
		return XtNewString(file);

	if((s = strrchr(file, '/')) == NULL)
		s = file;
	else
		++s;

	out = wl_calloc(strlen(tempdir) + 1 + strlen(s) + 1 + 128 + 1, 1);
	sprintf(out, "%s/%s.%ld.%d", tempdir, s,
				(long)XtWindow(m->main), m->serial);

	tmp = wl_calloc(strlen(tempdir) + 1 + 128 + 1 + 1, 1);
	sprintf(tmp, "%s/%ld.%d", tempdir, (long)getpid(), m->serial);

	cmd_size = strlen(file) + strlen(out) + strlen(tmp) + sizeof(CMD_FMT);

	cmd = NULL;
	switch(t & IS_COMPRESSED) {
	case FT_COMPRESS:
		cmd = wl_calloc(cmd_size + strlen(r->decompress), 1);
		sprintf(cmd, CMD_FMT, r->decompress, file, out, tmp);
		break;
	case FT_GZIP:
		cmd = wl_calloc(cmd_size + strlen(r->gunzip), 1);
		sprintf(cmd, CMD_FMT, r->gunzip, file, out, tmp);
		break;
	case FT_BZIP:
		cmd = wl_calloc(cmd_size + strlen(r->bunzip), 1);
		sprintf(cmd, CMD_FMT, r->bunzip, file, out, tmp);
		break;
	case FT_NORM:
		/* this should mean PDF at this point */
		wl_assert((t & FT_PDF) != 0);
		cmd = NULL;
		break;
	}

	errno = 0;
	if(cmd != NULL) {
		if(system(cmd) != 0 && errno != 0) {
			wl_free(out);
			out = XtNewString(file);
		}
	}
	else {
		out = XtNewString(file);
	}
	showmsg(m, file, &tmp[0]);
	wl_free(cmd);
	cmd = NULL;

	/*
	 * Now we can look at PDF issues.
	 * We don't have to worry about removing intermediate files
	 * (i.e. uncompressed pdf files), the cleanup routines blow
	 * away the entire temp directory.
	 */
	if((t & FT_PDF) != 0
	&& r->pdfconv   != NULL) {
		file = out;
		if(t & IS_COMPRESSED) {
			out = wl_calloc(strlen(out) + 1 + 1, 1);
			sprintf(out, "%s.", file);
		}
		else {
			out = wl_calloc(strlen(tempdir) + 1 + strlen(s)
					+ 1 + 128 + 1, 1);
			sprintf(out, "%s/%s.%ld.%d", tempdir, s,
					(long)XtWindow(m->main), m->serial);
		}

		cmd_size = strlen(file) + strlen(out) + strlen(tmp)
							+ sizeof(PDF_FMT);
		cmd = wl_calloc(cmd_size + strlen(r->pdfconv), 1);
		sprintf(cmd, PDF_FMT, r->pdfconv, file, out, tmp);

		errno = 0;
		if(system(cmd) != 0 && errno != 0) {
			wl_free(out);
			out = XtNewString(file);
		}
		showmsg(m, file, &tmp[0]);
		cmd = wl_free(cmd);
		wl_free(file);
	}

	wl_free(tmp);

	return out;
#	undef PDF_FMT
#	undef CMD_FMT
}

/*
 * Create a new tempfile and copy src to it.  The new temp file will
 * be openned (for reading) and returned.  This implementation is pretty
 * brain-dead but it should be okay for our purposes.
 */
FILE *
mgv_tcopy(MGV *m, char **name, FILE *src)
{
#	define STDIN_FMT "%s/stdin.%ld.%d"
	FILE  *fp;
	char  *out, *tempdir;
	char   buf[5120];
	size_t sz;

	tempdir = mgv_app_tmpdir();

	out = wl_calloc(strlen(tempdir) + sizeof(STDIN_FMT) + 64, 1);
	sprintf(out, "%s/stdin.%ld.%d", tempdir, (long)XtWindow(m->main),
								m->serial);

	if((fp = fopen(out, "w")) == NULL)
		return wl_free(out);
	while((sz = fread((void *)&buf[0], 1, sizeof(buf), src)) > 0)
		fwrite((void *)&buf[0], 1, sz, fp);
	fclose(fp);

	*name = out;
	return fopen(*name, "r");
#	undef STDIN_FMT
}
