# Makefile.in generated by automake 1.16.5 from Makefile.am.
# src/Makefile.  Generated from Makefile.in by configure.

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
#am__append_1 = $(posix_headers)
#am__append_2 = -DUSE_POSIX_API
#am__append_3 = -DUSE_BINARY_COMPATIBLE_POSIX_API
#am__append_4 = -Wl,--output-def,$(LIBONIG_DEF_FILE)
subdir = src
ACLOCAL_M4 = $(top_srcdir)/aclocal.m4
am__aclocal_m4_deps = $(top_srcdir)/m4/libtool.m4 \
	$(top_srcdir)/m4/ltoptions.m4 $(top_srcdir)/m4/ltsugar.m4 \
	$(top_srcdir)/m4/ltversion.m4 $(top_srcdir)/m4/lt~obsolete.m4 \
	$(top_srcdir)/configure.ac
am__configure_deps = $(am__aclocal_m4_deps) $(CONFIGURE_DEPENDENCIES) \
	$(ACLOCAL_M4)
DIST_COMMON = $(srcdir)/Makefile.am $(am__include_HEADERS_DIST) \
	$(am__DIST_COMMON)
mkinstalldirs = $(install_sh) -d
CONFIG_HEADER = config.h
CONFIG_CLEAN_FILES =
CONFIG_CLEAN_VPATH_FILES =
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
am__installdirs = "$(DESTDIR)$(libdir)" "$(DESTDIR)$(includedir)"
LTLIBRARIES = $(lib_LTLIBRARIES)
libonig_la_LIBADD =
am__libonig_la_SOURCES_DIST = regint.h regparse.h regenc.h st.h \
	regparse.c regcomp.c regexec.c regenc.c regerror.c regext.c \
	regsyntax.c regtrav.c regversion.c st.c reggnu.c regposix.c \
	regposerr.c unicode.c unicode_unfold_key.c unicode_fold1_key.c \
	unicode_fold2_key.c unicode_fold3_key.c ascii.c utf8.c \
	utf16_be.c utf16_le.c utf32_be.c utf32_le.c euc_jp.c \
	euc_jp_prop.c sjis.c sjis_prop.c iso8859_1.c iso8859_2.c \
	iso8859_3.c iso8859_4.c iso8859_5.c iso8859_6.c iso8859_7.c \
	iso8859_8.c iso8859_9.c iso8859_10.c iso8859_11.c iso8859_13.c \
	iso8859_14.c iso8859_15.c iso8859_16.c euc_tw.c euc_kr.c \
	big5.c gb18030.c koi8_r.c cp1251.c onig_init.c
#am__objects_1 = regposix.lo regposerr.lo
am_libonig_la_OBJECTS = regparse.lo regcomp.lo regexec.lo regenc.lo \
	regerror.lo regext.lo regsyntax.lo regtrav.lo regversion.lo \
	st.lo reggnu.lo $(am__objects_1) unicode.lo \
	unicode_unfold_key.lo unicode_fold1_key.lo \
	unicode_fold2_key.lo unicode_fold3_key.lo ascii.lo utf8.lo \
	utf16_be.lo utf16_le.lo utf32_be.lo utf32_le.lo euc_jp.lo \
	euc_jp_prop.lo sjis.lo sjis_prop.lo iso8859_1.lo iso8859_2.lo \
	iso8859_3.lo iso8859_4.lo iso8859_5.lo iso8859_6.lo \
	iso8859_7.lo iso8859_8.lo iso8859_9.lo iso8859_10.lo \
	iso8859_11.lo iso8859_13.lo iso8859_14.lo iso8859_15.lo \
	iso8859_16.lo euc_tw.lo euc_kr.lo big5.lo gb18030.lo koi8_r.lo \
	cp1251.lo onig_init.lo
