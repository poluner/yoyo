#include "utility/errcode.h"
#include "utility/sd_string.h"
#include "utility/utility.h"
#include "utility/list.h"
#include "utility/arg.h"
#include "utility/sd_fs.h"


#if defined(LINUX)
#include <memory.h>
#include<ctype.h>
#endif
#include <string.h>
#include <stdio.h>


#define PRN_LBUF_SIZE  (512)

static const unsigned char g_char_map[] =
{
    (unsigned char)'\000',(unsigned char)'\001',(unsigned char)'\002',(unsigned char)'\003',(unsigned char)'\004',(unsigned char)'\005',(unsigned char)'\006',(unsigned char)'\007',
    (unsigned char)'\010',(unsigned char)'\011',(unsigned char)'\012',(unsigned char)'\013',(unsigned char)'\014',(unsigned char)'\015',(unsigned char)'\016',(unsigned char)'\017',
    (unsigned char)'\020',(unsigned char)'\021',(unsigned char)'\022',(unsigned char)'\023',(unsigned char)'\024',(unsigned char)'\025',(unsigned char)'\026',(unsigned char)'\027',
    (unsigned char)'\030',(unsigned char)'\031',(unsigned char)'\032',(unsigned char)'\033',(unsigned char)'\034',(unsigned char)'\035',(unsigned char)'\036',(unsigned char)'\037',
    (unsigned char)'\040',(unsigned char)'\041',(unsigned char)'\042',(unsigned char)'\043',(unsigned char)'\044',(unsigned char)'\045',(unsigned char)'\046',(unsigned char)'\047',
    (unsigned char)'\050',(unsigned char)'\051',(unsigned char)'\052',(unsigned char)'\053',(unsigned char)'\054',(unsigned char)'\055',(unsigned char)'\056',(unsigned char)'\057',
    (unsigned char)'\060',(unsigned char)'\061',(unsigned char)'\062',(unsigned char)'\063',(unsigned char)'\064',(unsigned char)'\065',(unsigned char)'\066',(unsigned char)'\067',
    (unsigned char)'\070',(unsigned char)'\071',(unsigned char)'\072',(unsigned char)'\073',(unsigned char)'\074',(unsigned char)'\075',(unsigned char)'\076',(unsigned char)'\077',
    (unsigned char)'\100',(unsigned char)'\141',(unsigned char)'\142',(unsigned char)'\143',(unsigned char)'\144',(unsigned char)'\145',(unsigned char)'\146',(unsigned char)'\147',
    (unsigned char)'\150',(unsigned char)'\151',(unsigned char)'\152',(unsigned char)'\153',(unsigned char)'\154',(unsigned char)'\155',(unsigned char)'\156',(unsigned char)'\157',
    (unsigned char)'\160',(unsigned char)'\161',(unsigned char)'\162',(unsigned char)'\163',(unsigned char)'\164',(unsigned char)'\165',(unsigned char)'\166',(unsigned char)'\167',
    (unsigned char)'\170',(unsigned char)'\171',(unsigned char)'\172',(unsigned char)'\133',(unsigned char)'\134',(unsigned char)'\135',(unsigned char)'\136',(unsigned char)'\137',
    (unsigned char)'\140',(unsigned char)'\141',(unsigned char)'\142',(unsigned char)'\143',(unsigned char)'\144',(unsigned char)'\145',(unsigned char)'\146',(unsigned char)'\147',
    (unsigned char)'\150',(unsigned char)'\151',(unsigned char)'\152',(unsigned char)'\153',(unsigned char)'\154',(unsigned char)'\155',(unsigned char)'\156',(unsigned char)'\157',
    (unsigned char)'\160',(unsigned char)'\161',(unsigned char)'\162',(unsigned char)'\163',(unsigned char)'\164',(unsigned char)'\165',(unsigned char)'\166',(unsigned char)'\167',
    (unsigned char)'\170',(unsigned char)'\171',(unsigned char)'\172',(unsigned char)'\173',(unsigned char)'\174',(unsigned char)'\175',(unsigned char)'\176',(unsigned char)'\177',
    (unsigned char)'\200',(unsigned char)'\201',(unsigned char)'\202',(unsigned char)'\203',(unsigned char)'\204',(unsigned char)'\205',(unsigned char)'\206',(unsigned char)'\207',
    (unsigned char)'\210',(unsigned char)'\211',(unsigned char)'\212',(unsigned char)'\213',(unsigned char)'\214',(unsigned char)'\215',(unsigned char)'\216',(unsigned char)'\217',
    (unsigned char)'\220',(unsigned char)'\221',(unsigned char)'\222',(unsigned char)'\223',(unsigned char)'\224',(unsigned char)'\225',(unsigned char)'\226',(unsigned char)'\227',
    (unsigned char)'\230',(unsigned char)'\231',(unsigned char)'\232',(unsigned char)'\233',(unsigned char)'\234',(unsigned char)'\235',(unsigned char)'\236',(unsigned char)'\237',
    (unsigned char)'\240',(unsigned char)'\241',(unsigned char)'\242',(unsigned char)'\243',(unsigned char)'\244',(unsigned char)'\245',(unsigned char)'\246',(unsigned char)'\247',
    (unsigned char)'\250',(unsigned char)'\251',(unsigned char)'\252',(unsigned char)'\253',(unsigned char)'\254',(unsigned char)'\255',(unsigned char)'\256',(unsigned char)'\257',
    (unsigned char)'\260',(unsigned char)'\261',(unsigned char)'\262',(unsigned char)'\263',(unsigned char)'\264',(unsigned char)'\265',(unsigned char)'\266',(unsigned char)'\267',
    (unsigned char)'\270',(unsigned char)'\271',(unsigned char)'\272',(unsigned char)'\273',(unsigned char)'\274',(unsigned char)'\275',(unsigned char)'\276',(unsigned char)'\277',
    (unsigned char)'\300',(unsigned char)'\341',(unsigned char)'\342',(unsigned char)'\343',(unsigned char)'\344',(unsigned char)'\345',(unsigned char)'\346',(unsigned char)'\347',
    (unsigned char)'\350',(unsigned char)'\351',(unsigned char)'\352',(unsigned char)'\353',(unsigned char)'\354',(unsigned char)'\355',(unsigned char)'\356',(unsigned char)'\357',
    (unsigned char)'\360',(unsigned char)'\361',(unsigned char)'\362',(unsigned char)'\363',(unsigned char)'\364',(unsigned char)'\365',(unsigned char)'\366',(unsigned char)'\367',
    (unsigned char)'\370',(unsigned char)'\371',(unsigned char)'\372',(unsigned char)'\333',(unsigned char)'\334',(unsigned char)'\335',(unsigned char)'\336',(unsigned char)'\337',
    (unsigned char)'\340',(unsigned char)'\341',(unsigned char)'\342',(unsigned char)'\343',(unsigned char)'\344',(unsigned char)'\345',(unsigned char)'\346',(unsigned char)'\347',
    (unsigned char)'\350',(unsigned char)'\351',(unsigned char)'\352',(unsigned char)'\353',(unsigned char)'\354',(unsigned char)'\355',(unsigned char)'\356',(unsigned char)'\357',
    (unsigned char)'\360',(unsigned char)'\361',(unsigned char)'\362',(unsigned char)'\363',(unsigned char)'\364',(unsigned char)'\365',(unsigned char)'\366',(unsigned char)'\367',
    (unsigned char)'\370',(unsigned char)'\371',(unsigned char)'\372',(unsigned char)'\373',(unsigned char)'\374',(unsigned char)'\375',(unsigned char)'\376',(unsigned char)'\377',
};

