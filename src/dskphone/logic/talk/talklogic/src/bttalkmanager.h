#ifndef __BTTALK_MANAGER_H__
#define __BTTALK_MANAGER_H__

#ifdef BLUETOOTH_USED_OLD_VERSION
#include "wireless/bluetoothOld/include/btdefines.h"
#else
#include "wireless/bluetoothNew/include/btdefines.h"
#endif // IF_OLD_BLUETOOTH

#ifdef IF_BT_SUPPORT_PHONE
#define  _BTMobileManager CBTTalkManager::GetInstance()
class CBTTalkManager
{
public:
    static CBTTalkManager& GetInstance()
    {
        static CBTTalkManager s;
        return s;
    }

    //是否允许新的蓝牙话路
    bool IsAllowNewBTCall();

    //消息处理函数
    bool OnMobileMsg(msgObject& msg);

    //呼出话路
    bool CallMobileOut(const yl::string& strNum, const yl::string& strName = "", int nCallID = -1,
                       bool bColorRingback = false);

    //处理来电
    bool ProcessIncommingCall(const yl::string& strNum, int nCallID);

    //处理呼出
    bool ProcessCallOut(CSingleSession* pSingleSession, CCallInfo* pCallInfo,
                        const yl::string& strNum, const yl::string& strName = "");

    //处理呼出逻辑
    bool DoCallOut(const yl::string& strNum, const yl::string& strName, CCallInfo* pCallInfo,
                   yl::string& strReason);

    //进入拨号界面
    CSingleSession* EnterDialUI(const yl::string& strNumber = "", ROUTINE_STATE eRoutine = ROUTINE_BTDIALING);

    //进入预拨号界面
    CSingleSession* EnterPredialUI(int iKey);

    //获取蓝牙回话数
    int GetBTSessionNumber(bool bExceptDial = true);

    // 进入呼叫完成状态的处理
    void ProcessAfterCallFinished(CSingleSession* pSession, void* pData, bool bCallFail = true,
                                  bool bEndCall = true);

    //声音通道是否建立
    bool IsChannelLinked()
    {
        return m_bChannelLinked;
    }

    //退出所有手机联动状态
    void ExitMobileSync();

    //获取dsskey状态
    void GetDsskeyStatus(CBaseSession* pSession, Dsskey_Status& eStatus);

    //建立通话
    bool EstablishTalk(void* pData);

    //是否为蓝牙HOLD消息
    bool IsHoldMsg(int iHoldState);

    //设置通话参数
    void SetVoiceParam(CCallInfo* pCallInfo);

    //从驱动结构体中通话信息
    void MapBTCallUserInfo(BTTalkCallUserInfo& dstInfo, const void* pSrcInfo);

    //是否存在其他蓝牙通话话路
    bool IsMobileTalkSessionExist(int iExceptCallID = -1);

    bool GetAllBtSessionID(YLVector<int>& vecSessionID);
protected:
    //处理手机状态改变消息
    bool ProcessMBCallStatusChange(msgObject& msg);

    //处理声音通话状态改变消息
    bool ProcessLinkStatusChange(const btkit_msg_t* pMsg);

    //处理消息转换
    void TranslateMsgToSip(CBaseSession* pSession, msgObject& msg, BTTalkCallUserInfo& btCallInfo);

private:
    bool m_bChannelLinked;       //声音通道是否建立
    CBTTalkManager();
    ~CBTTalkManager();
};

#endif // IF_BT_SUPPORT_PHONE
#endif //__BTTALK_MANAGER_H__
