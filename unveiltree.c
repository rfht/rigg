/*
 * Copyright (c) 2023 Thomas Frohwein
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

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "unveiltree.h"

size_t	uvt_count = 0;
size_t	uvt_total;
char	uvt_cwd[PATH_MAX];

/* allocate the array and also store uvt_cwd */
/* call this _before_ the first unveils are done so that getcwd works */
uvt_pair* unveiltree_alloc(size_t nuvp) {
	uvt_total = nuvp;
	if (getcwd(uvt_cwd, sizeof(uvt_cwd)) == NULL)
		err(1, "getcwd");
	return (uvt_pair *)reallocarray(NULL, uvt_total, sizeof(uvt_pair));
}

int unveiltree_add(uvt_pair *unveils, const char *path,
                          const char *permissions) {
	unveils[uvt_count++] = (uvt_pair) { (char *)path, permissions };
	return 0;
}

void _sort(uvt_pair *unveils) {
	/* note: doesn't handle duplicates */
	int x, y;

	for (x = 0; x < uvt_count; x++) {
		for (y = x + 1; y < uvt_count; y++) {
			if (strncmp(unveils[x].path, unveils[y].path, PATH_MAX) > 0) {
				char *tmp_path = (char*)unveils[x].path;
				char *tmp_perm = (char*)unveils[x].permissions;
				unveils[x].path = unveils[y].path;
				unveils[y].path = tmp_path;
				unveils[x].permissions = unveils[y].permissions;
				unveils[y].permissions = tmp_perm;
			}
		}
	}
}

void _abs(uvt_pair *unveils) {
	uvt_pair	uvtp;
	int		i;

	char		(*abspaths)[PATH_MAX] = reallocarray(NULL,
	                                                     UNVEIL_MAX,
	                                                     sizeof(*abspaths));

	for (i = 0; i < uvt_count; i++) {
		snprintf(abspaths[i], sizeof(abspaths[i]), "abspaths: %d", i);
	}
	for (i = 0; i < uvt_count; i++) {
		uvtp = unveils[i];

		if (uvtp.path[0] == '/') {
			if (strlcpy(abspaths[i], uvtp.path,
			            sizeof(abspaths[i])) > sizeof(abspaths[i]))
				warn("WARNING: path truncated (strlcpy): %s",
					abspaths[i]);
		}
		else {
			if (snprintf(abspaths[i], sizeof(abspaths[i]),
				     "%s/%s", uvt_cwd, uvtp.path)
				     > sizeof(abspaths[i]))
				warn("WARNING: path truncated (snprintf): %s", abspaths[i]);
		}
		unveils[i].path = abspaths[i];
	}
}

/* pp: path pointer */
/* stores pointers in **pp */
/* returns number of pp */
size_t _tokenize_path(char **tok, char *path) {
	int	i = 0;
	char	**pp = tok;

	/* all paths are absolute, insert root "/" at the beginning for all */
	*pp++ = "/";
	i++;

	while((*pp = strsep(&path, "/")) != NULL) {
		if (**pp != '\0') {
			if (strncmp(*pp, "..", PATH_MAX) == 0) {
				/* erase the current and the previous directory token */
				if(strlcpy(*pp, "", PATH_MAX) > PATH_MAX)
					errx(1, "strlcpy");	/* shouldn't be reachable*/
				if(strlcpy(*--pp, "", PATH_MAX) > PATH_MAX)
					errx(1, "strlcpy");	/* shouldn't be reachable*/
				pp = pp + 2;
				i--;
			}
			else if (strncmp(*pp, ".", PATH_MAX) != 0) {
				pp++;
				i++;
			}
			/* ignore if "." */
		}
	}

	return i;
}

int unveiltree_print(uvt_pair *unveils) {
	uvt_pair	uvtp;
	char		**tok = reallocarray(NULL, UNVEIL_MAX, PATH_MAX);
	/*
	 * **seen holds the last seen entry for every level of the file system
	 * dimensions: level of file system, directory name
	 * use PATH_MAX /2 for now for levels of file system as heuristic
         */
	char		(*seen)[PATH_MAX] = calloc(PATH_MAX / 2, sizeof(*seen));
	size_t		ntokens;
	int		i, j, k;

	_abs(unveils);
	_sort(unveils);

	for (i = 0; i < uvt_count; i++) {
		uvtp = unveils[i];
		//printf("%s\n", uvtp.path);
		ntokens = _tokenize_path(tok, uvtp.path);
		for (j = 0; j < ntokens; j++) {
			if (strcmp(seen[j], tok[j]) != 0) {
				if (j == 0)
					printf("%s\n", tok[j]);
				else {
					for (k = 0; k < j - 1; k++)
						if (k == 0)
							printf("|   ");
						else
							printf("    ");
					printf("`-- %s\n", tok[j]);
				}
				if (strlcpy(seen[j], tok[j], sizeof(seen[j]))
				    > sizeof(seen[j]))
					err(1, "strlcpy");
			}
		}
	}
	return 0;
}
