# SPDX-License-Identifier: GPL-3.0-or-later
# ==============================================================================
# == Makefile defines ==
#
# Define V=1 to enable verbose output
#
# == Portability/Library defines ==
#
# Define TARGET_WINDOWS is you're building for windows, and the environment is not
# detected automatically (eg: Mingw/Cygwin).
#
# Define NEEDS_MATH if your system requires linking to the math library (-lm),
#
# Define STATIC to build statically linked executables. However note, this will not
# work if any sanitizers are enabled.
#
# == Developer defines ==
#
# Define DEBUG to disable compiler optimizations and make debugging easier.
#
# Define DEV to enable the default set of compiler warnings, This option will
# also enable some options to make profiling easier (such as -fno-omit-frame-pointer).
#
# == Other defines ==
#
# Define SANITIZE to a comma-separated list of sanitizers to enable via -fsanitize.
#
# ==============================================================================
.SUFFIXES:
.DELETE_ON_ERROR:

# Disable VCS-based implicit rules from GNU make.
%: %,v
%: RCS/%
%: RCS/%,v
%: SCCS/s.%
%: s.%

# ==============================================================================
ifndef V
Q = @
QGEN = @echo '  GEN $@';
QCC = @echo '   CC $@';
QLD = @echo '   LD $@';
endif

# ==============================================================================
uname := $(shell uname | tr '[:upper:]' '[:lower:]')
comma := ,
empty :=
space := $(empty) $(empty)

PREFIX = /usr
BINDIR = $(PREFIX)/bin
MANDIR = $(PREFIX)/share/man
MAN1DIR = $(MANDIR)/man1
DOCDIR = $(PREFIX)/share/doc

PROG_CFLAGS = -std=c11 -flto=auto -g
PROG_LDFLAGS = -flto=auto
PROG_INCLUDES := -I$(realpath src/)
PROG_DEFS = \
	-D_XOPEN_SOURCE=700 \
	-D_FILE_OFFSET_BITS=64 \
	-D_TIME_BITS=64
PROG_LIBS =

ifneq ($(findstring mingw,$(uname)),)
TARGET_WINDOWS = Yes
endif
ifneq ($(findstring cygwin,$(uname)),)
TARGET_WINDOWS = Yes
endif
ifeq ($(uname),linux)
NEEDS_MATH = Yes
endif

ifdef SANITIZE
SANITIZERS := $(foreach flag,$(subst $(comma),$(space),$(SANITIZE)),$(flag))
PROG_CFLAGS += -fsanitize=$(SANITIZE) -fno-sanitize-recover=$(SANITIZE)
# let the driver decide what's needed
PROG_LDFLAGS += -fsanitize=$(SANITIZE)
NO_OFP = 1
ifneq ($(filter leak,$(SANITIZERS)),)
DEBUG = 1
endif
endif

ifdef DEBUG
PROG_CFLAGS += -O0
else
PROG_CFLAGS += -O2 -fstack-protector-strong -fstack-clash-protection
endif

ifdef DEV
PROG_CFLAGS += \
	-Wall \
	-Wextra \
	-Wpedantic \
	-Wconversion \
	-Wshadow \
	-Wnull-dereference \
	-Wdouble-promotion \
	-Wunused \
	-Wno-unused-parameter \
	-Wcast-align \
	-Winfinite-recursion \
	-Wtype-limits \
	-Wsign-compare \
	-Wsign-conversion \
	-Wformat=2 \
	-Wstrict-overflow=4
NO_OFP = 1
endif

ifneq ($(findstring clang,$(CC)),)
PROG_CFLAGS += -Wno-unused-command-line-argument
endif

ifdef NO_OFP
PROG_CFLAGS += -fno-omit-frame-pointer
endif

ifdef STATIC
PROG_LDFLAGS += -static
endif

X =
ifdef TARGET_WINDOWS
PROG_DEFS += -DPLATFORM_WINDOWS
X = .exe
endif

ifdef NEEDS_MATH
PROG_LIBS += -lm
endif

ALL_CFLAGS = $(CPPFLAGS) $(PROG_DEFS) $(PROG_INCLUDES) $(PROG_CFLAGS) $(CFLAGS)
ALL_LDFLAGS = $(PROG_LDFLAGS) $(LDFLAGS)
ALL_LIBS = $(PROG_LIBS) $(LDLIBS)

# ==============================================================================
CFG_H = src/config.h
COMPDB = compile_flags.txt

LIB_OBJS =
TEST_OBJS =

UNIT_TESTS = $(TEST_OBJS:.o=$X)

PROGRAM = j2l$X
TEST_PROGRAMS = $(UNIT_TESTS)

OBJECTS = $(LIB_OBJS) src/j2l.o $(TEST_OBJS) tests/tmain.o
DEPENDS = $(OBJECTS:.o=.d)

# ==============================================================================
# Targets
all: $(PROGRAM) $(TEST_PROGRAMS)

configure: $(CFG_H) $(COMPDB)

clean:
	$(Q)$(RM) $(PROGRAM) $(UNIT_TESTS) $(OBJECTS) $(DEPENDS)

distclean: clean
	$(Q)$(RM) $(CFG_H) $(COMPDB)

install: all
	$(Q)strip $(PROGRAM)
	$(Q)install -vDm755 -t $(DESTDIR)$(BINDIR) $(PROGRAM)

uninstall:
	$(Q)$(RM) -v $(DESTDIR)$(BINDIR)/$(PROGRAM)

check: unit-tests

unit-tests: $(UNIT_TESTS)
	$(Q)set -e; \
	for test in $(UNIT_TESTS); do \
		./$$test; \
	done

-include $(DEPENDS)

# Build rules
j2l$X: src/j2l.o $(LIB_OBJS)
	$(QLD)$(CC) $(ALL_LDFLAGS) -o $@ $^ $(ALL_LIBS)

$(UNIT_TESTS): %$X: tests/%.o tests/tmain.o $(LIB_OBJS)
	$(QLD)$(CC) $(ALL_LDFLAGS) -o $@ $^ $(ALL_LIBS)

%.o: %.c | $(CFG_H)
	$(QCC)$(CC) $(ALL_CFLAGS) -MMD -MP -c $< -o $@

$(CFG_H): $(CFG_H).in
	$(QGEN)VERSION=$$(scripts/gen-version) && \
	sed \
		-e "s|@VERSION@|$$VERSION|g" \
		$< >$@

$(COMPDB): FORCE
	$(QGEN)echo -n "$(ALL_CFLAGS)" | sed -E 's|^\s+||;s|\s+|\n|g' >$@

# ==============================================================================
.PHONY: all compdb clean distclean install uninstall check unit-tests
FORCE: ;
