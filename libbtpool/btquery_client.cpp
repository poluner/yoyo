/*
 * ================================================================================================================
 *    Description:  BT Pool Query Server Com Srouce file
 *        Version:  1.0
 *        Created:  09/28/2016
 *         Author:  Melo
 * Modify History: 1. Created by Melo
 * ================================================================================================================
 */

#include "btquery_client.h"
#include "shub_encrypt.h"
#include "utility/define.h"
#include "utility/aes.h"
#include "utility/bytebuffer.h"
#include "utility/utility.h"
#include "package_helper.h"
#include "sd_string.h"
#include "md5.h"

namespace BTQUERY
{

int BTqueryClient::encode_query_params(QueryParams &params_in, std::string &packed_buf_out)
{
    if(PL_INFOID_LEN != params_in.infohash.size() || PL_PEERID_LEN != params_in.peerid.size())
    {
        return EC_PARAMS;
    }

    char *user_data = NULL;
    uint32_t user_data_len = 0;
    char *pack = NULL;

    int ret_gen_binfoid = gen_SHA1_infoid(params_in);
    if(EC_SUCC != ret_gen_binfoid)
    {
        return ret_gen_binfoid;
    }
    std::string params_buf = set_query_params(params_in, user_data_len);
    if(params_buf.empty())
    {
        return EC_SET_QUERY_PARAMS;
    }
    user_data = (char*)params_buf.c_str();
    uint32_t packLen = user_data_len;
    uint32_t encSrcLen = user_data_len;

    //aes
    packLen =( (packLen - HUB_CMD_HEADER_LEN) + HUB_ENCODE_PADDING_LEN)/HUB_ENCODE_PADDING_LEN * HUB_ENCODE_PADDING_LEN + HUB_CMD_HEADER_LEN;
    //rsa
    packLen = (packLen + HUB_ENCODE_PADDING_LEN)/HUB_ENCODE_PADDING_LEN*HUB_ENCODE_PADDING_LEN + RSA_ENCRYPT_HEADER_LEN;

    pack = new char[packLen];
    memset(pack, 0 , packLen);
    memcpy(pack + RSA_ENCRYPT_HEADER_LEN, user_data, user_data_len);
    int ret = xl_aes_encrypt(pack + RSA_ENCRYPT_HEADER_LEN, &encSrcLen);
    if(ret)
    {
        delete[] pack;
        return EC_AES_ENCRYPT;
    }

    unsigned char user_data_aes_key_tmp[16] = {0};
    if(gen_aes_key_by_user_data(user_data, user_data_aes_key_tmp))
    {
        delete[] pack;
        return EC_GEN_AES_KEY;
    }

    ret = aes_encrypt_with_known_key(pack + RSA_ENCRYPT_HEADER_LEN, &encSrcLen, user_data_aes_key_tmp);
    if(ret)
    {
        delete[] pack;
        return EC_AES_ENCRYPT_WITH_KEY;
    }

    int rsaHeadWriteSizeRemain = RSA_ENCRYPT_HEADER_LEN;
    char *pack_temp = pack;
    ret = build_rsa_encrypt_header(&pack_temp, &rsaHeadWriteSizeRemain, RSA_PUBKEY_VERSION_MAGNETHUB, user_data_aes_key_tmp, encSrcLen);
    if(ret)
    {
        delete[] pack;
        return EC_BUILD_RSA_ENCRYPT;
    }

    user_data_aes_key_ = std::string((char*)user_data_aes_key_tmp, 16);
    packed_buf_out = std::string(pack, packLen);

    delete[] pack;
    return EC_SUCC;

}


int BTqueryClient::decode_query_result(std::string &query_result_buf_in, QueryResponse &response_out)
{
    char* query_result = new char[query_result_buf_in.size() + 1];
    memset(query_result, 0, query_result_buf_in.size() + 1);
    memcpy(query_result, query_result_buf_in.c_str(), query_result_buf_in.size());

    uint32_t body_len = 0;
    int32_t length = query_result_buf_in.size();
    char* body_read_walk = query_result;
    sd_get_int32_from_lt(&body_read_walk, &length, (int32_t*)&body_len);
    if(body_len > length)
    {
        delete[] query_result;
        return EC_DECODE_BODY_LEN_ERROR;
    }

    int ret_decode = 0;
    body_read_walk = query_result;
    body_read_walk += sizeof(uint32_t);
    ret_decode = aes_decrypt_with_known_key(body_read_walk, &body_len, (uint8_t*)user_data_aes_key_.c_str());
    if(ret_decode)
    {
        delete[] query_result;
        return EC_AES_DECRYPT_WITH_KEY;
    }

    ret_decode = xl_aes_decrypt(body_read_walk, &body_len);
    if(ret_decode)
    {
        delete[] query_result;
        return EC_AES_DECRYPT;
    }

    if(parser_buffer_to_struct(body_read_walk, body_len, response_out))
    {
        delete[] query_result;
        return EC_PARSER_BUF_TO_STRUCT;
    }

    delete[] query_result;
    return EC_SUCC;
}

int BTqueryClient::parser_buffer_to_struct(char* buffer_in, int buffer_len, QueryResponse &struct_out)
{
    int resp_base_len = sizeof(struct_out.version)
                        + sizeof(struct_out.seq)
                        + sizeof(struct_out.len)
                        + sizeof(struct_out.client_ver)
                        + sizeof(struct_out.compress_flag)
                        + sizeof(struct_out.padding_len)
                        + sizeof(struct_out.cmd_id)
                        + sizeof(struct_out.result)
                        + sizeof(struct_out.compress)
                        + sizeof(uint32_t);
    if(resp_base_len > buffer_len)
    {
        return EC_ERROR;
    }

    size_t pos_get_value = 0;
    struct_out.version = *((uint32_t*)(buffer_in + pos_get_value));
    pos_get_value += sizeof(struct_out.version);

    struct_out.seq = *((uint32_t*)(buffer_in + pos_get_value));
    pos_get_value += sizeof(struct_out.seq);

    struct_out.len = *((uint32_t*)(buffer_in + pos_get_value));
    pos_get_value += sizeof(struct_out.len);

    struct_out.client_ver = *((uint32_t*)(buffer_in + pos_get_value));
    pos_get_value += sizeof(struct_out.client_ver);

    struct_out.compress_flag = *((int16_t*)(buffer_in + pos_get_value));
    pos_get_value += sizeof(struct_out.compress_flag);

    struct_out.padding_len = *((uint32_t*)(buffer_in + pos_get_value));
    pos_get_value += sizeof(struct_out.padding_len);

    struct_out.cmd_id = *((short*)(buffer_in + pos_get_value));
    pos_get_value += sizeof(struct_out.cmd_id);

    struct_out.result = *((int32_t*)(buffer_in + pos_get_value));
    pos_get_value += sizeof(struct_out.result);

    struct_out.compress = *((int32_t*)(buffer_in + pos_get_value));
    pos_get_value += sizeof(struct_out.compress);

    uint32_t file_len = *((uint32_t*)(buffer_in + pos_get_value));
    pos_get_value += sizeof(file_len);

    if((buffer_len - resp_base_len) < file_len)
    {
        return EC_ERROR;
    }

    struct_out.bt_file = std::string(buffer_in + pos_get_value, file_len);
    return EC_SUCC;
}

std::string BTqueryClient::get_user_data_aes_key()
{
    #ifdef DEBUG
    return user_data_aes_key_;
    #else
    return "";
    #endif
}
}


