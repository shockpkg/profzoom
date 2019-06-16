#include "envget.h"

#include <stdlib.h>

pz_cmd_response pz_hook_envget(
	const pz_cmd * cmd
) {
	if (cmd->argc != 1) {
		return pz_cmd_response_error_argc();
	}

	const char * env = getenv(cmd->argv[0].data);

	return env ?
		pz_cmd_response_from_string(env) :
		pz_cmd_response_from_null();
}
