#include "../include/uilogicmanager.h"
#include "../include/baseidlehandle.h"
#include "idlescreen/include/modidlescreen.h"
#include "broadsoft/callcenter/include/modcallcenter.h"

#include "talk/fwd_dnd/include/modfwddnd.h"
#include "dsskey/include/moddsskey.h"
#include "keyguard/include/modkeyguard.h"
#include "record/include/modrecord.h"
#include "settinguilogic/include/modsetuilogic.h"
#include "setting_page_define.h"
#include "adapterbox/include/modadapterbox.h"
#include "hotplug/commondefine.h"
#include "hotplug/modhotplug.h"
#include "extended/hotdesking/src/hotdesking.h"
#include "uilogiccommon/include/uilogiclanghelper.h"
#include "broadsoft/xsiactionhelper/include/xsicommon.h"
#include "lamp/powersaving/include/modpowersaving.h"
#include "lamp/powersaving/include/powersavingmsgdefine.h"
#include "interfacedefine.h"
#include "commonunits/modcommonunits.h"
#include "contacts/directory/include/moddirinterface.h"
#include "modidleul.h"
#include "autopcontrol/include/modautopcontrol.h"
#include "extended/xmlbrowser/include/logicanduicommon.h"
#if IF_FEATURE_METASWITCH
#include "metaswitch/mtswcontrol/include/modmtsw.h"
#endif

//网页发送参数类型
#define ST_POPUP_VOICE_MAIL     (0x01 << 0)
#define ST_POPUP_MISSED_CALL    (0x01 << 1)
#define ST_POPUP_FORWARD_CALL   (0x01 << 2)
#define ST_POPUP_TEXT_MSG       (0x01 << 3)

// 弹窗状态.
enum IdleFreeState
{
    IFS_NONE = 0,
    IFS_POPUP = 0x1 << 1,
    IFS_TEXT_NOTIFY = 0x1 << 2,
    IFS_SOFTKEY = 0x1 << 3,
    IFS_ACCOUNT = 0x1 << 4,
    IFS_ALL = 0xffffffff
};

namespace
{
#define     IDLE_LONG_PRESS_TIME_5S     5000
#define     RESET_SCRIPT_COMMAND        "reset-httpd.sh &"
#define     RESET_FACTORY_COMMAND       "reset_to_factory"
}


IMPLEMENT_GETINSTANCE(CUiLogicManager)

//softkey 回调
void SoftkeyCallBack(LIST_IDLE_STATUS_ITEM& Statuslist)
{
    _UiIdleLogicManager.IdleLogicSoftkeyCallBack(Statuslist);
}

//Icon回调
void IconCallBack(LIST_IDLE_STATUS_ITEM& Statuslist)
{
    _UiIdleLogicManager.IdleLogicIconCallBack(Statuslist);
}

#if IF_SUPPORT_TEXT_NOTIFY
//Text回调
void TextCallBack(LIST_IDLE_STATUS_ITEM& Statuslist)
{
    _UiIdleLogicManager.IdleLogicTextCallBack(Statuslist);
}
#endif

void PopUpCallBack(LIST_IDLE_STATUS_ITEM& Statuslist)
{
    IDLEUI_INFO("Logic PopUpCallBack.");
    _UiIdleLogicManager.IdleLogicPopupCallBack(Statuslist);
}

//账号状态回调
void AccountCallBack(LIST_IDLE_STATUS_ITEM& Statuslist)
{
    _UiIdleLogicManager.IdleLogicAccountCallBack(Statuslist);
}

void XmlBrowswerCallBack()
{
    _UiIdleLogicManager.IdleLogicUpdateCustomNotify();
}

CUiLogicManager::CUiLogicManager()
    : m_pIdleUi(NULL)
    , m_nPressedKey(PHONE_KEY_NONE)
{
    // 判断VPN是否连接
    if (0 != etlGetProfileInt("/tmp/openvpn_status", "link_status", "link", 0))
    {
        idleScreen_SetStatus(PS_STATE_VPN, "");
    }
}

CUiLogicManager::~CUiLogicManager()
{
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                            &CUiLogicManager::OnStatusMessage);
    etl_UnregisterMsgHandle(XSI_MESSAGE_REQ_FAIL, XSI_MESSAGE_REQ_FAIL,
                            &CUiLogicManager::OnStatusMessage);
}

void CUiLogicManager::Init(CBaseIdleUI * pIdleUi)
{
    if (NULL != pIdleUi)
    {
        m_pIdleUi = pIdleUi;
    }
    RegeisterCallBack();
    // 主动获取一次账号信息
    idleScreen_SetStatus(PS_STATE_ACCOUNT, "");
    idleScreen_SetStatus(PS_STATE_IDLE, "");
    m_uIdleStatus = 0;

    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                          &CUiLogicManager::OnStatusMessage);
    etl_RegisterMsgHandle(XSI_MESSAGE_REQ_FAIL, XSI_MESSAGE_REQ_FAIL,
                          &CUiLogicManager::OnStatusMessage);
}

void CUiLogicManager::RegeisterCallBack()
{
    //注册回调
    idleScreen_SetNotifyFunction(PopUpCallBack, SNT_POPUP);
    idleScreen_SetNotifyFunction(SoftkeyCallBack, SNT_SOFTKEY);
    idleScreen_SetNotifyFunction(IconCallBack, SNT_ICON);
#if IF_SUPPORT_TEXT_NOTIFY
    idleScreen_SetNotifyFunction(TextCallBack, SNT_TEXT_NOTIFY);
#endif
#warning SNT_ACCOUNT
    idleScreen_SetNotifyFunction(AccountCallBack, SNT_ACCOUNT);
}

