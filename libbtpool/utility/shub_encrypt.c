#include <openssl/rsa.h>
#include <openssl/x509.h>

#include "utility/define.h"
#include "shub_encrypt.h"
#include "string.h"
#include "utility/md5.h"
#include "utility/aes.h"
#include "utility/errcode.h"
#include "utility/bytebuffer.h"
#include "utility/sd_fs.h"
#include "utility/utility.h"
#include "utility/sd_string.h"
#include "utility/log_wrapper.h"
LOGID_MODEULE("shub_encrypt")

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <iostream>


int32_t gen_aes_key_by_user_data(const void *user_data, uint8_t *p_aeskey)
{
    uintptr_t p_value = (uintptr_t)user_data;
    ctx_md5 md5;
    uint8_t aeskey[16];
#ifdef _DEBUG
    char aeskey_str[40] = {0};
#endif
    md5_initialize(&md5);
    md5_update_dl(&md5, (const unsigned char*)&p_value, sizeof(uint32_t));
    md5_finish_dl(&md5, aeskey);

    sd_memcpy(p_aeskey, aeskey, 16);
#ifdef _DEBUG
    str2hex((char*)aeskey, 16, aeskey_str, 40);
    LOG_DEBUG("gen_aes_key_by_user_data, last_cmd user_data=0x%X, aeskey=%s", p_value, aeskey_str);
#endif
    return SUCCESS;
}

