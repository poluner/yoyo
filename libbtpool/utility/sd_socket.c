#include "utility/sd_socket.h"
#include "utility/errcode.h"
#include "utility/define.h"
#include "utility/sd_string.h"
#include "utility/utility.h"


#if defined(LINUX)
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#if defined(_ANDROID_LINUX)
#include <net/if.h>
#include <net/if_arp.h>
#include <dlfcn.h>
#endif

#elif defined(WINCE)
//#include <errno.h>
//#include <fcntl.h>
//#include <sys/types.h>
#include <winsock2.h>
#endif

#include "utility/log_wrapper.h"
LOGID_MODEULE("interface")

#if defined(_ANDROID_LINUX)
static pthread_once_t uid_socket_once = PTHREAD_ONCE_INIT;

typedef int(*qtaguid_tagSocket_t)(int, int, uid_t);
static qtaguid_tagSocket_t qtaguid_tagSocket = NULL;

#if defined(_ANDROID_LINUX_64)
static const char LIB_CUTILS_PATH[] = "/system/lib64/libcutils.so";
#else
static const char LIB_CUTILS_PATH[] = "/system/lib/libcutils.so";
#endif

static void init_tag_fun(void) {
    void* libcutils = dlopen(LIB_CUTILS_PATH, RTLD_LAZY);
    if (libcutils) {
        qtaguid_tagSocket = (qtaguid_tagSocket_t)dlsym(libcutils, "qtaguid_tagSocket");
    }
}

int sd_tag_socket_to_uid(int sockfd, uid_t uid) {
    pthread_once(&uid_socket_once, init_tag_fun);
    if (qtaguid_tagSocket) {
        return qtaguid_tagSocket(sockfd, 0, uid);
    }
    else {
        return -EPROTONOSUPPORT;
    }
}
#endif

//#define CONV_FROM_SD_SOCKADDR(addr, psd_addr)  {(addr).sin_family = (psd_addr)->_sin_family; (addr).sin_port = (psd_addr)->_sin_port; (addr).sin_addr.s_addr = (psd_addr)->_sin_addr;}
//#define CONV_TO_SD_SOCKADDR(psd_addr, addr)  {(psd_addr)->_sin_family = (addr).sin_family; (psd_addr)->_sin_port = (addr).sin_port; (psd_addr)->_sin_addr = (addr).sin_addr.s_addr;}

socklen_t get_sockaddr_len(sockaddr_storage *pAddr)
{
    socklen_t addr_len = sizeof(struct sockaddr_in);  //treat as AF_INET
    if(pAddr->ss_family == AF_INET6)
    {
        addr_len = sizeof(struct sockaddr_in6);
    }
    return addr_len;
}

static void conv_from_sd_sockaddr(struct sockaddr_storage *pAddr, const SD_SOCKADDR *pSdAddr)
{
    sd_assert(SD_AF_INET == pSdAddr->_sin_family || SD_AF_INET6 == pSdAddr->_sin_family);

    if (SD_AF_INET == pSdAddr->_sin_family)
    {
        struct sockaddr_in *pSin = (struct sockaddr_in *)pAddr;
        pSin->sin_family = pSdAddr->_sin_family;
        pSin->sin_port = pSdAddr->_sin_port;
        pSin->sin_addr.s_addr = pSdAddr->_sin_addr;
    }
    else if (SD_AF_INET6 == pSdAddr->_sin_family)
    {
        struct sockaddr_in6 *pSin6 = (struct sockaddr_in6 *)pAddr;
        pSin6->sin6_family = pSdAddr->_sin_family;
        pSin6->sin6_port = pSdAddr->_sin_port;
        sd_memcpy(&(pSin6->sin6_addr), &(pSdAddr->_sin6_addr), sizeof(struct in6_addr));
    }
}