bool  CUiLogicManager::IsIdleFreeStatus()
{
    if ((m_uIdleStatus & IFS_POPUP)
            || (m_uIdleStatus & IFS_SOFTKEY)
            || (m_uIdleStatus & IFS_TEXT_NOTIFY))
    {
        return false;
    }
    return true;
}

bool CUiLogicManager::IdleLogicSoftkeyCallBack(LIST_IDLE_STATUS_ITEM& Statuslist)
{
    //获取当前需要显示的softkey数据
    VEC_SOFTKET_TYPE VecSoftkey;
    VecSoftkey.clear();
    IdleStatusItem phoneState = idleScreen_GetTopStatus(SNT_SOFTKEY);
    IDLEUI_INFO("UpdateSoftkey. eState(%d).", phoneState.m_nId);

    if (PS_STATE_NONE == phoneState.m_nId)
    {
        m_uIdleStatus = m_uIdleStatus & ~IFS_SOFTKEY;
        return false;
    }
    if (PS_STATE_IDLE != phoneState.m_nId)
    {
        m_uIdleStatus = m_uIdleStatus | IFS_SOFTKEY;
    }

    VEC_SOFTKET_TYPE vecSoftkey;
    CBaseIdleHandle * pBaseIdleHandle = NULL;
    pBaseIdleHandle = GET_IDLE_HANDLE(phoneState.m_nId);
    if (NULL != pBaseIdleHandle)
    {
        pBaseIdleHandle->SetType(phoneState.m_nId);
        if (pBaseIdleHandle->GetSoftkey(vecSoftkey))
        {
            if (NULL != m_pIdleUi)
            {
                m_pIdleUi->UpdateSoftkey(vecSoftkey);
            }
        }
        else
        {
            idleScreen_DropStatus(phoneState.m_nId);
        }
    }
    if (NULL != m_pIdleUi)
    {
        bool bStatus = IsIdleFreeStatus();
        m_pIdleUi->IdleFreeStatusChangeNotify(bStatus);
    }

    return false;
}

//弹出框回调
bool CUiLogicManager::IdleLogicPopupCallBack(LIST_IDLE_STATUS_ITEM& Statuslist)
{
    IdleStatusItem phonestate = idleScreen_GetTopStatus(SNT_POPUP);

    IDLE_INFO("IdleLogicPopupCallBack, [%d] [%s].", phonestate.m_nId, phonestate.m_strHint.c_str());

    if (PS_STATE_NONE == phonestate.m_nId)
    {
        m_uIdleStatus = m_uIdleStatus & ~IFS_POPUP;
        //清除当前idle存在的弹出框
        if (NULL != m_pIdleUi)
        {
            IDLE_INFO("Current status is  %d close All MessageBox", phonestate.m_nId);
            m_pIdleUi->RemoveIdleMessageBox(MSGBOX_ID_STATUS);
        }
        return false;
    }
    m_uIdleStatus = m_uIdleStatus | IFS_POPUP;
    PopupBoxData LogicData;
    // 默认是状态ID
    LogicData.m_nId = MSGBOX_ID_STATUS;
    CBaseIdleHandle * pBaseIdleHandle = NULL;

    pBaseIdleHandle = GET_IDLE_HANDLE(phonestate.m_nId);
    if (NULL != pBaseIdleHandle)
    {
        pBaseIdleHandle->SetType(phonestate.m_nId);
        pBaseIdleHandle->SetHint(phonestate.m_strHint);
        if (pBaseIdleHandle->GetPopupBoxInfo(LogicData))
        {
            IDLE_INFO("Get %d Status Logic Data OK  ", phonestate.m_nId);
            PopupBoxInfo popboxInfo;
            popboxInfo.m_iType = LogicData.m_iType;
            popboxInfo.m_nId = LogicData.m_nId;
            popboxInfo.m_nTimes = LogicData.m_nTimes;
            popboxInfo.m_strIcon = LogicData.m_strIcon;
            popboxInfo.m_strTitle = LogicData.m_strTitle;
            popboxInfo.m_strNote = LogicData.m_strNote;
            popboxInfo.m_vecSoftkey = LogicData.m_vecSoftkey;
            IDLE_INFO("Pop %d Status MessageBox ", phonestate.m_nId);
            CreateMessageBox(popboxInfo);
        }
        else
        {
            IDLE_INFO("Get %d Status Logic Data error ", phonestate.m_nId);
            idleScreen_ClearNotifyTypeFlag(phonestate.m_nId, SNT_TEXT_NOTIFY | SNT_POPUP | SNT_SOFTKEY);
        }
    }

    if (NULL != m_pIdleUi)
    {
        bool bStatus = IsIdleFreeStatus();
        m_pIdleUi->IdleFreeStatusChangeNotify(bStatus);
    }
    return true;
}

//Icon回调
bool CUiLogicManager::IdleLogicIconCallBack(LIST_IDLE_STATUS_ITEM& Statuslist)
{
    LIST_IDLE_STATUS_ITEM::ListIterator iter = Statuslist.begin();
    CBaseIdleHandle * pBaseIdleHandle = NULL;
    IdleIconInfo iconInfo;
    VEC_ICON_INFO IconList;
    for (; iter != Statuslist.end(); ++iter)
    {
        IdleStatusItem & StatusItem = (*iter);
        iconInfo.m_strHint = StatusItem.m_strHint;
        pBaseIdleHandle = GET_IDLE_HANDLE(StatusItem.m_nId);
        if (NULL != pBaseIdleHandle)
        {
            pBaseIdleHandle->GetIconInfo(iconInfo);
        }
        IconList.push_back(iconInfo);
    }
    if (NULL != m_pIdleUi)
    {
        m_pIdleUi->UpdateIcon(IconList);
        return true;
    }
    return false;
}

