#include "utility/bytebuffer.h"
#include "utility/errcode.h"
#include "utility/sd_string.h"

#ifdef BIG_ENDIAN
#undef BIG_ENDIAN
#endif
#define BIG_ENDIAN		(0)
#ifdef LITTLE_ENDIAN
#undef LITTLE_ENDIAN
#endif
#define LITTLE_ENDIAN	(1)

static int32_t g_test_endian = 1;
static int32_t g_host_endian = LITTLE_ENDIAN;

#define TEST_ENDIAN	{g_host_endian = (*((char*)&(g_test_endian)) == 1);}

void bytebuffer_init(void)
{
    TEST_ENDIAN;
}

int32_t sd_get_int64_from_bg(char **buffer, int32_t *cur_buflen, int64_t *value)
{
    int32_t sizes = sizeof(int64_t), index = 0;
    char *pvalue = (char*)value;

#ifdef  NOT_SUPPORT_LARGE_INT_64
    if(*cur_buflen < sizes + sizeof(int32_t))
        return BUFFER_OVERFLOW;

    *cur_buflen -= (sizes + sizeof(int32_t));
#else
    if(*cur_buflen < sizes)
        return BUFFER_OVERFLOW;

    *cur_buflen -= sizes;
#endif
#ifdef  NOT_SUPPORT_LARGE_INT_64
    *buffer += ( sizeof(int32_t));
#endif

    if(g_host_endian == BIG_ENDIAN)
    {
        sd_memcpy(value, *buffer, sizeof(int64_t));
    }
    else
    {
        for(index = sizes; index > 0; index--)
        {
            *(pvalue++) = *(*buffer + index - 1);
        }
    }

    *buffer += sizes;

    return SUCCESS;
}

int32_t sd_get_int32_from_bg(char **buffer, int32_t *cur_buflen, int32_t *value)
{
    int32_t sizes = sizeof(int32_t), index = 0;
    char *pvalue = (char*)value;

    if(*cur_buflen < sizes)
        return BUFFER_OVERFLOW;

    *cur_buflen -= sizes;

    if(g_host_endian == BIG_ENDIAN)
    {
        sd_memcpy(value, *buffer, sizeof(int32_t));
    }
    else
    {
        for(index = sizes; index > 0; index--)
        {
            *(pvalue++) = *(*buffer + index - 1);
        }
    }

    *buffer += sizes;

    return SUCCESS;
}

int32_t sd_get_int16_from_bg(char **buffer, int32_t *cur_buflen, int16_t *value)
{
    int32_t sizes = sizeof(int16_t), index = 0;
    char *pvalue = (char*)value;

    if(*cur_buflen < sizes)
        return BUFFER_OVERFLOW;

    *cur_buflen -= sizes;

    if(g_host_endian == BIG_ENDIAN)
    {
        sd_memcpy(value, *buffer, sizeof(int16_t));
    }
    else
    {
        for(index = sizes; index > 0; index--)
        {
            *(pvalue++) = *(*buffer + index - 1);
        }
    }

    *buffer += sizes;

    return SUCCESS;
}

int32_t sd_get_int64_from_lt(char **buffer, int32_t *cur_buflen, int64_t *value)
{
    int32_t sizes = sizeof(int64_t), index = 0;
    char *pvalue = (char*)value;

#ifdef  NOT_SUPPORT_LARGE_INT_64
    if(*cur_buflen < sizes + sizeof(int32_t))
        return BUFFER_OVERFLOW;

    *cur_buflen -= (sizes + sizeof(int32_t));
#else
    if(*cur_buflen < sizes)
        return BUFFER_OVERFLOW;

    *cur_buflen -= sizes;
#endif

    if(g_host_endian == LITTLE_ENDIAN)
    {
        sd_memcpy(value, *buffer, sizeof(int64_t));
    }
    else
    {
        for(index = sizes; index > 0; index--)
        {
            *(pvalue++) = *(*buffer + index - 1);
        }
    }

    *buffer += sizes;
#ifdef  NOT_SUPPORT_LARGE_INT_64
    *buffer += ( sizeof(int32_t));
#endif

    return SUCCESS;
}

