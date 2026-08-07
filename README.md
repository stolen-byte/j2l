[![Build status](https://github.com/stolen-byte/j2l/workflows/ci/badge.svg)](https://github.com/stolen-byte/j2l/actions)
[![Latest](https://img.shields.io/github/v/release/stolen-byte/j2l)](https://github.com/stolen-byte/j2l/releases/latest)
![Windows](https://img.shields.io/badge/platform-windows-blue)
![Linux](https://img.shields.io/badge/platform-linux-blue)
![macOS](https://img.shields.io/badge/platform-macos-blue)

# j2l

**j2l** converts JSON data from a file, or standard input, to minified JSONL, and
writes the result to standard output.

The conversion process effectively removes all non-string whitespace, and
collapses top-level arrays into a stream of their contents.

Multiple JSON files can be processed, and concatenated by piping
them sequentially from standard input.

> [!NOTE]
> No validation is performed on the actual json structure, as that is
> outside the scope of this program, therefore: if you pass malformed data in,
> expect to get malformed data back.

---

## Quick Links

- [Why](#why)
- [Installation/Building](#installation--building)
- [Usage](#usage)
- [License](#license)

---

## Why

**j2l** was written to stream/pipe huge (~10GB+) database dumps, consisting of a single (root-level) json array of large objects, to external programs, using as small amount of memory as possible.

Most programs i could find which operate on json [^1], either:

- didn't support streaming, and require the entire data to be loaded in memory, or,
- supported streaming, but if the root 'object' was an array, then they still required the entire data to be
  processed/loaded.

As you can imagine, with large enough datasets, memory becomes an issue.

[^1]: _at the time of writing, the situation could very well have changed in the meantime._

## Installation / Building

Pre-built binaries are available for Linux, macOS, and Windows from the [releases](https://github.com/stolen-byte/j2l/releases) page.

All binaries are statically linked, and require no dependencies.

> [!NOTE]
> Due to GNU's libc being known for producing highly bloated binaries, linux releases are built
> using musl, resulting in _much_ smaller binaries.
> (and as an added bonus, quite a bit faster too, according to benchmarks).

For build instructions, see [here](docs/building.md)

## Usage

a brief usage overview:

```
j2l [OPTIONS...] [FILE]

with no FILE, or when FILE is -, read from standard input.

options:
  -b SIZE    buffer size to use for internal read/write buffers.
  -h         display this help and exit.
  -V         display version information and exit.
```

For detailed usage and options, see [usage](docs/includes/usage.adoc), and [examples](docs/includes/examples.adoc).

## Benchmark

As an example (since nothing to compare against), the output of the included benchmark, on my machine:

```
$ make run-bench
==============================
ITERS: 500
 DATA: 8.99 MB (4497.28 MB total)
---
running...done
---
total: 6479.829, lo: 12.700, mean: 12.960, hi: 18.825, rate: 694.04 MB/s

```

where `total`/`lo`/`mean`/`hi` are in milliseconds.

(this is with a RYZEN 5 2600X (6 Core) 3.6 Ghz)

---

## License

```
j2l is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

j2l is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
```

To see the full license text, see [LICENSE](LICENSE).
