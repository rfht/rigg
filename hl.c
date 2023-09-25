/*
 * Copyright (C)2015-2016	Haxe Foundation
 * Copyright (C)2023		Thomas Frohwein
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <sys/stat.h>
#include <glob.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include <hl.h>
#include <hlmodule.h>

#include "rigg.h"
#include "rigg_unveil.h"

typedef char pchar;
#define pprintf printf
#define pfopen fopen
#define pcompare strcmp
#define ptoi atoi
#define PSTR(x) x

static const unveil_pair unveils[] = {
	{ "/usr/lib",		"r"	},
	{ "/usr/local/lib",	"r"	},
	{ "/usr/X11R6",		"r"	},
	{ "/usr/share",		"r"	},
	{ "/dev",		"rw"	},
	{ "/tmp",		"rwc"	},	/* XXX: remove? */
	{ ".",			"rwc"	}
};

const char *unveil_globs[] = {
	"*.hdll",
	"*.so",
	"*.so.*"
};

typedef struct {
	pchar *file;
	hl_code *code;
	hl_module *m;
	vdynamic *ret;
	int file_time;
} main_context;

static int pfiletime( pchar *file )	{
	struct stat st;
	stat(file,&st);
	return (int)st.st_mtime;
}

static hl_code *load_code( const pchar *file, char **error_msg, bool print_errors ) {
	hl_code *code;
	FILE *f = pfopen(file,"rb");
	int pos, size;
	char *fdata;
	if( f == NULL ) {
		if( print_errors ) pprintf("File not found '%s'\n",file);
		return NULL;
	}
	fseek(f, 0, SEEK_END);
	size = (int)ftell(f);
	fseek(f, 0, SEEK_SET);
	fdata = malloc(size);
	pos = 0;
	while( pos < size ) {
		int r = (int)fread(fdata + pos, 1, size-pos, f);
		if( r <= 0 ) {
			if( print_errors ) pprintf("Failed to read '%s'\n",file);
			return NULL;
		}
		pos += r;
	}
	fclose(f);
	code = hl_code_read((unsigned char*)fdata, size, error_msg);
	free(fdata);
	return code;
}

static bool check_reload( main_context *m ) {
	int time = pfiletime(m->file);
	bool changed;
	if( time == m->file_time )
		return false;
	char *error_msg = NULL;
	hl_code *code = load_code(m->file, &error_msg, false);
	if( code == NULL )
		return false;
	changed = hl_module_patch(m->m, code);
	m->file_time = time;
	hl_code_free(code);
	return changed;
}

static void handle_signal( int signum ) {
	signal(signum, SIG_DFL);
	printf("SIGNAL %d\n",signum);
	hl_dump_stack();
	fflush(stdout);
	raise(signum);
}
static void setup_handler() {
	struct sigaction act;
	act.sa_sigaction = NULL;
	act.sa_handler = handle_signal;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	signal(SIGPIPE, SIG_IGN);
	sigaction(SIGSEGV,&act,NULL);
	sigaction(SIGTERM,&act,NULL);
}

int hl(int argc, pchar *argv[]) {
	static vclosure cl;
	char *error_msg = NULL;
	int debug_port = -1;
	bool debug_wait = false;
	bool hot_reload = false;
	int profile_count = -1;
	main_context ctx;
	bool isExc = false;
	int first_boot_arg = -1;
	pchar *file = *argv++;
	argc--;

	vprintf("launching global init\n");
	hl_global_init();
	hl_sys_init((void**)argv,argc,file);
	vprintf("registring main thread\n");
	hl_register_thread(&ctx);
	ctx.file = file;
	vprintf("loading code from %s\n", file);
	ctx.code = load_code(file, &error_msg, true);
	if( ctx.code == NULL ) {
		if( error_msg ) printf("%s\n", error_msg);
		return 1;
	}
	vprintf("initializing module\n");
	ctx.m = hl_module_alloc(ctx.code);
	if( ctx.m == NULL )
		return 2;
	if( !hl_module_init(ctx.m,hot_reload) )
		return 3;
	if( hot_reload ) {
		ctx.file_time = pfiletime(ctx.file);
		hl_setup_reload_check(check_reload,&ctx);
	}
	hl_code_free(ctx.code);
	if( debug_port > 0 && !hl_module_debug(ctx.m,debug_port,debug_wait) ) {
		fprintf(stderr,"Could not start debugger on port %d",debug_port);
		return 4;
	}
	cl.t = ctx.code->functions[ctx.m->functions_indexes[ctx.m->code->entrypoint]].type;
	cl.fun = ctx.m->functions_ptrs[ctx.m->code->entrypoint];
	cl.hasValue = 0;
	vprintf("setting up signal handler\n");
	setup_handler();
	hl_profile_setup(profile_count);

	/* unveil */
	glob_t		g;
	char		*home_dir;
	char		*match;
	char		xauthority[PATH_MAX];
	unveil_pair	uvp;
	int		i;

	vprintf("\n");
	for (i = 0; i < sizeof(unveils) / sizeof(unveils[0]); i++) {
		uvp = unveils[i];
		unveil_err(uvp.path, uvp.permissions);
	}
	if ((home_dir = getenv("HOME")) == NULL)
		err(1, "getenv(\"HOME\")");
	if (snprintf(xauthority, sizeof(xauthority), "%s/.Xauthority", home_dir) < 0) {
		err(1, "snprintf");
	}
	else {
		unveil_err(xauthority, "rw");
	}

	g.gl_offs = 0;
	for (i = 0; i < sizeof(unveil_globs) / sizeof(unveil_globs[0]); i++) {
		if (glob(unveil_globs[i], i > 0 ? GLOB_APPEND : 0, NULL, &g) != 0)
			err(1, "glob");
	}
	while ((match = *g.gl_pathv++) != NULL) {
		unveil_err(match, "");
	}

	unveil_err(NULL, NULL);
	vprintf("\n");

	vprintf("entering main program\n\n");
	ctx.ret = hl_dyn_call_safe(&cl,NULL,0,&isExc);
	vprintf("cleaning up...\n");
	hl_profile_end();
	if( isExc ) {
		varray *a = hl_exception_stack();
		int i;
		uprintf(USTR("Uncaught exception: %s\n"), hl_to_string(ctx.ret));
		for(i=0;i<a->size;i++)
			uprintf(USTR("Called from %s\n"), hl_aptr(a,uchar*)[i]);
		hl_debug_break();
		hl_global_free();
		return 1;
	}
	vprintf("main program concluded without exception; cleaning up...\n");
	hl_module_free(ctx.m);
	hl_free(&ctx.code->alloc);
	// do not call hl_unregister_thread() or hl_global_free will display error 
	// on global_lock if there are threads that are still running (such as debugger)
	hl_global_free();
	return 0;
}
