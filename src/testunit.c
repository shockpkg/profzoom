#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "b64.h"
#include "cmd.h"
#include "hook.h"

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

void assert_mem(
	int * err,
	const char * msg,
	size_t l,
	const void * a,
	const void * b
) {
	const unsigned char * _a = a;
	const unsigned char * _b = b;
	for (size_t i = 0; i < l; i++) {
		if (_a[i] != _b[i]) {
			*err = 1;
			printf("    FAIL: %s @ byte: %i\n", msg, (int)i);
			return;
		}
	}
	printf("    PASS: %s\n", msg);
}

void * read_pz_hook(
	const char * encode,
	size_t * size
) {
	*size = 0;
	pz_cmd_response response;
	if (!pz_hook("/someprefix/", encode, "rb", &response)) {
		return NULL;
	}

	fseek(response, 0, SEEK_END);
	size_t s = ftell(response);
	fseek(response, 0, SEEK_SET);

	void * r = (void *)malloc(s);
	if (!r) {
		fclose(response);
		return NULL;
	}

	if (fread(r, 1, s, response) != s) {
		fclose(response);
		return NULL;
	}
	fclose(response);
	*size = s;
	return r;
}

int test_pz_b64_decode(
	void
) {
	printf("test_pz_b64_decode:\n");
	int err = 0;
	typedef struct {
		const char * name;
		const char * encode;
		const char * data;
		size_t size;
	} sample;
	sample samples[] = {
		{
			.name = "size 18",
			.encode = "dGVzdGVyIGhlbGxvIHdvcmxk",
			.data = "tester hello world",
			.size = 18
		},
		{
			.name = "size 19",
			.encode = "dGVzdGVyIGhlbGxvIHdvcmxkIT",
			.data = "tester hello world!",
			.size = 19
		},
		{
			.name = "size 20",
			.encode = "dGVzdGVyIGhlbGxvIHdvcmxkIT8",
			.data = "tester hello world!?",
			.size = 20
		},
		{
			.name = "size 21",
			.encode = "dGVzdGVyIGhlbGxvIHdvcmxkIT8h",
			.data = "tester hello world!?!",
			.size = 21
		}
	};
	for (size_t i = 0; i < sizeof(samples) / sizeof(sample); i++) {
		sample s = samples[i];
		printf("  TEST: %s\n", s.name);

		size_t size;
		size_t len;
		void * data = pz_b64_decode(s.encode, &size, &len);
		assert_true(&err, "length", len == strlen(s.encode));
		assert_true(&err, "size", size == s.size);
		assert_mem(&err, "data", size, data, s.data);

		printf("\n");
	}
	printf("\n");
	return err;
}

int test_pz_cmd_decode(
	void
) {
	printf("test_pz_cmd_decode:\n");
	int err = 0;
	typedef struct {
		const char * name;
		const char * encode;
		const char * name_data;
		size_t name_size;
		size_t argc;
		size_t argv_total;
	} sample;
	sample samples[] = {
		{
			.name = "argc 0",
			.encode = "c29tZU1ldGhvZA",
			.name_data = "someMethod",
			.name_size = 10,
			.argc = 0,
			.argv_total = 0
		},
		{
			.name = "argc 1",
			.encode = "c29tZU1ldGhvZA"
				"$"
				"c29tZWFyZw",
			.name_data = "someMethod",
			.name_size = 10,
			.argc = 1,
			.argv_total = 7
		},
		{
			.name = "argc 2",
			.encode = "c29tZU1ldGhvZA"
				"$"
				"c29tZWFyZw"
				"$"
				"",
			.name_data = "someMethod",
			.name_size = 10,
			.argc = 2,
			.argv_total = 7
		},
		{
			.name = "argc 3",
			.encode = "c29tZU1ldGhvZA"
				"$"
				"c29tZWFyZw"
				"$"
				"$"
				"b3RoZXJhcmc",
			.name_data = "someMethod",
			.name_size = 10,
			.argc = 3,
			.argv_total = 15
		}
	};
	for (size_t i = 0; i < sizeof(samples) / sizeof(sample); i++) {
		sample s = samples[i];
		printf("  TEST: %s\n", s.name);

		pz_cmd * cmd = pz_cmd_decode(s.encode);
		size_t argv_total = 0;
		for (size_t i = 0; i < cmd->argc; i++) {
			argv_total += cmd->argv[i].size;
		}

		assert_true(&err, "name_size", cmd->name_size == s.name_size);
		assert_mem(&err, "name_data", s.name_size, cmd->name_data, s.name_data);
		assert_true(&err, "argc", cmd->argc == s.argc);
		assert_true(&err, "argv_total", argv_total == s.argv_total);

		pz_cmd_free(cmd);
		printf("\n");
	}
	printf("\n");
	return err;
}

