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

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-config.h>

#include "rigg.h"
#include "rigg_mono.h"
#include "rigg_unveil.h"

/* XXX: Vulkan will need: /home (SDL_GetPrefPath), /dev rwcx */
static const unveil_pair unveils[] = {
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

const unveil_quirk unveil_quirks[] = {
	{ "DadQuest.exe",	"HOME", "Library/Application Support/DadQuest",	"rwc"	},
	{ "NeuroVoider.exe",	"HOME",	".neurovoider",				"rwc"	},
	{ "NeuroVoider.exe",	"",	"NeuroVoider.exe.config",		""	}
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
	"System.ComponentModel.DataAnnotations.dll",
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
	"mscorlib.dll",
	"x86",
	"x64"
};

int mono(int argc, char** argv) {
	MonoDomain	*domain;
	MonoAssembly	*assembly;
	unveil_pair	uvp;
	char	*file = argv[0];
	char	*home_dir;
	char	*xdg_data_home;
	char	*xdg_config_home;
	char	config_dir[PATH_MAX];
	char	localshare_dir[PATH_MAX];
	char	sndio_dir[PATH_MAX];
	char	xauthority[PATH_MAX];
	int	i, r;

	vprintf("parsing Dllmap\n");
	mono_config_parse_memory(Dllmap);	/* void */
	if (setenv("MONO_PATH", MONO_PATH_DEFAULT, 0) == -1)
		err(1, "setenv"); /* setenv BEFORE mono_jit_init */
	vprintf("initializing mono jit for %s\n", file);
	if ((domain = mono_jit_init(file)) == NULL)
		err(1, "mono_jit_init");
	vprintf("opening executable: %s\n", file);
	if ((assembly = mono_domain_assembly_open(domain, file)) == NULL)
		err(1, "mono_domain_assembly_open");

	/* unveils */
	vprintf("\n");
	for (i = 0; i < sizeof(unveils) / sizeof(unveils[0]); i++) {
		uvp = unveils[i];
		unveil_err(uvp.path, uvp.permissions);
	}

	if ((home_dir = getenv("HOME")) == NULL)
		err(1, "getenv(\"HOME\")");

	if (snprintf(config_dir, sizeof(config_dir), "%s/.config", home_dir) < 0)
		err(1, "snprintf");
	else {
		unveil_err(config_dir, "rwc");
	}
	if (snprintf(localshare_dir, sizeof(localshare_dir), "%s/.local/share", home_dir) < 0)
		err(1, "snprintf");
	else {
		unveil_err(localshare_dir, "rwc");
	}
	if (snprintf(sndio_dir, sizeof(sndio_dir), "%s/.sndio", home_dir) < 0)
		err(1, "snprintf");
	else {
		unveil_err(sndio_dir, "rwc");
	}
	if (snprintf(xauthority, sizeof(xauthority), "%s/.Xauthority", home_dir) < 0)
		err(1, "snprintf");
	else {
		unveil_err(xauthority, "rw");
	}
	if ((xdg_data_home = getenv("XDG_DATA_HOME")) != NULL) {
		unveil_err(xdg_data_home, "rwc");
	}
	if ((xdg_config_home = getenv("XDG_CONFIG_HOME")) != NULL) {
		unveil_err(xdg_config_home, "rwc");
	}

	/* hide incompatible bundled files */
	for (i = 0; i < sizeof(unveil_hide) / sizeof(unveil_hide[0]); i++) {
		if (access(unveil_hide[i], F_OK) == 0) {
			unveil_err(unveil_hide[i], "");
		}
	}

	/* quirks based on file */
	unveil_quirk uvq;
	char *uvq_ev;
	char uvq_fullpath[PATH_MAX];
	for (i = 0; i < sizeof(unveil_quirks) / sizeof(unveil_quirks[0]); i++) {
		uvq = unveil_quirks[i];
		if (strncmp(uvq.file, file, MAX_INPUT) != 0)
			continue;
		if ((uvq_ev = getenv(uvq.env_var)) == NULL) {
			unveil_err(uvq.path, uvq.permissions);
		}
		else {
			if (snprintf(uvq_fullpath, sizeof(uvq_fullpath),
				"%s/%s", getenv(uvq.env_var), uvq.path) < 0) {
					err(1, "snprintf");
			}
			unveil_err(uvq_fullpath, uvq.permissions);
		}
	}

	unveil_err(NULL, NULL);
	vprintf("\n");

	vprintf("executing mono jit with the following arguments:");
	for (i = 0; i < argc; i++)
		vprintf(" \"%s\"", argv[i]);
	vprintf("\n\n");
	/* mono_jit_exec needs argc >= 1 and argv[0] == main_assembly.exe */
	r = mono_jit_exec(domain, assembly, argc, argv);

	vprintf("cleaning up...\n");
	mono_jit_cleanup(domain);	/* void */

	return r;
}
