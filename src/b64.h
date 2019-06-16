#ifndef _PZ_B64_H
#define _PZ_B64_H

#include <stddef.h>

void * pz_b64_decode(
	const char * data,
	size_t * size,
	size_t * len
);

#endif
