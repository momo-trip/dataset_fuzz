# Makefile.in generated by automake 1.17 from Makefile.am.
# debuginfod/Makefile.  Generated from Makefile.in by configure.

# Copyright (C) 1994-2024 Free Software Foundation, Inc.

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
am__rm_f = rm -f $(am__rm_f_notfound)
am__rm_rf = rm -rf $(am__rm_f_notfound)
pkgdatadir = $(datadir)/elfutils
pkgincludedir = $(includedir)/elfutils
pkglibdir = $(libdir)/elfutils
pkglibexecdir = $(libexecdir)/elfutils
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
am__append_1 = -DSYMBOL_VERSIONING
bin_PROGRAMS = $(am__EXEEXT_1) $(am__EXEEXT_2)
#am__append_2 = debuginfod
#am__append_3 = debuginfod-find
#am__append_4 = -Wno-unused-parameter
subdir = debuginfod
ACLOCAL_M4 = $(top_srcdir)/aclocal.m4
am__aclocal_m4_deps = $(top_srcdir)/m4/ax_cxx_compile_stdcxx.m4 \
	$(top_srcdir)/m4/biarch.m4 $(top_srcdir)/m4/build-to-host.m4 \
	$(top_srcdir)/m4/gettext.m4 $(top_srcdir)/m4/host-cpu-c-abi.m4 \
	$(top_srcdir)/m4/iconv.m4 $(top_srcdir)/m4/intlmacosx.m4 \
	$(top_srcdir)/m4/lib-ld.m4 $(top_srcdir)/m4/lib-link.m4 \
	$(top_srcdir)/m4/lib-prefix.m4 $(top_srcdir)/m4/nls.m4 \
	$(top_srcdir)/m4/po.m4 $(top_srcdir)/m4/progtest.m4 \
	$(top_srcdir)/m4/zip.m4 $(top_srcdir)/configure.ac
am__configure_deps = $(am__aclocal_m4_deps) $(CONFIGURE_DEPENDENCIES) \
	$(ACLOCAL_M4)
DIST_COMMON = $(srcdir)/Makefile.am $(am__pkginclude_HEADERS_DIST) \
	$(am__DIST_COMMON)
mkinstalldirs = $(install_sh) -d
CONFIG_HEADER = $(top_builddir)/config.h
CONFIG_CLEAN_FILES = debuginfod.h
CONFIG_CLEAN_VPATH_FILES =
#am__EXEEXT_1 = debuginfod$(EXEEXT)
#am__EXEEXT_2 = debuginfod-find$(EXEEXT)
am__installdirs = "$(DESTDIR)$(bindir)" "$(DESTDIR)$(pkgincludedir)"
PROGRAMS = $(bin_PROGRAMS)
LIBRARIES = $(noinst_LIBRARIES)
AM_V_AR = $(am__v_AR_$(V))
am__v_AR_ = $(am__v_AR_$(AM_DEFAULT_VERBOSITY))
am__v_AR_0 = @echo "  AR      " $@;
am__v_AR_1 = 
libdebuginfod_a_AR = $(AR) $(ARFLAGS)
libdebuginfod_a_RANLIB = $(RANLIB)
libdebuginfod_a_LIBADD =
am_libdebuginfod_a_OBJECTS = debuginfod-client.$(OBJEXT)
libdebuginfod_a_OBJECTS = $(am_libdebuginfod_a_OBJECTS)
libdebuginfod_pic_a_AR = $(AR) $(ARFLAGS)
libdebuginfod_pic_a_RANLIB = $(RANLIB)
libdebuginfod_pic_a_LIBADD =
libdebuginfod_pic_a_OBJECTS = $(am_libdebuginfod_pic_a_OBJECTS)
am_debuginfod_OBJECTS = debuginfod.$(OBJEXT)
debuginfod_OBJECTS = $(am_debuginfod_OBJECTS)
am__DEPENDENCIES_1 =
am__DEPENDENCIES_2 = ../libelf/libelf.so
#am__DEPENDENCIES_2 = ../libelf/libelf.a
am__DEPENDENCIES_3 = ../libdw/libdw.so
#am__DEPENDENCIES_3 = ../libdw/libdw.a \
#	$(am__DEPENDENCIES_1) $(am__DEPENDENCIES_2) \
#	$(libebl)
am__DEPENDENCIES_4 = ./libdebuginfod.so
#am__DEPENDENCIES_4 = ./libdebuginfod.a \
#	$(am__DEPENDENCIES_1) \
#	$(am__DEPENDENCIES_1) \
#	$(am__DEPENDENCIES_1)
##am__DEPENDENCIES_4 = ./libdebuginfod.a
debuginfod_DEPENDENCIES = $(am__DEPENDENCIES_3) $(am__DEPENDENCIES_2) \
	$(libeu) $(am__DEPENDENCIES_4) $(am__DEPENDENCIES_1) \
	$(am__DEPENDENCIES_1) $(am__DEPENDENCIES_1) \
	$(am__DEPENDENCIES_1) $(am__DEPENDENCIES_1) \
	$(am__DEPENDENCIES_1) $(am__DEPENDENCIES_1) \
	$(am__DEPENDENCIES_1) $(am__DEPENDENCIES_1)
