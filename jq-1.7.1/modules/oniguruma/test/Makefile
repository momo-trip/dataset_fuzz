# Makefile.in generated by automake 1.16.5 from Makefile.am.
# test/Makefile.  Generated from Makefile.in by configure.

# Copyright (C) 1994-2021 Free Software Foundation, Inc.

# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.



am__is_gnu_make = { \
  if test -z '$(MAKELEVEL)'; then \
    false; \
  elif test -n '$(MAKE_HOST)'; then \
    true; \
  elif test -n '$(MAKE_VERSION)' && test -n '$(CURDIR)'; then \
    true; \
  else \
    false; \
  fi; \
}
am__make_running_with_option = \
  case $${target_option-} in \
      ?) ;; \
      *) echo "am__make_running_with_option: internal error: invalid" \
              "target option '$${target_option-}' specified" >&2; \
         exit 1;; \
  esac; \
  has_opt=no; \
  sane_makeflags=$$MAKEFLAGS; \
  if $(am__is_gnu_make); then \
    sane_makeflags=$$MFLAGS; \
  else \
    case $$MAKEFLAGS in \
      *\\[\ \	]*) \
        bs=\\; \
        sane_makeflags=`printf '%s\n' "$$MAKEFLAGS" \
          | sed "s/$$bs$$bs[$$bs $$bs	]*//g"`;; \
    esac; \
  fi; \
  skip_next=no; \
  strip_trailopt () \
  { \
    flg=`printf '%s\n' "$$flg" | sed "s/$$1.*$$//"`; \
  }; \
  for flg in $$sane_makeflags; do \
    test $$skip_next = yes && { skip_next=no; continue; }; \
    case $$flg in \
      *=*|--*) continue;; \
        -*I) strip_trailopt 'I'; skip_next=yes;; \
      -*I?*) strip_trailopt 'I';; \
        -*O) strip_trailopt 'O'; skip_next=yes;; \
      -*O?*) strip_trailopt 'O';; \
        -*l) strip_trailopt 'l'; skip_next=yes;; \
      -*l?*) strip_trailopt 'l';; \
      -[dEDm]) skip_next=yes;; \
      -[JT]) skip_next=yes;; \
    esac; \
    case $$flg in \
      *$$target_option*) has_opt=yes; break;; \
    esac; \
  done; \
  test $$has_opt = yes
am__make_dryrun = (target_option=n; $(am__make_running_with_option))
am__make_keepgoing = (target_option=k; $(am__make_running_with_option))
pkgdatadir = $(datadir)/onig
pkgincludedir = $(includedir)/onig
pkglibdir = $(libdir)/onig
pkglibexecdir = $(libexecdir)/onig
am__cd = CDPATH="$${ZSH_VERSION+.}$(PATH_SEPARATOR)" && cd
install_sh_DATA = $(install_sh) -c -m 644
install_sh_PROGRAM = $(install_sh) -c
install_sh_SCRIPT = $(install_sh) -c
INSTALL_HEADER = $(INSTALL_DATA)
transform = $(program_transform_name)
NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
build_triplet = x86_64-pc-linux-gnu
host_triplet = x86_64-pc-linux-gnu
TESTS = test_utf8$(EXEEXT) \
	test_syntax$(EXEEXT) \
	test_options$(EXEEXT) testc$(EXEEXT) \
	testcu$(EXEEXT) test_regset$(EXEEXT) \
	test_back$(EXEEXT)
#TESTS = test_utf8$(EXEEXT) test_syntax$(EXEEXT) \
#	test_options$(EXEEXT) testc$(EXEEXT) \
#	testp$(EXEEXT) testcu$(EXEEXT) \
#	test_regset$(EXEEXT) test_back$(EXEEXT)
check_PROGRAMS = $(am__EXEEXT_1)
subdir = test
ACLOCAL_M4 = $(top_srcdir)/aclocal.m4
am__aclocal_m4_deps = $(top_srcdir)/m4/libtool.m4 \
	$(top_srcdir)/m4/ltoptions.m4 $(top_srcdir)/m4/ltsugar.m4 \
	$(top_srcdir)/m4/ltversion.m4 $(top_srcdir)/m4/lt~obsolete.m4 \
	$(top_srcdir)/configure.ac
am__configure_deps = $(am__aclocal_m4_deps) $(CONFIGURE_DEPENDENCIES) \
	$(ACLOCAL_M4)
DIST_COMMON = $(srcdir)/Makefile.am $(am__DIST_COMMON)
mkinstalldirs = $(install_sh) -d
CONFIG_HEADER = $(top_builddir)/src/config.h
CONFIG_CLEAN_FILES =
CONFIG_CLEAN_VPATH_FILES =
am__EXEEXT_1 = test_utf8$(EXEEXT) \
	test_syntax$(EXEEXT) \
	test_options$(EXEEXT) testc$(EXEEXT) \
	testcu$(EXEEXT) test_regset$(EXEEXT) \
	test_back$(EXEEXT)
#am__EXEEXT_1 = test_utf8$(EXEEXT) \
#	test_syntax$(EXEEXT) \
#	test_options$(EXEEXT) testc$(EXEEXT) \
#	testp$(EXEEXT) testcu$(EXEEXT) \
#	test_regset$(EXEEXT) test_back$(EXEEXT)
am_test_back_OBJECTS = test_back.$(OBJEXT)
test_back_OBJECTS = $(am_test_back_OBJECTS)
test_back_DEPENDENCIES = $(lib_onig)
AM_V_lt = $(am__v_lt_$(V))
am__v_lt_ = $(am__v_lt_$(AM_DEFAULT_VERBOSITY))
am__v_lt_0 = --silent
am__v_lt_1 = 
am_test_options_OBJECTS = test_options.$(OBJEXT)
test_options_OBJECTS = $(am_test_options_OBJECTS)
test_options_DEPENDENCIES = $(lib_onig)
am_test_regset_OBJECTS = test_regset.$(OBJEXT)
test_regset_OBJECTS = $(am_test_regset_OBJECTS)
test_regset_DEPENDENCIES = $(lib_onig)
am_test_syntax_OBJECTS = test_syntax.$(OBJEXT)
test_syntax_OBJECTS = $(am_test_syntax_OBJECTS)
test_syntax_DEPENDENCIES = $(lib_onig)
am_test_utf8_OBJECTS = test_utf8.$(OBJEXT)
test_utf8_OBJECTS = $(am_test_utf8_OBJECTS)
test_utf8_DEPENDENCIES = $(lib_onig)
am_testc_OBJECTS = testc.$(OBJEXT)
testc_OBJECTS = $(am_testc_OBJECTS)
testc_DEPENDENCIES = $(lib_onig)
am_testcu_OBJECTS = testu.$(OBJEXT)
testcu_OBJECTS = $(am_testcu_OBJECTS)
testcu_DEPENDENCIES = $(lib_onig)
am_testp_OBJECTS = testp.$(OBJEXT)
testp_OBJECTS = $(am_testp_OBJECTS)
testp_DEPENDENCIES = $(lib_onig)
AM_V_P = $(am__v_P_$(V))
am__v_P_ = $(am__v_P_$(AM_DEFAULT_VERBOSITY))
am__v_P_0 = false
am__v_P_1 = :
AM_V_GEN = $(am__v_GEN_$(V))
am__v_GEN_ = $(am__v_GEN_$(AM_DEFAULT_VERBOSITY))
am__v_GEN_0 = @echo "  GEN     " $@;
am__v_GEN_1 = 
AM_V_at = $(am__v_at_$(V))
am__v_at_ = $(am__v_at_$(AM_DEFAULT_VERBOSITY))
am__v_at_0 = @
am__v_at_1 = 
DEFAULT_INCLUDES = -I. -I$(top_builddir)/src
depcomp = $(SHELL) $(top_srcdir)/depcomp
am__maybe_remake_depfiles = depfiles
am__depfiles_remade = ./$(DEPDIR)/test_back.Po \
	./$(DEPDIR)/test_options.Po ./$(DEPDIR)/test_regset.Po \
	./$(DEPDIR)/test_syntax.Po ./$(DEPDIR)/test_utf8.Po \
	./$(DEPDIR)/testc.Po ./$(DEPDIR)/testp.Po ./$(DEPDIR)/testu.Po
