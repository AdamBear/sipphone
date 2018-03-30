#include "edkuimanager.h"
#include "edkuidatamanager.h"
#include "uimanager/moduimanager.h"
#include "settingui/include/modsettingui.h"
#include "directoryui/include/moddirectoryui.h"
#include "edk/include/modedk.h"
#include "feature/include/modfeature.h"
#include "idlescreenui/modidleui.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "dsskeyui/moddsskeyui.h"
#include "dsskey/include/moddsskey.h"
#include "keyguard/include/modkeyguard.h"
#include "commonapi/stringhelperapi.h"
#include "messagebox/modmessagebox.h"
#include "modedkui.h"
#include "settingui/include/pageiddefine.h"


#if IF_FEATURE_EDK
IMPLEMENT_GETINSTANCE(CEdkUIManager)

CEdkUIManager::CEdkUIManager()
{
    // 注册消息处理
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                          &CEdkUIManager::OnMessage);
}

CEdkUIManager::~CEdkUIManager()
{
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                            &CEdkUIManager::OnMessage);
}

// 初始化
bool CEdkUIManager::Init()
{
    InitGetEdkSoftkeyFun();
    if (Edk_IsEnable())
    {
        return _EDKUIDataMgr.LoadMapConfig();
    }

    return false;
}

// 通过softkey type获取提示名称
bool CEdkUIManager::GetTipName(SoftKey_TYPE eType, yl::string & strTipName)
{
    // 获取系统softkey的提示名称,否则获取edk softkey的提示名称
    if (!_EDKUIDataMgr.GetSoftkeyIDByType(eType, strTipName))
    {
        strTipName = Edk_GetEdkSoftKeyID((EDKSOFTKEYTYPE)eType);

        if (strTipName.empty())
        {
            return false;
        }
    }

    return true;
}

// 响应edk调用的softkey(系统softkey、edksoftkey、idle4个program key)功能
bool CEdkUIManager::OnExcuteSoftkey(const yl::string & strSoftkeyID)
{
    // 键盘menu key上锁,则不能执行custom softkey
    if (keyGuard_IsLock()
            && (KG_FUN_KEY == keyGuard_GetKeyGuardType()
                || KG_ALL_KEYS == keyGuard_GetKeyGuardType()))
    {
        return false;
    }

    // 退出所有的提示框
    MessageBox_RemoveAllMessageBox();

    // 获取系统softkey type,否则获取edk softkey
    SoftKey_TYPE eSoftkeyType = _EDKUIDataMgr.GetSoftkeyTypeByID(strSoftkeyID);
    if (ST_EMPTY == eSoftkeyType)
    {
        eSoftkeyType = (SoftKey_TYPE)Edk_GetEdkSoftKeyType(strSoftkeyID);
    }

    // 获取idle program key的type
    DssKey_Type eDsskeyType = _EDKUIDataMgr.GetProgramkeyTypeByID(strSoftkeyID);

    // 获取当前界面下所有softkey
    CArraySoftKeyBarData    arraySoftkeyData;
    CBaseDialog * pBaseDlg = UIManager_GetTopWindow();
    if (NULL == pBaseDlg)
    {
        return false;
    }
    pBaseDlg->GetSoftkey(arraySoftkeyData);

    // EDK的softkey是在Set时安装的，故GetSoftkey无法获取到EDK自定义的
    EdkUI_GetEDKSoftkeyData(arraySoftkeyData);

    if (0 == arraySoftkeyData.size())
    {
        return false;
    }

    EDK_INFO("CEdkUIManager::OnExcuteSoftkey ID[%s] softkey[%d] programkey[%d]", strSoftkeyID.c_str(), eSoftkeyType, eDsskeyType);

    for (UINT uIndex = 0; uIndex < arraySoftkeyData.size(); ++uIndex)
    {
        // idle下需要判断4个program key
        bool bHandle = false;
        const CSoftKeyBarData & stSoftkeyData = arraySoftkeyData[uIndex];
        if (UIManager_IsInIdle())
        {
            if (DT_NONE != eDsskeyType
                    && GetDsskeyTypeBySoftkey((SoftKey_TYPE)stSoftkeyData.m_iType) == eDsskeyType)
            {
                bHandle = true;
            }
        }

        // 判断当前界面下是否存在该类型的softkey
        if (!bHandle && eSoftkeyType != stSoftkeyData.m_iType)
        {
            continue;
        }

        // 调用真正响应softkey操作的接口
        pBaseDlg->OnEdkSoftkeyAction(stSoftkeyData.m_iType);
        return true;
    }

    return false;
}

