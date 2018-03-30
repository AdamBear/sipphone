#include "../include/baseidlehandle.h"
#include "idlescreen/include/modidlescreen.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "dsskey/include/moddsskey.h"
#include "record/include/modrecord.h"
#include "idleuilogic/include/modidleul.h"

int GetIconTypeByState(PhoneState eStateType)
{
    switch (eStateType)
    {
    case PS_STATE_DND:
        return IT_STATE_DND;
        break;
    case PS_STATE_AA:
        return IT_STATE_AA;
        break;
    case PS_STATE_FWD:
        return IT_STATE_FWD;
        break;
    case PS_STATE_LOCK:
        return IT_STATE_LOCK;
        break;
    case PS_STATE_MUTE:
        return IT_STATE_RING_MUTE;
        break;
    case PS_STATE_RING_MUTE:
        return IT_STATE_RING_MUTE;
        break;
    case PS_STATE_VOICEMAIL:
        return IT_STATE_VOICEMAIL;
        break;
    case PS_STATE_TEXTMESSAGE:
        return IT_STATE_TEXTMESSAGE;
        break;
    case PS_STATE_MISSCALL:
        return IT_STATE_MISSCALL;
        break;
    case PS_STATE_VPN:
        return IT_STATE_VPN;
        break;
    case PS_STATE_NETWORK_FAILED:
        return IT_STATE_INTERNET;
        break;
    case PS_STATE_TALK_MUTE:
        return IT_STATE_TALK_MUTE;
        break;
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    case PS_STATE_USB_CONNECT:
        return IT_USB_DEVICE;
        break;
    case PS_STATE_USB_CONNECTING:
        return IT_USB_DEVICE_DETECTING;
        break;
    case PS_STATE_IDLE_RECORD:
        return IT_USB_IDLE_RECORD;
        break;
    case PS_STATE_IDLE_RECORD_PAUSE:
        return IT_USB_IDLE_RECORD_PAUSE;
        break;
#endif
#ifdef IF_FEATURE_PSTN
    case PS_STATE_PSTN_ACCOUNT_ICON:
        return IT_STATE_PSTN;
        break;
#endif
#ifdef IF_SUPPORT_BLUETOOTH
    case PS_STATE_BLUETOOTH:
        return IT_STATE_BLUETOOTH;
        break;
    case PS_STATE_BLUETOOTH_CONNECT:
        return IT_STATE_BLUETOOTH_CONNECTED;
        break;
    case PS_STATE_BLUETOOTH_MOBILECONNECT:
        return IT_STATE_BLUETOOTH_PHONE_CONNECTED;
        break;
#endif
#ifdef IF_SUPPORT_WIFI
    case PS_STATE_WIFI:
        return IT_STATE_WIFI;
        break;
    case PS_STATE_WIFI_CONNECT:
        return IT_STATE_WIFI_CONNECTED;
        break;
#endif
    default:
        break;
    }
    return IT_UNKNOWN;
}

// 在某个位置插入Softkey，原来的SoftKey移到最后
bool SwapSoftKeyToEnd(VEC_SOFTKET_TYPE& vecSoftkey, int iIndex, SoftKey_TYPE eSoftKey)
{
    bool bRet = false;

    if (iIndex >= vecSoftkey.size())
    {
        vecSoftkey.push_back(eSoftKey);
        bRet = true;
    }
    // 插入key
    else if (vecSoftkey[iIndex] != eSoftKey)
    {
        vecSoftkey.push_back(vecSoftkey[iIndex]);
        vecSoftkey[iIndex] = eSoftKey;
        bRet = true;
    }

    return bRet;
}

CBaseIdleHandle::CBaseIdleHandle(int nType /*= PS_STATE_NONE*/)
    : m_iType(nType)
{
}

CBaseIdleHandle::~CBaseIdleHandle()
{
}

unsigned int CBaseIdleHandle::GetNeedClearNotifyType()
{
    unsigned int nNotifyType = 0;
    nNotifyType = idleScreen_GetNotifyType(m_iType);
    nNotifyType = nNotifyType & (~SNT_ICON);
    return nNotifyType;
}

bool CBaseIdleHandle::TransSoftkeyToKeyCode(SoftKey_TYPE eSoftKey, PHONE_KEY_CODE & eKeyCode)
{
    bool bRet = true;
    switch (eSoftKey)
    {
    case ST_IDLE_HOTKEY1:
        eKeyCode = PHONE_KEY_FN1;
        break;
    case ST_IDLE_HOTKEY2:
        eKeyCode = PHONE_KEY_FN2;
        break;
    case ST_IDLE_HOTKEY3:
        eKeyCode = PHONE_KEY_FN3;
        break;
    case ST_IDLE_HOTKEY4:
        eKeyCode = PHONE_KEY_FN4;
        break;
    default:
        bRet = false;
        break;
    }
    return bRet;
}