int32_t sd_strncpy(char *dest, const char *src, int32_t size)
{
    while (size && (*dest++ = *src++))
        size--;

    if (size)
    {
        while(--size)
            *dest++ = '\0';
    }

    return SUCCESS;
}

int32_t sd_vsnprintf(char *buffer, int32_t bufsize, const char *fmt, sd_va_list ap)
{
    int32_t src_idx = 0, obj_idx = 0;
    int32_t ret_val = SUCCESS;
    char *string_param = 0;

    buffer[bufsize - 1] = 0;
    for(obj_idx = 0, src_idx = 0; obj_idx < bufsize - 1 && fmt[src_idx];)
    {
        if(fmt[src_idx] == '%')
        {
            switch(fmt[src_idx + 1])
            {
            case 'd':
                ret_val = sd_i32toa(sd_va_arg(ap, int32_t), buffer + obj_idx, bufsize - obj_idx, 10);
                CHECK_VALUE(ret_val);

                src_idx += 2;
                break;
            case 'u':
                ret_val = sd_u32toa(sd_va_arg(ap, uint32_t), buffer + obj_idx, bufsize - obj_idx, 10);
                CHECK_VALUE(ret_val);

                src_idx += 2;
                break;
            case 'x':
            case 'X':
                ret_val = sd_u32toa(sd_va_arg(ap, uint32_t), buffer + obj_idx, bufsize - obj_idx, 16);
                CHECK_VALUE(ret_val);

                src_idx += 2;
                break;
            case 's':
                string_param = sd_va_arg(ap, char*);
                if(string_param)
                    ret_val = sd_strncpy(buffer + obj_idx, string_param, bufsize - obj_idx - 1);
                else
                    buffer[obj_idx] = 0;

                CHECK_VALUE(ret_val);

                src_idx += 2;
                break;

            case 'l':
                if(fmt[src_idx + 2] == 'd')
                {
                    ret_val = sd_i32toa(sd_va_arg(ap, int32_t), buffer + obj_idx, bufsize - obj_idx, 10);
                    CHECK_VALUE(ret_val);

                    src_idx += 3;
                }
                else if(fmt[src_idx + 2] == 'u')
                {
                    ret_val = sd_u32toa(sd_va_arg(ap, uint32_t), buffer + obj_idx, bufsize - obj_idx, 10);
                    CHECK_VALUE(ret_val);

                    src_idx += 3;
                }
                else if(sd_strncmp(fmt + src_idx + 2, "ld", 2) == 0)
                {
                    ret_val = sd_i64toa(sd_va_arg(ap, int64_t), buffer + obj_idx, bufsize - obj_idx, 10);
                    CHECK_VALUE(ret_val);

                    src_idx += 4;
                }
                else if(sd_strncmp(fmt + src_idx + 2, "lu", 2) == 0)
                {
                    ret_val = sd_u64toa(sd_va_arg(ap, uint64_t), buffer + obj_idx, bufsize - obj_idx, 10);
                    CHECK_VALUE(ret_val);

                    src_idx += 4;
                }
                else
                {
                    buffer[obj_idx] = fmt[src_idx++];
                    buffer[obj_idx + 1] = 0; /* obj_idx < bufsize - 1 */
                }

                break;
            default:
                buffer[obj_idx] = fmt[src_idx++];
                buffer[obj_idx + 1] = 0; /* obj_idx < bufsize - 1 */
                break;;
            }
            obj_idx += sd_strlen(buffer + obj_idx);
        }
        else
            buffer[obj_idx++] = fmt[src_idx++];
    }

    buffer[obj_idx] = 0;

    return obj_idx;
}

