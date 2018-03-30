#ifndef _BASE_IDLE_POPBOX_INFO_
#define _BASE_IDLE_POPBOX_INFO_

#include "popupboxproxyfactory.h"
#include <QString>
#include <ETLLib.hpp>
#include "idle/idle_data_define.h"
#include "uicommon/messagebox/messageboxbase.h"
#include "directoryui/include/moddirectoryui.h"
#include "idlenotemanager.h"


#define REGISTER_POPUPBOX_PROXY(type, className) \
        CPopupboxProxyFactory::RegisterProxy(type, className::CreateInstance);

struct PopupBoxInfoData
{
    QString m_strIcon;
    QString m_strTitle;
    QString m_strNote;

    QString m_strCancelBtnTitle;
    QString m_strConfirmBtnTitle;

    MESSAGEBOX_TYPE     m_iPopupBoxType;
    //弹出框存在的时间
    int    nTimes;
    int    m_nMsgBoxId;

    PopupBoxInfoData()
    {
        ReSet();
    }
    void ReSet()
    {
        m_strIcon = "";
        m_strTitle = "Note";
        m_strNote = "";
        m_iPopupBoxType = MESSAGEBOX_OkCancel;
        m_strCancelBtnTitle = "";
        m_strConfirmBtnTitle = "";
        nTimes = -1;
        m_nMsgBoxId = IDLE_POP_NOTE_MESSAGEBOX_ID;
    }
};

struct VoiceMailData
{
    int m_nCountInfo;
    int m_nOnlyAccountId;
    bool m_bIsSetVoiceMailNumber;
    int  m_nUnReadCount;
    VoiceMailData()
    {
        ReSet();
    }
    void ReSet()
    {
        m_nCountInfo = -1;
        m_nOnlyAccountId = -1;
        m_nUnReadCount = 0; //未读信息数据
        m_bIsSetVoiceMailNumber = false;
    }
};

struct TextMessageData
{
    int m_nCountInfo;
    int m_nUnReadCount;//未读信息数据
    TextMessageData()
    {
        ReSet();
    }
    void ReSet()
    {
        m_nCountInfo = -1;
        m_nUnReadCount = -1;
    }
};

class CBaseIdlePopupboxProxy
{
public:
    CBaseIdlePopupboxProxy(int iType);
    virtual ~CBaseIdlePopupboxProxy();

    // 准备弹窗信息
    virtual bool PreparePopupboxInfo() = 0;

    // 返回类型
    int GetType()
    {
        return m_iType;
    }
    void SetType(int type)
    {
        m_iType = type;
    }
    // 弹出窗口
    virtual int PopupBox();

    // 弹窗结束后的处理
    virtual bool ProcessFinishPopupBox(int nReason) = 0;

    virtual bool IsUrgentMsgBox(int nMsgBoxId)
    {
        return true;
    }

protected:
    // 类型
    int                 m_iType;

    // 弹出框信息
    PopupBoxInfoData    m_PopupBoxInfo;
};

#if IF_BUG_23548
class CompletionCallBackData;
class CCompletionCallBackPopboxProxy : public CBaseIdlePopupboxProxy
{
public:
    CCompletionCallBackPopboxProxy();
    virtual ~CCompletionCallBackPopboxProxy();

    virtual bool PreparePopupboxInfo();

    static CBaseIdlePopupboxProxy * CreateInstance();

protected:
    virtual bool ProcessFinishPopupBox(int nReason);
};
#endif

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
class CRecordSavePopboxProxy : public CBaseIdlePopupboxProxy
{
public:
    CRecordSavePopboxProxy();
    virtual ~CRecordSavePopboxProxy();

    virtual bool PreparePopupboxInfo();

    static CBaseIdlePopupboxProxy * CreateInstance();

protected:
    virtual bool ProcessFinishPopupBox(int nReason);
};
#endif

class CallCompletionData;
class CCallCompletionPopboxProxy : public CBaseIdlePopupboxProxy
{
public:
    CCallCompletionPopboxProxy();
    virtual ~CCallCompletionPopboxProxy();

    virtual bool PreparePopupboxInfo();

    static CBaseIdlePopupboxProxy * CreateInstance();

protected:
    virtual bool ProcessFinishPopupBox(int nReason);

private:
    CallCompletionData * m_pCallCompletionData;
};

// 修改为TalkUI模块增加
class CPickupMenuPopboxProxy : public CBaseIdlePopupboxProxy
{
public:
    CPickupMenuPopboxProxy();
    virtual ~CPickupMenuPopboxProxy();

    virtual bool PreparePopupboxInfo();

    static CBaseIdlePopupboxProxy * CreateInstance();

    // 弹出窗口
    virtual int PopupBox();

protected:
    virtual bool ProcessFinishPopupBox(int nReason);
};