uint8_t pub_key_10000[PUB_KEY_LEN + 1] =
{
    0x30, 0x81, 0x89, 0x02, 0x81, 0x81, 0x00, 0xBC, 0x72, 0x02, 0x82, 0xF1, 0xCA, 0x7F, 0xD3, 0x15,
    0xA7, 0x9B, 0x30, 0x32, 0x1F, 0x65, 0x4D, 0x69, 0x2B, 0xEC, 0x63, 0x0B, 0x05, 0xF9, 0xE9, 0xDF,
    0xC1, 0xB9, 0x8A, 0x57, 0x6B, 0xF5, 0x30, 0xB0, 0x02, 0xB7, 0x63, 0x9B, 0x79, 0x6B, 0x82, 0x22,
    0x4B, 0x89, 0x31, 0xF1, 0x6A, 0x35, 0x9E, 0xEF, 0x92, 0x3F, 0x77, 0x6D, 0x4F, 0xAC, 0x4A, 0x16,
    0x91, 0xC3, 0xC2, 0x90, 0x2E, 0x49, 0xF5, 0x46, 0x08, 0x9F, 0x47, 0x11, 0xCA, 0x83, 0x27, 0x2B,
    0x6F, 0xE9, 0x47, 0xD8, 0x34, 0xB0, 0x67, 0x53, 0x7E, 0x65, 0x18, 0xCF, 0xC0, 0x5E, 0x8D, 0x76,
    0x36, 0xEB, 0x46, 0xAE, 0x18, 0x05, 0xAE, 0xD5, 0x8A, 0x1F, 0xD7, 0xE3, 0x3B, 0x9E, 0x19, 0x45,
    0xBA, 0x23, 0xAF, 0x0A, 0x67, 0x48, 0x36, 0xF0, 0xC4, 0xFD, 0x8F, 0xA9, 0xAB, 0x19, 0x29, 0xBB,
    0x61, 0x91, 0x00, 0xC4, 0x41, 0x74, 0x6F, 0x02, 0x03, 0x01, 0x00, 0x01
};
uint8_t pub_key_40000[PUB_KEY_LEN + 1] =
{
    0x30, 0x81, 0x89, 0x02, 0x81, 0x81, 0x00, 0xD6, 0x50, 0x9C, 0x20, 0x23, 0x1B, 0x33, 0x26, 0x20,
    0x23, 0x24, 0x7E, 0x98, 0xE0, 0x89, 0xFB, 0xBE, 0x0A, 0x4A, 0x88, 0xAA, 0x17, 0xDB, 0x19, 0x8F,
    0x1F, 0x49, 0xE0, 0x3E, 0x05, 0x3F, 0xD0, 0x63, 0x81, 0xCF, 0x66, 0xC3, 0xA1, 0x0A, 0x69, 0xE6,
    0x49, 0xD7, 0xB8, 0x87, 0x38, 0xA1, 0x56, 0x6E, 0x7B, 0x8C, 0x75, 0x25, 0xE1, 0xAC, 0xD9, 0x6D,
    0xDE, 0x05, 0x5A, 0x8C, 0x73, 0x16, 0x61, 0x66, 0x0F, 0x69, 0xE6, 0xB8, 0xDD, 0xAE, 0xF6, 0x36,
    0x23, 0x42, 0xBF, 0xC6, 0x58, 0x31, 0x6E, 0xB2, 0x3E, 0x3F, 0x71, 0x38, 0xB3, 0xEB, 0xE9, 0xDC,
    0x9B, 0x7D, 0x13, 0x05, 0xDC, 0x25, 0xAE, 0xBF, 0x68, 0x89, 0xE0, 0x67, 0xDF, 0x6D, 0x29, 0xDE,
    0xF3, 0x90, 0xA1, 0x02, 0x05, 0xCD, 0x0B, 0xAC, 0x1A, 0xE1, 0xDE, 0xFE, 0x49, 0x4D, 0x93, 0x49,
    0xCC, 0x3B, 0xFA, 0xD5, 0xC0, 0x25, 0xFF, 0x02, 0x03, 0x01, 0x00, 0x01
};
uint8_t pub_key_60000[PUB_KEY_LEN + 1] =
{
    0x30, 0x81, 0x89, 0x02, 0x81, 0x81, 0x00, 0x96, 0xA9, 0x2D, 0x21, 0xF2, 0x21, 0x7E, 0xD1, 0x30,
    0xE0, 0x09, 0x93, 0xA0, 0x44, 0x78, 0x3E, 0x02, 0xF2, 0xC7, 0x05, 0x36, 0x31, 0xC5, 0x45, 0xBE,
    0xA3, 0x50, 0x5D, 0xFA, 0x4E, 0x95, 0x74, 0x27, 0x48, 0xAB, 0x03, 0x88, 0x29, 0x62, 0x44, 0x3B,
    0xE2, 0x08, 0x58, 0xBA, 0x97, 0x71, 0x7B, 0x04, 0x06, 0x17, 0x6B, 0x0D, 0x6F, 0x25, 0xB5, 0x2A,
    0x7D, 0x60, 0x64, 0x11, 0x63, 0x51, 0x1B, 0xE7, 0x00, 0x96, 0xD2, 0x82, 0x1C, 0x0F, 0x99, 0x30,
    0x62, 0x68, 0xA9, 0x0A, 0x66, 0xBC, 0x37, 0x2D, 0xF1, 0x91, 0xBE, 0x5A, 0x25, 0x0E, 0x76, 0xDC,
    0x43, 0xD2, 0xF3, 0x2F, 0x5C, 0xB5, 0x68, 0xCA, 0xB0, 0xFF, 0x0C, 0x1F, 0x20, 0x13, 0xD8, 0x83,
    0x6F, 0xB3, 0x10, 0x7D, 0x63, 0x1D, 0x4A, 0x17, 0x06, 0x66, 0x48, 0x3A, 0xEE, 0x28, 0xB8, 0x20,
    0x91, 0xE0, 0x20, 0xD0, 0x4C, 0xB5, 0x1B, 0x02, 0x03, 0x01, 0x00, 0x01

};
uint8_t pub_key_70000[PUB_KEY_LEN + 1] =
{
    0x30, 0x81, 0x89, 0x02, 0x81, 0x81, 0x00, 0xBF, 0xDB, 0xF7, 0x12, 0xC0, 0xED, 0x62, 0xBD, 0x3F,
    0xC6, 0xE4, 0x35, 0xF9, 0x85, 0x13, 0xEE, 0xC9, 0xCC, 0xCF, 0xF1, 0xC4, 0x5E, 0xB2, 0xA2, 0x42,
    0xCE, 0x8F, 0xD4, 0x73, 0x9C, 0x6D, 0xAE, 0x5B, 0x87, 0x42, 0xA6, 0xC5, 0xE7, 0xF6, 0xEB, 0x8F,
    0x3E, 0x4E, 0x67, 0x89, 0x27, 0x4A, 0x1C, 0x35, 0x3C, 0xAF, 0xFE, 0x95, 0x17, 0x67, 0xE9, 0x03,
    0x87, 0xC8, 0xAB, 0xC0, 0x89, 0xBB, 0x73, 0x3E, 0xF0, 0x0E, 0x4F, 0x52, 0xE9, 0xE5, 0x04, 0x0C,
    0x5D, 0x55, 0x73, 0x8C, 0x14, 0x75, 0x99, 0x88, 0x50, 0x0B, 0xA3, 0xA4, 0xA6, 0xC8, 0x7B, 0x7A,
    0xD4, 0xBD, 0x6F, 0xD8, 0x7C, 0x53, 0x78, 0x36, 0xD7, 0xF5, 0xDA, 0x42, 0xBF, 0xD7, 0x38, 0xB2,
    0x8D, 0x7D, 0x93, 0xB0, 0xB9, 0x49, 0x19, 0x5B, 0x7B, 0xA0, 0xDE, 0xEE, 0xDD, 0x88, 0xC0, 0x42,
    0x48, 0xE1, 0xEB, 0x35, 0x6E, 0xC8, 0x49, 0x02, 0x03, 0x01, 0x00, 0x01

};
uint8_t pub_key_80000[PUB_KEY_LEN + 1] =
{
    0x30, 0x81, 0x89, 0x02, 0x81, 0x81, 0x00, 0xD8, 0x29, 0xB5, 0xA4, 0x2D, 0x8D, 0x03, 0x77, 0x46,
    0x15, 0xE9, 0xC6, 0x78, 0xD2, 0x93, 0x66, 0xFA, 0xD7, 0x16, 0x39, 0xA2, 0xEE, 0x62, 0x7C, 0x03,
    0xC9, 0x61, 0x38, 0x72, 0x11, 0xB8, 0x10, 0x59, 0xBE, 0xDB, 0xCD, 0x16, 0x1E, 0xF9, 0x58, 0xD1,
    0x43, 0x07, 0xB7, 0x85, 0x4E, 0x6F, 0xEE, 0xD1, 0xFF, 0x06, 0xD1, 0xDA, 0xA3, 0x19, 0xB9, 0x51,
    0x23, 0x97, 0x8C, 0x13, 0xE1, 0xDA, 0xA7, 0x4C, 0x0A, 0x1A, 0x71, 0xC0, 0x65, 0x9B, 0x3B, 0x67,
    0x9B, 0xE5, 0x12, 0xDC, 0xA7, 0x6C, 0x64, 0xF2, 0xA0, 0x24, 0x0B, 0x0F, 0xEB, 0x2A, 0x30, 0x53,
    0xEF, 0x5A, 0xC7, 0xAF, 0x09, 0xB7, 0x27, 0x86, 0xAB, 0x60, 0x93, 0xB2, 0xBB, 0x52, 0x8B, 0xB7,
    0x1B, 0x8E, 0x5D, 0x53, 0x85, 0x14, 0xC6, 0xE9, 0x34, 0xDE, 0xB0, 0xC5, 0x99, 0x43, 0xB1, 0xFA,
    0xA0, 0xA3, 0x8D, 0x63, 0x65, 0x82, 0xE7, 0x02, 0x03, 0x01, 0x00, 0x01
};

