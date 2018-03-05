#ifndef BTPOOL_P2SPROTOCOL_QUERY_INTERFACE_H_
#define BTPOOL_P2SPROTOCOL_QUERY_INTERFACE_H_
#include "query_req.h"
#include "query_resp.h"
#include "common.h"

namespace BTPOOL
{

class InterfaceCrypto
{
public:
    InterfaceCrypto():str_aes_key_(""){};
    ~InterfaceCrypto(){};

public:

    /*将下载库查询请求解密成QueryStruct结构
     * @throw  无
     * @param(in)  body_buf:接收到下载库查询请求数据
     * @param(in)  buf_len:查询请求数据长度
     * @param(out)  query_struct_out:解密后输出QueryStruct结构指针，由外部传入
     * @return 参见 enum BTPOOL_ERR定义
     * @desc 所以参数对应地址空间由调用者维护
     */
    std::string interface_decode(char *body_buf, int32_t len);

    /* 处理查询后将查询记过QueryStruct加密成buf返回给下载库
     * @throw  无
     * @param(in) query_struct_in:查询结构
     * @param  body_buf_out:加密后输出地址,给进NULL即可，由内部申请内存，调用者使用后delete[]释放
     * @return 参见 enum BTPOOL_ERR定义
     */
    std::string interface_encode(char *body_buf, int32_t len);


public:
    std::string str_aes_key_;
};


}
#endif
