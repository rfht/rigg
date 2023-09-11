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

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-config.h>

/* XXX: move this to pathnames.h? */
#define RIGG_MONO_CONFIG "/home/thfr/cvs/projects/IndieRunner/share/config/dllmap.config"

int mono(char *file, int argc, char** argv) {
	MonoDomain	*domain;
	MonoAssembly	*assembly;
	int r;

	mono_config_parse(RIGG_MONO_CONFIG);		/* void */

	if ((domain = mono_jit_init(file)) == NULL)
		err(1, "mono_jit_init");
	if ((assembly = mono_domain_assembly_open(domain, file)) == NULL)
		err(1, "mono_domain_assembly_open");

	/*
	 * mono_set_dirs has to happen after mono_domain_assembly_open,
         * otherwise, core libraries won't be found.
         */
	mono_set_dirs("/usr/local/share/FNA", NULL);	/* void */

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
	if (unveil("FNA.dll", "") == -1)
		err(1, "unveil");
	if (unveil("FNA.dll.config", "") == -1)
		err(1, "unveil");

	//r = mono_jit_exec(domain, assembly, argc, argv);	// XXX
	r = mono_jit_exec(domain, assembly, 1, &file);

	mono_jit_cleanup(domain);	/* void */

	return r;
}
