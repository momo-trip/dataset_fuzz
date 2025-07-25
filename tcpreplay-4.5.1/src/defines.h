#pragma once

#include "config.h"

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef WIN32
#include "msvc_inttypes.h"
#include "msvc_stdint.h"
#else

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#endif /* WIN32 */

#ifdef HAVE_SYS_SOCKET
#include <sys/socket.h>
#endif

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#ifdef HAVE_NETINET_IN_SYSTM_H
#include <netinet/in_systm.h>
#endif

#ifdef HAVE_SCHED_H
#include <sched.h>
#endif

#include "tcpr.h"

#ifdef HAVE_BPF
#include <net/bpf.h>
#define PCAP_DONT_INCLUDE_PCAP_BPF_H 1
#endif

#ifdef HAVE_LIBBPF
#undef HAVE_BPF
#include <bpf/bpf.h>
#include <bpf/libbpf.h>
#define PCAP_DONT_INCLUDE_PCAP_BPF_H 1

struct bpf_program {
char dummy[0];
};

#endif

#ifdef HAVE_LIBXDP
#include <xdp/libxdp.h>
#endif

#if defined INCLUDE_PCAP_BPF_H_FILE && !defined PCAP_DONT_INCLUDE_PCAP_BPF_H
#include <@INCLUDE_PCAP_BPF_HEADER@>
#define PCAP_DONT_INCLUDE_PCAP_BPF_H 1 /* don't re-include it in pcap.h */
#endif

#include </usr/include/pcap.h>

/* include our own strlcat/strlcpy? */
#ifndef HAVE_STRLCPY
#include <lib/strlcpy.h>
#endif

/*
 * net/bpf.h doesn't include DLT types, but pcap-bpf.h does.
 * Unfortunately, pcap-bpf.h also includes things in net/bpf.h
 * while also missing some key things (wow, that sucks)
 * The result is that I stole the DLT types from pcap-bpf.h and
 * put them in here.
 */
#include <common/dlt_names.h>

#ifdef HAVE_BOOL_H
#include <bool.h>
#endif

#ifdef HAVE_STDBOOL_H
#include <stdbool.h>
#endif


/* should packet counters be 32 or 64 bit? --enable-64bit */
#ifdef ENABLE_64BITS
#define COUNTER unsigned long long
#define COUNTER_SPEC "%llu"
#else
#define COUNTER unsigned long
#define COUNTER_SPEC "%lu"
#endif
#define COUNTER_OVERFLOW_RISK (((COUNTER)~0) >> 23)

#include <common/cidr.h>
#include <common/list.h>


typedef struct tcpr_ipv4_hdr ipv4_hdr_t;
typedef struct tcpr_ipv6_hdr ipv6_hdr_t;
typedef struct tcpr_tcp_hdr tcp_hdr_t;
typedef struct tcpr_udp_hdr udp_hdr_t;
typedef struct tcpr_icmpv4_hdr icmpv4_hdr_t;
typedef struct tcpr_icmpv6_hdr icmpv6_hdr_t;
typedef struct tcpr_ethernet_hdr eth_hdr_t;
typedef struct tcpr_802_1q_hdr vlan_hdr_t;
typedef struct sll_header sll_hdr_t;
typedef struct sll2_header sll2_hdr_t;
typedef struct tcpr_arp_hdr arp_hdr_t;
typedef struct tcpr_dnsv4_hdr dnsv4_hdr_t;

/* our custom typdefs/structs */
typedef u_char tcpr_macaddr_t[TCPR_ETH_H];

typedef struct tcpr_bpf_s {
    char *filter;
    int optimize;
    struct bpf_program program;
} tcpr_bpf_t;

typedef struct tcpr_xX_s {
#define xX_MODE_INCLUDE 'x'
#define xX_MODE_EXCLUDE 'X'
    int mode;
    tcpr_list_t *list;
    tcpr_cidr_t *cidr;
#define xX_TYPE_LIST 1
#define xX_TYPE_CIDR 2
    int type;
} tcpr_xX_t;