libonig_la_OBJECTS = $(am_libonig_la_OBJECTS)
AM_V_lt = $(am__v_lt_$(V))
am__v_lt_ = $(am__v_lt_$(AM_DEFAULT_VERBOSITY))
am__v_lt_0 = --silent
am__v_lt_1 = 
libonig_la_LINK = $(LIBTOOL) $(AM_V_lt) --tag=CC $(AM_LIBTOOLFLAGS) \
	$(LIBTOOLFLAGS) --mode=link $(CCLD) $(AM_CFLAGS) $(CFLAGS) \
	$(libonig_la_LDFLAGS) $(LDFLAGS) -o $@
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
DEFAULT_INCLUDES = -I.
depcomp = $(SHELL) $(top_srcdir)/depcomp
am__maybe_remake_depfiles = depfiles
am__depfiles_remade = ./$(DEPDIR)/ascii.Plo ./$(DEPDIR)/big5.Plo \
	./$(DEPDIR)/cp1251.Plo ./$(DEPDIR)/euc_jp.Plo \
	./$(DEPDIR)/euc_jp_prop.Plo ./$(DEPDIR)/euc_kr.Plo \
	./$(DEPDIR)/euc_tw.Plo ./$(DEPDIR)/gb18030.Plo \
	./$(DEPDIR)/iso8859_1.Plo ./$(DEPDIR)/iso8859_10.Plo \
	./$(DEPDIR)/iso8859_11.Plo ./$(DEPDIR)/iso8859_13.Plo \
	./$(DEPDIR)/iso8859_14.Plo ./$(DEPDIR)/iso8859_15.Plo \
	./$(DEPDIR)/iso8859_16.Plo ./$(DEPDIR)/iso8859_2.Plo \
	./$(DEPDIR)/iso8859_3.Plo ./$(DEPDIR)/iso8859_4.Plo \
	./$(DEPDIR)/iso8859_5.Plo ./$(DEPDIR)/iso8859_6.Plo \
	./$(DEPDIR)/iso8859_7.Plo ./$(DEPDIR)/iso8859_8.Plo \
	./$(DEPDIR)/iso8859_9.Plo ./$(DEPDIR)/koi8_r.Plo \
	./$(DEPDIR)/onig_init.Plo ./$(DEPDIR)/regcomp.Plo \
	./$(DEPDIR)/regenc.Plo ./$(DEPDIR)/regerror.Plo \
	./$(DEPDIR)/regexec.Plo ./$(DEPDIR)/regext.Plo \
	./$(DEPDIR)/reggnu.Plo ./$(DEPDIR)/regparse.Plo \
	./$(DEPDIR)/regposerr.Plo ./$(DEPDIR)/regposix.Plo \
	./$(DEPDIR)/regsyntax.Plo ./$(DEPDIR)/regtrav.Plo \
	./$(DEPDIR)/regversion.Plo ./$(DEPDIR)/sjis.Plo \
	./$(DEPDIR)/sjis_prop.Plo ./$(DEPDIR)/st.Plo \
	./$(DEPDIR)/unicode.Plo ./$(DEPDIR)/unicode_fold1_key.Plo \
	./$(DEPDIR)/unicode_fold2_key.Plo \
	./$(DEPDIR)/unicode_fold3_key.Plo \
	./$(DEPDIR)/unicode_unfold_key.Plo ./$(DEPDIR)/utf16_be.Plo \
	./$(DEPDIR)/utf16_le.Plo ./$(DEPDIR)/utf32_be.Plo \
	./$(DEPDIR)/utf32_le.Plo ./$(DEPDIR)/utf8.Plo
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
SOURCES = $(libonig_la_SOURCES)
DIST_SOURCES = $(am__libonig_la_SOURCES_DIST)
am__can_run_installinfo = \
  case $$AM_UPDATE_INFO_DIR in \
    n|no|NO) false;; \
    *) (install-info --version) >/dev/null 2>&1;; \
  esac
