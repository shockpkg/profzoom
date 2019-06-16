#include "fileread.h"

pz_cmd_response pz_hook_fileread(
	const pz_cmd * cmd
) {
	if (cmd->argc != 1) {
		return pz_cmd_response_error_argc();
	}

	const char * arg0d = cmd->argv[0].data;
	FILE * f = fopen(arg0d, "rb");
	if (!f) {
		return pz_cmd_response_error("Failed to read file", 0);
	}

	pz_cmd_response r = pz_cmd_response_from_file(f);
	fclose(f);
	return r;
}