am__mv = mv -f
COMPILE = $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) \
	$(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
LTCOMPILE = $(LIBTOOL) $(AM_V_lt) --tag=CC $(AM_LIBTOOLFLAGS) \
	$(LIBTOOLFLAGS) --mode=compile $(CC) $(DEFS) \
	$(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) \
	$(AM_CFLAGS) $(CFLAGS)
AM_V_CC = $(am__v_CC_$(V))
am__v_CC_ = $(am__v_CC_$(AM_DEFAULT_VERBOSITY))
am__v_CC_0 = @echo "  CC      " $@;
am__v_CC_1 = 
CCLD = $(CC)
LINK = $(LIBTOOL) $(AM_V_lt) --tag=CC $(AM_LIBTOOLFLAGS) \
	$(LIBTOOLFLAGS) --mode=link $(CCLD) $(AM_CFLAGS) $(CFLAGS) \
	$(AM_LDFLAGS) $(LDFLAGS) -o $@
AM_V_CCLD = $(am__v_CCLD_$(V))
am__v_CCLD_ = $(am__v_CCLD_$(AM_DEFAULT_VERBOSITY))
am__v_CCLD_0 = @echo "  CCLD    " $@;
am__v_CCLD_1 = 
SOURCES = $(test_back_SOURCES) $(test_options_SOURCES) \
	$(test_regset_SOURCES) $(test_syntax_SOURCES) \
	$(test_utf8_SOURCES) $(testc_SOURCES) $(testcu_SOURCES) \
	$(testp_SOURCES)
DIST_SOURCES = $(test_back_SOURCES) $(test_options_SOURCES) \
	$(test_regset_SOURCES) $(test_syntax_SOURCES) \
	$(test_utf8_SOURCES) $(testc_SOURCES) $(testcu_SOURCES) \
	$(testp_SOURCES)
am__can_run_installinfo = \
  case $$AM_UPDATE_INFO_DIR in \
    n|no|NO) false;; \
    *) (install-info --version) >/dev/null 2>&1;; \
  esac
am__tagged_files = $(HEADERS) $(SOURCES) $(TAGS_FILES) $(LISP)
# Read a list of newline-separated strings from the standard input,
# and print each of them once, without duplicates.  Input order is
# *not* preserved.
am__uniquify_input = $(AWK) '\
  BEGIN { nonempty = 0; } \
  { items[$$0] = 1; nonempty = 1; } \
  END { if (nonempty) { for (i in items) print i; }; } \
'
# Make sure the list of sources is unique.  This is necessary because,
# e.g., the same source file might be shared among _SOURCES variables
# for different programs/libraries.
am__define_uniq_tagged_files = \
  list='$(am__tagged_files)'; \
  unique=`for i in $$list; do \
    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
  done | $(am__uniquify_input)`
am__tty_colors_dummy = \
  mgn= red= grn= lgn= blu= brg= std=; \
  am__color_tests=no
am__tty_colors = { \
  $(am__tty_colors_dummy); \
  if test "X$(AM_COLOR_TESTS)" = Xno; then \
    am__color_tests=no; \
  elif test "X$(AM_COLOR_TESTS)" = Xalways; then \
    am__color_tests=yes; \
  elif test "X$$TERM" != Xdumb && { test -t 1; } 2>/dev/null; then \
    am__color_tests=yes; \
  fi; \
  if test $$am__color_tests = yes; then \
    red='[0;31m'; \
    grn='[0;32m'; \
    lgn='[1;32m'; \
    blu='[1;34m'; \
    mgn='[0;35m'; \
    brg='[1m'; \
    std='[m'; \
  fi; \
}
am__vpath_adj_setup = srcdirstrip=`echo "$(srcdir)" | sed 's|.|.|g'`;
am__vpath_adj = case $$p in \
    $(srcdir)/*) f=`echo "$$p" | sed "s|^$$srcdirstrip/||"`;; \
    *) f=$$p;; \
  esac;
am__strip_dir = f=`echo $$p | sed -e 's|^.*/||'`;
am__install_max = 40
am__nobase_strip_setup = \
  srcdirstrip=`echo "$(srcdir)" | sed 's/[].[^$$\\*|]/\\\\&/g'`
am__nobase_strip = \
  for p in $$list; do echo "$$p"; done | sed -e "s|$$srcdirstrip/||"
am__nobase_list = $(am__nobase_strip_setup); \
  for p in $$list; do echo "$$p $$p"; done | \
  sed "s| $$srcdirstrip/| |;"' / .*\//!s/ .*/ ./; s,\( .*\)/[^/]*$$,\1,' | \
  $(AWK) 'BEGIN { files["."] = "" } { files[$$2] = files[$$2] " " $$1; \
    if (++n[$$2] == $(am__install_max)) \
      { print $$2, files[$$2]; n[$$2] = 0; files[$$2] = "" } } \
    END { for (dir in files) print dir, files[dir] }'
am__base_list = \
  sed '$$!N;$$!N;$$!N;$$!N;$$!N;$$!N;$$!N;s/\n/ /g' | \
  sed '$$!N;$$!N;$$!N;$$!N;s/\n/ /g'
am__uninstall_files_from_dir = { \
  test -z "$$files" \
    || { test ! -d "$$dir" && test ! -f "$$dir" && test ! -r "$$dir"; } \
    || { echo " ( cd '$$dir' && rm -f" $$files ")"; \
         $(am__cd) "$$dir" && rm -f $$files; }; \
  }
