#include "basetalkuiprocessor.h"
#include "talkuilogic_inc.h"
#include "talkuilogicdelegate.h"
#include "keyguard/include/keyguard_common.h"
#include "keyguard/include/modkeyguard.h"

#include "talkuilogicsoftkey.h"
#include "talkuilogicmanager.h"

#include "wireless/bluetoothNew/include/modbluetooth.h"

#include "dialprocessor.h"

#include "talkui/include/modtalkui.h"

#include "../logic//adapterbox/include/modadapterbox.h"
//#include "../uibase/globalinputhook/modinputhook.h"

//录制相关模块
#include "record/include/modrecord.h"

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
#include "broadsoft/callrecording/include/modcallrecording.h"
#endif

#include "talk/fwd_dnd/include/modfwddnd.h"//DND刷新

namespace
{
#define SOFTKEY_PAGE_COUNT      4
}

CBaseTalkUIProcessor::CBaseTalkUIProcessor(yl::string strName/* = ""*/)
#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    : m_eRecordStatue(CALL_RECORD_UNKNOWN)
#endif
{
    m_eSessionState = SESSION_UNKNOWN;
    m_iSessionID = -1;
    m_strProcessorName = strName;

    m_eNetworkStatus = TALK_NETWORK_STATUS_UNKNOWN;

    m_strTitle = "";
    //在init函数做初始化
//  m_listIcons.clear();

    m_tStartTime = time(0);     //通话计时 开始时间
    m_bShowTalkTime = false;        //是否显示通话时间

    m_bUseCustomSoftkey = false;
    m_vecCustomSoftkey.clear();
}

CBaseTalkUIProcessor::~CBaseTalkUIProcessor()
{
    TALKUI_INFO("CBaseTalkUIProcessor::~CBaseTalkUIProcessor  ProcessorName:%s",
                m_strProcessorName.c_str());
}

void CBaseTalkUIProcessor::Init()
{
    //这边不能设置 sessionid和 sessionstate 因为  init函数会后与setsessionid调用
//  m_eSessionState = SESSION_UNKNOWN;
//  m_iSessionID = -1;

    m_strTitle = "";
    m_listIcons.clear();

    m_eNetworkStatus = TALK_NETWORK_STATUS_UNKNOWN;

    m_bUseCustomSoftkey = false;
    m_vecCustomSoftkey.clear();
}

void CBaseTalkUIProcessor::Uninit()
{

}

//设置SessionID
void CBaseTalkUIProcessor::SetSessionID(int iSessionID)
{
    m_iSessionID = iSessionID;
}

void CBaseTalkUIProcessor::SetSessionState(SESSION_STATE  eSessionState)
{
    m_eSessionState = eSessionState;

}

//获取通话时间
int  CBaseTalkUIProcessor::GetTalkTime()
{
    if (m_bShowTalkTime) //是否显示通话时间
    {
        return time(0) - m_tStartTime;
    }

    return 0;
}

//从CallInfo 数据中获取 record图标类型
ICON_TYPE CBaseTalkUIProcessor::GetRecordIconTypeFromCallInfo(CCallInfo * pCallInfo)
{
    if (pCallInfo == NULL
#if IF_TALKUI_USB_ENABLE
            //如果本地录制可用，则不显示网络录制的图标
            || modRecord_IsRecordEnable(RECORD_AUDIO)
#endif
       )
    {
        return IT_UNKNOWN;
    }

    //下面都是网络录制的图标的相关设置

    ICON_TYPE eRecordIcon = IT_UNKNOWN;
#if IF_BUG_22457_Record
    if (pCallInfo->IsRecordOn(INF_RECORD_SIP)
            || pCallInfo->m_eSIPRecordStatus != SIP_RECORD_STATUS_UNKNOWN)
    {
        switch (pCallInfo->m_eSIPRecordStatus)
        {
        case SIP_RECORD_STATUS_OFF_FAILED:
            eRecordIcon = IT_CANNOT_BE_STOPPED;
            break;
        case SIP_RECORD_STATUS_OFF_SUCCESS:
            eRecordIcon = IT_UNKNOWN;
            break;
        case SIP_RECORD_STATUS_ON_FAILED:
            eRecordIcon = IT_CANNOT_BE_STARTED;
            break;
        case SIP_RECORD_STATUS_ON_SUCCESS:
            eRecordIcon = IT_RECORD;
            break;
        default:
            eRecordIcon = IT_UNKNOWN;
            break;
        }
    }
#else
    if (pCallInfo->IsRecordOn(INF_RECORD_SIP | INF_RECORD_VOICE))
    {
        eRecordIcon = IT_RECORD;
    }
#endif

    if (pCallInfo->IsRecordOn(INF_RECORD_URL)
            || pCallInfo->m_eURLRecordError != URE_NONE)
    {
        switch (pCallInfo->m_eURLRecordError)
        {
        case URE_NONE:
            eRecordIcon = IT_RECORD;
            break;
        case URE_CANNOT_BE_RECORDED:
            eRecordIcon = IT_CANNOT_BE_RECORDED;
            break;
        case URE_CANNOT_BE_STOPPED:
            eRecordIcon = IT_CANNOT_BE_STOPPED;
            break;
        case URE_CANNOT_BE_STARTED:
            eRecordIcon = IT_CANNOT_BE_STARTED;
            break;
        case URE_FULL:
            eRecordIcon = IT_RECORD_FULL;
            break;
        default:
            break;
        }
    }

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    if (pCallInfo->m_eRecordStatus != CALL_RECORD_UNKNOWN)
    {
        switch (pCallInfo->m_eRecordStatus)
        {
        case CALL_RECORD_NORMAL:
            eRecordIcon = IT_RECORD_NORMAL;
            break;
        case CALL_RECORD_START:
            eRecordIcon = IT_RECORD;
            break;
        case CALL_RECORD_PAUSE:
            eRecordIcon = IT_RECORD_PAUSE;
            break;
        default:
            break;
        }
    }
#endif

    return eRecordIcon;
}

//根据ICONType 获取 通话显示图标
TALKUI_ICON CBaseTalkUIProcessor::GetTalkUIIconByIconType(ICON_TYPE eIconType)
{
    switch (eIconType)
    {
    case IT_RECORD:     //录音图标
        {
            return TALKUI_ICON_RECORD;
        }
        break;
    case IT_CANNOT_BE_RECORDED:     //无法录音
        {
            return TALKUI_ICON_CANNOT_BE_RECORDED;
        }
        break;
    case IT_CANNOT_BE_STOPPED:      //无法停止录音
        {
            return TALKUI_ICON_CANNOT_BE_STOPPED_RECORD;
        }
        break;
    case IT_CANNOT_BE_STARTED:      //无法开始录音
        {
            return TALKUI_ICON_CANNOT_BE_STARTED_RECORD;
        }
        break;
    case IT_RECORD_FULL:        //录音已满
        {
            return TALKUI_ICON_RECORD_FULL;
        }
        break;
    case IT_RECORD_NORMAL:      //录音
        {
            return TALKUI_ICON_CALL_RECORD_NORMAL;
        }
        break;
    case IT_RECORD_PAUSE:       //录音暂停
        {
            return TALKUI_ICON_CALL_RECORD_PAUSE;
        }
        break;
    default:
        break;
    }

    return TALKUI_ICON_NONE;

}