// 响应edk调用的进入菜单功能
bool CEdkUIManager::EnterMenu(const yl::string & strMenuID)
{
    EDK_INFO("CEdkUIManager::EnterMenu ID[%s]", strMenuID.c_str());
    bool bResult = FALSE;

    yl::string strPageName;

    // 通话存在,不允许界面跳转
    if (talklogic_SessionExist())
    {
        return FALSE;
    }

    if (!IsAllowEnterUI(strMenuID))
    {
        EDK_INFO("Not allow enter ui");
        return FALSE;
    }

    // 判断是否为setting界面
    _EDKUIDataMgr.GetPageNameByMenuID(strMenuID, strPageName);
    EDK_INFO("Page Name[%s]", strPageName.c_str());

    if (EnterSettingPage(strPageName))
    {
        return true;
    }

    // 判断是否为directory界面
    int iDirType = _EDKUIDataMgr.GetDirTypeByMenuID(strMenuID);
    EDK_INFO("Dir Type[%d]", iDirType);

    if (DIRECTORY_TYPE_REMOTE == iDirType)
    {
        return DM_EnterDirectoryUI(iDirType, -1);
    }
    else if (DIRECTORY_TYPE_NONE != iDirType)
    {
        return DM_EnterDirectoryUI(iDirType);
    }

    return FALSE;
}

// 获取softkey对应的dsskey
DssKey_Type CEdkUIManager::GetDsskeyTypeBySoftkey(SoftKey_TYPE eType)
{
    bool bResult;
    int iKeyCode = 0xff;

    switch (eType)
    {
    case ST_IDLE_HOTKEY1:
        iKeyCode = PHONE_KEY_FN1;
        break;
    case ST_IDLE_HOTKEY2:
        iKeyCode = PHONE_KEY_FN2;
        break;
    case ST_IDLE_HOTKEY3:
        iKeyCode = PHONE_KEY_FN3;
        break;
    case ST_IDLE_HOTKEY4:
        iKeyCode = PHONE_KEY_FN4;
        break;
    default:
        return DT_NONE;
    }

    int iDsskeyID = dsskeyUI_GetDsskeyIDByKeycode(iKeyCode);

    DssKeyLogicData dsskeyData;
    if (dsskey_GetData(iDsskeyID, dsskeyData))
    {
        return dsskeyData.eDKType;
    }

    return DT_NONE;
}

LRESULT CEdkUIManager::OnMessage(msgObject & objMessage)
{
    BOOL bHandled = TRUE;
    EDK_INFO("CEdkUIManager::OnMesssage [%d, %d]", objMessage.message, objMessage.wParam);
    switch (objMessage.message)
    {
    case  CONFIG_MSG_BCAST_CHANGED:
        {
            // lParam == 1,提示模式配置改变,此处不需要处理
            if (ST_EDK_SWITCH == objMessage.wParam)
            {
                if (1 == objMessage.lParam)
                {
                    break;
                }
                else
                {
                    _EDKUIManager.InitGetEdkSoftkeyFun();
                }
            }

            if (ST_EDK_SWITCH == objMessage.wParam || ST_EDK_CONFIG == objMessage.wParam)
            {
                _EDKUIDataMgr.LoadMapConfig();
            }
        }
        break;
    default:
        {
            bHandled = FALSE;
        }
        break;
    }

    return bHandled ? 1 : 0;
}

// 判断是否允许进入menu的条件
bool CEdkUIManager::IsAllowEnterUI(const yl::string & strMenuID)
{
    FuncIsAllowEnterMenu pAllowEnterUI = NULL;

    if (!_EDKUIDataMgr.GetAllowEnterUIFunc(strMenuID, pAllowEnterUI)
            || NULL == pAllowEnterUI)
    {
        return true;
    }

    yl::string strBuffer = "";
    if (!_EDKUIDataMgr.GetPageNameByMenuID(strMenuID, strBuffer))
    {
        int iDirType = _EDKUIDataMgr.GetDirTypeByMenuID(strMenuID);
        if (DIRECTORY_TYPE_NONE == iDirType)
        {
            return true;
        }
        strBuffer = commonAPI_FormatString("%d", iDirType);
    }

    EDK_INFO("CEdkUIDataManager::IsAllowEnterUI. Page buffer[%s]", strBuffer.c_str());
    return pAllowEnterUI(strBuffer);
}

