#include "btquery_client.h"
#include "interface_query_crypto.h"
#include "query_struct_tools.h"
using namespace BTQUERY;
using namespace BTPOOL;

int main(int argc, char* argv[])
{
    if(8 != argc)
    {
        printf("Usage: ./test 8CF9317C7ECCBB657F5C35900DDA8FAF779F8BFA aaaaaaaaaaaaaaaa 1000 miui 62 10001 3059\n");
        return 1;
    }
    printf("input:\ninfoid:%s \npeerid:%s \nappid:%s \ndownload_channel:%s \nversion:%s \nseq:%s \ncmd_id:%s\n",
        argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]);
    //base
    QueryParams params;
    std::string str_require;

    params.infohash = argv[1];
    params.peerid = argv[2];
    params.appid = atoi(argv[3]);
    params.down_channel = argv[4];
    params.version = atoi(argv[5]);
    params.seq = atoi(argv[6]);
    params.cmd_id = atoi(argv[7]);

    BTqueryClient query_client;
    int ret_encode = query_client.encode_query_params(params, str_require);
    if(ret_encode)
    {
        printf("encode_query_params error:%d\n", ret_encode);
        return -1;
    }

    InterfaceCrypto inter_crypto;
    std::string decode_step_1_buf = inter_crypto.interface_decode((char*)str_require.c_str(), str_require.size());
    if(decode_step_1_buf.empty())
    {
        printf("btpool decode failed..\n");
        return -1;
    }

    TransferTool ttool;
    ttool.aes_key_ = inter_crypto.str_aes_key_;
    int ret_trans = ttool.trans_buf_to_struct((char*)decode_step_1_buf.data(), decode_step_1_buf.size());
    if(ret_trans)
    {
        printf("trans to struct failed  ret_trans:%d...\n", ret_trans);
        return -1;
    }
    /*
    printf("ver:%d \nseq:%d \nlen:%d \nclient_version_:%u \ncompress_flag_:%d \npadding_len_:%u \nproduct_version_:%s \nproduct_id_:%u \npartner_id_:%s \nthunder_version_:%s \ndownload_channel_:%s \ncmd_id:%d  \ninfo_id_:%s \nsupport_compress_:%d \npeer_id_:%s \nresult_:%d \ncompress_:%d\n",
        ttool.version_, ttool.seq_, ttool.length_, ttool.client_version_, ttool.compress_flag_, ttool.padding_len_, ttool.product_version_.c_str(), ttool.product_id_,
        ttool.partner_id_.c_str(), ttool.thunder_version_.c_str(),
        ttool.download_channel_.c_str(), ttool.cmd_id_, ttool.info_id_.c_str(), ttool.support_compress_, ttool.peer_id_.c_str(), ttool.result_, ttool.compress_);
    */
    printf("\n\n\nbtpool recv:"
         "\ninfoid:%s \nperrid:%s \npartner_id_:%s \ndownload_channel_:%s \nver:%d \nseq:%d \ncmd_id:%d \n\n  \nlen:%d \nclient_version_:%u"
         "\ncompress_flag_:%d \npadding_len_:%u \nproduct_version_:%s \nproduct_id_:%u"
         "\nthunder_version_:%s \nsupport_compress_:%d \nresult_:%d \ncompress_:%d\n",
        ttool.info_id_.c_str(),  ttool.peer_id_.c_str(), ttool.partner_id_.c_str(), ttool.download_channel_.c_str(), ttool.version_, ttool.seq_, ttool.cmd_id_, ttool.length_, ttool.client_version_,
        ttool.compress_flag_, ttool.padding_len_, ttool.product_version_.c_str(), ttool.product_id_,
        ttool.thunder_version_.c_str(),ttool.support_compress_, ttool.result_, ttool.compress_);

    return 0;
}
