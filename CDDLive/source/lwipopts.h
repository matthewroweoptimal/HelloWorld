/**
  ******************************************************************************
  * @file    lwipopts.h
  * This file is based on \src\include\lwip\opt.h
  ******************************************************************************
   * Copyright (c) 2013-2016, Freescale Semiconductor, Inc.
   * Copyright 2016-2017 NXP
   * All rights reserved.
   *
   *
   * SPDX-License-Identifier: BSD-3-Clause
   */

#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__


#if USE_RTOS
// Define LWIP_USE_FREERTOS_HEAP to force LWIP to used FreeRTOS heap rather than stdlib heap
// Note : Thread safety built in, unlike stdlib implementation.
// Also more efficient use of memory combining everything into FreeRTOS ucHeap[ 114kb ]
#define MEM_LIBC_MALLOC			1	// Define to use Heap management rather than internal memory pools
#ifdef TEST_SERVER
	#define LWIP_USE_FREERTOS_HEAP	0	// Test Server running under docker uses stdlib heap routings
#else
	#define LWIP_USE_FREERTOS_HEAP	1
#endif

// Define LWIP_DEBUG_HEAP_USAGE for debugging calls to heap management functions
#define LWIP_DEBUG_HEAP_USAGE	0

/**
 * SYS_LIGHTWEIGHT_PROT==1: if you want inter-task protection for certain
 * critical regions during buffer allocation, deallocation and memory
 * allocation and deallocation.
 */
#define SYS_LIGHTWEIGHT_PROT    1

/**
 * NO_SYS==0: Use RTOS
 */
#define NO_SYS                  0
/**
 * LWIP_NETCONN==1: Enable Netconn API (require to use api_lib.c)
 */
#define LWIP_NETCONN            1
/**
 * LWIP_SOCKET==1: Enable Socket API (require to use sockets.c)
 */
#define LWIP_SOCKET             1

/**
 * LWIP_SO_RCVTIMEO==1: Enable receive timeout for sockets/netconns and
 * SO_RCVTIMEO processing.
 */
#define LWIP_SO_RCVTIMEO        1

#else
/**
 * NO_SYS==1: Bare metal lwIP
 */
#define NO_SYS                  1
/**
 * LWIP_NETCONN==0: Disable Netconn API (require to use api_lib.c)
 */
#define LWIP_NETCONN            0
/**
 * LWIP_SOCKET==0: Disable Socket API (require to use sockets.c)
 */
#define LWIP_SOCKET             0

#endif
/* ---------- Memory options ---------- */
/**
 * MEM_ALIGNMENT: should be set to the alignment of the CPU
 *    4 byte alignment -> #define MEM_ALIGNMENT 4
 *    2 byte alignment -> #define MEM_ALIGNMENT 2
 */
#ifndef MEM_ALIGNMENT
#define MEM_ALIGNMENT           4
#endif

/**
 * MEM_SIZE: the size of the heap memory. If the application will send
 * a lot of data that needs to be copied, this should be set high.
 */
#ifndef MEM_SIZE
#define MEM_SIZE                (2*1024) // Breeze was (32*1024)
#endif
#define MEMP_NUM_NETCONN         20 // Breeze was 20

/* MEMP_NUM_PBUF: the number of memp struct pbufs. If the application
   sends a lot of data out of ROM (or other static memory), this
   should be set high. */
#ifndef MEMP_NUM_PBUF
#define MEMP_NUM_PBUF           15
#endif
/* MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
   per active UDP "connection". */
#ifndef MEMP_NUM_UDP_PCB
#define MEMP_NUM_UDP_PCB        4 // Breeze was 8
#endif
/* MEMP_NUM_TCP_PCB: the number of simultaneously active TCP
   connections. */
#ifndef MEMP_NUM_TCP_PCB
#define MEMP_NUM_TCP_PCB        11 // Breeze was 11
#endif
/* MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP
   connections. */
#ifndef MEMP_NUM_TCP_PCB_LISTEN
#define MEMP_NUM_TCP_PCB_LISTEN 11 // Breeze was 11
#endif
/* MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP
   segments. */
#ifndef MEMP_NUM_TCP_SEG
#define MEMP_NUM_TCP_SEG        18 // Breeze was 18
#endif
/* MEMP_NUM_SYS_TIMEOUT: the number of simultaneously active
   timeouts. */
#ifndef MEMP_NUM_SYS_TIMEOUT
#define MEMP_NUM_SYS_TIMEOUT    14  // Increased from 10 since AUTO-IP configured
#endif

/* ---------- Pbuf options ---------- */
/* PBUF_POOL_SIZE: the number of buffers in the pbuf pool. */
#ifndef PBUF_POOL_SIZE
#define PBUF_POOL_SIZE          20
#endif

/* PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool. */
/* Default value is defined in lwip\src\include\lwip\opt.h as LWIP_MEM_ALIGN_SIZE(TCP_MSS+40+PBUF_LINK_ENCAPSULATION_HLEN+PBUF_LINK_HLEN)*/

/* ---------- TCP options ---------- */
#ifndef LWIP_TCP
#define LWIP_TCP                1
#endif

#ifndef TCP_TTL
#define TCP_TTL                 255
#endif

/* Controls if TCP should queue segments that arrive out of
   order. Define to 0 if your device is low on memory. */
#ifndef TCP_QUEUE_OOSEQ
#define TCP_QUEUE_OOSEQ         0
#endif

/* TCP Maximum segment size. */
#ifndef TCP_MSS
#define TCP_MSS                 (1500 - 40)	  /* TCP_MSS = (Ethernet MTU - IP header size - TCP header size) */
#endif

/* TCP sender buffer space (bytes). */
#ifndef TCP_SND_BUF
#define TCP_SND_BUF             (6*TCP_MSS) // Breeze was (6*TCP_MSS)
#endif

/* TCP sender buffer space (pbufs). This must be at least = 2 *
   TCP_SND_BUF/TCP_MSS for things to work. */
#ifndef TCP_SND_QUEUELEN
#define TCP_SND_QUEUELEN        (3 * TCP_SND_BUF)/TCP_MSS //6
#endif

/* TCP receive window. */
#ifndef TCP_WND
#define TCP_WND                 (2*TCP_MSS)
#endif

/* Enable backlog*/
#ifndef TCP_LISTEN_BACKLOG
#define TCP_LISTEN_BACKLOG      1
#endif

/* ---------- Network Interfaces options ---------- */
/* Support netif api (in netifapi.c). */
#ifndef LWIP_NETIF_API
#define LWIP_NETIF_API 1
#endif

/* ---------- ICMP options ---------- */
#ifndef LWIP_ICMP
#define LWIP_ICMP                       1
#endif


/* ---------- DHCP options ---------- */
/* Define LWIP_DHCP to 1 if you want DHCP configuration of
   interfaces. DHCP is not implemented in lwIP 0.5.1, however, so
   turning this on does currently not work. */
#ifndef LWIP_DHCP
#define LWIP_DHCP               1
#endif

// Configure Auto-IP where a 169.254.x.x address will be configured in the absence of a DHCP server
#define LWIP_AUTOIP						1
#define LWIP_DHCP_AUTOIP_COOP			1   // Allow DHCP and AUTOIP to both be enabled at the same time
#define LWIP_DHCP_AUTOIP_COOP_TRIES     4	// Get Auto-IP address after 4 tries if no DHCP server present

/* ---------- UDP options ---------- */
#ifndef LWIP_UDP
#define LWIP_UDP                1
#endif
#ifndef UDP_TTL
#define UDP_TTL                 255
#endif

/* ---------- Statistics options ---------- */
#ifndef LWIP_STATS
#define LWIP_STATS 1
#endif
#ifndef LWIP_PROVIDE_ERRNO
#define LWIP_PROVIDE_ERRNO 1
#endif

/*
   --------------------------------------
   ---------- Checksum options ----------
   --------------------------------------
*/

/*
Some MCU allow computing and verifying the IP, UDP, TCP and ICMP checksums by hardware:
 - To use this feature let the following define uncommented.
 - To disable it and process by CPU comment the  the checksum.
*/
//#define CHECKSUM_BY_HARDWARE


#ifdef CHECKSUM_BY_HARDWARE
  /* CHECKSUM_GEN_IP==0: Generate checksums by hardware for outgoing IP packets.*/
  #define CHECKSUM_GEN_IP                 0
  /* CHECKSUM_GEN_UDP==0: Generate checksums by hardware for outgoing UDP packets.*/
  #define CHECKSUM_GEN_UDP                0
  /* CHECKSUM_GEN_TCP==0: Generate checksums by hardware for outgoing TCP packets.*/
  #define CHECKSUM_GEN_TCP                0
  /* CHECKSUM_CHECK_IP==0: Check checksums by hardware for incoming IP packets.*/
  #define CHECKSUM_CHECK_IP               0
  /* CHECKSUM_CHECK_UDP==0: Check checksums by hardware for incoming UDP packets.*/
  #define CHECKSUM_CHECK_UDP              0
  /* CHECKSUM_CHECK_TCP==0: Check checksums by hardware for incoming TCP packets.*/
  #define CHECKSUM_CHECK_TCP              0
