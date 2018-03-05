/*
 * ================================================================================================================
 *    Description:  BT Pool Server Query Response Stream Encode Srouce File
 *        Version:  1.0
 *        Created:  08/26/2016
 *         Author:  Melo
 * Modify History: 1. Created by Melo
 * ================================================================================================================
 */

#include "query_resp.h"

namespace BTPOOL
{

int32_t QueryResp::encrypt_resp(char *body_buf, int32_t buf_len, std::string &str_encoded_buf_out)
{
    uint32_t encode_outlen_step_1 = buf_len + AES_BLOCK_SIZE;
    char *encode_out_step_1 = new char[encode_outlen_step_1 + 1];
    memset(encode_out_step_1, 0, encode_outlen_step_1 + 1);

    /*encode_outlen:final encode out len*/
    int32_t encode_outlen = buf_len;

    /* 1 part encrypt */
    //get md5 key of user data
    unsigned char body_aes_key_buf[16];
    memset(body_aes_key_buf, 0, 16);
    md5_data((unsigned char*)body_buf, 8, body_aes_key_buf);

    AES aes_step_1;
    aes_step_1.setEncryptKey(body_aes_key_buf);

    //aes encrypt
    if(false == aes_step_1.encrypt((unsigned char*)body_buf + 3 * sizeof(uint32_t), (uint32_t)buf_len - 3 * sizeof(uint32_t), (unsigned char*)encode_out_step_1, (int*)&encode_outlen_step_1) || encode_outlen_step_1 < 3 * sizeof(uint32_t))
    {
        delete[] encode_out_step_1;
        encode_out_step_1 = NULL;
        return BTPOOL_ERR_AES_ENCRYPT;
    }
    encode_outlen = encode_outlen_step_1 + 3 * sizeof(uint32_t);

    char *encode_out_tmp = new char[encode_outlen + 1];
    memset(encode_out_tmp, 0, encode_outlen + 1);
    memcpy(encode_out_tmp, body_buf, 3 * sizeof(uint32_t));
    memcpy(encode_out_tmp + 3 * sizeof(uint32_t), encode_out_step_1, encode_outlen_step_1);
    *((uint32_t*)(encode_out_tmp + 2 * sizeof(uint32_t)))  = encode_outlen_step_1;

    /* step 2 encode */
    int32_t encode_outlen_step_2 = encode_outlen + AES_BLOCK_SIZE;
    char *encode_out_step_2 = new char[encode_outlen_step_2 + 1];
    memset(encode_out_step_2, 0, encode_outlen_step_2 + 1);

    AES aes_step_2;
    aes_step_2.setEncryptKey((unsigned char*)str_aes_key_.c_str());
    encode_outlen_step_2 -= sizeof(uint32_t);

    //AES encrypt
    //encode_out_step_2 + sizeof(uint32_t) :  struct - | len | data |
    if(false == aes_step_2.encrypt((unsigned char*)encode_out_tmp, encode_outlen, (unsigned char*)(encode_out_step_2 + sizeof(uint32_t)), &encode_outlen_step_2))
    {
        delete[] encode_out_tmp;
        encode_out_tmp = NULL;
        delete[] encode_out_step_1;
        encode_out_step_1 = NULL;
        delete[] encode_out_step_2;
        encode_out_step_2 = NULL;
        return BTPOOL_ERR_AES_ENCRYPT;
    }

    //set the len after encode step2
    *((uint32_t*)encode_out_step_2) = encode_outlen_step_2;

    encode_outlen_step_2 += sizeof(uint32_t);
    encode_outlen = encode_outlen_step_2;

    //for main logic(http resp body buf len)
    //outlen_ = encode_outlen_step_2; not use
    str_encoded_buf_out = std::string(encode_out_step_2, encode_outlen);

    //done
    delete[] encode_out_tmp;
    encode_out_tmp = NULL;
    delete[] encode_out_step_1;
    encode_out_step_1 = NULL;
    delete[] encode_out_step_2;
    encode_out_step_2 = NULL;

    return BTPOOL_ERR_SUCC;
}

}
