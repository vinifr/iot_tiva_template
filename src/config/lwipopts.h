/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 * Modified by: Joakim Myrland
 * website:      www.LDA.as
 * email:        joakim.myrland@LDA.as
 * project:      https://github.com/Lindem-Data-Acquisition-AS/iot_tiva_template/
 *
 */


 
#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

#include "FreeRTOS.h"

#define malloc pvPortMalloc
#define free vPortFree



/*
   -----------------------------------------------
   ---------- TM4C129 lwIP port options ----------
   -----------------------------------------------
*/
#define PHY_PHYS_ADDR                   0
#define EMAC_PHY_CONFIG (EMAC_PHY_TYPE_INTERNAL | EMAC_PHY_INT_MDIX_EN |      \
                         EMAC_PHY_AN_100B_T_FULL_DUPLEX)
#define NUM_RX_DESCRIPTORS              8           // default is 4
#define NUM_TX_DESCRIPTORS              24          // default is 8



/*
   ----------------------------------
   ---------- PTPD options ----------
   ----------------------------------
*/
#define LWIP_PTPD                       0



/*
   -----------------------------------------------
   ---------- Platform specific locking ----------
   -----------------------------------------------
*/
#define SYS_LIGHTWEIGHT_PROT            1           // default is 0
#define NO_SYS                          0
#define NO_SYS_NO_TIMERS                0

#if !NO_SYS
#define LWIP_COMPAT_MUTEX               1
#define RTOS_FREERTOS                   1
#endif /* !NO_SYS */

#define MEMCPY(dst,src,len)             memcpy(dst,src,len)
#define SMEMCPY(dst,src,len)            memcpy(dst,src,len)



/*
   ------------------------------------
   ---------- Memory options ----------
   ------------------------------------
*/
#define MEM_LIBC_MALLOC                 0
#define MEMP_MEM_MALLOC                 0
#define MEM_ALIGNMENT                   4           // default is 1
#define MEM_SIZE                        (32 * 1024) // default is 1600, was 16K
#define MEMP_SEPARATE_POOLS             0
#define MEMP_OVERFLOW_CHECK             0
#define MEMP_SANITY_CHECK               0
#define MEM_USE_POOLS                   0
#define MEM_USE_POOLS_TRY_BIGGER_POOL   0
#define MEMP_USE_CUSTOM_POOLS           MEM_USE_POOLS
#define LWIP_ALLOW_MEM_FREE_FROM_OTHER_CONTEXT 0



/*
   ------------------------------------------------
   ---------- Internal Memory Pool Sizes ----------
   ------------------------------------------------
*/
#define MEMP_NUM_PBUF                   16
#define MEMP_NUM_RAW_PCB                4
#define MEMP_NUM_UDP_PCB                4
#define MEMP_NUM_TCP_PCB                5
#define MEMP_NUM_TCP_PCB_LISTEN         8
#define MEMP_NUM_TCP_SEG                16
#define MEMP_NUM_REASSDATA              5
#define MEMP_NUM_FRAG_PBUF              15
#define MEMP_NUM_ARP_QUEUE              30
#define MEMP_NUM_IGMP_GROUP             8
#define MEMP_NUM_SYS_TIMEOUT            10    // (LWIP_TCP + IP_REASSEMBLY + LWIP_ARP + (2*LWIP_DHCP) + LWIP_AUTOIP + LWIP_IGMP + LWIP_DNS + PPP_SUPPORT)
#define MEMP_NUM_NETBUF                 4
#define MEMP_NUM_NETCONN                12
#define MEMP_NUM_TCPIP_MSG_API          8
#define MEMP_NUM_TCPIP_MSG_INPKT        8
#define MEMP_NUM_SNMP_NODE              50
#define MEMP_NUM_SNMP_ROOTNODE          30
#define MEMP_NUM_SNMP_VARBIND           2
#define MEMP_NUM_SNMP_VALUE             3
#define MEMP_NUM_NETDB                  1
#define MEMP_NUM_LOCALHOSTLIST          1
#define MEMP_NUM_PPPOE_INTERFACES       1
#define PBUF_POOL_SIZE                  16



/*
   ---------------------------------
   ---------- ARP options ----------
   ---------------------------------
*/
#define LWIP_ARP                        1

#if LWIP_ARP
#define ARP_TABLE_SIZE                  10
#define ARP_QUEUEING                    1           // default is 0
#define ETHARP_TRUST_IP_MAC             1           // default is 0
#define ETHARP_SUPPORT_VLAN             0
#define LWIP_ETHERNET                   (LWIP_ARP || PPPOE_SUPPORT)
#define ETH_PAD_SIZE                    0
#define ETHARP_SUPPORT_STATIC_ENTRIES   0
#endif