am__recheck_rx = ^[ 	]*:recheck:[ 	]*
am__global_test_result_rx = ^[ 	]*:global-test-result:[ 	]*
am__copy_in_global_log_rx = ^[ 	]*:copy-in-global-log:[ 	]*
# A command that, given a newline-separated list of test names on the
# standard input, print the name of the tests that are to be re-run
# upon "make recheck".
am__list_recheck_tests = $(AWK) '{ \
  recheck = 1; \
  while ((rc = (getline line < ($$0 ".trs"))) != 0) \
    { \
      if (rc < 0) \
        { \
          if ((getline line2 < ($$0 ".log")) < 0) \
	    recheck = 0; \
          break; \
        } \
      else if (line ~ /$(am__recheck_rx)[nN][Oo]/) \
        { \
          recheck = 0; \
          break; \
        } \
      else if (line ~ /$(am__recheck_rx)[yY][eE][sS]/) \
        { \
          break; \
        } \
    }; \
  if (recheck) \
    print $$0; \
  close ($$0 ".trs"); \
  close ($$0 ".log"); \
}'
# A command that, given a newline-separated list of test names on the
# standard input, create the global log from their .trs and .log files.
am__create_global_log = $(AWK) ' \
function fatal(msg) \
{ \
  print "fatal: making $@: " msg | "cat >&2"; \
  exit 1; \
} \
function rst_section(header) \
{ \
  print header; \
  len = length(header); \
  for (i = 1; i <= len; i = i + 1) \
    printf "="; \
  printf "\n\n"; \
} \
{ \
  copy_in_global_log = 1; \
  global_test_result = "RUN"; \
  while ((rc = (getline line < ($$0 ".trs"))) != 0) \
    { \
      if (rc < 0) \
         fatal("failed to read from " $$0 ".trs"); \
      if (line ~ /$(am__global_test_result_rx)/) \
        { \
          sub("$(am__global_test_result_rx)", "", line); \
          sub("[ 	]*$$", "", line); \
          global_test_result = line; \
        } \
      else if (line ~ /$(am__copy_in_global_log_rx)[nN][oO]/) \
        copy_in_global_log = 0; \
    }; \
  if (copy_in_global_log) \
    { \
      rst_section(global_test_result ": " $$0); \
      while ((rc = (getline line < ($$0 ".log"))) != 0) \
      { \
        if (rc < 0) \
          fatal("failed to read from " $$0 ".log"); \
        print line; \
      }; \
      printf "\n"; \
    }; \
  close ($$0 ".trs"); \
  close ($$0 ".log"); \
}'
# Restructured Text title.
am__rst_title = { sed 's/.*/   &   /;h;s/./=/g;p;x;s/ *$$//;p;g' && echo; }
# Solaris 10 'make', and several other traditional 'make' implementations,
# pass "-e" to $(SHELL), and POSIX 2008 even requires this.  Work around it
# by disabling -e (using the XSI extension "set +e") if it's set.
am__sh_e_setup = case $$- in *e*) set +e;; esac
# Default flags passed to test drivers.
am__common_driver_flags = \
  --color-tests "$$am__color_tests" \
  --enable-hard-errors "$$am__enable_hard_errors" \
  --expect-failure "$$am__expect_failure"