am__include_HEADERS_DIST = oniguruma.h oniggnu.h onigposix.h
HEADERS = $(include_HEADERS)
am__tagged_files = $(HEADERS) $(SOURCES) $(TAGS_FILES) $(LISP) \
	config.h.in
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
am__DIST_COMMON = $(srcdir)/Makefile.in $(srcdir)/config.h.in \
	$(top_srcdir)/depcomp
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
abs_builddir = /root/dataset_fuzz/jq-1.7.1/modules/oniguruma/src
abs_srcdir = /root/dataset_fuzz/jq-1.7.1/modules/oniguruma/src
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
libname = libonig.la
AM_CFLAGS = -Wall $(am__append_2) $(am__append_3)
AM_CPPFLAGS = -I$(top_srcdir)
include_HEADERS = oniguruma.h oniggnu.h $(am__append_1)
posix_headers = onigposix.h
posix_sources = 
#posix_sources = regposix.c regposerr.c
lib_LTLIBRARIES = $(libname)
libonig_la_SOURCES = regint.h regparse.h regenc.h st.h \
	regparse.c regcomp.c regexec.c \
	regenc.c regerror.c regext.c regsyntax.c regtrav.c regversion.c st.c \
	reggnu.c \
  $(posix_sources) \
	unicode.c \
	unicode_unfold_key.c \
	unicode_fold1_key.c \
	unicode_fold2_key.c \
	unicode_fold3_key.c \
	ascii.c utf8.c \
	utf16_be.c utf16_le.c \
	utf32_be.c utf32_le.c \
	euc_jp.c euc_jp_prop.c \
	sjis.c sjis_prop.c \
	iso8859_1.c \
	iso8859_2.c  iso8859_3.c \
	iso8859_4.c  iso8859_5.c \
	iso8859_6.c  iso8859_7.c \
	iso8859_8.c  iso8859_9.c \
	iso8859_10.c iso8859_11.c \
	iso8859_13.c iso8859_14.c \
	iso8859_15.c iso8859_16.c \
	euc_tw.c euc_kr.c big5.c \
	gb18030.c koi8_r.c cp1251.c \
	onig_init.c

EXTRA_DIST = koi8.c mktable.c \
	unicode_fold_data.c unicode_property_data.c \
	unicode_property_data_posix.c \
	unicode_egcb_data.c unicode_wb_data.c

libonig_la_LDFLAGS = $(EXTRA_LIBONIG_LDFLAGS) -version-info \
	$(LTVERSION) $(am__append_4)
all: config.h
	$(MAKE) $(AM_MAKEFLAGS) all-am

.SUFFIXES:
.SUFFIXES: .c .lo .o .obj
$(srcdir)/Makefile.in:  $(srcdir)/Makefile.am  $(am__configure_deps)
	@for dep in $?; do \
	  case '$(am__configure_deps)' in \
	    *$$dep*) \
	      ( cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh ) \
	        && { if test -f $@; then exit 0; else break; fi; }; \
	      exit 1;; \
	  esac; \
	done; \
	echo ' cd $(top_srcdir) && $(AUTOMAKE) --gnu src/Makefile'; \
	$(am__cd) $(top_srcdir) && \
	  $(AUTOMAKE) --gnu src/Makefile
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

config.h: stamp-h1
	@test -f $@ || rm -f stamp-h1
	@test -f $@ || $(MAKE) $(AM_MAKEFLAGS) stamp-h1

stamp-h1: $(srcdir)/config.h.in $(top_builddir)/config.status
	@rm -f stamp-h1
	cd $(top_builddir) && $(SHELL) ./config.status src/config.h
$(srcdir)/config.h.in:  $(am__configure_deps) 
	($(am__cd) $(top_srcdir) && $(AUTOHEADER))
	rm -f stamp-h1
	touch $@

distclean-hdr:
	-rm -f config.h stamp-h1

install-libLTLIBRARIES: $(lib_LTLIBRARIES)
	@$(NORMAL_INSTALL)
	@list='$(lib_LTLIBRARIES)'; test -n "$(libdir)" || list=; \
	list2=; for p in $$list; do \
	  if test -f $$p; then \
	    list2="$$list2 $$p"; \
	  else :; fi; \
	done; \
	test -z "$$list2" || { \
	  echo " $(MKDIR_P) '$(DESTDIR)$(libdir)'"; \
	  $(MKDIR_P) "$(DESTDIR)$(libdir)" || exit 1; \
	  echo " $(LIBTOOL) $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) --mode=install $(INSTALL) $(INSTALL_STRIP_FLAG) $$list2 '$(DESTDIR)$(libdir)'"; \
	  $(LIBTOOL) $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) --mode=install $(INSTALL) $(INSTALL_STRIP_FLAG) $$list2 "$(DESTDIR)$(libdir)"; \
	}

uninstall-libLTLIBRARIES:
	@$(NORMAL_UNINSTALL)
	@list='$(lib_LTLIBRARIES)'; test -n "$(libdir)" || list=; \
	for p in $$list; do \
	  $(am__strip_dir) \
	  echo " $(LIBTOOL) $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) --mode=uninstall rm -f '$(DESTDIR)$(libdir)/$$f'"; \
	  $(LIBTOOL) $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) --mode=uninstall rm -f "$(DESTDIR)$(libdir)/$$f"; \
	done

