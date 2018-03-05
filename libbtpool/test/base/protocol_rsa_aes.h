/*
 * ================================================================================================================
 *    Description:  BT Pool RAS and AES Interface Head File
 *        Version:  1.0
 *        Created:  08/26/2016
 *         Author:  Melo
 * Modify History: 1. Created by Melo
 * ================================================================================================================
 */

#ifndef BTPOOL_P2SPROTOCOL_RSA_AES_H_
#define BTPOOL_P2SPROTOCOL_RSA_AES_H_

#include "common.h"

namespace BTPOOL
{

class AES
{
public:
    int setEncryptKey(const unsigned char *userKey, int bits = 128)
    {
        return AES_set_encrypt_key(userKey, bits, &aesKey_);
    }

    int setDecryptKey(const unsigned char *userKey, int bits = 128)
    {
        return AES_set_decrypt_key(userKey, bits, &aesKey_);
    }

    void encrypt(const unsigned char *in, unsigned char *out)
    {
        AES_encrypt(in, out, &aesKey_);
    }

    void decrypt(const unsigned char *in, unsigned char *out)
    {
        AES_decrypt(in, out, &aesKey_);
    }

    bool encrypt(const unsigned char *in, int inlen, unsigned char *out, int *outlen);
    bool decrypt(const unsigned char *in, int inlen, unsigned char *out, int *outlen);

private:
    AES_KEY aesKey_;
};



//encrypt_aes():实际上和AES一样，只是封装了一下，故此不用这个了
class ProtocolCrypto
{
public:
    ProtocolCrypto(uint32_t magic_num=0, uint32_t version=0,
        const unsigned char* aes_key=NULL, int aes_keylen=0,
        const unsigned char* rsa_key=NULL/* DER格式*/, int rsa_keylen=0);
    ProtocolCrypto(const ProtocolCrypto& other);
    ProtocolCrypto& operator=(const ProtocolCrypto& other);
    ~ProtocolCrypto();

    /********************************
    * 功能: 设置/获取协议包头的魔数
    * 输入: 4字节的魔数
    * 输出: 无
    * 注释: 用于标记协议包的开始
    *********************************/
    void set_magic_num(uint32_t magic_num);
    uint32_t get_magic_num() const;

    /********************************
    * 功能: 设置/获取RSA算法的版本号
    * 输入: 4字节的版本号
    * 输出: 无
    * 注释: 用于标记RSA算法的公私钥
    *********************************/
    void set_version(uint32_t version);
    uint32_t get_version() const;

    /********************************
    * 功能: 设置/获取AES算法的密钥
    * 输入: key是密钥的缓冲区
    *              len是密钥的长度
    * 输出: 无
    *********************************/
    void set_aes_key(const unsigned char* key, int len);
    std::string get_aes_key() const;

    /********************************
    * 功能: 设置/获取RSA算法的公钥，DER格式
    * 输入: key是公钥的缓冲区
    *              len是公钥的长度
    * 输出: 无
    *********************************/
    void set_rsa_public_key(const unsigned char* key, int len);
    std::string get_rsa_public_key() const;    
    /********************************
    * 功能: 设置/获取RSA算法的私钥，DER格式
    * 输入: key是私钥的缓冲区
    *              len是私钥的长度
    * 输出: 返回true为成功，false为失败
    *********************************/
    void set_rsa_private_key(const unsigned char* key, int len);
    std::string get_rsa_private_key() const;

    /********************************
    * 功能: 加密数据
    * 输入: in是输入数据的缓冲区
    *              inlen是输入数据的长度
    *              out是输出数据的缓冲区
    *              outlen是输出数据的长度
    * 输出: 返回true为成功，false为失败
    * 注释: 如果*out指向NULL，函数内会自动分配内存，
    *              此时调用者需要主动调用delete[]释放内存
    *********************************/
    bool encrypt(const unsigned char* in, int inlen, unsigned char*& out, int* outlen);

    /********************************
    * 功能: 解密数据
    * 输入: in是输入数据的缓冲区
    *              inlen是输入数据的长度
    *              out是输出数据的缓冲区
    *              outlen是输出数据的长度
    * 输出: 返回true为成功，false为失败
    * 注释: 如果*out指向NULL，函数内会自动分配内存
    *              此时调用者需要主动调用delete[]释放内存
    *              如果调用者只设置了公钥，就使用公钥加密
    *              如果调用者同时设置了公私钥，就使用私钥加密
    *********************************/
    bool decrypt(const unsigned char* in, int inlen, unsigned char*& out, int* outlen);

    /********************************
    * 功能: 使用RSA公钥加密数据
    * 输入: in是输入数据的缓冲区
    *              inlen是输入数据的长度
    *              out是输出数据的缓冲区
    *              outlen是输出数据的长度
    * 输出: 返回true为成功，false为失败
    * 注释: 如果*out指向NULL，函数内会自动分配内存
    *              此时调用者需要主动调用delete[]释放内存
    *              如果调用者只设置了公钥，就使用公钥解密
    *              如果调用者同时设置了公私钥，就使用私钥解密
    *********************************/
    bool encrypt_rsa_public(const unsigned char* in, int inlen, unsigned char*& out, int* outlen);

    /********************************
    * 功能: 使用RSA私钥解密数据
    * 输入: in是输入数据的缓冲区
    *              inlen是输入数据的长度
    *              out是输出数据的缓冲区
    *              outlen是输出数据的长度
    * 输出: 返回true为成功，false为失败
    * 注释: 如果*out指向NULL，函数内会自动分配内存，
    *              此时调用者需要主动调用delete[]释放内存
    *********************************/
    bool decrypt_rsa_private(const unsigned char* in, int inlen, unsigned char*& out, int* outlen);
    bool decrypt_rsa_private(const unsigned char* in, int inlen, unsigned char*& out, int* outlen, std::string& aes_key);

    /********************************
    * 功能: 使用AES加密数据
    * 输入: in是输入数据的缓冲区
    *              inlen是输入数据的长度
    *              out是输出数据的缓冲区
    *              outlen是输出数据的长度
    * 输出: 返回true为成功，false为失败
    * 注释: 如果*out指向NULL，函数内会自动分配内存，
    *              此时调用者需要主动调用delete[]释放内存
    *********************************/
    bool encrypt_aes(const unsigned char* in, int inlen, unsigned char*& out, int* outlen);

    /********************************
    * 功能: 使用AES解密数据
    * 输入: in是输入数据的缓冲区
    *              inlen是输入数据的长度
    *              out是输出数据的缓冲区
    *              outlen是输出数据的长度
    * 输出: 返回true为成功，false为失败
    * 注释: 如果*out指向NULL，函数内会自动分配内存，
    *              此时调用者需要主动调用delete[]释放内存
    *********************************/
    bool decrypt_aes(const unsigned char* in, int inlen, unsigned char*& out, int* outlen);
    
private:
    /* 协议包头的魔数*/
    uint32_t m_magic_num;
    
    /* RSA算法的版本号*/
    uint32_t m_version;
    
    /* AES算法的密钥*/
    unsigned char m_aes_key[AES_BLOCK_SIZE];
    
    /* RSA算法的结构对象*/
    RSA* m_rsa;            // 要么是用于公钥加密要么是用于私钥解密
    
    /* RSA算法的密钥长度*/
    int m_rsa_size;
    
    /* RSA算法的公钥，DER格式*/
    std::string m_rsa_public_key;
    
    /* RSA算法的私钥，DER格式*/
    std::string m_rsa_private_key;
};

}
#endif // SD_PROTOCOL_CRYPTO_H_20100709

