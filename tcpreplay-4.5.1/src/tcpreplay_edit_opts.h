/*   -*- buffer-read-only: t -*- vi: set ro:
 *
 *  DO NOT EDIT THIS FILE   (tcpreplay_edit_opts.h)
 *
 *  It has been AutoGen-ed
 *  From the definitions    ../../src/tcpreplay_opts.def
 *  and the template file   options
 *
 * Generated from AutoOpts 42:1:17 templates.
 *
 *  AutoOpts is a copyrighted work.  This header file is not encumbered
 *  by AutoOpts licensing, but is provided under the licensing terms chosen
 *  by the tcpreplay author or copyright holder.  AutoOpts is
 *  licensed under the terms of the LGPL.  The redistributable library
 *  (``libopts'') is licensed under the terms of either the LGPL or, at the
 *  users discretion, the BSD license.  See the AutoOpts and/or libopts sources
 *  for details.
 *
 * The tcpreplay program is copyrighted and licensed
 * under the following terms:
 *
 *  Copyright (C) 2000-2024 Aaron Turner and Fred Klassen, all rights reserved.
 *  This is free software. It is licensed for use, modification and
 *  redistribution under the terms of the GNU General Public License,
 *  version 3 or later <http://gnu.org/licenses/gpl.html>
 *
 *  tcpreplay is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  tcpreplay is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 *  This file contains the programmatic interface to the Automated
 *  Options generated for the tcpreplay program.
 *  These macros are documented in the AutoGen info file in the
 *  "AutoOpts" chapter.  Please refer to that doc for usage help.
 */
#ifndef AUTOOPTS_TCPREPLAY_EDIT_OPTS_H_GUARD
#define AUTOOPTS_TCPREPLAY_EDIT_OPTS_H_GUARD 1
#include "config.h"
#include <autoopts/options.h>
#include <stdarg.h>
#include <stdnoreturn.h>

/**
 *  Ensure that the library used for compiling this generated header is at
 *  least as new as the version current when the header template was released
 *  (not counting patch version increments).  Also ensure that the oldest
 *  tolerable version is at least as old as what was current when the header
 *  template was released.
 */
#define AO_TEMPLATE_VERSION 172033
#if (AO_TEMPLATE_VERSION < OPTIONS_MINIMUM_VERSION) \
 || (AO_TEMPLATE_VERSION > OPTIONS_STRUCT_VERSION)
# error option template version mismatches autoopts/options.h header
  Choke Me.
#endif

#if GCC_VERSION > 40400
#define NOT_REACHED __builtin_unreachable();
#else
#define NOT_REACHED
#endif

/**
 *  Enumeration of each option type for tcpreplay
 */