/* number of ports 0-65535 */
#define NUM_PORTS 65536
typedef struct tcpr_services_s {
    char tcp[NUM_PORTS];
    char udp[NUM_PORTS];
} tcpr_services_t;

typedef struct tcpr_speed_s {
    /* speed modifiers */
    int mode;
#define SPEED_MULTIPLIER 1
#define SPEED_MBPSRATE 2
#define SPEED_PACKETRATE 3
#define SPEED_TOPSPEED 4
#define SPEED_ONEATATIME 5
    COUNTER speed;
    float multiplier;
    int pps_multi;
} tcpr_speed_t;

#define MAX_FILES 1024 /* Max number of files we can pass to tcpreplay */

/* Max Transmission Unit of standard ethernet  don't forget *frames* are MTU + L2 header! */
#define DEFAULT_MTU 1500

/* tell libpcap to capture the entire packet
 * this is the maximum size supported by libpcap
 * (https://github.com/the-tcpdump-group/libpcap/blob/master/pcap-int.h#L99-L125)
 */
#define MAX_SNAPLEN 262144

/* snap length plus some room for adding a
 * couple VLAN headers or a L2 header
 */
#define MAXPACKET (MAX_SNAPLEN + 22)

#define PACKET_HEADROOM 512 /* additional headroom allocated for packets to accommodate editing */

#define DNS_RESOLVE 1
#define DNS_DONT_RESOLVE 0

#define RESOLVE 0        /* disable dns lookups */
#define BPF_OPTIMIZE 1   /* default is to optimize bpf program */
#define PCAP_TIMEOUT 100 /* 100ms pcap_open_live timeout */

#define DEFAULT_FUZZ_FACTOR 8

/* HP-UX already defines TRUE/FALSE */
#ifndef TRUE
typedef enum bool_e { FALSE = 0, TRUE } bool_t;
#endif

#define EBUF_SIZE 1024 /* size of our error buffers */
#define MAC_SIZE 7     /* size of the mac[] buffer */

typedef enum pad_e { PAD_PACKET, TRUNC_PACKET } pad_t;

#define DNS_QUERY_FLAG 0x8000

typedef enum direction_e { DIR_UNKNOWN = -1, DIR_CLIENT = 0, DIR_SERVER = 1, DIR_ANY = 2 } direction_t;

typedef enum tcpprep_mode_e {
    ERROR_MODE,  /* Some kind of error has occurred */
    CIDR_MODE,   /* single pass, CIDR netblock */
    REGEX_MODE,  /* single pass, regex */
    PORT_MODE,   /* single pass, use src/dst ports to split */
    MAC_MODE,    /* single pass, use src mac to split */
    FIRST_MODE,  /* single pass, use first seen to split */
    AUTO_MODE,   /* first pass through in auto mode */
    ROUTER_MODE, /* second pass through in router mode */
    BRIDGE_MODE, /* second pass through in bridge mode */
    SERVER_MODE, /* second pass through in server (router) mode */
    CLIENT_MODE  /* second pass through in client (router) mode */
} tcpprep_mode_t;

#define BROADCAST_MAC "\xff\xff\xff\xff\xff\xff"
#define IPV4_MULTICAST_MAC "\x01\x00\x5e\x00\x00\x00"
#define IPV6_MULTICAST_MAC "\x33\x33\x00\x00\x00\x00"
#define IPV4_VRRP "\x00\x00\x50\x00\x01\x00"
#define IPV6_VRRP "\x00\x00\x50\x00\x02\x00"

/* MAC macros for printf */
#define MAC_FORMAT "%02X:%02X:%02X:%02X:%02X:%02X"
#define MAC_STR(x) x[0], x[1], x[2], x[3], x[4], x[5]