static void conv_to_sd_sockaddr(SD_SOCKADDR *pSdAddr, const struct sockaddr_storage *pAddr)
{
    sd_assert(SD_AF_INET == pAddr->ss_family || SD_AF_INET6 == pAddr->ss_family);

    if (SD_AF_INET == pAddr->ss_family)
    {
        struct sockaddr_in *pSin = (struct sockaddr_in *)pAddr;
        pSdAddr->_sin_family = pSin->sin_family;
        pSdAddr->_sin_port = pSin->sin_port;
        pSdAddr->_sin_addr = pSin->sin_addr.s_addr;
    }
    else if (SD_AF_INET6 == pAddr->ss_family)
    {
        struct sockaddr_in6 *pSin6 = (struct sockaddr_in6 *)pAddr;
        pSdAddr->_sin_family = pSin6->sin6_family;
        pSdAddr->_sin_port = pSin6->sin6_port;
        sd_memcpy(&(pSdAddr->_sin6_addr), &(pSin6->sin6_addr), sizeof(struct in6_addr));
    }
}

#if 0 //defined(LINUX)
static struct ifreq ifr;
static BOOL g_ifr_got = FALSE;
#endif
int32_t sd_create_socket(int32_t domain, int32_t type, int32_t protocol, uint32_t *sock, uid_t uid)
{
    int32_t ret_val = SUCCESS;
    int32_t flags = 0;


#ifdef LINUX

    flags = 0;



    *sock = socket(domain, type, protocol);

    if(*sock == (uint32_t)-1)
        return errno;

    if(*sock == 0)
    {
        /* find a case that socket descripor == 0 ... */

        /* try to get a non-zero descriptor */
        *sock = socket(domain, type, protocol);

        /* close the zero-descriptor */
        sd_close_socket(0);

        if(*sock == (uint32_t)-1)
            return errno;

        if(*sock == 0)
        {
            /* would this case happend? */
            return INVALID_SOCKET_DESCRIPTOR;
        }
    }

#if defined(_ANDROID_LINUX)
    LOG_DEBUG("create socket: uid = %u", uid);
    if (uid != 0)
    {
        int tag_ret = sd_tag_socket_to_uid(*sock, uid);
        LOG_DEBUG("Tag socket to uid ret = %d, libpath =%s", tag_ret, LIB_CUTILS_PATH);
    }
#endif

#ifdef MACOS
    int set = 1;
    setsockopt(*sock,SOL_SOCKET,SO_NOSIGPIPE,(void*)&set,sizeof(int));
#endif

    /* set nonblock */
    flags = fcntl(*sock, F_GETFL);

    ret_val = fcntl(*sock, F_SETFL, flags | O_NONBLOCK);

    if(ret_val < 0)
    {
        LOG_DEBUG("set non-block failed:%d, so close the new socket(id:%d)", errno, *sock);

        sd_close_socket(*sock);
        *sock = INVALID_SOCKET;

        ret_val = errno;
    }
    else
    {
        ret_val = SUCCESS;
        LOG_DEBUG("success to create socket(id:%d)", *sock);
    }
#elif defined(WINCE)

    uint32_t arg = 1;

    flags = 0;


    LOG_DEBUG("sd_create_socket:domain=%d,type=%d,protocol=%d,SD_SOCK_DGRAM=%d,SD_SOCK_STREAM=%d", domain,type,protocol,SD_SOCK_DGRAM,SD_SOCK_STREAM);
    if((type ==SD_SOCK_DGRAM)&&(protocol==0))
        protocol = IPPROTO_UDP;
    else if((type ==SD_SOCK_STREAM)&&(protocol==0))
        protocol = IPPROTO_TCP;

    *sock = socket(domain, type, protocol);

    if(*sock == (uint32_t)-1)
        return GetLastError();

    /* set nonblock */
    ret_val = ioctlsocket(*sock ,FIONBIO,&arg);

    if(ret_val < 0)
        ret_val = GetLastError();
#endif
    return ret_val;
}

int sd_setnonblock( int fd )
{
    int flags;

    flags = fcntl(fd, F_GETFL);
    flags |= O_NONBLOCK;
    return fcntl(fd, F_SETFL, flags);
}