//// 从各个Routine获取需要显示的icon
//void CBaseTalkUIProcessor::GetIconList(YLList<TALKUI_ICON>& listIcons)
//{
//  listIcons = m_listIcons;
//}
//
//// 返回左上角标题
//yl::string CBaseTalkUIProcessor::GetTitle()
//{
//  return m_strTitle;
//}

// 返回通道图标状态
TALKUI_CHANNEL_ICON CBaseTalkUIProcessor::GetChannelIcon()
{
    return _TalkUILogicDelegate.GetChannelIcon();
}

// 返回蓝牙图标状态
//TALKUI_BLUETOOTH_ICON CBaseTalkUIProcessor::GetBluetoothIcon()
//{
//  return _TalkUILogicDelegate.GetBlueToothIcon();
//}

//刷新UI通道图标
void CBaseTalkUIProcessor::UpdateUIChannelIcon()
{
    //获取通道类型
    CHANNEL_MODE eChannelMode = voice_GetCurrentChannel();

    if (S_GetCurrentChannelMode() != eChannelMode)
    {
        if (S_GetCurrentChannelMode() == CHANNEL_HANDFREE
                || S_GetCurrentChannelMode() == CHANNEL_GROUP
                || S_GetCurrentChannelMode() == CHANNEL_HANDSET
                || S_GetCurrentChannelMode() == CHANNEL_HEADSET)
        {
            S_SetIsShowVolume(true);
        }

        S_SetCurrentChannelMode(eChannelMode);
    }

    //处理完通道键后 需要更新通道
    _TalkUILogicFrameManager.UpdateChannelIcon();
}


//刷新通话时间显示
void CBaseTalkUIProcessor::UpdateTalkUITime()
{
    if (m_bShowTalkTime)    //需要显示时间 才需要去更新
    {
        int iTalkTime = GetTalkTime();

        uint uHour = iTalkTime / 3600;
        uint uMin = (iTalkTime / 60) % 60;
        uint uSecond = iTalkTime % 60;

        //调用函数刷新时间
        _TalkUILogicFrameManager.UpdateTalkUITime(uHour, uMin, uSecond);
    }
}

//刷新softkey
void CBaseTalkUIProcessor::UpdateTalkSoftkey()
{
    _TalkUILogicFrameManager.UpdateTalkUISoftkey();
}


//获取softkey数据
void CBaseTalkUIProcessor::GetTalkSoftkeyData(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    // 清空softkey数据
    VecSoftkey.clear();

    GetAlwaysBeforeSoftkey(VecSoftkey);

    // 先获取配置的softkey
    if (IsAllowDefineSoftkey()
            && m_bUseCustomSoftkey)
    {
        int nCustomSoftkeySize = m_vecCustomSoftkey.size();
        for (int i = 0; i < nCustomSoftkeySize; ++i)
        {
            VecSoftkey.push_back(m_vecCustomSoftkey[i]);
        }

        // softkey重排列
        RelayoutSoftkeyData(VecSoftkey);
    }
    //从子窗口获取softkey
    else
    {
        GetSoftkey(VecSoftkey);
    }

    GetAlwaysAfterSoftkey(VecSoftkey);

    if (IsAllowDefineSoftkey()
            && m_bUseCustomSoftkey)
    {
        AdjustCustomSoftkey(VecSoftkey);
    }
    else
    {
        CutEmptySoftkey(VecSoftkey);
    }
}

//获取softkey layout 的softkey数据
bool CBaseTalkUIProcessor::LoadCustomSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey,
        const yl::string & strFile,
        const yl::string & strStatus)
{
    // 读取配置文件
    xml_document xFile;
    //!!!!! 文件存放路径待定
    yl::string strPath = CONFIG_PATH"softkey/" + strFile;
    xml_parse_result objResult = xFile.load_file(strPath.c_str());
    if (objResult.status != status_ok)
    {
        TALKUI_INFO("CDlgBaseTalkUI::LoadSoftkey load file[%s] error", strFile.c_str());
        return false;
    }

    yl::string strNewFile = strFile;
    yl::string strNewStatus = strStatus;

    xml_node nodeXmlNode = xFile.child(strStatus.c_str());
    if (nodeXmlNode.empty())
    {
        TALKUI_INFO("CDlgBaseTalkUI::LoadSoftkey load status[%s] error", strNewStatus.c_str());
#if IF_FEATURE_CUSTOM_SOFTKEY
        if (commonUnits_GetBackupSoftkey(strNewFile, strNewStatus))
        {
            strPath = CONFIG_PATH"softkey/" + strNewFile;
            objResult = xFile.load_file(strPath.c_str());

            if (objResult.status != status_ok)
            {
                TALKUI_INFO("CDlgBaseTalkUI::LoadSoftkey load file[%s] error", strNewFile.c_str());
                return false;
            }

            nodeXmlNode = xFile.child(strNewStatus.c_str());
            if (nodeXmlNode.empty())
            {
                return false;
            }
        }
        else
#endif
        {
            return false;
        }
    }

#if IF_FEATURE_CUSTOM_SOFTKEY
    TALKUI_INFO("CDlgBaseTalkUI::LoadCustomSoftkey strFile[%s] strStatus[%s]", strNewFile.c_str(),
                strNewStatus.c_str());
    if (!commonUnits_IsSoftkeyEnable(strNewFile, strNewStatus))
    {
        return false;
    }
#endif

    nodeXmlNode = nodeXmlNode.child("Enable");
    if (nodeXmlNode.empty())
    {
        chWARNING2(false, "CDlgBaseTalkUI::LoadSoftkey load enable status error");
        return false;
    }

    if (!nodeXmlNode.empty())
    {
        xml_node nodeKeyNode = nodeXmlNode.child("Key");
        while (!nodeKeyNode.empty())
        {
            VecSoftkey.push_back(GetTypeOfKey(nodeKeyNode.attribute("Type").value()));
            //  AddSoftkey(objSoftkeyData, SoftkeyTypeToAction(nodeKeyNode.attribute("Type").value()));
            nodeKeyNode = nodeKeyNode.next_sibling();
        }
    }

    return true;
}

void CBaseTalkUIProcessor::PreLoadCustomSoftkey()
{
    m_bUseCustomSoftkey = false;
    m_vecCustomSoftkey.clear();
    if ((bool)configParser_GetConfigInt(kszCustomSoftkeySwitch))
    {
        yl::string strFile;
        yl::string strStatus;
        GetSoftkey(strFile, strStatus);
        //获取softkey layout 的数据
        m_bUseCustomSoftkey = LoadCustomSoftkey(m_vecCustomSoftkey, strFile, strStatus);
    }
}

void CBaseTalkUIProcessor::AdjustCustomSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    if (VecSoftkey.size() <= 0)
    {
        return;
    }

    //http://bugfree.yealink.com/Bug.php?BugID=132726
    //当softkey数量大于4(需要翻页)，不符合条件的softkey删除，softkey数量不大于4则将不符合条件的softkey置为Empty
    for (int i = 0; i < VecSoftkey.size();)
    {
        if (!AllowSoftkeyShow(VecSoftkey[i]))
        {
            if (VecSoftkey.size() > SOFTKEY_PAGE_COUNT)
            {
                // 如果不允许添加则删除
                VecSoftkey.removeAt(i);
                continue;
            }
            else
            {
                // 如果不允许添加则赋值为空的softkey
                VecSoftkey[i] = ST_EMPTY;
            }
        }

        ++i;
    }
}

