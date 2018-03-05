/*
 * ================================================================================================================
 *    Description:  BT Pool RAS and AES Interface Source File
 *        Version:  1.0
 *        Created:  08/26/2016
 *         Author:  Melo
 * Modify History: 1. Created by Melo
 * ================================================================================================================
 */

#include "protocol_rsa_aes.h"

namespace BTPOOL
{

bool AES::encrypt(const unsigned char *in, int inlen, unsigned char *out, int *outlen)
{
    if (!in || inlen < 0 || !out || !outlen)
    {
        return false;
    }

    *outlen = 0;

    while (inlen >= AES_BLOCK_SIZE)
    {
        encrypt(in, out);
        in += AES_BLOCK_SIZE;
        out += AES_BLOCK_SIZE;
        inlen -= AES_BLOCK_SIZE;
        *outlen += AES_BLOCK_SIZE;
    }

    if (inlen >= 0)
    {
        unsigned char tmp[AES_BLOCK_SIZE] = {0};
        memcpy(tmp, in, inlen);
        memset(tmp + inlen, AES_BLOCK_SIZE - inlen, AES_BLOCK_SIZE - inlen);
        encrypt(tmp, out);
        *outlen += AES_BLOCK_SIZE;
    }

    return true;
}

bool AES::decrypt(const unsigned char *in, int inlen, unsigned char *out, int *outlen)
{
    if (!in || inlen <= 0 || inlen % AES_BLOCK_SIZE != 0 || !out || !outlen)
    {
        return false;
    }

    *outlen = 0;

    while (inlen > 0)
    {
        decrypt(in, out);
        in += AES_BLOCK_SIZE;
        out += AES_BLOCK_SIZE;
        inlen -= AES_BLOCK_SIZE;
        *outlen += AES_BLOCK_SIZE;
    }

    --out;
    if (*out <= 0 || *out > AES_BLOCK_SIZE)
    {
        return false;
    }

    *outlen -= *out;

    return true;
}



ProtocolCrypto::ProtocolCrypto(
    uint32_t magic_num, uint32_t version,
    const unsigned char* aes_key, int aes_keylen,
    const unsigned char* rsa_key, int rsa_pkeylen)
{
    m_magic_num = magic_num;
    m_version = version;
    m_rsa = NULL;
    m_rsa_size = 0;
    memset(m_aes_key, '0', sizeof(m_aes_key));

    if (aes_key != NULL) {
        set_aes_key(aes_key, aes_keylen);
    }
    /* 先尝试rsa_key是不是公钥*/
    if (rsa_key != NULL) {
        set_rsa_public_key(rsa_key, rsa_pkeylen);
    }
    /* 如果不是，再尝试rsa_key是不是私钥*/
    if (rsa_key != NULL && m_rsa == NULL) {
        set_rsa_private_key(rsa_key, rsa_pkeylen);
    }
}

ProtocolCrypto::ProtocolCrypto(const ProtocolCrypto& other)
{
    m_magic_num = other.m_magic_num;
    m_version = other.m_version;
    m_rsa = NULL;
    m_rsa_size = 0;
    memset(m_aes_key, '0', sizeof(m_aes_key));

    if (other.m_rsa != NULL) {
        std::string aeskey = other.get_aes_key();
        if (!aeskey.empty()) {
            set_aes_key((const unsigned char*)aeskey.c_str(), aeskey.length());
        }

        std::string pubkey = other.get_rsa_public_key();
        if (!pubkey.empty()) {
            set_rsa_public_key((const unsigned char*)pubkey.c_str(), pubkey.length());
        }

        std::string prikey = other.get_rsa_private_key();
        if (!prikey.empty()) {
            set_rsa_private_key((const unsigned char*)prikey.c_str(), prikey.length());
        }
    }
}

ProtocolCrypto& ProtocolCrypto::operator=(const ProtocolCrypto& other)
{
    /* 自己不能给自己赋值*/
    if (this != &other) {
        if (m_rsa != NULL) {
            RSA_free(m_rsa);
        }

        m_magic_num = other.m_magic_num;
        m_version = other.m_version;
        m_rsa = NULL;
        m_rsa_size = 0;
        memset(m_aes_key, '0', sizeof(m_aes_key));
        m_rsa_public_key.erase();
        m_rsa_private_key.erase();

        if (other.m_rsa != NULL) {
            std::string aeskey = other.get_aes_key();
            if (!aeskey.empty()) {
                set_aes_key((const unsigned char*)aeskey.c_str(), aeskey.length());
            }

            std::string pubkey = other.get_rsa_public_key();
            if (!pubkey.empty()) {
                set_rsa_public_key((const unsigned char*)pubkey.c_str(), pubkey.length());
            }

            std::string prikey = other.get_rsa_private_key();
            if (!prikey.empty()) {
                set_rsa_private_key((const unsigned char*)prikey.c_str(), prikey.length());
            }
        }
    }

    return *this;
}

ProtocolCrypto::~ProtocolCrypto()
{
    if (m_rsa != NULL) {
        RSA_free(m_rsa);
    }
}

void ProtocolCrypto::set_magic_num(uint32_t magic_num)
{
    m_magic_num = magic_num;
}

uint32_t ProtocolCrypto::get_magic_num() const
{
    return m_magic_num;
}

void ProtocolCrypto::set_version(uint32_t version)
{
    m_version = version;
}

uint32_t ProtocolCrypto::get_version() const
{
    return m_version;
}

void ProtocolCrypto::set_aes_key(const unsigned char* key, int len)
{
    assert(key != NULL && len == sizeof(m_aes_key)); 

    memcpy(m_aes_key, key, len);
}

std::string ProtocolCrypto::get_aes_key() const
{
    return std::string((const char*)m_aes_key, sizeof(m_aes_key));
}

void ProtocolCrypto::set_rsa_public_key(const unsigned char* key, int len)
{
    assert(key != NULL && len > 0);

    if (m_rsa != NULL) {
        RSA_free(m_rsa);
    }

    m_rsa_public_key = std::string((const char*)key, len);
    m_rsa = d2i_RSAPublicKey(NULL, &key, len);
    if (m_rsa != NULL) {
        m_rsa_size = RSA_size(m_rsa);
    } else {
        m_rsa_public_key.erase();
    }
}

std::string ProtocolCrypto::get_rsa_public_key() const
{
    return m_rsa_public_key;
}

void ProtocolCrypto::set_rsa_private_key(const unsigned char* key, int len)
{
    assert(key != NULL && len > 0);

    if (m_rsa != NULL) {
        RSA_free(m_rsa);
    }

    m_rsa_private_key = std::string((const char*)key, len);
    m_rsa = d2i_RSAPrivateKey(NULL, &key, len);
    if (m_rsa != NULL) {
        m_rsa_size = RSA_size(m_rsa);
    } else {
        m_rsa_private_key.erase();
    }
}

std::string ProtocolCrypto::get_rsa_private_key() const
{
    return m_rsa_private_key;
}

bool ProtocolCrypto::encrypt(const unsigned char* in, int inlen, unsigned char*& out, int* outlen)
{
    assert(in != NULL && inlen > 0);
    assert(m_rsa != NULL);

    /* 根据是否设置了RSA私钥来决定调用哪种加密方式*/
    if (m_rsa->d != NULL) {
        return encrypt_aes(in, inlen, out, outlen);
    } else {
        return encrypt_rsa_public(in, inlen, out, outlen);
    }
}

bool ProtocolCrypto::decrypt(const unsigned char* in, int inlen, unsigned char*& out, int* outlen)
{
    assert(in != NULL && inlen > 0);
    assert(m_rsa != NULL);

    /* 根据是否设置了RSA私钥来决定调用哪种解密方式*/
    if (m_rsa->d != NULL) {
        return decrypt_rsa_private(in, inlen, out, outlen);
    } else {
        return decrypt_aes(in, inlen, out, outlen);
    }
}

bool ProtocolCrypto::encrypt_rsa_public(const unsigned char* in, int inlen, unsigned char*& out, int* outlen)
{
    assert(in != NULL && inlen > 0);
    assert(m_magic_num != 0 && m_version != 0);
    assert(m_rsa != NULL);

    /* 格式: 魔数+ 版本+ RSA(aes_key)+ AES(in, inlen, aes_key) */

    int len = sizeof(m_magic_num) + sizeof(m_version) + sizeof(uint32_t) + m_rsa_size;
    if (out == NULL) {
        /* 调用者需要调用delete[]来释放内存*/
        *outlen = len + sizeof(uint32_t) + inlen + sizeof(m_aes_key);
        out = new unsigned char[*outlen];
    }

    /* AES(in, inlen, aes_key) */    
    *outlen -= len - sizeof(uint32_t);
    AES aes;
    aes.setEncryptKey(m_aes_key);
    if (aes.encrypt(in, inlen, (out + len + sizeof(uint32_t)), outlen) == false) {
        return false;
    }

    /* RSA(aes_key) */
    if (RSA_public_encrypt(sizeof(m_aes_key), m_aes_key, (out + len - m_rsa_size), m_rsa, RSA_PKCS1_PADDING) != m_rsa_size) {
        return false;
    }

    /* 填充其它字段*/
    uint32_t* ptr = (uint32_t*)out;
    *ptr++ = m_magic_num;
    *ptr++ = m_version;
    *ptr++ = m_rsa_size;
    ptr = (uint32_t*)((char*)ptr + m_rsa_size);
    *ptr++ = *outlen;

    *outlen = len + sizeof(uint32_t) + *outlen;

    return true;
}

bool ProtocolCrypto::decrypt_rsa_private(const unsigned char* in, int inlen, unsigned char*& out, int* outlen)
{
    assert(in != NULL && inlen > 0);
    assert(m_rsa != NULL);

    /* 格式: 魔数+ 版本+ RSA(aes_key)+ AES(in, inlen, aes_key) */

    /* 检查合法性*/
    int len = sizeof(m_magic_num) + sizeof(m_version) + sizeof(uint32_t) + m_rsa_size;
    if (inlen <= int(len + sizeof(uint32_t))) {
        return false;
    }
    
    uint32_t* ptr = (uint32_t*)in;
    if (*ptr++ != m_magic_num) {
        return false;
    }
    if (*ptr++ != m_version) {
        return false;
    }
    if (*ptr++ != (uint32_t)m_rsa_size) {
        return false;
    }    

    if (out == NULL) {
        /* 调用者需要调用delete[]来释放内存*/
        *outlen = inlen - len - sizeof(uint32_t);
        out = new unsigned char[*outlen];
    }

    /* RSA(aes_key) */
    unsigned char aes_key[m_rsa_size];
    if (RSA_private_decrypt(m_rsa_size, (in + len - m_rsa_size), aes_key, m_rsa, RSA_PKCS1_PADDING) != AES_BLOCK_SIZE) {
        return false;
    }
    memcpy(m_aes_key, aes_key, AES_BLOCK_SIZE);

    /* AES(in, inlen, aes_key) */
    AES aes;
    aes.setDecryptKey(m_aes_key);
    if (aes.decrypt((in + len + sizeof(uint32_t)), (inlen - len - sizeof(uint32_t)), out, outlen) == false) {
        return false;
    }

    return true;
}

bool ProtocolCrypto::decrypt_rsa_private(const unsigned char* in, int inlen, unsigned char*& out, int* outlen, std::string& aes_key)
{
    assert(in != NULL && inlen > 0);
    assert(m_rsa != NULL);

    /* 格式: 魔数+ 版本+ RSA(aes_key)+ AES(in, inlen, aes_key) */

    /* 检查合法性*/
    int len = sizeof(m_magic_num) + sizeof(m_version) + sizeof(uint32_t) + m_rsa_size;
    if (inlen <= int(len + sizeof(uint32_t))) {
        return false;
    }
    
    uint32_t* ptr = (uint32_t*)in;
    if (*ptr++ != m_magic_num) {
        return false;
    }
    if (*ptr++ != m_version) {
        return false;
    }
    if (*ptr++ != (uint32_t)m_rsa_size) {
        return false;
    }    

    if (out == NULL) {
        /* 调用者需要调用delete[]来释放内存*/
        *outlen = inlen - len - sizeof(uint32_t);
        out = new unsigned char[*outlen];
    }

    /* RSA(aes_key) */
    unsigned char aes_key_buf[m_rsa_size];
    if (RSA_private_decrypt(m_rsa_size, (in + len - m_rsa_size), aes_key_buf, m_rsa, RSA_PKCS1_PADDING) != AES_BLOCK_SIZE) {
        return false;
    }
    //memcpy(m_aes_key, aes_key_buf, AES_BLOCK_SIZE);
    aes_key.assign((const char*)aes_key_buf, AES_BLOCK_SIZE);

    /* AES(in, inlen, aes_key) */
    AES aes;
    aes.setDecryptKey(aes_key_buf);
    if (aes.decrypt((in + len + sizeof(uint32_t)), (inlen - len - sizeof(uint32_t)), out, outlen) == false) {
        return false;
    }

    return true;
}

bool ProtocolCrypto::encrypt_aes(const unsigned char* in, int inlen, unsigned char*& out, int* outlen)
{
    assert(in != NULL && inlen > 0);
    //assert(m_rsa != NULL);

    if (out == NULL) {
        /* 调用者需要调用delete[]来释放内存*/
        *outlen = sizeof(uint32_t) + inlen + sizeof(m_aes_key);
        out = new unsigned char[*outlen];    
    }

    /* AES(in, inlen, aes_key) */    
    AES aes;
    aes.setEncryptKey(m_aes_key);
    *outlen -= sizeof(uint32_t);
    if (aes.encrypt(in, inlen, (out + sizeof(uint32_t)), outlen) == false) {
        return false;
    }

    *((uint32_t*)out) = *outlen;
    *outlen += sizeof(uint32_t);

    return true;
}

bool ProtocolCrypto::decrypt_aes(const unsigned char* in, int inlen, unsigned char*& out, int* outlen)
{
    assert(in != NULL && inlen > 0);
    assert(m_rsa != NULL);

    if (out == NULL) {
        /* 调用者需要调用delete[]来释放内存*/
        *outlen = inlen;
        out = new unsigned char[*outlen];    
    }

    /* AES(in, inlen, aes_key) */
    AES aes;
    aes.setDecryptKey(m_aes_key);
    if (aes.decrypt(in, inlen, out, outlen) == false) {
        return false;
    }

    return true;
}

}