int32_t sd_socket_bind(uint32_t socket, SD_SOCKADDR *paddr)
{
    int32_t ret_val = SUCCESS;

#if defined(LINUX)
    struct sockaddr_storage os_addr;

    int32_t reuse = 1;
    int32_t socket_type = 0;
    socklen_t sock_type_len = sizeof(socket_type);

    sd_memset((void*) &os_addr, 0, sizeof(os_addr));

    conv_from_sd_sockaddr(&os_addr, paddr);

    if(getsockopt(socket, SOL_SOCKET, SO_TYPE, &socket_type, &sock_type_len)==0)
    {
        if(socket_type==SD_SOCK_STREAM)
        {
            setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
        }
    }

    ret_val = bind(socket, (const struct sockaddr*)&os_addr, get_sockaddr_len(&os_addr));
    if(ret_val < 0)
        ret_val = errno;
    LOG_DEBUG("sd_socket_bind ret_val(%d)", ret_val);

#elif defined(WINCE)
    struct sockaddr_storage os_addr;
    int32_t reuse = 1;

    /* If the port is specified as 0, the service provider assigns a unique port to the application with a  appropriate value */
    paddr->_sin_port = 0;

    conv_from_sd_sockaddr(&os_addr, paddr);


    if(setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1)
    {
        //  setsockopt error
    }

    ret_val = bind(socket, (const struct sockaddr*)&os_addr, sizeof(os_addr));
    if(ret_val < 0)
        ret_val = GetLastError();

    if(ret_val == 0)
    {
        struct sockaddr_storage addr = {0};
        int length = sizeof(addr);
        uint16_t port = 0;
        getsockname(socket, (struct sockaddr*)&addr, &length);
        //port = ntohs(addr.sin_port);
        if (AF_INET == addr.ss_family)
        {
            struct sockaddr_in *pSin = (struct sockaddr_in *)&addr;
            port = pSin->sin_port;
        }
        else
        {
            struct sockaddr_in6 *pSin6 = (struct sockaddr_in6 *)&addr;
            port = pSin6->sin6_port;
        }
        if(port != os_addr.sin_port)
        {
            paddr->_sin_port = port;
        }
    }
#endif
    return ret_val;
}

int32_t sd_socket_listen(uint32_t socket, int32_t backlog)
{
    int32_t ret_val = SUCCESS;
#if defined(LINUX)

    ret_val = listen(socket, backlog);
    if(ret_val < 0)
        ret_val = errno;

#elif defined(WINCE)

    ret_val = listen(socket, backlog);
    if(ret_val < 0)
        ret_val = GetLastError();

#endif
    return ret_val;
}

int32_t sd_close_socket(uint32_t socket)
{
    int32_t ret_val = SUCCESS;
    int32_t ret_err = SUCCESS;


#if defined(LINUX)

#ifdef _ANDROID_LINUX
    shutdown(socket, SHUT_RDWR);
#endif

    do
    {
        ret_val = close(socket);
        ret_err = errno;
        if( ret_val < 0 )
        {
            LOG_ERROR("ERR to close socket(id:%d), err:%d", socket, ret_err);
        }
    }
    while(ret_val < 0 && errno == EINTR);

    //close(socket);
    LOG_DEBUG("success to close socket(id:%d)", socket);
#elif defined(WINCE)
    closesocket(socket);
#endif

    return ret_val;
}

