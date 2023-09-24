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

#ifndef _RIGG_UNVEIL_H
#define _RIGG_UNVEIL_H

#include <err.h>
#include <unistd.h>

#include "rigg.h"

#define UNVEIL_VPRINT_FMT	"\t%-32.32s \"%s\"\n"

typedef struct {
	const char *path;
	const char *permissions;
} unveil_pair;

typedef struct {
	const char *file;	/* program file for quirks detection */
	const char *env_var;	/* e.g. "HOME", set to NULL to ignore */
	const char *path;	/* relative to env_var if env_var is not NULL */
	const char *permissions;
} unveil_quirk;

static void unveil_err(const char *path, const char *permissions) {
	vprintf(UNVEIL_VPRINT_FMT, path, permissions);
	if (unveil(path, permissions) == -1)
		err(1, "unveil");
}

#endif /* _RIGG_UNVEIL_H */
