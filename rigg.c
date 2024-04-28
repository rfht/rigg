/*
 * Copyright (c) 2023-2024 Thomas Frohwein
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

unveilmode umode = STRICT;

int verbose = 0;

__dead static void usage(void) {
	fprintf(stderr,
	        "usage:\trigg [-v] [-u strict|permissive|none] mono|hl file [arguments]\n\trigg [-hl]\n"
	        );
	exit(1);
}

int main(int argc, char** argv) {
	int ch;

	while ((ch = getopt(argc, argv, "hlu:v")) != -1) {
		switch (ch) {
		case 'l':
			for (size_t i = 0; i < sizeof(runtimes) / sizeof(runtimes[0]); i++) {
				printf("%s\n", runtimes[i]);
			}
			exit(0);
		case 'u':
			if (strncmp(optarg, "none", 4) == 0)
				umode = NONE;
			else if (strncmp(optarg, "permissive", 10) == 0)
				umode = PERMISSIVE;
			else if (strncmp(optarg, "strict", 6) == 0)
				umode = STRICT;
			else
				(void)usage();
			break;
		case 'v':
			verbose = 1;
			break;
		case 'h':
		default:
			usage();
		}
	}
	argc -= optind;
	argv += optind;

	if (argc < 1)
		usage();

	if (strncmp(argv[0], "mono", MAX_INPUT) == 0) {
		return mono(argc - 1, argv + 1);
	}
	else if (strncmp(argv[0], "hl", MAX_INPUT) == 0) {
		return hl(argc - 1, argv + 1);
	}

	fprintf(stderr, "not a valid engine argument: %s\n", argv[0]);
	(void)usage();
}