am_debuginfod_find_OBJECTS = debuginfod-find.$(OBJEXT)
debuginfod_find_OBJECTS = $(am_debuginfod_find_OBJECTS)
debuginfod_find_DEPENDENCIES = $(am__DEPENDENCIES_3) \
	$(am__DEPENDENCIES_2) $(libeu) $(am__DEPENDENCIES_4) \
	$(am__DEPENDENCIES_1) $(am__DEPENDENCIES_1) \
	$(am__DEPENDENCIES_1)
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
DEFAULT_INCLUDES = -I. -I$(top_builddir)
depcomp = $(SHELL) $(top_srcdir)/config/depcomp
am__maybe_remake_depfiles = depfiles
am__depfiles_remade = ./$(DEPDIR)/debuginfod-client.Po \
	./$(DEPDIR)/debuginfod-find.Po ./$(DEPDIR)/debuginfod.Po
am__mv = mv -f
COMPILE = $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) \
	$(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
AM_V_CC = $(am__v_CC_$(V))
am__v_CC_ = $(am__v_CC_$(AM_DEFAULT_VERBOSITY))
am__v_CC_0 = @echo "  CC      " $@;
am__v_CC_1 = 
CCLD = $(CC)
LINK = $(CCLD) $(AM_CFLAGS) $(CFLAGS) $(AM_LDFLAGS) $(LDFLAGS) -o $@
AM_V_CCLD = $(am__v_CCLD_$(V))
am__v_CCLD_ = $(am__v_CCLD_$(AM_DEFAULT_VERBOSITY))
am__v_CCLD_0 = @echo "  CCLD    " $@;
am__v_CCLD_1 = 
CXXCOMPILE = $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)
AM_V_CXX = $(am__v_CXX_$(V))
am__v_CXX_ = $(am__v_CXX_$(AM_DEFAULT_VERBOSITY))
am__v_CXX_0 = @echo "  CXX     " $@;
am__v_CXX_1 = 
CXXLD = $(CXX)
CXXLINK = $(CXXLD) $(AM_CXXFLAGS) $(CXXFLAGS) $(AM_LDFLAGS) $(LDFLAGS) \
	-o $@
AM_V_CXXLD = $(am__v_CXXLD_$(V))
am__v_CXXLD_ = $(am__v_CXXLD_$(AM_DEFAULT_VERBOSITY))
am__v_CXXLD_0 = @echo "  CXXLD   " $@;
am__v_CXXLD_1 = 
SOURCES = $(libdebuginfod_a_SOURCES) $(libdebuginfod_pic_a_SOURCES) \
	$(debuginfod_SOURCES) $(debuginfod_find_SOURCES)
DIST_SOURCES = $(libdebuginfod_a_SOURCES) \
	$(libdebuginfod_pic_a_SOURCES) $(debuginfod_SOURCES) \
	$(debuginfod_find_SOURCES)
am__can_run_installinfo = \
  case $$AM_UPDATE_INFO_DIR in \
    n|no|NO) false;; \
    *) (install-info --version) >/dev/null 2>&1;; \
  esac
am__pkginclude_HEADERS_DIST = debuginfod.h
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
  { test ! -d "$$dir" && test ! -f "$$dir" && test ! -r "$$dir"; } \
  || { echo " ( cd '$$dir' && rm -f" $$files ")"; \
       $(am__cd) "$$dir" && echo $$files | $(am__xargs_n) 40 $(am__rm_f); }; \
  }
HEADERS = $(pkginclude_HEADERS)
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
am__DIST_COMMON = $(srcdir)/Makefile.in $(srcdir)/debuginfod.h.in \
	$(top_srcdir)/config/depcomp $(top_srcdir)/config/eu.am \
	ChangeLog