clean-libLTLIBRARIES:
	-test -z "$(lib_LTLIBRARIES)" || rm -f $(lib_LTLIBRARIES)
	@list='$(lib_LTLIBRARIES)'; \
	locs=`for p in $$list; do echo $$p; done | \
	      sed 's|^[^/]*$$|.|; s|/[^/]*$$||; s|$$|/so_locations|' | \
	      sort -u`; \
	test -z "$$locs" || { \
	  echo rm -f $${locs}; \
	  rm -f $${locs}; \
	}

libonig.la: $(libonig_la_OBJECTS) $(libonig_la_DEPENDENCIES) $(EXTRA_libonig_la_DEPENDENCIES) 
	$(AM_V_CCLD)$(libonig_la_LINK) -rpath $(libdir) $(libonig_la_OBJECTS) $(libonig_la_LIBADD) $(LIBS)

mostlyclean-compile:
	-rm -f *.$(OBJEXT)

distclean-compile:
	-rm -f *.tab.c

include ./$(DEPDIR)/ascii.Plo # am--include-marker
include ./$(DEPDIR)/big5.Plo # am--include-marker
include ./$(DEPDIR)/cp1251.Plo # am--include-marker
include ./$(DEPDIR)/euc_jp.Plo # am--include-marker
include ./$(DEPDIR)/euc_jp_prop.Plo # am--include-marker
include ./$(DEPDIR)/euc_kr.Plo # am--include-marker
include ./$(DEPDIR)/euc_tw.Plo # am--include-marker
include ./$(DEPDIR)/gb18030.Plo # am--include-marker
include ./$(DEPDIR)/iso8859_1.Plo # am--include-marker
include ./$(DEPDIR)/iso8859_10.Plo # am--include-marker
include ./$(DEPDIR)/iso8859_11.Plo # am--include-marker
include ./$(DEPDIR)/iso8859_13.Plo # am--include-marker
include ./$(DEPDIR)/iso8859_14.Plo # am--include-marker
include ./$(DEPDIR)/iso8859_15.Plo # am--include-marker
include ./$(DEPDIR)/iso8859_16.Plo # am--include-marker
include ./$(DEPDIR)/iso8859_2.Plo # am--include-marker
include ./$(DEPDIR)/iso8859_3.Plo # am--include-marker
include ./$(DEPDIR)/iso8859_4.Plo # am--include-marker
include ./$(DEPDIR)/iso8859_5.Plo # am--include-marker
include ./$(DEPDIR)/iso8859_6.Plo # am--include-marker
include ./$(DEPDIR)/iso8859_7.Plo # am--include-marker
include ./$(DEPDIR)/iso8859_8.Plo # am--include-marker
include ./$(DEPDIR)/iso8859_9.Plo # am--include-marker
include ./$(DEPDIR)/koi8_r.Plo # am--include-marker
include ./$(DEPDIR)/onig_init.Plo # am--include-marker
include ./$(DEPDIR)/regcomp.Plo # am--include-marker
include ./$(DEPDIR)/regenc.Plo # am--include-marker
include ./$(DEPDIR)/regerror.Plo # am--include-marker
include ./$(DEPDIR)/regexec.Plo # am--include-marker
include ./$(DEPDIR)/regext.Plo # am--include-marker
include ./$(DEPDIR)/reggnu.Plo # am--include-marker
include ./$(DEPDIR)/regparse.Plo # am--include-marker
include ./$(DEPDIR)/regposerr.Plo # am--include-marker
include ./$(DEPDIR)/regposix.Plo # am--include-marker
include ./$(DEPDIR)/regsyntax.Plo # am--include-marker
include ./$(DEPDIR)/regtrav.Plo # am--include-marker
include ./$(DEPDIR)/regversion.Plo # am--include-marker
include ./$(DEPDIR)/sjis.Plo # am--include-marker
include ./$(DEPDIR)/sjis_prop.Plo # am--include-marker
include ./$(DEPDIR)/st.Plo # am--include-marker
include ./$(DEPDIR)/unicode.Plo # am--include-marker
include ./$(DEPDIR)/unicode_fold1_key.Plo # am--include-marker
include ./$(DEPDIR)/unicode_fold2_key.Plo # am--include-marker
include ./$(DEPDIR)/unicode_fold3_key.Plo # am--include-marker
include ./$(DEPDIR)/unicode_unfold_key.Plo # am--include-marker
include ./$(DEPDIR)/utf16_be.Plo # am--include-marker
include ./$(DEPDIR)/utf16_le.Plo # am--include-marker
include ./$(DEPDIR)/utf32_be.Plo # am--include-marker
include ./$(DEPDIR)/utf32_le.Plo # am--include-marker
include ./$(DEPDIR)/utf8.Plo # am--include-marker

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
install-includeHEADERS: $(include_HEADERS)
	@$(NORMAL_INSTALL)
	@list='$(include_HEADERS)'; test -n "$(includedir)" || list=; \
	if test -n "$$list"; then \
	  echo " $(MKDIR_P) '$(DESTDIR)$(includedir)'"; \
	  $(MKDIR_P) "$(DESTDIR)$(includedir)" || exit 1; \
	fi; \
	for p in $$list; do \
	  if test -f "$$p"; then d=; else d="$(srcdir)/"; fi; \
	  echo "$$d$$p"; \
	done | $(am__base_list) | \
	while read files; do \
	  echo " $(INSTALL_HEADER) $$files '$(DESTDIR)$(includedir)'"; \
	  $(INSTALL_HEADER) $$files "$(DESTDIR)$(includedir)" || exit $$?; \
	done

