
#ifndef global_info_h__
#define global_info_h__
#include <map>
#include <string>

#include "utility/define.h"
#include "utility/singleton.h"
#include "utility/name_value_collection.h"

class GlobalInfo : public Singleton<GlobalInfo>
{
    PRIVATE_CONTRUCT_DECONSTRUCT_FOR_SINGLETON(GlobalInfo);
public:
    void SetSystemInfo(const char *appKey, uint32_t appKeySize,
                       const char *appName, uint32_t appNameSize,
                       const char *appVersion, uint32_t appVersionSize,
                       const char *peerid, uint32_t peeridSize,
                       const char *guid, uint32_t guidSize,
                       const char *statSavePath, uint32_t statSavePathSize,
                       const char *statCfgPath, uint32_t statCfgPathSize,
                       NetWorkType net);

    void SetUserId(const char *userid, uint32_t useridSize);
    const std::string& GetUserId();

    void SetMiuiVersion(const char *version, uint32_t versionSize);
    const std::string& GetMiuiVersion();

    void SetMac(const char *mac, uint32_t macLen);
    const std::string& GetMac();

    void SetImei(const char *imei, uint32_t imeiLen);
    const std::string& GetImei();

    const std::string& GetPeerid();
    const std::string& GetAppKey();
    const std::string& GetAppName();
    uint32_t GetProductFlag();
    const std::string& GetAppVersion();
    const std::string& GetGuid();
    const std::string& GetStatSavePath();
    const std::string& GetStatCfgSavePath();

    void SetNetWorkType(NetWorkType net);
    NetWorkType GetNetWorkType();

    void SetNetWorkCarrier(NetWorkCarrier carrier);
    NetWorkCarrier GetNetWorkCarrier();

    const char* GetPeeridPtr();
    uint32_t GetProductVersion();

    void SetWifiBssid(const std::string bssid);
    const std::string& GetWifiBssid(uint32_t &index);

    void SetLocalProperty(const std::string &strKey, const std::string &strValue);
    void GetLocalProperty(const std::string &strKey, std::string &strValue);

    void SetStatReportSwitch(bool reportSwitch);
    bool GetStatReportSwitch();

private:
    std::string m_peerid;
    std::string m_appKey;										// 合作key
    std::string m_appName;										// 合作商名称
    std::string m_appVersion;									// 应用的版本号
    std::string m_guid;											// 手机机器码
    std::string m_statSavePath;									// 统计上报持久化文件存储路径
    std::string m_statCfgPath;									// 哈勃统计配置文件路径
    std::string m_userId;										// 用户id
    std::string m_mac;											// mac
    std::string m_imei;											// miei
    std::string m_miuiVersion;									// miui version
    std::string m_bssid;										// wifi bssid
    uint32_t m_bssidIndex;										// 用于表示bssid是否更改的

    NameValueCollection m_localProperty;		// 本地属性

    NetWorkType m_netType;
    NetWorkCarrier m_netCarrier;

    bool m_reportSwitch;
};

#endif // global_info_h__