typedef enum {
    INDEX_OPT_PORTMAP                   =  1,
    INDEX_OPT_SEED                      =  2,
    INDEX_OPT_PNAT                      =  3,
    INDEX_OPT_SRCIPMAP                  =  4,
    INDEX_OPT_DSTIPMAP                  =  5,
    INDEX_OPT_ENDPOINTS                 =  6,
    INDEX_OPT_TCP_SEQUENCE              =  7,
    INDEX_OPT_SKIPBROADCAST             =  8,
    INDEX_OPT_FIXCSUM                   =  9,
    INDEX_OPT_FIXHDRLEN                 = 10,
    INDEX_OPT_MTU                       = 11,
    INDEX_OPT_MTU_TRUNC                 = 12,
    INDEX_OPT_EFCS                      = 13,
    INDEX_OPT_TTL                       = 14,
    INDEX_OPT_TOS                       = 15,
    INDEX_OPT_TCLASS                    = 16,
    INDEX_OPT_FLOWLABEL                 = 17,
    INDEX_OPT_FIXLEN                    = 18,
    INDEX_OPT_FUZZ_SEED                 = 19,
    INDEX_OPT_FUZZ_FACTOR               = 20,
    INDEX_OPT_SKIPL2BROADCAST           = 21,
    INDEX_OPT_DLT                       = 22,
    INDEX_OPT_ENET_DMAC                 = 23,
    INDEX_OPT_ENET_SMAC                 = 24,
    INDEX_OPT_ENET_SUBSMAC              = 25,
    INDEX_OPT_ENET_MAC_SEED             = 26,
    INDEX_OPT_ENET_MAC_SEED_KEEP_BYTES  = 27,
    INDEX_OPT_ENET_VLAN                 = 28,
    INDEX_OPT_ENET_VLAN_TAG             = 29,
    INDEX_OPT_ENET_VLAN_CFI             = 30,
    INDEX_OPT_ENET_VLAN_PRI             = 31,
    INDEX_OPT_ENET_VLAN_PROTO           = 32,
    INDEX_OPT_HDLC_CONTROL              = 33,
    INDEX_OPT_HDLC_ADDRESS              = 34,
    INDEX_OPT_USER_DLT                  = 35,
    INDEX_OPT_USER_DLINK                = 36,
    INDEX_OPT_DBUG                      = 37,
    INDEX_OPT_QUIET                     = 38,
    INDEX_OPT_TIMER                     = 39,
    INDEX_OPT_MAXSLEEP                  = 40,
    INDEX_OPT_VERBOSE                   = 41,
    INDEX_OPT_DECODE                    = 42,
    INDEX_OPT_PRELOAD_PCAP              = 43,
    INDEX_OPT_CACHEFILE                 = 44,
    INDEX_OPT_DUALFILE                  = 45,
    INDEX_OPT_INTF1                     = 46,
    INDEX_OPT_INTF2                     = 47,
    INDEX_OPT_WRITE                     = 48,
    INDEX_OPT_INCLUDE                   = 49,
    INDEX_OPT_EXCLUDE                   = 50,
    INDEX_OPT_LISTNICS                  = 51,
    INDEX_OPT_LOOP                      = 52,
    INDEX_OPT_LOOPDELAY_MS              = 53,
    INDEX_OPT_LOOPDELAY_NS              = 54,
    INDEX_OPT_PKTLEN                    = 55,
    INDEX_OPT_LIMIT                     = 56,
    INDEX_OPT_DURATION                  = 57,
    INDEX_OPT_MULTIPLIER                = 58,
    INDEX_OPT_PPS                       = 59,
    INDEX_OPT_MBPS                      = 60,
    INDEX_OPT_TOPSPEED                  = 61,
    INDEX_OPT_ONEATATIME                = 62,
    INDEX_OPT_PPS_MULTI                 = 63,
    INDEX_OPT_UNIQUE_IP                 = 64,
    INDEX_OPT_UNIQUE_IP_LOOPS           = 65,
    INDEX_OPT_NETMAP                    = 66,
    INDEX_OPT_NM_DELAY                  = 67,
    INDEX_OPT_NO_FLOW_STATS             = 68,
    INDEX_OPT_FLOW_EXPIRY               = 69,
    INDEX_OPT_PID                       = 70,
    INDEX_OPT_STATS                     = 71,
    INDEX_OPT_SUPPRESS_WARNINGS         = 72,
    INDEX_OPT_XDP                       = 73,
    INDEX_OPT_XDP_BATCH_SIZE            = 74,
    INDEX_OPT_VERSION                   = 75,
    INDEX_OPT_LESS_HELP                 = 76,
    INDEX_OPT_HELP                      = 77,
    INDEX_OPT_MORE_HELP                 = 78,
    INDEX_OPT_SAVE_OPTS                 = 79,
    INDEX_OPT_LOAD_OPTS                 = 80
} teOptIndex;
/** count of all options for tcpreplay */
#define OPTION_CT    81

/**
 *  Interface defines for all options.  Replace "n" with the UPPER_CASED
 *  option name (as in the teOptIndex enumeration above).
 *  e.g. HAVE_OPT(TCPEDIT)
 */
#define         DESC(n) (tcpreplayOptions.pOptDesc[INDEX_OPT_## n])
/** 'true' if an option has been specified in any way */
#define     HAVE_OPT(n) (! UNUSED_OPT(& DESC(n)))
/** The string argument to an option. The argument type must be \"string\". */
#define      OPT_ARG(n) (DESC(n).optArg.argString)
/** Mask the option state revealing how an option was specified.
 *  It will be one and only one of \a OPTST_SET, \a OPTST_PRESET,
 * \a OPTST_DEFINED, \a OPTST_RESET or zero.
 */
#define    STATE_OPT(n) (DESC(n).fOptState & OPTST_SET_MASK)
/** Count of option's occurrances *on the command line*. */
#define    COUNT_OPT(n) (DESC(n).optOccCt)
/** mask of \a OPTST_SET and \a OPTST_DEFINED. */
#define    ISSEL_OPT(n) (SELECTED_OPT(&DESC(n)))
/** 'true' if \a HAVE_OPT would yield 'false'. */
#define ISUNUSED_OPT(n) (UNUSED_OPT(& DESC(n)))
/** 'true' if OPTST_DISABLED bit not set. */
#define  ENABLED_OPT(n) (! DISABLED_OPT(& DESC(n)))
/** number of stacked option arguments.
 *  Valid only for stacked option arguments. */