# To be inserted before the command running the test.  Creates the
# directory for the log if needed.  Stores in $dir the directory
# containing $f, in $tst the test, in $log the log.  Executes the
# developer- defined test setup AM_TESTS_ENVIRONMENT (if any), and
# passes TESTS_ENVIRONMENT.  Set up options for the wrapper that
# will run the test scripts (or their associated LOG_COMPILER, if
# thy have one).
am__check_pre = \
$(am__sh_e_setup);					\
$(am__vpath_adj_setup) $(am__vpath_adj)			\
$(am__tty_colors);					\
srcdir=$(srcdir); export srcdir;			\
case "$@" in						\
  */*) am__odir=`echo "./$@" | sed 's|/[^/]*$$||'`;;	\
    *) am__odir=.;; 					\
esac;							\
test "x$$am__odir" = x"." || test -d "$$am__odir" 	\
  || $(MKDIR_P) "$$am__odir" || exit $$?;		\
if test -f "./$$f"; then dir=./;			\
elif test -f "$$f"; then dir=;				\
else dir="$(srcdir)/"; fi;				\
tst=$$dir$$f; log='$@'; 				\
if test -n '$(DISABLE_HARD_ERRORS)'; then		\
  am__enable_hard_errors=no; 				\
else							\
  am__enable_hard_errors=yes; 				\
fi; 							\
case " $(XFAIL_TESTS) " in				\
  *[\ \	]$$f[\ \	]* | *[\ \	]$$dir$$f[\ \	]*) \
    am__expect_failure=yes;;				\
  *)							\
    am__expect_failure=no;;				\
esac; 							\
$(AM_TESTS_ENVIRONMENT) $(TESTS_ENVIRONMENT)
# A shell command to get the names of the tests scripts with any registered
# extension removed (i.e., equivalently, the names of the test logs, with
# the '.log' extension removed).  The result is saved in the shell variable
# '$bases'.  This honors runtime overriding of TESTS and TEST_LOGS.  Sadly,
# we cannot use something simpler, involving e.g., "$(TEST_LOGS:.log=)",
# since that might cause problem with VPATH rewrites for suffix-less tests.
# See also 'test-harness-vpath-rewrite.sh' and 'test-trs-basic.sh'.
am__set_TESTS_bases = \
  bases='$(TEST_LOGS)'; \
  bases=`for i in $$bases; do echo $$i; done | sed 's/\.log$$//'`; \
  bases=`echo $$bases`
AM_TESTSUITE_SUMMARY_HEADER = ' for $(PACKAGE_STRING)'
RECHECK_LOGS = $(TEST_LOGS)
AM_RECURSIVE_TARGETS = check recheck
TEST_SUITE_LOG = test-suite.log
TEST_EXTENSIONS =  .test
LOG_DRIVER = $(SHELL) $(top_srcdir)/test-driver
LOG_COMPILE = $(LOG_COMPILER) $(AM_LOG_FLAGS) $(LOG_FLAGS)
am__set_b = \
  case '$@' in \
    */*) \
      case '$*' in \
        */*) b='$*';; \
          *) b=`echo '$@' | sed 's/\.log$$//'`; \
       esac;; \
    *) \
      b='$*';; \
  esac
am__test_logs1 = $(TESTS:=.log)
am__test_logs2 = $(am__test_logs1:.log=.log)
TEST_LOGS = $(am__test_logs2:.test.log=.log)
TEST_LOG_DRIVER = $(SHELL) $(top_srcdir)/test-driver
TEST_LOG_COMPILE = $(TEST_LOG_COMPILER) $(AM_TEST_LOG_FLAGS) \
	$(TEST_LOG_FLAGS)
am__DIST_COMMON = $(srcdir)/Makefile.in $(top_srcdir)/depcomp \
	$(top_srcdir)/test-driver
DISTFILES = $(DIST_COMMON) $(DIST_SOURCES) $(TEXINFOS) $(EXTRA_DIST)
ACLOCAL = ${SHELL} '/root/dataset_fuzz/jq-1.7.1/modules/oniguruma/missing' aclocal-1.16
ALLOCA = 
AMTAR = $${TAR-tar}
AM_DEFAULT_VERBOSITY = 1
AR = ar
AUTOCONF = ${SHELL} '/root/dataset_fuzz/jq-1.7.1/modules/oniguruma/missing' autoconf
AUTOHEADER = ${SHELL} '/root/dataset_fuzz/jq-1.7.1/modules/oniguruma/missing' autoheader
AUTOMAKE = ${SHELL} '/root/dataset_fuzz/jq-1.7.1/modules/oniguruma/missing' automake-1.16
AWK = gawk
CC = gcc
CCDEPMODE = depmode=gcc3
CFLAGS = -fsanitize=address -g -O1 -Wno-error=maybe-uninitialized
CPPFLAGS = 
CSCOPE = cscope
CTAGS = ctags
CYGPATH_W = echo
DEFS = -DHAVE_CONFIG_H
DEPDIR = .deps
DLLTOOL = false
DSYMUTIL = 
DUMPBIN = 
ECHO_C = 
ECHO_N = -n
ECHO_T = 
EGREP = /usr/bin/grep -E
ETAGS = etags
EXEEXT = 
EXTRA_LIBONIG_LDFLAGS =  
FGREP = /usr/bin/grep -F
GREP = /usr/bin/grep
INSTALL = /usr/bin/install -c
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_PROGRAM = ${INSTALL}
INSTALL_SCRIPT = ${INSTALL}
INSTALL_STRIP_PROGRAM = $(install_sh) -c -s
LD = /usr/bin/ld -m elf_x86_64
LDFLAGS = -fsanitize=address
LIBOBJS = 
LIBONIG_DEF_FILE = 
LIBS = 
LIBTOOL = $(SHELL) $(top_builddir)/libtool
LIPO = 
LN_S = ln -s
LTLIBOBJS = 
LTVERSION = 8:0:3
LT_SYS_LIBRARY_PATH = 
MAKEINFO = ${SHELL} '/root/dataset_fuzz/jq-1.7.1/modules/oniguruma/missing' makeinfo
MANIFEST_TOOL = :
MKDIR_P = /usr/bin/mkdir -p
NM = /usr/bin/nm -B
NMEDIT = 
OBJDUMP = objdump
OBJEXT = o
OTOOL = 
OTOOL64 = 
PACKAGE = onig
PACKAGE_BUGREPORT = 
PACKAGE_NAME = onig
PACKAGE_STRING = onig 6.9.8
PACKAGE_TARNAME = onig
PACKAGE_URL = 
PACKAGE_VERSION = 6.9.8
PATH_SEPARATOR = :
RANLIB = ranlib
SED = /usr/bin/sed
SET_MAKE = 
SHELL = /bin/bash
STRIP = strip
VERSION = 6.9.8
abs_builddir = /root/dataset_fuzz/jq-1.7.1/modules/oniguruma/test
abs_srcdir = /root/dataset_fuzz/jq-1.7.1/modules/oniguruma/test
abs_top_builddir = /root/dataset_fuzz/jq-1.7.1/modules/oniguruma
abs_top_srcdir = /root/dataset_fuzz/jq-1.7.1/modules/oniguruma
ac_ct_AR = ar
ac_ct_CC = gcc
ac_ct_DUMPBIN = 
am__include = include
am__leading_dot = .
am__quote = 
am__tar = $${TAR-tar} chof - "$$tardir"
am__untar = $${TAR-tar} xf -
bindir = ${exec_prefix}/bin
build = x86_64-pc-linux-gnu
build_alias = 
build_cpu = x86_64
build_os = linux-gnu
build_vendor = pc
builddir = .
datadir = ${datarootdir}
datarootdir = ${prefix}/share
docdir = ${datarootdir}/doc/${PACKAGE_TARNAME}
dvidir = ${docdir}
exec_prefix = ${prefix}
host = x86_64-pc-linux-gnu
host_alias = 
host_cpu = x86_64
host_os = linux-gnu
host_vendor = pc
htmldir = ${docdir}
includedir = ${prefix}/include
infodir = ${datarootdir}/info
install_sh = ${SHELL} /root/dataset_fuzz/jq-1.7.1/modules/oniguruma/install-sh
libdir = ${exec_prefix}/lib
libexecdir = ${exec_prefix}/libexec
localedir = ${datarootdir}/locale
localstatedir = ${prefix}/var
mandir = ${datarootdir}/man
mkdir_p = $(MKDIR_P)
oldincludedir = /usr/include
pdfdir = ${docdir}
prefix = /usr/local
program_transform_name = s,x,x,
psdir = ${docdir}
runstatedir = ${localstatedir}/run
sbindir = ${exec_prefix}/sbin
sharedstatedir = ${prefix}/com
srcdir = .
sysconfdir = ${prefix}/etc
target_alias = 
top_build_prefix = ../
top_builddir = ..
top_srcdir = ..
lib_onig = ../src/libonig.la
AM_LDFLAGS = -L$(libdir)
AM_CFLAGS = -Wall -Wno-invalid-source-encoding
AM_CPPFLAGS = -I$(top_srcdir)/src
test_utf8_SOURCES = test_utf8.c
test_utf8_LDADD = $(lib_onig)
test_syntax_SOURCES = test_syntax.c
test_syntax_LDADD = $(lib_onig)
test_options_SOURCES = test_options.c
test_options_LDADD = $(lib_onig)
testc_SOURCES = testc.c
testc_LDADD = $(lib_onig)
testp_SOURCES = testp.c
testp_LDADD = $(lib_onig)
testcu_SOURCES = testu.c
testcu_LDADD = $(lib_onig)
test_regset_SOURCES = test_regset.c
test_regset_LDADD = $(lib_onig)
test_back_SOURCES = test_back.c
test_back_LDADD = $(lib_onig)
all: all-am

.SUFFIXES:
.SUFFIXES: .c .lo .log .o .obj .test .test$(EXEEXT) .trs
$(srcdir)/Makefile.in:  $(srcdir)/Makefile.am  $(am__configure_deps)
	@for dep in $?; do \
	  case '$(am__configure_deps)' in \
	    *$$dep*) \
	      ( cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh ) \
	        && { if test -f $@; then exit 0; else break; fi; }; \
	      exit 1;; \
	  esac; \
	done; \
	echo ' cd $(top_srcdir) && $(AUTOMAKE) --gnu test/Makefile'; \
	$(am__cd) $(top_srcdir) && \
	  $(AUTOMAKE) --gnu test/Makefile
Makefile: $(srcdir)/Makefile.in $(top_builddir)/config.status
	@case '$?' in \
	  *config.status*) \
	    cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh;; \
	  *) \
	    echo ' cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@ $(am__maybe_remake_depfiles)'; \
	    cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@ $(am__maybe_remake_depfiles);; \
	esac;

$(top_builddir)/config.status: $(top_srcdir)/configure $(CONFIG_STATUS_DEPENDENCIES)
	cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh

$(top_srcdir)/configure:  $(am__configure_deps)
	cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh
$(ACLOCAL_M4):  $(am__aclocal_m4_deps)
	cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh
$(am__aclocal_m4_deps):

clean-checkPROGRAMS:
	@list='$(check_PROGRAMS)'; test -n "$$list" || exit 0; \
	echo " rm -f" $$list; \
	rm -f $$list || exit $$?; \
	test -n "$(EXEEXT)" || exit 0; \
	list=`for p in $$list; do echo "$$p"; done | sed 's/$(EXEEXT)$$//'`; \
	echo " rm -f" $$list; \
	rm -f $$list

test_back$(EXEEXT): $(test_back_OBJECTS) $(test_back_DEPENDENCIES) $(EXTRA_test_back_DEPENDENCIES) 
	@rm -f test_back$(EXEEXT)
	$(AM_V_CCLD)$(LINK) $(test_back_OBJECTS) $(test_back_LDADD) $(LIBS)

test_options$(EXEEXT): $(test_options_OBJECTS) $(test_options_DEPENDENCIES) $(EXTRA_test_options_DEPENDENCIES) 
	@rm -f test_options$(EXEEXT)
	$(AM_V_CCLD)$(LINK) $(test_options_OBJECTS) $(test_options_LDADD) $(LIBS)

test_regset$(EXEEXT): $(test_regset_OBJECTS) $(test_regset_DEPENDENCIES) $(EXTRA_test_regset_DEPENDENCIES) 
	@rm -f test_regset$(EXEEXT)
	$(AM_V_CCLD)$(LINK) $(test_regset_OBJECTS) $(test_regset_LDADD) $(LIBS)

test_syntax$(EXEEXT): $(test_syntax_OBJECTS) $(test_syntax_DEPENDENCIES) $(EXTRA_test_syntax_DEPENDENCIES) 
	@rm -f test_syntax$(EXEEXT)
	$(AM_V_CCLD)$(LINK) $(test_syntax_OBJECTS) $(test_syntax_LDADD) $(LIBS)

test_utf8$(EXEEXT): $(test_utf8_OBJECTS) $(test_utf8_DEPENDENCIES) $(EXTRA_test_utf8_DEPENDENCIES) 
	@rm -f test_utf8$(EXEEXT)
	$(AM_V_CCLD)$(LINK) $(test_utf8_OBJECTS) $(test_utf8_LDADD) $(LIBS)

testc$(EXEEXT): $(testc_OBJECTS) $(testc_DEPENDENCIES) $(EXTRA_testc_DEPENDENCIES) 
	@rm -f testc$(EXEEXT)
	$(AM_V_CCLD)$(LINK) $(testc_OBJECTS) $(testc_LDADD) $(LIBS)

testcu$(EXEEXT): $(testcu_OBJECTS) $(testcu_DEPENDENCIES) $(EXTRA_testcu_DEPENDENCIES) 
	@rm -f testcu$(EXEEXT)
	$(AM_V_CCLD)$(LINK) $(testcu_OBJECTS) $(testcu_LDADD) $(LIBS)

testp$(EXEEXT): $(testp_OBJECTS) $(testp_DEPENDENCIES) $(EXTRA_testp_DEPENDENCIES) 
	@rm -f testp$(EXEEXT)
	$(AM_V_CCLD)$(LINK) $(testp_OBJECTS) $(testp_LDADD) $(LIBS)

mostlyclean-compile:
	-rm -f *.$(OBJEXT)

distclean-compile:
	-rm -f *.tab.c

include ./$(DEPDIR)/test_back.Po # am--include-marker
include ./$(DEPDIR)/test_options.Po # am--include-marker
include ./$(DEPDIR)/test_regset.Po # am--include-marker
include ./$(DEPDIR)/test_syntax.Po # am--include-marker
include ./$(DEPDIR)/test_utf8.Po # am--include-marker
include ./$(DEPDIR)/testc.Po # am--include-marker
include ./$(DEPDIR)/testp.Po # am--include-marker
include ./$(DEPDIR)/testu.Po # am--include-marker

$(am__depfiles_remade):
	@$(MKDIR_P) $(@D)
	@echo '# dummy' >$@-t && $(am__mv) $@-t $@

am--depfiles: $(am__depfiles_remade)

.c.o:
	$(AM_V_CC)$(COMPILE) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ $<
	$(AM_V_at)$(am__mv) $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Po
#	$(AM_V_CC)source='$<' object='$@' libtool=no \
#	DEPDIR=$(DEPDIR) $(CCDEPMODE) $(depcomp) \
#	$(AM_V_CC_no)$(COMPILE) -c -o $@ $<

.c.obj:
	$(AM_V_CC)$(COMPILE) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ `$(CYGPATH_W) '$<'`
	$(AM_V_at)$(am__mv) $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Po
#	$(AM_V_CC)source='$<' object='$@' libtool=no \
#	DEPDIR=$(DEPDIR) $(CCDEPMODE) $(depcomp) \
#	$(AM_V_CC_no)$(COMPILE) -c -o $@ `$(CYGPATH_W) '$<'`

.c.lo:
	$(AM_V_CC)$(LTCOMPILE) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ $<
	$(AM_V_at)$(am__mv) $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Plo
#	$(AM_V_CC)source='$<' object='$@' libtool=yes \
#	DEPDIR=$(DEPDIR) $(CCDEPMODE) $(depcomp) \
#	$(AM_V_CC_no)$(LTCOMPILE) -c -o $@ $<

mostlyclean-libtool:
	-rm -f *.lo

clean-libtool:
	-rm -rf .libs _libs

ID: $(am__tagged_files)
	$(am__define_uniq_tagged_files); mkid -fID $$unique
tags: tags-am
TAGS: tags

tags-am: $(TAGS_DEPENDENCIES) $(am__tagged_files)
	set x; \
	here=`pwd`; \
	$(am__define_uniq_tagged_files); \
	shift; \
	if test -z "$(ETAGS_ARGS)$$*$$unique"; then :; else \
	  test -n "$$unique" || unique=$$empty_fix; \
	  if test $$# -gt 0; then \
	    $(ETAGS) $(ETAGSFLAGS) $(AM_ETAGSFLAGS) $(ETAGS_ARGS) \
	      "$$@" $$unique; \
	  else \
	    $(ETAGS) $(ETAGSFLAGS) $(AM_ETAGSFLAGS) $(ETAGS_ARGS) \
	      $$unique; \
	  fi; \
	fi
ctags: ctags-am

CTAGS: ctags
ctags-am: $(TAGS_DEPENDENCIES) $(am__tagged_files)
	$(am__define_uniq_tagged_files); \
	test -z "$(CTAGS_ARGS)$$unique" \
	  || $(CTAGS) $(CTAGSFLAGS) $(AM_CTAGSFLAGS) $(CTAGS_ARGS) \
	     $$unique

GTAGS:
	here=`$(am__cd) $(top_builddir) && pwd` \
	  && $(am__cd) $(top_srcdir) \
	  && gtags -i $(GTAGS_ARGS) "$$here"
cscopelist: cscopelist-am

cscopelist-am: $(am__tagged_files)
	list='$(am__tagged_files)'; \
	case "$(srcdir)" in \
	  [\\/]* | ?:[\\/]*) sdir="$(srcdir)" ;; \
	  *) sdir=$(subdir)/$(srcdir) ;; \
	esac; \
	for i in $$list; do \
	  if test -f "$$i"; then \
	    echo "$(subdir)/$$i"; \
	  else \
	    echo "$$sdir/$$i"; \
	  fi; \
	done >> $(top_builddir)/cscope.files

distclean-tags:
	-rm -f TAGS ID GTAGS GRTAGS GSYMS GPATH tags

# Recover from deleted '.trs' file; this should ensure that
# "rm -f foo.log; make foo.trs" re-run 'foo.test', and re-create
# both 'foo.log' and 'foo.trs'.  Break the recipe in two subshells
# to avoid problems with "make -n".
.log.trs:
	rm -f $< $@
	$(MAKE) $(AM_MAKEFLAGS) $<

# Leading 'am--fnord' is there to ensure the list of targets does not
# expand to empty, as could happen e.g. with make check TESTS=''.
am--fnord $(TEST_LOGS) $(TEST_LOGS:.log=.trs): $(am__force_recheck)
am--force-recheck:
	@:

$(TEST_SUITE_LOG): $(TEST_LOGS)
	@$(am__set_TESTS_bases); \
	am__f_ok () { test -f "$$1" && test -r "$$1"; }; \
	redo_bases=`for i in $$bases; do \
	              am__f_ok $$i.trs && am__f_ok $$i.log || echo $$i; \
	            done`; \
	if test -n "$$redo_bases"; then \
	  redo_logs=`for i in $$redo_bases; do echo $$i.log; done`; \
	  redo_results=`for i in $$redo_bases; do echo $$i.trs; done`; \
	  if $(am__make_dryrun); then :; else \
	    rm -f $$redo_logs && rm -f $$redo_results || exit 1; \
	  fi; \
	fi; \
	if test -n "$$am__remaking_logs"; then \
	  echo "fatal: making $(TEST_SUITE_LOG): possible infinite" \
	       "recursion detected" >&2; \
	elif test -n "$$redo_logs"; then \
	  am__remaking_logs=yes $(MAKE) $(AM_MAKEFLAGS) $$redo_logs; \
	fi; \
	if $(am__make_dryrun); then :; else \
	  st=0;  \
	  errmsg="fatal: making $(TEST_SUITE_LOG): failed to create"; \
	  for i in $$redo_bases; do \
	    test -f $$i.trs && test -r $$i.trs \
	      || { echo "$$errmsg $$i.trs" >&2; st=1; }; \
	    test -f $$i.log && test -r $$i.log \
	      || { echo "$$errmsg $$i.log" >&2; st=1; }; \
	  done; \
	  test $$st -eq 0 || exit 1; \
	fi
	@$(am__sh_e_setup); $(am__tty_colors); $(am__set_TESTS_bases); \
	ws='[ 	]'; \
	results=`for b in $$bases; do echo $$b.trs; done`; \
	test -n "$$results" || results=/dev/null; \
	all=`  grep "^$$ws*:test-result:"           $$results | wc -l`; \
	pass=` grep "^$$ws*:test-result:$$ws*PASS"  $$results | wc -l`; \
	fail=` grep "^$$ws*:test-result:$$ws*FAIL"  $$results | wc -l`; \
	skip=` grep "^$$ws*:test-result:$$ws*SKIP"  $$results | wc -l`; \
	xfail=`grep "^$$ws*:test-result:$$ws*XFAIL" $$results | wc -l`; \
	xpass=`grep "^$$ws*:test-result:$$ws*XPASS" $$results | wc -l`; \
	error=`grep "^$$ws*:test-result:$$ws*ERROR" $$results | wc -l`; \
	if test `expr $$fail + $$xpass + $$error` -eq 0; then \
	  success=true; \
	else \
	  success=false; \
	fi; \
	br='==================='; br=$$br$$br$$br$$br; \
	result_count () \
	{ \
	    if test x"$$1" = x"--maybe-color"; then \
	      maybe_colorize=yes; \
	    elif test x"$$1" = x"--no-color"; then \
	      maybe_colorize=no; \
	    else \
	      echo "$@: invalid 'result_count' usage" >&2; exit 4; \
	    fi; \
	    shift; \
	    desc=$$1 count=$$2; \
	    if test $$maybe_colorize = yes && test $$count -gt 0; then \
	      color_start=$$3 color_end=$$std; \
	    else \
	      color_start= color_end=; \
	    fi; \
	    echo "$${color_start}# $$desc $$count$${color_end}"; \
	}; \
	create_testsuite_report () \
	{ \
	  result_count $$1 "TOTAL:" $$all   "$$brg"; \
	  result_count $$1 "PASS: " $$pass  "$$grn"; \
	  result_count $$1 "SKIP: " $$skip  "$$blu"; \
	  result_count $$1 "XFAIL:" $$xfail "$$lgn"; \
	  result_count $$1 "FAIL: " $$fail  "$$red"; \
	  result_count $$1 "XPASS:" $$xpass "$$red"; \
	  result_count $$1 "ERROR:" $$error "$$mgn"; \
	}; \
	{								\
	  echo "$(PACKAGE_STRING): $(subdir)/$(TEST_SUITE_LOG)" |	\
	    $(am__rst_title);						\
	  create_testsuite_report --no-color;				\
	  echo;								\
	  echo ".. contents:: :depth: 2";				\
	  echo;								\
	  for b in $$bases; do echo $$b; done				\
	    | $(am__create_global_log);					\
	} >$(TEST_SUITE_LOG).tmp || exit 1;				\
	mv $(TEST_SUITE_LOG).tmp $(TEST_SUITE_LOG);			\
	if $$success; then						\
	  col="$$grn";							\
	 else								\
	  col="$$red";							\
	  test x"$$VERBOSE" = x || cat $(TEST_SUITE_LOG);		\
	fi;								\
	echo "$${col}$$br$${std}"; 					\
	echo "$${col}Testsuite summary"$(AM_TESTSUITE_SUMMARY_HEADER)"$${std}";	\
	echo "$${col}$$br$${std}"; 					\
	create_testsuite_report --maybe-color;				\
	echo "$$col$$br$$std";						\
	if $$success; then :; else					\
	  echo "$${col}See $(subdir)/$(TEST_SUITE_LOG)$${std}";		\
	  if test -n "$(PACKAGE_BUGREPORT)"; then			\
	    echo "$${col}Please report to $(PACKAGE_BUGREPORT)$${std}";	\
	  fi;								\
	  echo "$$col$$br$$std";					\
	fi;								\
	$$success || exit 1

check-TESTS: $(check_PROGRAMS)
	@list='$(RECHECK_LOGS)';           test -z "$$list" || rm -f $$list
	@list='$(RECHECK_LOGS:.log=.trs)'; test -z "$$list" || rm -f $$list
	@test -z "$(TEST_SUITE_LOG)" || rm -f $(TEST_SUITE_LOG)
	@set +e; $(am__set_TESTS_bases); \
	log_list=`for i in $$bases; do echo $$i.log; done`; \
	trs_list=`for i in $$bases; do echo $$i.trs; done`; \
	log_list=`echo $$log_list`; trs_list=`echo $$trs_list`; \
	$(MAKE) $(AM_MAKEFLAGS) $(TEST_SUITE_LOG) TEST_LOGS="$$log_list"; \
	exit $$?;
recheck: all $(check_PROGRAMS)
	@test -z "$(TEST_SUITE_LOG)" || rm -f $(TEST_SUITE_LOG)
	@set +e; $(am__set_TESTS_bases); \
	bases=`for i in $$bases; do echo $$i; done \
	         | $(am__list_recheck_tests)` || exit 1; \
	log_list=`for i in $$bases; do echo $$i.log; done`; \
	log_list=`echo $$log_list`; \
	$(MAKE) $(AM_MAKEFLAGS) $(TEST_SUITE_LOG) \
	        am__force_recheck=am--force-recheck \
	        TEST_LOGS="$$log_list"; \
	exit $$?
test_utf8.log: test_utf8$(EXEEXT)
	@p='test_utf8$(EXEEXT)'; \
	b='test_utf8'; \
	$(am__check_pre) $(LOG_DRIVER) --test-name "$$f" \
	--log-file $$b.log --trs-file $$b.trs \
	$(am__common_driver_flags) $(AM_LOG_DRIVER_FLAGS) $(LOG_DRIVER_FLAGS) -- $(LOG_COMPILE) \
	"$$tst" $(AM_TESTS_FD_REDIRECT)
test_syntax.log: test_syntax$(EXEEXT)
	@p='test_syntax$(EXEEXT)'; \
	b='test_syntax'; \
	$(am__check_pre) $(LOG_DRIVER) --test-name "$$f" \
	--log-file $$b.log --trs-file $$b.trs \
	$(am__common_driver_flags) $(AM_LOG_DRIVER_FLAGS) $(LOG_DRIVER_FLAGS) -- $(LOG_COMPILE) \
	"$$tst" $(AM_TESTS_FD_REDIRECT)
test_options.log: test_options$(EXEEXT)
	@p='test_options$(EXEEXT)'; \
	b='test_options'; \
	$(am__check_pre) $(LOG_DRIVER) --test-name "$$f" \
	--log-file $$b.log --trs-file $$b.trs \
	$(am__common_driver_flags) $(AM_LOG_DRIVER_FLAGS) $(LOG_DRIVER_FLAGS) -- $(LOG_COMPILE) \
	"$$tst" $(AM_TESTS_FD_REDIRECT)
testc.log: testc$(EXEEXT)
	@p='testc$(EXEEXT)'; \
	b='testc'; \
	$(am__check_pre) $(LOG_DRIVER) --test-name "$$f" \
	--log-file $$b.log --trs-file $$b.trs \
	$(am__common_driver_flags) $(AM_LOG_DRIVER_FLAGS) $(LOG_DRIVER_FLAGS) -- $(LOG_COMPILE) \
	"$$tst" $(AM_TESTS_FD_REDIRECT)
testcu.log: testcu$(EXEEXT)
	@p='testcu$(EXEEXT)'; \
	b='testcu'; \
	$(am__check_pre) $(LOG_DRIVER) --test-name "$$f" \
	--log-file $$b.log --trs-file $$b.trs \
	$(am__common_driver_flags) $(AM_LOG_DRIVER_FLAGS) $(LOG_DRIVER_FLAGS) -- $(LOG_COMPILE) \
	"$$tst" $(AM_TESTS_FD_REDIRECT)
test_regset.log: test_regset$(EXEEXT)
	@p='test_regset$(EXEEXT)'; \
	b='test_regset'; \
	$(am__check_pre) $(LOG_DRIVER) --test-name "$$f" \
	--log-file $$b.log --trs-file $$b.trs \
	$(am__common_driver_flags) $(AM_LOG_DRIVER_FLAGS) $(LOG_DRIVER_FLAGS) -- $(LOG_COMPILE) \
	"$$tst" $(AM_TESTS_FD_REDIRECT)
test_back.log: test_back$(EXEEXT)
	@p='test_back$(EXEEXT)'; \
	b='test_back'; \
	$(am__check_pre) $(LOG_DRIVER) --test-name "$$f" \
	--log-file $$b.log --trs-file $$b.trs \
	$(am__common_driver_flags) $(AM_LOG_DRIVER_FLAGS) $(LOG_DRIVER_FLAGS) -- $(LOG_COMPILE) \
	"$$tst" $(AM_TESTS_FD_REDIRECT)
testp.log: testp$(EXEEXT)
	@p='testp$(EXEEXT)'; \
	b='testp'; \
	$(am__check_pre) $(LOG_DRIVER) --test-name "$$f" \
	--log-file $$b.log --trs-file $$b.trs \
	$(am__common_driver_flags) $(AM_LOG_DRIVER_FLAGS) $(LOG_DRIVER_FLAGS) -- $(LOG_COMPILE) \
	"$$tst" $(AM_TESTS_FD_REDIRECT)
.test.log:
	@p='$<'; \
	$(am__set_b); \
	$(am__check_pre) $(TEST_LOG_DRIVER) --test-name "$$f" \
	--log-file $$b.log --trs-file $$b.trs \
	$(am__common_driver_flags) $(AM_TEST_LOG_DRIVER_FLAGS) $(TEST_LOG_DRIVER_FLAGS) -- $(TEST_LOG_COMPILE) \
	"$$tst" $(AM_TESTS_FD_REDIRECT)
#.test$(EXEEXT).log:
#	@p='$<'; \
#	$(am__set_b); \
#	$(am__check_pre) $(TEST_LOG_DRIVER) --test-name "$$f" \
#	--log-file $$b.log --trs-file $$b.trs \
#	$(am__common_driver_flags) $(AM_TEST_LOG_DRIVER_FLAGS) $(TEST_LOG_DRIVER_FLAGS) -- $(TEST_LOG_COMPILE) \
#	"$$tst" $(AM_TESTS_FD_REDIRECT)
distdir: $(BUILT_SOURCES)
	$(MAKE) $(AM_MAKEFLAGS) distdir-am

distdir-am: $(DISTFILES)
	@srcdirstrip=`echo "$(srcdir)" | sed 's/[].[^$$\\*]/\\\\&/g'`; \
	topsrcdirstrip=`echo "$(top_srcdir)" | sed 's/[].[^$$\\*]/\\\\&/g'`; \
	list='$(DISTFILES)'; \
	  dist_files=`for file in $$list; do echo $$file; done | \
	  sed -e "s|^$$srcdirstrip/||;t" \
	      -e "s|^$$topsrcdirstrip/|$(top_builddir)/|;t"`; \
	case $$dist_files in \
	  */*) $(MKDIR_P) `echo "$$dist_files" | \
			   sed '/\//!d;s|^|$(distdir)/|;s,/[^/]*$$,,' | \
			   sort -u` ;; \
	esac; \
	for file in $$dist_files; do \
	  if test -f $$file || test -d $$file; then d=.; else d=$(srcdir); fi; \
	  if test -d $$d/$$file; then \
	    dir=`echo "/$$file" | sed -e 's,/[^/]*$$,,'`; \
	    if test -d "$(distdir)/$$file"; then \
	      find "$(distdir)/$$file" -type d ! -perm -700 -exec chmod u+rwx {} \;; \
	    fi; \
	    if test -d $(srcdir)/$$file && test $$d != $(srcdir); then \
	      cp -fpR $(srcdir)/$$file "$(distdir)$$dir" || exit 1; \
	      find "$(distdir)/$$file" -type d ! -perm -700 -exec chmod u+rwx {} \;; \
	    fi; \
	    cp -fpR $$d/$$file "$(distdir)$$dir" || exit 1; \
	  else \
	    test -f "$(distdir)/$$file" \
	    || cp -p $$d/$$file "$(distdir)/$$file" \
	    || exit 1; \
	  fi; \
	done
