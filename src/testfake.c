#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void assert_true(
	int * err,
	const char * msg,
	int value
) {
	if (!value) {
		*err = 1;
	}
	printf("    %s: %s\n", (value ? "PASS" : "FAIL"), msg);
}

void * read_pz_hook(
	const char * encode,
	size_t * size
) {
	FILE * response = fopen(encode, "rb");
	if (!response) {
		return NULL;
	}

	fseek(response, 0, SEEK_END);
	long s = ftell(response);
	fseek(response, 0, SEEK_SET);

	void * r = (void *)malloc(s);
	if (!r) {
		fclose(response);
		return NULL;
	}

	fread(r, 1, s, response);
	fclose(response);
	*size = s;
	return r;
}

int test_all(
	void
) {
	printf("test_all:\n");
	int err = 0;
	typedef struct {
		const char * name;
		const char * encode;
	} sample;
	sample samples[] = {
		{
			.name = "env: IS_SET",
			.encode = "/someprefix/"
				"ZW52Z2V0"
				"$"
				"SVNfU0VU"
		},
		{
			.name = "env: NOT_EXIST",
			.encode = "/someprefix/"
				"ZW52Z2V0"
				"$"
				"Tk9UX0VYSVNU"
		},
		{
			.name = "env: IS_EMPTY",
			.encode = "/someprefix/"
				"ZW52Z2V0"
				"$"
				"SVNfRU1QVFk"
		},
		{
			.name = "read: testfake-data/data.txt",
			.encode = "/someprefix/"
				"ZmlsZXJlYWQ"
				"$"
				"dGVzdGZha2UtZGF0YS9kYXRhLnR4dA"
		},
		{
			.name = "write: 0 testfake-data/file.txt",
			.encode = "/someprefix/"
				"ZmlsZXdyaXRl"
				"$"
				"MA"
				"$"
				"dGVzdGZha2UtZGF0YS9maWxlLnR4dA"
				"$"
				"ZGF0YQo"
		},
		{
			.name = "write: 1 testfake-data/sub/file.txt",
			.encode = "/someprefix/"
				"ZmlsZXdyaXRl"
				"$"
				"MQ"
				"$"
				"dGVzdGZha2UtZGF0YS9zdWIvZmlsZS50eHQ"
				"$"
				"ZGF0YQo"
		}
	};
	for (size_t i = 0; i < sizeof(samples) / sizeof(sample); i++) {
		sample s = samples[i];
		printf("  TEST: %s\n", s.name);

		size_t size = 0;
		unsigned char * data = (unsigned char *)read_pz_hook(
			s.encode,
			&size
		);
		assert_true(&err, "data", !!data);

		printf("      data:");
		for (size_t i = 0; i < size; i++) {
			printf(" %02X", data[i]);
		}
		printf("\n");

		free(data);
		printf("\n");
	}
	printf("\n");
	return err;
}

int main(void) {
	int err = 0;
	err = test_all() || err;
	printf("%s\n", err ? "FAIL" : "PASS");
	return err;
}
