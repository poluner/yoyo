#ifndef basic_type_convertion_h__
#define basic_type_convertion_h__

#include "utility/define.h"
#include <string>
#include <vector>



class BasicTypeConversion
{

public:

    static std::string Char2Str( char );
    static std::string Short2Str( short );
    static std::string Long2Str( long );
    static std::string LongLong2Str( int64_t );

    static std::string UChar2Str( unsigned char );
    static std::string UShort2Str( unsigned short );
    static std::string ULong2Str( unsigned long );
    static std::string ULongLong2Str( uint64_t );


    // parse full std::string to an integer value or throw an exception if std::string is incorrect
    // exception includes:
    // 1.empty std::string
    // 2.overflow: ex. fullstr2char( "1000" )
    // 3.include non-numeric character: ex. fullstr2long( "123abc" )
    // the 'space characters' in the begin of std::string will be skipped without throwing exception.
    // but the 'spaces characters' in the end of  std::string will cause exception of no.3
    //static int Str2Char(const std::string & , char &);
    //static int Str2Short(const std::string &, short & );
    //static int Str2Long(const std::string &, long & );
    //static int Str2LongLong(const std::string &, int64_t &);

    //static int Str2UChar( const std::string &, unsigned char & );
    //static int Str2UShort( const std::string &, unsigned short & );
    //static int Str2ULong( const std::string &, unsigned long & );
    //static int Str2ULongLong( const std::string &, uint64_t & );

    static char     Str2Char(const std::string & );
    static short    Str2Short(const std::string & );
    static long     Str2Long(const std::string & );
    static int64_t   Str2LongLong(const std::string & );

    static unsigned char    Str2UChar( const std::string & );
    static unsigned short   Str2UShort( const std::string & );
    static unsigned long    Str2ULong( const std::string & );
    static uint64_t  Str2ULongLong( const std::string & );

    static bool HexStringToInt(const std::string& , int32_t*);

    static bool HexToChar(char hex[2], char* c);
    static std::string CharToHex(unsigned char c);

    static bool IsHexStringVaild(const std::string& strHex);

    static bool IsHexCharVaild(char c);


    //////////////////////////////////////////////////////////////////////////
    //todo[lvfei]:下面的函数需要移到其他地方去

    // pBuffer: 需要解析的字符串
    // nLength: 字符串的长度
    // delimiter: 分隔符
    // name value
    // return value: 如果不包含分隔符则返回false，如果name或value不存在，则赋值为""
    static bool NameValueParse(const char* pBuffer, int32_t nLength,
                               char delimiter,
                               std::string& name, std::string& value);

    // 分割字符串，例如将“a b c d e”以空格符分割为"a","b","c","d","e" 五个字符串
    // 支持连续分隔符及头尾含分隔符，例如“  a   b  c  ”以空格符分割为"a","b","c"
    static void DivideString(  const std::string & src, char devide_char,
                               std::vector< std::string > & result );

    static std::string Trim(const std::string& str);

    static void ToLower(std::string& str);

    static bool IsSuffix(const std::string& strFull, const std::string& strSuffix);

private:
    static unsigned ConvertHexDigit(char c);
    static bool IsHexNumValid(char hex[2]);
    static std::string TrimHelper(int trimType, const std::string& str);

};


#endif // basic_type_convertion_h__