int32_t sd_snprintf(char *buffer, int32_t bufsize, const char *fmt, ...)
{
    int32_t ret_val = SUCCESS;

    sd_va_list ap;
    sd_va_start(ap, fmt);

#if 0
#ifdef WINCE
    ret_val = _vsnprintf(buffer, bufsize, fmt, ap);
    buffer[bufsize - 1] = 0;
#endif
#ifdef LINUX
    ret_val = vsnprintf(buffer, bufsize, fmt, ap);
    buffer[bufsize - 1] = 0;
#endif

#else
    ret_val = sd_vsnprintf(buffer, bufsize, fmt, ap);
#endif

    sd_va_end(ap);

    return ret_val;
}



int32_t sd_strlen(const char *str)
{
    if(str)
        return strlen(str);
    else
        return 0;
}

int32_t sd_strcat(char *dest, const char *src, int32_t n)
{
    strncat(dest, src, n);
    return SUCCESS;
}

int32_t sd_strcmp(const char *s1, const char *s2)
{
    return strcmp(s1, s2);
}

int32_t sd_strncmp(const char *s1, const char *s2, int32_t n)
{
    return strncmp(s1, s2, n);
}

int32_t sd_stricmp(const char *s1, const char *s2)
{
#if defined(LINUX)
    return strcasecmp(s1, s2);
#elif defined(WINCE)
    return _stricmp(s1, s2);
#endif
}

