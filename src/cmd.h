#ifndef _PZ_CMD_H
#define _PZ_CMD_H

#include <stddef.h>
#include <stdio.h>

typedef struct {
	size_t size;
	void * data;
} pz_cmd_arg;

typedef struct {
	size_t name_size;
	void * name_data;
	size_t argc;
	pz_cmd_arg argv[];
} pz_cmd;

typedef FILE * pz_cmd_response;

pz_cmd * pz_cmd_decode(
	const char * data
);

void pz_cmd_free(
	pz_cmd * cmd
);

pz_cmd_response pz_cmd_response_from_null(
	void
);

pz_cmd_response pz_cmd_response_from_empty(
	void
);

pz_cmd_response pz_cmd_response_from_data(
	const void * data,
	size_t size
);

pz_cmd_response pz_cmd_response_from_string(
	const char * str
);

pz_cmd_response pz_cmd_response_from_file(
	FILE * f
);

pz_cmd_response pz_cmd_response_error(
	const char * msg,
	int heap
);

pz_cmd_response pz_cmd_response_error_argc(
	void
);

pz_cmd_response pz_cmd_response_error_unknown_command_name(
	void
);

#endif