check-am: all-am
	$(MAKE) $(AM_MAKEFLAGS) $(check_PROGRAMS)
	$(MAKE) $(AM_MAKEFLAGS) check-TESTS
check: check-am
all-am: Makefile
installdirs:
install: install-am
install-exec: install-exec-am
install-data: install-data-am
uninstall: uninstall-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am

installcheck: installcheck-am
install-strip:
	if test -z '$(STRIP)'; then \
	  $(MAKE) $(AM_MAKEFLAGS) INSTALL_PROGRAM="$(INSTALL_STRIP_PROGRAM)" \
	    install_sh_PROGRAM="$(INSTALL_STRIP_PROGRAM)" INSTALL_STRIP_FLAG=-s \
	      install; \
	else \
	  $(MAKE) $(AM_MAKEFLAGS) INSTALL_PROGRAM="$(INSTALL_STRIP_PROGRAM)" \
	    install_sh_PROGRAM="$(INSTALL_STRIP_PROGRAM)" INSTALL_STRIP_FLAG=-s \
	    "INSTALL_PROGRAM_ENV=STRIPPROG='$(STRIP)'" install; \
	fi
mostlyclean-generic:
	-test -z "$(TEST_LOGS)" || rm -f $(TEST_LOGS)
	-test -z "$(TEST_LOGS:.log=.trs)" || rm -f $(TEST_LOGS:.log=.trs)
	-test -z "$(TEST_SUITE_LOG)" || rm -f $(TEST_SUITE_LOG)

