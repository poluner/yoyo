#include "utility/utility.h"
#include "utility/errcode.h"
#include "utility/sd_string.h"
#include "utility/sd_socket.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <net/if.h>

#if defined(LINUX)
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#elif defined(WINCE)
#include <winsock2.h>
#endif
#include <string>
#include <sstream>


#include   <stdio.h> 
#include   <sys/ioctl.h> 
#include   <sys/socket.h> 
#include   <netinet/in.h> 
#include   <net/if.h> 
#include   <string.h>

#include <arpa/inet.h>
#include <errno.h>



#define INT2HEX(ch)	((ch) < 10 ? (ch) + '0' : (ch) - 10 + 'A')

uint32_t sd_inet_addr(const char *cp)
{

#if defined(LINUX)
    return inet_addr(cp);
#elif defined(WINCE)
    return inet_addr(cp);
#endif

}

int32_t sd_inet_aton(const char *cp, uint32_t *nip)
{
    int32_t ret_val = SUCCESS;

#if defined(LINUX)
    ret_val = inet_addr(cp);

    if(ret_val == (int32_t)INADDR_NONE)
        ret_val = ERROR_INVALID_INADDR;
    else
    {
        *nip = ret_val;
        ret_val = SUCCESS;
    }

#elif defined(WINCE)
    ret_val = inet_addr(cp);

    if(ret_val == INADDR_NONE)
        ret_val = ERROR_INVALID_INADDR;
    else
    {
        *nip = ret_val;
        ret_val = SUCCESS;
    }
#endif

    return ret_val;
}


int32_t sd_inet_ntoa(uint32_t inp, char *out_buf, int32_t bufsize)
{

#if defined(LINUX)
    char *out = NULL;
    struct in_addr addr;
    sd_memset(&addr, 0, sizeof(addr));
    addr.s_addr = inp;
    out = inet_ntoa(addr);
    sd_strncpy(out_buf, out, bufsize);

#elif defined(WINCE)
    char *out = NULL;
    struct in_addr addr;
    addr.s_addr = inp;

    out = inet_ntoa(addr);
    sd_strncpy(out_buf, out, bufsize);
#endif

    return SUCCESS;
}

int32_t sd_atoi(const char* nptr)
{
    return atoi(nptr);
}

int32_t sd_srand(uint32_t seeds)
{
    srand(seeds);

    return SUCCESS;
}

int32_t sd_rand(void)
{
    return rand();
}

uint16_t sd_htons(uint16_t h)
{
#if defined(LINUX)
    return htons(h);
#elif defined(WINCE)
    return htons(h);
#endif
}

uint32_t sd_htonl(uint32_t h)
{
#if defined(LINUX)
    return htonl(h);
#elif defined(WINCE)
    return htonl(h);
#endif
}

uint16_t sd_ntohs(uint16_t n)
{
#if defined(LINUX)
    return ntohs(n);
#elif defined(WINCE)
    return ntohs(n);
#endif
}