bool CBaseTalkUIProcessor::AllowSoftkeyShow(SoftKey_TYPE eType)
{
    //http://bugfree.yealink.com/Bug.php?BugID=134148 之前遗漏了对TALK_UI_ACTION_SWITCH的判断
    if (ST_SWAP == eType || ST_SWITCH == eType)
    {
        if (talklogic_GetAmountOfSession() <= 1)
        {
            return false;
        }
    }
    else if (ST_NEWCALL == eType && !talklogic_BeAllowNewCall())
    {
        return false;
    }
    else if (ST_REJECT == eType && 0 == configParser_GetConfigInt(kszRejectIncomingCallSwitch))
    {
        return false;
    }

    return true;
}

void CBaseTalkUIProcessor::CutEmptySoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    if (VecSoftkey.size() <= 0)
    {
        return;
    }

    //如果超过一页，则从前往后删除空的softkey
    for (int i = 0; i < VecSoftkey.size();)
    {
        if (VecSoftkey.size() <= SOFTKEY_PAGE_COUNT)
        {
            break;
        }

        if (ST_EMPTY == VecSoftkey[i])
        {
            VecSoftkey.removeAt(i);
            continue;
        }

        ++i;
    }
}

//softkey处理
bool CBaseTalkUIProcessor::DealSoftkeyPress(SoftKey_TYPE eSoftkey)
{
    SendUIRequestAction(TALK_ACTION_KEYPRESS, 0);

    //是否允许softkey事件通过
    if (!AllowSoftKeyEventOnKeyLock(eSoftkey))
    {
        return true;
    }

    //判断是否需要播放softkey按键音
    if (IsSoftkeyNeedToPlayKeyTone(eSoftkey))
    {
        _TalkUILogicDelegate.PlayKeyTone(eSoftkey);
    }

    //如果子类有处理 那么直接返回true
    if (true == OnSoftkeyPress(eSoftkey))
    {
        return true;
    }

    bool bHandled = false;
    switch (eSoftkey)
    {
    case ST_CANCEL:
    case ST_ENDCALL:
        {
            bHandled = SendUIRequestAction(TALK_ACTION_CANCEL);
        }
        break;
//     case ST_LINE:
//         {
//             if (acc_GetRegisteredAccountNum() > 1)
//             {
//                 bHandled = SendUIRequestAction(TALK_ACTION_ACCOUNT_CHANGE, 0, 1);
//             }
//         }
//         break;
    case ST_HOLD:
        {
            bHandled = SendUIRequestAction(TALK_ACTION_HOLD);
        }
        break;
    case ST_RESUME:
        {
            bHandled = SendUIRequestAction(TALK_ACTION_RESUME);
        }
        break;
    case ST_SPLIT:
        {
            bHandled = SendUIRequestAction(TALK_ACTION_SPLIT);
        }
        break;
    case ST_NEWCALL:
        {
            bHandled = SendUIRequestAction(TALK_ACTION_NEWCALL);
        }
        break;
    case ST_CALLINFORWARD:
        {
            bHandled = SendUIRequestAction(TALK_ACTION_FWD_TO);
        }
        break;
    case ST_MUTE:
        {
            bHandled = SendUIRequestAction(TALK_ACTION_MUTE);
        }
        break;
    case ST_SWAP:
        {
            SendUIRequestAction(TALK_ACTION_SWAP);

            bHandled = true;
        }
        break;
    case ST_SWITCH:
        {
            bHandled = true;
            talklogic_SwitchFocusSession(true);
        }
        break;
        //录制相关
        //开始录制
#if IF_TALKUI_USB_ENABLE

    case ST_START_RECORD:
        {
            bHandled = true;

            //键盘锁锁上 那么不允许继续操作
            if (keyGuard_IsLock() && (keyGuard_GetKeyGuardType() != KG_MENU_KEY))
            {
                //不允许通过后需要弹出解锁框
                keyGuard_IsEnableModuld(KGM_MODULE_DSSKEY);

                return true;
            }

            //处理开始录制视频或音频
            DealBeginRecord();

        }
        break;
    //停止录制
    case ST_STOP_RECORD:
        {
            bHandled = true;

            //键盘锁锁上 那么不允许继续操作
            if (keyGuard_IsLock() && (keyGuard_GetKeyGuardType() != KG_MENU_KEY))
            {
                //不允许通过后需要弹出解锁框
                keyGuard_IsEnableModuld(KGM_MODULE_DSSKEY);

                return true;
            }

            //处理停止录制
            DealStopRecord();

        }
        break;

    //暂停录制
    case ST_PAUSE_RECORD:
        {
            bHandled = true;

            //键盘锁锁上 那么不允许继续操作
            if (keyGuard_IsLock() && (keyGuard_GetKeyGuardType() != KG_MENU_KEY))
            {
                //不允许通过后需要弹出解锁框
                keyGuard_IsEnableModuld(KGM_MODULE_DSSKEY);

                return true;
            }

            //处理暂停录制
            DealPauseRecord();
        }
        break;

    //恢复录制
    case ST_RESUME_RECORD:
        {
            bHandled = true;

            //键盘锁锁上 那么不允许继续操作
            if (keyGuard_IsLock() && (keyGuard_GetKeyGuardType() != KG_MENU_KEY))
            {
                //不允许通过后需要弹出解锁框
                keyGuard_IsEnableModuld(KGM_MODULE_DSSKEY);

                return true;
            }

            //处理停止录制
            DealResumeRecord();
        }
        break;

    //截图
    case ST_SCREENSHOT:
        {
            bHandled = true;

            //处理截图
            DealScreenshot();

        }
        break;
#endif          // IF_TALKUI_USB_ENABLE
#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    case ST_RECORD_START:
        {
            CCallInfo * pCallInfo = GetFocusCallInfo();
            if (pCallInfo != NULL && pCallInfo->m_eRecordResult != CALL_RECORD_ACTION_PROCESSING)
            {
                pCallInfo->m_eRecordResult = CALL_RECORD_ACTION_PROCESSING;
                SendUIRequestAction(TALK_ACTION_RECORD_START);
            }
            else
            {
                ShowDealLastNote();
            }

            bHandled = true;
        }
        break;
    case ST_RECORD_STOP:
        {
            CCallInfo * pCallInfo = GetFocusCallInfo();
            if (pCallInfo != NULL && pCallInfo->m_eRecordResult != CALL_RECORD_ACTION_PROCESSING)
            {
                pCallInfo->m_eRecordResult = CALL_RECORD_ACTION_PROCESSING;
                SendUIRequestAction(TALK_ACTION_RECORD_STOP);
            }
            else
            {
                ShowDealLastNote();
            }

            bHandled = true;
        }
        break;
    case ST_RECORD_PAUSE:
        {
            CCallInfo * pCallInfo = GetFocusCallInfo();
            if (pCallInfo != NULL && pCallInfo->m_eRecordResult != CALL_RECORD_ACTION_PROCESSING)
            {
                pCallInfo->m_eRecordResult = CALL_RECORD_ACTION_PROCESSING;
                SendUIRequestAction(TALK_ACTION_RECORD_PAUSE);
            }
            else
            {
                ShowDealLastNote();
            }

            bHandled = true;
        }
        break;
    case ST_RECORD_RESUME:
        {
            CCallInfo * pCallInfo = GetFocusCallInfo();
            if (pCallInfo != NULL && pCallInfo->m_eRecordResult != CALL_RECORD_ACTION_PROCESSING)
            {
                pCallInfo->m_eRecordResult = CALL_RECORD_ACTION_PROCESSING;
                SendUIRequestAction(TALK_ACTION_RECORD_RESUME);
            }
            else
            {
                ShowDealLastNote();
            }

            bHandled = true;
        }
        break;
#endif

#ifdef IF_FEATURE_ENTRANCE_GUARD
    case ST_OPENDOOR:
        {
            bHandled = SendUIRequestAction(TALK_ACTION_OPENDOOR);
        }
        break;
#endif

    default:
        break;
    }

    return bHandled;
}