/* struct timeval print structs */
#ifdef HAVE_DARWIN
/* Darwin defines usec as an __int32_t, not unsigned long. */
#define TIMEVAL_FORMAT "%lus %dusec"
#else
#define TIMEVAL_FORMAT "%lus %luusec"
#endif
#define TIMESPEC_FORMAT "%lus %lunsec"

/* force a word or half-word swap on both Big and Little endian systems */
#ifndef SWAPLONG
#define SWAPLONG(y) ((((y)&0xff) << 24) | (((y)&0xff00) << 8) | (((y)&0xff0000) >> 8) | (((y) >> 24) & 0xff))
#endif

#ifndef SWAPSHORT
#define SWAPSHORT(y) ((((y)&0xff) << 8) | ((u_short)((y)&0xff00) >> 8))
#endif

/* converts a 64bit int to network byte order */
#if !defined ntohll && !defined htonll
#ifndef HAVE_NTOHLL
#ifdef WORDS_BIGENDIAN
#define ntohll(x) (x)
#define htonll(x) (x)
#else
/* stolen from http://www.codeproject.com/cpp/endianness.asp */
#define ntohll(x) (((u_int64_t)(ntohl((int)((x << 32) >> 32))) << 32) | (unsigned int)ntohl(((int)(x >> 32))))
#define htonll(x) ntohll(x)
#endif /* WORDS_BIGENDIAN */
#endif /* HAVE_NTOHLL */
#endif /* !ntohll && !htonll */

#define DEBUG_INFO 1   /* informational only, lessthan 1 line per packet */
#define DEBUG_BASIC 2  /* limited debugging, one line per packet */
#define DEBUG_DETAIL 3 /* more detailed, a few lines per packet */
#define DEBUG_MORE 4   /* even more detail */
#define DEBUG_CODE 5   /* examines code & values, many lines per packet */

#if defined HAVE_IOPERM && defined __i386_
#define HAVE_IOPORT_SLEEP
#endif

/* Win32 doesn't know about PF_INET6 */
#ifndef PF_INET6
#ifdef AF_INET6
#define PF_INET6 AF_INET6
#else
#define PF_INET6 30 /* stolen from OS/X */
#endif
#endif

/* convert IPv6 Extension Header Len value to bytes */
#define IPV6_EXTLEN_TO_BYTES(x) ((x * 4) + 8)

#ifndef HAVE_UINT8_T
typedef u_int8_t uint8_t typedef u_int16_t uint16_t typedef u_int32_t uint32_t
#endif

/* Support for flexible arrays. */
#undef __flexarr
#if defined(__GNUC__) && ((__GNUC__ > 2) || (__GNUC__ == 2 && __GNUC_MINOR__ >= 97))
/* GCC 2.97 supports C99 flexible array members.  */
#define __flexarr []
#else
#ifdef __GNUC__
#define __flexarr [0]
#else
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#define __flexarr []
#elif defined(_WIN32)
/* MS VC++ */
#define __flexarr []
#else
/* Some other non-C99 compiler. Approximate with [1]. */
#define __flexarr [1]
#endif
#endif
#endif


/* Time converters */
#define SEC_TO_MILLISEC(x) (x * 1000)
#define SEC_TO_MICROSEC(x) (x * 1000000)
#define SEC_TO_NANOSEC(x) ((u_int64_t)x * 1000000000)

#define MILLISEC_TO_SEC(x) (x / 1000)
#define MICROSEC_TO_SEC(x) (x / 1000000)
#define NANOSEC_TO_SEC(x) ((u_int64_t)x / 1000000000)

#define TIMEVAL_TO_MILLISEC(x)  (((x)->tv_sec * 1000) + ((x)->tv_usec / 1000))
#define TIMEVAL_TO_MICROSEC(x)  (((x)->tv_sec * 1000000) + (x)->tv_usec)
#define TIMEVAL_TO_NANOSEC(x)   ((u_int64_t)((x)->tv_sec * 1000000000) + ((u_int64_t)(x)->tv_usec * 1000))