#define  STACKCT_OPT(n) (((tArgList*)(DESC(n).optCookie))->useCt)
/** stacked argument vector.
 *  Valid only for stacked option arguments. */
#define STACKLST_OPT(n) (((tArgList*)(DESC(n).optCookie))->apzArgs)
/** Reset an option. */
#define    CLEAR_OPT(n) STMTS( \
                DESC(n).fOptState &= OPTST_PERSISTENT_MASK;   \
                if ((DESC(n).fOptState & OPTST_INITENABLED) == 0) \
                    DESC(n).fOptState |= OPTST_DISABLED; \
                DESC(n).optCookie = NULL )
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/**
 *  Enumeration of tcpreplay exit codes
 */
typedef enum {
    TCPREPLAY_EXIT_SUCCESS         = 0,
    TCPREPLAY_EXIT_FAILURE         = 1,
    TCPREPLAY_EXIT_USAGE_ERROR     = 64,
    TCPREPLAY_EXIT_NO_CONFIG_INPUT = 66,
    TCPREPLAY_EXIT_LIBOPTS_FAILURE = 70
}   tcpreplay_exit_code_t;
/**
 *  Interface defines for specific options.
 * @{
 */
#define VALUE_OPT_PORTMAP        'r'
#define VALUE_OPT_SEED           's'

#define OPT_VALUE_SEED           (DESC(SEED).optArg.argInt)
#define VALUE_OPT_PNAT           'N'
#define VALUE_OPT_SRCIPMAP       'S'
#define VALUE_OPT_DSTIPMAP       'D'
#define VALUE_OPT_ENDPOINTS      'e'
#define VALUE_OPT_TCP_SEQUENCE   0x1001

#define OPT_VALUE_TCP_SEQUENCE   (DESC(TCP_SEQUENCE).optArg.argInt)
#define VALUE_OPT_SKIPBROADCAST  'b'
#define VALUE_OPT_FIXCSUM        'C'
#define VALUE_OPT_FIXHDRLEN      0x1002
#define VALUE_OPT_MTU            'm'

#define OPT_VALUE_MTU            (DESC(MTU).optArg.argInt)
#define VALUE_OPT_MTU_TRUNC      0x1003
#define VALUE_OPT_EFCS           'E'
#define VALUE_OPT_TTL            0x1004
#define VALUE_OPT_TOS            0x1005

#define OPT_VALUE_TOS            (DESC(TOS).optArg.argInt)
#define VALUE_OPT_TCLASS         0x1006

#define OPT_VALUE_TCLASS         (DESC(TCLASS).optArg.argInt)
#define VALUE_OPT_FLOWLABEL      0x1007

#define OPT_VALUE_FLOWLABEL      (DESC(FLOWLABEL).optArg.argInt)
#define VALUE_OPT_FIXLEN         'F'
#define VALUE_OPT_FUZZ_SEED      0x1008

#define OPT_VALUE_FUZZ_SEED      (DESC(FUZZ_SEED).optArg.argInt)
#define VALUE_OPT_FUZZ_FACTOR    0x1009

#define OPT_VALUE_FUZZ_FACTOR    (DESC(FUZZ_FACTOR).optArg.argInt)
#define VALUE_OPT_SKIPL2BROADCAST 0x100A
#define VALUE_OPT_DLT            0x100B
#define VALUE_OPT_ENET_DMAC      0x100C
#define VALUE_OPT_ENET_SMAC      0x100D
#define VALUE_OPT_ENET_SUBSMAC   0x100E
#define VALUE_OPT_ENET_MAC_SEED  0x100F

#define OPT_VALUE_ENET_MAC_SEED  (DESC(ENET_MAC_SEED).optArg.argInt)
#define VALUE_OPT_ENET_MAC_SEED_KEEP_BYTES 0x1010

#define OPT_VALUE_ENET_MAC_SEED_KEEP_BYTES (DESC(ENET_MAC_SEED_KEEP_BYTES).optArg.argInt)
#define VALUE_OPT_ENET_VLAN      0x1011
#define VALUE_OPT_ENET_VLAN_TAG  0x1012

#define OPT_VALUE_ENET_VLAN_TAG  (DESC(ENET_VLAN_TAG).optArg.argInt)
#define VALUE_OPT_ENET_VLAN_CFI  0x1013

#define OPT_VALUE_ENET_VLAN_CFI  (DESC(ENET_VLAN_CFI).optArg.argInt)
#define VALUE_OPT_ENET_VLAN_PRI  0x1014

#define OPT_VALUE_ENET_VLAN_PRI  (DESC(ENET_VLAN_PRI).optArg.argInt)
#define VALUE_OPT_ENET_VLAN_PROTO 0x1015
#define VALUE_OPT_HDLC_CONTROL   0x1016