uint8_t pub_key_50000[PUB_KEY_LEN + 1] =
{
    0x30, 0x81, 0x89, 0x02, 0x81, 0x81, 0x00, 0xC9, 0xB0, 0xDE, 0xBD, 0x3B, 0x40, 0x51, 0x4B, 0xA9,
    0x8F, 0xD5, 0x41, 0xED, 0xEF, 0x9D, 0xAA, 0x18, 0x1B, 0x4F, 0x4C, 0x9B, 0x5C, 0x6A, 0xA9, 0x9A,
    0xDD, 0xB9, 0x31, 0x92, 0x0E, 0xCA, 0x26, 0x48, 0x3E, 0x29, 0x19, 0xDA, 0x90, 0x46, 0xAE, 0xA8,
    0x44, 0x1E, 0x2A, 0x4D, 0xBF, 0xA4, 0x9B, 0x27, 0xA4, 0x60, 0x5C, 0xC7, 0xFE, 0xA5, 0x06, 0x02,
    0x4F, 0xF8, 0x07, 0x03, 0xCC, 0x7A, 0x52, 0x49, 0x21, 0x55, 0xA5, 0x57, 0xB2, 0xC8, 0x2B, 0x22,
    0x74, 0x22, 0x70, 0x9E, 0x64, 0x0D, 0x2F, 0xF0, 0x0F, 0xF0, 0x88, 0xD0, 0x9C, 0x26, 0xB6, 0x0D,
    0x6D, 0xB1, 0xFC, 0x9E, 0x40, 0x17, 0x1E, 0xD2, 0x4D, 0x87, 0x6E, 0x2D, 0x2D, 0x71, 0x5F, 0x06,
    0xFA, 0xBE, 0x09, 0x13, 0x00, 0x77, 0xAD, 0xA3, 0xF8, 0xEB, 0x9C, 0x0D, 0xDD, 0x55, 0xC0, 0xAD,
    0xF2, 0x7B, 0x3C, 0x48, 0x9B, 0x57, 0x2B, 0x02, 0x03, 0x01, 0x00, 0x01
};

