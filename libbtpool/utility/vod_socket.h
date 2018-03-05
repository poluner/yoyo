#pragma once

#include "utility/define.h"
#include <sys/types.h>
#include <sys/socket.h>

typedef uint32_t SOCKET;
#define INVALID_SOCKET (0xFFFFFFFF)


typedef struct tagSD_SOCKADDR
{
    uint16_t _sin_family;
    uint16_t _sin_port;
    uint32_t _sin_addr;
} VOD_SOCKADDR, *pSD_SOCKADDR;

typedef struct
{
    uint8_t	 _ip_verlen;		/* ip version and ip header lenth*/
    uint8_t	 _ip_tos;			/* ip type of service */
    uint16_t _ip_len;			/* ip packet lenghth */
    uint16_t _ip_id;			/* ip packet identification */
    uint16_t _ip_fragoff;		/* ip packet fragment and offset */
    uint8_t  _ip_ttl;			/* ip packet time to live */
    uint8_t	 _ip_proto;			/* ip packet protocol type */
    uint16_t _ip_chksum;		/* ip packet header checksum */
    uint32_t _ip_src_addr;		/* ip source ip adress */
    uint32_t _ip_dst_addr;		/* ip destination ip adress */
} VOD_IP_HEADER;

typedef struct
{
    uint8_t	 _type;
    uint8_t	 _code;
    uint16_t _checksum;
    uint16_t _id;
    uint16_t _sequence;
} VOD_ICMP_HEADER;

#ifndef ICMP_ECHO
#define ICMP_ECHO		(8)
#define ICMP_ECHOREPLY	(0)
#endif
#define SD_IPPROTO_ICMP	(1)
#define WOULDBLOCK		(-2)

#define SD_PF_INET		(PF_INET)
#define SD_AF_INET		(SD_PF_INET)
#define SD_SOCK_DGRAM	(SOCK_DGRAM)
#define SD_SOCK_STREAM	(SOCK_STREAM)
#define	SD_SOCK_RAW		(SOCK_RAW)

#define ANY_ADDRESS		((uint32_t)0x00000000)


#define CI_IDX_COUNT			    (25)

/* function index */
#define CI_FS_IDX_OPEN           (0)
#define CI_FS_IDX_ENLARGE_FILE   (1)
#define CI_FS_IDX_CLOSE          (2)
#define CI_FS_IDX_READ           (3)
#define CI_FS_IDX_WRITE          (4)
#define CI_FS_IDX_PREAD          (5)
#define CI_FS_IDX_PWRITE         (6)
#define CI_FS_IDX_FILEPOS        (7)
#define CI_FS_IDX_SETFILEPOS     (8)
#define CI_FS_IDX_FILESIZE       (9)
#define CI_FS_IDX_FREE_DISK      (10)

#define CI_SOCKET_IDX_SET_SOCKOPT  (11)

#define CI_MEM_IDX_GET_MEM           (12)
#define CI_MEM_IDX_FREE_MEM          (13)
#define CI_ZLIB_UNCOMPRESS          (14)
#define CI_FS_IDX_GET_FILESIZE_AND_MODIFYTIME   (15)
#define CI_FS_IDX_DELETE_FILE     (16)
#define CI_FS_IDX_RM_DIR              (17)
#define CI_FS_IDX_MAKE_DIR              (18)
#define CI_FS_IDX_RENAME_FILE              (19)
#define CI_SOCKET_IDX_CREATE  (20)
#define CI_SOCKET_IDX_CLOSE  (21)
#define CI_FS_IDX_FILE_EXIST          (22)
#define CI_DNS_GET_DNS_SERVER          (23)
#define CI_LOG_WRITE_LOG                (24)


/* all socket are non-blocked */
int32_t vod_create_socket(int32_t domain, int32_t type, int32_t protocol, uint32_t *socket);
int32_t vod_socket_bind(uint32_t socket, VOD_SOCKADDR *addr);
int32_t vod_socket_listen(uint32_t socket, int32_t backlog);
int32_t vod_close_socket(uint32_t socket);