clean-generic:

distclean-generic:
	-test -z "$(CONFIG_CLEAN_FILES)" || rm -f $(CONFIG_CLEAN_FILES)
	-test . = "$(srcdir)" || test -z "$(CONFIG_CLEAN_VPATH_FILES)" || rm -f $(CONFIG_CLEAN_VPATH_FILES)

maintainer-clean-generic:
	@echo "This command is intended for maintainers to use"
	@echo "it deletes files that may require special tools to rebuild."
clean: clean-am

clean-am: clean-checkPROGRAMS clean-generic clean-libtool \
	mostlyclean-am

distclean: distclean-am
		-rm -f ./$(DEPDIR)/test_back.Po
	-rm -f ./$(DEPDIR)/test_options.Po
	-rm -f ./$(DEPDIR)/test_regset.Po
	-rm -f ./$(DEPDIR)/test_syntax.Po
	-rm -f ./$(DEPDIR)/test_utf8.Po
	-rm -f ./$(DEPDIR)/testc.Po
	-rm -f ./$(DEPDIR)/testp.Po
	-rm -f ./$(DEPDIR)/testu.Po
	-rm -f Makefile
distclean-am: clean-am distclean-compile distclean-generic \
	distclean-tags

dvi: dvi-am

dvi-am:

html: html-am

