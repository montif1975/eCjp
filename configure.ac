AC_PREREQ([2.69])
AC_INIT([ecjp], [1.0], [montif1975@gmail.com])

AM_INIT_AUTOMAKE([foreign subdir-objects -Wall])
AC_CONFIG_SRCDIR([src/ecjp.c])
AC_CONFIG_HEADERS([include/config.h])

# ---- Compiler ----
AC_PROG_CC
AM_PROG_AR

# ---- Libtool ----
LT_INIT

# ---- Opzioni configure ----
AC_ARG_ENABLE([debug],
    [AS_HELP_STRING([--enable-debug], [Enable DEBUG macro])],
    [debug_flag=yes], [debug_flag=no])

AC_ARG_ENABLE([debug-verbose],
    [AS_HELP_STRING([--enable-debug-verbose], [Enable DEBUG_VERBOSE macro])],
    [debug_verbose_flag=yes], [debug_verbose_flag=no])

AC_ARG_ENABLE([token-list],
    [AS_HELP_STRING([--enable-token-list], [Enable ECJP_TOKEN_LIST macro])],
    [token_list_flag=yes], [token_list_flag=no])

AC_ARG_ENABLE([run-on-pc],
    [AS_HELP_STRING([--enable-run-on-pc], [Enable ECJP_RUN_ON_PC macro])],
    [run_on_pc_flag=yes], [run_on_pc_flag=no])

AC_ARG_ENABLE([run-on-mcu],
    [AS_HELP_STRING([--enable-run-on-mcu], [Enable ECJP_RUN_ON_MCU macro])],
    [run_on_mcu_flag=yes], [run_on_mcu_flag=no])
    
# ---- Definizioni delle macro ----
if test "x$debug_flag" = "xyes"; then
    AC_DEFINE([DEBUG], [1], [Enable debug code])
fi

if test "x$debug_verbose_flag" = "xyes"; then
    AC_DEFINE([DEBUG_VERBOSE], [1], [Enable verbose debug output])
fi

if test "x$token_list_flag" = "xyes"; then
    AC_DEFINE([ECJP_TOKEN_LIST], [1], [Enable ECJP_TOKEN_LIST])
fi

if test "x$run_on_pc_flag" = "xyes"; then
    AC_DEFINE([ECJP_RUN_ON_PC], [1], [Enable ECJP_RUN_ON_PC])
fi

if test "x$run_on_mcu_flag" = "xyes"; then
    AC_DEFINE([ECJP_RUN_ON_MCU], [1], [Enable ECJP_RUN_ON_MCU])
fi

# ---- Makefile ----
AC_CONFIG_FILES([
    Makefile
    src/Makefile
])

AC_OUTPUT