class CAutoPPopboxProxy : public CBaseIdlePopupboxProxy
{
public:
    CAutoPPopboxProxy();
    virtual ~CAutoPPopboxProxy();

    virtual bool PreparePopupboxInfo();

    static CBaseIdlePopupboxProxy * CreateInstance();

protected:
    virtual bool ProcessFinishPopupBox(int nReason);
};

class CCallForwardPopboxProxy : public CBaseIdlePopupboxProxy
{
public:
    CCallForwardPopboxProxy();
    virtual ~CCallForwardPopboxProxy();

    virtual bool PreparePopupboxInfo();

    static CBaseIdlePopupboxProxy * CreateInstance();

protected:
    virtual bool ProcessFinishPopupBox(int nReason);
};

class CMissCallPopboxProxy : public CBaseIdlePopupboxProxy
{
public:
    CMissCallPopboxProxy();
    virtual ~CMissCallPopboxProxy();

    virtual bool PreparePopupboxInfo();

    static CBaseIdlePopupboxProxy * CreateInstance();

protected:
    virtual bool ProcessFinishPopupBox(int nReason);
};

class CVoiceMailPopboxProxy : public CBaseIdlePopupboxProxy
{
public:
    CVoiceMailPopboxProxy();
    virtual ~CVoiceMailPopboxProxy();

    virtual bool PreparePopupboxInfo();

    static CBaseIdlePopupboxProxy * CreateInstance();

protected:
    virtual bool ProcessFinishPopupBox(int nReason);

private:
    VoiceMailData   m_VoiceMailData;
};

class CTextMessagePopboxProxy : public CBaseIdlePopupboxProxy
{
public:
    CTextMessagePopboxProxy();
    virtual ~CTextMessagePopboxProxy();

    virtual bool PreparePopupboxInfo();

    static CBaseIdlePopupboxProxy * CreateInstance();

protected:
    virtual bool ProcessFinishPopupBox(int nReason);

private:
    TextMessageData m_TextMsgData;
};

class CNetworkWorkFailedPopboxProxy : public CBaseIdlePopupboxProxy
{
public:
    CNetworkWorkFailedPopboxProxy();
    virtual ~CNetworkWorkFailedPopboxProxy();

    virtual bool PreparePopupboxInfo();

    static CBaseIdlePopupboxProxy * CreateInstance();
    //lcm为了防抖专门提供
    PopupBoxInfoData GetPopBoxData()
    {
        return m_PopupBoxInfo;
    }

protected:
    virtual bool ProcessFinishPopupBox(int nReason);
};

class CRemoteContactDownloadFailedPopboxProxy : public CBaseIdlePopupboxProxy
{
public:
    CRemoteContactDownloadFailedPopboxProxy();
    virtual ~CRemoteContactDownloadFailedPopboxProxy();

    virtual bool PreparePopupboxInfo();

    static CBaseIdlePopupboxProxy * CreateInstance();

protected:
    virtual bool ProcessFinishPopupBox(int nReason);
};

class CCRTRequestPopboxProxy : public CBaseIdlePopupboxProxy
{
public:
    CCRTRequestPopboxProxy();
    virtual ~CCRTRequestPopboxProxy();

    virtual bool PreparePopupboxInfo();

    static CBaseIdlePopupboxProxy * CreateInstance();

protected:
    virtual bool ProcessFinishPopupBox(int nReason);
};

class CHotdesktPopboxProxy : public CBaseIdlePopupboxProxy
{
public:
    CHotdesktPopboxProxy();
    virtual ~CHotdesktPopboxProxy();

    virtual bool PreparePopupboxInfo();

    static CBaseIdlePopupboxProxy * CreateInstance();

protected:
    virtual bool ProcessFinishPopupBox(int nReason);
};

class CAdapterAddPopboxProxy : public CBaseIdlePopupboxProxy
{
public:
    CAdapterAddPopboxProxy();
    virtual ~CAdapterAddPopboxProxy();

    virtual bool PreparePopupboxInfo();

    static CBaseIdlePopupboxProxy * CreateInstance();

protected:
    virtual bool ProcessFinishPopupBox(int nReason);
};

class CAdapterRemovePopboxProxy : public CBaseIdlePopupboxProxy
{
public:
    CAdapterRemovePopboxProxy();
    virtual ~CAdapterRemovePopboxProxy();

    virtual bool PreparePopupboxInfo();

    static CBaseIdlePopupboxProxy * CreateInstance();

protected:
    virtual bool ProcessFinishPopupBox(int nReason);
};

class CWifiAdapterAddPopboxProxy : public CBaseIdlePopupboxProxy
{
public:
    CWifiAdapterAddPopboxProxy();
    virtual ~CWifiAdapterAddPopboxProxy();

    virtual bool PreparePopupboxInfo();

    static CBaseIdlePopupboxProxy * CreateInstance();

protected:
    virtual bool ProcessFinishPopupBox(int nReason);
};