int32_t sd_accept(uint32_t socket, uint32_t *accept_sock, SD_SOCKADDR *addr)
{
    int32_t ret_val = SUCCESS;
#if defined(LINUX)
    struct sockaddr_storage os_addr;
    int32_t flags = 0;

    sd_memset((void*) &os_addr, 0, sizeof(os_addr));
    os_addr.ss_family = addr->_sin_family;
    socklen_t addr_len = get_sockaddr_len(&os_addr);

    while(1)
    {
        ret_val = accept(socket, (struct sockaddr*)&os_addr, &addr_len);
        if(ret_val < 0 && errno == EINTR)
            continue;

        break;
    }

    if(ret_val >= 0)
    {
        *accept_sock = ret_val;
        conv_to_sd_sockaddr(addr, &os_addr);

        /* set socket nonblock */
        flags = fcntl(*accept_sock, F_GETFL);
        ret_val = fcntl(*accept_sock, F_SETFL, flags | O_NONBLOCK);

        if(ret_val < 0)
        {
            LOG_DEBUG("set non-block failed:%d, so close the accepted socket(id:%d)", errno, *accept_sock);

            /* close this socket? */
            sd_close_socket(*accept_sock);
            *accept_sock = INVALID_SOCKET;

            ret_val = errno;
        }
        else
        {
            ret_val = SUCCESS;


            if(ret_val != SUCCESS)
            {
                LOG_DEBUG("set sockopt failed:%d, so close the accpeted socket(id:%d)", ret_val, *accept_sock);

                /* close this socket? */
                sd_close_socket(*accept_sock);
                *accept_sock = INVALID_SOCKET;
            }

            LOG_DEBUG("success to accept new socket(id:%d)", *accept_sock);
        }
    }
    else
    {
        /* failed */
        ret_val = errno;
        if(ret_val == EAGAIN)
            ret_val = WOULDBLOCK;
    }
#elif defined(WINCE)
    struct sockaddr_storage os_addr;
    uint32_t addr_len = sizeof(os_addr);
    int32_t flags = 0;
    uint32_t   arg = 1;


    while(1)
    {
        ret_val = accept(socket, (struct sockaddr*)&os_addr, &addr_len);
        if(ret_val < 0 && WSAGetLastError() == WSAEINTR)
            continue;

        break;
    }

    if(ret_val >= 0)
    {
        *accept_sock = ret_val;
        conv_to_sd_sockaddr(addr, &os_addr);

        /* set socket nonblock */
        /* set socket nonblock */
        ret_val = ioctlsocket(*accept_sock ,FIONBIO,&arg);

        if(ret_val < 0)
        {
            /* close this socket? */
            sd_close_socket(*accept_sock);

            ret_val = WSAGetLastError() ;
        }
        else
            ret_val = SUCCESS;
    }
    else
    {
        /* failed */
        ret_val = WSAGetLastError() ;
        if(ret_val == WSAEWOULDBLOCK)
            ret_val = WOULDBLOCK;
    }
#endif

    return ret_val;
}

int32_t sd_connect(uint32_t socket, const SD_SOCKADDR *paddr)
{
    int32_t ret_val = SUCCESS;

#if defined(LINUX)
    struct sockaddr_storage os_addr;

#ifdef _DEBUG
    if (SD_AF_INET == paddr->_sin_family)
    {
        char ip_addr[MAX_SERVER_IP_LEN] = {0};
        sd_inet_ntoa(paddr->_sin_addr, ip_addr, sizeof(ip_addr));
        LOG_DEBUG("ready to connect (0x%X=)%s : (%u=)%d about (socket:%d)", paddr->_sin_addr, ip_addr, paddr->_sin_port, sd_ntohs(paddr->_sin_port), socket);
    }
    else if (SD_AF_INET6 == paddr->_sin_family)
    {
        char ip6Str[MAX_IN6ADDR_STR_LEN];
        sd_inet6_ntop(&paddr->_sin6_addr, ip6Str, MAX_IN6ADDR_STR_LEN);
        LOG_DEBUG("ready to connect %s : (%u=)%d about (socket:%d)",  ip6Str, paddr->_sin_port, sd_ntohs(paddr->_sin_port), socket);
    }
    else
    {
        LOG_DEBUG("sd_connect Invalid address family");
        return ERR_INVALID_ADDRESS_FAMILY;
    }
#endif

    sd_memset((void*) &os_addr, 0, sizeof(os_addr));

    conv_from_sd_sockaddr(&os_addr, paddr);
    socklen_t addr_len = get_sockaddr_len(&os_addr);
    //mac下sizeof（sockaddr_storage）是一个很大的结构体，导致socklen提供错误

    do
    {
        ret_val = connect(socket, (struct sockaddr*)&os_addr, addr_len);
    }
    while(ret_val < 0 && errno == EINTR);


    if(ret_val >= 0 || errno == EISCONN)
    {
        ret_val = SUCCESS;
    }
    else
    {
        /* failed */
        ret_val = errno;
        if(ret_val == EINPROGRESS)
            ret_val = WOULDBLOCK;
#ifdef _DEBUG
        else
            LOG_ERROR("sd_connect ret:%d,fd:%d",ret_val, socket);
#endif

    }

#elif defined(WINCE)
    struct sockaddr_storage os_addr;
    int32_t addr_len = sizeof(os_addr);
    int32_t last_err = 0;

#ifdef _DEBUG
    if (SD_AF_INET == paddr->_sin_family)
    {
        char ip_addr[MAX_SERVER_IP_LEN] = { 0 };
        sd_inet_ntoa(paddr->_sin_addr, ip_addr, sizeof(ip_addr));;
        LOG_DEBUG("ready to connect %s : %d about (socket:%d)", ip_addr, sd_ntohs(paddr->_sin_port), socket);
    }
    else if (SD_AF_INET6 == paddr->_sin_family)
    {
        char ip6Str[MAX_IN6ADDR_STR_LEN];
        sd_inet6_ntop(&paddr->_sin6_addr, ip6Str, MAX_IN6ADDR_STR_LEN);
        LOG_DEBUG("ready to connect %s : %d about (socket:%d)", ip6Str, sd_ntohs(paddr->_sin_port), socket);
    }
    else
    {
        LOG_DEBUG("sd_connect Invalid address family");
        return ERR_INVALID_ADDRESS_FAMILY;
    }
#endif


    conv_from_sd_sockaddr(&os_addr, paddr);

    do
    {
        ret_val = connect(socket, (struct sockaddr*)&os_addr, addr_len);
        if(ret_val<0)
        {
            last_err = WSAGetLastError();
        }
    }
    while(ret_val < 0 && last_err== WSAEINTR);

    //LOG_DEBUG("sd_connect 1(fd:%u),ret_val=%d,last_err=%d ",socket,ret_val,last_err);
    if(ret_val >= 0)
    {
        ret_val = SUCCESS;
    }
    else
    {
        /* failed */
        ret_val = last_err;//WSAGetLastError();
        if(ret_val == WSAEWOULDBLOCK)
            ret_val = WOULDBLOCK;
    }

    //LOG_DEBUG("sd_connect 2(fd:%u),ret_val=%d ",socket,ret_val);

#endif

    return ret_val;
}