#if IF_TALKUI_USB_ENABLE
//添加 录制相关softkey
bool CBaseTalkUIProcessor::AddUSBRecordSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    //当前如果是处于 视频 那么添加 停止录制的softkey
    if (modRecord_IsRecording(RECORD_VIDEO))
    {
        VecSoftkey.push_back(ST_STOP_RECORD);

        return true;
    }

    //没有视频存在 那么要录制 音频   可以录制音频 那么加softkey
    if (modRecord_IsRecordEnable(RECORD_AUDIO))
    {
        if (modRecord_IsRecording(RECORD_AUDIO))
        {
            VecSoftkey.push_back(ST_STOP_RECORD);
            VecSoftkey.push_back(ST_PAUSE_RECORD);
        }
        else if (modRecord_IsRecordPause(RECORD_AUDIO))
        {
            VecSoftkey.push_back(ST_STOP_RECORD);
            VecSoftkey.push_back(ST_RESUME_RECORD);
        }
        else
        {
            VecSoftkey.push_back(ST_START_RECORD);
        }

        return true;
    }

    return false;
}

//添加 USB截图相关softkey
void CBaseTalkUIProcessor::AddUSBScreenshotSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{

}

//处理开始录音
void CBaseTalkUIProcessor::DealBeginRecord()
{
    if (!modRecord_IsStorageConnect()
            || modRecord_IsRecording(RECORD_AUDIO))
    {
        return;
    }

    StorageInfo strginfo;
    modRecord_GetStorageInfo(strginfo);
    long lAvaliableTime = strginfo.m_ulAudioRecordTime;
    if (lAvaliableTime > 0)
    {
#ifndef _WIN32
#warning talklogic_GetCallInfoBySessionId
#endif
#if 0
        //获取焦点的联系人信息
        int iSessionID = talklogic_GetFocusedSessionID();
        if (iSessionID == -1)
        {
            TALKUI_WARN("Get focus session fail.");
            return;
        }
        CCallInfo* pCallInfo = talklogic_GetCallInfoBySessionId(iSessionID);
        if (NULL == pCallInfo)
        {
            TALKUI_WARN("Cannot get call info.");
            return;
        }
        CContactInfo objContactInfo;
        GetContactInfoFromCallInfo(objContactInfo, pCallInfo);

        const char * pName = objContactInfo.strName.c_str();
#else
        CContactInfo objContactInfo;
        bool bResult = GetFocusContactInfo(objContactInfo);

        const char * pName = objContactInfo.strName.c_str();
#endif

        modRecord_RecordStart(RECORD_AUDIO, pName);

        yl::string strHint = _UILOGIC_LANG_TRAN(TRID_RECORD_TIME);
        char bufTip[100] = {0};
        if (lAvaliableTime > 3600)
        {
            sprintf(bufTip, ":\n%.1fh", lAvaliableTime / (float)3600);
        }
        else
        {
            sprintf(bufTip, ":\n%.1fmin", lAvaliableTime / (float)60);
        }
        strHint += bufTip;

        _TalkUILogicFrameManager.UpdateMsgboxNoteNotify(strHint);
    }
    else
    {
        _TalkUILogicFrameManager.UpdateMsgboxNoteNotify(_UILOGIC_LANG_TRAN(TRID_USB_SPACE_FULL));
    }
}

//处理停止录音
void CBaseTalkUIProcessor::DealStopRecord()
{
    if (!modRecord_IsRecording(RECORD_AUDIO)
            && !modRecord_IsRecordPause(RECORD_AUDIO))
    {
        return;
    }

    modRecord_RecordStop();
}

//处理停止录音
void CBaseTalkUIProcessor::DealPauseRecord()
{
    if (!modRecord_IsRecording(RECORD_AUDIO))
    {
        return;
    }

    modRecord_RecordPause();
}

//处理停止录音
void CBaseTalkUIProcessor::DealResumeRecord()
{
    if (!modRecord_IsRecordPause(RECORD_AUDIO))
    {
        return;
    }

    modRecord_RecordResume();
}

//处理截图
void CBaseTalkUIProcessor::DealScreenshot()
{
    //键盘锁锁上 那么不允许继续操作
    if (keyGuard_IsLock() && (keyGuard_GetKeyGuardType() != KG_MENU_KEY))
    {
        //不允许通过后需要弹出解锁框
        keyGuard_IsEnableModuld(KGM_MODULE_DSSKEY);

        return ;
    }

    //获取焦点的联系人信息
    CContactInfo objContactInfo;
    bool bResult = GetFocusContactInfo(objContactInfo);

    const char * pName = objContactInfo.strName.c_str();

    modRecord_Screenshot(pName);
}
#endif

//获取当前话路的焦点联系人信息
bool CBaseTalkUIProcessor::GetFocusContactInfo(CContactInfo & objContactInfo)
{
    return false;
}

//设置数据之前做的一些操作， 在这是先提取了联系人信息
void CBaseTalkUIProcessor::DealSetTalkData(void * pData, UPDATE_REASON eReason)
{
    m_eNetworkStatus = TALK_NETWORK_STATUS_UNKNOWN;

    //给自身设置数据
    SetTalkData(pData, eReason);

    //设置title
    SetTitle(pData, eReason);

    //设置 icons
    SetIcons(pData, eReason);

    SetDNDIcon();

    //挪到外面单独处理  在当前函数中只是把数据设置到processor
    ////给子界面设置数据
    //SetTalkUIData(eReason);

    PreLoadCustomSoftkey();
}

void CBaseTalkUIProcessor::SetDNDIcon()
{
    if (!configParser_GetConfigInt(kszKeepDNDAfterTalk)
            && dnd_IsEnable(acc_GetDefaultAccount()))
    {
        AddTalkUIIcon(TALKUI_ICON_DND, true);
    }
}

bool CBaseTalkUIProcessor::OnSoftkeyPress(SoftKey_TYPE eSoftkey)
{
    bool bHandled = false;
    switch (eSoftkey)
    {
    case ST_CANCEL:
    case ST_ENDCALL:
        {
            bHandled = SendUIRequestAction(TALK_ACTION_CANCEL);
        }
        break;
//     case ST_LINE:
//         {
//             if (acc_GetRegisteredAccountNum() > 1)
//             {
//                 bHandled = SendUIRequestAction(TALK_ACTION_ACCOUNT_CHANGE);
//             }
//         }
//         break;
    case ST_HOLD:
        {
            bHandled = SendUIRequestAction(TALK_ACTION_HOLD);
        }
        break;
    case ST_RESUME:
        {
            bHandled = SendUIRequestAction(TALK_ACTION_RESUME);
        }
        break;
    case ST_SPLIT:
        {
            bHandled = SendUIRequestAction(TALK_ACTION_SPLIT);
        }
        break;
    case ST_NEWCALL:
        {
            bHandled = SendUIRequestAction(TALK_ACTION_NEWCALL);
        }
        break;
    case ST_CALLINFORWARD:
        {
            bHandled = SendUIRequestAction(TALK_ACTION_FWD_TO);
        }
        break;
    case ST_MUTE:
        {
            bHandled = SendUIRequestAction(TALK_ACTION_MUTE);
        }
        break;
    case ST_SWITCH:
        {
            bHandled = true;
            talklogic_SwitchFocusSession(true);
        }
        break;
    default:
        break;
    }
    return bHandled;
}