#define _GET_PUBKEY(v) pub_key_##v
#define GET_PUBKEY(v) _GET_PUBKEY(v)

int32_t res_query_rsa_pub_encrypt(int32_t len, uint8_t *text, char *encrypted, uint32_t *p_enc_len, int32_t key_ver)
{
    RSA *rsa = NULL;
    uint32_t rsa_size = 0;
    int32_t ret_val = SUCCESS;
    const uint8_t *key = NULL;
#ifdef _DEBUG
    uint8_t encrypted_aes_hex[1024] = {0};
    uint8_t text_str[128] = {0};
#endif //_DEBUG

    switch(key_ver)
    {
    case 10000:
        key = GET_PUBKEY(10000);//goall pub_key_10000
        break;
    case 40000:
        key = GET_PUBKEY(40000);
        break;
    case 50000:
        key = GET_PUBKEY(50000);
        break;
    case 60000:
        key = GET_PUBKEY(60000);
        break;
    case 70000:
        key = GET_PUBKEY(70000);
        break;
    case 80000:
        key = GET_PUBKEY(80000);
        break;
    default:
        LOG_ERROR("xxxxx invalid rsa pub key version: %d", key_ver);
        assert(FALSE);
        return -1;
    }

    if(text == NULL || encrypted == NULL || p_enc_len == NULL)
    {
        return -1;
    }
    rsa = d2i_RSAPublicKey(NULL, (const unsigned char **)&key, PUB_KEY_LEN);
    if (NULL == rsa)
        return -2;
    rsa_size = RSA_size(rsa);
    if (rsa_size <= 0)
    {
        RSA_free(rsa);
        rsa = NULL;
        return -3;
    }
    if(len > rsa_size)
    {
        RSA_free(rsa);
        rsa = NULL;
        return -4;
    }
    ret_val =  RSA_public_encrypt(len, text, (uint8_t*)encrypted, rsa, 1);
    *p_enc_len = ret_val;
#ifdef _DEBUG
    str2hex(encrypted, ret_val, (char*)encrypted_aes_hex, 1024);
    str2hex((char*)text, len, (char*)text_str, 128);
    LOG_DEBUG("res_query_rsa_pub_encrypt, key_ver=%d, encryted len=%d, text=%s, encrypted_aes=%s",
              key_ver, ret_val, text_str, encrypted_aes_hex);
#endif //_DEBUG
    RSA_free(rsa);
    return SUCCESS;
}


int32_t build_rsa_encrypt_header(char **ppbuf, int32_t *p_buflen, int32_t pubkey_ver,
                                uint8_t *aes_key, int32_t data_len)
{
    int32_t ret = SUCCESS;
    char encrypted_aes[RES_QUERY_AES_CIPHER_LEN] = {0};
    uint32_t encrypted_aes_len = 0;

    if (0 != res_query_rsa_pub_encrypt(16, aes_key, encrypted_aes, &encrypted_aes_len, pubkey_ver))
    {
        LOG_ERROR("build_rsa_encrypt_header, failed encrypt aes key by RSA");
        return -1;
    }
    LOG_DEBUG("build_rsa_encrypt_header, encrypted aes len=%d", encrypted_aes_len);
    if(encrypted_aes_len != RES_QUERY_AES_CIPHER_LEN)
    {
        LOG_ERROR("build_rsa_encrypt_header, aes cipher len: required len=%u, real len = %u",
                  RES_QUERY_AES_CIPHER_LEN, encrypted_aes_len);
        sd_assert(FALSE);
        return -1;
    }

    ret = sd_set_int32_to_lt(ppbuf, p_buflen, (int32_t)RSA_ENCRYPT_MAGIC_NUM);
    CHECK_VALUE(ret);
    ret = sd_set_int32_to_lt(ppbuf, p_buflen, pubkey_ver);
    CHECK_VALUE(ret);
    ret = sd_set_int32_to_lt(ppbuf, p_buflen, (int32_t)RES_QUERY_AES_CIPHER_LEN);
    CHECK_VALUE(ret);
    ret = sd_set_bytes(ppbuf, p_buflen, encrypted_aes, (int32_t)RES_QUERY_AES_CIPHER_LEN);
    CHECK_VALUE(ret);
    ret = sd_set_int32_to_lt(ppbuf, p_buflen, data_len);
    CHECK_VALUE(ret);

    return ret;
}