int32_t sd_strnicmp(const char *s1, const char *s2, int32_t n)
{
#if defined(LINUX)
    return strncasecmp(s1, s2, n);
#elif defined(WINCE)
    return _strnicmp(s1, s2, n);
#else
    return NOT_IMPLEMENT;
#endif
}

char* sd_strchr(char *dest, char ch, int32_t from)
{
    return strchr(dest + from, (int32_t)ch);
}

char* sd_strichr(char *dest, char ch, int32_t from)
{
    char *s = (char *)(dest + from);
    for (;;)
    {
        if (g_char_map[(uint8_t)*s] == g_char_map[(uint8_t)ch])
            return (char *) s;
        if (*s == 0)
            return 0;
        s++;
    }
}

char* sd_strstr(const char *dest, const char *search_str, int32_t from)
{
    return (char*)strstr((const char *)dest + from, search_str);
}

char* sd_stristr(const char *dest, const char *search_str, int32_t from)
{
    const char *p, *q;
    char *s1 = (char *)(dest + from);
    const char *s2 = search_str;
    for (; *s1; s1++)
    {
        p = s1, q = s2;
        while (*q && *p)
        {
            if (g_char_map[(uint8_t)*q] != g_char_map[(uint8_t)*p])
                break;
            p++, q++;
        }
        if (*q == 0)
            return (char *)s1;
    }
    return NULL;
}

char* sd_strrchr(const char *dest, char ch)
{
    return (char*)strrchr(dest, (int32_t)ch);
}

char* sd_strirchr(char *dest, char ch)
{
    uint32_t i = 0;
    while (dest[i] != 0)
        i++;
    do
        if (g_char_map[(uint8_t)dest[i]] == g_char_map[(uint8_t)ch])
            return (char *) dest + i;
    while (i-- != 0);
    return NULL;
}

/* Nonzero if X is not aligned on a "long" boundary.  */
#define UNALIGNED_X(X) \
  ((int32_t)X & (sizeof (int32_t) - 1))

/* Nonzero if either X or Y is not aligned on a "long" boundary.  */
#define UNALIGNED(X, Y) \
  (((int32_t)X & (sizeof (int32_t) - 1)) | ((int32_t)Y & (sizeof (int32_t) - 1)))

/* How many bytes are copied each iteration of the 4X unrolled loop.  */
#define BIGBLOCKSIZE    (sizeof (int32_t) << 2)

/* How many bytes are copied each iteration of the word copy loop.  */
#define LITTLEBLOCKSIZE (sizeof (int32_t))

/* Threshhold for punting to the byte copier.  */
#define TOO_SMALL(LEN)  ((LEN) < BIGBLOCKSIZE)

int32_t sd_memset(void *dest, int32_t c, int32_t count)
{
    memset(dest, c, count);
    return SUCCESS;
}

int32_t sd_memcpy(void *dest, const void *src, int32_t n)
{
    memcpy(dest, src, n);
    return SUCCESS;
//#endif
}

int32_t sd_memmove(void *dest, const void *src, int32_t n)
{
    char *dstp;
    const char *srcp;

    if (dest == src || n == 0) return SUCCESS;

    srcp = (const char*)src;
    dstp = (char *)dest;
    if (srcp < dstp)
        while (n-- != 0)
            dstp[n] = srcp[n];
    else
        while (n-- != 0)
            *dstp++ = *srcp++;
    return SUCCESS;
}

int32_t sd_memcmp(const void* src, const void* dest, uint32_t len)
{
    int32_t i;
    const int8_t *src_m = (const int8_t *)src;
    const int8_t *dest_m = (const int8_t *)dest;

    for(i=0; i<len; i++)
    {
        if(src_m[i]!=dest_m[i])
        {
            return src_m[i]-dest_m[i];
        }
    }
    return 0;
}

