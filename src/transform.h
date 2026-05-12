/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "config.h"
#include "common.h"

// =============================================================================
typedef struct transform_ctx {
	unsigned level;
	bool escaped : 1;
	bool quoted  : 1;
} transform_ctx;

// =============================================================================
size_t
transform_next(transform_ctx ctx[restrict static 1],
               size_t size,
               const char in[restrict size],
               char out[restrict size * 2]);
