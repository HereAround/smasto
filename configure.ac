## Process this file to produce a configure script:
##   aclocal -I build-aux/m4 && autoheader && autoconf && automake
##

AC_PREREQ([2.63])
AC_INIT([smasto],[0.10.12],[riccardo.murri@gmail.com])
AC_CONFIG_SRCDIR([sms-norm.cpp])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([build-aux/m4])
AM_INIT_AUTOMAKE([1.10 no-define])
AC_CONFIG_HEADERS([config.h])


# Checks for programs.
AC_PROG_CXX
AC_PROG_INSTALL

# sources are C++ only
AC_LANG_PUSH([C++])


# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_LONG_LONG_INT
AC_TYPE_LONG_DOUBLE

AC_ARG_WITH([openmp],
    [AS_HELP_STRING([--with-openmp=yes|no], [Use OpenMP if available.])],
    [ # option '--with-openmp' given
      if test "_$withval" != _no ; then :
         AC_OPENMP
      fi
    ])


# Checks for library functions.
AC_CHECK_FUNCS([sqrt strdup strerror])


# Final output
AC_LANG_POP([C++])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
