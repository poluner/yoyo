#ifndef SD_SOCKET_H_00138F8F2E70_200806111927
#define SD_SOCKET_H_00138F8F2E70_200806111927
#ifdef __cplusplus
extern "C"
{
#endif
#include "utility/define.h"

#if defined(LINUX)
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#elif defined(WINCE)
#define AF_INET 2
#define PF_INET AF_INET
#define AF_INET6 10
#define PF_INET6 AF_INET6
#define SOCK_STREAM     1
#define SOCK_DGRAM		2
#endif

typedef uint32_t SOCKET;
#define INVALID_SOCKET (0xFFFFFFFF)

typedef struct tagSD_SOCKADDR
{
    uint16_t _sin_family;
    uint16_t _sin_port;
    union
    {
        uint32_t _sin_addr;
        struct in6_addr _sin6_addr;
    };
} SD_SOCKADDR, *pSD_SOCKADDR;

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
} SD_IP_HEADER;

typedef struct
{
    uint8_t	 _type;
    uint8_t	 _code;
    uint16_t _checksum;
    uint16_t _id;
    uint16_t _sequence;
} SD_ICMP_HEADER;

#ifndef ICMP_ECHO
#define ICMP_ECHO		(8)
#define ICMP_ECHOREPLY	(0)
#endif
#define SD_IPPROTO_ICMP	(1)
#define WOULDBLOCK		(-2)

#define SD_PF_INET		(PF_INET)
#define SD_AF_INET		(SD_PF_INET)
#define SD_PF_INET6     (PF_INET6)
#define SD_AF_INET6     (SD_PF_INET6)
#define SD_SOCK_DGRAM	(SOCK_DGRAM)
#define SD_SOCK_STREAM	(SOCK_STREAM)
#define	SD_SOCK_RAW		(SOCK_RAW)

#define ANY_ADDRESS		((uint32_t)0x00000000)

/* all socket are non-blocked */
int32_t sd_create_socket(int32_t domain, int32_t type, int32_t protocol, uint32_t *socket, uid_t uid = 0);
int32_t sd_socket_bind(uint32_t socket, SD_SOCKADDR *addr);
int32_t sd_socket_listen(uint32_t socket, int32_t backlog);
int32_t sd_close_socket(uint32_t socket);

/* need asyn operation */
int32_t sd_accept(uint32_t socket, uint32_t *accept_sock, SD_SOCKADDR *addr);

int32_t sd_connect(uint32_t socket, const SD_SOCKADDR *addr);
int32_t sd_asyn_proxy_connect(uint32_t socket, const char* host, uint16_t port,  void* user_own_data, void* user_msg_data);

int32_t sd_recv(uint32_t socket, char *buffer, int32_t bufsize, int32_t *recved_len);
int32_t sd_recvfrom(uint32_t socket, char *buffer, int32_t bufsize, SD_SOCKADDR *addr, int32_t *recved_len);

int32_t sd_send(uint32_t socket, char *buffer, int32_t sendsize, int32_t *sent_len);
int32_t sd_sendto(uint32_t socket, char *buffer, int32_t sendsize, const SD_SOCKADDR *addr, int32_t *sent_len);

bool sd_IsPeerActiveDisconnect(int32_t sock);
/* some api */

/* @Simple Function@
 * Return : errcode of sock.
 */
int32_t get_socket_error(uint32_t sock);


int32_t sd_set_snd_timeout(uint32_t socket, uint32_t timeout /* ms */);
int32_t sd_set_rcv_timeout(uint32_t socket, uint32_t timeout /* ms */);


int32_t sd_getpeername(uint32_t socket, SD_SOCKADDR *addr);

int32_t sd_getsockname(uint32_t socket, SD_SOCKADDR *addr);

int32_t sd_set_socket_ttl(uint32_t socket, int32_t ttl);
void* sd_gethostbyname(const char * name);
int sd_setnonblock( int fd );

#if defined(_ANDROID_LINUX)
int sd_tag_socket_to_uid(int sockfd, uid_t uid);
#endif

