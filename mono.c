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

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-config.h>

#define RIGG_MONO_CONFIG "/home/thfr/cvs/projects/IndieRunner/share/config/dllmap.config"

int mono(char *file, int argc, char** argv) {
	MonoDomain	*domain;
	MonoAssembly	*assembly;
	int r;

	mono_config_parse(RIGG_MONO_CONFIG);	/* void function */

	if ((domain = mono_jit_init(file)) == NULL)
		err(1, "mono_jit_init");
	if ((assembly = mono_domain_assembly_open(domain, file)) == NULL)
		err(1, "mono_domain_assembly_open");

	//r = mono_jit_exec(domain, assembly, argc, argv);
	r = mono_jit_exec(domain, assembly, 1, &file);
	mono_jit_cleanup(domain);	/* void function */
	return r;
}