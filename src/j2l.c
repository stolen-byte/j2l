/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "config.h"
#include "common.h"
#include "error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// ==========================================================================================================
static noreturn void
usage(int status)
{
	printf("usage: %s [OPTION...] [FILE]\n"
	       "transform json from FILE, to jsonl, and write to standard output.\n"
	       "\n"
	       "with no FILE, or when FILE is -, read from standard input\n\n"
	       "options:\n"
	       "  -h    display this help and exit.\n"
	       "  -V    display version information and exit.\n"
	       "\n",
	       program_name());
	exit(status);
}

static int
do_transform(FILE* restrict in, FILE* restrict out)
{
	char* buf = malloc(BUFSIZ);
	if (!buf) {
		die(EXIT_FAILURE, "malloc");
	}

	do {
		size_t r = fread(buf, 1, BUFSIZ, in);
		if (r <= 0) break;

		// transform

		if (fwrite(buf, 1, r, out) < r) {
			die(EXIT_FAILURE, "write error");
		}
	} while (!feof(in) && !ferror(in));

	if (ferror(in)) {
		die(EXIT_FAILURE, "read error");
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
			die(EXIT_FAILURE, "%s", path);
		}
	}

	setvbuf(fp, NULL, _IONBF, 0);
	return fp;
}

int
main(int argc, char* const argv[argc])
{
	set_program_name("j2l");

	int opt;
	while ((opt = getopt(argc, argv, ":hV")) != -1) {
		switch (opt) {
		case 'h': usage(EXIT_SUCCESS);
		case 'V': printf("j2l v%s\n\n", J2L_VERSION); return EXIT_SUCCESS;
		case ':': die(EXIT_FAILURE, "option '%c' requires an argument", optopt);
		case '?': die(EXIT_FAILURE, "unknown option '%c'", optopt);
		default:  UNREACHABLE();
		}
	}

	FILE* in = NULL;
	if (optind == argc) {
		in = open_stream("-");
	} else {
		const char* inpath = argv[optind++];

		if (optind != argc) {
			die(EXIT_FAILURE, "extra operand '%s'", argv[optind]);
		}

		in = open_stream(inpath);
	}

	int status = do_transform(in, stdout);
	fclose(in);

	return status;
}
