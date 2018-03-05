#pragma once
#include <string>
#include <stdint.h>
#include <errno.h>

class StringHelper
{
public:
    static std::string Trim(std::string str, bool blTrim = true, bool bRTrim = true);
    static std::string Upper(std::string str);
    static std::string Lower(std::string str);
    static std::string Format(const char* format, ...); /// 默认长度 4096
    static std::string NFormat(int32_t len, const char* format, ...);
    static std::string FormatTimeMS(uint64_t ullTimeMs = 0); /// 0 表示获取当前的时间对应的字符串

    static std::string ErrInfo(const char* szFullPath, int32_t nLine, int32_t nErrno = errno, const char* szFormat = NULL, ...);
    static std::string ToHex(const char* s, int32_t len);
    static std::string ToHex(const std::string &s);
};