html-am:

info: info-am

info-am:

install-data-am:

install-dvi: install-dvi-am

install-dvi-am:

install-exec-am:

install-html: install-html-am

install-html-am:

install-info: install-info-am

install-info-am:

install-man:

install-pdf: install-pdf-am

install-pdf-am:

install-ps: install-ps-am

install-ps-am:

installcheck-am:

maintainer-clean: maintainer-clean-am
		-rm -f ./$(DEPDIR)/test_back.Po
	-rm -f ./$(DEPDIR)/test_options.Po
	-rm -f ./$(DEPDIR)/test_regset.Po
	-rm -f ./$(DEPDIR)/test_syntax.Po
	-rm -f ./$(DEPDIR)/test_utf8.Po
	-rm -f ./$(DEPDIR)/testc.Po
	-rm -f ./$(DEPDIR)/testp.Po
	-rm -f ./$(DEPDIR)/testu.Po
	-rm -f Makefile
maintainer-clean-am: distclean-am maintainer-clean-generic

mostlyclean: mostlyclean-am

mostlyclean-am: mostlyclean-compile mostlyclean-generic \
	mostlyclean-libtool

pdf: pdf-am

pdf-am:

ps: ps-am

ps-am:

uninstall-am:

.MAKE: check-am install-am install-strip

