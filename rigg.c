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

#include <sys/cdefs.h>

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "rigg.h"

#define STR_MAX		1024

__dead static int usage(void) {
	fprintf(stderr,
	        "usage: rigg\n"
	        "\n");
	exit(1);
}

int main(int argc, char** argv) {
	int vflag, ch;
	char eflag[STR_MAX] = "\0", gflag[STR_MAX] = "\0";

	vflag = 0;
	while ((ch = getopt(argc, argv, "e:g:hv")) != -1) {
		switch (ch) {
		case 'e':
			if (strlcpy(eflag, optarg, sizeof(eflag)) >= sizeof(eflag))
				err(1, "strlcpy");
			break;
		case 'g':
			if (strlcpy(gflag, optarg, sizeof(gflag)) >= sizeof(gflag))
				err(1, "strlcpy");
			break;
		case 'v':
			break;
		case 'h':
		default:
			(void)usage();
		}
	}
	argc -= optind;
	argv += optind;

	if (*eflag != '\0') {
		if (strncmp(eflag, "mono", STR_MAX) == 0) {
			if (argc > 0) {
				return mono(argv[0], argc, argv);
			}
			errx(1, "too few arguments");
		}
		else if (strncmp(eflag, "hl", STR_MAX) == 0) {
			if (argc > 0) {
				return hl(argv[0], 0, NULL);
			}
			errx(1, "too few arguments");
		}
	}

	fprintf(stderr, "no engine specified\n");

	return 1;
}