/*
   --------------------------------
   ---------- IP options ----------
   --------------------------------
*/
#define IP_FORWARD                      0
#define IP_OPTIONS_ALLOWED              1
#define IP_REASSEMBLY                   1           // default is 1
#define IP_FRAG                         1           // default is 1
#define IP_REASS_MAXAGE                 3
#define IP_REASS_MAX_PBUFS              10
#define IP_FRAG_USES_STATIC_BUF         1           // default is 0

#if IP_FRAG_USES_STATIC_BUF && !defined(IP_FRAG_MAX_MTU)
#define IP_FRAG_MAX_MTU                 1500
#endif

#define IP_DEFAULT_TTL                  255
#define IP_SOF_BROADCAST                0
#define IP_SOF_BROADCAST_RECV           0
#define IP_FORWARD_ALLOW_TX_ON_RX_NETIF 0
#define LWIP_RANDOMIZE_INITIAL_LOCAL_PORTS 0



/*
   ----------------------------------
   ---------- ICMP options ----------
   ----------------------------------
*/
#define LWIP_ICMP                       1

#if LWIP_ICMP
#define ICMP_TTL                       (IP_DEFAULT_TTL)
#define LWIP_BROADCAST_PING             0
#define LWIP_MULTICAST_PING             0
#endif



/*
   ---------------------------------
   ---------- RAW options ----------
   ---------------------------------
*/
#define LWIP_RAW                        1
#define RAW_TTL                        (IP_DEFAULT_TTL)



/*
   ----------------------------------
   ---------- DHCP options ----------
   ----------------------------------
*/
#define LWIP_DHCP                       1           // default is 0
#define DHCP_DOES_ARP_CHECK             ((LWIP_DHCP) && (LWIP_ARP))



/*
   ------------------------------------
   ---------- AUTOIP options ----------
   ------------------------------------
*/
#define LWIP_AUTOIP                     1           // default is 0
#define LWIP_DHCP_AUTOIP_COOP           ((LWIP_DHCP) && (LWIP_AUTOIP))  // default is 0
#define LWIP_DHCP_AUTOIP_COOP_TRIES     5           // default is 9



/*
   ----------------------------------
   ---------- SNMP options ----------
   ----------------------------------
*/
#define LWIP_SNMP                       0

#if LWIP_SNMP
#define SNMP_CONCURRENT_REQUESTS        1
#define SNMP_TRAP_DESTINATIONS          1
#define SNMP_PRIVATE_MIB                0
#define SNMP_SAFE_REQUESTS              1
#define SNMP_MAX_OCTET_STRING_LEN       127
#define SNMP_MAX_TREE_DEPTH             15
#define SNMP_MAX_VALUE_SIZE             LWIP_MAX((SNMP_MAX_OCTET_STRING_LEN)+1, sizeof(s32_t)*(SNMP_MAX_TREE_DEPTH))
#endif



/*
   ----------------------------------
   ---------- IGMP options ----------
   ----------------------------------
*/
#define LWIP_IGMP                       0



/*
   ----------------------------------
   ---------- DNS options -----------
   ----------------------------------
*/
#define LWIP_DNS                        0

#if LWIP_DNS
#define DNS_TABLE_SIZE                  4
#define DNS_MAX_NAME_LENGTH             256
#define DNS_MAX_SERVERS                 2
#define DNS_DOES_NAME_CHECK             1
#define DNS_MSG_SIZE                    512
#define DNS_LOCAL_HOSTLIST              0
#define DNS_LOCAL_HOSTLIST_IS_DYNAMIC   0
#endif


/*
   ---------------------------------
   ---------- UDP options ----------
   ---------------------------------
*/
#define LWIP_UDP                        1
#define LWIP_UDPLITE                    0
#define UDP_TTL                         (IP_DEFAULT_TTL)
#define LWIP_NETBUF_RECVINFO            0



/*
   ---------------------------------
   ---------- TCP options ----------
   ---------------------------------
*/
#define LWIP_TCP                        1

