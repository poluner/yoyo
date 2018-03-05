/*
 * ================================================================================================================
 *    Description:  BT Pool Query Server Com Common Srouce file
 *        Version:  1.0
 *        Created:  09/28/2016
 *         Author:  Melo
 * Modify History: 1. Created by Melo
 * ================================================================================================================
 */

#include "btquery_client_common.h"
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

int ParamTool::gen_SHA1_infoid(QueryParams &params)
{
    if (params.infohash.size() % 2 != 0)
    {
        return EC_INFOID_SIZE;
    }

    std::string str_bin;
    str_bin.resize(params.infohash.size() / 2);
    for (size_t i = 0; i < str_bin.size(); i++)
    {
        uint8_t c_temp = 0;
        for (size_t j = 0; j < 2; j++)
        {
            char c_cur = params.infohash[2 * i + j];
            if (c_cur >= '0' && c_cur <= '9')
            {
                c_temp = (c_temp << 4) + (c_cur - '0');
            }
            else if (c_cur >= 'a' && c_cur <= 'f')
            {
                c_temp = (c_temp << 4) + (c_cur - 'a' + 10);
            }
            else if (c_cur >= 'A' && c_cur <= 'F')
            {
                c_temp = (c_temp << 4) + (c_cur - 'A' + 10);
            }
            else
            {
                return EC_HEX_INFOID_UNKNOW;
            }
        }
        str_bin[i] = c_temp;
    }

    params.sha1_infohash = str_bin;
    return EC_SUCC;
}

const std::string ParamTool::build_reserve6x(QueryParams &params, size_t &len, bool force)
{
    char reserve6x[MAX_VERSION_LEN * 3 + MAX_APP_NAME_LEN + 24] = { 0 };
    uint32_t reserve6x_length = 0;
    char product_version[MAX_VERSION_LEN] = {0};
    char thunder_version[MAX_VERSION_LEN] = {0};
    char partner_id[MAX_APP_NAME_LEN] = { 0 };

    if(reserve6x_length > 0 && !force)
    {
        len = reserve6x_length + 4;
        return std::string(reserve6x, len);
    }

    char* write_walk_ptr = reserve6x;
    int32_t size_remain = sizeof(reserve6x);

    sd_strncpy(product_version, XL_THUNDER_SDK_VERSION, sd_strlen(XL_THUNDER_SDK_VERSION));
    sd_strncpy(thunder_version, XL_THUNDER_SDK_VERSION, sd_strlen(XL_THUNDER_SDK_VERSION));
    sd_u32_to_str(params.appid, partner_id, MAX_APP_NAME_LEN);

    uint32_t productId = XL_THUNDER_SDK_PRODUCT_ID;
    int32_t product_version_len = sd_strlen(product_version) ;
    int32_t thunder_version_len = sd_strlen(thunder_version);
    int32_t partner_id_len = sd_strlen(partner_id);
    int32_t down_channel_len = params.down_channel.size();

    // 20 is shal b_infoid len
    reserve6x_length = 20 + product_version_len + thunder_version_len +
                      partner_id_len+ down_channel_len;

    sd_set_int32_to_lt(&write_walk_ptr, &size_remain, reserve6x_length);
    sd_set_int32_to_lt(&write_walk_ptr, &size_remain, product_version_len);
    sd_set_bytes(&write_walk_ptr, &size_remain, product_version, product_version_len);

    sd_set_int32_to_lt(&write_walk_ptr, &size_remain, productId);
    sd_set_int32_to_lt(&write_walk_ptr, &size_remain, partner_id_len);
    sd_set_bytes(&write_walk_ptr, &size_remain, partner_id, partner_id_len);

    sd_set_int32_to_lt(&write_walk_ptr, &size_remain, thunder_version_len);
    sd_set_bytes(&write_walk_ptr, &size_remain, thunder_version, thunder_version_len);

    sd_set_int32_to_lt(&write_walk_ptr, &size_remain, down_channel_len);
    sd_set_bytes(&write_walk_ptr, &size_remain, params.down_channel.c_str(), down_channel_len);
    sd_assert(size_remain > 0);
    len = reserve6x_length + 4;

    return std::string(reserve6x, len);
}

std::string ParamTool::set_query_params(QueryParams &params, uint32_t &plain_pack_len)
{
    size_t rlen = 0;
    std::string reserve6x = build_reserve6x(params, rlen);
    const char* reserve = reserve6x.c_str();

    uint32_t cmd_len = (uint32_t)(
            4
          + 2
          + rlen
          + sizeof(uint16_t)
          + params.sha1_infohash.size() + 4
          + 4
          + params.peerid.size() + 4
          );

    char *plain_pack = new char[cmd_len + HUB_CMD_HEADER_LEN];
    plain_pack_len = cmd_len + HUB_CMD_HEADER_LEN;
    PackageHelper wh(plain_pack, (int32_t)plain_pack_len);
    wh.PushValue(params.version);
    wh.PushValue(params.seq);
    wh.PushValue((uint32_t)cmd_len);
    wh.PushValue((uint32_t)0);
    wh.PushValue((uint16_t)0);
    wh.PushBytes(reserve,(int32_t)rlen);
    wh.PushValue(params.cmd_id);
    wh.PushString(params.sha1_infohash);
    wh.PushValue(params.support_compress);
    wh.PushString(params.peerid);
    if(wh.ErrorOccur())
    {
        delete[] plain_pack;
        return "";
    }

    sd_assert(wh.RemainSize() == 0);
    std::string params_buf(plain_pack, plain_pack_len);
    delete[] plain_pack;

    return params_buf;
}
}


