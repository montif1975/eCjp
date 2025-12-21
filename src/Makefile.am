AM_CPPFLAGS = -I$(top_srcdir)/include

# ---- Libreria condivisa ----
lib_LTLIBRARIES = libecjp.la
libecjp_la_SOURCES = ecjp.c
libecjp_la_LDFLAGS = -version-info 0:0:0 -no-undefined

# ---- Eseguibili ----
bin_PROGRAMS = example_ecjp_1 \
	           example_ecjp_2 \
               test_lib_check_and_load \
               test_lib_check_syntax \
               test_lib_load \
               test_lib_version

example_ecjp_1_SOURCES = example_ecjp_1.c
example_ecjp_1_LDADD = libecjp.la

example_ecjp_2_SOURCES = example_ecjp_2.c
example_ecjp_2_LDADD = libecjp.la

test_lib_check_and_load_SOURCES = test_lib_check_and_load.c
test_lib_check_and_load_LDADD = libecjp.la

test_lib_check_syntax_SOURCES = test_lib_check_syntax.c
test_lib_check_syntax_LDADD = libecjp.la

test_lib_load_SOURCES = test_lib_load.c
test_lib_load_LDADD = libecjp.la

test_lib_version_SOURCES = test_lib_version.c
test_lib_version_LDADD = libecjp.la

