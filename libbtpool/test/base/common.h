/*
 * ================================================================================================================
 *    Description:  BT Pool Server Common Head File
 *        Version:  1.0
 *        Created:  08/26/2016
 *         Author:  Melo
 * Modify History: 1. Created by Melo
 * ================================================================================================================
 */

#ifndef BTPOOL_P2SPROTOCOL_COMMON_H_
#define BTPOOL_P2SPROTOCOL_COMMON_H_

#include <string>
#include <time.h>
#include <vector>
#include <cstring>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <sys/mman.h>
#include <openssl/aes.h>
#include <openssl/rsa.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include "protocol_rsa_aes.h"
#include "md5.h"

namespace BTPOOL
{

enum BTPOOL_ERR
{
    /*---sys---*/
    BTPOOL_ERR_SYS = -1,
    BTPOOL_ERR_SUCC = 0,

    /*---decrypt_req---*/
    BTPOOL_ERR_ARG = 100,
    BTPOOL_ERR_GET_MAGIC_NUM = 101,
    BTPOOL_ERR_GET_VERSION = 102,
    BTPOOL_ERR_GET_RSALEN = 103,
    BTPOOL_ERR_GET_DUP_RSA_PRIKEY = 104,
    BTPOOL_ERR_GET_AESKEY = 105,
    BTPOOL_ERR_DECRYPT_LIMIT_LEN = 106,

    BTPOOL_ERR_GET_BODY_VER = 107,
    BTPOOL_ERR_GET_BODY_SEQ = 108,
    BTPOOL_ERR_GET_BODY_LEN = 109,
    BTPOOL_ERR_GET_BODY_CV = 110,
    BTPOOL_ERR_GET_BODY_CF = 111,
    BTPOOL_ERR_GET_BODY_PAD_LEN = 112,
    BTPOOL_ERR_GET_BODY_PV = 113,
    BTPOOL_ERR_GET_BODY_PID = 114,
    BTPOOL_ERR_GET_BODY_PARTID = 115,
    BTPOOL_ERR_GET_BODY_THUNDER_VER = 116,
    BTPOOL_ERR_GET_BODY_DOWN_CHL = 117,
    BTPOOL_ERR_GET_BODY_CMDID = 118,
    BTPOOL_ERR_GET_BODY_INFOID = 119,
    BTPOOL_ERR_GET_BODY_SC = 120,
    BTPOOL_ERR_GET_BODY_PEERID = 121,

    BTPOOL_ERR_AES_DECRYPT = 200,
    BTPOOL_ERR_TRANS_BUF2STRUCT = 201,

    /*---encrypt_resp---*/
    BTPOOL_ERR_TRANS_STRUCT2BUF = 300,
    BTPOOL_ERR_AES_ENCRYPT = 301,

};

}
#endif


