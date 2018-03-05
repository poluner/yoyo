/*
 * ================================================================================================================
 *    Description:  BT Pool Server Common Struct CPP Head File
 *        Version:  1.0
 *        Created:  08/26/2016
 *         Author:  Melo
 * Modify History: 1. Created by Melo
 * ================================================================================================================
 */

#ifndef BTPOOL_P2SPROTOCOL_QUERY_STRUCT_TOOL_H_
#define BTPOOL_P2SPROTOCOL_QUERY_STRUCT_TOOL_H_

#include "common.h"

namespace BTPOOL
{

template <unsigned int stack_size>
class MyArray
{
public:
    MyArray()
    {
        m_stack_length = 0;
        m_max_stack_size = stack_size;
    }
    ~MyArray()
    {
    }

    void set(char* buffer, unsigned int buffer_length)
    {
        if (buffer_length <= m_max_stack_size )
            m_stack_length = buffer_length;
        else
            m_stack_length = m_max_stack_size;

        memcpy(m_stack, buffer, m_stack_length);
    }
    char operator[](int index) const
    {
        return m_stack[index];
    }
    std::string to_string() const
    {
        return std::string(m_stack, m_stack_length);
    }
    const char* c_str() const
    {
        return m_stack;
    }
    const unsigned int length() const
    {
        return m_stack_length;
    }

    const unsigned size() const
    {
        return m_max_stack_size;
    }

private:
    unsigned int m_max_stack_size;
    unsigned int m_stack_length;
    char m_stack[stack_size];
};

class HostByteBuffer
{
public:
    HostByteBuffer(char * buffer, const unsigned int max_buffer_len, bool del=false)
    {
        m_del_flag = del;
        m_buffer = buffer;
        m_offset = 0;
        m_capacity = max_buffer_len;
    }

    ~HostByteBuffer()
    {
        if (m_del_flag && m_buffer != NULL)
        {
            delete[] m_buffer;
            m_buffer = NULL;
        }
    }

    void set_del_flag(bool del_flag)
    {
        m_del_flag = del_flag;
    }

    bool setPosition(unsigned int position)
    {
        if (position >= m_capacity)
            return false;

        m_offset = position;
        return true;
    }

    const char* c_str()
    {
        return m_buffer;
    }

    unsigned int getPosition()
    {
        return m_offset;
    }

    unsigned int length()
    {
        return m_capacity;
    }

    void set_length(unsigned int length)
    {
        m_capacity = length;
    }

    void putBoolean(bool b)
    {
        memcpy(m_buffer+m_offset, &b, sizeof(b));
        m_offset += sizeof(b);
    }

    void putByte(unsigned char uc)
    {
        memcpy(m_buffer+m_offset, &uc, sizeof(uc));
        m_offset += sizeof(uc);
    }

    void putChar(char c)
    {
        memcpy(m_buffer+m_offset, &c, sizeof(c));
        m_offset += sizeof(c);
    }

    void putShort(short s)
    {
        memcpy(m_buffer+m_offset, &s, sizeof(s));
        m_offset += sizeof(s);
    }

    void putInt(int i)
    {
        memcpy(m_buffer+m_offset, &i, sizeof(i));
        m_offset += sizeof(i);
    }

    void putUInt(unsigned int i)
    {
        memcpy(m_buffer+m_offset, &i, sizeof(i));
        m_offset += sizeof(i);
    }

    void putFloat(float f)
    {
        memcpy(m_buffer+m_offset, &f, sizeof(f));
        m_offset += sizeof(f);
    }

    void putDouble(double d)
    {
        memcpy(m_buffer+m_offset, &d, sizeof(d));
        m_offset += sizeof(d);
    }

    void putULong(unsigned long long ull)
    {
        memcpy(m_buffer+m_offset, &ull, sizeof(ull));
        m_offset += sizeof(ull);
    }

    void putLong(long long ll)
    {
        memcpy(m_buffer+m_offset, &ll, sizeof(ll));
        m_offset += sizeof(ll);
    }

    void putBytes(const char* buffer, const unsigned int buffer_len)
    {
        unsigned int len = sizeof(unsigned int);
        unsigned int attr_len = buffer_len;
        memcpy(m_buffer+m_offset, &attr_len, len);
        m_offset += len;
        if (attr_len > 0)
        {
            len = attr_len;
            memcpy(m_buffer+m_offset, buffer, len);
            m_offset += len;
        }
    }

    void putString(const std::string& str)
    {
        unsigned int len = sizeof(unsigned int);
        unsigned int attr_len = str.length();
        memcpy(m_buffer+m_offset, &attr_len, len);
        m_offset += len;
        if (attr_len > 0)
        {
            len = attr_len;
            memcpy(m_buffer+m_offset, str.c_str(), len);
            m_offset += len;
        }
    }

    bool getBoolean(bool& b)
    {
        unsigned int len = sizeof(b);
        if ((m_offset+len) > m_capacity)
            return false;
        memcpy(&b, m_buffer+m_offset, len);
        m_offset += len;
        return true;
    }

    bool getByte(unsigned char& uc)
    {
        unsigned int len = sizeof(uc);
        if ((m_offset+len) > m_capacity)
            return false;
        memcpy(&uc, m_buffer+m_offset, len);
        m_offset += len;
        return true;
    }

    bool getChar(char& c)
    {
        unsigned int len = sizeof(c);
        if ((m_offset+len) > m_capacity)
            return false;
        memcpy(&c, m_buffer+m_offset, len);
        m_offset += len;
        return true;
    }

