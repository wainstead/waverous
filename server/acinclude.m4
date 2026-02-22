dnl Original LambdaMOO m4 macros from configure.in, updated for modern autoconf.

dnl ***************************************************************************
dnl	MOO_FUNC_DECL_CHECK(header, func,
dnl			    if-declared [, if-not-declared[, extra-hdr]])
dnl Do `if-declared' if `func' is declared in `header', and if-not-declared
dnl otherwise.  If `extra-hdr' is provided, it is added after the #include of
dnl `header'.
AC_DEFUN([MOO_FUNC_DECL_CHECK], [
changequote(,)dnl
pattern="[^_a-zA-Z0-9]$2 *("
changequote([,])dnl
AC_EGREP_CPP([$pattern], [
#include <$1>
$5
], [$3], [$4])])

dnl ***************************************************************************
dnl 	MOO_NDECL_FUNCS(header, func1 func2 ...[, extra-hdr])
dnl Defines NDECL_func1, NDECL_func2, ... if they are not declared in header.
AC_DEFUN([MOO_NDECL_FUNCS], [
changequote(,)dnl
trfrom='[a-z]' trto='[A-Z]'
changequote([,])dnl
for func in $2
do
echo "checking whether $func is declared in $1"
MOO_FUNC_DECL_CHECK($1, $func, ,
	AC_DEFINE_UNQUOTED([NDECL_]`echo $func | tr "$trfrom" "$trto"`, [1],
	[Define to 1 if $func is not declared in <$1>.]), $3)
done
])

dnl ***************************************************************************
dnl	MOO_VAR_DECL_CHECK(header, variable,
dnl			   if-declared [, if-not-declared[, extra-hdr]])
dnl Do `if-declared' if `variable' is declared in `header', and if-not-declared
dnl otherwise.  If `extra-hdr' is provided, it is added after the #include of
dnl `header'.
AC_DEFUN([MOO_VAR_DECL_CHECK], [
changequote(,)dnl
pattern="[^_a-zA-Z0-9]$2"
changequote([,])dnl
AC_EGREP_CPP([$pattern], [
#include <$1>
$5
], [$3], [$4])])

dnl ***************************************************************************
dnl 	MOO_NDECL_VARS(header, var1 var2 ...[, extra-hdr])
dnl Defines NDECL_var1, NDECL_var2, ... if they are not declared in header.
AC_DEFUN([MOO_NDECL_VARS], [
changequote(,)dnl
trfrom='[a-z]' trto='[A-Z]'
changequote([,])dnl
for var in $2
do
echo "moo_ndecl: checking whether $var is declared in $1"
MOO_VAR_DECL_CHECK($1, $var, ,
	AC_DEFINE_UNQUOTED([NDECL_]`echo $var | tr "$trfrom" "$trto"`, [1],
	[Define to 1 if $var is not declared in <$1>.]), $3)
done
])

dnl ***************************************************************************
dnl 	MOO_HEADER_STANDS_ALONE(header [, extra-code])
dnl Defines header_NEEDS_HELP if header cannot be compiled all by itself.
AC_DEFUN([MOO_HEADER_STANDS_ALONE], [
changequote(,)dnl
trfrom='[a-z]./' trto='[A-Z]__'
changequote([,])dnl
AC_MSG_CHECKING([for self-sufficiency of $1])
AC_COMPILE_IFELSE([
  AC_LANG_PROGRAM([[
#include <$1>
$2
  ]], [[]])
], [
  AC_MSG_RESULT([yes])
], [
  AC_MSG_RESULT([no])
  AC_DEFINE_UNQUOTED(`echo $1 | tr "$trfrom" "$trto"`_NEEDS_HELP, [1],
    [Define to 1 if <$1> requires additional includes.])
])
])

dnl ***************************************************************************
dnl	MOO_HAVE_FUNC_LIBS(func1 func2 ..., lib1 "lib2a lib2b" lib3 ...)
dnl For each `func' in turn, if `func' is defined using the current LIBS value,
dnl leave LIBS alone.  Otherwise, try adding each of the given libs to LIBS in
dnl turn, stopping when one of them succeeds in providing `func'.  Define
dnl HAVE_func if `func' is eventually found.
AC_DEFUN([MOO_HAVE_FUNC_LIBS], [
for func in $1
do
  changequote(,)dnl
  trfrom='[a-z]' trto='[A-Z]'
  var=HAVE_`echo $func | tr "$trfrom" "$trto"`
  changequote([,])dnl
  AC_CHECK_FUNC([$func],
    [AC_DEFINE_UNQUOTED([$var], [1], [Define to 1 if the function '$func' is available.])], [
      SAVELIBS="$LIBS"
      for lib in $2
      do
        LIBS="$LIBS $lib"
        AC_CHECK_FUNC([$func], [
          AC_DEFINE_UNQUOTED([$var], [1], [Define to 1 if the function '$func' is available.])
          break
        ], [LIBS="$SAVELIBS"])
      done
    ])
done
])

dnl ***************************************************************************
dnl	MOO_HAVE_HEADER_DIRS(header1 header2 ..., dir1 dir2 ...)
dnl For each `header' in turn, if `header' is found using the current CPPFLAGS
dnl value leave CPPFLAGS alone.  Otherwise, try adding each of the given
dnl `dir's to CPPFLAGS in turn, stopping when one of them succeeds in
dnl providing `header'.  Define HAVE_header if `header' is eventually found.
AC_DEFUN([MOO_HAVE_HEADER_DIRS], [
for hdr in $1
do
  changequote(,)dnl
  trfrom='[a-z]./' trto='[A-Z]__'
  var=HAVE_`echo $hdr | tr "$trfrom" "$trto"`
  changequote([,])dnl
  AC_CHECK_HEADER([$hdr], [
    AC_DEFINE_UNQUOTED([$var], [1], [Define to 1 if <$hdr> is available.])
  ], [
    SAVECPPFLAGS="$CPPFLAGS"
    for dir in $2
    do
      CPPFLAGS="$CPPFLAGS $dir"
      AC_CHECK_HEADER([$hdr], [
        AC_DEFINE_UNQUOTED([$var], [1], [Define to 1 if <$hdr> is available.])
        break
      ], [CPPFLAGS="$SAVECPPFLAGS"])
    done
  ])
done
])