void CBaseIdleHandle::SetHint(const yl::string strHint)
{
    m_strHint = strHint;
}

bool CBaseIdleHandle::GetIconInfo(IdleIconInfo & IconData)
{
    IconData.m_bFlash = false;
    IconData.m_strHint = "";
    IconData.m_eIconType = GetIconTypeByState((PhoneState)m_iType);
    return true;
}

bool CBaseIdleHandle::HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode)
{
    bool bRet = true;
    switch (eSoftkey)
    {
    case ST_IDLE_HOTKEY1:
    case ST_IDLE_HOTKEY2:
    case ST_IDLE_HOTKEY3:
    case ST_IDLE_HOTKEY4:
        {
            if (ST_EMPTY != eSoftkey && !TransSoftkeyToKeyCode(eSoftkey, eKeyCode))
            {
                return false;
            }
            int iDsskeyID = IdleUL_GetDsskeyIDByKeyCode(eKeyCode);
            DSSKEY_MODULE_TYPE eModuleType = dsskey_GetModuleType(iDsskeyID);
            if (DMT_PROGRAMEKEY_MODULE != eModuleType)
            {
                return false;
            }
            dsskey_OnDsskeyClick(iDsskeyID);
        }
        break;
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    case ST_PAUSE_RECORD:
        {
            modRecord_RecordPause(RECORD_AUDIO);
        }
        break;
    case ST_RESUME_RECORD:
        {
            modRecord_RecordResume(RECORD_AUDIO);
        }
        break;
    case ST_STOP_RECORD:
        {
            modRecord_RecordStop(RECORD_AUDIO);
        }
        break;
#endif
    default:
        {
            bRet = false;
        }
        break;
    }
    return bRet;
}

bool CBaseIdleHandle::GetSoftkey(VEC_SOFTKET_TYPE& m_vecSoftkey)
{
    bool bRet = false;
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    if (modRecord_IsRecording(RECORD_AUDIO))
    {
        m_vecSoftkey.push_back(ST_STOP_RECORD);
        m_vecSoftkey.push_back(ST_PAUSE_RECORD);
        bRet = true;
    }
    else if (modRecord_IsRecordPause(RECORD_AUDIO))
    {
        m_vecSoftkey.push_back(ST_STOP_RECORD);
        m_vecSoftkey.push_back(ST_RESUME_RECORD);
        bRet = true;
    }
#endif
    return bRet;
}

int UpdateNotify(char * strId, PhoneState estatus)
{
    //不是四种之一，直接return
    if (0 != strcmp(strId, kszDisplayMissedCallPopup)
            && 0 != strcmp(strId, kszDisplayTextMessagePopup)
            && 0 != strcmp(strId, kszDisplayForwardCallPopup)
            && 0 != strcmp(strId, kszDisplayVoiceMailPopup))
    {
        return -1;
    }
    //设置是否弹出消息提示框
    bool IsPopWind = !configParser_GetConfigInt(kszStatusChangeIsNotPopWind);
    bool IsPopNotify = IsPopWind;
    //判断具体开关，找不到配置就按中性处理
    if (IsPopNotify)
    {
        yl::string strPopValue = configParser_GetConfigString(strId);
        if (!strPopValue.empty())
        {
            IsPopNotify = atoi(strPopValue.c_str());
        }
    }

    if (!IsPopNotify)
    {
        //清除已经弹出来的标志
        idleScreen_ClearNotifyTypeFlag(estatus, SNT_TEXT_NOTIFY | SNT_POPUP | SNT_SOFTKEY);
        //如果配置是不弹出弹出提示，那么就需要清除已经注册上的类型

#if IF_SUPPORT_TEXT_NOTIFY
        idleScreen_ClearRegStatusTypeFlag(estatus, SNT_TEXT_NOTIFY);
        idleScreen_ClearRegStatusTypeFlag(estatus, SNT_SOFTKEY);
#else
        idleScreen_ClearRegStatusTypeFlag(estatus, SNT_POPUP);
#endif
    }
    else//如果是需要弹出提示，则注册上相应的状态
    {
#if IF_SUPPORT_TEXT_NOTIFY
        idleScreen_SetNotifyType(estatus, SNT_TEXT_NOTIFY, 0);
        idleScreen_SetNotifyType(estatus, SNT_SOFTKEY, PS_STATE_MAX);
#else
        idleScreen_SetNotifyType(estatus, SNT_POPUP, 0);
#endif
    }
    idleScreen_TurnStatusNotify(true);
    return IsPopNotify ? 1 : 0;
}

