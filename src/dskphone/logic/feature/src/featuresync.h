#ifndef __FEATURE_SYNC_H__
#define __FEATURE_SYNC_H__

#include <singletonclassdefine.h> // DECLARE_SINGLETON_CLASS GET_SINGLETON_INSTANCE IMPLEMENT_GETINSTANCE
#include <ETLLib.hpp> // msgObject

#include "talk/fwd_dnd/include/fwddndtypes.h" // FWD_TYPE


class CSyncBaseProcess;

class CFeatureSyncManager
{
    DECLARE_SINGLETON_CLASS(CFeatureSyncManager)

public:
    /** 初始化同步管理类 */
    void InitFeatureSync();

    // SIP消息处理
    static LRESULT OnSIPMessage(msgObject & objMsg);
    // 配置变更消息处理
    static LRESULT OnConfigChange(msgObject & objMsg);
    // DND状态改变消息处理
    BOOL ProcessDNDMsg(int iAccountId);
    // FWD状态改变消息处理
    BOOL ProcessFWDMsg(int iAccountId, FWD_TYPE eType);
    // 账号状态变化消息处理(anonymous code)
    BOOL ProcessAccountMsg(msgObject & objMsg);

    /**
     * 判断BoradSoft Feature Key同步功能是否开启
     *
     * @return  FeatureKey同步功能是否开启
     */
    bool IsFeatureKeySync(int iAccountID = -1);

    /**
     * 判断DND同步功能是否开启
     *
     * @return  DND同步功能是否开启
     */
    bool IsDNDSync(int iAccountID = -1);

    /**
     * 判断FWD同步功能是否开启
     *
     * @return  DND同步功能是否开启
     */
    bool IsForwardSync(int iAccountID = -1);

    /**
     * 设置BoradSoft Feature Key同步功能是否开启
     *
     * @param   bEnable FeatureKey同步功能是否开启
     */
    void SetFeatureKeySync(bool bEnable, int iAccountID = -1);

    /**
     * 设置DND同步功能是否开启
     *
     * @param   bEnable DND同步功能是否开启
     */
    void SetDNDSync(bool bEnable, int iAccountID = -1);

    /**
     * 设置FWD同步功能是否开启
     *
     * @param   bEnable FWD同步功能是否开启
     */
    void SetForwardSync(bool bEnable, int iAccountID = -1);

    /**
     * 判断启用DND同步功能时，本地是否需要处理DND（拒接）
     *
     * @param   iAccount    账号ID
     *
     * @return  本地是否需要处理DND
     */
    bool IsDNDLocalProcess(int iAccount);

    /**
     * 判断启用FWD同步功能时，本地是否需要处理FWD（转移）
     *
     * @param   iAccount    账号ID
     *
     * @return  本地是否需要处理FWD
     */
    bool IsForwardLocalProcess(int iAccount);

    struct FeatureSyncInfo
    {
        bool bFeatureKeySync;
        bool bDndSync;
        bool bForwardSync;
        FeatureSyncInfo()
        {
            bFeatureKeySync = false;
            bDndSync = false;
            bForwardSync = false;
        }
    };

private:
    enum SYNC_FEATURE_TYPE
    {
        SFT_DND,
        SFT_FWD,
        SFT_UPDATE
    };

    //定义全局配置存储ID
    enum FEATURE_SYNC_ID
    {
        FeatureSyncIDGlobal = -1
    };

    typedef YLHashMap<int, FeatureSyncInfo> typeFeatureKeySyncMap;
    typeFeatureKeySyncMap m_mapFeatureKeyInfo;

#if IF_BUG_27027
    BOOL  m_bFortinetDndSync; //Fortinet客户特制DND同步
#endif

    //发送Anonymous同步码
    void SendAnonymousRejectCode(int iAccount);
    void SendAnonymousCode(int iAccount);

    // 重新加载配置
    void ReloadConfig();

    void GetAccountFeatureSyncInfo(const FeatureSyncInfo & globalSyncInfo, int iAccountID, FeatureSyncInfo & accSyncInfo);

    // 给服务器发送同步的消息
    BOOL PostSyncStatusToServer(SYNC_FEATURE_TYPE eType, int iLineId, void * pTypeData = NULL);

    // 解析服务器的同步消息(XML格式)
    void ParseXMLExtraData(LPCSTR pXMLContent, int iAccount);

    // 获取Sync功能处理模块的指针
    CSyncBaseProcess * GetSyncProcesser(SYNC_FEATURE_TYPE eType, int iAccountID = -1);

    // 根据XML文件中基节点的名称获取Sync功能处理模块指针
    CSyncBaseProcess * GetSyncProcesser(LPCSTR lpszNodeName, int iAccountID = -1);

    // 给服务器发送状态同步请求
    void PostUpdateRequestToServer();
};

// 获取实例
#define _FeatureSyncMng GET_SINGLETON_INSTANCE(CFeatureSyncManager)

#endif // __FEATURE_SYNC_H__