int32_t xl_aes_encrypt(char* buffer,uint32_t* len)
{
    int32_t ret;
    char *pOutBuff;
    char *tmp_buffer = NULL;
    int32_t tmplen = *len;
    int32_t nOutLen;
    int32_t nBeginOffset;
    uint8_t szKey[16];
    ctx_md5 md5;
    ctx_aes aes;
    int nInOffset;
    int nOutOffset;
    unsigned char inBuff[ENCRYPT_BLOCK_SIZE],ouBuff[ENCRYPT_BLOCK_SIZE];
    if (buffer == NULL)
    {
        return -1;
    }
    pOutBuff = (char*)malloc(*len + 16);

    nOutLen = 0;
    nBeginOffset = sizeof(uint32_t)*3;
    md5_initialize(&md5);
    md5_update_dl(&md5, (const unsigned char*)buffer, sizeof(uint32_t) * 2);
    md5_finish_dl(&md5, szKey);
    /*aes encrypt*/
    aes_init(&aes, 16, szKey);
    nInOffset = nBeginOffset;
    nOutOffset = 0;
    sd_memset(inBuff,0,ENCRYPT_BLOCK_SIZE);
    sd_memset(ouBuff,0,ENCRYPT_BLOCK_SIZE);
    while(TRUE)
    {
        if (*len - nInOffset >= ENCRYPT_BLOCK_SIZE)
        {
            sd_memcpy(inBuff,buffer+nInOffset,ENCRYPT_BLOCK_SIZE);
            aes_cipher(&aes, inBuff, ouBuff);
            sd_memcpy(pOutBuff+nOutOffset,ouBuff,ENCRYPT_BLOCK_SIZE);
            nInOffset += ENCRYPT_BLOCK_SIZE;
            nOutOffset += ENCRYPT_BLOCK_SIZE;
        }
        else
        {
            int nDataLen = *len - nInOffset;
            int nFillData = ENCRYPT_BLOCK_SIZE - nDataLen;
            sd_memset(inBuff,nFillData,ENCRYPT_BLOCK_SIZE);
            sd_memset(ouBuff,0,ENCRYPT_BLOCK_SIZE);
            if (nDataLen > 0)
            {
                sd_memcpy(inBuff,buffer+nInOffset,nDataLen);
                aes_cipher(&aes, inBuff, ouBuff);
                sd_memcpy(pOutBuff+nOutOffset,ouBuff,ENCRYPT_BLOCK_SIZE);
                nInOffset += nDataLen;
                nOutOffset += ENCRYPT_BLOCK_SIZE;
            }
            else
            {
                aes_cipher(&aes, inBuff, ouBuff);
                sd_memcpy(pOutBuff+nOutOffset,ouBuff,ENCRYPT_BLOCK_SIZE);
                nOutOffset += ENCRYPT_BLOCK_SIZE;
            }
            break;
        }
    }
    nOutLen = nOutOffset;
    sd_memcpy(buffer + nBeginOffset,pOutBuff, nOutLen);
    tmp_buffer = buffer + sizeof(uint32_t) * 2;
    sd_set_int32_to_lt(&tmp_buffer, &tmplen, nOutLen);
    free(pOutBuff);
    pOutBuff = NULL;
    if(nOutLen + nBeginOffset > *len + 16)
        return -1;
    *len = nOutLen + nBeginOffset;
    return SUCCESS;
}
int32_t xl_aes_decrypt(char* pDataBuff, uint32_t* nBuffLen)
{
    int32_t ret;
    int nBeginOffset;
    char *pOutBuff;
    int  nOutLen;
    unsigned char szKey[16];
    ctx_md5 md5;
    ctx_aes aes;
    int nInOffset;
    int nOutOffset;
    unsigned char inBuff[ENCRYPT_BLOCK_SIZE],ouBuff[ENCRYPT_BLOCK_SIZE];
    char * out_ptr;
    if (pDataBuff == NULL)
    {
        return FALSE;
    }
    nBeginOffset = sizeof(uint32_t)*3;
    if ((*nBuffLen-nBeginOffset)%ENCRYPT_BLOCK_SIZE != 0)
    {
        return FALSE;
    }
    pOutBuff = (char*)malloc(*nBuffLen + 16);

    nOutLen = 0;
    md5_initialize(&md5);
    md5_update_dl(&md5, (unsigned char*)pDataBuff,sizeof(uint32_t)*2);
    md5_finish_dl(&md5, szKey);

    aes_init(&aes,16,(unsigned char*)szKey);
    nInOffset = nBeginOffset;
    nOutOffset = 0;
    sd_memset(inBuff,0,ENCRYPT_BLOCK_SIZE);
    sd_memset(ouBuff,0,ENCRYPT_BLOCK_SIZE);
    while(*nBuffLen - nInOffset > 0)
    {
        sd_memcpy(inBuff,pDataBuff+nInOffset,ENCRYPT_BLOCK_SIZE);
        aes_invcipher(&aes, inBuff,ouBuff);
        sd_memcpy(pOutBuff+nOutOffset,ouBuff,ENCRYPT_BLOCK_SIZE);
        nInOffset += ENCRYPT_BLOCK_SIZE;
        nOutOffset += ENCRYPT_BLOCK_SIZE;
    }
    nOutLen = nOutOffset;
    sd_memcpy(pDataBuff + nBeginOffset,pOutBuff,nOutLen);
    out_ptr = pOutBuff + nOutLen - 1;
    if (*out_ptr <= 0 || *out_ptr > ENCRYPT_BLOCK_SIZE)
    {
        ret = -1;
    }
    else
    {
        if(nBeginOffset + nOutLen - *out_ptr < *nBuffLen)
        {
            *nBuffLen = nBeginOffset + nOutLen - *out_ptr;
            ret = SUCCESS;
        }
        else
        {
            ret = -1;
        }
    }
    free(pOutBuff);
    pOutBuff = NULL;
    return ret;
}

