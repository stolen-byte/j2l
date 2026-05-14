# Building

Building from source requires just a c11 compiler, and is pretty standard:

    $ make all man docs # as yourself
    $ make PREFIX=/usr install install-man install-docs # as root

The beginning of the makefile documents many variables that effect the way
j2l is built, you can override them from the command line (as with `PREFIX` above).

On windows, you will need a working installation of [MSYS2](https://www.msys2.org/), and your preferred
compiler toolchain.

For gcc, that would be `mingw-w64-ucrt-x86_64-gcc`, and for clang - `mingw-w64-ucrt-x86_64-clang`.
(note: ucrt64 variants are generally preferred)

both of these can be installed easily via `pacboy`:

    $ pacman -S base-devel pactoys # to install pacboy itself, and other necessary tools
    $ pacboy -S gcc:u # to install gcc
    $ pacboy -S clang:u # to install clang

To build the documentation/manpages, a working installation of [Ruby](https://www.ruby-lang.org/en/) >= 3.1
is required, with the following gems:

- [asciidoctor](https://github.com/asciidoctor/asciidoctor)
- [asciidoctor-pdf](https://github.com/asciidoctor/asciidoctor-pdf) (for the pdf docs only)
- [rouge](https://github.com/rouge-ruby/rouge) (syntax highlighting)

a [Gemfile](../Gemfile) is provided, so to install all the dependencies at once, simply run:

    $ bundle install

from the project root.

There are 4 documentation targets, each built with:

    $ make man # for the manpages
    $ make pdf # for the pdf documentation
    $ make html # for the html5 documentation
    $ make docs # both pdf & html documentation

Note that, due to them having extra dependencies (+ ruby), these are not included in the
default build target.

For hacking on the code itself, there exists the `configure` target:

    $ make configure

which will generate `src/config.h`, and write all the necessary used build flags to `compile_flags.txt` in the
project root, this file is required for clang tools to work properly (clang-format, clang-tidy, and clangd).

There are also 2 useful scripts in the `scripts/` directory, namely `check`, and `git-config`.
The former will run `clang-format`/`clang-tidy` against all tracked code files, and the latter will
setup the local `.git-blame-ignore-revs` file, and a pre-commit hook pointing to the afore-mentioned `check`
script.

If you are planning on contributing changes to the repo, i would implore you to use both of these,
as CI will outright reject any PR's where the `check` script fails, so better to catch them locally at
commit-time, rather than later.

## Tests

All tests are included in the default (all) make target, and can be run with:

    $ make check # to run all tests
    $ make unit-tests # to run only the core unit tests
    $ make json-tests # to run only the data-driven transformation tests

## Benchmarks

A small/simple benchmark utility is included, which can be built/ran using:

    $ make bench # to build only
    $ make run-bench # to build + run

> [!NOTE]
> The benchmark expects a JSON file to work with, once of which is provided in the `benches/` directory.
> also note: this sample file is read into memory in its entirety, so be careful with the filesize if you
> use a custom one.
