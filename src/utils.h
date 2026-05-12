/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "config.h"
#include "common.h"

// =============================================================================
/**
 * parse a size-suffixed string such as '32kb'
 *
 * return value:
 * 	EILSEQ: invalid suffix
 * 	ERANGE: value outside of range 0<>SIZE_MAX
 * 	EINVAL: str was empty/not a number
 */
size_t
parse_size(const char str[static 1]);
