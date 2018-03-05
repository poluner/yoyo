
#ifndef _XL_THUNDER_DEFINE_H__
#define _XL_THUNDER_DEFINE_H__
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include "version.h"

typedef  int    BOOL;
// ...  vod点播库代码移植引入 ... 开始
#define VOD_MAX_P2P_CONNECTION_COUNT 200
#define FAILED (-1)
#define VodNewList_pop list_pop
#define VodNewList_push list_push
#define VodNewList_erase list_erase
#define VOD_QUEUE QUEUE
#define ASSERT sd_assert
#define	SAFE_DELETE(p)	{if(p) {sd_free(p);(p) = NULL;}}
#define VodNewQueue_init queue_init
#define VOD_CHECK_VALUE CHECK_VALUE
#define VodNewQueue_reserved queue_reserved
#define VodNewQueue_uninit queue_uninit
#define VodNewQueue_pop queue_pop
#define VodNewQueue_push_without_alloc queue_push_without_alloc

#define VodNewLocalInfo_get_peer_id() GlobalInfo::GetInstance()->GetPeeridPtr()
#define VodNewList_size list_size
#define setnonblock sd_setnonblock

// ...  vod点播库代码移植引入 ... 结束

#define MIN_LIST_MEMORY	(2048)
#define EM_MIN_LIST_MEMORY 128

#define EM_MIN_SET_MEMORY 128
#define EM_MIN_MAP_MEMORY 64

#define EM_MIN_MSG_COUNT 64
#define EM_MIN_QUEUE_MEMORY 64
#define EM_MIN_SETTINGS_ITEM_MEMORY 16
#define EM_MIN_TIMER_COUNT 8

#define MIN_TREE_NODE_MEMORY 32
#define MIN_NODE_NAME_MEMORY 32
#define MIN_TREE_MEMORY 3

#define MIN_VOD_TASK_MEMORY 1

#define MIN_TASK_INFO_MEMORY 16
#define MIN_DL_TASK_MEMORY 16
#define MIN_DT_EIGENVALUE_MEMORY 16


#define MIN_LIST_MEMORY	(2048)
#define MIN_MAP_MEMORY	(1024)
#define MIN_SET_MEMORY	(1024 + MIN_MAP_MEMORY)

#define SET_ELEMENT_NOT_FOUND (9928)
#define MIN_QUEUE_MEMORY	(256)
#ifndef NULL
#define NULL	((void*)(0))
#endif

#ifndef FALSE
#define FALSE (0)
#endif

#ifndef TRUE
#define TRUE (1)
#endif

#ifndef CXX_SAFE_DELETE
#define CXX_SAFE_DELETE(p) {if(p) {delete (p);(p) = NULL;}}
#endif

#define NOT_USED(x)     (void)(x)

#define CID_SIZE (20)
#define MAX_U32 (0xFFFFFFFF)
#define SD_ERROR   (-1)

#define MAX_TD_CFG_SUFFIX_LEN (8)

#define MAX_FILE_NAME_LEN               (256)  /// 最长的 文件/目录 名的长度
#define MAX_FULL_PATH_LEN               (1024) /// 最长的 全路径（包括文件名和路径）的总长度
#define MAX_CFG_LINE_LEN                (1024)
#define MAX_CFG_DOMAIN_LEN              (256)
#define MAX_CFG_NAME_LEN                (256)
#define MAX_CFG_VALUE_LEN               (256)

#define MAX_USER_NAME_LEN 64
#define MAX_PASSWORD_LEN 64
#define MAX_HOST_NAME_LEN 128
#define MAX_SUFFIX_LEN 16

#define MIN_FREE_DISK_SIZE (100*1024)
#define MIN_SETTINGS_ITEM_MEMORY 128

#define MAX_APP_KEY_LEN 100
#define MAX_APP_NAME_LEN 100
#define MAX_APP_VERSION_LEN 100

#define HTTP_DEFAULT_PORT 80
#define HTTPS_DEFAULT_PORT 443
#define FTP_DEFAULT_PORT 21

#include <stdlib.h>
#define S_LOG_CONFIG_FILE "slog.config"

#define MIN_TIMER_COUNT             (256)
#define WAIT_INFINITE               (0xFFFFFFFF)


#define GUID_SIZE                   (40)
#define PEER_ID_SIZE                (16)
#define MAX_VERSION_LEN             (64)
#define MAX_SYSTEM_VERSION_LEN      (256)
#define MAX_PARTNER_ID_LEN          (16)
#define MAX_LICENSE_LEN             (256)
#define MAX_OS_LEN                  (64)

#define MAX_CMD_LEN                 (50 * 1024)
#define HUB_CMD_HEADER_LEN          (12)
#define HUB_ENCODE_PADDING_LEN      (16)