// 是否允许给逻辑层发送Action
bool CBaseTalkUIProcessor::IsAllowSendAction(TALK_ACTION_TYPE eActionType)
{
    // 键盘锁未上锁,或者处于MenuKey的状态
    // 允许所有Action通过
    if (!keyGuard_IsLock()
            || keyGuard_GetKeyGuardType() == KG_MENU_KEY)
    {
        return true;
    }

    // 在Allkeys、Funkey开启的时候只允许接听呼出和接听通话
    return (eActionType == TALK_ACTION_SEND
            || eActionType == TALK_ACTION_CANCEL
            || eActionType == TALK_ACTION_ANSWER
            || eActionType == TALK_ACTION_KEYPRESS
            || eActionType == TALK_ACTION_TEXT_CHANGE
            || eActionType == TALK_ACTION_NEWCALL
            || eActionType == TALK_ACTION_REJECT
            || eActionType == TALK_ACTION_FOCUSE_CHANGE
            || eActionType == TALK_ACTION_SCREEN_CLICK
            || eActionType == TALK_ACTION_VOLUME_CHANGE
            || eActionType == TALK_ACTION_VOLUMEBAR_SHOW
            || eActionType == TALK_ACTION_WND_FOCUSE_CHANGE
            || eActionType == TALK_ACTION_EXIT_FAILD_INFO
            //在通话过程中按数字键属于DTMF
            || eActionType == TALK_ACTION_DTMF
            //不允许hold但运行resume，因为进入line界面后会自动hold
            || eActionType == TALK_ACTION_RESUME
            || eActionType == TALK_ACTION_ACCOUNT_CHANGE
//             || eActionType == TALK_ACTION_JOIN
           );
}

//UI界面请求操作
bool CBaseTalkUIProcessor::SendUIRequestAction(TALK_ACTION_TYPE eActionType, WPARAM wActionParam,
        LPARAM lActionParam, void * pExtraData)
{
    //放到talkuilogicdelegate去处理， 因为有可能外部直接调用talkuilogicdelegate的接口
    ////是否允许该action 通过
    //if (!IsAllowSendAction(eActionType))
    //{
    //  //不允许通过后需要弹出解锁框
    //  keyGuard_IsEnableModuld(KGM_MODULE_DSSKEY);
    //  return true;
    //}

    return _TalkUILogicDelegate.SendUIRequestAction(GetSessionID(), eActionType, wActionParam,
            lActionParam, pExtraData);

//  return talklogic_UIEvent(m_iSessionID, eActionType, wActionParam, lActionParam, pExtraData);
}


//获取Label  Label 不存在那么 获取GetDisplayName 函数数据
yl::string CBaseTalkUIProcessor::GetLabel(int iAccountID)
{
    yl::string strLabel = _TalkUILogicDelegate.GetLabelFromLogic(iAccountID);

    if (strLabel.empty())
    {
        strLabel = GetDisplayName(iAccountID);
    }

    return strLabel;
}

//获取Displayname Display 如果获取不成功 那么用 GetUserName 函数数据
yl::string CBaseTalkUIProcessor::GetDisplayName(int iAccountID)
{
    yl::string strDisplayName = _TalkUILogicDelegate.GetDisplayNameFromLogic(iAccountID);

    if (strDisplayName.empty())
    {
        strDisplayName = GetUserName(iAccountID);
    }

    return strDisplayName;
}

//获取UserName 数据
yl::string CBaseTalkUIProcessor::GetUserName(int iAccountID)
{
    return _TalkUILogicDelegate.GetUserNameFromLogic(iAccountID);
}

//添加通话图标  bAdd=tru表示添加， bAdd=false表示删除
void CBaseTalkUIProcessor::AddTalkUIIcon(TALKUI_ICON eTalkUIIcon, bool bAdd)
{
    YLList<TALKUI_ICON>::iterator iteList;

    //如果是添加
    if (true == bAdd)
    {
        iteList = m_listIcons.begin();

        for (; m_listIcons.end() != iteList ;)
        {
            //有相同的直接返回 不需要添加了
            if (*iteList == eTalkUIIcon)
            {
                return ;
            }

            iteList++;
        }

        //这边也就是没有查找到 直接插入
        if (m_listIcons.end() == iteList)
        {
            m_listIcons.push_back(eTalkUIIcon);
        }
    }
    else    //如果是删除
    {
        iteList = m_listIcons.begin();

        for (; m_listIcons.end() != iteList ;)
        {
            //有相同的直接删除
            if (*iteList == eTalkUIIcon)
            {
                m_listIcons.erase(iteList);
                return ;
            }

            iteList++;
        }
    }
}

// 是否允许键盘事件通过
bool CBaseTalkUIProcessor::AllowKeyPadEventOnKeyLock(int nKeyCode, bool bPress)
{
    // 键盘锁开启的时候才进行判断
    if (!keyGuard_IsLock())
    {
        return true;
    }

    KEY_GUARD_TYPE eGuyardType = keyGuard_GetKeyGuardType();
    if (!AllowKeyPadResponseEvent(eGuyardType, nKeyCode, bPress))
    {
        return false;
    }

    return true;
}

bool CBaseTalkUIProcessor::AllowSoftKeyEventOnKeyLock(SoftKey_TYPE eSoftkey)
{
    // 键盘锁未上锁,或者处于MenuKey的状态
    // 允许所有Action通过
    if (!keyGuard_IsLock()
            || keyGuard_GetKeyGuardType() == KG_MENU_KEY)
    {
        return true;
    }

#if IF_TALKUI_USB_ENABLE
    if (eSoftkey == ST_START_RECORD
            || eSoftkey == ST_STOP_RECORD
            || eSoftkey == ST_PAUSE_RECORD
            || eSoftkey == ST_RESUME_RECORD
            || eSoftkey == ST_SCREENSHOT)
    {
        // 弹出解锁框
        keyGuard_IsEnableModuld(KGM_MODULE_DSSKEY);
        return false;
    }
#endif

    KEY_GUARD_TYPE eGuyardType = keyGuard_GetKeyGuardType();
    //虚函数判断当前UI逻辑是否允许softkey事件通过
    if (!AllowSoftkeyResponseEvent(eSoftkey, eGuyardType))
    {
        // 弹出解锁框
        keyGuard_IsEnableModuld(KGM_MODULE_DSSKEY);
        return false;
    }

    return true;
}


// 判断是否需要播放按键音
bool CBaseTalkUIProcessor::IsNeedToPlayKeyTone(int iKey)
{
    return false;
}

