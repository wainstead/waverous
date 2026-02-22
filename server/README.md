# Waverous Server Directory

This directory contains the LambdaMOO/Waverous server source, build system,
sample databases, optional extensions, and operational helper tools.

For historical startup/operations notes, also see `server/README` and
`server/README.Minimal`.

## High-Level Layout

- Core runtime and VM:
  - `server.c`, `network.c`, `execute.c`, `eval_vm.c`, `tasks.c`
- Parser and language frontend:
  - `parser.y`, generated parser artifacts, `ast.*`, `code_gen.*`
- Database and object model:
  - `db_*.c`, `objects.c`, `verbs.c`, `property.c`, `program.c`
- Built-in functions and libraries:
  - `functions.c`, `numbers.c`, `list.c`, `match.c`, `streams.c`
- Networking backends and multiplexing:
  - `net_*.c`, `net_mplex.*`, `net_proto.*`, `name_lookup.*`
- Portability wrappers and config-dependent headers:
  - `my-*.h`, `config.h`, `options.h`, `structures.h`
- Optional extensions:
  - `ext-xml.c` (Expat XML support)
  - `ext-FUP.c` and `ext-FUP-1.8/` (FUP package)
  - `extension-fileio.c` and `fileio-1.5p1/` (File I/O package)
- Data and operational assets:
  - `databases/` sample DB files (`Minimal.db`, LambdaCore/JHCore archives)
  - `tools/` helper scripts and utilities
  - `mooctl` server management wrapper

## Build System And Tool Roles

- `configure.ac`, `acinclude.m4`: Autoconf inputs (feature checks, options,
  portability behavior).
- `configure`: Generated configure script (run this before `make`).
- `Makefile.in` -> `Makefile`: Build recipes and dependency wiring.
- `bison`/`yacc`: Generates parser sources from `parser.y`.
- `gperf`: Generates keyword tables from `keywords.gperf`.
- `g++`/`clang++`: Compiles and links the server binary (`moo`).
- `autoreconf -fi`: Regenerates autotools outputs after changing autotools
  sources.

## Typical Development Flow

```bash
cd server
autoreconf -fi
./configure --with-expat --with-fileio --with-fup \
  --with-pattern-cache-size=20 \
  --with-input-backspace=yes \
  --with-ignore-prop-protected=yes
make -j4
./moo databases/Minimal.db output.db
```

On macOS, prefer Homebrew `bison` and GNU `libtool` over system defaults:

```bash
export PATH="/opt/homebrew/opt/bison/bin:/opt/homebrew/opt/libtool/libexec/gnubin:$PATH"
```

## Useful Existing Docs In This Directory

- `README`: Historical upstream getting-started and operational behavior.
- `README.Minimal`: How to bootstrap from the minimal database.
- `README.rX`: Release notes/history context.
- `ChangeLog.txt`: Codebase history and upgrade notes.
- `AddingNewMOOTypes.txt`, `MOOCodeSequences.txt`: Internal implementation
  notes for advanced contributors.