DISTFILES = $(DIST_COMMON) $(DIST_SOURCES) $(TEXINFOS) $(EXTRA_DIST)
ACLOCAL = ${SHELL} '/root/dataset_fuzz/elfutils-0.193/config/missing' aclocal-1.17
AMTAR = $${TAR-tar}
AM_DEFAULT_VERBOSITY = 0
AR = ar
AUTOCONF = ${SHELL} '/root/dataset_fuzz/elfutils-0.193/config/missing' autoconf
AUTOHEADER = ${SHELL} '/root/dataset_fuzz/elfutils-0.193/config/missing' autoheader
AUTOMAKE = ${SHELL} '/root/dataset_fuzz/elfutils-0.193/config/missing' automake-1.17
AWK = gawk
BZ2_LIB = -lbz2 
CC = afl-clang-fast
CCDEPMODE = depmode=gcc3
CC_BIARCH = afl-clang-fast -m32
CFLAGS = -D_FORTIFY_SOURCE=3 -fsanitize=address -g -O1 -fno-omit-frame-pointer -fno-addrsig
CPP = afl-clang-fast -E
CPPFLAGS = 
CSCOPE = cscope
CTAGS = ctags
CXX = afl-clang-fast++ -std=c++11
CXXDEPMODE = depmode=gcc3
CXXFLAGS = -D_FORTIFY_SOURCE=3 -fsanitize=address -g -O1 -fno-omit-frame-pointer
CYGPATH_W = echo
DEBUGINFOD_IMA_CERT_PATH = 
DEBUGINFOD_URLS = 
DEBUGPRED = 0
DEFS = -D_GNU_SOURCE -DHAVE_CONFIG_H -DLOCALEDIR='"${localedir}"'
DEPDIR = .deps
ECHO_C = 
ECHO_N = -n
ECHO_T = 
ETAGS = etags
EXEEXT = 
GCOV = 
GENHTML = 
GETTEXT_MACRO_VERSION = 0.22
GMSGFMT = /usr/bin/msgfmt
GMSGFMT_015 = /usr/bin/msgfmt
HAVE_BISON = yes
HAVE_BUNZIP2 = yes
HAVE_CXX11 = yes
HAVE_FLEX = yes
HAVE_GAWK = yes
HAVE_VALGRIND = 
HAVE_ZSTD = no
INSTALL = /usr/bin/install -c
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_PROGRAM = ${INSTALL}
INSTALL_SCRIPT = ${INSTALL}
INSTALL_STRIP_PROGRAM = $(install_sh) -c -s
INTLLIBS = 
INTL_MACOSX_LIBS = 
LCOV = 
LDFLAGS = -fsanitize=address
LEX = flex
LEXLIB = 
LEX_OUTPUT_ROOT = lex.yy
LIBDEBUGINFOD_SONAME = libdebuginfod.so.1
LIBICONV = -liconv
LIBINTL = 
LIBLZMA = liblzma
LIBOBJS = 
LIBS = -lbz2 
LIBZSTD = libzstd
LTLIBICONV = -liconv
LTLIBINTL = 
LTLIBOBJS = 
MAINT = 
MAKEINFO = ${SHELL} '/root/dataset_fuzz/elfutils-0.193/config/missing' makeinfo
MKDIR_P = /usr/bin/mkdir -p
MSGFMT = /usr/bin/msgfmt
MSGMERGE = /usr/bin/msgmerge
MSGMERGE_FOR_MSGFMT_OPTION = --for-msgfmt
NM = nm
NO_UNDEFINED = -Wl,--no-undefined
OBJEXT = o
PACKAGE = elfutils
PACKAGE_BUGREPORT = https://sourceware.org/bugzilla
PACKAGE_NAME = elfutils
PACKAGE_STRING = elfutils 0.193
PACKAGE_TARNAME = elfutils
PACKAGE_URL = http://elfutils.org/
PACKAGE_VERSION = 0.193
PATH_SEPARATOR = :
PKG_CONFIG = /usr/bin/pkg-config
PKG_CONFIG_LIBDIR = 
PKG_CONFIG_PATH = 
POSUB = po
RANLIB = ranlib
READELF = readelf
SED = /usr/bin/sed
SET_MAKE = 
SHELL = /bin/bash
STRIP = 
USE_NLS = yes
VERSION = 0.193
XGETTEXT = /usr/bin/xgettext
XGETTEXT_015 = /usr/bin/xgettext
XGETTEXT_EXTRA_OPTIONS = 
YACC = bison -y
YFLAGS = 
ZSTD_COMPRESS_CFLAGS = 
ZSTD_COMPRESS_LIBS = -lzstd
abs_builddir = /root/dataset_fuzz/elfutils-0.193/debuginfod
abs_srcdir = /root/dataset_fuzz/elfutils-0.193/debuginfod
abs_top_builddir = /root/dataset_fuzz/elfutils-0.193
abs_top_srcdir = /root/dataset_fuzz/elfutils-0.193
ac_ct_AR = ar
ac_ct_CC = afl-clang-fast
ac_ct_CXX = 
am__include = include
am__leading_dot = .
am__quote = 
am__rm_f_notfound = 
am__tar = $${TAR-tar} chof - "$$tardir"
am__untar = $${TAR-tar} xf -
am__xargs_n = xargs -n
argp_LDADD = 
bindir = ${exec_prefix}/bin
build = x86_64-pc-linux-gnu
build_alias = 
build_cpu = x86_64
build_os = linux-gnu
build_vendor = pc
builddir = .
crypto_LIBS = 
datadir = ${datarootdir}
datarootdir = ${prefix}/share
docdir = ${datarootdir}/doc/${PACKAGE_TARNAME}
dso_LDFLAGS = -shared -Wl,-z,defs -Wl,-z,relro
dvidir = ${docdir}
eu_version = 193
exec_prefix = ${prefix}
fpic_CFLAGS = -fPIC
fpie_CFLAGS = -fPIE
fts_LIBS = 
host = x86_64-pc-linux-gnu
host_alias = 
host_cpu = x86_64
host_os = linux-gnu
host_vendor = pc
htmldir = ${docdir}
includedir = ${prefix}/include
infodir = ${datarootdir}/info
install_sh = ${SHELL} /root/dataset_fuzz/elfutils-0.193/config/install-sh
jsonc_CFLAGS = 
jsonc_LIBS = 
libarchive_CFLAGS = 
libarchive_LIBS = 
libcurl_CFLAGS = -I/usr/include/x86_64-linux-gnu
libcurl_LIBS = -lcurl
libdir = ${exec_prefix}/lib
libexecdir = ${exec_prefix}/libexec
libmicrohttpd_CFLAGS = 
libmicrohttpd_LIBS = 
localedir = ${datarootdir}/locale
localedir_c = "/usr/local/share/locale"
localedir_c_make = \"$(localedir)\"
localstatedir = ${prefix}/var
lzma_LIBS = -llzma 
mandir = ${datarootdir}/man
mkdir_p = $(MKDIR_P)
obstack_LIBS = 
oldincludedir = /usr/include
oldlibmicrohttpd_CFLAGS = 
oldlibmicrohttpd_LIBS = 
pdfdir = ${docdir}
prefix = /usr/local
program_transform_name = s,x,x,
psdir = ${docdir}
rpm_LIBS = 
runstatedir = ${localstatedir}/run
sbindir = ${exec_prefix}/sbin
sharedstatedir = ${prefix}/com
sqlite3_CFLAGS = 
sqlite3_LIBS = 
srcdir = .
sysconfdir = ${prefix}/etc
target_alias = 
top_build_prefix = ../
top_builddir = ..
top_srcdir = ..
zip_LIBS = -lzstd -llzma  -lbz2 
zstd_LIBS = -lzstd -llzma  -lbz2 
AM_CPPFLAGS = -iquote . -I$(srcdir) -I$(top_srcdir)/lib -I.. \
	-I$(srcdir) -I$(srcdir)/../libelf -I$(srcdir)/../libebl \
	-I$(srcdir)/../libdw -I$(srcdir)/../libdwelf \
	$(libmicrohttpd_CFLAGS) $(libcurl_CFLAGS) $(sqlite3_CFLAGS) \
	$(libarchive_CFLAGS) $(jsonc_CFLAGS) $(am__append_4)

