## Process this file to produce a configure script:
##   aclocal -I build-aux/m4 && autoheader && autoconf && automake
##

AC_PREREQ([2.63])
AC_INIT([smasto], [0.10.12], [riccardo.murri@gmail.com], [], 
                  [http://smasto.googlecode.com/])
AC_CONFIG_SRCDIR([sms-norm.cpp])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([build-aux/m4])
AM_INIT_AUTOMAKE([1.10 no-define])
AC_CONFIG_HEADERS([config.h])


# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL

# sources are C++ only
AC_LANG_PUSH([C++])


# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_LONG_LONG_INT
AC_TYPE_LONG_DOUBLE

dnl One day, `sms-reord` will use OpenMP for speedup...
dnl AC_ARG_WITH([openmp],
dnl     [AS_HELP_STRING([--with-openmp=yes|no], [Use OpenMP if available.])],
dnl     [ # option '--with-openmp' given
dnl       if test "_$withval" != _no ; then :
dnl          AC_OPENMP
dnl       fi
dnl     ])


# Checks for library functions.
AC_CHECK_FUNCS([sqrt strdup strerror])


# check for needed Boost libraries 
BOOST_REQUIRE([1.15]) # Boost.Random introduced in 1.15.0
BOOST_RANDOM


# Final output
AC_LANG_POP([C++])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
