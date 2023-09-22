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

#include <jni.h>	/* /usr/local/jdk-1.8.0/include/jni.h etc for 11, 17 */

#include "rigg.h"
#include "rigg_unveil.h"

static const unveil_pair unveils[] = {
};


int java17(int argc, char** argv) {

	errx(1, "not implemented: java");
	return 1;

#if 0
	/* https://docs.oracle.com/javase/8/docs/technotes/guides/jni/spec/invocation.html */

	JavaVM		*jvm;
	JNIEnv		*env;
	JavaVMInitArgs	vm_args;
	JavaVMOption	*options = new JavaVMOption[1];

	options[0].optionString		= "-Djava.class.path=/usr/local/jdk-17";
	vm_args.version			= JNI_VERSION_1_6;
	vm_args.nOptions		= 1;
	vm_args.options			= options;
	vm_args.ignoreUnrecognized	= false

	JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);
	delete options;

	/* invoke the specified method */
	jclass cls	= env->FindClass("Main");	/* XXX: replace Main */
	jmethodID mid	= env->GetStaticMethodID(cls, "test", "(I)V");	/* XXX: replace test */
	env->CallStaticVoidMethod(cls, mid, 100);

	jvm->DestroyJavaVM();
#endif

#if 0
	vprintf("unveiling:\n");
	for (i = 0; i < sizeof(unveils) / sizeof(unveils[0]); i++) {
		uvp = unveils[i];
		vprintf(UNVEIL_VPRINT_FMT, uvp.path, uvp.permissions);
		unveil_err(uvp.path, uvp.permissions);
	}

	if ((home_dir = getenv("HOME")) == NULL)
		err(1, "getenv(\"HOME\")");

	if (snprintf(config_dir, sizeof(config_dir), "%s/.config", home_dir) < 0)
		err(1, "snprintf");
	else {
		vprintf(UNVEIL_VPRINT_FMT, config_dir, "rwc");
		unveil_err(config_dir, "rwc");
	}
	if (snprintf(localshare_dir, sizeof(localshare_dir), "%s/.local/share", home_dir) < 0)
		err(1, "snprintf");
	else {
		vprintf(UNVEIL_VPRINT_FMT, localshare_dir, "rwc");
		unveil_err(localshare_dir, "rwc");
	}
	if (snprintf(sndio_dir, sizeof(sndio_dir), "%s/.sndio", home_dir) < 0)
		err(1, "snprintf");
	else {
		vprintf(UNVEIL_VPRINT_FMT, sndio_dir, "rwc");
		unveil_err(sndio_dir, "rwc");
	}
	if (snprintf(xauthority, sizeof(xauthority), "%s/.Xauthority", home_dir) < 0)
		err(1, "snprintf");
	else {
		vprintf(UNVEIL_VPRINT_FMT, xauthority, "rw");
		unveil_err(xauthority, "rw");
	}
	if ((xdg_data_home = getenv("XDG_DATA_HOME")) != NULL) {
		vprintf(UNVEIL_VPRINT_FMT, xdg_data_home, "rwc");
		unveil_err(xdg_data_home, "rwc");
	}

	/* hide incompatible bundled files */
	for (i = 0; i < sizeof(unveil_hide) / sizeof(unveil_hide[0]); i++) {
		if (access(unveil_hide[i], F_OK) == 0) {
			vprintf(UNVEIL_VPRINT_FMT, unveil_hide[i], "");
			unveil_err(unveil_hide[i], "");
		}
	}

	vprintf("\n");

	unveil_err(NULL, NULL);

	vprintf("executing mono jit with the following arguments:");
	for (i = 0; i < argc; i++)
		vprintf(" \"%s\"", argv[i]);
	vprintf("\n\n");

	vprintf("cleaning up...\n");

	return r;
#endif

}