uninstall-includeHEADERS:
	@$(NORMAL_UNINSTALL)
	@list='$(include_HEADERS)'; test -n "$(includedir)" || list=; \
	files=`for p in $$list; do echo $$p; done | sed -e 's|^.*/||'`; \
	dir='$(DESTDIR)$(includedir)'; $(am__uninstall_files_from_dir)

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
all-am: Makefile $(LTLIBRARIES) $(HEADERS) config.h
installdirs:
	for dir in "$(DESTDIR)$(libdir)" "$(DESTDIR)$(includedir)"; do \
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

clean-generic:

distclean-generic:
	-test -z "$(CONFIG_CLEAN_FILES)" || rm -f $(CONFIG_CLEAN_FILES)
	-test . = "$(srcdir)" || test -z "$(CONFIG_CLEAN_VPATH_FILES)" || rm -f $(CONFIG_CLEAN_VPATH_FILES)

maintainer-clean-generic:
	@echo "This command is intended for maintainers to use"
	@echo "it deletes files that may require special tools to rebuild."
install-data-hook:
uninstall-hook:
clean: clean-am

clean-am: clean-generic clean-libLTLIBRARIES clean-libtool \
	mostlyclean-am

distclean: distclean-am
		-rm -f ./$(DEPDIR)/ascii.Plo
	-rm -f ./$(DEPDIR)/big5.Plo
	-rm -f ./$(DEPDIR)/cp1251.Plo
	-rm -f ./$(DEPDIR)/euc_jp.Plo
	-rm -f ./$(DEPDIR)/euc_jp_prop.Plo
	-rm -f ./$(DEPDIR)/euc_kr.Plo
	-rm -f ./$(DEPDIR)/euc_tw.Plo
	-rm -f ./$(DEPDIR)/gb18030.Plo
	-rm -f ./$(DEPDIR)/iso8859_1.Plo
	-rm -f ./$(DEPDIR)/iso8859_10.Plo
	-rm -f ./$(DEPDIR)/iso8859_11.Plo
	-rm -f ./$(DEPDIR)/iso8859_13.Plo
	-rm -f ./$(DEPDIR)/iso8859_14.Plo
	-rm -f ./$(DEPDIR)/iso8859_15.Plo
	-rm -f ./$(DEPDIR)/iso8859_16.Plo
	-rm -f ./$(DEPDIR)/iso8859_2.Plo
	-rm -f ./$(DEPDIR)/iso8859_3.Plo
	-rm -f ./$(DEPDIR)/iso8859_4.Plo
	-rm -f ./$(DEPDIR)/iso8859_5.Plo
	-rm -f ./$(DEPDIR)/iso8859_6.Plo
	-rm -f ./$(DEPDIR)/iso8859_7.Plo
	-rm -f ./$(DEPDIR)/iso8859_8.Plo
	-rm -f ./$(DEPDIR)/iso8859_9.Plo
	-rm -f ./$(DEPDIR)/koi8_r.Plo
	-rm -f ./$(DEPDIR)/onig_init.Plo
	-rm -f ./$(DEPDIR)/regcomp.Plo
	-rm -f ./$(DEPDIR)/regenc.Plo
	-rm -f ./$(DEPDIR)/regerror.Plo
	-rm -f ./$(DEPDIR)/regexec.Plo
	-rm -f ./$(DEPDIR)/regext.Plo
	-rm -f ./$(DEPDIR)/reggnu.Plo
	-rm -f ./$(DEPDIR)/regparse.Plo
	-rm -f ./$(DEPDIR)/regposerr.Plo
	-rm -f ./$(DEPDIR)/regposix.Plo
	-rm -f ./$(DEPDIR)/regsyntax.Plo
	-rm -f ./$(DEPDIR)/regtrav.Plo
	-rm -f ./$(DEPDIR)/regversion.Plo
	-rm -f ./$(DEPDIR)/sjis.Plo
	-rm -f ./$(DEPDIR)/sjis_prop.Plo
	-rm -f ./$(DEPDIR)/st.Plo
	-rm -f ./$(DEPDIR)/unicode.Plo
	-rm -f ./$(DEPDIR)/unicode_fold1_key.Plo
	-rm -f ./$(DEPDIR)/unicode_fold2_key.Plo
	-rm -f ./$(DEPDIR)/unicode_fold3_key.Plo
	-rm -f ./$(DEPDIR)/unicode_unfold_key.Plo
	-rm -f ./$(DEPDIR)/utf16_be.Plo
	-rm -f ./$(DEPDIR)/utf16_le.Plo
	-rm -f ./$(DEPDIR)/utf32_be.Plo
	-rm -f ./$(DEPDIR)/utf32_le.Plo
	-rm -f ./$(DEPDIR)/utf8.Plo
	-rm -f Makefile
