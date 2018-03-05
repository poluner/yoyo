#ifndef SD_ERRCODE_H_00138F8F2E70_200806111932
#define SD_ERRCODE_H_00138F8F2E70_200806111932

#ifdef __cplusplus
extern "C"
{
#endif

#include "sd_assert.h"
#include "define.h"

#define SUCCESS                            (0)
#define FAILE                              (1)
#define SD_ERR_CODE_RESERVE  (0x0FFFFFFF)
#define HTTP_CANCEL    10


#ifdef _DEBUG
int32_t sd_check_value(int32_t errcode,const char * func, const char * file , int line);

#ifdef _ANDROID_LINUX
#define CHECK_VALUE(errcode)     {sd_check_value(errcode,__func__, __FILE__,  __LINE__); if(errcode != SUCCESS) { int r_errcode = errcode; if(r_errcode==SD_ERR_CODE_RESERVE) { r_errcode=-1;}    return r_errcode;}}
#else
#define CHECK_VALUE(errcode)     sd_check_value(errcode,__func__, __FILE__,  __LINE__)
#endif

#else
#define CHECK_VALUE(errcode)    { if(errcode != SUCCESS) { int r_errcode = errcode; if(r_errcode==SD_ERR_CODE_RESERVE) { r_errcode=-1;}    return r_errcode;}}

#endif

int32_t set_critical_error(int32_t errcode);
int32_t get_critical_error(void);

#define CHECK_CRITICAL_ERROR  {CHECK_VALUE(get_critical_error());}


/* preserved for os */


/* use defined: */

/***************************************************************************/
/* common */
/***************************************************************************/
#define COMMON_ERRCODE_BASE                 (111024)
#define TARGET_THREAD_STOPING               (111025)
#define OUT_OF_MEMORY                       (111026)
//#define INVALID_MEMORY                    (111027)
//#define INVALID_SSLAB_SIZE                (111028)
//#define TOO_FEW_MPAGE                     (111029)
//#define INVALID_HANDLER                   (111030)
#define TASK_USE_TOO_MUCH_MEM               (111031)
#define OUT_OF_FIXED_MEMORY                 (111032)
#define    QUEUE_NO_ROOM                    (111033)
//#define LOCK_SHAREBRD_FAILED              (111034)
#define MAP_UNINIT                          (111035)
#define MAP_DUPLICATE_KEY                   (111036)
#define MAP_KEY_NOT_FOUND                   (111037)
#define INVALID_ITERATOR                    (111038)
#define BUFFER_OVERFLOW                     (111039)
//#define    BITMAP_BITSIZE_OVERFLOW        (111040)
#define INVALID_ARGUMENT                    (111041)
//#define NOT_FOUND_MAC_ADDR                (111042)
//#define GETTING_IMEI_ADDR                 (111043)
//#define NETWORK_NOT_READY                 (111044)
//#define USER_ABORT_NETWORK                (111045)
//#define NEED_RECONNECT_NETWORK            (111046)
//#define NETWORK_INITIATING                (111047)
#define URL_PARSER_ERROR                    (111046)
#define URL_IS_TOO_LONG                     (111047)
#define INVALID_SOCKET_DESCRIPTOR           (111048)
//#define ERROR_LOG_CONF_FILE               (111049)
#define ERROR_INVALID_INADDR                (111050)
//#define ERROR_INVALID_PEER_ID             (111051)
//#define ERROR_WAIT_NOTIFY                 (111052)
//#define ERROR_DATA_IS_WRITTING            (111053)
#define REDIRECT_TOO_MUCH                   (111181)

/***************************************************************************/
/***************************************************************************/
//#define ASYN_FRAME_ERRCODE_BASE           (111054)
//#define INVALID_OPERATION_TYPE            (111055)
//#define INVALID_MSG_HANDLER               (111056)
#define NOT_IMPLEMENT                       (111057)
//#define REACTOR_LOGIC_ERROR               (111058)
//#define REACTOR_LOGIC_ERROR_1             (111059)
//#define REACTOR_LOGIC_ERROR_2             (111060)
//#define REACTOR_LOGIC_ERROR_3             (111061)
//#define REACTOR_LOGIC_ERROR_4             (111062)
//#define REACTOR_LOGIC_ERROR_5             (111063)
//#define SOCKET_EPOLLERR                   (111064)
//#define SOCKET_CLOSED                     (111065)
//#define SOCKET_NOT_ENOUGH                 (111066)
//#define SOCKET_ERROR_FD                   (111067)
//#define SOCKET_UNKNOWN_ENCAP              (111068)
//#define END_OF_FILE                       (111069)
//#define BAD_POLL_EVENT                    (111070)
//#define NO_ROOM_OF_POLL                   (111071)
//#define BAD_POLL_ARUMENT                  (111072)
//#define NOT_FOUND_POLL_EVENT              (111073)
#define INVALID_TIMER_INDEX                 (111074)
//#define    TOO_MANY_EVENT                 (111075)
//#define    INVALID_EVENT_HANDLE           (111076)
#define    DNS_NO_SERVER                    (111077)
#define    DNS_INVALID_ADDR                 (111078)
//#define    DNS_NETWORK_EXCEPTION          (111079)
//#define    DNS_INVALID_REQUEST            (111080)

/***************************************************************************/
/*os-interface*/
/***************************************************************************/
//#define INTERFACE_ERRCODE_BASE            (111081)
//#define UNKNOWN_OPENING_FILE_ERR          (111082)
#define BAD_DIR_PATH                        (111083)
#define FILE_CANNOT_TRUNCATE                (111084)
#define INSUFFICIENT_DISK_SPACE             (111085)
#define FILE_TOO_BIG                        (111086)
//#define INVALID_CUSTOMED_INTERFACE_IDX    (111087)
//#define INVALID_CUSTOMED_INTERFACE_PTR    (111088)
//#define ALREADY_ET_INIT                   (111089)
//#define IS_NOT_INITIALIZED                (111090)
//#define INVALID_SD_CARD                   (111091)
/***************************************************************************/
/*task_mgr*/
/***************************************************************************/
//#define TASK_MGR_ERRCODE_BASE             (111092)
//#define TM_ERR_UNKNOWN                    (111093)
//#define TM_ERR_TASK_MANAGER_EXIST         (111094)
//#define TM_ERR_FILE_NOT_EXIST             (111095)
//#define TM_ERR_CFG_FILE_NOT_EXIST         (111096)
//#define TM_ERR_INVALID_URL                (111097)
//#define TM_ERR_INVALID_FILE_PATH          (111098)
//#define TM_ERR_INVALID_FILE_NAME          (111099)
//#define TM_ERR_TASK_FULL                  (111100)
//#define TM_ERR_RUNNING_TASK_FULL          (111101)
//#define TM_ERR_INVALID_TCID               (111102)
//#define TM_ERR_INVALID_FILE_SIZE          (111103)
//#define TM_ERR_INVALID_TASK_ID            (111104)
//#define TM_ERR_INVALID_DOWNLOAD_TASK      (111105)
//#define TM_ERR_TASK_IS_RUNNING            (111106)
//#define TM_ERR_TASK_NOT_RUNNING           (111107)
//#define TM_ERR_IVALID_GCID                (111108)
//#define TM_ERR_INVALID_PARAMETER          (111109)
//#define TM_ERR_NO_TASK                    (111110)
//#define TM_ERR_RES_QUERY_REQING           (111111)
//#define TM_ERR_BUFFER_NOT_ENOUGH          (111112)
//#define TM_ERR_TASK_TYPE                  (111113)
//#define TM_ERR_TASK_IS_NOT_READY          (111114)
//#define TM_ERR_LICENSE_REPORT_FAILED      (111115)
//#define TM_ERR_OPERATION_CLASH            (111116)
//#define TM_ERR_WAIT_FOR_SIGNAL            (111117)




/***************************************************************************/
/*dispatcher*/
/***************************************************************************/
#define DISPATCHER_ERRCODE_BASE            (111118)


/***************************************************************************/
/*data_mgr*/
/***************************************************************************/
#define DATA_MGR_ERRCODE_BASE               (111119)
#define FILE_PATH_TOO_LONG                  (111120)
//#define ALLOC_INVALID_SIZE                  (111120)
//#define DATA_BUFFER_IS_FULL                 (111121)
//#define BLOCK_NO_INVALID                    (111122)
//#define CHECK_DATA_BUFFER_NOT_ENOUGH        (111123)
//#define BCID_CHECK_FAIL                     (111124)
//#define BCID_ONCE_CHECT_TOO_MUCH            (111125)
//#define SD_INVALID_FILE_SIZE                (111130)
//#define CLOSE_FILE_ERROR                    (111125)
#define FILE_CFG_NOT_SUPPORT                (111123)
#define FILE_CFG_INFO_CONFLICT              (111124)
#define FILE_CFG_URL_CHANGE                 (111125)
#define READ_FILE_ERR                       (111126)
#define WRITE_FILE_ERR                      (111127)
#define OPEN_FILE_ERR                       (111128)
#define FILE_CFG_TRY_FIX                    (111129)
#define FILE_CFG_ERASE_ERROR                (111130)
#define FILE_CFG_MAGIC_ERROR                (111131)
#define FILE_CFG_READ_ERROR                 (111132)
#define FILE_CFG_WRITE_ERROR                (111133)
#define FILE_CFG_READ_HEADER_ERROR          (111134)
#define FILE_CFG_RESOLVE_ERROR              (111135)
#define TASK_FAILURE_NO_DATA_PIPE           (111136)
#define TASK_NO_FILE_NAME                   (111137)
//#define CANNOT_GET_FILE_NAME                (111138)
#define CREATE_FILE_FAIL                    (111139)
#define OPEN_OLD_FILE_FAIL                  (111140)
#define FILE_SIZE_NOT_BELIEVE               (111141)  //只有当切换到认证网络后，续传资源被重定向到WIFI认证页面时，才对上层返回该错误码
#define FILE_SIZE_TOO_SMALL                 (111142)
#define FILE_NOT_EXIST                      (111143)
#define FILE_INVALID_PARA                   (111144)
#define FILE_CREATING                       (111145)
#define FIL_INFO_INVALID_DATA               (111146)
#define FIL_INFO_RECVED_DATA                (111147)
#define TASK_NO_INDEX_NO_ORIGIN             (111148)
#define TASK_ORIGIN_NONEXISTENCE            (111149)
#define SET_SHUB_FILESIZE_ERR               (111150)
#define TASK_FAILURE_RESTRICTION            (111151)

/****************************************************************************/
/*conf-file mgr*/
/****************************************************************************/
#define CONF_MGR_ERRCODE_BASE               (111159)
#define SETTINGS_ERR_UNKNOWN                (111160)
#define SETTINGS_ERR_INVALID_FILE_NAME      (111161)
#define SETTINGS_ERR_CFG_FILE_NOT_EXIST     (111162)
#define SETTINGS_ERR_INVALID_LINE           (111163)
#define SETTINGS_ERR_INVALID_ITEM_NAME      (111164)
#define SETTINGS_ERR_INVALID_ITEM_VALUE     (111165)
#define SETTINGS_ERR_LIST_EMPTY             (111166)
#define SETTINGS_ERR_ITEM_NOT_FOUND         (111167)


/***************************************************************************/
/*net-reactor*/
/***************************************************************************/
#define NET_REACTOR_ERRCODE_BASE            (111168)
#define NET_CONNECT_SSL_ERR                 (111169)
#define NET_BROKEN_PIPE                     (111170)
#define NET_CONNECTION_REFUSED              (111171)
#define NET_SSL_GET_FD_ERROR                (111172)
#define NET_OP_CANCEL                       (111173)
#define NET_UNKNOWN_ERROR                   (111174)
#define NET_NORMAL_CLOSE                    (111175)
#define TASK_FAIL_LONG_TIME_NO_RECV_DATA    (111176)
#define TASK_FILE_SIZE_TOO_LARGE            (111177)
#define TASK_RETRY_ALWAY_FAIL               (111178)


#define CORRECT_TIMES_TOO_MUCH              (111179)
#define CORRECT_CDN_ERROR                   (111180)

/**************************************************************************/
/*asyn-file*/
/***************************************************************************/
#define ASYN_FILE_E_BASE                     111300
#define ASYN_FILE_E_OP_NONE               (ASYN_FILE_E_BASE+1)
#define ASYN_FILE_E_OP_BUSY               (ASYN_FILE_E_BASE+2)
#define ASYN_FILE_E_FILE_NOT_OPEN        (ASYN_FILE_E_BASE+3)
#define ASYN_FILE_E_FILE_REOPEN          (ASYN_FILE_E_BASE+4)
#define ASYN_FILE_E_EMPTY_FILE           (ASYN_FILE_E_BASE+5)
#define ASYN_FILE_E_FILE_SIZE_LESS       (ASYN_FILE_E_BASE+6)
#define ASYN_FILE_E_TOO_MUCH_DATA        (ASYN_FILE_E_BASE+7)
#define ASYN_FILE_E_FILE_CLOSING         (ASYN_FILE_E_BASE+8)
//#define    ERR_UDT_RESET                            (112000)    //17408
//#define    ERR_UDT_RECV_TIMEOUT                    (112100)
//#define    ERR_PTL_TRY_UDT_CONNECT                (112200)
//#define    ERR_PTL_PROTOCOL_VERSION                (112300)
#define    ERR_PTL_PROTOCOL_NOT_SUPPORT        (112400)
#define    ERR_PTL_PEER_OFFLINE                    (112500)
#define    ERR_PTL_GET_PEERSN_FAILED            (112600)

/***************************************************************************/
/*p2p_pipe*/
/***************************************************************************/
#define P2P_PIPE_ERRCODE_BASE			    (11300)
#define ERR_P2P_VERSION_NOT_SUPPORT		    (11301)	//11265
#define ERR_P2P_WAITING_CLOSE			    (11302)
#define ERR_P2P_HANDSHAKE_RESP_FAIL		    (11303)
#define ERR_P2P_REQUEST_RESP_FAIL		    (11304)
#define ERR_P2P_UPLOAD_OVER_MAX			    (11305)	//11271
#define ERR_P2P_REMOTE_UNKNOWN_MY_CMD	    (11306)
#define ERR_P2P_NOT_SUPPORT_UDT			    (11307)
#define ERR_P2P_BROKER_CONNECT			    (11308)
#define	ERR_P2P_INVALID_COMMAND			    (11309)
#define ERR_P2P_INVALID_PARAM			    (11310)
#define	ERR_P2P_CONNECT_FAILED			    (11311)
#define	ERR_P2P_CONNECT_UPLOAD_SLOW	        (11312) //11280
#define	ERR_P2P_ALLOC_MEM_ERR               (11313)
#define	ERR_P2P_SEND_HANDSHAKE              (11314)

/***************************************************************************/
/*bt,emule task*/
/***************************************************************************/

#define TASK_FAILURE_QUERY_EMULE_HUB_FAILED     (114001)
#define TASK_FAILURE_EMULE_NO_RECORD            (114101)
#define TASK_FAILURE_SUBTASK_FAILED             (114002)
#define TASK_FAILURE_CANNOT_START_SUBTASK       (114003)
#define TASK_FAILURE_QUERY_BT_HUB_FAILED        (114004)
#define TASK_FAILURE_PARSE_TORRENT_FAILED       (114005)
#define TASK_FAILURE_GET_TORRENT_FAILED         (114006)
#define TASK_FAILURE_SAVE_TORRENT_FAILED        (114007)
#define TASK_FAILURE_BTHUB_NO_RECORD            (114008)
#define TASK_FAILURE_ALL_SUBTASK_FAILED         (114009)
#define TASK_FAILURE_THEONLY_SUBTASK_FAILED     (114010)
#define TASK_FAILURE_PART_SUBTASK_FAILED        (114011)
#define TASK_FAILURE_QUERY_HUB_TYPE_ERR         (114012)

//#define ERR_TORRENT_GET_FAILED    TASK_FAILURE_GET_TORRENT_FAILED
//#define ERR_TORRENT_STILL_GETTING               (114010)
//#define ERR_TORRENT_PARSE_FAILED  TASK_FAILURE_PARSE_TORRENT_FAILED
//#define ERR_TORRENT_INFO_MALLOC                 (114011)
//#define ERR_TORRENT_INVALID                     (114012)
//#define ERR_TORRENT_INFOHASH_MISMATCH           (114013)
//#define TORRENT_PARSE_ERROR                     (114201)
//#define TORRENT_IMCOMPLETE                      (114202)
//#define THUNDER_URL_PARSE_ERROR                 (114301)

/**************************************************************************/
/*new  res_query */
/***************************************************************************/
#define RES_QUERY_E_BASE                     115000
/* more defines, please search and see query_return_value.h */

/**************************************************************************/
/*new  http_hub_client */
/***************************************************************************/
#define HTTP_HUB_CLIENT_E_BASE               115100
/* more defines, please search and see hub_http_connection.h */

/***************************************************************************/
/* IPv6 related */
/***************************************************************************/
#define IP6_ERRCODE_BASE                    (116000)
#define ERR_INVALID_ADDRESS_FAMILY          (116001)
#define IP6_INVALID_IN6ADDR                 (116002)
#define IP6_NOT_SUPPORT_SSL                 (116003)

/***************************************************************************/
/* pause task related */
/***************************************************************************/

#define PAUSE_TASK_WRITE_CFG_ERR            (117000)
#define PAUSE_TASK_WRITE_DATA_TIMEOUT       (117001)

/***************************************************************************/
/* download play related */
/***************************************************************************/
#define     ERR_DPLAY_ALL_SEND_COMPLETE                 (118000)    /// 所有的数据发送完毕了，服务器主动断开session
#define     ERR_DPLAY_CLIENT_ACTIVE_DISCONNECT          (118001)    /// 客户端主动断开了 session
#define     ERR_DPLAY_TASK_FINISH_DESTROY               (118002)    /// 任务结束后，session也需要关闭
#define     ERR_DPLAY_TASK_FINISH_CONTINUE              (118003)    /// 任务结束后，session不需要关闭
#define     ERR_DPLAY_TASK_FINISH_CANNT_DOWNLOAD        (118004)    /// 任务结束后，session不需要关闭
#define     ERR_DPLAY_NOT_FOUND                         (118005)
#define     ERR_DPLAY_SEND_FAILED                       (118300)
#define     ERR_DPLAY_SEND_RANGE_INVALID                (118301)
#define     ERR_DPLAY_HANDLE_DOWNLOAD_FAILED            (118302)
#define     ERR_DPLAY_UNKNOW_HTTP_METHOD                (118303)
#define     ERR_DPLAY_PLAY_FILE_NOT_EXIST               (118304)
#define     ERR_DPLAY_DO_DOWNLOAD_FAIL                  (118305)
#define     ERR_DPLAY_BROKEN_SOCKET_SEND                (118306)
#define     ERR_DPLAY_BROKEN_SOCKET_RECV                (118307)
#define     ERR_DPLAY_RECV_STATE_INVALID                (118308)
#define     ERR_DPLAY_SEND_STATE_INVALID                (118309)
#define     ERR_DPLAY_EV_SEND_TIMTOUT                   (118310)
#define     ERR_DPLAY_DO_READFILE_FAIL                  (118311)

#ifdef __cplusplus
}
#endif

#endif
