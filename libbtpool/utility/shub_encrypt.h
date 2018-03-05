#ifndef __SHUB_ENCRYPT__H__20141120_
#define __SHUB_ENCRYPT__H__20141120_

#include "utility/define.h"

extern const int32_t kAesKeyLen;
/* rsa encrypted */
#define PUB_KEY_LEN 140
#define RSA_ENCRYPT_HEADER_LEN              (4+4+4+128+4)  //【魔数】【密钥版本】【密钥长度】【密钥内容】【数据长度】
#define RES_QUERY_AES_CIPHER_LEN            128            //经RSA加密后的AES的密文长度
#define RSA_ENCRYPT_MAGIC_NUM               637753480
#define RSA_PUBKEY_VERSION_SHUB             10000
#define RSA_PUBKEY_VERSION_BTHUB            10000
#define RSA_PUBKEY_VERSION_EMULEHUB         10000
#define RSA_PUBKEY_VERSION_PHUB             40000
#define RSA_PUBKEY_VERSION_VIPHUB           60000
#define RSA_PUBKEY_VERSION_TRACKER          70000
#define RSA_PUBKEY_VERSION_PARTNER_CDN      80000
#define RSA_PUBKEY_VERSION_DPHUB            50000
#define RSA_PUBKEY_VERSION_MAGNETHUB        10000

int32_t gen_aes_key_by_user_data(const void *user_data, uint8_t *p_aeskey);
int32_t res_query_rsa_pub_encrypt(int32_t len, uint8_t *text, char *encrypted, uint32_t *p_enc_len, int32_t key_ver);
int32_t build_rsa_encrypt_header(char **ppbuf, int32_t *p_buflen, int32_t pubkey_ver,uint8_t *aes_key, int32_t data_len);

int32_t xl_aes_encrypt(char* buffer,uint32_t* len);
int32_t xl_aes_decrypt(char* pDataBuff, uint32_t* nBuffLen);
int32_t simple_aes_encrypt(char * buf , int32_t buf_len);
int32_t simple_aes_decrypt(char * buf , int32_t buf_len);

#ifndef Bits128
#define Bits128      16  /*密钥长度*/
#endif
int32_t aes_encrypt(char * buf, int32_t buf_len, unsigned char szKey[Bits128]);
int32_t aes_decrypt(char * buf, int32_t buf_len, unsigned char szKey[Bits128]);

#endif
