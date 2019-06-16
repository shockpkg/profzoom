#include "b64.h"

#include <stdlib.h>

static unsigned char pz_b64_decode_table[256] = {
	'\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
	'\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
	'\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
	'\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
	'\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
	'\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\x3F', '\xFF', '\xFF',
	'\x34', '\x35', '\x36', '\x37', '\x38', '\x39', '\x3A', '\x3B',
	'\x3C', '\x3D', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
	'\xFF', '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06',
	'\x07', '\x08', '\x09', '\x0A', '\x0B', '\x0C', '\x0D', '\x0E',
	'\x0F', '\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16',
	'\x17', '\x18', '\x19', '\xFF', '\xFF', '\xFF', '\xFF', '\x3E',
	'\xFF', '\x1A', '\x1B', '\x1C', '\x1D', '\x1E', '\x1F', '\x20',
	'\x21', '\x22', '\x23', '\x24', '\x25', '\x26', '\x27', '\x28',
	'\x29', '\x2A', '\x2B', '\x2C', '\x2D', '\x2E', '\x2F', '\x30',
	'\x31', '\x32', '\x33', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
	'\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
	'\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
	'\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
	'\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
	'\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
	'\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
	'\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
	'\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
	'\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
	'\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
	'\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
	'\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
	'\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
	'\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
	'\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF',
	'\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF'
};

void * pz_b64_decode(
	const char * data,
	size_t * size,
	size_t * len
) {
	const unsigned char * udata = (const unsigned char *)data;
	const unsigned char * d;

	// Find end with first byte that is not in the lookup table.
	size_t l = 0;
	for (d = udata; pz_b64_decode_table[*d] != 0xFF; d++);
	l = d - udata;
	if (len) {
		*len = l;
	}
	size_t o = (l % 4);
	size_t l4 = l - o;

	// Calculate the decode size, or return failure if bad size.
	size_t s = (l / 4) * 3;
	if (o) {
		if (o == 1) {
			return NULL;
		}
		s += o - 1;
	}

	// Allocate memory for the decode, or return failure.
	// Include an extra null byte after, for easy string casting.
	unsigned char * r = (unsigned char * )malloc(s + 1);
	if (!r) {
		return NULL;
	}
	r[s] = '\x00';

	// Decode the encoded string.
	d = udata;
	unsigned char * p = r;
	for (const unsigned char * end = d + l4; d < end;) {
		unsigned char b1 = pz_b64_decode_table[*(d++)];
		unsigned char b2 = pz_b64_decode_table[*(d++)];
		unsigned char b3 = pz_b64_decode_table[*(d++)];
		unsigned char b4 = pz_b64_decode_table[*(d++)];
		*(p++) = (b1 << 2) | (b2 >> 4);
		*(p++) = ((b2 & 15) << 4) | (b3 >> 2);
		*(p++) = ((b3 & 3) << 6) | (b4 & 63);
	}
	if (o == 2) {
		unsigned char b1 = pz_b64_decode_table[*(d++)];
		unsigned char b2 = pz_b64_decode_table[*(d++)];
		*(p++) = (b1 << 2) | (b2 >> 4);
	}
	else if (o == 3) {
		unsigned char b1 = pz_b64_decode_table[*(d++)];
		unsigned char b2 = pz_b64_decode_table[*(d++)];
		unsigned char b3 = pz_b64_decode_table[*(d++)];
		*(p++) = (b1 << 2) | (b2 >> 4);
		*(p++) = ((b2 & 15) << 4) | (b3 >> 2);
	}

	// Pass out the size and return data pointer.
	if (size) {
		*size = s;
	}
	return r;
}
