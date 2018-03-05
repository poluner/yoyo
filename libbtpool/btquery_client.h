/*
 * ================================================================================================================
 *    Description:  BT Pool Query Server Com Head file
 *        Version:  1.0
 *        Created:  09/28/2016
 *         Author:  Melo
 * Modify History: 1. Created by Melo
 * ================================================================================================================
 */

#ifndef HUB_BTQUERY_CLIENT_H_
#define HUB_BTQUERY_CLIENT_H_
#include "btquery_client_common.h"

namespace BTQUERY
{
class BTqueryClient:public ParamTool
{
public:
    BTqueryClient(){};
    ~BTqueryClient(){};

public:
    /* 种子查询接口
       * @throw  无
       * @param(in)  params_in:查询参数结构体
       * @param(out) packed_buf_out:加密后的请求体，用于HTTP POST操作
       * @return 参见 enum ERROR_CODE
       */
    int encode_query_params(QueryParams &params_in, std::string &packed_buf_out);

    /* 处理种子查询结果接口
       * @throw  无
       * @param(in)  query_result_buf_in: HTTP返回的查询结果
       * @param(out) response_out:解析后的查询结果结构体
       * @return 参见 enum ERROR_CODE
       */
    int decode_query_result(std::string &query_result_buf_in, QueryResponse &response_out);

    std::string get_user_data_aes_key();

protected:
    /* 将HTTP返回的查询结构buffer解析成QueryResponse结构体
       * @throw  无
       * @param(in)  buffer_in: http返回的查询结果buffer
       * @param(in)  buffer_len: http返回的查询结果buffer长度
       * @param(out) struct_out:解析后的查询结果结构体
       * @return 参见 enum ERROR_CODE
       */
    int parser_buffer_to_struct(char* buffer_in, int buffer_len, QueryResponse &struct_out);

private:
    //加密查询外层KEY
    std::string user_data_aes_key_;
};
}

#endif //HUB_BTQUERY_CLIENT_H_