#if IF_SUPPORT_TEXT_NOTIFY
//Text回调
bool CUiLogicManager::IdleLogicTextCallBack(LIST_IDLE_STATUS_ITEM& Statuslist)
{
    if (NULL == m_pIdleUi)
    {
        return false;
    }
    ST_TextNotifyData stNotifyData;
    IdleStatusItem phonestate = idleScreen_GetTopStatus(SNT_TEXT_NOTIFY);
    if (PS_STATE_NONE == phonestate.m_nId)
    {
        m_uIdleStatus = m_uIdleStatus & ~IFS_TEXT_NOTIFY;
        m_pIdleUi->UpdatePromptText(stNotifyData);
        return false;
    }

    m_uIdleStatus = m_uIdleStatus | IFS_TEXT_NOTIFY;
    CBaseIdleHandle* pBaseIdleHandle = NULL;

    pBaseIdleHandle = GET_IDLE_HANDLE(phonestate.m_nId);
    if (NULL != pBaseIdleHandle)
    {
        pBaseIdleHandle->SetType(phonestate.m_nId);
        if (pBaseIdleHandle->GetTextNotifyData(stNotifyData))
        {
            m_pIdleUi->UpdatePromptText(stNotifyData);
        }
        else
        {
            idleScreen_DropStatus(phonestate.m_nId);
        }
    }

    if (NULL != m_pIdleUi)
    {
        bool bStatus = IsIdleFreeStatus();
        m_pIdleUi->IdleFreeStatusChangeNotify(bStatus);
    }
    return true;
}
#endif

//账号回调
bool CUiLogicManager::IdleLogicAccountCallBack(LIST_IDLE_STATUS_ITEM& Statuslist)
{
#warning SNT_ACCOUNT
    IdleStatusItem phonestate = idleScreen_GetTopStatus(SNT_ACCOUNT);
    if (PS_STATE_NONE == phonestate.m_nId)
    {
        m_uIdleStatus = m_uIdleStatus & ~IFS_ACCOUNT;
        return false;
    }

    m_uIdleStatus = m_uIdleStatus | IFS_ACCOUNT;
    CBaseIdleHandle* pBaseIdleHandle = NULL;

    pBaseIdleHandle = GET_IDLE_HANDLE(phonestate.m_nId);
    if (NULL != pBaseIdleHandle)
    {
        pBaseIdleHandle->SetType(phonestate.m_nId);
        ICON_TYPE iconType;
        yl::string strAccountName;
        if (pBaseIdleHandle->GetAccountInfo(strAccountName, iconType))
        {
            if (NULL != m_pIdleUi)
            {
                m_pIdleUi->UpdateAccountInfo(strAccountName, iconType);
            }
        }
        else
        {
            idleScreen_DropStatus(phonestate.m_nId);
        }
    }

    if (NULL != m_pIdleUi)
    {
        bool bStatus = IsIdleFreeStatus();
        m_pIdleUi->IdleFreeStatusChangeNotify(bStatus);
    }
    return true;
}

//_XmlBrowswer 回调
bool CUiLogicManager::IdleLogicUpdateCustomNotify()
{
    if (NULL == m_pIdleUi)
    {
        return false;
    }
    VEC_CUSTOM_NOTIFY CustomNotifyList;
    GetXmlBrowser(CustomNotifyList);

    bool bCustNotify = false;

    StatusItem StatusItemData;
    VEC_STATUS_ITEM m_vecItemData;
    m_vecItemData.clear();

    // 无状态，直接返回
    if (CustomNotifyList.size() > 0)
    {
        yl::string strShowAccount = "";
        int iAccount;
        if (acc_AccountNum(ACCOUNT_TYPE_ALL) <= 1)
        {
            iAccount = acc_GetFirstUserNameAccount();
        }

        iAccount = acc_GetUsableAccount(false, ACCOUNT_TYPE_ALL);
        if (acc_IsDirectIPAccount(iAccount))
        {
            iAccount = INVALID_ACCOUNTID;
        }
        if (acc_IsAccountIDValid(iAccount))
        {
            strShowAccount = acc_GetUsername(iAccount);
            strShowAccount.append("@");
            strShowAccount.append(acc_GetServerName(iAccount));
        }
        // 添加
        typeCusNotifyList::iterator iterCusInfo = CustomNotifyList.begin();
        for (; iterCusInfo != CustomNotifyList.end(); ++iterCusInfo)
        {
            CustomNotify & refNotify = (*iterCusInfo);
            StatusItemData.iAlign = refNotify.iAlign;
            StatusItemData.strIconPath = refNotify.strIcon;
            StatusItemData.eColor = (TEXT_FONT_COLOR)refNotify.iColor;
            StatusItemData.eSize = (X_FONT_TYPE)refNotify.iSize;
            StatusItemData.strMsg = refNotify.strText;
            m_vecItemData.push_back(StatusItemData);
            bCustNotify = true;
#if !IF_IDLE_SHOW_STATUS_ROWS
            bCustNotify = false;
            if (refNotify.strAccount.empty()
                    || refNotify.strAccount == strShowAccount)
            {
                bCustNotify = true;
                break;
            }
#endif
        }
    }

    if (!bCustNotify)
    {
        yl::string strQueueStatus;
        ACD_STATUS eAcdStatus = ACD_GetAcdStatus();
        if (GetACDQueueStatus(eAcdStatus, strQueueStatus))
        {
            StatusItem stCusNotify;
            stCusNotify.strMsg = strQueueStatus;
            stCusNotify.iAlign = DT_CENTER | DT_VCENTER;
            m_vecItemData.push_back(stCusNotify);
            bCustNotify = true;
        }
    }

    m_pIdleUi->UpdateCustomNotify(m_vecItemData, bCustNotify);

    return true;
}

void CUiLogicManager::GetXmlBrowser(VEC_CUSTOM_NOTIFY& CustomList)
{
    //具体的信息过滤放在DlgIdleScreen里
    CustomList = *(idleScreen_GetCustomNotify());
}

