#include "interface_query_crypto.h"

namespace BTPOOL
{

std::string InterfaceCrypto::interface_decode(char *body_buf, int32_t len)
{
    if(NULL == body_buf || 0 == len)
    {
        return "";
    }

    std::string str_query_struct_out;
    QueryReq decode(637753480, 10000);
    int ret_decrypt_req = decode.decrypt_req(body_buf, len, str_query_struct_out);
    if(ret_decrypt_req)
    {
        return "";
    }
    this->str_aes_key_ = decode.str_aes_key_;

    return str_query_struct_out;
}


std::string InterfaceCrypto::interface_encode(char *body_buf, int32_t len)
{
    std::string encoded_buf;
    QueryResp encode;
    encode.str_aes_key_ = this->str_aes_key_;
    int ret_encrypt_resp = encode.encrypt_resp(body_buf, len, encoded_buf);
    if(ret_encrypt_resp)
    {
        return "";
    }

    return encoded_buf;
}

}