#if LWIP_TCP
#define TCP_TTL                         (IP_DEFAULT_TTL)
#define TCP_WND                         (4 * TCP_MSS)   // default is (4 * TCP_MSS)
#define TCP_MAXRTX                      12
#define TCP_SYNMAXRTX                   6
#define TCP_QUEUE_OOSEQ                 (LWIP_TCP)
#define TCP_MSS                         1460            // default is 536
#define TCP_CALCULATE_EFF_SEND_MSS      1
#define TCP_SND_BUF                     (2 * TCP_MSS)   // default is (2 * TCP_MSS)
#define TCP_SND_QUEUELEN                ((4 * (TCP_SND_BUF) + (TCP_MSS - 1))/(TCP_MSS))
#define TCP_SNDLOWAT                    ((TCP_SND_BUF)/2)
#define TCP_SNDQUEUELOWAT               ((TCP_SND_QUEUELEN)/2)
#define TCP_OOSEQ_MAX_BYTES             0
#define TCP_OOSEQ_MAX_PBUFS             0
#define TCP_LISTEN_BACKLOG              0
#if TCP_LISTEN_BACKLOG
#error "TCP_LISTEN_BACKLOG must be set to 0. Bug in lwIP v1.4.1. http://comments.gmane.org/gmane.network.lwip.general/7286"
#endif
#define TCP_DEFAULT_LISTEN_BACKLOG      0xff
#define TCP_OVERSIZE                    TCP_MSS
#define LWIP_TCP_TIMESTAMPS             0
#define TCP_WND_UPDATE_THRESHOLD        (TCP_WND / 4)

#endif /* LWIP_TCP */



/*
   ---------------------------------
   ---------- API options ----------
   ---------------------------------
*/
#define LWIP_EVENT_API                  0
#define LWIP_CALLBACK_API               1

#if (LWIP_EVENT_API) && (LWIP_CALLBACK_API)
#error "Only one of EVENT_API or CALLBACK_API can be defined. See opt.h"
#endif


/*
   ----------------------------------
   ---------- Pbuf options ----------
   ----------------------------------
*/
#define PBUF_LINK_HLEN                  16
#define PBUF_POOL_BUFSIZE               LWIP_MEM_ALIGN_SIZE(TCP_MSS+40+PBUF_LINK_HLEN)



/*
   ------------------------------------------------
   ---------- Network Interfaces options ----------
   ------------------------------------------------
*/
#define LWIP_NETIF_HOSTNAME             0
#define LWIP_NETIF_API                  1
#define LWIP_NETIF_STATUS_CALLBACK      0
#define LWIP_NETIF_LINK_CALLBACK        0
#define LWIP_NETIF_REMOVE_CALLBACK      0
#define LWIP_NETIF_HWADDRHINT           0
#define LWIP_NETIF_LOOPBACK             0
#define LWIP_LOOPBACK_MAX_PBUFS         0
#define LWIP_NETIF_LOOPBACK_MULTITHREADING    (!NO_SYS)
#define LWIP_NETIF_TX_SINGLE_PBUF             0



/*
   ------------------------------------
   ---------- LOOPIF options ----------
   ------------------------------------
*/
#define LWIP_HAVE_LOOPIF                0
//#define LWIP_LOOPIF_MULTITHREADING      1



/*
   ------------------------------------
   ---------- SLIPIF options ----------
   ------------------------------------
*/
#define LWIP_HAVE_SLIPIF                0



/*
   ------------------------------------
   ---------- Thread options ----------
   ------------------------------------
*/
#define TCPIP_THREAD_NAME              "tcpip_thread"
#define TCPIP_THREAD_STACKSIZE          2048
#define TCPIP_THREAD_PRIO               3
#define TCPIP_MBOX_SIZE                 32
// #define SLIPIF_THREAD_NAME             "slipif_loop"
// #define SLIPIF_THREAD_STACKSIZE         0
// #define SLIPIF_THREAD_PRIO              3
// #define PPP_THREAD_NAME                "pppInputThread"
// #define PPP_THREAD_STACKSIZE            0
// #define PPP_THREAD_PRIO                 3
#define DEFAULT_THREAD_NAME            "lwIP"
#define DEFAULT_THREAD_STACKSIZE        2048
#define DEFAULT_THREAD_PRIO             3
#define DEFAULT_RAW_RECVMBOX_SIZE       4
#define DEFAULT_UDP_RECVMBOX_SIZE       4
#define DEFAULT_TCP_RECVMBOX_SIZE       40
#define DEFAULT_ACCEPTMBOX_SIZE         4



/*
   ----------------------------------------------
   ---------- Sequential layer options ----------
   ----------------------------------------------
*/
#define LWIP_NETCONN                    1
#define LWIP_TCPIP_TIMEOUT              1



