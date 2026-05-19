/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "config.h"
#include "common.h"
#include "buffer.h"
#include "error.h"
#include "jstring.h"
#include "transform.h"
#include "utils.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef PLATFORM_WINDOWS
	#include <fcntl.h> // _O_BINARY
	#include <io.h>    // _setmode, _fileno
#endif

// ==============================================================================
static noreturn void
usage(int status)
{
	printf("usage: %s [OPTION...] [FILE]\n"
	       "transform json from FILE, to jsonl, and write to standard output.\n"
	       "\n"
	       "with no FILE, or when FILE is -, read from standard input\n\n"
	       "options:\n"
	       "  -b SIZE    buffer size to use for internal read/write buffers.\n"
	       "             accepts b,k[b],m[b],g[b] size suffixes. (default: 32k).\n"
	       "  -h         display this help and exit.\n"
	       "  -V         display version information and exit.\n"
	       "\n",
	       program_name());
	exit(status);
}

static int
do_transform(FILE* restrict in, FILE* restrict out, io_buffer io[restrict static 1])
{
	transform_ctx ctx = {0};

	do {
		size_t read = fread(io->in, 1, io->size, in);
		if (read <= 0) break;

		size_t done = transform_next(&ctx, read, io->in, io->out);
		if (done > 0) {
			if (fwrite(io->out, 1, done, out) < done) {
				error("write error");
				return EXIT_FAILURE;
			}
		}
	} while (!feof(in) && !ferror(in));

	if (ferror(in)) {
		error("read error");
		return EXIT_FAILURE;
	}

	return 0;
}

static FILE*
open_stream(const char* path)
{
	FILE* fp = NULL;

	if (strcmp(path, "-") == 0) {
#ifdef PLATFORM_WINDOWS
		_setmode(_fileno(stdin), _O_BINARY);
#endif
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
	set_program_name(jbasename(argv[0]));

	int opt;
	io_buffer buf = {.size = 32768};
	while ((opt = getopt(argc, argv, ":hVb:")) != -1) {
		switch (opt) {
		case 'h': usage(EXIT_SUCCESS);
		case 'V': printf("j2l v%s\n\n", J2L_VERSION); return EXIT_SUCCESS;
		case 'b':
			buf.size = parse_size(optarg);
			if (errno != 0) {
				// check for this explicitly, as the 'Invalid or incomplete multibyte or wide character' error string
				// is not very meaningful in this context
				if (errno == EILSEQ) {
					errno = 0;
					die(EXIT_FAILURE, "invalid size suffix '%s'", optarg);
				}
				die(EXIT_FAILURE, "invalid size '%s'", optarg);
			}
			if (buf.size == 0) die(EXIT_FAILURE, "size cannot be zero");
			break;
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

#ifdef PLATFORM_WINDOWS
	_setmode(_fileno(stdout), _O_BINARY);
#endif

	int status = EXIT_FAILURE;
	if (io_buffer_init(&buf, buf.size)) {
		status = do_transform(in, stdout, &buf);
		io_buffer_free(&buf);
	} else {
		error("io_buffer_init");
	}

	fclose(in);

	return status;
}