int32_t sd_asyn_proxy_connect(uint32_t socket, const char* host, uint16_t port,  void* user_own_data, void* user_msg_data)
{
    int32_t ret_val = SUCCESS;
    SD_SOCKADDR addr;
    SD_SOCKADDR *paddr=&addr;
    struct sockaddr_storage os_addr;
    //int32_t addr_len = sizeof(os_addr);
    uint32_t proxy_ip = 0;
    uint16_t proxy_port = 0;
#ifdef _DEBUG
    char ip_addr[MAX_SERVER_IP_LEN] = {0};
#endif
    sd_assert(0);
    sd_memset((void*) &addr, 0, sizeof(SD_SOCKADDR));
    addr._sin_addr = proxy_ip;//0xAC00000A;//10.0.0.172
    addr._sin_port = sd_htons(proxy_port);
    addr._sin_family = SD_AF_INET;
#ifdef _DEBUG
    sd_inet_ntoa(paddr->_sin_addr,  ip_addr, sizeof(ip_addr));
    LOG_ERROR("ready to proxy connect (0x%X=)%s : (%u=)%d about (socket:%d)\n",paddr->_sin_addr, ip_addr, paddr->_sin_port,sd_ntohs(paddr->_sin_port), socket);
#endif

    //TODO(HYY)：该函数目前没被调用，暂时不增加IPv6的支持

    sd_memset((void*) &os_addr, 0, sizeof(os_addr));

    conv_from_sd_sockaddr(&os_addr, paddr);
    socklen_t addr_len = get_sockaddr_len(&os_addr);

#if defined( WINCE)
    do
    {
        ret_val = connect(socket, (struct sockaddr*)&os_addr, addr_len);
    }
    while(ret_val < 0 && WSAGetLastError() == WSAEINTR);

    if(ret_val >= 0)
    {
        ret_val = SUCCESS;
    }
    else
    {
        /* failed */
        ret_val = WSAGetLastError();
        if(ret_val == WSAEWOULDBLOCK)
            ret_val = WOULDBLOCK;
    }
#else
    do
    {
        ret_val = connect(socket, (struct sockaddr*)&os_addr, addr_len);
    }
    while(ret_val < 0 && errno == EINTR);

    if(ret_val >= 0)
    {
        ret_val = SUCCESS;
    }
    else
    {
        /* failed */
        ret_val = errno;
        if(ret_val == EINPROGRESS)
            ret_val = WOULDBLOCK;
    }
#endif

    return ret_val;
}

