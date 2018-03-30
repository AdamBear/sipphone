#ifndef ACTIONUR_ACITONURL_H_
#define ACTIONUR_ACITONURL_H_


#include <ylhashmap.h>

#include "commondefine.h"
#include "taskaction/taskactioncallbackbase.h"

#if IF_FEATURE_EVENT_MODULE
#include "extended/event/src/eventbase.h"
#endif

struct ActionUrlData;

namespace NS_TA
{
class CTaskAction;
}

namespace NS_AU
{

// Class CActionUrl, 状态同步类. 当话机某些状态改变
// 或者是话机触发了预定操作后, 向服务器发送相应的消息.
#if IF_FEATURE_EVENT_MODULE
class CActionUrl : public NS_TA::CTaskActionCallbackBase, public CEventBase
#else
class CActionUrl : public NS_TA::CTaskActionCallbackBase
#endif
{
public:

    typedef YLHashMap<int, yl::string> ConfigureMap;

    // 获取单键.
    static CActionUrl * GetInstance() DECLARE_ATTR;
    // 销毁单键.
    static void ReleaseInstance();

    // Override.
    void OnTaskActionCallback(NS_TA::CTaskAction * pTaskAction);

#if IF_FEATURE_EVENT_MODULE
    //发送事件
    virtual bool SendEvent(EventDataBase * pData);
    //获取forward事件数据
    void GetForwardEventData(EventDataBase * pData, ActionUrlData & refData);
    //获取talk事件数据
    void GetTalkEventData(EventDataBase * pData, ActionUrlData & refData);
    //获取对方信息数据
    void GetRemoteInfoData(EventDataBase * pData, ActionUrlData & refData);
    //获取本地账号数据
    void GetAccountData(int nLineID, ActionUrlData & refData);
    //获取本地账号数据
    void GetPhoneInfoData(EventDataBase * pData);
    //获取外围设备数据
    void GetConnectedDeviceDate(EventDataBase * pData, ActionUrlData & refData);
#endif

    // 发送Url.
    // | nType | 是Action Url类型, 为ActionUrlType枚举值.
    // 如果发送成功则返回true, 否则返回false.
    bool Send(int nType);

    // 发送Url, 并连带数据.
    // | nType | 是Action Url类型, 为ActionUrlType枚举值.
    // | refData | 是要发送的数据.
    // 如果发送成功则返回true, 否则返回false.
    bool Send(int nType, const ActionUrlData & refData);

#ifdef IF_SUPPORT_UME

    //设置远程Ip后，给该ip地址发送OK。
    void SendIpOk();


    //发送话机信息到UC服务器
    //用于通讯伴侣
    void SendPhoneInfo();
#if !IF_FEATURE_EVENT_MODULE
    bool IsIPChange();
#endif
#endif
#if !IF_FEATURE_EVENT_MODULE
    // 处理计时器到时消息.
    bool OnTimeoutMessage(unsigned int uTimerID) DECLARE_ATTR;
#endif

private:
    // 构造函数和析构函数.
    CActionUrl();
    virtual ~CActionUrl();

    // 初始化.
    // 如果初始化成功则返回true, 否则返回false.
    bool MyInit();
    // 销毁.
    // 如果销毁成功则返回true, 否则返回false.
    bool MyFinal();

    // 发送Url和数据.
    // | nType | 是Action Url类型, 为ActionUrlType枚举值.
    // | refData | 是要发送的数据.
    // 如果发送成功则返回true, 否则返回false.
    bool MySend(int nType, const ActionUrlData & refData);

    // 替换变量符号为具体的值. 变量符号形如"$mac".
    // | strInput | 是输入字符串.
    // | refData | 是要用户传入的数据.
    // 返回被替换后的字符串.
    yl::string MyReplaceVar(const yl::string & strInput,
                            const ActionUrlData & refData);

#if !IF_FEATURE_EVENT_MODULE
    // 判断是否是启动时的IP change
    bool IsPowerOnChangeIP();
    // 重置标志
    void ResetPowerOnChangeIP();
    // 消息处理
    static LRESULT OnMessage(msgObject & msg);
#endif

#ifdef IF_SUPPORT_UME
    const char * GetOperationString(int nType);

    //获取账号列表，账号间，号隔开，如sip:2967@192.168.1.100,sip:8805@10.2.1.199
    void GetAccountList(yl::string & strList);
#endif

private:
    static CActionUrl * m_pInst; // 单键实例.
    ConfigureMap m_mapCfg;  // 配置项map.

    yl::string m_strIp;
    // 一些固定的信息
    yl::string m_strMAC;
    yl::string m_strPhoneType;
    yl::string m_strFirmware;
#if !IF_FEATURE_EVENT_MODULE
    bool m_bPowerOnChangeIP;
#endif
    yl::string m_strExpNum;      //EXP个数
    yl::string m_strEhsNum;      //EHS个数
    yl::string m_strUsbNum;      //USB个数
    yl::string m_strWifiDongleNum; //wifi dongle
    yl::string m_strBlueToothDongleNum; //bluetooth
    yl::string m_strUSBHeadSet;     //usb headset
    DISALLOW_COPY_AND_ASSIGN(CActionUrl);
};

}  // namespace NS_AU

#define g_pActionUrl (NS_AU::CActionUrl::GetInstance())

#endif  // ACTIONUR_ACITONURL_H_