distclean-am: clean-am distclean-compile distclean-generic \
	distclean-hdr distclean-tags

dvi: dvi-am

dvi-am:

html: html-am

html-am:

info: info-am

info-am:

install-data-am: install-includeHEADERS
	@$(NORMAL_INSTALL)
	$(MAKE) $(AM_MAKEFLAGS) install-data-hook
install-dvi: install-dvi-am

install-dvi-am:

install-exec-am: install-libLTLIBRARIES

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
		-rm -f ./$(DEPDIR)/ascii.Plo
	-rm -f ./$(DEPDIR)/big5.Plo
	-rm -f ./$(DEPDIR)/cp1251.Plo
	-rm -f ./$(DEPDIR)/euc_jp.Plo
	-rm -f ./$(DEPDIR)/euc_jp_prop.Plo
	-rm -f ./$(DEPDIR)/euc_kr.Plo
	-rm -f ./$(DEPDIR)/euc_tw.Plo
	-rm -f ./$(DEPDIR)/gb18030.Plo
	-rm -f ./$(DEPDIR)/iso8859_1.Plo
	-rm -f ./$(DEPDIR)/iso8859_10.Plo
	-rm -f ./$(DEPDIR)/iso8859_11.Plo
	-rm -f ./$(DEPDIR)/iso8859_13.Plo
	-rm -f ./$(DEPDIR)/iso8859_14.Plo
	-rm -f ./$(DEPDIR)/iso8859_15.Plo
	-rm -f ./$(DEPDIR)/iso8859_16.Plo
	-rm -f ./$(DEPDIR)/iso8859_2.Plo
	-rm -f ./$(DEPDIR)/iso8859_3.Plo
	-rm -f ./$(DEPDIR)/iso8859_4.Plo
	-rm -f ./$(DEPDIR)/iso8859_5.Plo
	-rm -f ./$(DEPDIR)/iso8859_6.Plo
	-rm -f ./$(DEPDIR)/iso8859_7.Plo
	-rm -f ./$(DEPDIR)/iso8859_8.Plo
	-rm -f ./$(DEPDIR)/iso8859_9.Plo
	-rm -f ./$(DEPDIR)/koi8_r.Plo
	-rm -f ./$(DEPDIR)/onig_init.Plo
	-rm -f ./$(DEPDIR)/regcomp.Plo
	-rm -f ./$(DEPDIR)/regenc.Plo
	-rm -f ./$(DEPDIR)/regerror.Plo
	-rm -f ./$(DEPDIR)/regexec.Plo
	-rm -f ./$(DEPDIR)/regext.Plo
	-rm -f ./$(DEPDIR)/reggnu.Plo
	-rm -f ./$(DEPDIR)/regparse.Plo
	-rm -f ./$(DEPDIR)/regposerr.Plo
	-rm -f ./$(DEPDIR)/regposix.Plo
	-rm -f ./$(DEPDIR)/regsyntax.Plo
	-rm -f ./$(DEPDIR)/regtrav.Plo
	-rm -f ./$(DEPDIR)/regversion.Plo
	-rm -f ./$(DEPDIR)/sjis.Plo
	-rm -f ./$(DEPDIR)/sjis_prop.Plo
	-rm -f ./$(DEPDIR)/st.Plo
	-rm -f ./$(DEPDIR)/unicode.Plo
	-rm -f ./$(DEPDIR)/unicode_fold1_key.Plo
	-rm -f ./$(DEPDIR)/unicode_fold2_key.Plo
	-rm -f ./$(DEPDIR)/unicode_fold3_key.Plo
	-rm -f ./$(DEPDIR)/unicode_unfold_key.Plo
	-rm -f ./$(DEPDIR)/utf16_be.Plo
	-rm -f ./$(DEPDIR)/utf16_le.Plo
	-rm -f ./$(DEPDIR)/utf32_be.Plo
	-rm -f ./$(DEPDIR)/utf32_le.Plo
	-rm -f ./$(DEPDIR)/utf8.Plo
	-rm -f Makefile