#define MILLISEC_TO_TIMEVAL(x, tv)                                                                                     \
    do {                                                                                                               \
        (tv)->tv_sec = (x) / 1000;                                                                                     \
        (tv)->tv_usec = (x * 1000) - ((tv)->tv_sec * 1000000);                                                         \
    } while (0)

#define MICROSEC_TO_TIMEVAL(x, tv)                                                                                     \
    do {                                                                                                               \
        (tv)->tv_sec = (x) / 1000000;                                                                                  \
        (tv)->tv_usec = (x) - ((tv)->tv_sec * 1000000);                                                                \
    } while (0)

#define NANOSEC_TO_TIMEVAL(x, tv)                                                                                      \
    do {                                                                                                               \
        (tv)->tv_sec = (x) / 1000000000;                                                                               \
        (tv)->tv_usec = ((x) % 1000000000) / 1000;                                                                     \
    } while (0)

#define NANOSEC_TO_TIMESPEC(x, ts)                                                                                     \
    do {                                                                                                               \
        (ts)->tv_sec = (x) / 1000000000;                                                                               \
        (ts)->tv_nsec = (x) % 1000000000;                                                                              \
    } while (0)

#define TIMESPEC_TO_MILLISEC(x) (((x)->tv_sec * 1000) + ((x)->tv_nsec / 1000000))
#define TIMESPEC_TO_MICROSEC(x) (((x)->tv_sec * 1000000) + (x)->tv_nsec / 1000)
#define TIMESPEC_TO_NANOSEC(x) ((u_int64_t)((x)->tv_sec * 1000000000) + ((u_int64_t)(x)->tv_nsec))

#define TIMEVAL_SET(a, b)                                                                                              \
    do {                                                                                                               \
        (a)->tv_sec = (b)->tv_sec;                                                                                     \
        (a)->tv_usec = (b)->tv_usec;                                                                                   \
    } while (0)

#define TIMESPEC_SET(a, b)                                                                                             \
    do {                                                                                                               \
        (a)->tv_sec = (b)->tv_sec;                                                                                     \
        (a)->tv_nsec = (b)->tv_nsec;                                                                                   \
    } while (0)

/* libpcap that supports it, puts nanosecond values in tv_usec when pcap file is read with nanosec precision,
 * and so tv_usec is directly copied to tv_nsec.
 * But older versions do that do not support nanosecond precision need to multiply tv_usec by 1000 to convert
 * to tv_nsec.
 */
#define PCAP_TIMEVAL_TO_TIMESPEC_SET(a, b)                              \
    do {                                                                \
        (b)->tv_sec = (a)->tv_sec;                                      \
        (b)->tv_nsec = (a)->tv_usec * PCAP_TSTAMP_US_TO_NS_MULTIPLIER;  \
    } while(0)

#define PCAP_TIMEVAL_TO_TIMEVAL_SET(a, b)                               \
    do {                                                                \
        (b)->tv_sec = (a)->tv_sec;                                      \
        (b)->tv_usec = (a)->tv_usec / PCAP_TSTAMP_US_TO_US_DIVISOR;     \
    } while(0)

/*
 * Help suppress some compiler warnings
 * No problem if variable is actually used 
*/
#ifdef UNUSED
#elif defined(__GNUC__)
#define UNUSED(x) x __attribute__((unused))
#elif defined(__LCLINT__)
#define UNUSED(x) /*@unused@*/ x
#else
#define UNUSED(x) x
#endif

#ifndef max
#define max(a, b)                                                                                                      \
    ({                                                                                                                 \
        __typeof__(a) _a = (a);                                                                                        \
        __typeof__(b) _b = (b);                                                                                        \
        _a > _b ? _a : _b;                                                                                             \
    })
#endif

#ifndef min
#define min(a, b)                                                                                                      \
    ({                                                                                                                 \
        __typeof__(a) _a = (a);                                                                                        \
        __typeof__(b) _b = (b);                                                                                        \
        _a > _b ? _b : _a;                                                                                             \
    })
#endif