int test_pz_hook_envget(
	void
) {
	printf("test_pz_hook_envget:\n");
	int err = 0;
	typedef struct {
		const char * name;
		const char * encode;
		int error;
		int empty;
		int null;
	} sample;
	sample samples[] = {
		{
			.name = "env: IS_SET",
			.encode = "/someprefix/"
				"ZW52Z2V0"
				"$"
				"SVNfU0VU",
			.error = 0,
			.empty = 0,
			.null = 0
		},
		{
			.name = "env: DOES_NOT_EXIST",
			.encode = "/someprefix/"
				"ZW52Z2V0"
				"$"
				"Tk9UX0VYSVNU",
			.error = 0,
			.empty = 0,
			.null = 1
		},
		{
			.name = "env: IS_EMPTY",
			.encode = "/someprefix/"
				"ZW52Z2V0"
				"$"
				"SVNfRU1QVFk",
			.error = 0,
			.empty = 1,
			.null = 0
		},
		{
			.name = "0 arguments",
			.encode = "/someprefix/"
				"ZW52Z2V0",
			.error = 1,
			.empty = 0,
			.null = 0
		},
		{
			.name = "2 arguments",
			.encode = "/someprefix/"
				"ZW52Z2V0"
				"$"
				"UEFUSA"
				"$"
				"UEFUSA",
			.error = 1,
			.empty = 0,
			.null = 0
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
		assert_true(&err, "not null", data != NULL);
		assert_true(&err, "not empty", size != 0);
		if (!data) {
			continue;
		}

		// char * d = response.data;
		if (s.error) {
			assert_true(&err, "is error", data[size - 1] == '\x00');
		}
		else {
			assert_true(&err, "no error", data[size - 1] != '\x00');

			if (s.null) {
				assert_true(&err, "is null", data[size - 1] == '\x02');
			}
			else {
				assert_true(&err, "has data", data[size - 1] == '\x01');
			}
		}
		free(data);
		printf("\n");
	}
	printf("\n");
	return err;
}

int test_pz_hook_fileread(
	void
) {
	printf("test_pz_hook_fileread:\n");
	int err = 0;
	typedef struct {
		const char * name;
		const char * encode;
		const char * data;
		int error;
	} sample;
	sample samples[] = {
		{
			.name = "read: testunit-data/data.txt",
			.encode = "/someprefix/"
				"ZmlsZXJlYWQ"
				"$"
				"dGVzdHVuaXQtZGF0YS9kYXRhLnR4dA",
			.data = "readdata",
			.error = 0
		},
		{
			.name = "0 arguments",
			.encode = "/someprefix/"
				"ZmlsZXJlYWQ",
			.data = NULL,
			.error = 1
		},
		{
			.name = "2 arguments",
			.encode = "/someprefix/"
				"ZmlsZXJlYWQ"
				"$"
				"dGVzdHVuaXQtZGF0YS9kYXRhLnR4dA"
				"$"
				"MQ",
			.data = NULL,
			.error = 1
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

		if (s.error) {
			assert_true(&err, "is error", data[size - 1] == '\x00');
		}
		else {
			assert_true(&err, "no error", data[size - 1] != '\x00');
		}

		if (s.data) {
			assert_true(&err, "has data", data[size - 1] == '\x01');
		}

		free(data);
		printf("\n");
	}
	printf("\n");
	return err;
}

int test_pz_hook_filewrite(
	void
) {
	printf("test_pz_hook_filewrite:\n");
	int err = 0;
	typedef struct {
		const char * name;
		const char * encode;
		int error;
	} sample;
	sample samples[] = {
		{
			.name = "write: 0 testunit-data/file.txt",
			.encode = "/someprefix/"
				"ZmlsZXdyaXRl"
				"$"
				"MA"
				"$"
				"dGVzdHVuaXQtZGF0YS9maWxlLnR4dA"
				"$"
				"ZGF0YQo",
			.error = 0
		},
		{
			.name = "write: 0 testunit-data/sub/file.txt",
			.encode = "/someprefix/"
				"ZmlsZXdyaXRl"
				"$"
				"MA"
				"$"
				"dGVzdHVuaXQtZGF0YS9zdWIvZmlsZS50eHQ"
				"$"
				"ZGF0YQo",
			.error = 1
		},
		{
			.name = "write: 1 testunit-data/sub/file.txt",
			.encode = "/someprefix/"
				"ZmlsZXdyaXRl"
				"$"
				"MQ"
				"$"
				"dGVzdHVuaXQtZGF0YS9zdWIvZmlsZS50eHQ"
				"$"
				"ZGF0YQo",
			.error = 0
		},
		{
			.name = "0 arguments",
			.encode = "/someprefix/"
				"ZmlsZXdyaXRl",
			.error = 1
		},
		{
			.name = "4 arguments",
			.encode = "/someprefix/"
				"ZmlsZXdyaXRl"
				"$"
				"MA"
				"$"
				"MA"
				"$"
				"MA"
				"$"
				"MA",
			.error = 1
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

		if (s.error) {
			assert_true(&err, "is error", data[size - 1] == '\x00');
		}
		else {
			assert_true(&err, "no error", data[size - 1] != '\x00');
		}

		free(data);
		printf("\n");
	}
	printf("\n");
	return err;
}

int main(void) {
	int err = 0;
	err = test_pz_b64_decode() || err;
	err = test_pz_cmd_decode() || err;
	err = test_pz_hook_envget() || err;
	err = test_pz_hook_fileread() || err;
	err = test_pz_hook_filewrite() || err;
	printf("%s\n", err ? "FAIL" : "PASS");
	return err;
}
