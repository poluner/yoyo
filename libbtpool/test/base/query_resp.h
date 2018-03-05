/*
 * ================================================================================================================
 *    Description:  BT Pool Server Query Response Stream Encode Head File
 *        Version:  1.0
 *        Created:  08/26/2016
 *         Author:  Melo
 * Modify History: 1. Created by Melo
 * ================================================================================================================
 */

#ifndef BTPOOL_P2SPROTOCOL_QUERY_RESP_H_
#define BTPOOL_P2SPROTOCOL_QUERY_RESP_H_

#include "common.h"

namespace BTPOOL
{
class HostByteBuffer;

class QueryResp
{
public:
    QueryResp(){};
    ~QueryResp(){};

public:
    int32_t encrypt_resp(char *body_buf, int32_t buf_len, std::string &str_encoded_buf_out);

public:
    std::string str_aes_key_;
};

}
#endif //__BTPOOL_QUERY_RESP__

