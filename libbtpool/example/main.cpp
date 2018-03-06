/*
 * ================================================================================================================
 *    Description:  BT Pool Query Server Com Example code
 *        Version:  1.0
 *        Created:  09/28/2016
 *         Author:  Melo
 * Modify History: 1. Created by Melo
 * ================================================================================================================
 */

#include "btquery_client.h"
#include "curl_tool.h"

using namespace BTQUERY;

int main(int argc, char* argv[])
{
    if (argc != 2) {
        return -1;
    }
    //base
    QueryParams params;
    std::string str_require;
    QueryResponse response;
    std::string str_response;
    std::string infohash(argv[1]);

    //���ò���
    params.infohash = infohash;
    params.peerid = "aaaaaaaaaaaaaaaa";
    params.appid = 1000;
    params.down_channel = "miui";
    params.version = 62;
    params.seq = 10001;
    params.cmd_id = 3095;

    //����
    BTqueryClient query_client;
    int ret_encode = query_client.encode_query_params(params, str_require);
    if(ret_encode)
    {
        printf("encode_query_params error:%d\n", ret_encode);
        return -1;
    }

    //��������
    std::string url = "127.0.0.1:801";
    http_tool::CHttpClient http_client_tool;
    int ret_post = http_client_tool.Post(url, str_require, str_response);
    if(ret_post)
    {
        printf("http_client_tool.Post error:%d\n", ret_post);
        return -1;
    }

    //�������ؽ��
    int ret_decode = query_client.decode_query_result(str_response, response);
    if(ret_decode)
    {
        printf("decode_query_result error:%d\n", ret_decode);
        return -1;
    }

    //����response
    if(2 == response.result)
    {
        printf("no");
    }
    else if(0 == response.result)
    {
        printf("yes");
        //����
        std::string file_path = "/tmp/torrent/" + infohash + ".torrent";
        FILE *pFile_save = fopen(file_path.c_str(), "w");
        if(!pFile_save)
        {
            return -1;
        }

        fwrite (response.bt_file.c_str(), 1, response.bt_file.size(), pFile_save);
        fclose(pFile_save);
    }

    return 0;
}