# Drop the 'u' flag that automake adds by default. It is incompatible
# with deterministic archives.
ARFLAGS = cr
STACK_USAGE_WARNING = 

# Warn about stack usage of more than 256K = 262144 bytes.
#STACK_USAGE_WARNING = -Wstack-usage=262144
STACK_USAGE_NO_ERROR = 
#STACK_USAGE_NO_ERROR = -Wno-error=stack-usage=
LOGICAL_OP_WARNING = 
#LOGICAL_OP_WARNING = -Wlogical-op
DUPLICATED_COND_WARNING = 
#DUPLICATED_COND_WARNING = -Wduplicated-cond
#NULL_DEREFERENCE_WARNING = 
NULL_DEREFERENCE_WARNING = -Wnull-dereference
IMPLICIT_FALLTHROUGH_WARNING = -Wimplicit-fallthrough

# Use strict fallthrough. Only __attribute__((fallthrough)) will prevent the
# warning
#IMPLICIT_FALLTHROUGH_WARNING = -Wimplicit-fallthrough=5
#IMPLICIT_FALLTHROUGH_WARNING = 
TRAMPOLINES_WARNING = 
#TRAMPOLINES_WARNING = -Wtrampolines
NO_PACKED_NOT_ALIGNED_WARNING = 
#NO_PACKED_NOT_ALIGNED_WARNING = -Wno-packed-not-aligned
USE_AFTER_FREE3_WARNING = 
#USE_AFTER_FREE3_WARNING = -Wuse-after-free=3
AM_CFLAGS = -Wall -Wshadow -Wformat=2 \
	    -Wold-style-definition -Wstrict-prototypes $(TRAMPOLINES_WARNING) \
	    $(LOGICAL_OP_WARNING) $(DUPLICATED_COND_WARNING) \
	    $(NULL_DEREFERENCE_WARNING) $(IMPLICIT_FALLTHROUGH_WARNING) \
	    $(USE_AFTER_FREE3_WARNING) \
	    $(if $($(*F)_no_Werror),,-Werror) \
	    $(if $($(*F)_no_Wunused),,-Wunused -Wextra) \
	    $(if $($(*F)_no_Wstack_usage),,$(STACK_USAGE_WARNING)) \
	    $(if $($(*F)_no_Wpacked_not_aligned),$(NO_PACKED_NOT_ALIGNED_WARNING),) \
	    $($(*F)_CFLAGS)

AM_CXXFLAGS = -std=c++11 -Wall -Wshadow \
	   $(TRAMPOLINES_WARNING) \
	   $(LOGICAL_OP_WARNING) $(DUPLICATED_COND_WARNING) \
	   $(NULL_DEREFERENCE_WARNING) $(IMPLICIT_FALLTHROUGH_WARNING) \
	   $(if $($(*F)_no_Werror),,-Werror) \
	   $(if $($(*F)_no_Wunused),,-Wunused -Wextra) \
	   $(if $($(*F)_no_Wstack_usage),,$(STACK_USAGE_WARNING)) \
	   $(if $($(*F)_no_Wpacked_not_aligned),$(NO_PACKED_NOT_ALIGNED_WARNING),) \
	   $($(*F)_CXXFLAGS)