bool CUiLogicManager::OnSoftkeyAction(SoftKey_TYPE eSoftkey)
{
    ResetKeypadLock();
    IdleStatusItem phonestate = idleScreen_GetTopStatus(SNT_SOFTKEY);
    CBaseIdleHandle * phandle = GET_IDLE_HANDLE(phonestate.m_nId);

    if (keyGuard_IsLock() && (ST_MORE != eSoftkey || ST_EMPTY != eSoftkey) && KG_MENU_KEY != keyGuard_GetKeyGuardType())
    {
        EnterPageByType(IEPT_UNLOCK_CHECK);
        return true;
    }

    bool bHandle = false;
    if (NULL != phandle)
    {
        bHandle = phandle->HandleKeyProcess(eSoftkey, PHONE_KEY_NONE);
    }

    return bHandle;
}

bool CUiLogicManager::TransSoftkeyToKeyCode(SoftKey_TYPE eSoftKey, PHONE_KEY_CODE & eKeyCode)
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

//处理MessageBox的softkey按键处理
bool CUiLogicManager::OnMessageKeyAction(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode,
        int iMsgBoxId)
{
    bool bRet = false;
    switch (iMsgBoxId)
    {
    case MSGBOX_ID_STATUS:
        {
            IdleStatusItem phonestate = idleScreen_GetTopStatus(SNT_POPUP);
            CBaseIdleHandle * phandle = GET_IDLE_HANDLE(phonestate.m_nId);
            if (NULL != phandle)
            {
                bRet = phandle->HandleKeyProcess(eSoftkey, eKeyCode);
            }
        }
        break;
    case IMSGBOX_ID_OK:
        {
            if (ST_OK == eSoftkey
                    || PHONE_KEY_OK == eKeyCode)
            {
                //恢复出厂是否需要权限，弹出密码输入界面
                if (1 == configParser_GetConfigInt(kszIdleResetNeedPwd))
                {
                    if (NULL != m_pIdleUi)
                    {
                        m_pIdleUi->EnterPasswordCheckPage();
                    }
                }
                else//不需要权限
                {
#if IF_BUG_20555
                    if (acc_LogoutBeforeReset())
#endif  //IF_BUG_20555
                    {
                        IDLEUI_INFO("Before AdapterBox_ResetFactory.");
                        // 恢复出厂配置
                        AdapterBox_ResetFactory(RESET_OPTION_ALL);
                    }
                }
                bRet = true;
            }//end of ST_ok
        }
        break;
    case MSGBOX_ID_CANCEL:
        {
            if (ST_OK == eSoftkey
                    || PHONE_KEY_OK == eKeyCode)
            {
                // 重启话机
                AdapterBox_Reboot(0);
            }
            bRet = true;
        }
        break;
    case MSGBOX_ID_PASSWORD:
        {
            //输入密码正确。
            if (ST_OK == eSoftkey)
            {
#if IF_BUG_20555
                if (acc_LogoutBeforeReset())
#endif  //IF_BUG_20555
                {
                    // 恢复出厂配置
                    AdapterBox_ResetFactory(RESET_OPTION_ALL);
                }
            }
            else if (ST_CANCEL == eSoftkey)//密码输入错误，需要弹出框
            {
                PopupBoxInfo popboxInfo;
                popboxInfo.m_iType = IDLE_MSGBOX_TYPE_NOTE; //tips
                popboxInfo.m_nTimes = POPUP_BOX_SHOWING_SHORT_TIME;
                popboxInfo.m_strNote = _UILOGIC_LANG_TRAN(TRID_PASSWORD_IS_WRONG) ;
                popboxInfo.m_nId = MSGBOX_ID_PASSWORD_RESET_ERROR;//
                _UiIdleLogicManager.CreateMessageBox(popboxInfo);
            }
            else if (ST_EMPTY == eSoftkey)
            {
                ////UI层点击cancel，不处理
            }
            bRet = true;
        }
        break;
    case MSGBOX_ID_PASSWORD_RESET_ERROR://验证恢复出厂密码错误需要继续弹出密码验证框
        {
            m_pIdleUi->EnterPasswordCheckPage();
        }
        break;
    //
    case MSGBOX_ID_HOTDESK_WAITING:
        {
            _HotDesking.SetLogonWizard(FALSE);
            EnterPageByType(IEPT_HOT_DESK);
        }
        break;
#if IF_FEATURE_METASWITCH
    case MSGBOX_ID_MTSW_LOGOUT:
        {
            if (ST_OK == eSoftkey)
            {
                MTSW_Commprotal_Logout();
                AdapterBox_Reboot(0);
            }
        }
        break;
#endif
    case MSGBOX_ID_UPDATE_CONFIG:
        {
            Autop_NotifyAutoServer(false, ATP_MSG_REQ_START_BY_DEFAULT, 0, 1);
        }
        break;
    default:
        break;
    }
    return bRet;
}
bool CUiLogicManager::IsChannelKey(PHONE_KEY_CODE eKeyCode)
{
    if (PHONE_KEY_HANDSET_OFF_HOOK == eKeyCode || PHONE_KEY_HANDSET_ON_HOOK == eKeyCode  \
            || PHONE_KEY_HANDFREE == eKeyCode || PHONE_KEY_HEADSET_CTRL == eKeyCode \
            || PHONE_KEY_EHS_OPEN == eKeyCode || PHONE_KEY_EHS_CLOSE == eKeyCode \
            || PHONE_KEY_HEADSET_OUT == eKeyCode || PHONE_KEY_HEADSET_IN == eKeyCode \
            || PHONE_KEY_HANDSET == eKeyCode

            //http://10.3.5.199/Bug.php?BugID=73475
            //Mute键不属于声道键
            //      ||PHONE_KEY_MUTE ==eKeyCode
       )
    {
        return true;
    }
    return false;
}
bool CUiLogicManager::OnChannelKeyPress(PHONE_KEY_CODE eKeyCode)
{

    if (!IsChannelKey(eKeyCode))
    {
        return false;
    }
    bool bHandled = true;
    // 按任意键重置自动上锁定时器
    //StartKeyLockTimer();
    if (voice_IsCallCenterMode())
    {
        if (eKeyCode == PHONE_KEY_HANDSET_OFF_HOOK
                || eKeyCode == PHONE_KEY_HANDSET_ON_HOOK)
        {
            bHandled = false;
            // 双耳麦模式下Idle界面摘挂机无效
            return bHandled;
        }
    }
    voice_OnKeyPress(eKeyCode);
    voice_StopRingInMenu();
    switch (eKeyCode)
    {
    case PHONE_KEY_HANDFREE:
        {
            yl::string  strDialNum = "";
            talklogic_EnterDialUI(strDialNum);
        }
        break;
    case PHONE_KEY_HANDSET_OFF_HOOK:
#ifdef IF_HAVE_EXP
    case PHONE_KEY_EHS_OPEN:
#endif  // IF_HAVE_EXP
        {
            yl::string  strDialNum = "";
            talklogic_EnterDialUI(strDialNum);
        }
        break;
    case PHONE_KEY_HANDSET_ON_HOOK:
        {
            IdleStatusItem phonestate = idleScreen_GetTopStatus(SNT_SOFTKEY);
            if (PS_STATE_NONE != phonestate.m_nId)
            {
                CBaseIdleHandle * phandle = GET_IDLE_HANDLE(phonestate.m_nId);
                if (NULL != phandle)
                {
                    bHandled = phandle->HandleKeyProcess(ST_EMPTY, PHONE_KEY_CANCEL);
                }
            }
        }
        break;
    default:
        bHandled = false;
        break;
    }
    return bHandled;
}
//idle ShortCut响应(不可配置成programbelkey的按键响应)
bool CUiLogicManager::ProcessShortCut(PHONE_KEY_CODE eKeyCode)
{
    bool bHandle = true;
    IDLEUI_INFO("iKey(%d).", eKeyCode);
    switch (eKeyCode)
    {
#ifndef _CP920
    case PHONE_KEY_MENU:
        {
            EnterPageByType(IEPT_MAIN_MENU);
        }
        break;
#if !IF_BUG_4090
    case PHONE_KEY_HISTORY:
#endif
    case PHONE_KEY_DND:
        {
            // dnd 按键
            if (dnd_IsActive() && dnd_GetMode() == FWDDND_MODE_ACCOUNT)
            {
                AdapterBox_SettingUIEnterPage(MENU_SETTING_CUSTOM_DND);
            }
            else
            {
                dnd_SetEnable(FWDDND_ACCOUNT_GLOBAL, !dnd_IsEnable(FWDDND_ACCOUNT_GLOBAL));
                dsskey_UpdateDNDStatus(dnd_IsEnable(acc_GetDefaultAccount()));
            }
        }
        break;
#endif
#ifdef IF_SUPPORT_VIDEO
    case PHONE_KEY_VIDEO:
        {
            int camerastate = modhotplug_GetCameraStatus();
            if (CST_NOT_CONNECTED == camerastate || CST_CONNECTED == camerastate)
            {
                PopupBoxInfo popboxInfo;
                popboxInfo.m_iType = IDLE_MSGBOX_TYPE_WARNING;
                popboxInfo.m_nTimes = POPUP_BOX_SHOWING_SHORT_TIME; //不自动退出
                if (CST_NOT_CONNECTED == camerastate)
                {
                    popboxInfo.m_strNote = _UILOGIC_LANG_TRAN(TRID_CAMERA_NOT_DETECTED);
                }
                else
                {
                    popboxInfo.m_strNote = _UILOGIC_LANG_TRAN(TRID_CAMERA_NOT_READY);
                }
                CreateMessageBox(popboxInfo);
                m_pIdleUi->CreateMessageBox(popboxInfo);
            }
            else if (CST_READY == camerastate)//进入本地视频界面
            {
                EnterPageByType(IEPT_USB_VIEW_CAMERA);
            }
        }
        break;
#endif
#if defined(_CP920) && defined(IF_SUPPORT_BLUETOOTH)
    case PHONE_KEY_BLUETOOTH:
        {
            EnterPageByType(IEPT_BLUE_TOOTH_MENU);
        }
        break;
#endif
    default:
        bHandle = false;
        break;
    }
    return bHandle;
}
bool CUiLogicManager::OnKeyPress(PHONE_KEY_CODE ekeyCode)
{
    m_nPressedKey = ekeyCode;
    ResetKeypadLock();
    bool bRet = false;
    //softkeyAction已经响应过，要过滤这个几个按键
    if (ekeyCode >= PHONE_KEY_FN1 && ekeyCode <= PHONE_KEY_FN4)
    {
        return bRet;
    }

    if (PHONE_KEY_OK == ekeyCode || PHONE_KEY_CANCEL == ekeyCode ||
            PHONE_KEY_UP == ekeyCode || PHONE_KEY_DOWN == ekeyCode)
    {
        IdleStatusItem phonestate = idleScreen_GetTopStatus(SNT_SOFTKEY);
        if (PS_STATE_NONE != phonestate.m_nId)
        {
            CBaseIdleHandle * phandle = GET_IDLE_HANDLE(phonestate.m_nId);
            if (NULL != phandle)
            {
                bRet = phandle->HandleKeyProcess(ST_EMPTY, ekeyCode);
            }
        }
    }
    else if (OnChannelKeyPress(ekeyCode))
    {
        bRet = true;
    }
    else if (ProcessShortCut(ekeyCode))
    {
        bRet = true;
    }

    switch (ekeyCode)
    {
    case PHONE_KEY_0:
    case PHONE_KEY_1:
    case PHONE_KEY_2:
    case PHONE_KEY_3:
    case PHONE_KEY_4:
    case PHONE_KEY_5:
    case PHONE_KEY_6:
    case PHONE_KEY_7:
    case PHONE_KEY_8:
    case PHONE_KEY_9:
    case PHONE_KEY_STAR:
        {
            talklogic_EnterPredialUI(ekeyCode);
            bRet = true;
        }
        break;
    case PHONE_KEY_POUND:
        {
            if (keyGuard_IsLock() || !keyGuard_IsEnable())
            {
                talklogic_EnterPredialUI(ekeyCode);
                bRet = true;
            }
        }
        break;
#if IF_BUG_28571
    // Fortinet T66 长按Hold键,恢复出厂设置
    case PHONE_KEY_HOLD:
        {
            StopKeypadLockTimer();
            SetLongPressTimeByKey(ekeyCode, IDLE_LONG_PRESS_TIME_5S);
        }
        break;
#else
    case PHONE_KEY_OK:
        {
            if (!IsExistMessageBox())
            {
                if (0 == configParser_GetConfigInt(kszResetByLongPress)
                        || !feature_UserModeConfigItemShow("reset_to_factory") || keyGuard_IsLock())
                {

                    if (AdapterBox_IsInIdle() && OnProgramKeyProcess(ekeyCode))
                    {
                        bRet = true;
                    }
                }
                else
                {
                    StopKeypadLockTimer();
                    SetLongPressTimeByKey(ekeyCode, IDLE_LONG_PRESS_TIME_5S);
                }
            }
        }
        break;
#endif
#ifndef _CP920
#if IF_BUG_28571
    case PHONE_KEY_OK:
#else
    case PHONE_KEY_CANCEL:
#endif
#endif
    case PHONE_KEY_MESSAGE:
    case PHONE_KEY_REDIAL:
        {
            if (keyGuard_IsLock())
            {
                if (AdapterBox_IsInIdle())
                {
                    OnProgramKeyProcess(ekeyCode);
                    bRet = true;
                }
            }
            else
            {
                StopKeypadLockTimer();
                SetLongPressTimeByKey(ekeyCode, IDLE_LONG_PRESS_TIME_5S);
            }
        }
        break;
    default://默认调用programkey处理掉
        {
            bRet = OnProgramKeyProcess(ekeyCode);
        }
        break;
    }

    if (bRet)
    {
        m_nPressedKey = PHONE_KEY_NONE;
    }

    return bRet;
}
bool CUiLogicManager::OnKeyLongPress(PHONE_KEY_CODE ekeyCode)
{
    bool bRet = false;

    if (PHONE_KEY_NONE == m_nPressedKey)
    {
        return;
    }

    switch (ekeyCode)
    {
    case PHONE_KEY_POUND:
        {
            if (keyGuard_IsEnable())
            {
                keyGuard_Lock(true);
                bRet = true;
            }
        }
        break;
#ifdef _T49
    case PHONE_KEY_STAR:// 长按*重启
        {
            PopupBoxInfo popboxInfo;
            popboxInfo.m_nTimes = 0;//不自动退出
            popboxInfo.m_iType = IDLE_MSGBOX_TYPE_POPUPNORMAL; //messagebox弹窗
            popboxInfo.m_strTitle = _UILOGIC_LANG_TRAN(TRID_WARNING);
            popboxInfo.m_strNote = _UILOGIC_LANG_TRAN(TRID_IF_REBOOT_THE_PHONE);
            popboxInfo.m_nId = MSGBOX_ID_CANCEL;
            popboxInfo.m_vecSoftkey[0]  = ST_CANCEL;
            popboxInfo.m_vecSoftkey[1]  = ST_EMPTY;
            popboxInfo.m_vecSoftkey[2]  = ST_EMPTY;
            popboxInfo.m_vecSoftkey[3]  = ST_OK;
            CreateMessageBox(popboxInfo);
            bRet = true;
        }
        break;
#endif
#if IF_BUG_28571
    // Fortinet T66 长按Hold键,恢复出厂设置
    case PHONE_KEY_HOLD:
#else
    case PHONE_KEY_OK:
#endif
        {
            ResetKeyLongPressTime(ekeyCode);
            if (0 == configParser_GetConfigInt(kszResetByLongPress)
                    || !feature_UserModeConfigItemShow(RESET_FACTORY_COMMAND))
            {
                bRet = false;
            }
            else
            {
                PopOKCancelMessageBox(IMSGBOX_ID_OK, _UILOGIC_LANG_TRAN(TRID_WARNING),
                                      _UILOGIC_LANG_TRAN(TRID_IF_RESET_TO_FACTORY_SETTING));
            }
        }
        break;
#ifndef _CP920
#if IF_BUG_28571
    case PHONE_KEY_OK:
#else
    case PHONE_KEY_CANCEL:
#endif
        {
            ResetKeyLongPressTime(ekeyCode);
            // 长按*重启
            PopOKCancelMessageBox(MSGBOX_ID_CANCEL, _UILOGIC_LANG_TRAN(TRID_WARNING),
                                  _UILOGIC_LANG_TRAN(TRID_IF_REBOOT_THE_PHONE));
            bRet = true;
        }
        break;
#endif
    case PHONE_KEY_MESSAGE:
        {
            ResetKeyLongPressTime(ekeyCode);
            if (AdapterBox_IsInIdle())
            {
                commonUnits_System(PHONE_SCRIPTS_PATH RESET_SCRIPT_COMMAND);
                bRet = true;
            }
        }
        break;
    case PHONE_KEY_REDIAL:
        {
            ResetKeyLongPressTime(ekeyCode);
            PopOKCancelMessageBox(MSGBOX_ID_UPDATE_CONFIG, _UILOGIC_LANG_TRAN(TRID_WARNING),
                                  _UILOGIC_LANG_TRAN(TRID_IF_PROVISION_NOW));
            bRet = true;
        }
        break;
    default:
        break;
    }

    if (bRet)
    {
        m_nPressedKey = PHONE_KEY_NONE;
    }

    return bRet;
}