const int32_t kAesKeyLen = 16;
char szAesKey[kAesKeyLen] =
{
    '\xE6','\x9A','\xB4','\xE9',
    '\x9B','\xAA','\xE5','\x98',
    '\x89','\xE5','\xB9','\xB4',
    '\xE5','\x8D','\x8E','\x81'
};


#ifdef _DEBUG
#define TMPINT2HEX(ch)        ((ch) < 10 ? (ch) + '0' : (ch) - 10 + 'A')
int tmp_str2hex(const char *in_buf, int in_bufsize, char *out_buf, int out_bufsize)
{
    int ret_val = 0;
    int idx = 0;
    uint8_t *inp = (uint8_t*)in_buf, ch;

    for(idx = 0; idx < in_bufsize; idx++)
    {
        if(idx * 2 >= out_bufsize)
            break;

        ch = inp[idx] >> 4;
        out_buf[idx * 2] = TMPINT2HEX(ch);
        ch = inp[idx] & 0x0F;
        out_buf[idx * 2 + 1] = TMPINT2HEX(ch);
    }
    return ret_val;
}
#endif

int32_t simple_aes_encrypt(char * buf , int32_t buf_len )
{
    if(buf_len%ENCRYPT_BLOCK_SIZE != 0 || buf == NULL)
        return -1;
    ctx_aes aes;
    aes_init(&aes,kAesKeyLen,(unsigned char*)szAesKey);
    unsigned char inBuff[ENCRYPT_BLOCK_SIZE],ouBuff[ENCRYPT_BLOCK_SIZE];

    int32_t pos = 0;
    for(; pos < buf_len; pos += ENCRYPT_BLOCK_SIZE )
    {
        sd_memset(inBuff,0,ENCRYPT_BLOCK_SIZE);
        sd_memset(ouBuff,0,ENCRYPT_BLOCK_SIZE);
        sd_memcpy(inBuff, buf + pos , ENCRYPT_BLOCK_SIZE);
        aes_cipher(&aes, inBuff,ouBuff);
        sd_memcpy(buf + pos , ouBuff, ENCRYPT_BLOCK_SIZE);
    }
#ifdef _DEBUG
    char log[1024] = {0};
    tmp_str2hex((const char*)buf, buf_len, log, sizeof(log));
    LOG_DEBUG("simple_encrypt:%s",log);
#endif
    return SUCCESS;
}

