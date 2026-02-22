## Waverous LambdaMOO is a fork of the LambdaMOO server version 1.8.3.

To quote the original project: "LambdaMOO is a network-accessible, multi-user, programmable, interactive system well-suited to the construction of text-based adventure games, conferencing systems, and other collaborative software."

Waverous LambdaMOO:

* has been ported from C to C++
* adds Expat for XML parsing (kudos to Ken Fox for his file)
* has conditional compilation via choices when you run ./configure (i.e. --without-fup --with-fileio --with-expat --with-forked-checkpoints)
* Has an extensively updated set of automake and autoconf files

## Directory Guides

To help new contributors orient quickly, there are focused READMEs in key
subdirectories:

* [`manual/README.md`](manual/README.md): explains what documentation sources
  are in `manual/`, why they matter, and how to generate output formats (text,
  HTML, PDF, info).
* [`server/README.md`](server/README.md): maps the server code layout by
  subsystem and explains which tools are used for parsing, code generation,
  configuration, and builds.

## Recent Updates (October 2025)

The project has been modernized and is now fully functional on modern systems:

* **Build System Fixed**: Resolved compilation issues for modern C++ compilers and ARM64 architecture
* **Dependencies Updated**: Added support for modern autotools and library versions
* **Cross-Platform**: Successfully tested on macOS ARM64 and Linux
* **Ready for Development**: Clean compilation with modern toolchain

### February 2026

* **Autotools Modernized Again**: Updated legacy `configure.ac`/`acinclude.m4` macros for current autoconf behavior and regenerated autotools outputs.
* **Compatibility Preserved**: Restored required legacy config header behavior (`int32`/`unsigned32` typedef block and guards) so existing server code still builds cleanly.
* **Build Verified with Latest Brew Tools**: Confirmed successful `autoreconf`, `configure`, and `make` using Homebrew toolchain (`autoconf 2.72`, `automake 1.18.1`, `bison 3.8.2`, `libtool 2.5.4`).
* **Pushed Upstream**: Changes are committed and pushed on `master` (`2fe1a64`).

### Build Instructions

```bash
# Install dependencies (macOS with Homebrew)
brew install autoconf automake bison libtool expat gperf

# Prefer Homebrew bison/libtool over macOS defaults
export PATH="/opt/homebrew/opt/bison/bin:/opt/homebrew/opt/libtool/libexec/gnubin:$PATH"

# Regenerate autotools files, then configure and build
cd server
autoreconf -fi
export LDFLAGS="-L/opt/homebrew/opt/expat/lib"
export CPPFLAGS="-I/opt/homebrew/opt/expat/include"
./configure --with-expat --with-fileio --with-fup \
  --with-pattern-cache-size=20 \
  --with-input-backspace=yes \
  --with-ignore-prop-protected=yes
make -j4

# Run the server
./moo databases/Minimal.db output.db
```

## Waverous changes were incorporated into Stunt.io

In 2013 I duplicated the C++ porting work I did for Waverous for [Todd Sundsted's clone of the LambdaMOO server](https://github.com/toddsundsted/stunt). At that point I stopped work on this project.

## Future Development Goals

* **LLM Integration**: Add LambdaMOO objects that can communicate with external LLM APIs (ChatGPT, Gemini, etc.)
* **Modernization**: Update deprecated C++ features and improve code quality
* **Raspberry Pi Support**: Optimize for ARM-based single-board computers
* **Docker Support**: Create containerized deployment options

## A free book on using LambdaMOO

Elizabeth Hess has written a book about using LambdaMOO, "Yib's Guide to MOOing." The complete text is available as a PDF from her site: www.yibco.com 