int32_t sd_trim_prefix_lws( char * str )
{
    char * temp = str,* temp2 = str;

    if(temp[0]=='\0')
        return SUCCESS;

    while((temp[0]!='\0')&&((temp[0]==' ')||(temp[0]=='\t')||(temp[0]=='\r')||(temp[0]=='\n')))
        temp++;

    if(temp!=str)
    {
        while(temp[0]!='\0')
        {
            temp2[0]=temp[0];
            temp++;
            temp2++;
        }
        temp2[0]='\0';
        //sd_memcpy(str, temp, strlen(temp)+1);
    }

    return SUCCESS;
}

int32_t sd_trim_postfix_lws( char * str )
{
    char * temp = NULL;

    if(str[0] == '\0')
        return SUCCESS;

    temp = str+sd_strlen(str)-1;

    while((temp[0]==' ')||(temp[0]=='\t')||(temp[0]=='\r')||(temp[0]=='\n'))
    {
        if(temp== str)
        {
            str[0] = '\0';
            return SUCCESS;
        }
        temp--;
    }

    str[temp - str+1] = '\0';
    return SUCCESS;
}

char sd_toupper(char ch)
{
#ifndef LINUX
    if( ch >= 'a' && ch <= 'z')
        return (ch + 'A' - 'a');
    else
        return ch;
#else
    return toupper(ch);
#endif
}

char sd_tolower(char ch)
{
#ifndef LINUX
    if( ch >= 'A' && ch <= 'Z')
        return (ch - 'A' + 'a');
    else
        return ch;
#else
    return tolower(ch);
#endif
}

void sd_strtolower(char * str)
{
    while(str&&*str!='\0')
    {
        *str = sd_tolower(*str);
        str++;
    }
    return;
}

/*  指定最大字符的长度，返回其截断的子串的长度，utf-8编码 */
uint32_t sd_get_sub_utf8_str_len(char *utf8_str, uint32_t max_len)
{
    uint32_t count = 0;	// 字的个数，包含英文或中文等
    uint32_t index = 0;
    uint32_t len = 0;
    //uint32_t ret_index = 0;	// 返回的长度，在index的增长过程中可能会超过max_len，所以这个存的是每次增长前的长度
    char c = 0;

    if(utf8_str == NULL) return 0;

    len = strlen(utf8_str);

    while (index < len && index < max_len)
    {
//		ret_index = index;
        c = utf8_str[index];

//		if (index >= len)
//		{
//			if (index <= max_len)
//				ret_index = index;
//
//			break;
//		}

        if (c & 0x80)
        {
            c <<= 1;
            count++;

            while (c & 0x80)
            {
                count++;
                c <<= 1;
            }
            if (count <= len && count <= max_len)
            {
                index = count;
            }
            else
            {
                break;
            }
        }
        else
        {
            count++;
            index++;
        }
    }

    return index;
}


int32_t sd_vfprintf(int32_t fd, const char *fmt, sd_va_list ap)
{
    int32_t ret_val = 0;
    uint32_t write_size = 0;
    char buffer[PRN_LBUF_SIZE];

    ret_val = sd_vsnprintf(buffer, sizeof(buffer), fmt, ap);
    ret_val = sd_write(fd, buffer, ret_val, &write_size);

    return ret_val;
}


int32_t sd_fprintf(int32_t fd, const char *fmt, ...)
{
    int32_t ret_val = SUCCESS;

    sd_va_list ap;
    sd_va_start(ap, fmt);

    ret_val = sd_vfprintf(fd, fmt, ap);

    sd_va_end(ap);

    return ret_val;
}

char* sd_strndup(const char* src, uint32_t n)
{
    uint32_t len = n + 1;
    char* dest = NULL;
    dest = (char*)malloc(len);
    if (dest == NULL)
    {
        return NULL;
    }

    sd_strncpy(dest, src, n);
    dest[n] = '\0';

    return dest;
}