#define OPT_VALUE_HDLC_CONTROL   (DESC(HDLC_CONTROL).optArg.argInt)
#define VALUE_OPT_HDLC_ADDRESS   0x1017

#define OPT_VALUE_HDLC_ADDRESS   (DESC(HDLC_ADDRESS).optArg.argInt)
#define VALUE_OPT_USER_DLT       0x1018

#define OPT_VALUE_USER_DLT       (DESC(USER_DLT).optArg.argInt)
#define VALUE_OPT_USER_DLINK     0x1019
#define VALUE_OPT_DBUG           'd'
#ifdef DEBUG
#define OPT_VALUE_DBUG           (DESC(DBUG).optArg.argInt)
#endif /* DEBUG */
#define VALUE_OPT_QUIET          'q'
#define VALUE_OPT_TIMER          'T'
#define VALUE_OPT_MAXSLEEP       0x101A

#define OPT_VALUE_MAXSLEEP       (DESC(MAXSLEEP).optArg.argInt)
#define VALUE_OPT_VERBOSE        'v'
#ifdef ENABLE_VERBOSE
#define SET_OPT_VERBOSE   STMTS( \
        DESC(VERBOSE).optActualIndex = 41; \
        DESC(VERBOSE).optActualValue = VALUE_OPT_VERBOSE; \
        DESC(VERBOSE).fOptState &= OPTST_PERSISTENT_MASK; \
        DESC(VERBOSE).fOptState |= OPTST_SET )
#endif /* ENABLE_VERBOSE */
#define VALUE_OPT_DECODE         'A'
#define VALUE_OPT_PRELOAD_PCAP   'K'
#define VALUE_OPT_CACHEFILE      'c'
#define VALUE_OPT_DUALFILE       '2'
#define VALUE_OPT_INTF1          'i'

/** Define the option value intf1 is equivalenced to */
#define WHICH_OPT_INTF1          (DESC(INTF1).optActualValue)
/** Define the index of the option intf1 is equivalenced to */
#define WHICH_IDX_INTF1          (DESC(INTF1).optActualIndex)
#define VALUE_OPT_INTF2          'I'
#define VALUE_OPT_WRITE          'w'
#define VALUE_OPT_INCLUDE        0x101B
#define VALUE_OPT_EXCLUDE        0x101C
#define VALUE_OPT_LISTNICS       0x101D
#define VALUE_OPT_LOOP           'l'

#define OPT_VALUE_LOOP           (DESC(LOOP).optArg.argInt)
#define VALUE_OPT_LOOPDELAY_MS   0x101E

#define OPT_VALUE_LOOPDELAY_MS   (DESC(LOOPDELAY_MS).optArg.argInt)
#define VALUE_OPT_LOOPDELAY_NS   0x101F

#define OPT_VALUE_LOOPDELAY_NS   (DESC(LOOPDELAY_NS).optArg.argInt)
#define VALUE_OPT_PKTLEN         0x1020
#define VALUE_OPT_LIMIT          'L'

#define OPT_VALUE_LIMIT          (DESC(LIMIT).optArg.argInt)
#define VALUE_OPT_DURATION       0x1021

#define OPT_VALUE_DURATION       (DESC(DURATION).optArg.argInt)
#define VALUE_OPT_MULTIPLIER     'x'
#define VALUE_OPT_PPS            'p'
#define VALUE_OPT_MBPS           'M'
#define VALUE_OPT_TOPSPEED       't'
#define VALUE_OPT_ONEATATIME     'o'
#define VALUE_OPT_PPS_MULTI      0x1022

#define OPT_VALUE_PPS_MULTI      (DESC(PPS_MULTI).optArg.argInt)
#define VALUE_OPT_UNIQUE_IP      0x1023
#define VALUE_OPT_UNIQUE_IP_LOOPS 0x1024
#define VALUE_OPT_NETMAP         0x1025
#define VALUE_OPT_NM_DELAY       0x1026
#ifdef HAVE_NETMAP
#define OPT_VALUE_NM_DELAY       (DESC(NM_DELAY).optArg.argInt)
#endif /* HAVE_NETMAP */
#define VALUE_OPT_NO_FLOW_STATS  0x1027
#define VALUE_OPT_FLOW_EXPIRY    0x1028

#define OPT_VALUE_FLOW_EXPIRY    (DESC(FLOW_EXPIRY).optArg.argInt)
#define VALUE_OPT_PID            'P'
#define VALUE_OPT_STATS          0x1029

#define OPT_VALUE_STATS          (DESC(STATS).optArg.argInt)
#define VALUE_OPT_SUPPRESS_WARNINGS 'W'

