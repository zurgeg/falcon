AC_INIT([falcon], [v0.1.0-boot0])
AC_PROG_CC
AC_ARG_ENABLE([boot0-security],
    AS_HELP_STRING([--enable-boot0-security], [Enable boot1 signing, forcing an "authentic" boot0]))
AS_IF([test "x$enable_boot0_security" = "xyes"], [
  AC_DEFINE([ENABLE_BOOT0_SECURITY], [1], [Enables boot1 signing. Recommended if you want an "authentic" boot0])
])
AM_INIT_AUTOMAKE(subdir-objects foreign)
AC_CONFIG_FILES([Makefile])
AC_CONFIG_HEADERS([config.h:config.h.in])
AC_OUTPUT