// 通道键处理
bool CBaseTalkUIProcessor::DealChannelKeyPress(int iKey)
{
    SendUIRequestAction(TALK_ACTION_KEYPRESS, iKey);

    //播放按键音
    if (IsNeedToPlayKeyTone(iKey))
    {
        _TalkUILogicDelegate.PlayKeyTone(iKey);
    }

    // http://192.168.1.99/Bug.php?BugID=19141
    // 焦点是通话界面，且耳麦打开的情况下不再处理channel按键
#ifdef IF_FEATURE_HEADSETKEY_IN_TALK
    //当前有显示通话子界面
    //是否拨号界面在Park 状态
    bool bIsDialParkStatus = false;

    //如果是在拨号界面
    if (GetProcessorName() == PROCESSORNAME_DIAL)
    {
        CDialProcessor * pDialProcessor = (CDialProcessor *)this;
        //如果UI类型是CallPark  那么才是
        if (DIAL_UI_CALLPARK == pDialProcessor->GetDialUIType())
        {
            bIsDialParkStatus = true;
        }
    }
    // http://192.168.1.99/Bug.php?BugID=19141
    // 焦点是通话界面，且耳麦打开的情况下不再处理channel按键
    if (configParser_GetConfigInt(kszHeadSetKeyInTalkSwitch) == 0
            && voice_GetHeadsetStatus()
            && (GetProcessorName() == PROCESSORNAME_TALKING                  //普通通话
                || GetProcessorName() == PROCESSORNAME_CONF             //会议
                || GetProcessorName() == PROCESSORNAME_CONFMANAGER              //会议管理
                || GetProcessorName() == PROCESSORNAME_NETCONF              //网络会议
                || GetProcessorName() == PROCESSORNAME_NETCONFMGR              //网络会议管理
                || GetProcessorName() == PROCESSORNAME_RTPSTATUS                    //RTP状态下
                || bIsDialParkStatus                                    //拨号park状态下
               ))
    {
        //更新通道图标
        UpdateUIChannelIcon();

        return false;
    }

#endif

    // 这边不能再交给voice 处理了  因为在qT46Application::PreKeyHookBegin 函数中会处理一次！！！！！！
    //  bHandled = voice_OnKeyPress(iKey);

    bool bHandled = false;
    // 双耳麦模式，摘挂机只更改通道，不再继续往下传
    if (voice_IsCallCenterMode()
            && (iKey == PHONE_KEY_HANDSET_OFF_HOOK
                || iKey == PHONE_KEY_HANDSET_ON_HOOK))
    {
        bHandled = true;
    }
    else
    {
        bHandled = OnChannelKeyPress(iKey);
    }

    //可能这个时候已经退出了通话
    if (AdapterBox_IsInTalk())
    {
        TALKUI_INFO("basetalkuiprocessor dealchannelkey before UpdateUIChannelIcon() this=0x%x", this);

        CBaseTalkUIProcessor * pTalkUIProcessor = _TalkUILogicFrameManager.GetCurrentTalkUIProcessor();

        //处理完通道键后 需要更新通道
        if (NULL != pTalkUIProcessor)
        {
            pTalkUIProcessor->UpdateUIChannelIcon();
        }

        TALKUI_INFO("basetalkuiprocessor dealchannelkey end UpdateUIChannelIcon() this=0x%x", this);

        //这个要放在 通话处理之后， 因为这个时候可能 已经退出了通话界面了， 避免退到Idle后还出现音量条
        //需要放在更新通道 图标之后 因为 更新通道图标会把m_bIsShowVolume 的状态值 刷新
        if (PHONE_KEY_HANDFREE == iKey
                || PHONE_KEY_HANDSET == iKey
                || PHONE_KEY_HANDSET_OFF_HOOK == iKey
                || PHONE_KEY_HANDSET_ON_HOOK == iKey
                || PHONE_KEY_HEADSET_CTRL == iKey
           )
        {
            //摘手柄或者按免提导致声道切换，需要显示切换后声道的音量
            //通过模拟按键显示音量条
            if (NULL != pTalkUIProcessor)
            {
                if ((pTalkUIProcessor->GetProcessorName() != PROCESSORNAME_RING)
                        && (pTalkUIProcessor->GetProcessorName() != PROCESSORNAME_ACDCALLINFO)
                   )
                {
                    _TalkUILogicFrameManager.UpdateVolumebar(true);

                    if (S_GetIsShowVolume())
                    {
//                         AdapterBox_SendKey(PHONE_KEY_VOLUME_DECREASE, true);

                        S_SetIsShowVolume(false);
                    }
                }
            }
        }
    }

    return bHandled;

}

// 按键预处理
bool CBaseTalkUIProcessor::DealKeyPreProcess(int iKey)
{
    return false;
}

// 功能键处理
bool CBaseTalkUIProcessor::DealFunckeyPress(int iKey)
{
    SendUIRequestAction(TALK_ACTION_KEYPRESS, iKey);

    //播放按键音
    if (IsNeedToPlayKeyTone(iKey))
    {
        _TalkUILogicDelegate.PlayKeyTone(iKey);
    }

    return OnFunckeyPress(iKey);
}

//处理数字键
bool CBaseTalkUIProcessor::DealDigitkeyPress(int iKey)
{
    SendUIRequestAction(TALK_ACTION_KEYPRESS, iKey);

    //播放按键音
    if (IsNeedToPlayKeyTone(iKey))
    {
        _TalkUILogicDelegate.PlayKeyTone(iKey);
    }

    return OnDigitkeyPress(iKey);
}

//触屏事件处理
bool CBaseTalkUIProcessor::DealScreenPress()
{
    SendUIRequestAction(TALK_ACTION_SCREEN_CLICK);

    return true;
}

//UpdateWnd事件处理
bool CBaseTalkUIProcessor::DealUpdateWnd()
{
    //先调用一下 处理
    OnUpdateWnd();

    //刷新title
    _TalkUILogicFrameManager.UpdateTalkUITitle(GetTitle());

    //最后都需要刷新一下通道图标
    UpdateUIChannelIcon();

    //刷新一下pagenum
    _TalkUILogicFrameManager.UpdateTalkUIPageNum();

    //刷新通话时间
    UpdateTalkUITime();

    //一般来说先刷新普通图标在刷新蓝牙图标  因为更新普通图标会清空 普通图标和蓝牙图标
    //刷新图标
    _TalkUILogicFrameManager.UpdateTalkUIIcons();

    //刷新通道图标
    _TalkUILogicFrameManager.UpdateBlueToothIcon();

    return false;
}

// 焦点改变.
// | bFocused | [in] 为将要改变的焦点状态. true为获取焦点，false为失去焦点
void CBaseTalkUIProcessor::OnFocusChanged(bool bFocused)
{
    SendUIRequestAction(TALK_ACTION_WND_FOCUSE_CHANGE, (int)bFocused);
}

//这边就不用做了  直接在basetalkuiprocessor 的时候 也同时执行 volumnvalue 改变的事件
////音量条显示状态改变  要不然逻辑层不会刷新状态  ring界面的silence， softkey 按下 可能无效， 因为逻辑层认为当前已经处于silence状态了
void CBaseTalkUIProcessor::OnTalkVolumnVisibleChange(bool bVisible)
{
    if (true == bVisible)
    {
        SendUIRequestAction(TALK_ACTION_VOLUMEBAR_SHOW);

        //这边还是要调用一下这个 因为 给逻辑层发送 TALK_ACTION_VOLUME_CHANGE 事件会让逻辑层刷新界面
        //这样逻辑层的softkey才能起作用
        OnTalkVolumnValueChange();
    }
}

//音量条变化调用函数
void CBaseTalkUIProcessor::OnTalkVolumnValueChange()
{
    int nValue = voice_GetVolume();
    //发送音量变化的事件给逻辑层
    SendUIRequestAction(TALK_ACTION_VOLUME_CHANGE, nValue);
}

