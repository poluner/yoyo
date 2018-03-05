/*
 * ================================================================================================================
 *    Description:  BT Pool Server Common Struct CPP Source File
 *        Version:  1.0
 *        Created:  08/26/2016
 *         Author:  Melo
 * Modify History: 1. Created by Melo
 * ================================================================================================================
 */
#include "query_struct_tools.h"

namespace BTPOOL
{

uint32_t TransferTool::get_encode_param_len()
{
    uint32_t len = 4 * sizeof(uint32_t) + sizeof(short) + sizeof(uint32_t)
            + sizeof(short) + 2 * sizeof(int32_t) + sizeof(unsigned int) + bt_file_.size();

    return len;
}


std::string TransferTool::str_to_hex(const char *s, int str_len)
{
    if(s == NULL || 0 == str_len)
    {
        return "";
    }
    int len = str_len * 2;
    char *out_ptr = new char[len + 1];
    memset(out_ptr, 0, len + 1);
    std::string out_str = "";
    const char *hex = "0123456789ABCDEF";
    uint32_t i = 0;
    uint32_t c = 0;
    for(uint32_t j = 0; j < (uint32_t)str_len; j++)
    {
        c = (uint32_t)s[j];
        out_ptr[i++] = hex[(c & 0xf0) >> 4];
        out_ptr[i++] = hex[(c & 0x0f)];
    }

    out_ptr[i] = '\0';
    out_str.append(out_ptr, len);
    delete[] out_ptr;
    out_ptr = NULL;
    return out_str;
}


int TransferTool::trans_buf_to_struct(char *buff, int len)
{
    if(NULL == buff || 0 == len)
    {
        return BTPOOL_ERR_ARG;
    }

    int decode_outlen = 0;
    char *decode_out = new char[1024];
    memset(decode_out, 0, 1024);
    memcpy(decode_out, buff, 3 * sizeof(uint32_t));

    int decode_outlen_tmp= 0;
    char *decode_out_tmp = new char[1024];
    memset(decode_out_tmp, 0, 1024);

    //get md5 key of user data
    unsigned char body_aes_key_buf[16];
    memset(body_aes_key_buf, 0, 16);
    md5_data((unsigned char*)buff, 8, body_aes_key_buf);

    AES aes;
    aes.setDecryptKey(body_aes_key_buf);
    if(false == aes.decrypt((unsigned char*)buff + 3 * sizeof(uint32_t), len - 3 * sizeof(uint32_t), (unsigned char*)decode_out_tmp, &decode_outlen_tmp) || decode_outlen_tmp < 0)
    {
        delete[] decode_out_tmp;
        decode_out_tmp = NULL;
        delete[] decode_out;
        decode_out = NULL;
        return BTPOOL_ERR_AES_DECRYPT;
    }
    memcpy(decode_out + 3 * sizeof(uint32_t), decode_out_tmp, decode_outlen_tmp);
    delete[] decode_out_tmp;
    decode_out_tmp = NULL;

    // 还原长度字段 |uint32|uint32|uint32 len|...inter data decoded by step 2...|
    decode_outlen = 3 * sizeof(uint32_t) + decode_outlen_tmp;
    *((uint32_t*)(decode_out + 2 * sizeof(uint32_t))) = decode_outlen_tmp;

    //host_byte_buffer transfer
    bool ret = true;
    HostByteBuffer decoder(decode_out, decode_outlen, true);

    ret = decoder.getUInt(version_);
    if (!ret)
    {
        return BTPOOL_ERR_GET_BODY_VER;
    }

    ret = decoder.getUInt(seq_);
    if (!ret)
    {
        return BTPOOL_ERR_GET_BODY_SEQ;
    }

    ret = decoder.getUInt(length_);
    if (!ret)
    {
        return BTPOOL_ERR_GET_BODY_LEN;
    }

    ret = decoder.getUInt(client_version_);
    if (!ret)
    {
        return BTPOOL_ERR_GET_BODY_CV;
    }

    ret = decoder.getShort(compress_flag_);
    if (!ret)
    {
        return BTPOOL_ERR_GET_BODY_CF;
    }

    ret = decoder.getUInt(padding_len_);
    if (!ret)
    {
        return BTPOOL_ERR_GET_BODY_PAD_LEN;
    }

    ret = decoder.getString(product_version_);
    if (!ret)
    {
        return BTPOOL_ERR_GET_BODY_PV;
    }

    ret = decoder.getInt((int&)product_id_);
    if (!ret)
    {
        return BTPOOL_ERR_GET_BODY_PID;
    }

    ret = decoder.getString(partner_id_);
    if (!ret)
    {
        return BTPOOL_ERR_GET_BODY_PARTID;
    }

    ret = decoder.getString(thunder_version_);
    if (!ret)
    {
        return BTPOOL_ERR_GET_BODY_THUNDER_VER;
    }

    ret = decoder.getString(download_channel_);
    if (!ret)
    {
        return BTPOOL_ERR_GET_BODY_DOWN_CHL;
    }

    ret = decoder.getShort(cmd_id_);
    if (!ret)
    {
        return BTPOOL_ERR_GET_BODY_CMDID;
    }

    std::string info_id = "";
    ret = decoder.getString(info_id);
    if (!ret)
    {
        return BTPOOL_ERR_GET_BODY_INFOID;
    }

    info_id_ = str_to_hex((const char *)(info_id.c_str()), info_id.length());

    ret = decoder.getInt(support_compress_);
    if (!ret)
    {
        return BTPOOL_ERR_GET_BODY_SC;
    }

    ret = decoder.getString(peer_id_);
    if (!ret)
    {
        return BTPOOL_ERR_GET_BODY_PEERID;
    }

    return BTPOOL_ERR_SUCC;
}

std::string TransferTool::trans_struct_to_buf()
{
    int32_t buffer_len = get_encode_param_len();

    char *buffer = new char[buffer_len + 1];
    memset(buffer, 0, buffer_len + 1);

    HostByteBuffer encoder(buffer, buffer_len);

    encoder.putUInt(version_);
    encoder.putUInt(seq_);
    encoder.putUInt(length_);
    encoder.putUInt(client_version_);
    encoder.putShort(compress_flag_);
    encoder.putUInt(0);
    encoder.putShort((short)3060);//QUERY_MAGNET_FILE_RESP_CMD
    encoder.putInt(result_);
    encoder.putInt(compress_);
    encoder.putBytes(bt_file_.c_str(), bt_file_.size());

    std::string buf_for_encode = std::string(buffer, buffer_len);

    delete[] buffer;
    buffer = NULL;
    return buf_for_encode;
}


}
