#include "filewrite.h"

#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>

pz_cmd_response pz_hook_filewrite(
	const pz_cmd * cmd
) {
	if (cmd->argc != 3) {
		return pz_cmd_response_error_argc();
	}

	const char * arg0d = cmd->argv[0].data;

	char * arg1d = cmd->argv[1].data;
	FILE * f = fopen(arg1d, "wb");

	// If failed to open, maybe need to create the path.
	if (!f && errno == ENOENT && arg0d[0] == '1') {
		// Create path.
		size_t l = cmd->argv[1].size;
		int slashed = 0;
		for (size_t i = 0; i < l; i++) {
			char c = arg1d[i];
			if (c != '/') {
				slashed = 1;
				continue;
			}
			if (!slashed) {
				continue;
			}

			arg1d[i] = '\x00';
			mkdir(arg1d, S_IRWXU);
			arg1d[i] = c;
		}

		// Try to repoen file.
		f = fopen(arg1d, "wb");
	}

	if (!f) {
		return pz_cmd_response_error("Failed to write file", 0);
	}

	const char * arg2d = cmd->argv[2].data;
	size_t arg2s = cmd->argv[2].size;
	fwrite(arg2d, 1, arg2s, f);
	fclose(f);
	return pz_cmd_response_from_null();
}
