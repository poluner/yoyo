#include "btquery_client.h"
#include "interface_query_crypto.h"
#include "query_struct_tools.h"

using namespace BTQUERY;
using namespace BTPOOL;

int main(int argc, char* argv[])
{
    if(10 != argc)
    {
        printf("Usage: ./test ./8CF9317C7ECCBB657F5C35900DDA8FAF779F8BFA.torrent 8CF9317C7ECCBB657F5C35900DDA8FAF779F8BFA aaaaaaaaaaaaaaaa 1000 miui 62 10001 3060 0 \n");
        return -1;
    }

    //base
    QueryParams params;
    QueryResponse response;
    std::string str_require;
    std::string str_response;

    params.infohash = argv[2];
    params.peerid = argv[3];
    params.appid = atoi(argv[4]);
    params.down_channel = argv[5];
    params.version = atoi(argv[6]);
    params.seq = atoi(argv[7]);
    params.cmd_id = atoi(argv[8]);

    BTqueryClient query_client;
    int ret_encode = query_client.encode_query_params(params, str_require);
    if(ret_encode)
    {
        printf("encode_query_params error:%d\n", ret_encode);
        return -1;
    }
    std::string key_temp = query_client.get_user_data_aes_key();

    TransferTool ttool;
    ttool.version_ = atoi(argv[6]);
    ttool.seq_ = atoi(argv[7]);
    ttool.compress_flag_ = 0;
    ttool.padding_len_ = 0;
    ttool.cmd_id_ = atoi(argv[8]);
    ttool.result_ = atoi(argv[9]);
    ttool.compress_ = 0;

    if(0 == atoi(argv[9]))
    {
        if(std::string(argv[1]).empty())
        {
            return -1;
        }

        FILE *pFile_read = fopen(argv[1], "r");
        if(!pFile_read)
        {
            printf("not found the bt file!\n");
            return -1;
        }

        fseek(pFile_read, 0, SEEK_END);
        int len = ftell(pFile_read);
        char *pBuf = new char[len];

        rewind(pFile_read);

        fread(pBuf, 1, len, pFile_read);
        fclose(pFile_read);

        ttool.bt_file_ = std::string(pBuf, len);
    }

    std::string buf_for_encode = ttool.trans_struct_to_buf();
    InterfaceCrypto inter_crypto_rel;
    inter_crypto_rel.str_aes_key_ = key_temp;
    std::string encoded_buf = inter_crypto_rel.interface_encode((char*)buf_for_encode.data(), buf_for_encode.size());
    if(encoded_buf.empty())
    {
        printf("btpool encode failed...\n");
        return -1;
    }

    //解析返回结果
    str_response = encoded_buf;
    int ret_decode = query_client.decode_query_result(str_response, response);
    if(ret_decode)
    {
        printf("decode_query_result error:%d\n", ret_decode);
        return -1;
    }

    //debug
    printf("ver:%u seq:%u len:%u cliver:%u compress_flag:%d padd:%u cmd_id:%d result:%d compress:%d file_size:%u\n",
    response.version, response.seq, response.len, response.client_ver, response.compress_flag, response.padding_len,
    response.cmd_id, response.result, response.compress, response.bt_file.size());

    //处理response
    if(0 == response.result)
    {
        printf("found\n");
        //保存
        FILE *pFile_save = fopen("get.torrent", "w");
        if(!pFile_save)
        {
            return -1;
        }

        fwrite (response.bt_file.c_str(), 1, response.bt_file.size(), pFile_save);
        fclose(pFile_save);
    }
    else
    {
        printf("not found\n");
    }

    return 0;
}
