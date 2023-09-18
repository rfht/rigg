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
#include <glob.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <hlc.h>
#include <hlmodule.h>

typedef struct {
	char *file;
	hl_code *code;
	hl_module *m;
	vdynamic *ret;
	int file_time;
} main_context;

static hl_code *load_code(const char *file, char **error_msg, bool print_errors) {
	hl_code *code;
	FILE *f = fopen(file, "rb");
	long size;
	int pos;
	char *fdata;

	if ((f = fopen(file, "rb")) == NULL)
		err(1, "fopen");
	fseek(f, 0, SEEK_END);	/* XXX: check return */
	size = ftell(f);
	fseek(f, 0, SEEK_SET);	/* XXX: check return */
	fdata = malloc(size);
	pos = 0;
	while (pos < size) {
		size_t r = fread(fdata + pos, 1, size-pos, f);
		if(r == 0) {
			if(print_errors)
				printf("Failed to read '%s'\n", file);
			return NULL;
		}
		pos += r;
	}
	if (fclose(f) != 0)
		err(1, "fclose");
	code = hl_code_read((unsigned char*)fdata, size, error_msg);
	free(fdata);
	return code;
}

int hl(char *file, int argc, char** argv) {
	static vclosure cl;
	char *error_msg = NULL;
	main_context ctx;
	bool isExc = false;
	int first_boot_arg = -1;
	char *home_dir;
	char xauthority[PATH_MAX];

	if ((home_dir = getenv("HOME")) == NULL)
		err(1, "getenv(\"HOME\")");
	if (snprintf(xauthority, sizeof(xauthority), "%s/.Xauthority", home_dir) < 0)
		err(1, "snprintf");

	hl_global_init();			/* void */
	hl_sys_init((void**)argv, argc, file);	/* void */
	hl_register_thread(&ctx);		/* void */

	ctx.file = file;
	ctx.code = load_code(file, &error_msg, true);
	if (ctx.code == NULL)
		errx(1, "%s", error_msg);
	ctx.m = hl_module_alloc(ctx.code);
	if (ctx.m == NULL)
		return 2;
	if(!hl_module_init(ctx.m, false))
		return 3;
	hl_code_free(ctx.code);

	cl.t = ctx.code->functions[ctx.m->functions_indexes[ctx.m->code->entrypoint]].type;
	cl.fun = ctx.m->functions_ptrs[ctx.m->code->entrypoint];
	cl.hasValue = 0;

	/* general unveil */
	if (unveil("/usr/lib",		"r")	== -1) err(1, "unveil");
	if (unveil("/usr/local/lib",	"r")	== -1) err(1, "unveil");
	if (unveil("/usr/X11R6",	"r")	== -1) err(1, "unveil");
	if (unveil("/usr/share",	"r")	== -1) err(1, "unveil");

	if (unveil("/dev",		"rw")	== -1) err(1, "unveil");
	if (unveil("/tmp",		"rwc")	== -1) err(1, "unveil");	/* XXX: needed? */

	if (unveil(xauthority,		"rw")	== -1) err(1, "unveil");
	if (unveil(".",			"rwc")	== -1) err(1, "unveil");

	/* hide the incompatible bundled files */

	glob_t g;
	char *match;

	g.gl_offs = 0;
	if (glob("*.hdll", 0, NULL, &g) != 0)
		err(1, "glob");
	if (glob("*.so", GLOB_APPEND, NULL, &g) != 0)
		err(1, "glob");
	if (glob("*.so.*", GLOB_APPEND, NULL, &g) != 0)
		err(1, "glob");

	while ((match = *g.gl_pathv++) != NULL) {
		if (unveil(match, "") == -1) err(1, "unveil");
	}

	if (unveil(NULL, NULL) == -1) err(1, "unveil");

	/* run the main hashlink application */
	ctx.ret = hl_dyn_call_safe(&cl, NULL, 0, &isExc);
	if( isExc ) {
		varray *a = hl_exception_stack();
		int i;
		uprintf(USTR("Uncaught exception: %s\n"), hl_to_string(ctx.ret));
		for (i = 0; i<a->size; i++)
			uprintf(USTR("Called from %s\n"), hl_aptr(a, uchar*)[i]);
	}
	hl_module_free(ctx.m);
	hl_free(&ctx.code->alloc);
	hl_global_free();	/* void */
	return (int)isExc;
}
