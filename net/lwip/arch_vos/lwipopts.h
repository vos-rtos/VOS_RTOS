#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

#include "lwip/debug.h"


extern void *vmalloc(unsigned int size);
extern void vfree(void *ptr);
extern void *vrealloc(void *ptr, unsigned int size);
extern void *vcalloc(unsigned int nitems, unsigned int size);

#define SOCKETS_DEBUG		LWIP_DBG_ON|LWIP_DBG_TYPES_ON
#define IP_DEBUG 			LWIP_DBG_ON|LWIP_DBG_TYPES_ON
#define TCP_DEBUG			LWIP_DBG_ON|LWIP_DBG_TYPES_ON
#define TCP_INPUT_DEBUG 	LWIP_DBG_ON|LWIP_DBG_TYPES_ON
#define TCP_OUTPUT_DEBUG	LWIP_DBG_ON|LWIP_DBG_TYPES_ON

#define LWIP_TCPIP_CORE_LOCKING 1//0

#define LWIP_RAW 1

#define LWIP_DNS 1

#define PING_USE_SOCKETS 0

#define LWIP_RAND() HwRandomBuild()

#define LWIP_TIMEVAL_PRIVATE	0

#define mem_clib_free vfree

#define mem_clib_malloc vmalloc

#define mem_clib_calloc vcalloc

#define MEM_LIBC_MALLOC 1

#define sys_msleep(ms) VOSTaskDelay(ms)

#define LWIP_NETIF_HOSTNAME 	1

#define LWIP_AUTOIP 			1

#define ERRNO	1





#define LWIP_PROVIDE_ERRNO 		1

#define LWIP_TCPIP_CORE_LOCKING  0//1

#define LWIP_COMPAT_MUTEX_ALLOWED 1



#ifndef TCPIP_THREAD_PRIO
#define TCPIP_THREAD_PRIO		11	//定义内核任务的优先级为5
#endif
#undef  DEFAULT_THREAD_PRIO
#define DEFAULT_THREAD_PRIO		2


#define SYS_LIGHTWEIGHT_PROT    1

//NO_SYS==1:不使用操作系统
#define NO_SYS                  0  // 使用VOS操作系统

//使用4字节对齐模式
#define MEM_ALIGNMENT           4  

//MEM_SIZE:heap内存的大小,如果在应用中有大量数据发送的话这个值最好设置大一点 
#define MEM_SIZE                500*1024//25600 //内存堆大小

//MEMP_NUM_PBUF:memp结构的pbuf数量,如果应用从ROM或者静态存储区发送大量数据时,这个值应该设置大一点
#define MEMP_NUM_PBUF           256

//MEMP_NUM_UDP_PCB:UDP协议控制块(PCB)数量.每个活动的UDP"连接"需要一个PCB.
#define MEMP_NUM_UDP_PCB        6

//MEMP_NUM_TCP_PCB:同时建立激活的TCP数量
#define MEMP_NUM_TCP_PCB        10

//MEMP_NUM_TCP_PCB_LISTEN:能够监听的TCP连接数量
#define MEMP_NUM_TCP_PCB_LISTEN 6

//MEMP_NUM_SYS_TIMEOUT:能够同时激活的timeout个数
#define MEMP_NUM_SYS_TIMEOUT    9



#define LWIP_TCP                1  //为1是使用TCP
#define TCP_TTL                 255//生存时间

#define MAX_QUEUE_ENTRIES 		100//20//50

#define PPP_SUPPORT				1
#define PAP_SUPPORT				1
#define CHAP_SUPPORT			1
#define PPP_IPV4_SUPPORT		1

/**
 * TCPIP_MBOX_SIZE: The mailbox size for the tcpip thread messages
 * The queue size value itself is platform-dependent, but is passed to
 * sys_mbox_new() when tcpip_init is called.
 */
#undef TCPIP_MBOX_SIZE
#define TCPIP_MBOX_SIZE         MAX_QUEUE_ENTRIES

/**
 * DEFAULT_TCP_RECVMBOX_SIZE: The mailbox size for the incoming packets on a
 * NETCONN_TCP. The queue size value itself is platform-dependent, but is passed
 * to sys_mbox_new() when the recvmbox is created.
 */
#undef DEFAULT_TCP_RECVMBOX_SIZE
#define DEFAULT_TCP_RECVMBOX_SIZE       MAX_QUEUE_ENTRIES


/**
 * DEFAULT_ACCEPTMBOX_SIZE: The mailbox size for the incoming connections.
 * The queue size value itself is platform-dependent, but is passed to
 * sys_mbox_new() when the acceptmbox is created.
 */
#undef DEFAULT_ACCEPTMBOX_SIZE
#define DEFAULT_ACCEPTMBOX_SIZE         MAX_QUEUE_ENTRIES

/*当TCP的数据段超出队列时的控制位,当设备的内存过小的时候此项应为0*/
#define TCP_QUEUE_OOSEQ         1//0

//最大TCP分段
#define TCP_MSS                 (1500 - 40)	  //TCP_MSS = (MTU - IP报头大小 - TCP报头大小


/* 说明：
 * MEMP_MEM_MALLOC==1: vmalloc作为内存池源，发送速度最大200KB/s, MCU 97%
 * MEMP_MEM_MALLOC==0: 编译器初始化时分配好，发送速度最大 270KB/s MCU 97%
 */
#define MEMP_MEM_MALLOC 1

