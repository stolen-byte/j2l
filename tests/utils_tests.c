/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "testing.h"
#include "utils.h"

#include <errno.h>

// =============================================================================
#define assert_size(size, want)                                             \
	do {                                                                      \
		size_t a = parse_size(size);                                            \
		size_t b = (size_t)(want);                                              \
		require(errno == 0);                                                    \
		if (a != b) {                                                           \
			fail_fmt("parsing '%s' failed:\n got: %zu\nwant: %zu\n", size, a, b); \
		}                                                                       \
	} while (0)

#define assert_size_errno(size, errnum)             \
	do {                                              \
		require(parse_size(size) == 0);                 \
		if (errno != (errnum)) {                        \
			fail_fmt("parsing '%s':\n"                    \
			         "expected errno: " #errnum " (%d)\n" \
			         "got: %d\n",                         \
			         size,                                \
			         errnum,                              \
			         errno);                              \
		}                                               \
	} while (0)

// =============================================================================
TEST(parse_size)
{
	// invalid
	assert_size_errno("", EINVAL);
	assert_size_errno("XYZ", EINVAL);
	assert_size_errno(".g", EINVAL);
	assert_size_errno("-1", ERANGE);
	assert_size_errno("-1gb", ERANGE);
	assert_size_errno("18446744073709551616", ERANGE);
	assert_size_errno("18446744073709551615gb", ERANGE);
	assert_size_errno("5zb", EILSEQ);
	assert_size_errno("5z", EILSEQ);
	assert_size_errno("5m t", EILSEQ);
	assert_size_errno("5m bt", EILSEQ);
	assert_size_errno("5m b t", EILSEQ);

	// no suffix
	assert_size("100", 100);
	assert_size("1024", 1024);
	assert_size("102400", 102400);

	// 'b' suffix
	assert_size("100 b", 100);
	assert_size("100b", 100);
	assert_size("100B", 100);
	assert_size("1024b", 1024);
	assert_size("102400b", 102400);

	// kb
	assert_size("1 kb", 1024);
	assert_size("1kb", 1024);
	assert_size("1KB", 1024); //
	assert_size("1Kb", 1024); // case-sensitivity variants
	assert_size("1kB", 1024); //

	assert_size("100kb", 102400);
	assert_size("1024kb", 1048576);
	assert_size("102400kb", 104857600);

	assert_size("1k", 1024);  // 'b' should be optional
	assert_size("1k ", 1024); // trailing space

	assert_size("1.25kb", 1280);
	assert_size("1.5kb", 1536);
	assert_size("1.75kb", 1792);

	// mb
	assert_size("1mb", 1048576);
	assert_size("1024mb", 1073741824);
	assert_size("1m", 1048576);
	assert_size("1.25mb", 1310720);
	assert_size("1.5mb", 1572864);
	assert_size("1.75mb", 1835008);

	// gb
	assert_size("1gb", 1073741824);
	assert_size("1024gb", 1099511627776);
	assert_size("1g", 1073741824);
	assert_size("1.25gb", 1342177280);
	assert_size("1.5gb", 1610612736);
	assert_size("1.75gb", 1879048192);
}

// =============================================================================
DECLARE_TESTS("utils_tests", parse_size)