class CWifiAdapterRemovePopboxProxy : public CBaseIdlePopupboxProxy
{
public:
    CWifiAdapterRemovePopboxProxy();
    virtual ~CWifiAdapterRemovePopboxProxy();

    virtual bool PreparePopupboxInfo();

    static CBaseIdlePopupboxProxy * CreateInstance();

protected:
    virtual bool ProcessFinishPopupBox(int nReason);
};

class CWifiConnectOverTimePopboxProxy : public CBaseIdlePopupboxProxy
{
public:
    CWifiConnectOverTimePopboxProxy();
    virtual ~CWifiConnectOverTimePopboxProxy();

    virtual bool PreparePopupboxInfo();

    static CBaseIdlePopupboxProxy * CreateInstance();

protected:
    virtual bool ProcessFinishPopupBox(int nReason);
};

class CAutopErrorPopboxProxy : public CBaseIdlePopupboxProxy
{
public:
    CAutopErrorPopboxProxy();
    virtual ~ CAutopErrorPopboxProxy();

    virtual bool PreparePopupboxInfo();

    static CBaseIdlePopupboxProxy * CreateInstance();

protected:
    virtual bool ProcessFinishPopupBox(int nReason);
};

class CHotelingGuestOutPopboxProxy : public CBaseIdlePopupboxProxy
{
public:
    CHotelingGuestOutPopboxProxy();
    virtual ~CHotelingGuestOutPopboxProxy();

    virtual bool PreparePopupboxInfo();

    static CBaseIdlePopupboxProxy * CreateInstance();

protected:
    virtual bool ProcessFinishPopupBox(int nReason);
};

class CHotelingUnlockPopboxProxy : public CBaseIdlePopupboxProxy
{
public:
    CHotelingUnlockPopboxProxy();
    virtual ~CHotelingUnlockPopboxProxy();

    virtual bool PreparePopupboxInfo();

    static CBaseIdlePopupboxProxy * CreateInstance();

protected:
    virtual bool ProcessFinishPopupBox(int nReason);
};

#if IF_FEATURE_XMLBROWSER_HOTELING
class CHotelingLogoutPopboxProxy : public CBaseIdlePopupboxProxy
{
public:
    CHotelingLogoutPopboxProxy();
    virtual ~CHotelingLogoutPopboxProxy();

    virtual bool PreparePopupboxInfo();

    static CBaseIdlePopupboxProxy * CreateInstance();

    virtual int PopupBox();

protected:
    virtual bool ProcessFinishPopupBox(int nReason);
};

#endif

class CAtuoPUpdatingPopboxProxy : public CBaseIdlePopupboxProxy
{
public:
    CAtuoPUpdatingPopboxProxy();
    virtual ~CAtuoPUpdatingPopboxProxy();

    virtual bool PreparePopupboxInfo();

    static CBaseIdlePopupboxProxy * CreateInstance();

    virtual bool IsUrgentMsgBox(int nMsgBoxId);

    virtual bool ProcessFinishPopupBox(int nReason);
};

class CAtuoPUpdateFailedPopboxProxy : public CBaseIdlePopupboxProxy
{
public:
    CAtuoPUpdateFailedPopboxProxy();
    virtual ~CAtuoPUpdateFailedPopboxProxy();

    virtual bool PreparePopupboxInfo();

    static CBaseIdlePopupboxProxy * CreateInstance();

    virtual bool IsUrgentMsgBox(int nMsgBoxId);

    virtual bool ProcessFinishPopupBox(int nReason);
};

class CAtuoPUpdateSuccessPopboxProxy : public CBaseIdlePopupboxProxy
{
public:
    CAtuoPUpdateSuccessPopboxProxy();
    virtual ~CAtuoPUpdateSuccessPopboxProxy();

    virtual bool PreparePopupboxInfo();

    static CBaseIdlePopupboxProxy * CreateInstance();

    virtual bool IsUrgentMsgBox(int nMsgBoxId);

    virtual bool ProcessFinishPopupBox(int nReason);
};

class CBluetoothMobileAskSyncDirectory : public CBaseIdlePopupboxProxy
{
public:
    CBluetoothMobileAskSyncDirectory();
    virtual ~CBluetoothMobileAskSyncDirectory();

    virtual bool PreparePopupboxInfo();
    static CBaseIdlePopupboxProxy * CreateInstance();

protected:
    virtual bool ProcessFinishPopupBox(int nReason);
};

#if IF_FEATURE_EMERGENCY_HELD
class CE911CallBackPopboxProxy : public CBaseIdlePopupboxProxy
{
public:
    CE911CallBackPopboxProxy();
    virtual ~CE911CallBackPopboxProxy();

    virtual bool PreparePopupboxInfo();

    static CBaseIdlePopupboxProxy * CreateInstance();
protected:
    virtual bool ProcessFinishPopupBox(int nReason);
};
#endif
#endif
