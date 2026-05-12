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
