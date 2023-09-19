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
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "rigg.h"

__dead static int usage(void) {
	fprintf(stderr,
	        "usage: rigg [-v] mono|hl file [arguments]\n"
	        );
	exit(1);
}

int main(int argc, char** argv) {
	int vflag, ch;

	if (argc < 3)
		(void)usage();

	vflag = 0;
	while ((ch = getopt(argc, argv, "hv")) != -1) {
		switch (ch) {
		case 'v':
			vflag = 1;
			break;
		case 'h':
		default:
			(void)usage();
		}
	}
	argc -= optind;
	argv += optind;

	if (strncmp(argv[0], "mono", MAX_INPUT) == 0) {
		return mono(argc - 1, argv + 1);
	}
	else if (strncmp(argv[0], "hl", MAX_INPUT) == 0) {
		return hl(argc - 1, argv + 1);
	}

	fprintf(stderr, "not a valid engine argument: %s\n", argv[0]);
	(void)usage();
}