int32_t sd_get_int32_from_lt(char **buffer, int32_t *cur_buflen, int32_t *value)
{
    int32_t sizes = sizeof(int32_t), index = 0;
    char *pvalue = (char*)value;

    if(*cur_buflen < sizes)
        return BUFFER_OVERFLOW;

    *cur_buflen -= sizes;

    if(g_host_endian == LITTLE_ENDIAN)
    {
        sd_memcpy(value, *buffer, sizeof(int32_t));
    }
    else
    {
        for(index = sizes; index > 0; index--)
        {
            *(pvalue++) = *(*buffer + index - 1);
        }
    }

    *buffer += sizes;

    return SUCCESS;
}

int32_t sd_get_int16_from_lt(char **buffer, int32_t *cur_buflen, int16_t *value)
{
    int32_t sizes = sizeof(int16_t), index = 0;
    char *pvalue = (char*)value;

    if(*cur_buflen < sizes)
        return BUFFER_OVERFLOW;

    *cur_buflen -= sizes;

    if(g_host_endian == LITTLE_ENDIAN)
    {
        sd_memcpy(value, *buffer, sizeof(int16_t));
    }
    else
    {
        for(index = sizes; index > 0; index--)
        {
            *(pvalue++) = *(*buffer + index - 1);
        }
    }

    *buffer += sizes;

    return SUCCESS;
}

int32_t sd_get_int8(char **buffer, int32_t *cur_buflen, int8_t *value)
{
    if(*cur_buflen < 1)
        return BUFFER_OVERFLOW;

    *cur_buflen -= 1;

    *value = *((*buffer)++);

    return SUCCESS;
}

int32_t sd_set_int64_to_bg(char **buffer, int32_t *cur_buflen, int64_t value)
{
    int32_t sizes = sizeof(int64_t), index = 0;
    char *pvalue = (char*)&value;

#ifdef  NOT_SUPPORT_LARGE_INT_64
    if(*cur_buflen < sizes + sizeof(int32_t))
        return BUFFER_OVERFLOW;

    *cur_buflen -= (sizes + sizeof(int32_t));
#else
    if(*cur_buflen < sizes)
        return BUFFER_OVERFLOW;

    *cur_buflen -= sizes;
#endif
#ifdef  NOT_SUPPORT_LARGE_INT_64
    sd_memset(*buffer, 0 , sizeof(int32_t));
    *buffer += ( sizeof(int32_t));
#endif

    if(g_host_endian == BIG_ENDIAN)
    {
        sd_memcpy(*buffer, &value, sizeof(int64_t));
    }
    else
    {
        for(index = sizes; index > 0; index--)
        {
            *(*buffer + index - 1) = *(pvalue++);
        }
    }

    *buffer += sizes;

    return SUCCESS;
}

int32_t sd_set_int32_to_bg(char **buffer, int32_t *cur_buflen, int32_t value)
{
    int32_t sizes = sizeof(int32_t), index = 0;
    char *pvalue = (char*)&value;

    if(*cur_buflen < sizes)
        return BUFFER_OVERFLOW;

    *cur_buflen -= sizes;

    if(g_host_endian == BIG_ENDIAN)
    {
        sd_memcpy(*buffer, &value, sizeof(int32_t));
    }
    else
    {
        for(index = sizes; index > 0; index--)
        {
            *(*buffer + index - 1) = *(pvalue++);
        }
    }

    *buffer += sizes;

    return SUCCESS;
}

int32_t sd_set_int16_to_bg(char **buffer, int32_t *cur_buflen, int16_t value)
{
    int32_t sizes = sizeof(int16_t), index = 0;
    char *pvalue = (char*)&value;

    if(*cur_buflen < sizes)
        return BUFFER_OVERFLOW;

    *cur_buflen -= sizes;

    if(g_host_endian == BIG_ENDIAN)
    {
        sd_memcpy(*buffer, &value, sizeof(int16_t));
    }
    else
    {
        for(index = sizes; index > 0; index--)
        {
            *(*buffer + index - 1) = *(pvalue++);
        }
    }

    *buffer += sizes;

    return SUCCESS;
}

