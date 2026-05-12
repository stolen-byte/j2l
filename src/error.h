/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "config.h"
#include "common.h"

// =============================================================================
ATTRIBUTE(format(printf, 1, 2))
void
error(const char fmt[restrict static 1], ...);

ATTRIBUTE(format(printf, 2, 3))
noreturn void
die(int status, const char fmt[restrict static 1], ...);