void CEdkUIManager::InitGetEdkSoftkeyFun()
{
    if (Edk_IsEnable())
    {
        UIManager_SetFunGetCustomSoftkey(EdkUI_GetEDKSoftkeyData);
    }
    else
    {
        UIManager_SetFunGetCustomSoftkey(NULL);
    }
}

bool CEdkUIManager::EnterSettingPage(const yl::string & strPageName)
{
    if (strPageName.empty())
    {
        return false;
    }
    int nID = ID_INVALID;
    yl::string strNextPageName = "";
    yl::string strRealPageName = "";

    if (kszEdkPageIDCustomAccountForward == strPageName)
    {
        strRealPageName = kszPageIDCallForward;
        nID = acc_GetDefaultAccount();
    }
#ifdef IF_FEATURE_BROADSOFT_CLID
    else if (kszEdkPageIDCallcontrolAnonymousCallselectline == strPageName)
    {
        strRealPageName = kszPageIDLineSelect;
        strNextPageName = kszPageIDCallControllHideNumber;
    }
    else if (kszEdkPageIDCallcontrolAnonymousRejectionselectline == strPageName)
    {
        strRealPageName = kszPageIDLineSelect;
        strNextPageName = kszPageIDCallControllAnonymoRrejection;
    }
#endif
#ifdef IF_FEATURE_BROADSOFT_REMOTE_OFFICE
    else if (kszEdkPageIDCallcontrolRemoteOfficeSelectline == strPageName)
    {
        strRealPageName = kszPageIDLineSelect;
        strNextPageName = kszPageIDCallControllRemoteOffice;
    }
#endif
#ifdef IF_FEATURE_BROADSOFT_ANYWHERE
    else if (kszEdkPageIDCallcontrolAnywhereSelectline == strPageName)
    {
        strRealPageName = kszPageIDLineSelect;
        strNextPageName = kszPageIDAnyWhereLocationList;
    }
#endif
#ifdef IF_FEATURE_BROADSOFT_CALL_FORWARD_NOT_REACHABLE
    else if (kszEdkPageIDCallcontrolForwardNotReachableSelectline == strPageName)
    {
        strRealPageName = kszPageIDLineSelect;
        strNextPageName = kszPageIDCallControllCFNR;
    }
#endif
#ifdef IF_FEATURE_BROADSOFT_PRIORITY_ALERT
    else if (kszEdkPageIDCallcontrolSimultaneousRingSelectline == strPageName)
    {
        strRealPageName = kszPageIDLineSelect;
        strNextPageName = kszPageIDCallControllSimultaneousRingList;
    }
#endif
#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    else if (kszEdkPageIDCallcontrolAssistantSelectline == strPageName)
    {
        strRealPageName = kszPageIDLineSelect;
        strNextPageName = kszPageIDCallControllAssistantDetails;
    }
#endif
#ifdef IF_FEATURE_BROADSOFT_CALL_WAITING
    else if (kszEdkPageIDCallcontrolCallWaitingselectline == strPageName)
    {
        strRealPageName = kszPageIDLineSelect;
        strNextPageName = kszPageIDCallControllCallWaiting;
    }
#endif
#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    else if (kszEdkPageIDCallcontrolCallRecordSelectline == strPageName)
    {
        strRealPageName = kszPageIDLineSelect;
        strNextPageName = kszPageIDCallControllCallRecord;
    }
#endif
#ifdef IF_FEATURE_BROADSOFT_MOBILITY
    else if (kszEdkPageIDCallcontrolMobilityselectline == strPageName)
    {
        strRealPageName = kszPageIDLineSelect;
        strNextPageName = kszPageIDCallControllMobility;
    }
#endif
#ifdef IF_FEATURE_BROADSOFT_GROUP_NIGHT_FORWARD
    else if (kszEdkPageIDCallcontrolGroupNightForwardSelectline == strPageName)
    {
        strRealPageName = kszPageIDLineSelect;
        strNextPageName = kszPageIDCallControllGNF;
    }
#endif
#ifdef IF_FEATURE_BROADSOFT_CALL_FORWARD_SELECTIVE
    else if (kszEdkPageIDCallcontrolforwardselectiveselectline == strPageName)
    {
        strRealPageName = kszPageIDLineSelect;
        strNextPageName = kszPageIDCallControllForwardSelective;
    }
#endif
    else if (kszEdkPageIDCustomAlwaysForward == strPageName)
    {
        strRealPageName = kszPageIDAlwaysForward;
        nID = acc_GetDefaultAccount();
    }
    else if (kszEdkPageIDCustomBusyForward == strPageName)
    {
        strRealPageName = kszPageIDBusyForward;
        nID = acc_GetDefaultAccount();
    }
    else if (kszEdkPageIDCustomNoAnswerForward == strPageName)
    {
        strRealPageName = kszPageIDNoAnswerForward;
        nID = acc_GetDefaultAccount();
    }
    else if (kszPageIDAnonymousCall == strPageName)
    {
        strRealPageName = strPageName;
        nID = acc_GetUsableAccount(true);
    }
    else if (kszEdkPageIDCustomAccountDND == strPageName)
    {
        strRealPageName = kszPageIDAccountDND;
        nID = acc_GetUsableAccount(true);
    }
    else
    {
        strRealPageName = strPageName;
    }

    // 进入page,做三级权限判断
    if (strRealPageName.empty()
            || !feature_UserModeConfigItemShow(strRealPageName)/*三级权限判断*/)
    {
        return false;
    }

    if (ID_INVALID != nID)
    {
        SettingUI_EnterPage(strRealPageName, (void *)(&nID));
    }
    else if (!strNextPageName.empty())
    {
        SettingUI_EnterPage(strRealPageName, (void *)(&strNextPageName));
    }
    else
    {
        SettingUI_EnterPage(strRealPageName);
    }

    return true;
}

