#include "utility/basic_type_convertion.h"
#include "utility/sd_assert.h"
#include <math.h>
#include <stdio.h>
#include <sstream>


std::string BasicTypeConversion::Char2Str( char c)
{
    return Long2Str( (long)c );
}

std::string BasicTypeConversion::Short2Str( short i)
{
    return Long2Str( (long)i );
}

std::string BasicTypeConversion::Long2Str( long i)
{
    char out[16] = {0};
    sprintf(out, "%lu", i);
    return out;
}

std::string BasicTypeConversion::LongLong2Str( int64_t i)
{
    char out[32];
    sprintf(out, "%lld", i);
    return out;
}

std::string BasicTypeConversion::UChar2Str( unsigned char c)
{
    return ULong2Str( (unsigned long)c );
}

std::string BasicTypeConversion::UShort2Str( unsigned short i)
{
    return ULong2Str( (unsigned long)i );
}

std::string BasicTypeConversion::ULong2Str( unsigned long i)
{
    char out[16];
    sprintf(out, "%lu", i);
    return out;
}

std::string BasicTypeConversion::ULongLong2Str( uint64_t i)
{
    char out[32];
    sprintf(out, "%llu", i);
    return out;
}

char BasicTypeConversion::Str2Char( const std::string & s )
{
    return (char)Str2Long(s);
}

short BasicTypeConversion::Str2Short( const std::string & s )
{
    return (short)Str2Long(s);
}

long BasicTypeConversion::Str2Long( const std::string & s )
{
    return atol( s.c_str() );
}

int64_t BasicTypeConversion::Str2LongLong( const std::string & s )
{
    return atoll( s.c_str() );
}

unsigned char BasicTypeConversion::Str2UChar( const std::string & s )
{
    return (unsigned char)Str2Long( s );

}

unsigned short BasicTypeConversion::Str2UShort( const std::string & s )
{
    return (unsigned short)Str2Long( s );
}

unsigned long BasicTypeConversion::Str2ULong( const std::string & s )
{
    return atol( s.c_str() );
}

uint64_t BasicTypeConversion::Str2ULongLong( const std::string & s )
{
    return atoll( s.c_str() );
}

bool BasicTypeConversion::HexStringToInt(const std::string& strHex , int32_t* result)
{
    if (!IsHexStringVaild(strHex))
    {
        return false;
    }
    else
    {
        if (strHex.size() > sizeof(int32_t) * 2)
        {
            sd_assert(false);
        }
        double dResult = 0;
        for (unsigned i = 0; i < strHex.size() ; ++i)
        {
            unsigned n = ConvertHexDigit(strHex[strHex.size() - i - 1]);
            dResult += (n * pow(16.0, i*1.0));
        }
        *result = (int32_t)dResult;
    }
    return true;
}

bool BasicTypeConversion::IsHexStringVaild( const std::string& strHex )
{
    for (unsigned i = 0; i < strHex.size(); ++i)
    {
        if (!IsHexCharVaild(strHex[i]))
        {
            return false;
        }
    }
    return true;
}

bool BasicTypeConversion::IsHexCharVaild( char c )
{
    if ((c >= '0') && (c <= '9'))
    {
        return true;
    }
    else if ((c >= 'a') && (c <= 'f'))
    {
        return true;
    }
    else if ((c >= 'A' )&& (c <= 'F'))
    {
        return true;
    }

    return false;
}

unsigned BasicTypeConversion::ConvertHexDigit( char c )
{
    if ((c <= '9') && (c >= '0'))
    {
        return (c - '0');
    }
    if ((c >= 'a') && (c <= 'f'))
    {
        return ((c - 'a') + 10);
    }
    if ((c < 'A') || (c > 'F'))
    {
        sd_assert(false);
    }
    return ((c - 'A') + 10);
}

bool BasicTypeConversion::HexToChar(char hex[2], char* c )
{
    if (IsHexNumValid(hex))
    {
        unsigned high = ConvertHexDigit(hex[0] );
        unsigned low = ConvertHexDigit(hex[1]);
        *c = (high << 4) + low;
        return true;
    }
    else
    {
        return false;
    }
}