.PHONY: CTAGS GTAGS TAGS all all-am am--depfiles check check-TESTS \
	check-am clean clean-checkPROGRAMS clean-generic clean-libtool \
	cscopelist-am ctags ctags-am distclean distclean-compile \
	distclean-generic distclean-libtool distclean-tags distdir dvi \
	dvi-am html html-am info info-am install install-am \
	install-data install-data-am install-dvi install-dvi-am \
	install-exec install-exec-am install-html install-html-am \
	install-info install-info-am install-man install-pdf \
	install-pdf-am install-ps install-ps-am install-strip \
	installcheck installcheck-am installdirs maintainer-clean \
	maintainer-clean-generic mostlyclean mostlyclean-compile \
	mostlyclean-generic mostlyclean-libtool pdf pdf-am ps ps-am \
	recheck tags tags-am uninstall uninstall-am

.PRECIOUS: Makefile


test: test_uchar test_regex_t $(TESTS)
	@echo "[Oniguruma API, UTF-8 check]"
	@./test_utf8  | grep RESULT
	@echo "[Oniguruma API, SYNTAX check]"
	@./test_syntax  | grep RESULT
	@echo "[Oniguruma API, Options check]"
	@./test_options  | grep RESULT
	@echo "[Oniguruma API, EUC-JP check]"
	@./testc  | grep RESULT
#	@echo "[POSIX API, UTF-8 check]"
#	@./testp  | grep RESULT
	@echo "[Oniguruma API, UTF-16 check]"
	@./testcu | grep RESULT
	@echo ""
	@echo "[Oniguruma API, regset check]"
	@./test_regset
	@echo "[Oniguruma API, backward search check]"
	@./test_back  | grep RESULT

test_uchar:
	@echo "[UChar in oniguruma.h check]"
	@grep "\(^\|[^g]\)UChar" $(top_srcdir)/src/oniguruma.h
	@echo ""

test_regex_t:
	@echo "[regex_t in oniguruma.h check]"
	@grep "\(^\|[^[:alnum:]]\)regex_t" $(top_srcdir)/src/oniguruma.h
	@echo ""

gcov:
	make CFLAGS="--coverage" test_utf8
	make CFLAGS="--coverage" test_syntax
	make CFLAGS="--coverage" test_options
	make CFLAGS="--coverage" testc
#	make CFLAGS="--coverage" testp
	make CFLAGS="--coverage" testcu
	make CFLAGS="--coverage" test_regset
	make CFLAGS="--coverage" test_back

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
