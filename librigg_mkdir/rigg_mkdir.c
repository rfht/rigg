/*
 * Copyright (c) 2024 Thomas Frohwein
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/stat.h>
#include <dlfcn.h>
#include <err.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int mkdir(const char *path, mode_t mode) {
	int	(*orig_mkdir)(const char *, mode_t);
	int	ret;

	orig_mkdir = dlsym(RTLD_NEXT, "mkdir");
	if (orig_mkdir == NULL)
		_exit(1);

	ret = orig_mkdir(path, mode);
	if (ret == -1 && errno == ENOENT) {
		char	*homedir= getenv("HOME");
		char	localdir[PATH_MAX];
		char	mypath[PATH_MAX];
		size_t	len	= strnlen(path, PATH_MAX);

		if (strlcpy(mypath, path, sizeof(mypath)) >= sizeof(mypath))
			err(1, "strlcpy: truncated");
		if (homedir == NULL)
			err(1, "unable to obtain HOME from environment");

		if (snprintf(localdir, sizeof(localdir), "%s/.config", homedir) < 0)
			err(1, "snprintf");

		/* remove trailing '/' */
		if (mypath[len - 1] == '/') {
			mypath[len -1] = '\0';
		}

		/* return with EEXIST for certain paths known to be hidden by unveil(2) */
		if ((strncmp(mypath, "/home",	sizeof("/home")) == 0) ||
		    (strncmp(mypath, homedir,	len) == 0) ||
		    (strncmp(mypath, localdir,	len) == 0)) {
			    //errno = EEXIST;
			    //return -1;
			    return 0;
		}
	}

	return ret;
}
