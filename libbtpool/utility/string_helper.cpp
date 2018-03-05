#include <stdarg.h>
#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <sys/time.h>
#include "utility/string_helper.h"
#include "utility/sd_time.h"
#include "utility/sd_fs.h"

std::string StringHelper::Trim(std::string strSrc, bool blTrim, bool bRTrim)
{
    std::string::size_type nSize  = strSrc.length();
    std::string::size_type posBeg = 0;
    if (blTrim)
    {
        for (; posBeg < nSize; posBeg++)
        {
            if (' ' != strSrc[posBeg])
                break;
        }
    }

    int32_t posEnd = nSize-1;
    if (bRTrim)
    {
        for (; posEnd >= 0; posEnd--)
        {
            if (' ' != strSrc[posEnd])
                break;
        }
    }

    std::string strDst = strSrc.substr(posBeg, posEnd-posBeg+1);

    return  strDst;
}

std::string StringHelper::Upper(std::string strSrc)
{
    int32_t nSize = strSrc.length();
    char*   szDst = new char[nSize+1];
    char   delta = 'a' - 'A';

    for (int32_t nOff = 0; nOff < nSize; nOff++)
    {
        if (strSrc[nOff] >= 'a' && strSrc[nOff] <= 'z')
        {
            szDst[nOff] = strSrc[nOff] - delta;
        }
        else
        {
            szDst[nOff] = strSrc[nOff];
        }
    }

    szDst[nSize] = 0;
    std::string strDst(szDst, nSize);

    delete[] szDst;
    return  strDst;
}

std::string StringHelper::Lower(std::string strSrc)
{
    int32_t nSize = strSrc.length();
    char*   szDst = new char[nSize+1];
    char   delta = 'a' - 'A';

    for (int32_t nOff = 0; nOff < nSize; nOff++)
    {
        if (strSrc[nOff] >= 'A' && strSrc[nOff] <= 'Z')
        {
            szDst[nOff] = strSrc[nOff] + delta;
        }
        else
        {
            szDst[nOff] = strSrc[nOff];
        }
    }

    szDst[nSize] = 0;
    std::string strDst(szDst, nSize);

    delete[] szDst;
    return  strDst;
}

std::string StringHelper::NFormat(int32_t len, const char *format, ...)
{
    char*   szContent = new char[len];
    if (NULL == szContent)
        return  "";

    va_list args;
    va_start(args, format);

    vsnprintf(szContent, len, format, args);

     va_end(args);

     std::string strContent(szContent);

     delete szContent;
    szContent = NULL;

    return  strContent;
}

std::string StringHelper::Format(const char *format, ...)
{
    char szContent[4096] = {0};
    va_list args;
    va_start(args, format);
    vsnprintf(szContent, sizeof(szContent), format, args);
     va_end(args);
     std::string strContent(szContent);
     return strContent;
}

std::string StringHelper::FormatTimeMS(uint64_t ullTimeMs)
{
    struct tm local_time = { 0 };
    char szTime[UCHAR_MAX] = { 0 };

    if (0 == ullTimeMs)
    {
        ullTimeMs = sd_current_time_ms();
    }

    time_t lTimeSecond = (time_t)(ullTimeMs / 1000);
    localtime_r(&lTimeSecond, &local_time);

    sprintf(szTime, "%04d-%02d-%02d %02d:%02d:%02d:%03d",
        local_time.tm_year + 1900, local_time.tm_mon + 1, local_time.tm_mday,
        local_time.tm_hour, local_time.tm_min, local_time.tm_sec, (int32_t)(ullTimeMs % 1000));

    return std::string(szTime);
}

std::string StringHelper::ErrInfo(const char* szFullPath, int32_t nLine, int32_t nErrno, const char* szFormat, ...)
{
    char szContent[4096] = { 0 };
    const char* szFile = sd_get_file_name(szFullPath);
    snprintf(szContent, sizeof(szContent), "%s-%d-%d", szFile, nLine, nErrno);

    if (NULL != szFormat)
    {
        strcat(szContent, ":");
        int32_t nLen = strlen(szContent);
        va_list args;
        va_start(args, szFormat);
        vsnprintf(szContent + nLen, sizeof(szContent) - nLen, szFormat, args);
        va_end(args);
    }

    return std::string(szContent);
}

std::string StringHelper::ToHex(const char* s, int32_t s_len)
{
    if (s_len <= 0)
    {
        return "";
    }

    int32_t d_len = s_len * 2 + 1;
    char* d = new char[d_len];
    if (NULL == d)
    {
        return "";
    }

    int32_t s_off = 0;
    int32_t d_off = 0;
    for (s_off = 0; s_off < s_len; s_off++)
    {
        sprintf(d + d_off, "%02hhX", s[s_off]);
        d_off += 2;
    }

    std::string d_str(d, d_len-1);
    delete[] d;
    return d_str;
}

std::string StringHelper::ToHex(const std::string &s)
{
    return ToHex(s.c_str(), s.length());
}
