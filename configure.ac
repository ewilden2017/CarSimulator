AC_INIT([CarSim], [0.0.1], [ewilden2017@gmail.com])
AC_PREREQ([2.68])

AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([foreign subdir-objects 1.11 -Wall -Werror])
AC_CONFIG_FILES([Makefile])

AC_PROG_CXX

#PKG-CONFIG
PKG_CHECK_MODULES([DEPS], [glfw3 glew])
#other stuff

AC_OUTPUT