bool CUiLogicManager::OnKeyRelease(PHONE_KEY_CODE ekeycode)
{
    ResetKeypadLock();
    if (ekeycode != m_nPressedKey)
    {
        m_nPressedKey = PHONE_KEY_NONE;
        return false;
    }

    m_nPressedKey = PHONE_KEY_NONE;

    switch (ekeycode)
    {
    case PHONE_KEY_OK:
        {
            ResetKeyLongPressTime(ekeycode);
            return OnShortKeyRelease(ekeycode);
        }
        break;
#ifndef _CP920
    case PHONE_KEY_CANCEL:
        {
            ResetKeyLongPressTime(ekeycode);
            return OnShortKeyRelease(ekeycode);
        }
        break;
#endif
    case PHONE_KEY_MESSAGE:
        {
            ResetKeyLongPressTime(ekeycode);
            if (AdapterBox_IsInIdle())   //这次按键为长按就不响应短按了
            {
                if (!OnProgramKeyProcess(ekeycode))
                {
#if IF_FEATURE_BROADSOFT_VOICEMAIL
                    if (1 == configParser_GetConfigInt(kszVoiceMailMessageKeyMode))
                    {
                        SettingUILogic_ViewVoiceMail();
                    }
                    else
#endif
#if IF_FEATURE_METASWITCH_VOICEMAIL
                        if (IdleUL_EnterMTSWVoiceMail())
                        {
                            return true;
                        }
                        else
#endif
                        {
                            int iAccountId = acc_GetLastVMailAccountId();
                            if (iAccountId < 0)
                            {
                                iAccountId = acc_GetDefaultAccount();
                            }
                            //呼出配置的默认语音帐号号码（如果帐号未配置，则进入默认账号的语音信箱设置
                            SettingUILogic_ConnectVoiceMail(iAccountId);
                        }
                }

                return true;
            }
        }
        break;
    case PHONE_KEY_POUND:
        {
            ResetKeyLongPressTime(ekeycode);
            if (keyGuard_IsLock()
                    || !keyGuard_IsEnable())//此条件下，按下的时候就已经进入了拨号界面了，不需要在进去
            {
                return false;
            }
            else
            {
                talklogic_EnterPredialUI(ekeycode);
                return true;
            }
        }
        break;
    case PHONE_KEY_REDIAL://redial键不是按下时响应
        {
            ResetKeyLongPressTime(ekeycode);
            if (AdapterBox_IsInIdle())
            {
                if (!OnShortKeyRelease(ekeycode))
                {
#if IF_BUG_27029 && !IF_BUG_27501
                    talklogic_Redial();
#else
                    if (BsftLog_IsEnable()
                            && 0 != configParser_GetConfigInt(kszBWCallLogandDir))
                    {
                        EnterPageByType(IEPT_BROADSOFT_CALLLOG_PLACEED);
                    }
#if IF_FEATURE_METASWITCH_CALLLOG
                    else if (MTSWLog_IsEnable() && !LocalLog_IsEnable())
                    {
                        EnterPageByType(IEPT_MTSW_CALLLOG_PLACEED);
                    }
#endif
                    else
                    {
                        EnterPageByType(IEPT_LOCAL_CALLLOG_PLACEED);
                    }
#endif
                    break;
                }
            }
        }
        break;
    default:
        break;
    }
    m_nPressedKey = PHONE_KEY_NONE;
    return false;
}