int32_t sd_recv(uint32_t socket, char *buffer, int32_t bufsize, int32_t *recved_len)
{
    int32_t ret_val = SUCCESS;

#if defined(LINUX)

    (*recved_len) = 0;

    do
    {
        ret_val = recv(socket, buffer, bufsize, 0);
    }
    while(ret_val < 0 && errno == EINTR);

    if(ret_val >= 0)
    {
        *recved_len = ret_val;
        ret_val = SUCCESS;
    }
    else
    {
        /* failed */
        ret_val = errno;
        if(ret_val == EAGAIN)
            ret_val = WOULDBLOCK;
    }
#elif defined(WINCE)
    (*recved_len) = 0;

    do
    {
        ret_val = recv(socket, buffer, bufsize, 0);
    }
    while(ret_val < 0 && WSAGetLastError() == WSAEINTR);

    if(ret_val >= 0)
    {
        *recved_len = ret_val;
        ret_val = SUCCESS;
    }
    else
    {
        /* failed */
        ret_val = WSAGetLastError();
        if(ret_val == WSAEWOULDBLOCK)
            ret_val = WOULDBLOCK;
    }

#endif

    return ret_val;
}

int32_t sd_recvfrom(uint32_t socket, char *buffer, int32_t bufsize, SD_SOCKADDR *addr, int32_t *recved_len)
{
    int32_t ret_val = SUCCESS;

#if defined(LINUX)

    struct sockaddr_storage os_addr;

    sd_memset((void*) &os_addr, 0, sizeof(os_addr));
    os_addr.ss_family = addr->_sin_family;
    socklen_t addr_len = get_sockaddr_len(&os_addr);

    *recved_len = 0;

    do
    {
        ret_val = recvfrom(socket, buffer, bufsize, 0, (struct sockaddr*)&os_addr, &addr_len);
    }
    while(ret_val < 0 && errno == EINTR);

    if(ret_val >= 0)
    {
        *recved_len = ret_val;
        conv_to_sd_sockaddr(addr, &os_addr);
        ret_val = SUCCESS;
    }
    else
    {
        /* failed */
        ret_val = errno;
        if(ret_val == EAGAIN)
            ret_val = WOULDBLOCK;
    }

#elif defined(WINCE)
    struct sockaddr_storage os_addr;
    uint32_t addr_len = sizeof(os_addr);

    *recved_len = 0;

    do
    {
        ret_val = recvfrom(socket, buffer, bufsize, 0, (struct sockaddr*)&os_addr, &addr_len);
    }
    while(ret_val < 0 && WSAGetLastError() == WSAEINTR);

    if(ret_val >= 0)
    {
        *recved_len = ret_val;
        conv_to_sd_sockaddr(addr, &os_addr);
        ret_val = SUCCESS;
    }
    else
    {
        /* failed */
        ret_val =  WSAGetLastError();
        if(ret_val == WSAEWOULDBLOCK)
            ret_val = WOULDBLOCK;
    }
#endif

    return ret_val;
}

int32_t sd_send(uint32_t socket, char *buffer, int32_t sendsize, int32_t *sent_len)
{
    int32_t ret_val = SUCCESS;

#if defined(LINUX)

    *sent_len = 0;

    do
    {
        ret_val = send(socket, buffer, sendsize, 0);
    }
    while(ret_val < 0 && errno == EINTR);

    if(ret_val >= 0)
    {
        *sent_len = ret_val;
        ret_val = SUCCESS;
    }
    else
    {
        /* failed */
        ret_val = errno;
        if(ret_val == EAGAIN)
            ret_val = WOULDBLOCK;
    }

#elif defined(WINCE)
    *sent_len = 0;

    do
    {
        ret_val = send(socket, buffer, sendsize, 0);
    }
    while(ret_val < 0 && WSAGetLastError() == WSAEINTR);

    if(ret_val >= 0)
    {
        *sent_len = ret_val;
        ret_val = SUCCESS;
    }
    else
    {
        /* failed */
        ret_val = WSAGetLastError() ;
        if(ret_val == WSAEWOULDBLOCK)
            ret_val = WOULDBLOCK;
    }

#endif

    return ret_val;
}