int32_t sd_set_int64_to_lt(char **buffer, int32_t *cur_buflen, int64_t value)
{
    int32_t sizes = sizeof(int64_t), index = 0;
    char *pvalue = (char*)&value;

#ifdef  NOT_SUPPORT_LARGE_INT_64
    if(*cur_buflen < sizes + sizeof(int32_t))
        return BUFFER_OVERFLOW;

    *cur_buflen -= (sizes + sizeof(int32_t));
#else
    if(*cur_buflen < sizes)
        return BUFFER_OVERFLOW;

    *cur_buflen -= sizes;
#endif

    if(g_host_endian == LITTLE_ENDIAN)
    {
        sd_memcpy(*buffer, &value, sizeof(int64_t));
    }
    else
    {
        for(index = sizes; index > 0; index--)
        {
            *(*buffer + index - 1) = *(pvalue++);
        }
    }

    *buffer += sizes;
#ifdef  NOT_SUPPORT_LARGE_INT_64
    sd_memset(*buffer, 0 , sizeof(int32_t));
    *buffer += ( sizeof(int32_t));
#endif

    return SUCCESS;
}

int32_t sd_set_int32_to_lt(char **buffer, int32_t *cur_buflen, int32_t value)
{
    int32_t sizes = sizeof(int32_t), index = 0;
    char *pvalue = (char*)&value;

    if(*cur_buflen < sizes)
        return BUFFER_OVERFLOW;

    *cur_buflen -= sizes;

    if(g_host_endian == LITTLE_ENDIAN)
    {
        sd_memcpy(*buffer, &value, sizeof(int32_t));
    }
    else
    {
        for(index = sizes; index > 0; index--)
        {
            *(*buffer + index - 1) = *(pvalue++);
        }
    }

    *buffer += sizes;

    return SUCCESS;
}

int32_t sd_set_int16_to_lt(char **buffer, int32_t *cur_buflen, int16_t value)
{
    int32_t sizes = sizeof(int16_t), index = 0;
    char *pvalue = (char*)&value;

    if(*cur_buflen < sizes)
        return BUFFER_OVERFLOW;

    *cur_buflen -= sizes;

    if(g_host_endian == LITTLE_ENDIAN)
    {
        sd_memcpy(*buffer, &value, sizeof(int16_t));
    }
    else
    {
        for(index = sizes; index > 0; index--)
        {
            *(*buffer + index - 1) = *(pvalue++);
        }
    }

    *buffer += sizes;

    return SUCCESS;
}

int32_t sd_set_int8(char **buffer, int32_t *cur_buflen, int8_t value)
{
    if(*cur_buflen < 1)
        return BUFFER_OVERFLOW;

    *cur_buflen -= 1;

    *((*buffer)++) = value;

    return SUCCESS;
}

int32_t sd_get_bytes(char **buffer, int32_t *cur_buflen, char *dest_buf, int32_t dest_len)
{
    if(*cur_buflen < dest_len)
        return BUFFER_OVERFLOW;

    sd_memcpy(dest_buf, *buffer, dest_len);

    *cur_buflen -= dest_len;
    *buffer += dest_len;

    return SUCCESS;
}

int32_t sd_set_bytes(char **buffer, int32_t *cur_buflen,const char *src_buf, int32_t src_len)
{
    if(src_len==0)
        return INVALID_ARGUMENT;

    if(*cur_buflen < src_len)
        return BUFFER_OVERFLOW;

    sd_memcpy(*buffer, src_buf, src_len);

    *cur_buflen -= src_len;
    *buffer += src_len;

    return SUCCESS;
}

int32_t sd_get_string(char **buffer, int32_t *cur_buflen, char *value, int32_t value_buffer_len, int32_t *value_len)
{
    int32_t ret = sd_get_int32_from_lt(buffer, cur_buflen, value_len);
    if (SUCCESS != ret) return ret;

    if (NULL == value )
    {
        *buffer -= sizeof(int32_t);
        *cur_buflen += sizeof(int32_t);
        return SUCCESS;
    }
    else if (value_buffer_len <*value_len)
    {
        *buffer -= sizeof(int32_t);
        *cur_buflen += sizeof(int32_t);
        return BUFFER_OVERFLOW;
    }

    ret = sd_get_bytes(buffer, cur_buflen, value, *value_len);
    return ret;

}

int32_t sd_set_string(char **buffer, int32_t *cur_buflen, char *value, int32_t value_len)
{
    int32_t ret = sd_set_int32_to_lt(buffer, cur_buflen, value_len);
    if (SUCCESS == ret)
    {
        ret = sd_set_bytes(buffer, cur_buflen, value, value_len);
    }
    return ret;
}