bool CUiLogicManager::UpdateAccountInfo(const yl::string & strName, ICON_TYPE eIcon)
{
    if (NULL != m_pIdleUi)
    {
        m_pIdleUi->UpdateAccountInfo(strName, eIcon);
    }
    return true;
}

bool CUiLogicManager::CreateMessageBox(const PopupBoxInfo & popData, bool bOnlyInIdle)
{
    if (NULL == m_pIdleUi || (bOnlyInIdle && !AdapterBox_IsInIdle()))
    {
        return false;
    }
    IDLE_INFO("IDLEUL: CreateMessageBox note %s.", popData.m_strNote.c_str());
    return m_pIdleUi->CreateMessageBox(popData);
}

bool CUiLogicManager::RemoveMessagebox(int nID)
{
    if (NULL == m_pIdleUi)
    {
        return false;
    }
    IDLE_INFO("IDLEUL: RemoveMessagebox.");
    return m_pIdleUi->RemoveIdleMessageBox(nID);
}

void CUiLogicManager::ResetKeypadLock()
{
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    // Idle录制不上锁
    keyGuard_StopKeypadLockTimer();

    if (modRecord_IsIdleRecording(RECORD_AUDIO))
    {
        return;
    }
#endif
    keyGuard_ResetKeypadLock();
}