////处理UI事件
//void CBaseTalkUIProcessor::OnTalkUIAction(SoftKey_TYPE eSoftkeyType)
//{
//  OnSoftkeyPress(eSoftkeyType);
//
//}

// 通道按键处理
bool CBaseTalkUIProcessor::OnChannelKeyPress(int iKey)
{
#if IF_TALKUI_HANDFREE_DIAL
    if (PHONE_KEY_HANDFREE == iKey)
    {
        voice_SwitchChannel(CHANNEL_HANDFREE);
        return true;
    }
#endif

    bool bHandled = false;

    if (IsChannelKeyToExit(iKey))
    {
        bHandled = true;

        int nlParam = 0;

#if IF_TALKUI_ONHOOK_EXITCALL
        // CP通道键引起的退出LParam为1:允许Hold时退出
        if (PHONE_KEY_HANDSET_ON_HOOK == iKey)
        {
            nlParam = 1;
        }
#endif

        SendUIRequestAction(TALK_ACTION_CANCEL, 1, nlParam);

        // 如果由通道键引发的退出，且还有通话存在，则切换通道
        if (talklogic_SessionExist())
        {
            voice_SwitchChannel();
        }
    }
    else
    {
        // 呼叫中心模式，免提模式下按免提键可以切换成耳麦通道
        if (voice_IsCallCenterMode() && iKey == PHONE_KEY_HANDFREE)
        {
            if (!voice_GetHandfreeStatus())
            {
                if (voice_GetHandsetStatus())
                {
                    voice_SwitchChannel(CHANNEL_2HEADSET);
                }
                else
                {
                    voice_SwitchChannel(CHANNEL_HEADSET);
                }

                bHandled = true;
            }
        }
    }

    return bHandled;
}


// 判断Softkey是否需要播放按键音
bool CBaseTalkUIProcessor::IsSoftkeyNeedToPlayKeyTone(SoftKey_TYPE softkeyType)
{
    return false;
}


bool CBaseTalkUIProcessor::AllowKeyPadResponseEvent(KEY_GUARD_TYPE eGuyardType, int nKeyCode,
        bool bPress)
{
    // 音量声道按键不做限制
    if (nKeyCode == PHONE_KEY_VOLUME_INCREASE
            || nKeyCode == PHONE_KEY_VOLUME_DECREASE
            || nKeyCode == PHONE_KEY_HEADSET_CTRL)
    {
        return true;
    }

    // 判断是否允许按键通过
    bool bAllow = false;
    switch (keyGuard_GetKeyGuardType())
    {
    case KG_ALL_KEYS:
    case KG_FUN_KEY:
        {
            bAllow = (nKeyCode == PHONE_KEY_OK
                      || nKeyCode == PHONE_KEY_HANDFREE
                      || nKeyCode == PHONE_KEY_HANDSET
                      || nKeyCode == PHONE_KEY_HANDSET_ON_HOOK
                      || nKeyCode == PHONE_KEY_HANDSET_OFF_HOOK
                      //|| nKeyCode == PHONE_KEY_TRANSFER
                      //|| nKeyCode == PHONE_KEY_HOLD
                      //|| nKeyCode == PHONE_KEY_CONFERENCE
                      || nKeyCode == PHONE_KEY_HEADSET_OUT
                      || nKeyCode == PHONE_KEY_HEADSET_IN
                      || nKeyCode == PHONE_KEY_EHS_OPEN
                      || nKeyCode == PHONE_KEY_EHS_CLOSE
                      || nKeyCode == PHONE_KEY_STAR
                      || nKeyCode == PHONE_KEY_POUND
                      || nKeyCode == PHONE_KEY_CANCEL
                      || (PHONE_KEY_0 <= nKeyCode && nKeyCode <= PHONE_KEY_9)
                      || (PHONE_KEY_LINE1 <= nKeyCode && nKeyCode <= PHONE_KEY_LINE10)
                      //|| (PHONE_KEY_LEFT <= nKeyCode && nKeyCode <= PHONE_KEY_DOWN)
                      || (PHONE_KEY_FN1 <= nKeyCode && nKeyCode <= PHONE_KEY_FN4));
        }
        break;
    case KG_MENU_KEY:
        {
            bAllow = true;
        }
        break;
    default:
        {
            bAllow = false;
        }
        break;
    }

    return bAllow;
}

//softkey事件是否通过逻辑分开处理
bool CBaseTalkUIProcessor::AllowSoftkeyResponseEvent(SoftKey_TYPE eSoftkey,
        KEY_GUARD_TYPE eGuyardType)
{
    // 键盘锁未上锁,或者处于MenuKey的状态
    // 允许所有Action通过
    if (eGuyardType == KG_MENU_KEY)
    {
        return true;
    }

    // 在Allkeys、Funkey开启的时候只允许接听呼出和接听通话
    return (eSoftkey == ST_SEND
            || eSoftkey == ST_CANCEL
            || eSoftkey == ST_ANSWER
            || eSoftkey == ST_DELETE
            || eSoftkey == ST_IMECHANGE
            //|| eSoftkey == TALK_ACTION_KEYPRESS
            //|| eSoftkey == TALK_ACTION_TEXT_CHANGE
            || eSoftkey == ST_NEWCALL
            || eSoftkey == ST_REJECT
            /*|| eSoftkey == TALK_ACTION_FOCUSE_CHANGE
            || eSoftkey == TALK_ACTION_SCREEN_CLICK
            || eSoftkey == TALK_ACTION_VOLUME_CHANGE
            || eSoftkey == TALK_ACTION_VOLUMEBAR_SHOW
            || eSoftkey == TALK_ACTION_WND_FOCUSE_CHANGE
            || eSoftkey == TALK_ACTION_EXIT_FAILD_INFO*/
            //在通话过程中按数字键属于DTMF
            /*|| eSoftkey == TALK_ACTION_DTMF*/
            //不允许hold但运行resume，因为进入line界面后会自动hold
            || eSoftkey == ST_RESUME
            || eSoftkey == ST_ENDCALL
            || eSoftkey == ST_MORE
            || eSoftkey == ST_SILENCE
            || eSoftkey == ST_EXIT
            /*|| eSoftkey == TALK_ACTION_ACCOUNT_CHANGE*/
//             || eSoftkey == ST_JOIN
           );
}

// 切换通话账号
void CBaseTalkUIProcessor::ChangeAccount(int iAccountID, bool bRefresh)
{
    if (true == bRefresh)
    {
        //呼出
        SendUIRequestAction(TALK_ACTION_ACCOUNT_CHANGE, iAccountID, 1);
    }
    else
    {
        //不呼出
        SendUIRequestAction(TALK_ACTION_ACCOUNT_CHANGE, iAccountID, 0);
    }
}

// 是否需要锁住按键
bool CBaseTalkUIProcessor::IsNeedLockKey(int iLockType, int iKey)
{
    switch (iLockType)
    {
    case KG_ALL_KEYS:
    case KG_FUN_KEY:
        {
            return (PHONE_KEY_OK != iKey // 不锁OK
                    && PHONE_KEY_CANCEL != iKey // 不锁Cancel
                    && !voice_IsChannelKey(iKey)); // 不锁声道
        }
        break;
    default:
        break;
    }

    return false;
}

