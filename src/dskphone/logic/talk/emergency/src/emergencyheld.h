#ifndef __E911_PROCESSER_H__
#define __E911_PROCESSER_H__
#if IF_FEATURE_EMERGENCY_HELD
#include "extended/event/src/eventbase.h"
#include "taskaction/taskactioncallbackbase.h"
#include "xmlparser.hpp"

using namespace NS_TA;

class CEmergencyHELDProcesser : public NS_TA::CTaskActionCallbackBase, public CEventBase
{
    enum EM_HELD_REQUEST_TYPE
    {
        EM_REQ_SIMPLE = 0,
        EM_REQ_REDSKY = 1,
    };

public:

    static CEmergencyHELDProcesser & GetInstace();

    void Init();
    void UnInit();
    // 发送事件
    virtual bool SendEvent(EventDataBase * pData);
    // 发送话机信息
    void SendPhoneInfo(EventDataBase * pData);
    // 获取Localtion
    yl::string getLocation()
    {
        return m_strLocation;
    }
    // 请求Location
    void requestLocationbyHELD();
    // TaskAction回调
    void OnTaskActionCallback(NS_TA::CTaskAction * pTaskAction);
    // 重新加载配置
    void ReloadConfig();
    // 处理定时器消息
    bool OnTimer(UINT uTimerID) DECLARE_ATTR;

private:
    CEmergencyHELDProcesser();
    virtual ~CEmergencyHELDProcesser();

    void setLocaltion(yl::string strLocation)
    {
        m_strLocation = strLocation;
    }
    // 添加额外请求元素
    void AddRequestElement(xml_node & xmlNode);
    // 解析Location信息
    bool ParseLocation(const yl::string & strFile);
    // 发送话机信息
    void SendPhoneInfo();

private:
    PhoneInfoEventData      m_phoneInfoData;
    yl::string              m_strLocation;
    yl::string              m_strE911Server;
    bool                    m_bIsEnable;
    EM_HELD_REQUEST_TYPE    m_eRequestType;
};

#define g_objEmergencyHELDManager (CEmergencyHELDProcesser::GetInstace())

#endif

#endif