void CUiLogicManager::UpdateStatus()
{
    IDLE_INFO("CUiLogicManager::UpdateStatus.");
    LIST_IDLE_STATUS_ITEM statusList;
    statusList = idleScreen_GetStatusList(SNT_ICON);
    IdleLogicPopupCallBack(statusList);
    IdleLogicSoftkeyCallBack(statusList);
    IdleLogicIconCallBack(statusList);
#if IF_SUPPORT_TEXT_NOTIFY
    IdleLogicTextCallBack(statusList);
#endif
    // 主动更新账号信息
    idleScreen_SetStatus(PS_STATE_ACCOUNT, "");
}

void CUiLogicManager::StopKeypadLockTimer()
{
    keyGuard_StopKeypadLockTimer();
}

LRESULT CUiLogicManager::OnStatusMessage(msgObject & msg)
{
    BOOL bRet = FALSE;
    switch (msg.message)
    {
    case CONFIG_MSG_BCAST_CHANGED:
        {
            if (NULL != _UiIdleLogicManager.m_pIdleUi)
            {
                bRet = _UiIdleLogicManager.m_pIdleUi->OnConfigChangeMsg(msg);
            }
        }
        break;
    case XSI_MESSAGE_REQ_FAIL:
        {
            if (XSI_PARK_RETRIVE == msg.wParam)
            {
                PopupBoxInfo stPopInfo;
                stPopInfo.m_nTimes = POPUP_BOX_SHOWING_SHORT_TIME;
                stPopInfo.m_iType = IDLE_MSGBOX_TYPE_NOTE; //提示
                stPopInfo.m_strNote = _UILOGIC_LANG_TRAN(TRID_RETRIEVE_FAILED);
                stPopInfo.m_nId = MSGBOX_ID_NOTE;
                _UiIdleLogicManager.CreateMessageBox(stPopInfo, false);
            }
        }
        break;
    default:
        break;
    }
    return bRet;
}

