dnl Process this file with autoconf to produce a configure script.

AC_INIT(multicastclientserverexample, 0.1)


AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE



AC_CANONICAL_HOST
case $host_os in
     darwin*)
	HOST_IS_DARWIN="yes"
	;;
     *mingw32*)
	HOST_IS_WINDOWS="yes"
	;;
     *)
	HOST_IS_DARWIN="no"
	HOST_IS_WINDOWS="no"
	;;
esac



if test "$HOST_IS_WINDOWS" = "yes"; then
        WSOCKLIB="-lws2_32"
	AC_SUBST(WSOCKLIB)
fi


AM_CONDITIONAL(MINGW, [ test "$HOST_IS_WINDOWS" = "yes" ])
AM_CONDITIONAL(DARWIN, [ test "$HOST_IS_DARWIN" = "yes" ])

AC_ISC_POSIX
AC_HEADER_STDC
AC_PROG_CPP
AC_PROG_CC
AM_PROG_CC_STDC
AM_PROG_CC_C_O


AC_OUTPUT([
Makefile
src/Makefile
])