/*****发送缓冲区设定******MAX[270KB/s]***********************/
//MEMP_NUM_TCP_SEG:最多同时在队列中的TCP段数量
#define MEMP_NUM_TCP_SEG        224
//TCP发送缓冲区大小(bytes).
#define TCP_SND_BUF             (28*TCP_MSS)
//TCP_SND_QUEUELEN: TCP发送缓冲区大小(pbuf).这个值最小为(2 * TCP_SND_BUF/TCP_MSS) */
#define TCP_SND_QUEUELEN        (8* TCP_SND_BUF/TCP_MSS)
/*************************************************************/

/*****接收窗口设定**********MAX[130KB/s]**********************/
//PBUF_POOL_SIZE:pbuf内存池个数.
#define PBUF_POOL_SIZE          50//100//200//120//100
//PBUF_POOL_BUFSIZE:每个pbuf内存池大小.
#define PBUF_POOL_BUFSIZE       512//512//512
//TCP接收窗口
#define TCP_WND                 (20*TCP_MSS)//(43*TCP_MSS)//(2*TCP_MSS)
/*************************************************************/

/* ---------- ICMP选项---------- */
#define LWIP_ICMP                 1 //使用ICMP协议


/* ---------- DHCP选项---------- */
//当使用DHCP时此位应该为1,LwIP 0.5.1版本中没有DHCP服务.
#define LWIP_DHCP               1


/* ---------- UDP选项 ---------- */
#define LWIP_UDP                1 //使用UDP服务
#define UDP_TTL                 255 //UDP数据包生存时间


/* ---------- Statistics options ---------- */
#define LWIP_STATS 				1
#define LWIP_PROVIDE_ERRNO 		1

//STM32F4x7允许通过硬件识别和计算IP,UDP和ICMP的帧校验和

//#define CHECKSUM_BY_HARDWARE //定义CHECKSUM_BY_HARDWARE,使用硬件帧校验

#ifdef CHECKSUM_BY_HARDWARE
  //CHECKSUM_GEN_IP==0: 硬件生成IP数据包的帧校验和
  #define CHECKSUM_GEN_IP                 0
  //CHECKSUM_GEN_UDP==0: 硬件生成UDP数据包的帧校验和
  #define CHECKSUM_GEN_UDP                0
  //CHECKSUM_GEN_TCP==0: 硬件生成TCP数据包的帧校验和
  #define CHECKSUM_GEN_TCP                0 
  //CHECKSUM_CHECK_IP==0: 硬件检查输入的IP数据包帧校验和
  #define CHECKSUM_CHECK_IP               0
  //CHECKSUM_CHECK_UDP==0: 硬件检查输入的UDP数据包帧校验和
  #define CHECKSUM_CHECK_UDP              0
  //CHECKSUM_CHECK_TCP==0: 硬件检查输入的TCP数据包帧校验和
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

  #define CHECKSUM_GEN_ICMP	0 //ICMP出去的checksum关闭，硬件产生。
#endif


//LWIP_NETCONN==1:使能NETCON函数(要求使用api_lib.c)
#define LWIP_NETCONN                    1

//LWIP_SOCKET==1:使能Sicket API(要求使用sockets.c)
#define LWIP_SOCKET                     1

#define LWIP_COMPAT_MUTEX               1

#define LWIP_SO_RCVTIMEO                1 //禁止阻塞线程

#define TCPIP_THREAD_STACKSIZE          (4*1024)//1500	//内核任务堆栈大小
#define DEFAULT_UDP_RECVMBOX_SIZE       1000
#define DEFAULT_THREAD_STACKSIZE        512

#define LWIP_TCP_SACK_OUT 1 // 允许批量确认

#if 1
// 配置DHCP
#define LWIP_DHCP 1
#define LWIP_NETIF_HOSTNAME 1

// 配置DNS
#define LWIP_DNS 1

// 配置DHCPD
#define MEMP_NUM_SYS_TIMEOUT (LWIP_NUM_SYS_TIMEOUT_INTERNAL + 1) // DHCPD需要用到1个timeout定时器
#if !LWIP_DHCP
// 在不启用DHCP的情况下也能使用DHCPD, 但是需要为IP地址0.0.0.0开放DHCP UDP端口号
#define LWIP_IP_ACCEPT_UDP_PORT(port) ((port) == LWIP_IANA_PORT_DHCP_CLIENT)
#endif

// 广播包过滤器
// 如果打开了这个过滤器, 那么就需要在套接字上设置SOF_BROADCAST选项才能收发广播数据包
//#define IP_SOF_BROADCAST 1
//#define IP_SOF_BROADCAST_RECV 1

// 配置IPv6
#define LWIP_IPV6 0
#define LWIP_ND6_RDNSS_MAX_DNS_SERVERS LWIP_DNS // 允许SLAAC获取DNS服务器的地址

// 允许在网络接口间转发数据包
// 88W8801的STA和uAP功能可以同时打开, 如果想要手机或电脑连上88W8801创建的热点后能上网, 就需要开启这两个选项
// 电脑/手机 <---> 88W8801 <---> 能上网的路由器 <---> Internet
// 选项开启后, 还需要在路由器上配置路由表才行, 因为此时88W8801也相当于一个路由器
#define IP_FORWARD 1
#define LWIP_IPV6_FORWARD 1

// 开启调试信息输出
#define LWIP_DEBUG
#define DHCPD_DEBUG LWIP_DBG_ON
#define ND6D_DEBUG LWIP_DBG_ON


#define LWIP_DEBUG                     0

#define ICMP_DEBUG                      LWIP_DBG_OFF
#endif
#endif