int32_t simple_aes_decrypt(char * buf , int32_t buf_len)
{
    if(buf_len%ENCRYPT_BLOCK_SIZE != 0 || buf == NULL)
        return -1;
    ctx_aes aes;
    aes_init(&aes,kAesKeyLen,(unsigned char*)szAesKey);
    unsigned char inBuff[ENCRYPT_BLOCK_SIZE],ouBuff[ENCRYPT_BLOCK_SIZE];

    int32_t pos = 0;
    for(; pos < buf_len; pos += ENCRYPT_BLOCK_SIZE )
    {
        sd_memset(inBuff,0,ENCRYPT_BLOCK_SIZE);
        sd_memset(ouBuff,0,ENCRYPT_BLOCK_SIZE);
        sd_memcpy(inBuff, buf + pos , ENCRYPT_BLOCK_SIZE);
        aes_invcipher(&aes, inBuff,ouBuff);
        sd_memcpy(buf + pos , ouBuff, ENCRYPT_BLOCK_SIZE);
    }
#ifdef _DEBUG
    char log[2048] = {0};
    tmp_str2hex((const char*)buf, buf_len, log, sizeof(log));
    LOG_DEBUG("simple_decrypt:%s",log);
#endif
    return SUCCESS;
}


int32_t aes_encrypt(char * buf, int32_t buf_len, unsigned char szKey[Bits128])
{
    if(buf_len%ENCRYPT_BLOCK_SIZE != 0 || buf == NULL)
        return -1;
    ctx_aes aes;
    aes_init(&aes, Bits128, (unsigned char*)szKey);
    unsigned char inBuff[ENCRYPT_BLOCK_SIZE];
    unsigned char ouBuff[ENCRYPT_BLOCK_SIZE];

    int32_t pos = 0;
    for(; pos < buf_len; pos += ENCRYPT_BLOCK_SIZE )
    {
        sd_memset(inBuff,0,ENCRYPT_BLOCK_SIZE);
        sd_memset(ouBuff,0,ENCRYPT_BLOCK_SIZE);
        sd_memcpy(inBuff, buf + pos , ENCRYPT_BLOCK_SIZE);
        aes_cipher(&aes, inBuff,ouBuff);
        sd_memcpy(buf + pos , ouBuff, ENCRYPT_BLOCK_SIZE);
    }
#ifdef _DEBUG
    char log[1024] = {0};
    tmp_str2hex((const char*)buf, buf_len, log, sizeof(log));
    LOG_DEBUG("simple_encrypt:%s",log);
#endif
    return SUCCESS;
}

int32_t aes_decrypt(char * buf, int32_t buf_len, unsigned char szKey[Bits128])
{
    if(buf_len%ENCRYPT_BLOCK_SIZE != 0 || buf == NULL)
        return -1;
    ctx_aes aes;
    aes_init(&aes, Bits128, (unsigned char*)szKey);
    unsigned char inBuff[ENCRYPT_BLOCK_SIZE];
    unsigned char ouBuff[ENCRYPT_BLOCK_SIZE];

    int32_t pos = 0;
    for(; pos < buf_len; pos += ENCRYPT_BLOCK_SIZE )
    {
        sd_memset(inBuff,0,ENCRYPT_BLOCK_SIZE);
        sd_memset(ouBuff,0,ENCRYPT_BLOCK_SIZE);
        sd_memcpy(inBuff, buf + pos , ENCRYPT_BLOCK_SIZE);
        aes_invcipher(&aes, inBuff,ouBuff);
        sd_memcpy(buf + pos , ouBuff, ENCRYPT_BLOCK_SIZE);
    }
#ifdef _DEBUG
    char log[2048] = {0};
    tmp_str2hex((const char*)buf, buf_len, log, sizeof(log));
    LOG_DEBUG("simple_decrypt:%s",log);
#endif
    return SUCCESS;
}
