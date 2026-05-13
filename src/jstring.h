/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "config.h"
#include "common.h"

#include <string.h> // IWYU pragma: export

// =============================================================================
/**
 * same as xsi `basename` provided by `libgen.h`,
 * except it supports windows paths.
 *
 * this version also returns `const char*`, as it *may* return a pointer to
 * a non-modifiable string literal.
 */
const char*
jbasename(char* path);

/**
 * same as bsd `strlcpy`, except for argument order
 */
size_t
jstrlcpy(size_t size, char dst[restrict size], const char src[restrict static 1]);

/**
 * locale-independant isspace
 */
static inline bool
is_space(char ch)
{
	// 09 0a 0b 0c 0d
	// \v \n \t \f \r
	return (ch == 0x20) || ((unsigned)(ch - 0x09) <= 4);
}

/**
 * locale-independant tolower
 */
static inline char
to_lower(char ch)
{
	return (char)((unsigned char)ch | 0x20);
}
