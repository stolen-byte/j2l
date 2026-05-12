/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "config.h"
#include "common.h"

#include <string.h> // IWYU pragma: export

// =============================================================================
/**
 * same as bsd `strlcpy`, except for argument order
 */
size_t
jstrlcpy(size_t size, char dst[restrict size], const char src[restrict static 1]);