/*
   ------------------------------------
   ---------- Socket options ----------
   ------------------------------------
*/
#define LWIP_SOCKET                     1

#if LWIP_SOCKET
#define LWIP_COMPAT_SOCKETS             0
#define LWIP_POSIX_SOCKETS_IO_NAMES     1
#define LWIP_TCP_KEEPALIVE              0
#define LWIP_SO_SNDTIMEO                0
#define LWIP_SO_RCVTIMEO                0
#define LWIP_SO_RCVBUF                  0
#define RECV_BUFSIZE_DEFAULT            INT_MAX
#define SO_REUSE                        0
#define SO_REUSE_RXTOALL                0
#endif



/*
   ----------------------------------------
   ---------- Statistics options ----------
   ----------------------------------------
*/
#define LWIP_STATS                      1

#if LWIP_STATS
#define LWIP_STATS_DISPLAY              0
#define LINK_STATS                      1
#define ETHARP_STATS                    (LWIP_ARP)
#define IP_STATS                        1
#define IPFRAG_STATS                    (IP_REASSEMBLY || IP_FRAG)
#define ICMP_STATS                      1
#define IGMP_STATS                      (LWIP_IGMP)
#define UDP_STATS                       (LWIP_UDP)
#define TCP_STATS                       (LWIP_TCP)
#define MEM_STATS                       ((MEM_LIBC_MALLOC == 0) && (MEM_USE_POOLS == 0))
#define MEMP_STATS                      (MEMP_MEM_MALLOC == 0)
#define PBUF_STATS                      1
#define SYS_STATS                       (NO_SYS == 0)
#define LWIP_STATS_LARGE                1
#endif /* LWIP_STATS */



/*
   ---------------------------------
   ---------- PPP options ----------
   ---------------------------------
*/
#define PPP_SUPPORT                     0
#define PPPOE_SUPPORT                   0
#define PPPOS_SUPPORT                   PPP_SUPPORT

#if PPP_SUPPORT

#define NUM_PPP                         1
#define PAP_SUPPORT                     0
#define CHAP_SUPPORT                    0
#define MSCHAP_SUPPORT                  0
#define CBCP_SUPPORT                    0
#define CCP_SUPPORT                     0
#define VJ_SUPPORT                      0
#define MD5_SUPPORT                     0

/*
 * Timeouts
 */
#define FSM_DEFTIMEOUT                  6       /* Timeout time in seconds */
#define FSM_DEFMAXTERMREQS              2       /* Maximum Terminate-Request transmissions */
#define FSM_DEFMAXCONFREQS              10      /* Maximum Configure-Request transmissions */
#define FSM_DEFMAXNAKLOOPS              5       /* Maximum number of nak loops */
#define UPAP_DEFTIMEOUT                 6       /* Timeout (seconds) for retransmitting req */
#define UPAP_DEFREQTIME                 30      /* Time to wait for auth-req from peer */
#define CHAP_DEFTIMEOUT                 6       /* Timeout time in seconds */
#define CHAP_DEFTRANSMITS               10      /* max # times to send challenge */
#define LCP_ECHOINTERVAL                0
#define LCP_MAXECHOFAILS                3
#define PPP_MAXIDLEFLAG                 100

/*
 * Packet sizes
 *
 * Note - lcp shouldn't be allowed to negotiate stuff outside these
 *    limits.  See lcp.h in the pppd directory.
 * (XXX - these constants should simply be shared by lcp.c instead
 *    of living in lcp.h)
 */
#define PPP_MTU                         1500    /* Default MTU (size of Info field) */
/* #define PPP_MAXMTU  65535 - (PPP_HDRLEN + PPP_FCSLEN) */
#define PPP_MAXMTU                      1500    /* Largest MTU we allow */
#define PPP_MINMTU                      64
#define PPP_MRU                         1500    /* default MRU = max length of info field */
#define PPP_MAXMRU                      1500    /* Largest MRU we allow */
#define PPP_DEFMRU                      296     /* Try for this */
#define PPP_MINMRU                      128     /* No MRUs below this */
#define MAXNAMELEN                      256     /* max length of hostname or name for auth */
#define MAXSECRETLEN                    256     /* max length of password or secret */
    
#endif /* PPP_SUPPORT */