#define SET_OPT_SUPPRESS_WARNINGS   STMTS( \
        DESC(SUPPRESS_WARNINGS).optActualIndex = 72; \
        DESC(SUPPRESS_WARNINGS).optActualValue = VALUE_OPT_SUPPRESS_WARNINGS; \
        DESC(SUPPRESS_WARNINGS).fOptState &= OPTST_PERSISTENT_MASK; \
        DESC(SUPPRESS_WARNINGS).fOptState |= OPTST_SET )
#define VALUE_OPT_XDP            0x102A
#define VALUE_OPT_XDP_BATCH_SIZE 0x102B
#ifdef HAVE_LIBXDP
#define OPT_VALUE_XDP_BATCH_SIZE (DESC(XDP_BATCH_SIZE).optArg.argInt)
#endif /* HAVE_LIBXDP */
#define VALUE_OPT_VERSION        'V'
#define VALUE_OPT_LESS_HELP      'h'
/** option flag (value) for help-value option */
#define VALUE_OPT_HELP          'H'
/** option flag (value) for more-help-value option */
#define VALUE_OPT_MORE_HELP     '!'
/** option flag (value) for save-opts-value option */
#define VALUE_OPT_SAVE_OPTS     0x102C
/** option flag (value) for load-opts-value option */
#define VALUE_OPT_LOAD_OPTS     0x102D
#define SET_OPT_SAVE_OPTS(a)   STMTS( \
        DESC(SAVE_OPTS).fOptState &= OPTST_PERSISTENT_MASK; \
        DESC(SAVE_OPTS).fOptState |= OPTST_SET; \
        DESC(SAVE_OPTS).optArg.argString = (char const*)(a))
/*
 *  Interface defines not associated with particular options
 */
#define ERRSKIP_OPTERR  STMTS(tcpreplayOptions.fOptSet &= ~OPTPROC_ERRSTOP)
#define ERRSTOP_OPTERR  STMTS(tcpreplayOptions.fOptSet |= OPTPROC_ERRSTOP)
#define RESTART_OPT(n)  STMTS( \
                tcpreplayOptions.curOptIdx = (n); \
                tcpreplayOptions.pzCurOpt  = NULL )
#define START_OPT       RESTART_OPT(1)
#define USAGE(c)        (*tcpreplayOptions.pUsageProc)(&tcpreplayOptions, c)

#ifdef  __cplusplus
extern "C" {
#endif


/* * * * * *
 *
 *  Declare the tcpreplay option descriptor.
 */
extern tOptions tcpreplayOptions;

#if defined(ENABLE_NLS)
# ifndef _
#   include <stdio.h>
#   ifndef HAVE_GETTEXT
      extern char * gettext(char const *);
#   else
#     include <libintl.h>
#   endif

# ifndef ATTRIBUTE_FORMAT_ARG
#   define ATTRIBUTE_FORMAT_ARG(_a)
# endif

static inline char* aoGetsText(char const* pz) ATTRIBUTE_FORMAT_ARG(1);
static inline char* aoGetsText(char const* pz) {
    if (pz == NULL) return NULL;
    return (char*)gettext(pz);
}
#   define _(s)  aoGetsText(s)
# endif /* _() */

# define OPT_NO_XLAT_CFG_NAMES  STMTS(tcpreplayOptions.fOptSet |= \
                                    OPTPROC_NXLAT_OPT_CFG;)
# define OPT_NO_XLAT_OPT_NAMES  STMTS(tcpreplayOptions.fOptSet |= \
                                    OPTPROC_NXLAT_OPT|OPTPROC_NXLAT_OPT_CFG;)

# define OPT_XLAT_CFG_NAMES     STMTS(tcpreplayOptions.fOptSet &= \
                                  ~(OPTPROC_NXLAT_OPT|OPTPROC_NXLAT_OPT_CFG);)
# define OPT_XLAT_OPT_NAMES     STMTS(tcpreplayOptions.fOptSet &= \
                                  ~OPTPROC_NXLAT_OPT;)

#else   /* ENABLE_NLS */
# define OPT_NO_XLAT_CFG_NAMES
# define OPT_NO_XLAT_OPT_NAMES

# define OPT_XLAT_CFG_NAMES
# define OPT_XLAT_OPT_NAMES

# ifndef _
#   define _(_s)  _s
# endif
#endif  /* ENABLE_NLS */


#ifdef  __cplusplus
}
#endif
#endif /* AUTOOPTS_TCPREPLAY_EDIT_OPTS_H_GUARD */

/* tcpreplay_edit_opts.h ends here */