    bool getShort(short& s)
    {
        unsigned int len = sizeof(s);
        if ((m_offset+len) > m_capacity)
            return false;
        memcpy(&s, m_buffer+m_offset, len);
        m_offset += len;
        return true;
    }


    bool getUInt(uint32_t& i)
    {
        unsigned int len = sizeof(i);
        if ((m_offset+len) > m_capacity)
            return false;
        memcpy(&i, m_buffer+m_offset, len);
        //i = *((int*)(m_buffer+m_offset));
        m_offset += len;
        return true;
    }

    bool getInt(int& i)
    {
        unsigned int len = sizeof(i);
        if ((m_offset+len) > m_capacity)
            return false;
        memcpy(&i, m_buffer+m_offset, len);
        //i = *((int*)(m_buffer+m_offset));
        m_offset += len;
        return true;
    }

    bool getFloat(float& f)
    {
        unsigned int len = sizeof(f);
        if ((m_offset+len) > m_capacity)
            return false;
        memcpy(&f, m_buffer+m_offset, len);
        m_offset += len;
        return true;
    }

    bool getDouble(double& d)
    {
        unsigned int len = sizeof(d);
        if ((m_offset+len) > m_capacity)
            return false;
        memcpy(&d, m_buffer+m_offset, len);
        m_offset += len;
        return true;
    }

    bool getULong(unsigned long long& ull)
    {
        unsigned int len = sizeof(ull);
        if ((m_offset+len) > m_capacity)
            return false;
        memcpy(&ull, m_buffer+m_offset, len);
        m_offset += len;
        return true;
    }

    #ifdef x86_64
    bool getULong(uint64_t& ull)
    {
        unsigned int len = sizeof(ull);
        if ((m_offset+len) > m_capacity)
            return false;
        memcpy(&ull, m_buffer+m_offset, len);
        m_offset += len;
        return true;
    }
    #endif

    bool getLong(long long& ll)
    {
        unsigned int len = sizeof(ll);
        if ((m_offset+len) > m_capacity)
            return false;
        memcpy(&ll, m_buffer+m_offset, len);
        m_offset += len;
        return true;
    }

    template<unsigned int size>
    bool getMyArray(MyArray<size>& my_array)
    {
        unsigned int len = sizeof(unsigned int);
        if ((m_offset+len) > m_capacity)
            return false;
        unsigned int attr_len = 0;
        memcpy(&attr_len, m_buffer+m_offset, len);
        m_offset += len;

        len = attr_len;
        if ((m_offset+len) > m_capacity)
            return false;

        my_array.set(m_buffer+m_offset, len);
        m_offset += len;

        return true;
    }

    bool getFixBytes(char* buffer, const unsigned int size)
    {
        unsigned int len = sizeof(unsigned int);
        if ((m_offset+len) > m_capacity)
            return false;
        unsigned int attr_len = 0;
        memcpy(&attr_len, m_buffer+m_offset, len);
        m_offset += len;

        len = attr_len;
        if ((m_offset+len) > m_capacity || len != size)
            return false;
        memcpy(buffer, m_buffer+m_offset, len);
        m_offset += len;

        return true;
    }

    bool getMaxBytes(char* buffer, unsigned int& size)
    {
        unsigned int len = sizeof(unsigned int);
        if ((m_offset+len) > m_capacity)
            return false;
        unsigned int attr_len = 0;
        memcpy(&attr_len, m_buffer+m_offset, len);
        m_offset += len;

        len = attr_len;
        if ((m_offset+len) > m_capacity || len > size)
            return false;
        memcpy(buffer, m_buffer+m_offset, len);
        m_offset += len;
        size = len;

        return true;
    }

    bool getString(std::string& str)
    {
        unsigned int len = sizeof(unsigned int);
        if ((m_offset+len) > m_capacity)
            return false;
        unsigned int attr_len = 0;
        memcpy(&attr_len, m_buffer+m_offset, len);
        m_offset += len;

        len = attr_len;
        if ((m_offset+len) > m_capacity)
            return false;
        if (len == 0)
            str.erase();
        else
            str = std::string(m_buffer+m_offset, len);
        m_offset += len;
        return true;
    }

//private:
public:
    bool m_del_flag;
    char* m_buffer;
    unsigned int m_offset;
    unsigned int m_capacity;
};


class TransferTool
{
public:
    TransferTool():version_(0)
    ,seq_(0)
    ,length_(0)
    ,client_version_(0)
    ,compress_flag_(0)
    ,padding_len_(0)
    ,product_version_("")
    ,product_id_(0)
    ,partner_id_("")
    ,thunder_version_("")
    ,download_channel_("")
    ,cmd_id_(0)
    ,info_id_("")
    ,support_compress_(0)
    ,peer_id_("")
    ,result_(0)
    ,compress_(0)
    ,bt_file_("")
    ,outlen_(0)
    ,ifd_(0)
    ,aes_key_("")
    {

    };
    ~TransferTool(){};

public:
    std::string trans_struct_to_buf();
    int trans_buf_to_struct(char *buff_in, int len_in);

public:
    std::string str_to_hex(const char *s, int str_len);
    uint32_t get_encode_param_len();


public:
    uint32_t version_;
    uint32_t seq_;
    uint32_t length_;
    uint32_t client_version_;
    short compress_flag_;
    uint32_t padding_len_;
    std::string product_version_;
    uint32_t product_id_;
    std::string partner_id_;
    std::string thunder_version_;
    std::string download_channel_;
    short cmd_id_;
    std::string info_id_;
    int32_t support_compress_;
    std::string peer_id_;

    int32_t result_;
    int32_t compress_;
    std::string bt_file_;
    int32_t outlen_;
    int ifd_;
    std::string aes_key_;
};

}
#endif
