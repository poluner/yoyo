/*----------------------------------------------------------------------------------------------------------
author:		ZHANGSHAOHAN
created:	2008/08/20
-----------------------------------------------------------------------------------------------------------*/
#ifndef _AES_H_
#define _AES_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "utility/define.h"

#define Bits128				16  /*密钥长度*/
#define Bits192				24
#define Bits256				32
#define ENCRYPT_BLOCK_SIZE	16

typedef struct
{
    int32_t Nb;         /* block size in 32-bit words.  Always 4 for AES.  (128 bits). */
    int32_t Nk;         /* key size in 32-bit words.  4, 6, 8.  (128, 192, 256 bits). */
    int32_t Nr;         /* number of rounds. 10, 12, 14. */
    uint8_t	   State[4][4];/* State matrix */
    uint8_t	   key[32];    /* the seed key. size will be 4 *keySize from ctor. */
    uint8_t    w[16*15];   /* key schedule array. (Nb*(Nr+1))*4 */
} ctx_aes;

void aes_init(ctx_aes* aes, int keySize, uint8_t* keyBytes);
void aes_cipher(ctx_aes* aes, uint8_t* input, uint8_t* output);  /* encipher 16-bit input */
void aes_invcipher(ctx_aes* aes, uint8_t* input, uint8_t* output);
//
void SetNbNkNr(ctx_aes* aes, int32_t keyS);
void AddRoundKey(ctx_aes* aes, int32_t round);
void SubBytes(ctx_aes* aes);
void InvSubBytes(ctx_aes* aes);
void ShiftRows(ctx_aes* aes);
void InvShiftRows(ctx_aes* aes);
void MixColumns(ctx_aes* aes);
void InvMixColumns(ctx_aes* aes);
uint8_t  gfmultby01(uint8_t b);
uint8_t gfmultby02(uint8_t b);
uint8_t gfmultby03(uint8_t b);
unsigned char gfmultby09(unsigned char b);
unsigned char gfmultby0b(unsigned char b);
unsigned char gfmultby0d(unsigned char b);
unsigned char gfmultby0e(unsigned char b);
void KeyExpansion(ctx_aes* aes);
void SubWord(uint8_t *word, uint8_t *result);
void RotWord(uint8_t *word, uint8_t *result);

int32_t gen_aes_key_by_ptr(const void *ptr, uint8_t *p_aeskey);
int32_t aes_encrypt_with_known_key(char* buffer,uint32_t* len, uint8_t *key);
int32_t aes_decrypt_with_known_key(char* p_data_buff, uint32_t* p_data_buff_len, uint8_t *key);

//////////////////////////////////
//对src中的数据进行AES(128Bytes)加密，加密后数据保存在des中
//aes_key:用于加密的key,但是注意，这个key还需要进行MD5运算，用算出的MD5值作为加密解密的密钥!
//src:待加密数据
//src_len:待加密数据长度
//des:加密后的数据
//des_len:输入输出参数，传进去的的是des的内存最大长度,传出来的是加密后的数据长度
int32_t sd_aes_encrypt( const char* aes_key,const uint8_t * src, uint32_t src_len,uint8_t * des, uint32_t * des_len);

//////////////////////////////////
//对src中的数据进行AES(128Bytes)解密，解密后数据保存在des中
//aes_key:用于解密的key,但是注意，这个key还需要进行MD5运算，用算出的MD5值作为加密解密的密钥!
//src:待解密数据
//src_len:待解密数据长度
//des:解密后的数据
//des_len:输入输出参数，传进去的的是des的内存最大长度,传出来的是解密后的数据长度
int32_t sd_aes_decrypt(const  char* aes_key,const uint8_t * src, uint32_t src_len,uint8_t * des, uint32_t * des_len);

//////////////////////////////////
//对整个文件进行AES(128Bytes)解密，并用解密后的文件替换解密前的文件
//aes_key:用于解密的key,但是注意，这个key还需要进行MD5运算，用算出的MD5值作为加密解密的密钥!
//file_name:待解密文件全名
int32_t sd_aes_decrypt_file(const  char* aes_key,const char * file_name);


int32_t em_aes_encrypt(char* in_buffer ,uint32_t in_len, char* out_buffer, uint32_t* out_len, uint8_t key[16]);
int32_t em_aes_decrypt(char* in_buffer ,uint32_t in_len, char* out_buffer, uint32_t* out_len, uint8_t key[16]);

#ifdef __cplusplus
}
#endif

#endif

