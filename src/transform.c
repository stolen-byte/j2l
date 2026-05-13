/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "transform.h"
#include "jstring.h"

// =============================================================================
static inline bool
is_array_boundary(char ch)
{
	return (ch == '[' || ch == ']' || ch == ',');
}

size_t
transform_next(transform_ctx ctx[restrict static 1],
               size_t size,
               const char in[restrict size],
               char out[restrict size * 2])
{
	const char* end = in + size;
	char* oout = out;

	while (in != end) {
		char ch = *in++;

		if (ctx->quoted) {
			if (ctx->escaped) {
				ctx->escaped = false;
			} else if (ch == '\\') {
				ctx->escaped = true;
			} else if (ch == '"') {
				ctx->quoted = false;
			}
			*out++ = ch;
			continue;
		}

		if (is_space(ch) || (ctx->level == 0 && is_array_boundary(ch))) //
			continue;

		*out++ = ch;
		if (ch == '"') {
			ctx->quoted = true;
		} else if (ch == '{') {
			ctx->level++;
		} else if (ch == '}') {
			if (ctx->level > 0) ctx->level--;
			if (ctx->level == 0) *out++ = '\n';
		}
	}

	return (size_t)(out - oout);
}