#define MAX_URL_LEN                 (2048)

#define MAX_SERVER_IP_LEN           (32)  //理论上16足够，但是稳妥起见，多提供一些
#define MAX_IN6ADDR_STR_LEN         (64)

#define MAX_IN6ADDR_STR_LEN         (64)

typedef enum tagHUB_TYPE
{
    SHUB,
    PHUB,
    PARTNER_CDN,
    TRACKER,
    BT_HUB,
    BT_POOL,
    EMULE_HUB,
    BT_TRACKER,
    BT_DHT,
    EMULE_KAD,
    CDN_MANAGER,
    VIP_HUB,
    KANKAN_CDN_MANAGER,
    CONFIG_HUB,
    DPHUB_ROOT,
    DPHUB_NODE,
    EMULE_TRACKER,
    NORMAL_CDN_MANAGER,
    VIP_URL_HUB
} HUB_TYPE;

enum NetWorkType
{
    NWT_None = 0,              //断网
    NWT_Phone = 1,             // 手机网络
    NWT_Mobile2G = 2,         //2G
    NWT_Mobile3G = 3,         //3G
    NWT_Mobile4G = 4,         //4G
    NWT_Unknown = 5,          //未知
    NWT_Wifi = 9,             //wifi
};

enum NetWorkCarrier
{
    NWC_Unknow,   // 未知
    NWC_CMCC,     // 移动
    NWC_CU,       // 联通
    NWC_CT,       // 电信
};

enum IndexQueryByWhat
{
    kIdxByWhat_Url,     // = 0,
    kIdxByWhat_CID,     // = 1,
    kIdxByWhat_BCID,    // = 2
    kIdxByWhat_Other,
    CountOfIdxByWhat
};

enum QueryIndexFailReason
{
    //QIFR_NoneIndex = 0,
    QIFR_NetworkError = 1,
    QIFR_HubNoRecordByUrl,
    QIFR_HubNoRecordByHubCid,
    
    QIFR_HubNoRecord,    //QIFR_HubNoRecord 即 QIFR_NoneIndex
    QIFR_ConflictNotAccept, //冲突不一致未被采纳
};

enum QueryIndexError
{
    kReturnDataErrorByUrl = 1,   // url 查询返回数据错误
    kRetrunDataErrorByCID,   // cid 查询返回数据错误
    kQueryErrorByUrl,        // url 查询网络错误
    kQueryErrorByCID         // cid 查询网络错误
};


#define FILE_ID_SIZE                    (16)
#define AICH_HASH_SIZE                  (20)
#define XL_THUNDER_SDK_PRODUCT_ID       (0x80000001)

enum ResComeFrom
{
    RCF_UNKNOW          = 0,
    RCF_ORIGIN          = 1,        /// 原始资源
    RCF_MIRROR          = 2,        /// 候选资源
    //RCF_SIMILAR         = 4,      /// 相似资源
    RCF_FILE            = 8,        /// 来自配置文件读取
    RCF_PHUB            = 16,       /// 普通phub资源
    RCF_DPHUB           = 32,       /// dphub资源
    RCF_TRACKER         = 64,       /// tracker边下边传
    RCF_DCDN            = 128,       /// dcdn资源，挖矿机
    RCF_HIGH            = 256,       /// 插入的高速资源, p2p_vip资源
    RCF_OFFLINE         = 512,        /// 离线资源, Cdn资源, p2s_vip资源
    RCF_ANTIHACK        = 1024,      /// 反劫持纠错CDN资源
    RCF_ANTIHTTPS       = 2048,      /// 反劫持纠错HTTPS资源
    RCF_EXTP            = 4096,      /// 扩展协议 目前就是agip
    RCF_TORRENT         = 8192       // 种子资源
};

/// download play
#define SETTING_DEFAULT_NOT_CLOSE_PIPE_LAST_TIME            (1000)  /// 单位毫秒
#define SETTING_DEFAULT_NOT_CLOSE_PIPE_WILL_DOWNLOAD_TIME   (1000)  /// 单位毫秒
#define SETTING_DEFAULT_SESSION_PER_DOWNLOAD_SIZE           (64 * 1024)  /// 单位 字节
#define	SETTING_DEFAULT_PRIOR_DOWNLOAD_CACHE_SiZE           (SETTING_DEFAULT_SESSION_PER_DOWNLOAD_SIZE * 3)

typedef enum _PermissionLevel
{
	PL_REMAIN = 0,	//预留等级
	PL_LOW = 1,
	PL_MIDDLE = 2,
	PL_HIGH = 3,
} PermissionLevel;

#endif