COMPILE.os = $(filter-out -fprofile-arcs -ftest-coverage, $(COMPILE))
DEFS.os = -DPIC -DSHARED $(am__append_1)
COVERAGE_OUTPUT_DIRECTORY = coverage
COVERAGE_OUTPUT_FILE = $(PACKAGE_NAME).lcov
textrel_msg = echo "WARNING: TEXTREL found in '$@'"
#textrel_found = $(textrel_msg)
textrel_found = $(textrel_msg); exit 1
textrel_check = if $(READELF) -d $@ | grep -F -q TEXTREL; then $(textrel_found); fi

# Disable eu- prefixing for artifacts (binaries & man pages) in this
# directory, since they do not conflict with binutils tools.
program_prefix = 
libasm = ../libasm/libasm.so
#libasm = ../libasm/libasm.a
libdw = ../libdw/libdw.so
#libdw = ../libdw/libdw.a -lz $(zip_LIBS) $(libelf) $(libebl) -ldl -lpthread
libelf = ../libelf/libelf.so
#libelf = ../libelf/libelf.a -lz
libdebuginfod = ./libdebuginfod.so
#libdebuginfod = ./libdebuginfod.a -lpthread $(libcurl_LIBS) $(crypto_LIBS) $(jsonc_LIBS)
##libdebuginfod = ./libdebuginfod.a
libebl = ../libebl/libebl.a
libeu = ../lib/libeu.a
AM_LDFLAGS = -Wl,-rpath-link,../libelf:../libdw:.
debuginfod_SOURCES = debuginfod.cxx
debuginfod_LDADD = $(libdw) $(libelf) $(libeu) $(libdebuginfod) $(argp_LDADD) $(fts_LIBS) $(libmicrohttpd_LIBS) $(sqlite3_LIBS) $(libarchive_LIBS) $(rpm_LIBS) $(jsonc_LIBS) $(libcurl_LIBS) $(lzma_LIBS) -lpthread -ldl
debuginfod_find_SOURCES = debuginfod-find.c
debuginfod_find_LDADD = $(libdw) $(libelf) $(libeu) $(libdebuginfod) $(argp_LDADD) $(fts_LIBS) $(jsonc_LIBS)
#noinst_LIBRARIES = libdebuginfod.a \
#	libdebuginfod_pic.a
libdebuginfod_a_SOURCES = debuginfod-client.c
libdebuginfod_pic_a_SOURCES = debuginfod-client.c
am_libdebuginfod_pic_a_OBJECTS = $(libdebuginfod_a_SOURCES:.c=.os)
#pkginclude_HEADERS = debuginfod.h
#libdebuginfod_so_LIBS = libdebuginfod_pic.a
#libdebuginfod_so_LDLIBS = -lpthread $(libcurl_LIBS) $(fts_LIBS) $(libelf) $(crypto_LIBS) $(jsonc_LIBS)
##libdebuginfod_so_LDLIBS = 
EXTRA_DIST = libdebuginfod.map
EXTRA_libdebuginfod_a_DEPENDENCIES = libdebuginfod.manifest
MOSTLYCLEANFILES = $(am_libdebuginfod_a_OBJECTS) $(am_libdebuginfod_pic_a_OBJECTS) $(LIBDEBUGINFOD_SONAME)
CLEANFILES = libdebuginfod.so $(EXTRA_libdebuginfod_a_DEPENDENCIES)
all: all-am

.SUFFIXES:
.SUFFIXES: .c .cxx .o .obj
$(srcdir)/Makefile.in:  $(srcdir)/Makefile.am $(top_srcdir)/config/eu.am $(am__configure_deps)
	@for dep in $?; do \
	  case '$(am__configure_deps)' in \
	    *$$dep*) \
	      ( cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh ) \
	        && { if test -f $@; then exit 0; else break; fi; }; \
	      exit 1;; \
	  esac; \
	done; \
	echo ' cd $(top_srcdir) && $(AUTOMAKE) --gnits debuginfod/Makefile'; \
	$(am__cd) $(top_srcdir) && \
	  $(AUTOMAKE) --gnits debuginfod/Makefile
Makefile: $(srcdir)/Makefile.in $(top_builddir)/config.status
	@case '$?' in \
	  *config.status*) \
	    cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh;; \
	  *) \
	    echo ' cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@ $(am__maybe_remake_depfiles)'; \
	    cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@ $(am__maybe_remake_depfiles);; \
	esac;
$(top_srcdir)/config/eu.am $(am__empty):

$(top_builddir)/config.status: $(top_srcdir)/configure $(CONFIG_STATUS_DEPENDENCIES)
	cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh

$(top_srcdir)/configure:  $(am__configure_deps)
	cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh
$(ACLOCAL_M4):  $(am__aclocal_m4_deps)
	cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh
