/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "buffer.h"
#include "error.h"

#include <stdlib.h>

// =============================================================================
void
io_buffer_init(io_buffer buf[restrict static 1], size_t size)
{
	buf->in = malloc(size * 3);
	if (UNLIKELY(!buf->in)) {
		die(EXIT_FAILURE, "malloc");
	}
	buf->out = buf->in + size;
	buf->size = size;
}

void
io_buffer_free(io_buffer buf[restrict static 1])
{
	free(buf->in);
	buf->in = buf->out = NULL;
}
