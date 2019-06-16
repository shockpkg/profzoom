#ifndef _PZ_HOOK_H
#define _PZ_HOOK_H

#include "cmd.h"

int pz_hook(
	const char * prefix,
	const char * path,
	const char * mode,
	pz_cmd_response * response
);

#endif
