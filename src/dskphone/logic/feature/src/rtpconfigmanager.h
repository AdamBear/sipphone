#ifndef RTP_CONFIG_MANAGER_H
#define RTP_CONFIG_MANAGER_H

#include "../include/featurecommon.h"

class CRTPConfigManager
{
    DECLARE_SINGLETON_CLASS(CRTPConfigManager)

public:
    // 初始化
    void Init();

    // 获取显示的RTP配置项列表
    const RTPConfigureList * GetRTPOptionEnabledList()
    {
        return &m_listCfg;
    }

private:
    // 配置项列表发生改变的消息处理
    static LRESULT OnMessage(msgObject & refObj);

    // 加载rtp.xml
    bool LoadRTPConfig();

private:
    // 配置项列表
    RTPConfigureList m_listCfg;
};

// 获取实例
#define _RTPConfigManager GET_SINGLETON_INSTANCE(CRTPConfigManager)

#endif  //RTP_CONFIG_MANAGER_H
