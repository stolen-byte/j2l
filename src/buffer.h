/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "config.h"
#include "common.h"

// =============================================================================
typedef struct io_buffer {
	size_t size;
	char* in;
	char* out;
} io_buffer;

// =============================================================================
void
io_buffer_init(io_buffer buf[restrict static 1], size_t size);

void
io_buffer_free(io_buffer buf[restrict static 1]);
