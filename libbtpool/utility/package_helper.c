#include "package_helper.h"
#include "utility/bytebuffer.h"
#include "utility/errcode.h"

#define PushFunc8(buf,size,value) sd_set_int8(buf,size,value)
#define PushFunc16(buf,size,value) sd_set_int16_to_lt(buf,size,value)
#define PushFunc32(buf,size,value) sd_set_int32_to_lt(buf,size,value)
#define PushFunc64(buf,size,value) sd_set_int64_to_lt(buf,size,value)
#define PopFunc8(buf,size,pv) sd_get_int8(buf,size,(int8_t*)pv)
#define PopFunc16(buf,size,pv) sd_get_int16_from_lt(buf,size,(int16_t*)pv)
#define PopFunc32(buf,size,pv) sd_get_int32_from_lt(buf,size,(int32_t*)pv)
#define PopFunc64(buf,size,pv) sd_get_int64_from_lt(buf,size,(int64_t*)pv)

#define DefinePush(type,wide) bool PackageHelper::PushValue(const type &value)\
{\
    int32_t ret = PushFunc##wide(&m_buf,&m_size,value);\
    if(ret!=SUCCESS)\
    {  m_size = -1; return false; }\
    return true;\
}
#define DefinePop(type,wide) bool PackageHelper::PopValue(type &value)\
{\
    int32_t ret = PopFunc##wide(&m_buf,&m_size,&value);\
    if(ret!=SUCCESS)\
    {  m_size = -1; return false; }\
    return true;\
}

DefinePush(uint8_t,8)
DefinePush(uint16_t,16)
DefinePush(uint32_t,32)
DefinePush(uint64_t,64)
DefinePop(uint8_t,8)
DefinePop(uint16_t,16)
DefinePop(uint32_t,32)
DefinePop(int32_t,32)
DefinePop(uint64_t,64)

PackageHelper::PackageHelper(char *buf, int32_t size)
    : m_buf(buf)
    , m_size(size)
{
}



//template<typename T>
//bool PackageHelper::PushValue(const T &value)
//{
//    int32_t ret = pushV(&m_buf,&m_size,value, (sizeof(value)*8));
//    if(ret!=SUCCESS)
//    {  //使后续调用都会持续失败
//        //buf += m_size;
//        m_size = -1;
//        return false;
//    }
//    return true;
//}

bool PackageHelper::PushString(const std::string &str)
{
    return PushCStr(str.c_str(),str.size());
}

bool PackageHelper::PushCStr(const char *buf, int32_t size)
{
    if(size<0) size = strlen(buf);
    int32_t ret = PushFunc32(&m_buf,&m_size,size);
    if(ret != SUCCESS) return false;
    if(size>0)
    {
        return PushBytes(buf,size);
    }
    return (ret == SUCCESS);
}

bool PackageHelper::PushBytes(const char *buf, int32_t size)
{
    int32_t ret = sd_set_bytes(&m_buf,&m_size,buf,size);
    if(ret != SUCCESS) m_size = -1;
    return (ret == SUCCESS);
}


//template<typename T>
//bool PackageHelper::PopValue(T &value)
//{
//    int32_t ret = popV(&m_buf,&m_size,value,(sizeof(value)*8));
//    if(ret!=SUCCESS)
//    {  //使后续调用都会持续失败
//        //buf += m_size;
//        m_size = -1;
//        return false;
//    }
//    return true;
//}

bool PackageHelper::PopString(std::string &str)
{
    str.clear();
    int32_t len = 0;
    int32_t ret = PopFunc32(&m_buf,&m_size,&len);
    if (SUCCESS != ret)
    {
        return false;
    }

    if (m_size < len || len < 0)
    {
        return false;
    }

    str.assign(m_buf, len);
    m_size -= len;
    m_buf += len;
    return true;

}

bool PackageHelper::PopCStr(char *buf, int32_t &size)
{
    int32_t need;
    int32_t ret;
    ret = PopFunc32(&m_buf,&m_size,&need);
    if(ret == SUCCESS && need<=m_size && need<=size)
    {
        size = need;
        ret = sd_get_bytes(&m_buf,&m_size,buf,need);
        if(ret!=SUCCESS) m_size = -1;
        return (ret==SUCCESS);
    }
    else
    {
        m_size = -1;
    }
    return false;
}

bool PackageHelper::Retreat(int32_t size)
{
    sd_assert(m_size>=0);
    if(m_size<0)return false;
    int32_t move = m_size - size;
    m_buf += move;
    m_size -= move;
    return true;
}

bool PackageHelper::IgnoreByte(int32_t size)
{
    if(m_size>=size)
    {
        m_buf+=size;
        m_size-=size;
        return true;
    }
    else
    {
        m_size = -1;  //error set
        return false;
    }
}

bool PackageHelper::IgnoreString()
{
    int32_t need;
    int32_t ret;
    ret = PopFunc32(&m_buf,&m_size,&need);
    if(ret == SUCCESS && need<=m_size)
    {
        return this->IgnoreByte(need);
    }
    return false;

}
