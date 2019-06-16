#define _GNU_SOURCE

#include "cmd.h"

#include <stdlib.h>
#include <string.h>

#include "b64.h"

static const char pz_cmd_response_data_null[] = {'\x02'};

static const char pz_cmd_response_data_empty[] = {'\x01'};

pz_cmd * pz_cmd_decode(
	const char * data
) {
	const char * d = data;

	// Count arguments.
	size_t argc = 0;
	for (; *d; d++) {
		if (*d == '$') {
			argc++;
		}
	}

	// Allocate memory or fail.
	pz_cmd * r = (pz_cmd *)malloc(sizeof(pz_cmd) + sizeof(pz_cmd_arg) * argc);
	if (!r) {
		return NULL;
	}

	// Initialize structure.
	r->name_data = NULL;
	r->argc = argc;
	for (size_t i = 0; i < argc; i++) {
		r->argv[i].data = NULL;
	}

	// Decode data.
	d = data;
	do {
		size_t size;
		size_t len;
		void * dec;

		// Decode name or fail.
		dec = pz_b64_decode(d, &size, &len);
		if (!dec) {
			break;
		}
		d += len + 1;
		r->name_data = dec;
		r->name_size = size;

		// Decode arguments or fail.
		int fail = 0;
		for (size_t i = 0; i < argc; i++) {
			dec = pz_b64_decode(d, &size, &len);
			if (!dec) {
				fail = 1;
				break;
			}
			d += len + 1;
			r->argv[i].data = dec;
			r->argv[i].size = size;
		}
		if (fail) {
			break;
		}

		// Successful decode.
		return r;
	}
	while (0);

	// Failed to finish decode, free memory.
	if (r->name_data) {
		free(r->name_data);
	}
	for (size_t i = 0; i < argc; i++) {
		if (r->argv[i].data) {
			free(r->argv[i].data);
		}
	}
	free(r);
	return NULL;
}

void pz_cmd_free(
	pz_cmd * cmd
) {
	free(cmd->name_data);
	for (size_t i = 0, l = cmd->argc; i < l; i++) {
		free(cmd->argv[i].data);
	}
	free(cmd);
}

pz_cmd_response pz_cmd_response_from_null(
	void
) {
	return fmemopen(
		(void *)pz_cmd_response_data_null,
		sizeof(pz_cmd_response_data_null),
		"rb"
	);
}

pz_cmd_response pz_cmd_response_from_empty(
	void
) {
	return fmemopen(
		(void *)pz_cmd_response_data_empty,
		sizeof(pz_cmd_response_data_empty),
		"rb"
	);
}

pz_cmd_response pz_cmd_response_from_data(
	const void * data,
	size_t size
) {
	// Use empty if empty.
	if (!size) {
		return pz_cmd_response_from_empty();
	}

	pz_cmd_response r = fmemopen(
		NULL,
		size + 1,
		"rb+"
	);
	if (!r) {
		return NULL;
	}

	// Write data, or fail.
	char footer = '\x01';
	if (
		fwrite(data, 1, size, r) != size ||
		fwrite(&footer, 1, 1, r) != 1
	) {
		fclose(r);
		return pz_cmd_response_error("Error creating response", 0);
	}


	fseek(r, 0, SEEK_SET);
	return r;
}

pz_cmd_response pz_cmd_response_from_string(
	const char * str
) {
	return pz_cmd_response_from_data(str, strlen(str));
}

pz_cmd_response pz_cmd_response_from_file(
	FILE * f
) {
	// Remember offset, get size.
	long before = ftell(f);
	fseek(f, 0, SEEK_END);
	long size = ftell(f);
	fseek(f, 0, SEEK_SET);

	pz_cmd_response r = fmemopen(
		NULL,
		size + 1,
		"rb+"
	);
	if (!r) {
		return NULL;
	}

	// Write data in chunks.
	int success = 1;
	do {
		char buffer[1024];
		size_t size_chunk = sizeof(buffer);
		size_t size_remain = size % sizeof(buffer);
		size_t size_limited = size - size_remain;
		for (size_t i = 0; i < size_limited; i += size_chunk) {
			if (fread(buffer, 1, size_chunk, f) != size_chunk) {
				success = 0;
				break;
			}
			if (fwrite(buffer, 1, size_chunk, r) != size_chunk) {
				success = 0;
				break;
			}
		}
		if (success && size_remain) {
			if (fread(buffer, 1, size_remain, f) != size_remain) {
				success = 0;
				break;
			}
			if (fwrite(buffer, 1, size_remain, r) != size_remain) {
				success = 0;
				break;
			}
		}
	}
	while (0);
	fseek(f, before, SEEK_SET);

	// Fail if any errors with reading.
	if (!success) {
		fclose(r);
		return pz_cmd_response_error("Error reading file", 0);
	}

	// Write footer, or fail.
	char footer = '\x01';
	if (fwrite(&footer, 1, 1, r) != 1) {
		fclose(r);
		return pz_cmd_response_error("Error creating response", 0);
	}

	fseek(r, 0, SEEK_SET);
	return r;
}

pz_cmd_response pz_cmd_response_error(
	const char * msg,
	int heap
) {
	size_t l = strlen(msg);
	if (!l) {
		return NULL;
	}
	if (!heap) {
		return fmemopen((void *)msg, l + 1, "rb");
	}
	pz_cmd_response r = fmemopen(
		NULL,
		l + 1,
		"rb+"
	);
	if (!r) {
		return NULL;
	}
	fwrite(msg, 1, l + 1, r);
	fseek(r, 0, SEEK_SET);
	return r;
}

pz_cmd_response pz_cmd_response_error_argc(
	void
) {
	return pz_cmd_response_error("Bad argument count", 0);
}

pz_cmd_response pz_cmd_response_error_unknown_command_name(
	void
) {
	return pz_cmd_response_error("Unknown command name", 0);
}
