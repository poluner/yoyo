/*
 * ================================================================================================================
 *    Description:  BT Pool Server Query Require Stream Decode Head File
 *        Version:  1.0
 *        Created:  08/26/2016
 *         Author:  Melo
 * Modify History: 1. Created by Melo
 * ================================================================================================================
 */

#ifndef  BTPOOL_P2SPROTOCOL_QUERY_REQ_H_
#define  BTPOOL_P2SPROTOCOL_QUERY_REQ_H_

#include "common.h"

namespace BTPOOL
{

class QueryReq
{
public:
    QueryReq(uint32_t magic_num = 637753480, uint32_t version = 10000):str_aes_key_(""), rsa_len_(0), ptr_rsa_(NULL), magic_num_(magic_num), version_(version)
    {
        init();
    };
    ~QueryReq()
    {
        if(ptr_rsa_)
        {
           RSA_free(ptr_rsa_);
           ptr_rsa_ = NULL;
        }
    };

public:
    int32_t decrypt_req(char *body_buf, int32_t buf_len, std::string &str_query_struct_out);

private:
    void init();

    int32_t load_RSA_private_key();


public:
    std::string str_aes_key_;

private:
    int32_t rsa_len_;
    RSA *ptr_rsa_;
    uint32_t magic_num_;
    uint32_t version_;

};

}
#endif