$(am__aclocal_m4_deps):
debuginfod.h: $(top_builddir)/config.status $(srcdir)/debuginfod.h.in
	cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@
install-binPROGRAMS: $(bin_PROGRAMS)
	@$(NORMAL_INSTALL)
	@list='$(bin_PROGRAMS)'; test -n "$(bindir)" || list=; \
	if test -n "$$list"; then \
	  echo " $(MKDIR_P) '$(DESTDIR)$(bindir)'"; \
	  $(MKDIR_P) "$(DESTDIR)$(bindir)" || exit 1; \
	fi; \
	for p in $$list; do echo "$$p $$p"; done | \
	sed 's/$(EXEEXT)$$//' | \
	while read p p1; do if test -f $$p \
	  ; then echo "$$p"; echo "$$p"; else :; fi; \
	done | \
	sed -e 'p;s,.*/,,;n;h' \
	    -e 's|.*|.|' \
	    -e 'p;x;s,.*/,,;s/$(EXEEXT)$$//;$(transform);s/$$/$(EXEEXT)/' | \
	sed 'N;N;N;s,\n, ,g' | \
	$(AWK) 'BEGIN { files["."] = ""; dirs["."] = 1 } \
	  { d=$$3; if (dirs[d] != 1) { print "d", d; dirs[d] = 1 } \
	    if ($$2 == $$4) files[d] = files[d] " " $$1; \
	    else { print "f", $$3 "/" $$4, $$1; } } \
	  END { for (d in files) print "f", d, files[d] }' | \
	while read type dir files; do \
	    if test "$$dir" = .; then dir=; else dir=/$$dir; fi; \
	    test -z "$$files" || { \
	      echo " $(INSTALL_PROGRAM_ENV) $(INSTALL_PROGRAM) $$files '$(DESTDIR)$(bindir)$$dir'"; \
	      $(INSTALL_PROGRAM_ENV) $(INSTALL_PROGRAM) $$files "$(DESTDIR)$(bindir)$$dir" || exit $$?; \
	    } \
	; done

uninstall-binPROGRAMS:
	@$(NORMAL_UNINSTALL)
	@list='$(bin_PROGRAMS)'; test -n "$(bindir)" || list=; \
	files=`for p in $$list; do echo "$$p"; done | \
	  sed -e 'h;s,^.*/,,;s/$(EXEEXT)$$//;$(transform)' \
	      -e 's/$$/$(EXEEXT)/' \
	`; \
	test -n "$$list" || exit 0; \
	echo " ( cd '$(DESTDIR)$(bindir)' && rm -f" $$files ")"; \
	cd "$(DESTDIR)$(bindir)" && $(am__rm_f) $$files

clean-binPROGRAMS:
	-$(am__rm_f) $(bin_PROGRAMS)

clean-noinstLIBRARIES:
	-$(am__rm_f) $(noinst_LIBRARIES)

libdebuginfod.a: $(libdebuginfod_a_OBJECTS) $(libdebuginfod_a_DEPENDENCIES) $(EXTRA_libdebuginfod_a_DEPENDENCIES) 
	$(AM_V_at)-rm -f libdebuginfod.a
	$(AM_V_AR)$(libdebuginfod_a_AR) libdebuginfod.a $(libdebuginfod_a_OBJECTS) $(libdebuginfod_a_LIBADD)
	$(AM_V_at)$(libdebuginfod_a_RANLIB) libdebuginfod.a

libdebuginfod_pic.a: $(libdebuginfod_pic_a_OBJECTS) $(libdebuginfod_pic_a_DEPENDENCIES) $(EXTRA_libdebuginfod_pic_a_DEPENDENCIES) 
	$(AM_V_at)-rm -f libdebuginfod_pic.a
	$(AM_V_AR)$(libdebuginfod_pic_a_AR) libdebuginfod_pic.a $(libdebuginfod_pic_a_OBJECTS) $(libdebuginfod_pic_a_LIBADD)
	$(AM_V_at)$(libdebuginfod_pic_a_RANLIB) libdebuginfod_pic.a

debuginfod$(EXEEXT): $(debuginfod_OBJECTS) $(debuginfod_DEPENDENCIES) $(EXTRA_debuginfod_DEPENDENCIES) 
	@rm -f debuginfod$(EXEEXT)
	$(AM_V_CXXLD)$(CXXLINK) $(debuginfod_OBJECTS) $(debuginfod_LDADD) $(LIBS)

debuginfod-find$(EXEEXT): $(debuginfod_find_OBJECTS) $(debuginfod_find_DEPENDENCIES) $(EXTRA_debuginfod_find_DEPENDENCIES) 
	@rm -f debuginfod-find$(EXEEXT)
	$(AM_V_CCLD)$(LINK) $(debuginfod_find_OBJECTS) $(debuginfod_find_LDADD) $(LIBS)

mostlyclean-compile:
	-rm -f *.$(OBJEXT)

distclean-compile:
	-rm -f *.tab.c