yl::string CEdkUIManager::GetMapNameByPageData(const yl::string & strPageName,
        const yl::string & strNextPageName, const int & nID)
{
    yl::string strMapName = "";
    if (ID_INVALID != nID)
    {
        if (kszPageIDCallForward == strPageName)
        {
            strMapName = kszEdkPageIDCustomAccountForward;
        }
        else if (kszPageIDAlwaysForward == strPageName)
        {
            strMapName = kszEdkPageIDCustomAlwaysForward;
        }
        else if (kszPageIDBusyForward == strPageName)
        {
            strMapName = kszEdkPageIDCustomBusyForward;
        }
        else if (kszPageIDNoAnswerForward == strPageName)
        {
            strMapName = kszEdkPageIDCustomNoAnswerForward;
        }
        else
        {
            strMapName = strPageName;
        }
    }
    else if (!strNextPageName.empty() && kszPageIDLineSelect == strPageName)
    {
        if (kszPageIDCallControllHideNumber == strNextPageName)
        {
            strMapName = kszEdkPageIDCallcontrolAnonymousCallselectline;
        }
        else if (kszPageIDCallControllAnonymoRrejection == strNextPageName)
        {
            strMapName = kszEdkPageIDCallcontrolAnonymousRejectionselectline;
        }
        else if (kszPageIDCallControllRemoteOffice == strNextPageName)
        {
            strMapName = kszEdkPageIDCallcontrolRemoteOfficeSelectline;
        }
        else if (kszPageIDAnyWhereLocationList == strNextPageName)
        {
            strMapName = kszEdkPageIDCallcontrolAnywhereSelectline;
        }
        else if (kszPageIDCallControllCFNR == strNextPageName)
        {
            strMapName = kszEdkPageIDCallcontrolForwardNotReachableSelectline;
        }
        else if (kszPageIDCallControllSimultaneousRingList == strNextPageName)
        {
            strMapName = kszEdkPageIDCallcontrolSimultaneousRingSelectline;
        }
        else if (kszPageIDCallControllAssistantDetails == strNextPageName)
        {
            strMapName = kszEdkPageIDCallcontrolAssistantSelectline;
        }
        else if (kszPageIDCallControllCallWaiting == strNextPageName)
        {
            strMapName = kszEdkPageIDCallcontrolCallWaitingselectline;
        }
        else if (kszPageIDCallControllMobility == strNextPageName)
        {
            strMapName = kszEdkPageIDCallcontrolMobilityselectline;
        }
        else if (kszPageIDCallControllGNF == strNextPageName)
        {
            strMapName = kszEdkPageIDCallcontrolGroupNightForwardSelectline;
        }
        else if (kszPageIDCallControllForwardSelective == strNextPageName)
        {
            strMapName = kszEdkPageIDCallcontrolforwardselectiveselectline;
        }
        else
        {
            strMapName = strPageName;
        }
    }
    else
    {
        strMapName = strPageName;
    }

    return strMapName;
}

#endif