maintainer-clean-am: distclean-am maintainer-clean-generic

mostlyclean: mostlyclean-am

mostlyclean-am: mostlyclean-compile mostlyclean-generic \
	mostlyclean-libtool

pdf: pdf-am

pdf-am:

ps: ps-am

ps-am:

uninstall-am: uninstall-includeHEADERS uninstall-libLTLIBRARIES
	@$(NORMAL_INSTALL)
	$(MAKE) $(AM_MAKEFLAGS) uninstall-hook
.MAKE: all install-am install-data-am install-strip uninstall-am

.PHONY: CTAGS GTAGS TAGS all all-am am--depfiles check check-am clean \
	clean-generic clean-libLTLIBRARIES clean-libtool cscopelist-am \
	ctags ctags-am distclean distclean-compile distclean-generic \
	distclean-hdr distclean-libtool distclean-tags distdir dvi \
	dvi-am html html-am info info-am install install-am \
	install-data install-data-am install-data-hook install-dvi \
	install-dvi-am install-exec install-exec-am install-html \
	install-html-am install-includeHEADERS install-info \
	install-info-am install-libLTLIBRARIES install-man install-pdf \
	install-pdf-am install-ps install-ps-am install-strip \
	installcheck installcheck-am installdirs maintainer-clean \
	maintainer-clean-generic mostlyclean mostlyclean-compile \
	mostlyclean-generic mostlyclean-libtool pdf pdf-am ps ps-am \
	tags tags-am uninstall uninstall-am uninstall-hook \
	uninstall-includeHEADERS uninstall-libLTLIBRARIES

.PRECIOUS: Makefile


#install-data-hook:
#	echo "$(LIBTOOL) $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) --mode=install $(INSTALL_DATA) $(LIBONIG_DEF_FILE) $(DESTDIR)$(libdir)"; \
#	$(LIBTOOL) $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) --mode=install $(INSTALL_DATA) $(LIBONIG_DEF_FILE) $(DESTDIR)$(libdir) || exit 1

#uninstall-hook:
#	echo " $(LIBTOOL) $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) --mode=uninstall rm -f '$(DESTDIR)$(libdir)/$(LIBONIG_DEF_FILE)'"; \
#	$(LIBTOOL) $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) --mode=uninstall rm -f "$(DESTDIR)$(libdir)/$(LIBONIG_DEF_FILE)"

dll:
	$(CXX) -shared -Wl,--output-def,libonig.def -o libonig.dll *.o \
	$(LIBS)
	strip libonig.dll

# character-types-table source generator
mktable: mktable.c regenc.h
	$(CC) -I$(top_srcdir) -o mktable mktable.c

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