include ./$(DEPDIR)/debuginfod-client.Po # am--include-marker
include ./$(DEPDIR)/debuginfod-find.Po # am--include-marker
include ./$(DEPDIR)/debuginfod.Po # am--include-marker

$(am__depfiles_remade):
	@$(MKDIR_P) $(@D)
	@: >>$@

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

.cxx.o:
	$(AM_V_CXX)$(CXXCOMPILE) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ $<
	$(AM_V_at)$(am__mv) $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Po
#	$(AM_V_CXX)source='$<' object='$@' libtool=no \
#	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
#	$(AM_V_CXX_no)$(CXXCOMPILE) -c -o $@ $<

.cxx.obj:
	$(AM_V_CXX)$(CXXCOMPILE) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ `$(CYGPATH_W) '$<'`
	$(AM_V_at)$(am__mv) $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Po
#	$(AM_V_CXX)source='$<' object='$@' libtool=no \
#	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
#	$(AM_V_CXX_no)$(CXXCOMPILE) -c -o $@ `$(CYGPATH_W) '$<'`
install-pkgincludeHEADERS: $(pkginclude_HEADERS)
	@$(NORMAL_INSTALL)
	@list='$(pkginclude_HEADERS)'; test -n "$(pkgincludedir)" || list=; \
	if test -n "$$list"; then \
	  echo " $(MKDIR_P) '$(DESTDIR)$(pkgincludedir)'"; \
	  $(MKDIR_P) "$(DESTDIR)$(pkgincludedir)" || exit 1; \
	fi; \
	for p in $$list; do \
	  if test -f "$$p"; then d=; else d="$(srcdir)/"; fi; \
	  echo "$$d$$p"; \
	done | $(am__base_list) | \
	while read files; do \
	  echo " $(INSTALL_HEADER) $$files '$(DESTDIR)$(pkgincludedir)'"; \
	  $(INSTALL_HEADER) $$files "$(DESTDIR)$(pkgincludedir)" || exit $$?; \
	done

uninstall-pkgincludeHEADERS:
	@$(NORMAL_UNINSTALL)
	@list='$(pkginclude_HEADERS)'; test -n "$(pkgincludedir)" || list=; \
	files=`for p in $$list; do echo $$p; done | sed -e 's|^.*/||'`; \
	dir='$(DESTDIR)$(pkgincludedir)'; $(am__uninstall_files_from_dir)

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
check: check-am
all-am: Makefile $(PROGRAMS) $(LIBRARIES) $(HEADERS)
installdirs:
	for dir in "$(DESTDIR)$(bindir)" "$(DESTDIR)$(pkgincludedir)"; do \
	  test -z "$$dir" || $(MKDIR_P) "$$dir"; \
	done
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
	-$(am__rm_f) $(MOSTLYCLEANFILES)

clean-generic:
	-$(am__rm_f) $(CLEANFILES)

distclean-generic:
	-$(am__rm_f) $(CONFIG_CLEAN_FILES)
	-test . = "$(srcdir)" || $(am__rm_f) $(CONFIG_CLEAN_VPATH_FILES)

maintainer-clean-generic:
	@echo "This command is intended for maintainers to use"
	@echo "it deletes files that may require special tools to rebuild."
clean: clean-am

clean-am: clean-binPROGRAMS clean-generic clean-local \
	clean-noinstLIBRARIES mostlyclean-am

distclean: distclean-am
	-rm -f ./$(DEPDIR)/debuginfod-client.Po
	-rm -f ./$(DEPDIR)/debuginfod-find.Po
	-rm -f ./$(DEPDIR)/debuginfod.Po
	-rm -f Makefile
distclean-am: clean-am distclean-compile distclean-generic \
	distclean-tags

dvi: dvi-am

dvi-am:

html: html-am

html-am:

info: info-am

info-am:

install-data-am: install-pkgincludeHEADERS

install-dvi: install-dvi-am

install-dvi-am:

install-exec-am: install-binPROGRAMS

install-html: install-html-am

install-html-am:

install-info: install-info-am

install-info-am:

install-man:

install-pdf: install-pdf-am

install-pdf-am:

install-ps: install-ps-am

install-ps-am:

installcheck-am: installcheck-binPROGRAMS

maintainer-clean: maintainer-clean-am
	-rm -f ./$(DEPDIR)/debuginfod-client.Po
	-rm -f ./$(DEPDIR)/debuginfod-find.Po
	-rm -f ./$(DEPDIR)/debuginfod.Po
	-rm -f Makefile
maintainer-clean-am: distclean-am maintainer-clean-generic

mostlyclean: mostlyclean-am

mostlyclean-am: mostlyclean-compile mostlyclean-generic

pdf: pdf-am

pdf-am:

ps: ps-am

ps-am:

uninstall-am: uninstall-binPROGRAMS uninstall-pkgincludeHEADERS

.MAKE: install-am install-strip

