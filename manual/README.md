# Waverous Manual Directory

This directory contains the Texinfo sources for the LambdaCore manuals and the
build assets used to publish them in multiple formats.

## What Is Here

- `ProgrammersManual.texinfo`: Main entrypoint used by `manual/Makefile` to
  build distributable docs.
- `LambdaCoreUserMan.texinfo`: User-facing LambdaCore documentation.
- `LambdaCoreProgMan.texinfo`: Programmer-facing LambdaCore documentation.
- `common.css`: Stylesheet copied into generated HTML output.
- `t2hinit.pl`: `texi2html` initialization script used during HTML generation.
- `Makefile`: Build targets for `info`, `txt`, `html`, `dvi`, `ps`, and `pdf`.

## Why This Is Useful

- Keeps canonical documentation sources in version control.
- Lets maintainers generate docs in formats suitable for:
  - in-terminal reading (`.info`, `.txt`)
  - web browsing (split HTML)
  - printable artifacts (`.pdf`, `.ps`, `.dvi`)
- Preserves historical LambdaCore documentation that explains how to use and
  extend a MOO world, not just how to compile the server.

## How To Use It

From the repository root:

```bash
cd manual
make info      # build GNU info output
make txt       # build plain text output
make html      # build split HTML under manual/html/
make pdf       # build PDF
make clean     # remove temporary build files
```

The default target (`make`) attempts to build all configured output formats.

To build everything in one pass:

```bash
cd manual
make
```

## Tooling Required

- Texinfo tools (`makeinfo`, `texi2pdf`, `texindex`)
- `texi2html` for HTML output
- TeX tools (`tex`, `dvips`) for DVI/PS/PDF paths
- Standard UNIX utilities (`sed`, `cp`, `gzip`)

On macOS with Homebrew, this setup works:

```bash
brew install texinfo texi2html texlive
```

This provides all commands needed by `manual/Makefile`, including PDF
generation.

## Build Notes

- `make txt` can emit benign warnings like `document without nodes` due to the
  intermediate `ProgrammersManual.tex-no-info` transformation.
- `make html` may warn about legacy Texinfo syntax in the historical source
  files; it still produces usable HTML output.
- If you only need text docs, `make info` and `make txt` are the fastest
  targets.