bool CUiLogicManager::OnProgramKeyProcess(PHONE_KEY_CODE eKeyCode)
{
    int iDsskeyID = GetDsskeyIDByKeyCode(eKeyCode);
    DSSKEY_MODULE_TYPE eModuleType = dsskey_GetModuleType(iDsskeyID);
    DssKey_Type eDsskeyType = dsskey_GetDsskeyType(iDsskeyID);
    if (DMT_PROGRAMEKEY_MODULE == eModuleType)
    {
        // 只有可编程按键才在这边处理
        return dsskey_OnDsskeyClick(iDsskeyID);
    }
    return false;
}

bool CUiLogicManager::OnShortKeyRelease(PHONE_KEY_CODE eKeyCode)
{
    if (OnProgramKeyProcess(eKeyCode))
    {
        return true;
    }
    return false;
}

bool CUiLogicManager::PopOKCancelMessageBox(int nMesgBoxID, const yl::string& strTitle, const yl::string& strNote, int nTime /*= 0*/)
{
    PopupBoxInfo popboxInfo;
    popboxInfo.m_nTimes = nTime;//不自动退出
    popboxInfo.m_iType = IDLE_MSGBOX_TYPE_POPUPNORMAL; //messagebox弹窗
    popboxInfo.m_strTitle = strTitle;
    popboxInfo.m_strNote = strNote;
    popboxInfo.m_nId = nMesgBoxID;
    popboxInfo.m_vecSoftkey[0] = ST_CANCEL;
    popboxInfo.m_vecSoftkey[1] = ST_EMPTY;
    popboxInfo.m_vecSoftkey[2] = ST_EMPTY;
    popboxInfo.m_vecSoftkey[3] = ST_OK;
    CreateMessageBox(popboxInfo);
    return true;
}

void CUiLogicManager::SetLongPressTimeByKey(int nKey, int nTime)
{
    if (NULL != m_pIdleUi)
    {
        m_pIdleUi->SetLongPressTimeByKey(nKey, nTime);
    }
}

void CUiLogicManager::ResetKeyLongPressTime(int nKey)
{
    if (NULL != m_pIdleUi)
    {
        m_pIdleUi->ResetLongPressTimeByKey(nKey);
    }
}

bool CUiLogicManager::IsExistMessageBox()
{
    bool bRet = false;
    if (NULL != m_pIdleUi)
    {
        bRet = m_pIdleUi->IsExistMessageBox();
    }
    return bRet;
}

void CUiLogicManager::EnterPageByType(IDLE_ENTER_PAGE_TYPE_E eEnterPageType)
{
    if (NULL != m_pIdleUi)
    {
        m_pIdleUi->EnterPageByType(eEnterPageType);
    }
}

int CUiLogicManager::GetDsskeyIDByKeyCode(int nKey)
{
    if (NULL != m_pIdleUi)
    {
        return m_pIdleUi->GetDsskeyIDByKeyCode(nKey);
    }
    return -1;
}

bool CUiLogicManager::GetACDQueueStatus(ACD_STATUS eAcdStatus, yl::string& strQueue)
{
    if (!CallCenter_IsEnableQueueStatus())
    {
        // 未开启Queue Status，隐藏控件
        return false;
    }

    bool bRet = true;
    //更新queute status
    const CallCenterQueueStatus& stQueueStatus = ACD_GetQueueStatus();
    strQueue = stQueueStatus.m_strName.c_str();
    strQueue += ": ";
    switch (stQueueStatus.m_eStatus)
    {
    //empty
    case QUEUE_EMPTY:
        {
            strQueue += _UILOGIC_LANG_TRAN(TRID_EMPTY);
        }
        break;
    //normal
    case QUEUE_NORMAL:
        {
            strQueue += _UILOGIC_LANG_TRAN(TRID_QUEUEING);
        }
        break;
    //Threshold Exceeded
    case QUEUE_THRESHOLD_EXCEEDED:
        {
            strQueue += _UILOGIC_LANG_TRAN(TRID_ALERT);
        }
        break;
    default:
        {
            strQueue.clear();
            bRet = false;
        }
        break;
    }
    return bRet;
}

#if IF_SUPPORT_CENTER_ACD_STATUS
void CUiLogicManager::UpdateCenterNote(const yl::string& strNote)
{
    if (NULL == m_pIdleUi)
    {
        return;
    }
    m_pIdleUi->UpdateCenterNote(strNote);
}
#endif

#if IF_SUPPORT_EXTRA_IDLE_NOTE
void CUiLogicManager::UpdateExtraNote(const yl::string& strNote)
{
    if (NULL == m_pIdleUi)
    {
        return;
    }
    m_pIdleUi->UpdateExtraNote(strNote);
}
#endif