.PHONY: CTAGS GTAGS TAGS all all-am am--depfiles check check-am clean \
	clean-binPROGRAMS clean-generic clean-local \
	clean-noinstLIBRARIES cscopelist-am ctags ctags-am distclean \
	distclean-compile distclean-generic distclean-tags distdir dvi \
	dvi-am html html-am info info-am install install-am \
	install-binPROGRAMS install-data install-data-am install-dvi \
	install-dvi-am install-exec install-exec-am install-html \
	install-html-am install-info install-info-am install-man \
	install-pdf install-pdf-am install-pkgincludeHEADERS \
	install-ps install-ps-am install-strip installcheck \
	installcheck-am installcheck-binPROGRAMS installdirs \
	maintainer-clean maintainer-clean-generic mostlyclean \
	mostlyclean-compile mostlyclean-generic pdf pdf-am ps ps-am \
	tags tags-am uninstall uninstall-am uninstall-binPROGRAMS \
	uninstall-pkgincludeHEADERS

.PRECIOUS: Makefile


%.os: %.c %.o
	$(AM_V_CC)if $(COMPILE.os) -c -o $@ $(fpic_CFLAGS) $(DEFS.os) -MT $@ -MD -MP \
	  -MF "$(DEPDIR)/$*.Tpo" `test -f '$<' || echo '$(srcdir)/'`$<; \
	then cat "$(DEPDIR)/$*.Tpo" >> "$(DEPDIR)/$*.Po"; \
	     rm -f "$(DEPDIR)/$*.Tpo"; \
	else rm -f "$(DEPDIR)/$*.Tpo"; exit 1; \
	fi
#	$(AM_V_CC)$(COMPILE.os) -c -o $@ $(fpic_CFLAGS) $(DEFS.os) $<

.PHONY: clean-coverage coverage

clean-local: clean-coverage
clean-coverage:
	-rm -rf $(COVERAGE_OUTPUT_DIRECTORY)
	-rm -f $(COVERAGE_OUTPUT_FILE) *.gcno *.gcda

print-%:
	@echo $*=$($*)
#$(LIBDEBUGINFOD_SONAME): $(srcdir)/libdebuginfod.map $(libdebuginfod_so_LIBS)
#	$(AM_V_CCLD)$(LINK) $(dso_LDFLAGS) -o $@ \
#		-Wl,--soname,$(LIBDEBUGINFOD_SONAME) \
#		-Wl,--version-script,$< \
#		$(NO_UNDEFINED) \
#		-Wl,--whole-archive $(libdebuginfod_so_LIBS) -Wl,--no-whole-archive \
#		$(libdebuginfod_so_LDLIBS)
#	@$(textrel_check)

#libdebuginfod.so: $(LIBDEBUGINFOD_SONAME)
#	ln -fs $< $@

#install: install-am libdebuginfod.so
#	$(mkinstalldirs) $(DESTDIR)$(libdir)
#	$(INSTALL_PROGRAM) $(LIBDEBUGINFOD_SONAME) \
#		$(DESTDIR)$(libdir)/libdebuginfod-$(PACKAGE_VERSION).so
#	ln -fs libdebuginfod-$(PACKAGE_VERSION).so $(DESTDIR)$(libdir)/$(LIBDEBUGINFOD_SONAME)
#	ln -fs libdebuginfod-$(PACKAGE_VERSION).so $(DESTDIR)$(libdir)/libdebuginfod.so
#uninstall: uninstall-am
#	rm -f $(DESTDIR)$(libdir)/libdebuginfod-$(PACKAGE_VERSION).so
#	rm -f $(DESTDIR)$(libdir)/$(LIBDEBUGINFOD_SONAME)
#	rm -f $(DESTDIR)$(libdir)/libdebuginfod.so
#	rmdir --ignore-fail-on-non-empty $(DESTDIR)$(includedir)/elfutils

libdebuginfod.manifest: $(libdebuginfod_a_OBJECTS)
	$(AM_V_GEN)echo $^ > $@

# automake std-options override: arrange to pass LD_LIBRARY_PATH
installcheck-binPROGRAMS: $(bin_PROGRAMS)
	bad=0; pid=$$$$; list="$(bin_PROGRAMS)"; for p in $$list; do \
	  case ' $(AM_INSTALLCHECK_STD_OPTIONS_EXEMPT) ' in \
	   *" $$p "* | *" $(srcdir)/$$p "*) continue;; \
	  esac; \
	  f=`echo "$$p" | \
	     sed 's,^.*/,,;s/$(EXEEXT)$$//;$(transform);s/$$/$(EXEEXT)/'`; \
	  for opt in --help --version; do \
	    if LD_LIBRARY_PATH=$(DESTDIR)$(libdir) \
	       $(DESTDIR)$(bindir)/$$f $$opt > c$${pid}_.out 2> c$${pid}_.err \
		 && test -n "`cat c$${pid}_.out`" \
		 && test -z "`cat c$${pid}_.err`"; then :; \
	    else echo "$$f does not support $$opt" 1>&2; bad=1; fi; \
	  done; \
	done; rm -f c$${pid}_.???; exit $$bad

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:

# Tell GNU make to disable its built-in pattern rules.
%:: %,v
%:: RCS/%,v
%:: RCS/%
%:: s.%
%:: SCCS/s.%
