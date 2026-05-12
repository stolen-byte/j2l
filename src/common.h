/* SPDX-License-Identifier: GPL-3.0-or-later */
#pragma once
#include "config.h"

// IWYU pragma: begin_exports
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <sys/types.h>
// IWYU pragma: end_exports

// =============================================================================
#define ARRAYSIZE(x) (sizeof(x) / sizeof(*(x)))

// =============================================================================
void
set_program_name(const char name[restrict static 1]);

const char*
program_name(void);
