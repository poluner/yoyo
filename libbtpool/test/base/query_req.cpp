/*
 * ================================================================================================================
 *    Description:  BT Pool Server Query Require Stream Decode Srouce File
 *        Version:  1.0
 *        Created:  08/26/2016
 *         Author:  Melo
 * Modify History: 1. Created by Melo
 * ================================================================================================================
 */
#include "query_req.h"
#include "md5.h"

namespace BTPOOL
{

void QueryReq::init()
{
    load_RSA_private_key();
}

int32_t QueryReq::load_RSA_private_key()
{
    //load RSA private key
    std::string rsa_priKey_path = "./pri.pem";
    FILE* f_pemfile;
    if(NULL == (f_pemfile = fopen(rsa_priKey_path.c_str(),"r")))
    {
         return -1;
    }
    if(NULL == (ptr_rsa_ = PEM_read_RSAPrivateKey(f_pemfile,NULL,NULL,NULL)))
    {
        if(f_pemfile)
            fclose(f_pemfile);
        return -2;
    }

    rsa_len_ = RSA_size(ptr_rsa_);
    if(f_pemfile)
        fclose(f_pemfile);
    return 0;
}


int32_t QueryReq::decrypt_req(char *body_buf, int32_t buf_len, std::string &str_query_struct_out)
{
    if(NULL == body_buf || 0 == buf_len || 0 == rsa_len_ || NULL == ptr_rsa_)
    {
        return BTPOOL_ERR_ARG;
    }

    //rsa decode
    int limit_len = sizeof(magic_num_) + sizeof(version_) + sizeof(uint32_t) + rsa_len_;
    if(buf_len <= limit_len)
    {
        return BTPOOL_ERR_DECRYPT_LIMIT_LEN;
    }

    uint32_t *ptr_body = (uint32_t*)body_buf;
    if (*ptr_body++ != magic_num_)
    {
        return BTPOOL_ERR_GET_MAGIC_NUM;
    }

    if (*ptr_body++ != version_)
    {
         return BTPOOL_ERR_GET_VERSION;
    }

    if (*ptr_body++ != (uint32_t)rsa_len_)
    {
         return BTPOOL_ERR_GET_RSALEN;
    }

    //get aes key from buf decoded by rsa
    unsigned char *aes_key_buf = new unsigned char[rsa_len_];
    memset(aes_key_buf, 0, sizeof(aes_key_buf));

    //RSA dup private key
    RSA *duped_rsa = RSAPrivateKey_dup(ptr_rsa_);
    if(NULL == duped_rsa)
    {
        delete[] aes_key_buf;
        aes_key_buf = NULL;
        return    BTPOOL_ERR_GET_DUP_RSA_PRIKEY;
    }

    //RSA decode to get aes key
    if (AES_BLOCK_SIZE != RSA_private_decrypt(rsa_len_, (unsigned char*)(body_buf + limit_len - rsa_len_), aes_key_buf, duped_rsa, RSA_PKCS1_PADDING))
    {
        if(NULL != duped_rsa)
        {
            RSA_free(duped_rsa);
            duped_rsa = NULL;
        }

        delete[] aes_key_buf;
        aes_key_buf = NULL;
        return BTPOOL_ERR_GET_AESKEY;
    }
    else
    {
        RSA_free(duped_rsa);
        duped_rsa = NULL;
    }


    /*start aes decode step 1*/
    AES aes;
    aes.setDecryptKey(aes_key_buf);

    //decode out step 1
    int decode_outlen = 0;
    char *decode_out = new char[1024];
    memset(decode_out, 0, 1024);

    /*AES decrypt, decode the left buf which len is (buf_len - limit_len - sizeof(uint32_t))
     *decode_out is a big buf to save the all req body decoded
     *body_buf: |magic_num|version|uint32_t|RSA code (AES KEY)|uint32_t|rest data(step 1 can get the part data which can be trans to QueryStruct)|
     *rest data:step 2 can decode the part data except first 3*uint32_t after step 1, struct- |uint32|uint32|uint32 (inter data len)|...inter data decoded by step 2...|
     */
    if(false == aes.decrypt((unsigned char*)(body_buf + limit_len + sizeof(uint32_t)), (buf_len - limit_len - sizeof(uint32_t)), (unsigned char*)decode_out, &decode_outlen))
    {
        delete[] decode_out;
        decode_out = NULL;
        delete[] aes_key_buf;
        aes_key_buf = NULL;
        return BTPOOL_ERR_AES_DECRYPT;
    }

    str_aes_key_ = std::string((char*)aes_key_buf, rsa_len_);

    str_query_struct_out = std::string(decode_out, decode_outlen);

    delete[] decode_out;
    decode_out = NULL;
    delete[] aes_key_buf;
    aes_key_buf = NULL;
    return BTPOOL_ERR_SUCC;
}

}