#else
  /* CHECKSUM_GEN_IP==1: Generate checksums in software for outgoing IP packets.*/
  #define CHECKSUM_GEN_IP                 1
  /* CHECKSUM_GEN_UDP==1: Generate checksums in software for outgoing UDP packets.*/
  #define CHECKSUM_GEN_UDP                1
  /* CHECKSUM_GEN_TCP==1: Generate checksums in software for outgoing TCP packets.*/
  #define CHECKSUM_GEN_TCP                1
  /* CHECKSUM_CHECK_IP==1: Check checksums in software for incoming IP packets.*/
  #define CHECKSUM_CHECK_IP               1
  /* CHECKSUM_CHECK_UDP==1: Check checksums in software for incoming UDP packets.*/
  #define CHECKSUM_CHECK_UDP              1
  /* CHECKSUM_CHECK_TCP==1: Check checksums in software for incoming TCP packets.*/
  #define CHECKSUM_CHECK_TCP              1
#endif

/**
 * DEFAULT_THREAD_STACKSIZE: The stack size used by any other lwIP thread.
 * The stack size value itself is platform-dependent, but is passed to
 * sys_thread_new() when the thread is created.
 */
#ifndef DEFAULT_THREAD_STACKSIZE
#define DEFAULT_THREAD_STACKSIZE        4096
#endif

/**
 * DEFAULT_THREAD_PRIO: The priority assigned to any other lwIP thread.
 * The priority value itself is platform-dependent, but is passed to
 * sys_thread_new() when the thread is created.
 */
#ifndef DEFAULT_THREAD_PRIO
#define DEFAULT_THREAD_PRIO             3
#endif

/*
   ------------------------------------
   ---------- Debugging options ----------
   ------------------------------------
*/

#ifdef LWIP_DEBUG
#define U8_F "c"
#define S8_F "c"
#define X8_F "02x"
#define U16_F "u"
#define S16_F "d"
#define X16_F "x"
#define U32_F "u"
#define S32_F "d"
#define X32_F "x"
#define SZT_F "u"
#endif

#define TCPIP_MBOX_SIZE                 32
#define TCPIP_THREAD_PRIO	            8
//#define TCPIP_THREAD_PRIO	            4		// SC : For debugging, needs to be lower priority to allow IDE to trigger debugger
#define TCPIP_THREAD_STACKSIZE	        1024

/**
 * DEFAULT_RAW_RECVMBOX_SIZE: The mailbox size for the incoming packets on a
 * NETCONN_RAW. The queue size value itself is platform-dependent, but is passed
 * to sys_mbox_new() when the recvmbox is created.
 */
#define DEFAULT_RAW_RECVMBOX_SIZE       12

/**
 * DEFAULT_UDP_RECVMBOX_SIZE: The mailbox size for the incoming packets on a
 * NETCONN_UDP. The queue size value itself is platform-dependent, but is passed
 * to sys_mbox_new() when the recvmbox is created.
 */
#define DEFAULT_UDP_RECVMBOX_SIZE       12

/**
 * DEFAULT_TCP_RECVMBOX_SIZE: The mailbox size for the incoming packets on a
 * NETCONN_TCP. The queue size value itself is platform-dependent, but is passed
 * to sys_mbox_new() when the recvmbox is created.
 */
#define DEFAULT_TCP_RECVMBOX_SIZE       12

/**
 * DEFAULT_ACCEPTMBOX_SIZE: The mailbox size for the incoming connections.
 * The queue size value itself is platform-dependent, but is passed to
 * sys_mbox_new() when the acceptmbox is created.
 */
#define DEFAULT_ACCEPTMBOX_SIZE         12

#define LWIP_IGMP                       1
#define LWIP_MDNS_RESPONDER             1
#define LWIP_NUM_NETIF_CLIENT_DATA      2       // Increased by 1 for MDNS
#define LWIP_NETIF_EXT_STATUS_CALLBACK  1
#define MDNS_RESP_USENETIF_EXTCALLBACK  1
#define LWIP_NETIF_LINK_CALLBACK        1
#define LWIP_NETIF_STATUS_CALLBACK      1   // Match Breeze configuration settings

// Tuning
#define ETH_PAD_SIZE                    2
#define LWIP_RAW                        0
//#define MEMP_DEBUG LWIP_DBG_ON
//#define TCP_DEBUG  LWIP_DBG_ON
//#define SOCKETS_DEBUG  LWIP_DBG_ON
//#define INET_DEBUG  LWIP_DBG_ON
//#define PBUF_DEBUG LWIP_DBG_ON
//#define DHCP_DEBUG LWIP_DBG_ON


#if (LWIP_DNS || LWIP_IGMP || LWIP_IPV6) && !defined(LWIP_RAND)
    /* When using IGMP or IPv6, LWIP_RAND() needs to be defined to a random-function returning an u32_t random value*/
    #include "lwip/arch.h"
    u32_t lwip_rand(void);
    #define LWIP_RAND()     lwip_rand()
#endif

#define MDNS_MAX_SERVICES 2 // Advertise HTTP and OSC


#endif /* __LWIPOPTS_H__ */

/*****END OF FILE****/