int32_t sd_sendto(uint32_t socket, char *buffer, int32_t sendsize, const SD_SOCKADDR *addr, int32_t *sent_len)
{
    int32_t ret_val = SUCCESS;

#if defined(LINUX)

    struct sockaddr_storage os_addr;

    sd_memset((void*) &os_addr, 0, sizeof(os_addr));

    *sent_len = 0;
    conv_from_sd_sockaddr(&os_addr, addr);
    socklen_t addr_len = get_sockaddr_len(&os_addr);

    do
    {
        ret_val = sendto(socket, buffer, sendsize, 0, (const struct sockaddr*)&os_addr, addr_len);
    }
    while(ret_val < 0 && errno == EINTR);

    if(ret_val >= 0)
    {
        *sent_len = ret_val;
        ret_val = SUCCESS;
    }
    else
    {
        /* failed */
        ret_val = errno;
        if(ret_val == EAGAIN)
            ret_val = WOULDBLOCK;
    }

#elif defined(WINCE)

    struct sockaddr_storage os_addr;
    int32_t addr_len = sizeof(os_addr);

    *sent_len = 0;
    conv_from_sd_sockaddr(&os_addr, addr);

    do
    {
        ret_val = sendto(socket, buffer, sendsize, 0, (const struct sockaddr*)&os_addr, addr_len);
    }
    while(ret_val < 0 && WSAGetLastError() == WSAEINTR);

    if(ret_val >= 0)
    {
        *sent_len = ret_val;
        ret_val = SUCCESS;
    }
    else
    {
        /* failed */
        ret_val = WSAGetLastError();
        if(ret_val == WSAEWOULDBLOCK)
            ret_val = WOULDBLOCK;
    }
#endif

    return ret_val;
}

int32_t sd_getpeername(uint32_t socket, SD_SOCKADDR *addr)
{
    int32_t ret_val = SUCCESS;

#if defined(LINUX)
    struct sockaddr_storage os_addr;

    sd_memset((void*) &os_addr, 0, sizeof(os_addr));
    os_addr.ss_family = addr->_sin_family;
    socklen_t addr_len = get_sockaddr_len(&os_addr);

    ret_val = getpeername(socket, (struct sockaddr*)&os_addr, &addr_len);

    if(ret_val >= 0)
    {
        conv_to_sd_sockaddr(addr, &os_addr);
        ret_val = SUCCESS;
    }
    else
    {
        ret_val = errno;
    }
#elif defined(WINCE)
    struct sockaddr_storage os_addr;
    uint32_t addr_len = sizeof(os_addr);

    ret_val = getpeername(socket, (struct sockaddr*)&os_addr, &addr_len);

    if(ret_val >= 0)
    {
        conv_to_sd_sockaddr(addr, &os_addr);
        ret_val = SUCCESS;
    }
    else
    {
        ret_val = GetLastError();
    }

#endif

    return ret_val;
}

int32_t sd_getsockname(uint32_t socket, SD_SOCKADDR *addr)
{
    int32_t ret_val = SUCCESS;

#if defined(LINUX)
    struct sockaddr_storage os_addr;

    sd_memset((void*) &os_addr, 0, sizeof(os_addr));
    os_addr.ss_family = addr->_sin_family;
    socklen_t addr_len = get_sockaddr_len(&os_addr);

    ret_val = getsockname(socket, (struct sockaddr*)&os_addr, &addr_len);

    if(ret_val >= 0)
    {
        conv_to_sd_sockaddr(addr, &os_addr);
        ret_val = SUCCESS;
    }
    else
    {
        ret_val = errno;
    }

#elif defined(WINCE)
    struct sockaddr_storage os_addr;
    uint32_t addr_len = sizeof(os_addr);

    ret_val = getsockname(socket, (struct sockaddr*)&os_addr, &addr_len);

    if(ret_val >= 0)
    {
        conv_to_sd_sockaddr(addr, &os_addr);
        ret_val = SUCCESS;
    }
    else
    {
        ret_val = GetLastError();
    }

#endif

    return ret_val;
}

