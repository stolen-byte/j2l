/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "jstring.h"
#include "testing.h"

// =============================================================================
TEST(jstrlcpy)
{
	const char* reg = "testing";
	const size_t rlen = strlen(reg);

	char buf[rlen + 1];
	memset(buf, 'X', sizeof(buf));

	// should return length of string minus null terminator
	size_t copied = jstrlcpy(sizeof(buf), buf, reg);
	require_streq(buf, reg);
	require(copied == rlen);

	if (buf[rlen] != '\0') {
		fail("copied string was not null terminated");
	}

	// source smaller than dest
	const char* small = "abc";
	const size_t slen = strlen(small);
	copied = jstrlcpy(sizeof(buf), buf, small);
	require_streq(buf, small);
	require(copied == slen);
	if (buf[slen] != '\0') {
		fail("copied string was not null terminated");
	}

	const char* big = "i am a test string";
	const size_t blen = strlen(big);
	copied = jstrlcpy(sizeof(buf), buf, big);
	require_streq(buf, "i am a ");

	if (copied != blen) {
		fail("did not return the length of src");
	}

	if (buf[rlen] != '\0') {
		fail("copied string was not null terminated");
	}
}

TEST(jbasename)
{
#define dotest(path, want)               \
	do {                                   \
		char buf[] = path;                   \
		require_streq(jbasename(buf), want); \
	} while (0)

	dotest("", ".");
	dotest("/", "/");
	dotest("//", "/");
	dotest("///", "/");

	dotest("usr", "usr");
	dotest("/usr", "usr");

	dotest("/usr/lib", "lib");
	dotest("/home//dwc//test", "test");
	dotest("/home/.././test", "test");
	dotest("/home/dwc/.", ".");
	dotest("/home/dwc/..", "..");

#ifdef PLATFORM_WINDOWS
	dotest("c:", "c:");
	dotest("c:\\", "c:");
	dotest("c:\\users", "users");
	dotest("c:\\users\\dwc\\test.exe", "test.exe");
	dotest("c:\\users\\dwc\\.", ".");
	dotest("c:\\users\\dwc\\..", "..");
#endif
#undef dotest
}

TEST(is_space)
{
	const char* ws = " \v\n\t\f\r";
	const size_t n = strlen(ws);

	for (char ch = 33; ch < 127; ++ch) {
		if (is_space(ch)) {
			fail_fmt("is_space(0x%02x) failed, should return false ('%c')", ch, SAFECHAR(ch));
		}
	}

	for (size_t i = 0; i < n; ++i) {
		char ch = ws[i];
		if (!is_space(ch)) {
			fail_fmt("is_space(0x%02x) failed, should return true", ch);
		}
	}
}

TEST(to_lower)
{
	const char* hi = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	const char* lo = "abcdefghijklmnopqrstuvwxyz";
	const size_t len = strlen(hi);

	for (size_t i = 0; i < len; ++i) {
		char ch = to_lower(hi[i]);
		if (ch != lo[i]) {
			fail_fmt("to_lower('%c'): '%c' != '%c'", hi[i], ch, lo[i]);
		}
	}
}

// =============================================================================
DECLARE_TESTS("string_tests", jstrlcpy, jbasename, is_space, to_lower)
