/*
 * ================================================================================================================
 *    Description:  BT Pool Query Server Com Common Head File
 *        Version:  1.0
 *        Created:  09/28/2016
 *         Author:  Melo
 * Modify History: 1. Created by Melo
 * ================================================================================================================
 */

#ifndef HUB_BTQUERY_CLIENT_COMMON_H_
#define HUB_BTQUERY_CLIENT_COMMON_H_
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <stdint.h>

namespace BTQUERY
{
enum PARAM_LIMIT
{
    PL_INFOID_LEN = 40,
    PL_PEERID_LEN = 16,
};

enum ERROR_CODE
{
    /* common */
    EC_ERROR = -1,
    EC_SUCC = 0,

    /* query encode */
    EC_INFOID_SIZE = 100,
    EC_HEX_INFOID_UNKNOW = 101,
    EC_SET_QUERY_PARAMS = 102,
    EC_PARAMS = 103,

    EC_AES_ENCRYPT = 200,
    EC_GEN_AES_KEY = 201,
    EC_AES_ENCRYPT_WITH_KEY = 202,
    EC_BUILD_RSA_ENCRYPT = 203,

    /* query decode */
    EC_DECODE_BODY_LEN_ERROR = 300,
    EC_AES_DECRYPT_WITH_KEY = 301,
    EC_AES_DECRYPT = 302,

    EC_PARSER_BUF_TO_STRUCT = 400,
};


/* 加密查询参数结构体
 * 用户主要填写字段参见example.cpp示例
 */
struct QueryParams
{
    std::string infohash;
    std::string sha1_infohash;
    uint32_t support_compress;
    std::string peerid;
    uint16_t appid;
    std::string down_channel;
    uint32_t version;
    uint32_t seq;
    uint16_t cmd_id;

    QueryParams()
    {
        support_compress = 0;
        appid = 0;
        version = 0;
        seq = 0;
        cmd_id = 0;
    }
};

/* 加密查询结果结构体
 * 查询后将http返回内容buf解析为该结构体，简单使用可直接取bt_file种子文件字段
 */
struct QueryResponse
{
    uint32_t version;
    uint32_t seq;
    uint32_t len;
    uint32_t client_ver;
    int16_t compress_flag;
    uint32_t padding_len;
    uint16_t cmd_id;
    int32_t result;
    int32_t compress;
    std::string bt_file;

    QueryResponse()
    {
        version = 0;
        seq = 0;
        len = 0;
        client_ver = 0;
        compress_flag = 0;
        padding_len = 0;
        cmd_id = 0;
        result = 0;
        compress = 0;
    }
};


class ParamTool
{
public:
    ParamTool(){};
    ~ParamTool(){};

protected:
    /* 将用户提供的40字符HEX INFOID转化为二进制
       * @throw  无
       * @param(in)  params:查询参数结构体
       * @param(out) 无
       * @return 参见 enum ERROR_CODE
       * @desc hex infoid -> bin infoid 过程在该函数内部完成转换和赋值
       */
    int gen_SHA1_infoid(QueryParams &params);

    /* 根据用户提供的QueryParams结构体构建请求参数，用于后续加密操作
       * @throw  无
       * @param(in)  params:查询参数结构体
       * @param(out) plain_pack_len:buffer长度
       * @return 参数buffer
       */
    std::string set_query_params(QueryParams &params, uint32_t &plain_pack_len);

private:
    /* 内部接口:构建请求参数 */
    const std::string build_reserve6x(QueryParams &params, size_t &len, bool force = false);
};
}

#endif //HUB_BTQUERY_CLIENT_COMMON_H_

