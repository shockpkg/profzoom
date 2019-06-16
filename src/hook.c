#include "hook.h"

#include "hooks/envget.h"
#include "hooks/fileread.h"
#include "hooks/filewrite.h"

typedef struct {
	const char * name;
	size_t size;
	FILE * (*hook)(const pz_cmd * cmd);
} hook;

#define pz_hooked(_name, _hook) {\
	.name = _name,\
	.size = sizeof(_name) - 1,\
	.hook = _hook\
}

static hook hooks[] = {
	pz_hooked("envget", pz_hook_envget),
	pz_hooked("fileread", pz_hook_fileread),
	pz_hooked("filewrite", pz_hook_filewrite)
};

int pz_hook(
	const char * prefix,
	const char * path,
	const char * mode,
	pz_cmd_response * response
) {
	// Unknown error is default.
	*response = NULL;

	// Check for the prefix, return false if not.
	const char * p = prefix;
	const char * f = path;
	while (1) {
		if (!*p) {
			break;
		}
		if (*f != *p) {
			return 0;
		}
		f++;
		p++;
	}

	// Check if read mode, return hooked even if not valid.
	const char * m = mode;
	while (*m) {
		if (*m != 'r' && *m != 'b') {
			return 1;
		}
		m++;
	}

	// Prefix matches, and in read mode, decode remaining.
	pz_cmd * cmd = pz_cmd_decode(f);

	int found = 0;
	for (size_t i = 0; i < sizeof(hooks) / sizeof(hook); i++) {
		// Check if name is same length.
		if (hooks[i].size != cmd->name_size) {
			continue;
		}

		// Check if name matches.
		const char * a = (const char *)cmd->name_data;
		const char * b = hooks[i].name;
		int match = 1;
		while (*a || *b) {
			if (*a != *b) {
				match = 0;
				break;
			}
			a++;
			b++;
		}
		if (!match) {
			continue;
		}

		*response = hooks[i].hook(cmd);
		found = 1;
		break;
	}
	if (!found) {
		*response = pz_cmd_response_error_unknown_command_name();
	}

	pz_cmd_free(cmd);
	return 1;
}
