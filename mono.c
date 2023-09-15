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
#include <limits.h>
#include <stdio.h>
#include <unistd.h>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-config.h>

/* XXX: move this to pathnames.h? */
#define RIGG_MONO_CONFIG	"/home/thfr/cvs/projects/IndieRunner/share/config/dllmap.config"
#define FNA_DIR			"/usr/local/share/FNA"

const char *unveil_hide[] = {
	"FNA.dll",
	"FNA.dll.config"
};

int mono(char *file, int argc, char** argv) {
	MonoDomain	*domain;
	MonoAssembly	*assembly;
	char	*home_dir;
	char	config_dir[PATH_MAX];
	char	localshare_dir[PATH_MAX];
	char	sndio_dir[PATH_MAX];
	char	xauthority[PATH_MAX];
	int	i, r;

	if ((home_dir = getenv("HOME")) == NULL)
		err(1, "getenv(\"HOME\")");
	if (snprintf(config_dir, sizeof(config_dir), "%s/.config", home_dir) < 0)
		err(1, "snprintf");
	if (snprintf(localshare_dir, sizeof(localshare_dir), "%s/.local/share", home_dir) < 0)
		err(1, "snprintf");
	if (snprintf(sndio_dir, sizeof(sndio_dir), "%s/.sndio", home_dir) < 0)
		err(1, "snprintf");
	if (snprintf(xauthority, sizeof(xauthority), "%s/.Xauthority", home_dir) < 0)
		err(1, "snprintf");

	mono_config_parse(RIGG_MONO_CONFIG);		/* void */

	if ((domain = mono_jit_init(file)) == NULL)
		err(1, "mono_jit_init");
	if ((assembly = mono_domain_assembly_open(domain, file)) == NULL)
		err(1, "mono_domain_assembly_open");

	/*
	 * mono_set_dirs has to happen after mono_domain_assembly_open,
         * otherwise, core libraries won't be found.
         */
	mono_set_dirs(FNA_DIR, NULL);	/* void */

	/* general unveil */
	if (unveil(FNA_DIR, "r") == -1)
		err(1, "unveil");
	if (unveil("/usr/lib", "r") == -1)
		err(1, "unveil");
	if (unveil("/usr/local/lib", "r") == -1)
		err(1, "unveil");
	if (unveil("/usr/X11R6", "r") == -1)
		err(1, "unveil");
	if (unveil("/usr/share", "r") == -1)
		err(1, "unveil");

	if (unveil("/etc", "r") == -1)		/* XXX: only /etc/mono ? */
		err(1, "unveil");
	if (unveil("/dev", "rw") == -1)
		err(1, "unveil");
	if (unveil("/tmp", "rwc") == -1)
		err(1, "unveil");
	if (unveil(".", "rwcx") == -1)
		err(1, "unveil");
	if (unveil(config_dir, "rwcx") == -1)
		err(1, "unveil");
	if (unveil(localshare_dir, "rwcx") == -1)
		err(1, "unveil");
	if (unveil(sndio_dir, "rwcx") == -1)
		err(1, "unveil");
	if (unveil(RIGG_MONO_CONFIG, "r") == -1)
		err(1, "unveil");
	if (unveil(xauthority, "rw") == -1)
		err(1, "unveil");

	/* hide platform-incompatible files from mono_jit_exec with unveil */
	for (i = 0; i < sizeof(unveil_hide) / sizeof(unveil_hide[0]); i++) {
		if (access(unveil_hide[i], F_OK) == 0) {
			if (unveil(unveil_hide[i], "") == -1)
				err(1, "unveil");
		}
	}

	if (unveil(NULL, NULL) == -1)
		err(1, "unveil");

	//r = mono_jit_exec(domain, assembly, argc, argv);	/* XXX */
	r = mono_jit_exec(domain, assembly, 1, &file);

	mono_jit_cleanup(domain);	/* void */

	return r;
}
