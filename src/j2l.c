/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "config.h"
#include "common.h"

#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// ==========================================================================================================
static noreturn void
usage(int status)
{
	printf("usage: j2l [OPTION...] [FILE]\n"
	       "transform json from FILE, to jsonl, and write to standard output.\n"
	       "\n"
	       "with no FILE, or when FILE is -, read from standard input\n\n"
	       "options:\n"
	       "  -h    display this help and exit.\n"
	       "  -V    display version information and exit.\n"
	       "\n");
	exit(status);
}

static int
do_transform(FILE* restrict in, FILE* restrict out)
{
	char* buf = malloc(BUFSIZ);
	if (!buf) {
		error(EXIT_FAILURE, errno, "malloc");
	}

	do {
		size_t r = fread(buf, 1, BUFSIZ, in);
		if (r <= 0) break;

		// transform

		if (fwrite(buf, 1, r, out) < r) {
			error(EXIT_FAILURE, 0, "write error");
		}
	} while (!feof(in) && !ferror(in));

	if (ferror(in)) {
		error(EXIT_FAILURE, 0, "read error");
	}

	free(buf);
	return 0;
}

static FILE*
open_stream(const char* path)
{
	FILE* fp = NULL;

	if (strcmp(path, "-") == 0) {
		fp = stdin;
	} else {
		fp = fopen(path, "rb");
		if (!fp) {
			error(EXIT_FAILURE, errno, "%s", path);
		}
	}

	setvbuf(fp, NULL, _IONBF, 0);
	return fp;
}

int
main(int argc, char* const argv[argc])
{
	int opt;
	while ((opt = getopt(argc, argv, ":hV")) != -1) {
		switch (opt) {
		case 'h': usage(EXIT_SUCCESS);
		case 'V': printf("j2l v%s\n\n", J2L_VERSION); return EXIT_SUCCESS;
		case ':': error(EXIT_FAILURE, 0, "option '%c' requires an argument", optopt); break;
		case '?': error(EXIT_FAILURE, 0, "unknown option '%c'", optopt); break;
		default:  UNREACHABLE();
		}
	}

	FILE* in = NULL;
	if (optind == argc) {
		in = open_stream("-");
	} else {
		const char* inpath = argv[optind++];

		if (optind != argc) {
			error(EXIT_FAILURE, 0, "extra operand '%s'", argv[optind]);
		}

		in = open_stream(inpath);
	}

	int status = do_transform(in, stdout);
	fclose(in);

	return status;
}