/*
   --------------------------------------
   ---------- Checksum options ----------
   --------------------------------------
*/
#define CHECKSUM_GEN_IP                 1
#define CHECKSUM_GEN_UDP                1
#define CHECKSUM_GEN_TCP                1
#define CHECKSUM_GEN_ICMP               1
#define CHECKSUM_CHECK_IP               1
#define CHECKSUM_CHECK_UDP              1
#define CHECKSUM_CHECK_TCP              1
#define LWIP_CHECKSUM_ON_COPY           0



//*****************************************************************************
//
// ---------- Debugging options ----------
//
//*****************************************************************************
#ifdef DEBUG
#define U8_F "c"
#define S8_F "c"
#define X8_F "02x"
#define U16_F "u"
#define S16_F "d"
#define X16_F "x"
#define U32_F "u"
#define S32_F "d"
#define X32_F "x"
extern void UARTprintf(const char *pcString, ...);
#define LWIP_PLATFORM_DIAG(msg) UARTprintf msg
#define LWIP_PLATFORM_ASSERT(msg)                                             \
    UARTprintf("ASSERT FAIL at line %d of %s: %s\n", __LINE__, __FILE__, msg)
#define LWIP_DEBUG 1
#endif

//#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_OFF
//#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_ON
#define LWIP_DBG_MIN_LEVEL				  LWIP_DBG_LEVEL_ALL
//#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_WARNING
//#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_SERIOUS
//#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_SEVERE

#define LWIP_DBG_TYPES_ON               LWIP_DBG_ON
//#define LWIP_DBG_TYPES_ON               (LWIP_DBG_ON|LWIP_DBG_TRACE|        \
                                           LWIP_DBG_STATE|LWIP_DBG_FRESH)

#define LWIP_STATS						1
#define LWIP_STATS_DISPLAY				1
#define SYS_STATS						1
#define MEM_STATS						1
#define MEMP_STATS						1
//#define PBUF_STATS						1

#define NETIFUP_DEBUG						LWIP_DBG_ON
//#define ETHARP_DEBUG                    LWIP_DBG_ON     // default is OFF
//#define NETIF_DEBUG                     LWIP_DBG_ON
//#define PBUF_DEBUG                      LWIP_DBG_ON
//#define API_LIB_DEBUG                   LWIP_DBG_OFF
//#define API_MSG_DEBUG                   LWIP_DBG_OFF
#define SOCKETS_DEBUG                   LWIP_DBG_ON
#define SOCKETS_RECV_DEBUG				LWIP_DBG_OFF
#define HTTPD_DEBUG                     LWIP_DBG_ON
#define ICMP_DEBUG                      LWIP_DBG_ON
//#define IGMP_DEBUG                      LWIP_DBG_OFF
//#define INET_DEBUG                      LWIP_DBG_OFF
//#define IP_DEBUG                        LWIP_DBG_ON
//#define IP6_DEBUG						LWIP_DBG_ON
//#define IP_REASS_DEBUG                  LWIP_DBG_OFF
//#define RAW_DEBUG                       LWIP_DBG_OFF
#define MEM_DEBUG                       LWIP_DBG_ON
#define MEMP_DEBUG                      LWIP_DBG_ON
//#define SYS_DEBUG                       LWIP_DBG_OFF
//#define TCP_DEBUG                       LWIP_DBG_ON
//#define TCP_INPUT_DEBUG                 LWIP_DBG_ON
//#define TCP_FR_DEBUG                    LWIP_DBG_OFF
//#define TCP_RTO_DEBUG                   LWIP_DBG_OFF
//#define TCP_CWND_DEBUG                  LWIP_DBG_OFF
//#define TCP_WND_DEBUG                   LWIP_DBG_OFF
//#define TCP_OUTPUT_DEBUG                LWIP_DBG_ON
//#define TCP_RST_DEBUG                   LWIP_DBG_ON
//#define TCP_QLEN_DEBUG                  LWIP_DBG_OFF
//#define UDP_DEBUG                       LWIP_DBG_ON     // default is OFF
//#define TCPIP_DEBUG                     LWIP_DBG_ON
//#define PPP_DEBUG                       LWIP_DBG_OFF
//#define SLIP_DEBUG                      LWIP_DBG_OFF
//#define DHCP_DEBUG                      LWIP_DBG_ON     // default is OFF
//#define AUTOIP_DEBUG                    LWIP_DBG_ON
//#define SNMP_MSG_DEBUG                  LWIP_DBG_OFF
//#define SNMP_MIB_DEBUG                  LWIP_DBG_OFF
//#define DNS_DEBUG                       LWIP_DBG_OFF
#define HTTPSD_DEBUG					LWIP_DBG_OFF

#endif /* __LWIPOPTS_H__ */