// 是否需要锁指定softkey
bool CBaseTalkUIProcessor::IsNeedLockSoftKey(int iLockType, int eSoftKey)
{
    switch (iLockType)
    {
    case KG_ALL_KEYS:
    case KG_FUN_KEY:
        {
            return (ST_ANSWER != eSoftKey   // 不锁接听
                    && ST_REJECT != eSoftKey // 不锁拒接
                    && ST_EXIT != eSoftKey  // 不锁退出
                    && ST_DELETE != eSoftKey
                    && ST_IMECHANGE != eSoftKey
                    && ST_ENDCALL != eSoftKey
                    && ST_CANCEL != eSoftKey
                    && ST_MORE != eSoftKey //不锁换页
                    && ST_NEWCALL != eSoftKey); // 不锁NewCall
        }
        break;
    default:
        break;
    }

    return false;
}

// 是否*、#呼出键
bool CBaseTalkUIProcessor::IsSendDigitKey(yl::string strEditContent, int iKey)
{
    // 0--Disable，1--#，2--*
    int iSendKey = _TalkUILogicDelegate.GetKeyAsSend();
    // 拨号界面能够输入##（就算#设置为呼出键，此时也要能在拨号界面输入##） 主要是为了 呼出##
    if ((iSendKey == 1 && iKey == PHONE_KEY_POUND
            && (!_TalkUILogicDelegate.IsEnableSendPoundKey()     //这个配置的意思 应该是能不能呼出## 这么理解
                || yl::string("#") != strEditContent))
            || iSendKey == 2 && iKey == PHONE_KEY_STAR)
    {
        return true;
    }

    return false;
}

//获取TargetInfo 信息 主要是给 通话、会议 提供新来电 targetinfo 信息  获取的优先级为 Label、DisplayName、UserName
yl::string CBaseTalkUIProcessor::GetTargetInfo()
{
#if TALKUI_DEBUG_TEST
    return "LAB";
#endif

    yl::string strTargetInfo = "";

    //需要显示targetinfo
    if (_TalkUILogicDelegate.IsShowTargetInfo())
    {
        int iAccountID = _TalkUILogicDelegate.GetTalkingIncomingCallTargetAccountId();

        strTargetInfo = _TalkUILogicDelegate.GetHotelingUserName(iAccountID);

        //获取到的hoteling账号为空
        if (strTargetInfo.empty())
        {
            strTargetInfo = GetLabel(iAccountID);
        }
    }

    return strTargetInfo;
}

// 通道按键是否退出
bool CBaseTalkUIProcessor::IsChannelKeyToExit(int iKey)
{
    bool bExit = false;
    switch (iKey)
    {
    case PHONE_KEY_HANDFREE:
        {
            if (!voice_GetHandfreeStatus() && !voice_GetHandsetStatus())
            {
                bExit = true;
            }
        }
        break;
    case PHONE_KEY_HANDSET_ON_HOOK:
        {
#if IF_TALKUI_ONHOOK_EXITCALL
            bExit = true;
#else
            if (!voice_GetHandfreeStatus() && !voice_GetHandsetStatus()
                    && voice_GetCurrentChannel() != CHANNEL_HEADSET)
            {
                bExit = true;
            }
#endif
        }
        break;
    case PHONE_KEY_EHS_CLOSE:
        {
            if (voice_GetCurrentChannel() == CHANNEL_HEADSET)
            {
                // 如果耳麦打开则退出
                bExit = true;
            }
        }
        break;
#ifdef IF_FEATURE_HEADSET_AS_CANCEL
    case PHONE_KEY_HEADSET_CTRL:
        {
            if (configParser_GetConfigInt(kszHeadsetAsCancel) != 0)
            {
                if (!voice_GetHeadsetStatus() && !voice_GetHandsetStatus())
                {
                    bExit = true;
                    // 耳麦键退出时，需要将通道切到耳麦，比如耳麦通话中，按耳麦退出，要求通道还是耳麦
                    voice_SwitchChannel(CHANNEL_HEADSET);
                }
            }
        }
        break;
#endif
    default:
        break;
    }

    return bExit;
}

//当前的 通道模式
CHANNEL_MODE CBaseTalkUIProcessor::s_eChannelMode = CHANNEL_NULL;
//是否显示音量条
bool CBaseTalkUIProcessor::s_bIsShowVolume = false;

CHANNEL_MODE CBaseTalkUIProcessor::S_GetCurrentChannelMode()
{
    return s_eChannelMode;
}

void CBaseTalkUIProcessor::S_SetCurrentChannelMode(CHANNEL_MODE eChannelMode)
{
    s_eChannelMode = eChannelMode;
}

bool CBaseTalkUIProcessor::S_GetIsShowVolume()
{
    return s_bIsShowVolume;
}

void CBaseTalkUIProcessor::S_SetIsShowVolume(bool bIsShowVolume)
{
    s_bIsShowVolume = bIsShowVolume;
}

void CBaseTalkUIProcessor::ShowDealLastNote()
{
    CUILogicBaseTalkUI * pUILogicBaseTalkUI = GetBindUI();
    if (NULL != pUILogicBaseTalkUI)
    {
        pUILogicBaseTalkUI->ShowDealLastNote();
    }
}

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
CCallInfo * CBaseTalkUIProcessor::GetFocusCallInfo()
{
    int iSessionId = talklogic_GetFocusedSessionID();
    return (CCallInfo *)talklogic_GetCallInfoBySessionID(iSessionId);
}

bool CBaseTalkUIProcessor::SetRecordSoftkey(YLVector<SoftKey_TYPE> & VecSoftkey)
{
    if (!IfNeedCallRecording())
    {
        return false;
    }

    int iSessionId = talklogic_GetFocusedSessionID();
    CCallInfo * pCallInfo = (CCallInfo *)talklogic_GetCallInfoBySessionID(iSessionId);
    if (pCallInfo == NULL
            || !callRecord_IsCallRecordOn(pCallInfo->GetAccountID()))
    {
        return false;
    }
    CALL_RECORDING_MODE eMode = callRecord_GetRecordMode(pCallInfo->GetAccountID());

    if (CALL_RECORD_NORMAL == m_eRecordStatue)
    {
        if (eMode == CRM_ALWAYS_PAUSE_RESUME
                || eMode == CRM_DEMAND
                || eMode == CRM_DEMAND_USER_START)
        {
            VecSoftkey.push_back(ST_RECORD_START);
        }
    }
    else if (CALL_RECORD_START == m_eRecordStatue)
    {
        if (eMode == CRM_ALWAYS_PAUSE_RESUME
                || eMode == CRM_DEMAND)
        {
            VecSoftkey.push_back(ST_RECORD_PAUSE);
        }
        else if (eMode == CRM_DEMAND_USER_START)
        {
            VecSoftkey.push_back(ST_RECORD_PAUSE);
            VecSoftkey.push_back(ST_RECORD_STOP);
        }
    }
    else if (CALL_RECORD_PAUSE == m_eRecordStatue)
    {
        if (eMode == CRM_ALWAYS_PAUSE_RESUME
                || eMode == CRM_DEMAND)
        {
            VecSoftkey.push_back(ST_RECORD_RESUME);
        }
        else if (eMode == CRM_DEMAND_USER_START)
        {
            VecSoftkey.push_back(ST_RECORD_RESUME);
            VecSoftkey.push_back(ST_RECORD_STOP);
        }
    }

    return true;
}

// 是否允许自定义softkey
bool CBaseTalkUIProcessor::IsAllowDefineRecordSoftkey()
{
    return true;
}

#endif


