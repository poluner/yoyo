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
    /* ���Ӳ�ѯ�ӿ�
       * @throw  ��
       * @param(in)  params_in:��ѯ�����ṹ��
       * @param(out) packed_buf_out:���ܺ�������壬����HTTP POST����
       * @return �μ� enum ERROR_CODE
       */
    int encode_query_params(QueryParams &params_in, std::string &packed_buf_out);

    /* �������Ӳ�ѯ����ӿ�
       * @throw  ��
       * @param(in)  query_result_buf_in: HTTP���صĲ�ѯ���
       * @param(out) response_out:������Ĳ�ѯ����ṹ��
       * @return �μ� enum ERROR_CODE
       */
    int decode_query_result(std::string &query_result_buf_in, QueryResponse &response_out);

    std::string get_user_data_aes_key();

protected:
    /* ��HTTP���صĲ�ѯ�ṹbuffer������QueryResponse�ṹ��
       * @throw  ��
       * @param(in)  buffer_in: http���صĲ�ѯ���buffer
       * @param(in)  buffer_len: http���صĲ�ѯ���buffer����
       * @param(out) struct_out:������Ĳ�ѯ����ṹ��
       * @return �μ� enum ERROR_CODE
       */
    int parser_buffer_to_struct(char* buffer_in, int buffer_len, QueryResponse &struct_out);

private:
    //���ܲ�ѯ���KEY
    std::string user_data_aes_key_;
};
}

#endif //HUB_BTQUERY_CLIENT_H_