uint32_t sd_ntohl(uint32_t n)
{
#if defined(LINUX)
    return ntohl(n);
#elif defined(WINCE)
    return ntohl(n);
#endif
}
int32_t char2hex( unsigned char c , char *out_buf, int32_t out_bufsize)
{
    char hex[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    if(out_bufsize<3) return -1;
    out_buf[0] = hex[c/16];
    out_buf[1] = hex[c%16];
    out_buf[2] = '\0';
    return SUCCESS;
}
int32_t str2hex(const char *in_buf, int32_t in_bufsize, char *out_buf, int32_t out_bufsize)
{
    int32_t ret_val = SUCCESS;
    int32_t idx = 0;
    uint8_t *inp = (uint8_t*)in_buf, ch;

    for(idx = 0; idx < in_bufsize; idx++)
    {
        if(idx * 2 >= out_bufsize)
            break;

        ch = inp[idx] >> 4;
        out_buf[idx * 2] = INT2HEX(ch);
        ch = inp[idx] & 0x0F;
        out_buf[idx * 2 + 1] = INT2HEX(ch);
    }
    return ret_val;
}

//将Hex字符串转换成普通字符串
int32_t hex2str(const char* hex, int32_t hex_len, char* str, int32_t str_len)
{
    int i=0;
    int j=0;
    for(j = 0; j < hex_len -1; )
    {
        if(i >= str_len) break;

        int32_t a =  sd_hex_2_int(hex[j++]);
        int32_t b =  sd_hex_2_int(hex[j++]);
        str[i++] = (char)(a *16 + b);
    }

    return SUCCESS;
}

int32_t sd_hex_2_int(char chr)
{
    if( chr >= '0' && chr <= '9' )
    {
        return chr - '0';
    }
    if( chr >= 'a' && chr <= 'f' )
    {
        return chr - 'a' + 10;
    }
    if( chr >= 'A' && chr <= 'F' )
    {
        return chr - 'A' + 10;
    }
    else
    {
        return 0;
    }
}

int32_t sd_hexstr_2_int(const char * in_buf, int32_t in_bufsize)
{
    char * temp = (char*)in_buf;
    int32_t result = 0;

    if(in_bufsize > 10)
    {
        return 0;
    }
    if(in_bufsize > 8 && (*temp != '0' || (*(temp+1) != 'x' && *(temp+1) != 'X')))
    {
        return 0;
    }
    if(*temp == '0' && (*(temp+1) == 'x' || *(temp+1) == 'X'))
    {
        temp += 2;
        in_bufsize -= 2;
    }
    while(in_bufsize-- > 0)
    {
        result = result*16 + sd_hex_2_int(*temp++);
    }
    return result;
}

BOOL sd_is_bcid_equal(const uint8_t* _bcid1,uint32_t bcid1_len,const uint8_t* _bcid2,uint32_t bcid2_len)
{
    int32_t i ;

    if(_bcid1 == NULL||_bcid2==NULL)
    {
        return FALSE;
    }

    if(bcid1_len!=bcid2_len)
        return FALSE;

    for(i=0; i<bcid1_len; i++)
        if(_bcid1[i]!=_bcid2[i])
        {
            return FALSE;
        }

    return TRUE;
}

BOOL sd_is_cid_equal(const uint8_t* _cid1,const uint8_t* _cid2)
{
    int32_t i ;

    if(_cid1 == NULL||_cid2==NULL)
    {
        return FALSE;
    }

    for(i=0; i<CID_SIZE; i++)
        if(_cid1[i]!=_cid2[i])
        {
            return FALSE;
        }

    return TRUE;
}

/* @Simple Function@
 * Return : TRUE if valid,or FALSE
 *
 */
BOOL sd_is_cid_valid(const uint8_t* _cid)
{
    uint8_t _cid_t[CID_SIZE];

    if(_cid == NULL)
    {
        return FALSE;
    }

    sd_memset(_cid_t,0,CID_SIZE);

    if(sd_is_cid_equal(_cid,_cid_t))
    {
        return FALSE;
    }

    return TRUE;
}

BOOL sd_is_bcid_valid(uint64_t filesize, uint32_t bcid_size, uint32_t block_size)
{
    uint32_t bcid_num = (filesize + block_size-1)/(block_size );
    return (bcid_num==bcid_size) ? TRUE : FALSE;
}

uint32_t sd_elf_hashvalue(const char *str, uint32_t hash_value)
{
    uint32_t x = 0;

    if(!str)
        return 0;

    while(*str)
    {
        hash_value  =  (hash_value << 4 ) + (*(str++));
        if((x  =  hash_value & 0xF0000000L) != 0)
        {
            hash_value  ^=  (x  >> 24 );
            hash_value  &=   ~ x;
        }
    }
    return hash_value;
}




int32_t sd_str_to_u64( const char *str, uint32_t strlen, uint64_t *val )
{
#ifdef  NOT_SUPPORT_LARGE_INT_64
    uint32_t _result = 0;
    int j = 0;
    if( str == NULL || strlen == 0 )
        return INVALID_ARGUMENT;

    for(j=0; j<strlen; j++)
    {
        if((str[j]<'0')||(str[j]>'9'))
        {
            return INVALID_ARGUMENT;
        }
        else
        {
            _result *= 10;
            _result += str[j] - '0';
        }

        if(_result > (uint32_t)0xffffffff)
            //return BT_UNSUPPORT_64BIT_FILE_SIZE;
            return INVALID_ARGUMENT;
    }
    *val = _result;

#else
    uint64_t _result = 0;
    int j = 0;
    if( str == NULL || strlen == 0 )
        return INVALID_ARGUMENT;

    for(j=0; j<strlen; j++)
    {
        if((str[j]<'0')||(str[j]>'9'))
        {
            return INVALID_ARGUMENT;
        }
        else
        {
            _result *= 10;
            _result += str[j] - '0';
        }
    }
    *val = _result;
#endif
    return SUCCESS;
}

int32_t sd_u64_to_str( uint64_t val, char *str, uint32_t strlen )
{
#ifdef  NOT_SUPPORT_LARGE_INT_64
    return sd_snprintf( str, strlen, "%u", val);
#else
    return sd_snprintf( str, strlen, "%llu", val);
#endif
}

int32_t sd_u32_to_str( uint32_t val, char *str, uint32_t strlen )
{
    return sd_snprintf( str, strlen, "%u", val );
}

int32_t sd_abs(int32_t val)
{
    return abs(val);

}

BOOL sd_data_cmp(uint8_t* i_data, uint8_t* d_data, int32_t count)
{
    while(count-->0)
    {
        if(*i_data++ != *d_data++)
            return FALSE;
    }

    return TRUE;
}

/*
	Convert the string(40 chars hex ) to content id(20 bytes in hex)
*/
int32_t sd_string_to_cid(const char * str,uint8_t *cid)
{
    int i=0,is_cid_ok=0,cid_byte=0;

    if((str==NULL)||(sd_strlen(str)<40)||(cid==NULL))
        return -1;

    for(i=0; i<20; i++)
    {
        if(((str[i*2])>='0')&&((str[i*2])<='9'))
        {
            cid_byte=(str[i*2]-'0')*16;
        }
        else if(((str[i*2])>='A')&&((str[i*2])<='F'))
        {
            cid_byte=((str[i*2]-'A')+10)*16;
        }
        else if(((str[i*2])>='a')&&((str[i*2])<='f'))
        {
            cid_byte=((str[i*2]-'a')+10)*16;
        }
        else
            return -1;

        if(((str[i*2+1])>='0')&&((str[i*2+1])<='9'))
        {
            cid_byte+=(str[i*2+1]-'0');
        }
        else if(((str[i*2+1])>='A')&&((str[i*2+1])<='F'))
        {
            cid_byte+=(str[i*2+1]-'A')+10;
        }
        else if(((str[i*2+1])>='a')&&((str[i*2+1])<='f'))
        {
            cid_byte+=(str[i*2+1]-'a')+10;
        }
        else
            return -1;

        cid[i]=cid_byte;
        if((is_cid_ok==0)&&(cid_byte!=0))
            is_cid_ok=1;
    }

    if(is_cid_ok==1)
        return SUCCESS;
    else
        return 1;


}

/*
	Convert the string(chars hex ) to  bytes in hex
*/
int32_t sd_string_to_hex(char * str,uint8_t * hex)
{
    int i=0,is_hex_ok=0,hex_byte=0;

    if((str==NULL)||(sd_strlen(str)==0)||(hex==NULL))
        return -1;

    while(str[i*2]!='\0')
    {
        if(((str[i*2])>='0')&&((str[i*2])<='9'))
        {
            hex_byte=(str[i*2]-'0')*16;
        }
        else if(((str[i*2])>='A')&&((str[i*2])<='F'))
        {
            hex_byte=((str[i*2]-'A')+10)*16;
        }
        else if(((str[i*2])>='a')&&((str[i*2])<='f'))
        {
            hex_byte=((str[i*2]-'a')+10)*16;
        }
        else
            return -1;

        if(((str[i*2+1])>='0')&&((str[i*2+1])<='9'))
        {
            hex_byte+=(str[i*2+1]-'0');
        }
        else if(((str[i*2+1])>='A')&&((str[i*2+1])<='F'))
        {
            hex_byte+=(str[i*2+1]-'A')+10;
        }
        else if(((str[i*2+1])>='a')&&((str[i*2+1])<='f'))
        {
            hex_byte+=(str[i*2+1]-'a')+10;
        }
        else
            return -1;

        hex[i++]=hex_byte;
        if((is_hex_ok==0)&&(hex_byte!=0))
            is_hex_ok=1;
    }

    if(is_hex_ok==1)
        return SUCCESS;
    else
        return 1;
}



int32_t sd_i32toa(int32_t value, char *buffer, int32_t buf_len, int32_t radix)
{
    return sd_i64toa((int64_t)value, buffer, buf_len, radix);
}

int32_t sd_i64toa(int64_t value, char *buffer, int32_t buf_len, int32_t radix)
{
    if(value < 0)
    {
        if(buf_len < 3) /* "-1\0" */
        {
            sd_memset(buffer, 0, buf_len);
            return SUCCESS;
        }
        *buffer = '-';
        return sd_u64toa(-value, buffer + 1, buf_len - 1, radix);
    }
    else
    {
        return sd_u64toa(value, buffer, buf_len, radix);
    }
}

int32_t sd_u32toa(uint32_t value, char *buffer, int32_t buf_len, int32_t radix)
{
    return sd_u64toa((uint64_t)value, buffer, buf_len, radix);
}

int32_t sd_u64toa(uint64_t value, char *buffer, int32_t buf_len, int32_t radix)
{
    char hex[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    int32_t idx = 0, last_idx = 0, half_idx = 0;
    uint64_t div_value = 0;
    char swap_tmp;

    if(radix > 16 || radix <= 0)
        return INVALID_ARGUMENT;

    idx = 0;
    if(value == 0 && idx < buf_len - 1)
    {
        buffer[idx++] = '0';
    }
    else
    {
        for(div_value = value, idx = 0; div_value > 0 && idx < buf_len - 1; div_value /= radix, idx++)
        {
            buffer[idx] = hex[div_value % radix];
        }
    }

    buffer[idx] = 0;

    /* inverse */

    last_idx = idx - 1;
    half_idx = idx / 2;
    for(idx = 0; idx < half_idx; idx ++)
    {
        swap_tmp = buffer[idx];
        buffer[idx] = buffer[last_idx - idx];
        buffer[last_idx - idx] = swap_tmp;
    }

    return SUCCESS;
}

uint32_t sd_digit_bit_count( uint64_t value )
{
    uint32_t bit_count = 0;

    if( value == 0 ) return 1;

    while( value > 0 )
    {
        value = value / 10;
        bit_count++;
    }
    return bit_count;
}

BOOL is_cmwap_prompt_page(const char * http_header,uint32_t header_len)
{
    int32_t len=0;
    char buffer[64];
    char *p1=NULL, *p2=NULL;

    p1 = sd_strstr( (char *)http_header, "HTTP/1.1 200 ",0 ) ;
    if((p1==NULL)||(p1-http_header>=header_len))
    {
        return FALSE;
    }

    p1 = sd_strstr( (char *)http_header, "Content-Type:",0 ) ;
    if((p1!=NULL)&&(p1-http_header<header_len))
    {
        p1+=13;
        p2 = sd_strstr( p1, "\r\n",0 ) ;
        if((p2!=NULL)&&(p2-http_header<header_len))
        {
            len = p2-p1;
            if(len>=64) len = 63;
            sd_memset(buffer, 0, 64);
            sd_memcpy(buffer,p1,len);
            if(NULL!=sd_strstr( buffer, "vnd.wap.",0 ) )
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}

/*******************************************************************************
* 函数名称	: calc_file_cid
* 功能描述	: 计算一个文件的cid
* 参　　数	: const tchar* file_path	文件对应的路径
* 参　　数	:  uint8_t * p_cid				保存cid的缓存指针
* 返 回 值	: int32_t					=0表示成功，<0表示出错
* 作　　者	: 曾雨青
* 设计日期	: 2011年5月9日
* 修改日期		   修改人		  修改内容
*******************************************************************************/



void sd_exit(int32_t errcode)
{
    exit(errcode);
}

int32_t sd_cid_to_hex_string(const char* cid, int32_t cid_len,  char* hex_cid_str, int32_t hex_cid_len)
{
    int32_t i = 0;
    int32_t l = 0;
    int32_t r = 0;
    uint8_t tmp = 0;
    char hex_array[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    if (cid_len * 2 > hex_cid_len)
    {
        return SD_ERROR;
    }
    for( i = 0; i < cid_len; ++i)
    {
        tmp = cid[i];
        l = (tmp>>4) & (0x0F);
        hex_cid_str[i*2] = hex_array[l];
        r = tmp & 0x0F;
        hex_cid_str[i*2+1] = hex_array[r];
    }
    return SUCCESS;
}


int32_t sd_gm_time(uint32_t time_sec, TIME_t *p_time)
{
    struct tm *tm_time = NULL;
    time_t _time_sec = (time_t)time_sec;
    if (p_time)
    {
        sd_memset(p_time, 0, sizeof(*p_time));
        if ((uint32_t)-1 != _time_sec)
        {
            tm_time = gmtime(&_time_sec);
            if (tm_time)
            {
                p_time->sec = tm_time->tm_sec;
                p_time->min = tm_time->tm_min;
                p_time->hour = tm_time->tm_hour;
                p_time->mday = tm_time->tm_mday;
                p_time->mon = tm_time->tm_mon;
                p_time->year = tm_time->tm_year + 1900;
                p_time->wday = tm_time->tm_wday;
                p_time->yday = tm_time->tm_yday;
//              p_time->isdst = tm_time->tm_isdst;
                return 0;
            }
        }
    }
    return 1;
}

/** convert first len of str to int64_t, *val will be 0 if not success*/
int32_t sd_str_to_i64_v2(const char* str, uint32_t len, int64_t* val)
{
#ifdef  NOT_SUPPORT_LARGE_INT_64
#error "Not work well without int64_t !!"
#else

    *val=0;
    if( str == NULL || len == 0 ) return INVALID_ARGUMENT;

    BOOL sign = FALSE;
    const char* p_cur = str;
    const char* p_end = str+len;

    if(*p_cur=='-')sign=TRUE,++p_cur;
    else if(*p_cur=='+') ++p_cur;

    for( ; p_cur!=p_end ; ++p_cur)
    {
        if(*p_cur>='0' && *p_cur<='9')
            *val = *val*10 + (*p_cur&0xf);
        else return INVALID_ARGUMENT;
    }

    if(sign) *val = -*val;

#endif
    return SUCCESS;
}



#if defined(LINUX)
int32_t sd_getaddrinfo(const char *host, char *ip, int ip_len)
{
    int ret_val = 0;

#ifdef USING_DNS_GETHOSTBYNAME_R
    char tmp_buff[1024] = { 0 };
    struct hostent tmp_hostent;
    int  tmp_errono = 0;
    struct hostent* phostent = NULL;
    ret_val = gethostbyname_r(host, &tmp_hostent, tmp_buff, sizeof(tmp_buff), &phostent, &tmp_errono);
    if (ret_val != 0 || NULL == phostent)
    {
        return ret_val;
    }

    char **tmp_ip = phostent->h_addr_list;
    inet_ntop(phostent->h_addrtype, *tmp_ip, ip, ip_len);

#else
    struct addrinfo hints;
    struct addrinfo *result = NULL;
    struct sockaddr_in *sin_p = NULL;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; /* only for ipv4 */

    ret_val = getaddrinfo(host, NULL, &hints, &result);
    if(ret_val != 0 || !result)
    {
        return ret_val;
    }

    sin_p = (struct sockaddr_in *)result->ai_addr;
    inet_ntop(AF_INET, &sin_p->sin_addr, ip, ip_len);

    if (result)
        freeaddrinfo(result);
#endif

    return SUCCESS;
}
#endif


BOOL sd_is_lan_ip(uint32_t ip)
{
    uint8_t* pointer = (uint8_t*)&ip;
    uint8_t c1 = pointer[0];
    uint8_t c2 = pointer[1];
    if((c1 == 10) || ((c1 == 172) && (c2 > 15) && (c2 < 32)) || ((c1 == 192) && (c2 == 168)))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

uint32_t s_local_ip = 0;
uint32_t sd_get_local_ip(void)
{
/*
 * 此函数方法不准确, 只有正常发送过的socket获取其地址比较准确 by Zeyu
 *
 * [HYY]: 
 * 小米手机上存在usbnet0接口用于usb上网，该接口不管是否启用都配有地址，可以通过IFF_UP状态区分是否启用
 * 其他机型的手机，同时存在多个UP状态的接口可能性也比较低，暂时通过UP状态简单过滤。
 */
#if defined(LINUX)

#ifndef MACOS

    struct ifreq ifr, *pifr;
    struct ifconf ifc;
    uint32_t tmp_fd = 0;
    int32_t idx = 0, ret_val = 0;
    char if_buf[1024];
    struct sockaddr_in *paddr;


    if(s_local_ip == 0)
    {
        /* traverse network platform and get local ip */

        ret_val = sd_create_socket(SD_AF_INET, SD_SOCK_DGRAM, 0, &tmp_fd);
        if(ret_val != SUCCESS)
            return 0;

        ifc.ifc_len = sizeof(if_buf);
        ifc.ifc_buf = if_buf;
        ioctl(tmp_fd, SIOCGIFCONF, &ifc);

        pifr = ifc.ifc_req;
        for(idx = ifc.ifc_len / sizeof(struct ifreq); --idx >= 0; pifr++)
        {
            sd_strncpy(ifr.ifr_name, pifr->ifr_name, IFNAMSIZ);

            if(ioctl(tmp_fd, SIOCGIFFLAGS, &ifr) == 0)
            {
                if (!(ifr.ifr_flags & IFF_LOOPBACK) && (ifr.ifr_flags & IFF_UP))
                {
                    if(ioctl(tmp_fd, SIOCGIFADDR, &ifr) == 0)
                    {
                        paddr = (struct sockaddr_in*)(&ifr.ifr_addr);
                        s_local_ip = paddr->sin_addr.s_addr;
                        if(!sd_is_lan_ip(s_local_ip))
                            break;
                    }
                }
            }
        }

        sd_close_socket(tmp_fd);
    }
#else

    struct ifreq ifr, *pifr;
    struct ifconf ifc;
    uint32_t tmp_fd = 0;
    int32_t idx = 0, ret_val = 0;
    char if_buf[1024];
    struct sockaddr_in *paddr;
    char* cp, *cplim;

    if(s_local_ip == 0)
    {
        /* traverse network platform and get local ip */

        ret_val = sd_create_socket(SD_AF_INET, SD_SOCK_DGRAM, 0, &tmp_fd);
        if(ret_val != SUCCESS)
            return 0;

        ifc.ifc_len = sizeof(if_buf);
        ifc.ifc_buf = if_buf;
        ioctl(tmp_fd, SIOCGIFCONF, &ifc);

        pifr = ifc.ifc_req;
        cp = if_buf;
        cplim = if_buf + ifc.ifc_len;

        for(cp = if_buf; cp < cplim; )
        {
            pifr = (struct ifreq*)cp;

            sd_strncpy(ifr.ifr_name, pifr->ifr_name, IFNAMSIZ);

            if(ioctl(tmp_fd, SIOCGIFFLAGS, &ifr) == 0)
            {
                if (!(ifr.ifr_flags & IFF_LOOPBACK) && (ifr.ifr_flags & IFF_UP))
                {
                    if(ioctl(tmp_fd, SIOCGIFADDR, &ifr) == 0)
                    {
                        paddr = (struct sockaddr_in*)(&ifr.ifr_addr);
                        s_local_ip = paddr->sin_addr.s_addr;
                    }
                }
            }
            cp += sizeof(pifr->ifr_name) + MAX(sizeof(pifr->ifr_addr), pifr->ifr_addr.sa_len);
        }

        sd_close_socket(tmp_fd);
    }

#endif
#endif

    return s_local_ip;
}

std::string double_to_string(double num)
{
    std::stringstream ss;
    ss << num;
    return ss.str();
}

std::string cid_to_hex_string(const std::string& cid)
{
    std::string result;
    result.reserve(cid.size()*2);
    char hex_array[] = "0123456789ABCDEF";
    int cid_len = cid.length();
    for( int i = 0; i < cid_len; ++i)
    {
        uint8_t tmp = cid[i];
        int32_t l = (tmp>>4) & (0x0F);
        int32_t r = tmp & 0x0F;
        result += hex_array[l];
        result += hex_array[r];
    }
    return result;
}
std::string cid_to_hex_string_lc(const std::string& cid)
{
    std::string result;
    result.reserve(cid.size()*2);
    char hex_array[] = "0123456789abcdef";
    int cid_len = cid.length();
    for( int i = 0; i < cid_len; ++i)
    {
        uint8_t tmp = cid[i];
        int32_t l = (tmp>>4) & (0x0F);
        int32_t r = tmp & 0x0F;
        result += hex_array[l];
        result += hex_array[r];
    }
    return result;
}

std::string sd_get_suffix(const std::string& filename)
{
    if (filename.find_last_of('.') == std::string::npos)
        return "";

    std::string name_suffix = filename.substr(filename.find_last_of('.') + 1);
    for(uint32_t i=0; i < name_suffix.size(); i++)
    {
        char& c = name_suffix[i];
        if((c>='A'&&c<='Z')||(c>='a'&&c<='z')||(c>='0'&&c<='9'))
        {
            c = tolower(c);
        }
        else
        {
            return "";
        }
    }
    //std::transform(name_suffix.begin(), name_suffix.end(), name_suffix.begin(), ::tolower);

    return name_suffix;
}

void make_http_respone(std::string & outStr, uint64_t filesize)
{
    outStr.clear();
    std::ostringstream oss;
    oss << filesize;
    
    outStr += "HTTP/1.1 200 OK\r\n";
    outStr += "Server: nginx\r\n";
    outStr += "Content-Length: "; outStr += oss.str(); outStr += "\r\n";
    outStr += "Connection: close\r\n";
    outStr += "\r\n";
}

void make_error_http_header(std::string & outStr,const std::string& resourceLine, const std::string& shubLine)
{
    outStr.clear();
    if(!resourceLine.empty() && !shubLine.empty())
    {
        outStr += resourceLine;
        outStr += "\r\n";
        outStr += shubLine;
        outStr += "\r\n";        
        return ;
    }
}


int32_t sd_inet6_pton(const char *pIp6Str, void *pIp6)
{
    int32_t ret_val;

#if defined(LINUX)
    ret_val = inet_pton(SD_AF_INET6, pIp6Str, pIp6);

    ret_val = (ret_val > 0) ? SUCCESS : IP6_INVALID_IN6ADDR;
#elif defined(WINCE)
    //TODO
    ret_val = IP6_INVALID_IN6ADDR;
#endif

    return ret_val;
}

int32_t sd_inet6_ntop(const void *pIp6, char *pIp6Str, int32_t strSize)
{
#if defined(LINUX)
    inet_ntop(SD_AF_INET6, pIp6, pIp6Str, strSize);
#elif defined(WINCE)
    //TODO
#endif

    return SUCCESS;
}

BOOL sd_try_convert_host2address(const std::string &host, SD_IPADDR &ip)
{
    uint32_t nIp = 0;
    if (SUCCESS == sd_inet_aton(host.c_str(), &nIp))    //域名就是ip
    {
#ifdef MACOS //兼容MAC和IOS的NAT64网络，如果设备只有IPv6地址，则该IPv4地址会被转成兼容型IPv6地址
        struct addrinfo hints, *res;
        sd_memset(&hints, 0, sizeof(struct addrinfo));
        res = NULL;
        hints.ai_family = PF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_DEFAULT;

        int32_t ret = getaddrinfo(host.c_str(), "http", &hints, &res);
        if (0 == ret && res != NULL && SD_AF_INET6 == res->ai_family)
        {
            //只处理被转成兼容型IPv6地址的情形
            ip = ((struct sockaddr_in6 *)(res->ai_addr))->sin6_addr;
        }
        else
        {
            ip = nIp;
        }

        if (res)
            freeaddrinfo(res);
#else
        ip = nIp;
#endif
        return TRUE;
    }

    struct in6_addr nIp6;
    sd_memset(&nIp6, 0, sizeof(struct in6_addr));
    if(host.length() <= 2)
        return FALSE;
    std::string Ip6Str = host.substr(1, host.length() - 2);
    if (SUCCESS == sd_inet6_pton(Ip6Str.c_str(), &nIp6))    //域名就是IPv6地址，如"[2000::1]"
    {
        ip = nIp6;
        return TRUE;
    }

    return FALSE;
}

#define BIT_MASK(bit_pos) (0x01 << (bit_pos))  
void sd_reset_bit(uint32_t* value, unsigned char pos)
{
    if (pos >= sizeof(uint32_t) * 8) {
        sd_assert(FALSE);
        return ;
    }
    *value = (*value & ~BIT_MASK(pos)); //与0做与(&)运算  
}

/*
bool sd_get_linux_peerid(char * buf, int buf_len)
{
    if(buf_len < 20)
    {
        printf("sd_get_linux_peerid buf_len < 20\n "); 
        return   false; 
    }    
    sd_memset(buf,0,buf_len);
    struct   ifreq   ifreq; 
    int   sock; 

    if((sock=socket(AF_INET,SOCK_STREAM,0)) <0) 
    { 
        printf("sd_get_linux_peerid socket\n "); 
        return   false; 
    } 
    strcpy(ifreq.ifr_name,"eth0"); 
    if(ioctl(sock,SIOCGIFHWADDR,&ifreq) <0) 
    { 
        printf("sd_get_linux_peerid ioctl\n "); 
        return   false; 
    } 
    sprintf(buf, "%02X%02X%02X%02X%02X%02X004V", 
            (unsigned   char)ifreq.ifr_hwaddr.sa_data[0], 
            (unsigned   char)ifreq.ifr_hwaddr.sa_data[1], 
            (unsigned   char)ifreq.ifr_hwaddr.sa_data[2], 
            (unsigned   char)ifreq.ifr_hwaddr.sa_data[3], 
            (unsigned   char)ifreq.ifr_hwaddr.sa_data[4], 
            (unsigned   char)ifreq.ifr_hwaddr.sa_data[5]); 
    return 0; 
}
*/


bool sd_get_linux_peerid(char * buf1, int buf_len)
{
#ifdef MACOS
    return false;
#else 
    if(buf_len < 20)
    {
        printf("sd_get_linux_peerid buf_len < 20\n "); 
        return   false; 
    }    
    sd_memset(buf1,0,buf_len);
    int fd;
    int interfaceNum = 0;
    struct ifreq buf[16];
    struct ifconf ifc;
    struct ifreq ifrcopy;
    char mac[16] = {0};
//     char ip[32] = {0};
//     char broadAddr[32] = {0};
//     char subnetMask[32] = {0};

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket");
        close(fd);
        return false;
    }

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = (caddr_t)buf;
    if (!ioctl(fd, SIOCGIFCONF, (char *)&ifc))
    {
        interfaceNum = ifc.ifc_len / sizeof(struct ifreq);
        printf("interface num = %d\n", interfaceNum);
        while (interfaceNum-- > 0)
        {
             printf("\ndevice name: %s\n", buf[interfaceNum].ifr_name);

             //ignore the interface that not up or not runing
             ifrcopy = buf[interfaceNum];
             if (ioctl(fd, SIOCGIFFLAGS, &ifrcopy))
             {
                printf("ioctl: %s [%s:%d]\n", strerror(errno), __FILE__, __LINE__);
                close(fd);
                return false;
             }

             //get the mac of this interface
             if (!ioctl(fd, SIOCGIFHWADDR, (char *)(&buf[interfaceNum])))
             {
                memset(mac, 0, sizeof(mac));
                snprintf(mac, sizeof(mac), "%02X%02X%02X%02X%02X%02X",
                            (unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[0],
                            (unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[1],
                            (unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[2],
                            (unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[3],
                            (unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[4],
                            (unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[5]);
                printf("device mac: %s\n", mac);
                
                snprintf(buf1, buf_len, "%02X%02X%02X%02X%02X%02X004V",
                            (unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[0],
                            (unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[1],
                            (unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[2],
                            (unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[3],
                            (unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[4],
                            (unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[5]);
                
                break;
             }
            else
            {
                printf("ioctl: %s [%s:%d]\n", strerror(errno), __FILE__, __LINE__);
                close(fd);
                return false;
            }

            /*
            //get the IP of this interface
            if (!ioctl(fd, SIOCGIFADDR, (char *)&buf[interfaceNum]))
            {
                snprintf(ip, sizeof(ip), "%s",
                            (char *)inet_ntoa(((struct sockaddr_in *)&(buf[interfaceNum].ifr_addr))->sin_addr));
                printf("device ip: %s\n", ip);
            }
            else
            {
                printf("ioctl: %s [%s:%d]\n", strerror(errno), __FILE__, __LINE__);
                close(fd);
                return -1;
            }

            //get the broad address of this interface
            if (!ioctl(fd, SIOCGIFBRDADDR, &buf[interfaceNum]))
            {
                snprintf(broadAddr, sizeof(broadAddr), "%s",
                            (char *)inet_ntoa(((struct sockaddr_in *)&(buf[interfaceNum].ifr_broadaddr))->sin_addr));
                printf("device broadAddr: %s\n", broadAddr);
            }
            else
            {
                printf("ioctl: %s [%s:%d]\n", strerror(errno), __FILE__, __LINE__);
                close(fd);
                return -1;
            }

            //get the subnet mask of this interface
            if (!ioctl(fd, SIOCGIFNETMASK, &buf[interfaceNum]))
            {
                snprintf(subnetMask, sizeof(subnetMask), "%s",
                            (char *)inet_ntoa(((struct sockaddr_in *)&(buf[interfaceNum].ifr_netmask))->sin_addr));
                printf("device subnetMask: %s\n", subnetMask);
            }
            else
            {
                printf("ioctl: %s [%s:%d]\n", strerror(errno), __FILE__, __LINE__);
                close(fd);
                return -1;
            }*/
        }
    }
    else
    {
        printf("ioctl: %s [%s:%d]\n", strerror(errno), __FILE__, __LINE__);
        close(fd);
        return false;
    }

    close(fd);
    return true;
#endif
}