std::string BasicTypeConversion::CharToHex( unsigned char c )
{
    char hex[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    std::string strTemp;
    strTemp += hex[c>>4];
    strTemp += hex[c&15];
    return strTemp;
}

bool BasicTypeConversion::IsHexNumValid( char hex[2] )
{
    return (IsHexCharVaild(hex[0]) && IsHexCharVaild(hex[1]));
}

bool BasicTypeConversion::NameValueParse(const char* pBuffer, int32_t nLength,
        char delimiter,
        std::string& name, std::string& value )
{
    sd_assert(NULL != pBuffer && 0 != nLength);
    int32_t index = 0;
    bool bDelimiterFound = false;
    while(index < nLength)
    {
        if (bDelimiterFound == false && pBuffer[index] == delimiter)
        {
            bDelimiterFound = true;
            break;
        }
        ++index;
    }
    if (!bDelimiterFound)
    {
        return false;
    }

    if (index > 0)
    {
        name.assign(pBuffer, index);
    }
    else
    {
        name = "";
    }

    if (index < (nLength - 1) )
    {
        value.assign(pBuffer+index + 1, nLength-index-1);
    }
    else
    {
        value = "";
    }
    return true;
}

void BasicTypeConversion::DivideString(const std::string & src,
                                       char devide_char,
                                       std::vector<std::string> & result )
{
    sd_assert(!src.empty());
    result.clear();
    if ( !src.empty() )
    {
        std::string::size_type pos_s = 0, pos_e = 0;
        while( true )
        {
            pos_e = src.find( devide_char, pos_s );
            if ( std::string::npos == pos_e )
            {
                result.push_back( src.substr(pos_s, src.size()-pos_s) );
                return;
            }
            else
            {
                // 找到连续的分隔符
                if ( pos_s == pos_e )
                {
                    pos_s++;
                }
                else
                {
                    result.push_back( src.substr(pos_s, pos_e-pos_s) );
                    pos_s=pos_e+1;
                }

                if ( pos_s >= src.size() )
                {
                    return;
                }
            }
        }
    }
}

std::string BasicTypeConversion::Trim( const std::string& str )
{
    if (str.empty())
    {
        return str;
    }
    return TrimHelper(2, str);
}

std::string BasicTypeConversion::TrimHelper(int trimType, const std::string& str)
{
    unsigned end = str.size() - 1;
    unsigned start = 0;
    if (trimType != 1)
    {
        start = 0;
        while (start < str.size())
        {
            if (str[start] != ' ' && str[start] != '\t' && str[start] != '\n' && str[start] != '\r')
            {
                break;
            }
            start++;
        }
    }
    if (trimType != 0)
    {
        if (str.empty())
        {
            return "";
        }
        else
        {
            end = str.size() - 1;
            while (end >= start)
            {
                if (str[end] != ' ' && str[end] != '\t' && str[end] != '\n' && str[end] != '\r')
                {
                    break;
                }
                end--;
            }
        }
    }
    return str.substr(start, end - start + 1);
}

void BasicTypeConversion::ToLower(std::string& str )
{
    std::string::size_type len = str.length();
    for (std::string::size_type i = 0; i < len; i++)
    {
        if ((str[i] <= 'Z') && (str[i] >= 'A'))
            str[i] += 32;
    }
}

bool BasicTypeConversion::IsSuffix( const std::string& strFull, const std::string& strSuffix )
{
    int nFullLen = strFull.length();
    int nSuffixLen = strSuffix.length();
    if (0 == nSuffixLen)
    {
        return true;
    }

    if (nSuffixLen > nFullLen)
    {
        return false;
    }

    std::string::const_reverse_iterator itFull = strFull.rbegin();
    std::string::const_reverse_iterator itSuffix = strSuffix.rbegin();
    while(true)
    {
        if (*itFull == *itSuffix)
        {
            ++itSuffix;
            ++itFull;
        }
        else
        {
            return false;
        }

        if (itSuffix == strSuffix.rend())
        {
            break;
        }
    }
    return true;
}


