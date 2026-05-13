/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "common.h"
#include "buffer.h"
#include "error.h"
#include "transform.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// =============================================================================
#undef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#undef min
#define min(a, b) ((a) < (b) ? (a) : (b))

/**
 * returns current time in nanoseconds
 */
static inline uint64_t
clk(void)
{
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	return ((uint64_t)t.tv_sec * (uint64_t)1e9) + (uint64_t)t.tv_nsec;
}

static noreturn void
usage(int status)
{
	printf("usage: bench [-n ITERS] [-h]\n\n");
	exit(status);
}

static inline size_t
parse_int(const char* s)
{
	char* e = NULL;
	errno = 0;

	unsigned long long ret = strtoull(s, &e, 10);
	if (errno != 0 || e == s || *e != 0) {
		die(EXIT_FAILURE, "invalid value '%s'", s);
	}

	return (size_t)ret;
}

/**
 * expects `buf->in` to be pre-filled with the source data
 */
ATTRIBUTE(noinline)
static double
do_transform(io_buffer buf[restrict static 1])
{
	transform_ctx ctx = {0};

	uint64_t start = clk();
	transform_next(&ctx, buf->size, buf->in, buf->out);
	uint64_t end = clk();

	return (double)(end - start);
}

static int
run_bench(size_t iters, FILE* restrict in, io_buffer buf[restrict static 1])
{
	size_t r = fread(buf->in, 1, buf->size, in);
	if (r != buf->size) {
		error("read error");
		return EXIT_FAILURE;
	}

	static const double mb = 1024.0 * 1024.0;
	double mb_iter = (double)buf->size / mb;
	double mb_total = (double)(buf->size * iters) / mb;
	printf("==============================\n"
	       "ITERS: %zu\n"
	       " DATA: %.2f MB (%.2f MB total)\n"
	       "---\n"
	       "running...",
	       iters,
	       mb_iter,
	       mb_total);

	fflush(stdout);

	double t_mean = 0;
	double t_low = 3.40282347e+38; // FLT_MAX, but i can't guarantee that's available
	double t_high = 0;
	double t_total = 0;
	for (size_t i = 0; i < iters; ++i) {
		double t = do_transform(buf) / 1e6; // convert down to millis
		// Welford's mean calculation algorithm
		t_mean += (t - t_mean) / (double)(i + 1);
		t_low = min(t_low, t);
		t_high = max(t_high, t);
		t_total += t;
	}

	double tsec = t_total / 1e3; // convert to secs (from millis)
	double rate = mb_total / tsec;
	printf("done\n"
	       "---\n"
	       "total: %.3f, lo: %.3f, mean: %.3f, hi: %.3f, rate: %.2f MB/s\n\n",
	       t_total,
	       t_low,
	       t_mean,
	       t_high,
	       rate);

	return 0;
}

int
main(int argc, char* const argv[argc])
{
	size_t iters = 500;
	int opt;

	set_program_name("bench");
	while ((opt = getopt(argc, argv, ":hn:")) != -1) {
		switch (opt) {
		case 'h': usage(EXIT_SUCCESS);
		case 'n': iters = parse_int(optarg); break;
		case ':': die(EXIT_FAILURE, "option '%c' requires an argument", optopt);
		case '?': die(EXIT_FAILURE, "unknown option '%c'", optopt);
		default:  UNREACHABLE();
		}
	}

	if (optind == argc) {
		die(EXIT_FAILURE, "no input file given");
	}

	FILE* in = fopen(argv[optind], "rb");
	if (!in) {
		die(EXIT_FAILURE, "%s", argv[optind]);
	}

	setvbuf(in, NULL, _IONBF, 0);

	fseek(in, 0, SEEK_END);
	long size = ftell(in);
	fseek(in, 0, SEEK_SET);

	if (size < 0) {
		fclose(in);
		die(EXIT_FAILURE, "ftell");
	}

	io_buffer buf = {0};
	int status = EXIT_FAILURE;
	if (io_buffer_init(&buf, (size_t)size)) {
		status = run_bench(iters, in, &buf);
		io_buffer_free(&buf);
	} else {
		error("io_buffer_init");
	}

	fclose(in);

	return status;
}
