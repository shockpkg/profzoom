#define _GNU_SOURCE

#include "hook.h"

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

static const char * pz_prefix_env = "PROFZOOM_PREFIX";
static const char * pz_prefix_default = "/.profzoom/";

const char * pz_prefix(void) {
	static const char * r = NULL;
	if (!r) {
		r = getenv(pz_prefix_env);
		if (!r) {
			r = pz_prefix_default;
		}
	}
	return r;
}

FILE * fopen(
	const char * filename,
	const char * mode
) {
	// Check if the path is hooked.
	pz_cmd_response response = NULL;
	if (pz_hook(pz_prefix(), path, mode, &response)) {
		return response;
	}

	// Otherwise continue to the next fopen.
	FILE * (*_fopen)(const char*, const char*);
	*(void **) (&_fopen) = dlsym(RTLD_NEXT, "fopen");
	return (*_fopen)(path, mode);
}

FILE * fopen64(
	const char * filename,
	const char * mode
) {
	// Check if the path is hooked.
	pz_cmd_response response = NULL;
	if (pz_hook(pz_prefix(), path, mode, &response)) {
		return response;
	}

	// Otherwise continue to the next fopen64.
	FILE * (*_fopen)(const char*, const char*);
	*(void **) (&_fopen) = dlsym(RTLD_NEXT, "fopen64");
	return (*_fopen)(path, mode);
}
