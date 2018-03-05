#ifndef __PACKAGE_HELPER_H__20141124
#define __PACKAGE_HELPER_H__20141124

#include <stddef.h>
#include <stdint.h>
#include <string>
#include "utility/errcode.h"

#define DeclarePush(op,type) bool \
    op##Value(const type &value)
#define DeclarePop(op,type) bool \
    op##Value(type &value)

class PackageHelper
{
public:
    PackageHelper(char* buf, int32_t size);
    //如果因为长度不够发生错误，后续都会错误。 加入剩余2的时候写入4失败，后续写入多少都会持续失败
    //template<typename T>
    //bool PushValue(const T &value);
    //bool PushValue(uint8_t value){ return
    //DeclareFunc(push,int32_t);
    DeclarePush(Push,uint8_t);
    DeclarePush(Push,uint16_t);
    DeclarePush(Push,uint32_t);
    DeclarePush(Push,uint64_t);

    bool PushString(const std::string &str);
    bool PushCStr(const char* buf,int32_t size=-1);
    bool PushBytes(const char* buf,int32_t size);

    //template<typename T>
    //bool PopValue(T &value);
    DeclarePop(Pop,uint8_t);
    DeclarePop(Pop,uint16_t);
    DeclarePop(Pop,uint32_t);
    DeclarePop(Pop,int32_t);
    DeclarePop(Pop,uint64_t);

    bool PopString(std::string &str);
    bool PopCStr(char* buf,int32_t &size);

    //从还剩size字节未读的位置开始读，也就是说size表示buf从后往前可用的长度
    bool Retreat(int32_t size);  //先使用remainSize记录，然后使用retreat返回重读
    bool IgnoreByte(int32_t size);
    bool IgnoreString();
    inline int32_t RemainSize()
    {
        return m_size;
    }
    inline bool ErrorOccur()
    {
        return (m_size<0);
    }
private:
    char* m_buf;
    int32_t m_size;
};








#endif
