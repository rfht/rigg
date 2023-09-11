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
#include <unistd.h>

#include <hlc.h>

int hl(char *file, int argc, char** argv) {
	vdynamic *ret;
	bool isExc =		false;
	hl_type_fun tf =	{ 0 };
	hl_type clt =		{ 0 };
	vclosure cl =		{ 0 };

	hl_global_init();		/* void */
	hl_register_thread(&ret);	/* void */
	//hl_setup_exception(hlc_resolve_symbol, hlc_capture_stack);	/* void */
	//hl_setup_callbacks(hlc_static_call, hlc_get_wrapper);		/* void */
	hl_sys_init((void**)(argv + 1), argc - 1, file);		/* void */
	tf.ret = &hlt_void;
	clt.kind = HFUN;
	clt.fun = &tf;
	cl.t = &clt;
	//cl.fun = hl_entry_point;
	ret = hl_dyn_call_safe(&cl, NULL, 0, &isExc);
	if( isExc ) {
		varray *a = hl_exception_stack();
		int i;
		uprintf(USTR("Uncaught exception: %s\n"), hl_to_string(ret));
		for (i = 0; i<a->size; i++)
			uprintf(USTR("Called from %s\n"), hl_aptr(a, uchar*)[i]);
	}
	hl_global_free();	/* void */
	return (int)isExc;

#if 0
	/* hide platform-incompatible files from mono_jit_exec with unveil */
	if (unveil("/usr", "r") == -1)
		err(1, "unveil");
	if (unveil("/etc", "r") == -1)
		err(1, "unveil");
	if (unveil("/dev", "rw") == -1)
		err(1, "unveil");
	if (unveil("/tmp", "rwc") == -1)
		err(1, "unveil");
	if (unveil("/home", "rwc") == -1)
		err(1, "unveil");

	if (pledge(NULL, NULL) == -1)	/* XXX: add actual pledges */
		err(1, "pledge");

	return r;
#endif
}