int32_t sd_set_snd_timeout(uint32_t socket, uint32_t timeout)
{
    int32_t ret_val = SUCCESS;

#if defined(LINUX)
    struct timeval time;
    time.tv_sec = timeout / 1000;
    time.tv_usec = timeout % 1000 * 1000;

    ret_val = setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, &time, sizeof(time));
    if(ret_val < 0)
        ret_val = errno;

#elif defined(WINCE)
    struct timeval time;
    time.tv_sec = timeout / 1000;
    time.tv_usec = timeout % 1000 * 1000;

    ret_val = setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, &time, sizeof(time));
    if(ret_val < 0)
        ret_val = GetLastError();
#endif

    return ret_val;
}

int32_t sd_set_rcv_timeout(uint32_t socket, uint32_t timeout)
{
    int32_t ret_val = SUCCESS;

#if defined(LINUX)
    struct timeval time;
    time.tv_sec = timeout / 1000;
    time.tv_usec = timeout % 1000 * 1000;

    ret_val = setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &time, sizeof(time));
    if(ret_val < 0)
        ret_val = errno;
#elif defined(WINCE)
    struct timeval time;
    time.tv_sec = timeout / 1000;
    time.tv_usec = timeout % 1000 * 1000;

    ret_val = setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &time, sizeof(time));
    if(ret_val < 0)
        ret_val = GetLastError();

#endif

    return ret_val;
}


int32_t get_socket_error(uint32_t sock)
{
    int32_t ret_val = SUCCESS;

#if defined(LINUX)
    int32_t errcode = 0;
    uint32_t errcode_size = sizeof(int32_t);
    ret_val = getsockopt(sock, SOL_SOCKET, SO_ERROR, &errcode, (socklen_t*)&errcode_size);
    if(ret_val < 0)
        ret_val = errno;
    else
        ret_val = errcode;
#elif defined(WINCE)
    int32_t errcode = 0;
    uint32_t errcode_size = sizeof(int32_t);
    ret_val = getsockopt(sock, SOL_SOCKET, SO_ERROR, &errcode, &errcode_size);
    if(ret_val < 0)
        ret_val = GetLastError();
    else
        ret_val = errcode;
#endif

    return ret_val;
}

//NOTE(HYY): 该函数不支持IPv6，但目前未被调用
int32_t sd_set_socket_ttl(uint32_t sock, int32_t ttl)
{
    int32_t ret_val = SUCCESS;
#if defined(LINUX)
    ret_val = setsockopt(sock, IPPROTO_IP, IP_TTL, (void*)&ttl, sizeof(int32_t));
    if(ret_val < 0)
        ret_val = errno;
    else
        ret_val = SUCCESS;
#else
    sd_assert(FALSE);
#endif
    return ret_val;
}

void* sd_gethostbyname(const char * name)
{
#if defined(WINCE)
    return (void*)gethostbyname(name);
#else
    return NULL;
#endif
}



bool sd_IsPeerActiveDisconnect(int32_t sock)
{
    char    buffer[4096] = { 0 };
    int32_t idx = 0;
    int32_t nRecv = -1;
    int32_t nErrno = -1;
    for (; idx < 100; idx++)
    {
        nRecv = recv(sock, &buffer, sizeof(buffer), 0);
        nErrno = errno;

        if (nRecv > 0)
        {
            continue;   /// session socket cache may also have data, so continue
        }
        else if (0 == nRecv)
        {
            break; /// client active disconnect, return true
        }
        else if (-1 == nRecv)
        {
            if (EAGAIN == nErrno)
                break;  /// Try again, session socket recv cache didn't have date, return false

            if (EBADF == nErrno)
                break;  /// Bad file number, session socket is broken, return false

            if (ECONNRESET == nErrno)
                continue; /// Connection reset by peer, session socket cache should also have data, so continue

            continue; /// other also need continue
        }
        else
        {
            break; /// shouldn't happen event
        }
    }

    LOG_DEBUG("Session::IsClientSocketActiveDisconnect sock=[%d] nRecv=[%d] nErrno=[%d] errinfo=[%s] nIdx=[%d]",
        sock, nRecv, nErrno, strerror(nErrno), idx);
    return  (0 == nRecv);
}
