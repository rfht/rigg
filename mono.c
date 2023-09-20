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
#include <stdlib.h>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-config.h>

#include "rigg.h"
#include "rigg_mono.h"
#include "rigg_unveil.h"

#define UNVEIL_VPRINT_FMT	"\t%-32.32s \"%s\"\n"

/* XXX: Vulkan will need: /home (SDL_GetPrefPath), /dev rwcx */
const unveil_pair unveils[] = {
	{ MONO_PATH_DEFAULT,	"r"	},
	{ "/usr/lib",		"r"	},
	{ "/usr/local/lib",	"r"	},
	{ "/usr/X11R6",		"r"	},
	{ "/usr/share",		"r"	},
	{ "/etc",		"r"	}, /* XXX: only /etc/mono ? */
	{ "/dev",		"rw"	}, /* XXX: Vulkan: cx */
	{ "/tmp",		"rwc"	},
	{ ".",			"rwc"	}
};

const char *unveil_hide[] = {
	"FNA.dll",
	"FNA.dll.config",
	"I18N.CJK.dll",
	"I18N.MidEast.dll",
	"I18N.Other.dll",
	"I18N.Rare.dll",
	"I18N.West.dll",
	"I18N.dll",
	"Microsoft.CSharp.dll",
	"Mono.CSharp.dll",
	"Mono.Posix.dll",
	"Mono.Security.dll",
	"MonoGame.Framework.dll.config",
	"System.Configuration.dll",
	"System.Configuration.Install.dll",
	"System.Core.dll",
	"System.Data.dll",
	"System.Design.dll",
	"System.Drawing.dll",
	"System.IO.Compression.FileSystem.dll",
	"System.IO.Compression.dll",
	"System.Management.dll",
	"System.Net.dll",
	"System.Numerics.dll",
	"System.Runtime.Serialization.dll",
	"System.Security.dll",
	"System.ServiceModel.dll",
	"System.Transactions.dll",
	"System.Web.Extensions.dll",
	"System.Web.Http.dll",
	"System.Web.Services.dll",
	"System.Web.dll",
	"System.Windows.Forms.dll",
	"System.Xml.Linq.dll",
	"System.Xml.dll",
	"System.dll",
	"WindowsBase.dll",
	"libMonoPosixHelper.so",
	"libMonoPosixHelper.so.x86",
	"libMonoPosixHelper.so.x86_64",
	"libSteamworksNative.so",
	"mscorlib.dll"
};

int mono(int argc, char** argv) {
	MonoDomain	*domain;
	MonoAssembly	*assembly;
	char	*file = argv[0];
	char	*home_dir;
	char	*xdg_data_home;
	char	config_dir[PATH_MAX];
	char	localshare_dir[PATH_MAX];
	char	sndio_dir[PATH_MAX];
	char	xauthority[PATH_MAX];
	int	i, r;

	if (verbose)
		printf("parsing Dllmap\n");
	mono_config_parse_memory(Dllmap);	/* void */
	if (setenv("MONO_PATH", MONO_PATH_DEFAULT, 0) == -1)
		err(1, "setenv"); /* setenv BEFORE mono_jit_init */
	if (verbose)
		printf("initializing mono jit for %s\n", file);
	if ((domain = mono_jit_init(file)) == NULL)
		err(1, "mono_jit_init");
	if (verbose)
		printf("opening executable: %s\n", file);
	if ((assembly = mono_domain_assembly_open(domain, file)) == NULL)
		err(1, "mono_domain_assembly_open");

	if (verbose)
		printf("unveiling:\n");
	unveil_pair uvp;
	for (i = 0; i < sizeof(unveils) / sizeof(unveils[0]); i++) {
		uvp = unveils[i];
		if (verbose)
			printf(UNVEIL_VPRINT_FMT, uvp.path, uvp.permissions);
		unveil_err(uvp.path, uvp.permissions);
	}

	if ((home_dir = getenv("HOME")) == NULL)
		err(1, "getenv(\"HOME\")");

	if (snprintf(config_dir, sizeof(config_dir), "%s/.config", home_dir) < 0)
		err(1, "snprintf");
	else {
		if (verbose)
			printf(UNVEIL_VPRINT_FMT, config_dir, "rwc");
		unveil_err(config_dir, "rwc");
	}
	if (snprintf(localshare_dir, sizeof(localshare_dir), "%s/.local/share", home_dir) < 0)
		err(1, "snprintf");
	else {
		if (verbose)
			printf(UNVEIL_VPRINT_FMT, localshare_dir, "rwc");
		unveil_err(localshare_dir, "rwc");
	}
	if (snprintf(sndio_dir, sizeof(sndio_dir), "%s/.sndio", home_dir) < 0)
		err(1, "snprintf");
	else {
		if (verbose)
			printf(UNVEIL_VPRINT_FMT, sndio_dir, "rwc");
		unveil_err(sndio_dir, "rwc");
	}
	if (snprintf(xauthority, sizeof(xauthority), "%s/.Xauthority", home_dir) < 0)
		err(1, "snprintf");
	else {
		if (verbose)
			printf(UNVEIL_VPRINT_FMT, xauthority, "rw");
		unveil_err(xauthority, "rw");
	}
	if ((xdg_data_home = getenv("XDG_DATA_HOME")) != NULL) {
		if (verbose)
			printf(UNVEIL_VPRINT_FMT, xdg_data_home, "rwc");
		unveil_err(xdg_data_home, "rwc");
	}

	/* hide incompatible bundled files */
	for (i = 0; i < sizeof(unveil_hide) / sizeof(unveil_hide[0]); i++) {
		if (access(unveil_hide[i], F_OK) == 0) {
			if (verbose)
				printf(UNVEIL_VPRINT_FMT, unveil_hide[i], "");
			unveil_err(unveil_hide[i], "");
		}
	}
	if (verbose)
		printf("\n");

	if (unveil(NULL, NULL) == -1) err(1, "unveil");

	if (verbose) {
		printf("executing mono jit with the following arguments:");
		for (i = 0; i < argc; i++)
			printf(" \"%s\"", argv[i]);
		printf("\n\n");
	}
	/* mono_jit_exec needs argc >= 1 and argv[0] == main_assembly.exe */
	r = mono_jit_exec(domain, assembly, argc, argv);

	mono_jit_cleanup(domain);	/* void */

	return r;
}