#ifdef __cplusplus
}
#include <string>
//这是一个‘浅拷贝’实现的addr，当使用ipv6地址时指向一块区域，对象本身可以方便和ipv4地址一起放在数组中使用
class SD_IPADDR
{
private:
    struct _in6addr
    {
        in6_addr addr;
        int ref;
    };
    uint16_t _sin_family;
    union{
        _in6addr* _sin_addr6;
        in_addr _sin_addr4;
    };
public:
    SD_IPADDR() : _sin_family(AF_INET),_sin_addr6(NULL){}
    SD_IPADDR(const SD_IPADDR& other)  // copy construct
    {
        _sin_family = other._sin_family;
        if(other._sin_family == AF_INET6)
        {
            _sin_addr6 = other._sin_addr6;
            _sin_addr6->ref ++;
        }
        else
        {
            _sin_addr4 = other._sin_addr4;
        }
    }
    SD_IPADDR(in_addr_t nip)
    {
        _sin_family = AF_INET;
        _sin_addr4.s_addr = nip;
    }
    SD_IPADDR& operator=(in_addr_t nip)
    {
        _reset();  //赋值前先断掉‘浅拷贝’联系，这一点非常非常重要
        _sin_family = AF_INET;
        _sin_addr4.s_addr = nip;
        return *this;
    }
    SD_IPADDR& operator=(in_addr nip)
    {
        _reset();  //赋值前先断掉‘浅拷贝’联系，这一点非常非常重要
        _sin_family = AF_INET;
        _sin_addr4.s_addr = nip.s_addr;
        return *this;
    }
    SD_IPADDR& operator=(in6_addr nip6)
    {
        _reset();
        _sin_family = AF_INET6;
        _sin_addr6 = new _in6addr;
        _sin_addr6->ref = 1;
        memcpy(&_sin_addr6->addr,&nip6,sizeof(in6_addr));
        return *this;
    }
    SD_IPADDR& operator=(const SD_IPADDR& other)
    {
        _reset();
        _sin_family = other._sin_family;
        if(other._sin_family == AF_INET6)
        {
            _sin_addr6 = other._sin_addr6;
            _sin_addr6->ref ++;
        }
        else
        {
            _sin_addr4 = other._sin_addr4;
        }
        return *this;
    }
    ~SD_IPADDR()
    {
        _reset();
    }
    bool operator==(const SD_IPADDR& other)
    {
        if(_sin_family != other._sin_family) return false;
        if(_sin_family==AF_INET) return (_sin_addr4.s_addr==other._sin_addr4.s_addr);
        if(_sin_family==AF_INET6)
        {
            return (memcmp(&(_sin_addr6->addr),&(other._sin_addr6->addr),sizeof(in6_addr))==0);
        }
        return false;
    }
    inline const in6_addr& addr6() const
    {
        return _sin_addr6->addr;
    }
    inline const in_addr& addr4() const
    {
        return _sin_addr4;
    }
    inline bool isV4() const {return (_sin_family==AF_INET);}
    inline bool isV6() const {return (_sin_family==AF_INET6);}

    std::string toString() const
    {
        if(isV4())
        {
            char v4addr[MAX_SERVER_IP_LEN];
            return inet_ntop(_sin_family, &_sin_addr4, v4addr, MAX_SERVER_IP_LEN);
        }
        if(isV6())
        {
            char v6addr[MAX_IN6ADDR_STR_LEN];
            return inet_ntop(_sin_family, &_sin_addr6->addr, v6addr, MAX_IN6ADDR_STR_LEN);
        }
        return "<null>";
    }

    void setToSdSockaddr(SD_SOCKADDR *sockaddr) const
    {
        sockaddr->_sin_family = _sin_family;
        if (isV6())
        {
            memcpy(&(sockaddr->_sin6_addr), &(_sin_addr6->addr), sizeof(struct in6_addr));
        }
        else
        {
            sockaddr->_sin_addr = _sin_addr4.s_addr;
        }
    }

protected:
    void _reset()
    {
        if(_sin_family == AF_INET6 && _sin_addr6)
        {
            if(--_sin_addr6->ref == 0)
            {
                delete _sin_addr6;
            }
        }
        _sin_family = AF_INET;
        _sin_addr6 = NULL;
    }
};

#endif
#endif
