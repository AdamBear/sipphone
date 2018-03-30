#include "menucontroller.h"

// #include "CommonAPI/CommonAPI.h"
#include "setting/include/menucallback.h"
#include "directoryui/include/moddirectoryui.h"
#include "interfacedefine.h"
#include "contacts/ldaplogic/include/modldap.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "path_define.h"
#include "menugenerator.h"
#include "menutitlemanager.h"
#include "talk/talklogic/include/modtalklogic.h"
// #include "presence/include/presence_inc.h"

// UI mod
// #include "directoryui/moddirectoryui.h"
#include "baseui/modbaseui.h"
// #include "directoryui/directorycommon.h"
// #include "directoryui/modcalllogui.h"
#include "dsskey/include/moddsskey.h"
// #include "MessageUI/modMessageUI.h"
#include "setting/include/modzero.h"
#include "settingui/include/modsettingui.h"
#include "talkui/include/modtalkui.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "setting/include/common.h"
// #include "directoryui/moddirectoryui.h"
#include "talk/fwd_dnd/include/modfwddnd.h"
#include "setting_page_define.h"
#include "feature/include/modfeature.h"

#if defined(_VPX)
#include "CalibrationUI/modCalibration.h"
#endif

#ifdef IF_FEATURE_MULTIFWD
#include "uicommon/uimanager/uimanager_common.h"
#endif

namespace NS_MENU
{
// Open dynamic menu string. The dynamic menu means content will vary
// at times, so should be generated dynamicly.
static const char kszOpenDynamicAction[] = "OpenDynamic";

const char * const kszEnterFavoritesAction = "EnterFavorites";

//打开anonymous界面命令
static const char kszOpenAnonymous[] = "OpenPage(anonymous_call_list.xml)";

// Class CMenuController.
CMenuController * CMenuController::m_pMenuController = NULL;

CMenuController * CMenuController::GetInstance()
{
    // Instantiate one if not exists.
    if (m_pMenuController == NULL)
    {
        m_pMenuController = new CMenuController();
        if (m_pMenuController != NULL)
        {
            // Do initializations.
            m_pMenuController->MyInit();
        }
    }
    return m_pMenuController;
}

void CMenuController::ReleaseInstance()
{
    // Release the instance if exists.
    if (m_pMenuController != NULL)
    {
        // Do finalizations.
        m_pMenuController->MyFinal();

        delete m_pMenuController;
        m_pMenuController = NULL;
    }
}

//根据菜单的aciton获取对应的三级权限文件中的key
yl::string CMenuController::GetWebItemKeyByClickAction(const yl::string strClickAction) const
{
    yl::string strAction = strClickAction;
    //先取出括号中的内容
    yl::string strTmp1 = "(";
    int ipos1 = strAction.find(strTmp1);
    if (ipos1 != yl::string::npos)
    {
        strAction = strAction.substr(ipos1 + strTmp1.size()
                                     , strAction.size() - ipos1 - strTmp1.size() - 1);
    }

    //去掉.xml
    yl::string strTmp2 = ".xml";
    int ipos2 = strAction.find(strTmp2);
    if (ipos2 != yl::string::npos)
    {
        strAction = strAction.substr(0, strAction.size() - strTmp2.size());
    }
    return strAction;
}

bool CMenuController::DoAction(const yl::string & strClickAction)
{
    // Parse the click-action info.
    yl::string strAction;
    yl::string strContent;
    if (!commonAPI_ParseActionInfo(&strAction, &strContent, strClickAction))
    {
        return false;
    }

    yl::string strClickActionCpy(strClickAction);

    // Parse again.
    if (!commonAPI_ParseActionInfo(&strAction, &strContent, strClickActionCpy))
    {
        return false;
    }

    bool isSucceed = false;

    // 点击Menu的情况
    if (strAction == kszOpenXmlAction)
    {
        // If enter Menu clear falg
        ClearDsskeyFlag();
#ifndef _T48
        DM_SetSelectMode(false, NULL);
#endif
        if (strContent == "menu_list.xml")
        {
            //Enter Menu
            isSucceed = MyEnterMenuByString(strClickActionCpy);
        }
        else
        {
            //Enter PageMenu
            isSucceed = SettingUI_OpenPage(SubMenuData(strClickActionCpy), true);
        }
    }
    else if (strAction == kszOpenPageAction)
    {
        //Enter Page
        //isSucceed = BaseUI_CreatePageMenu(strClickActionCpy);
        isSucceed = SettingUI_OpenPage(SubMenuData(strClickActionCpy), true);
    }
    else if (strAction == kszOpenDirectoryPage)
    {
#if IF_BUG_PLCMDIR
        if (configParser_GetConfigInt(kszPlcmPhoneEnable) == 1)
        {
            // Enter directory page.
            DM_EnterDirectoryUI(DIRECTORY_TYPE_PLCM);
        }
        else
        {
            // Enter directory page.
            DM_EnterDirectoryUI(DIRECTORY_TYPE_LOCAL);
        }
#else
        // Enter directory page.
        DM_EnterDirectoryUI(DIRECTORY_TYPE_LOCAL);
#endif
        isSucceed = true;
    }
    else if (strAction == kszOpenCallLogPage)
    {
#if IF_FEATURE_METASWITCH_CALLLOG
        if (LocalLog_IsEnable())
        {
            // Enter call log.
            DM_EnterDirectoryUI(CALL_LOG_TYPE_LOCAL);
        }
        else
        {
            // Enter call log.
            DM_EnterDirectoryUI(CALL_LOG_TYPE_MTSW);
        }
#else
        // Enter call log.
        DM_EnterDirectoryUI(CALL_LOG_TYPE_LOCAL);
#endif
        isSucceed = true;
    }
    else if (strAction == kszOpenSubPageAction)
    {
        // Enter sub page.
        //进入设置界面
        isSucceed = MyEnterSubPageByString(strClickActionCpy);
    }
    else if (strAction == kszEnterDialAction)
    {
        // Enter dial.
        isSucceed = MyEnterDialByString(strClickActionCpy);
    }
    else
    {
        // Nothing to do.
    }

    return isSucceed;
}

bool CMenuController::DoLongAction(const yl::string & strLongClickAction) const
{
    // TODO: do long action.
    return true;
}

bool CMenuController::IsSpecialDlgToShowSuccessAccount(yl::string strSubPageAction) const
{
    if (strSubPageAction == kszOpenAnonymous)
    {
        return true;
    }

    return false;
}


MenuItemListData CMenuController::GetMenuListByClickAction(
    const yl::string & strClickAction) const
{
    if (strClickAction.empty())
    {
        return MenuItemListData();
    }

    // If model pointer is NULL, we cannot to get data, so return NULL.
    if (m_pMenuModel == NULL)
    {
        return MenuItemListData();
    }

    // Parse the click-action info.
    yl::string strAction;
    yl::string strContent;
    if (!commonAPI_ParseActionInfo(&strAction, &strContent, strClickAction))
    {
        return MenuItemListData();
    }

    MenuItemListData stMILD;
    MenuItemList * pMapList = NULL;

    // If the click action is dynamic menu, we get item list
    // from MenuGenerator.
    if (g_pMenuGen != NULL && g_pMenuGen->MayDynamicCreate(strClickAction))
    {
        g_pMenuGen->GetMenuListByClickAction(&stMILD, strClickAction);
        pMapList = stMILD.m_pMenuItemList;
    }
    else
    {
        stMILD.m_strClickAction = strClickAction;
        pMapList = m_pMenuModel->GetMenuListByXmlFileName(strContent);
    }

    ////V81Beta 统一显示名称，统一从acc_GetAccountShowText获取 不显示注册不成功的账号
    bool bSpecialResult = IsSpecialDlgToShowSuccessAccount(strClickAction);
    if (bSpecialResult)
    {
        if (NULL != m_pMenuItemList)
        {
            //清空原来的链表
            m_pMenuItemList->clear();
        }

        //三级权限为开需要过滤一些选项
        if (NULL != pMapList && NULL != m_pMenuItemList)
        {
            MenuItemListIter iterm = pMapList->begin();
            for (; iterm != pMapList->end(); ++iterm)
            {
                m_pMenuItemList->push_back(*iterm);
            }
        }

        if (pMapList != NULL)
        {
            MenuItemListIter iterm = m_pMenuItemList->begin();
            for (int i = 0; iterm != m_pMenuItemList->end(); ++i)
            {
                if (!acc_IsAccountAvailable(i))
                {
                    iterm = m_pMenuItemList->erase(iterm);
                    continue;
                }
                else
                {
                    MenuItemData * pData = *iterm;
                    if (pData == NULL)
                    {
                        ++iterm;
                        continue;
                    }

                    pData->m_strDisplayName = acc_GetAccountShowText(i);

                    ++iterm;
                }
            }
        }
    }

    //先判断三级权限是否开启
    if (!feature_UserModeIsOpen())
    {
        //若三级权限没有开启则直接返回列表
        if (bSpecialResult)
        {
            stMILD.m_pMenuItemList = m_pMenuItemList;
        }
        else
        {
            stMILD.m_pMenuItemList = pMapList;
        }

        return stMILD;
    }

    if (!bSpecialResult)
    {
        if (NULL != m_pMenuItemList)
        {
            //清空原来的链表
            m_pMenuItemList->clear();
        }

        //三级权限为开需要过滤一些选项
        if (NULL != pMapList && NULL != m_pMenuItemList)
        {
            MenuItemListIter iterm = pMapList->begin();
            for (; iterm != pMapList->end(); ++iterm)
            {
                m_pMenuItemList->push_back(*iterm);
            }
        }
    }

    if (NULL != m_pMenuItemList && m_pMenuItemList->size() != 0)
    {
        MenuItemListIter ite = m_pMenuItemList->begin();
        for (; ite != m_pMenuItemList->end(); /*none*/)
        {
            MenuItemData * pItem = *ite;
            if (!feature_UserModeConfigItemShow(GetWebItemKeyByClickAction(pItem->m_strClickAction)))
            {
                ite = m_pMenuItemList->erase(ite);
            }
            else
            {
                ++ite;
            }
        }
    }
    stMILD.m_pMenuItemList = m_pMenuItemList;
    return stMILD;
}

//单纯地从model层获通过文件名获取文件中的菜单项，不做附加处理
MenuItemList * CMenuController::GetFixMenuListInFileByClickAction(const yl::string & strClickAction)
const
{
    if (strClickAction.empty())
    {
        return NULL;
    }

    // If model pointer is NULL, we cannot to get data, so return NULL.
    if (m_pMenuModel == NULL)
    {
        return NULL;
    }

    // Parse the click-action info.
    yl::string strAction;
    yl::string strContent;
    if (!commonAPI_ParseActionInfo(&strAction, &strContent, strClickAction))
    {
        return NULL;
    }

    MenuItemList * pMapList = NULL;

    pMapList = m_pMenuModel->GetMenuListByXmlFileName(strContent);
    return pMapList;
}

const yl::string CMenuController::GetTitleByClickAction(
    const yl::string & strClickAction) const
{
    // 如果标题需要动态显示, 则从MenuGenerator中获取.
    return g_pMenuTitleManager != NULL
           ? g_pMenuTitleManager->GetTitleByClickAction(strClickAction) : "";
}

yl::string CMenuController::GetPriviledgeByClickAction(
    const yl::string & strClickAction) const
{
    // 从Model中获取页面权限.
    return m_pMenuModel != NULL
           ? m_pMenuModel->GetPriviledgeByClickAction(strClickAction) : "";
}

int CMenuController::GetIndexByPageEnum(int eEnum)
{
    return m_pMenuModel != NULL
           ? m_pMenuModel->GetIndexByPageEnum(eEnum)
           : -1;
}

bool CMenuController::IsPageValid(int nPageType)
{
    if (nPageType <= MENU_PAGE_TYPE_NONE
            || nPageType >= MENU_PAGE_TYPE_MAX)
    {
        chWARNING2(false, "CMenuController::EnterPageByType page type error!");
        return false;
    }
    return true;
}

bool CMenuController::EnterPageByType(int nPageType, int nIndex/* = 0*/, bool bEnter/* = true*/)
{
    MENU_INFO("MenuController::EnterPageByType [%d, %d, %d]", nPageType, nIndex, bEnter);
    if (nPageType <= MENU_PAGE_TYPE_NONE
            || nPageType >= MENU_PAGE_TYPE_MAX)
    {
        chWARNING2(false, "CMenuController::EnterPageByType page type error!");
        return false;
    }

    switch (nPageType)
    {
    case MENU_PAGE_MAIN:
        {
            BaseUI_CreateMenu("OpenMenu(menu_list.xml)");
        }
        break;
    case MENU_PAGE_LOCAL_CALLLOG:
        {
#ifdef _T48
            DM_EnterDirectoryUI(CALL_LOG_TYPE_LOCAL, nIndex);
#endif
        }
        break;
    case MENU_PAGE_BROADSOFT_CALLLOG:
        {
            // 进入Broadsoft calllog页面.
#ifdef _T48
            DM_EnterDirectoryUI(CALL_LOG_TYPE_BROADSOFT, nIndex);
#endif
        }
        break;
#if IF_FEATURE_METASWITCH_CALLLOG
    case MENU_PAGE_METASWITCH_CALLLOG:
        {
            // 进入metaswitch calllog页面.
            DM_EnterDirectoryUI(CALL_LOG_TYPE_MTSW, nIndex);
        }
        break;
#endif
    case  MENU_SETTING_FORWARD:
        {
            //打开call forward中与索引相对的界面
            //BaseUI_CreateSubMenu("OpenPage(call_forward_list.xml)", nIndex);
            SettingUI_OpenPage(SubMenuData("OpenPage(call_forward_list.xml)",  nIndex), true);
        }
        break;
    case MENU_SETTING_CUSTOM_FORWARD:
        {
            SettingUI_OpenPage("OpenSubPage(call_forward_for_custom)");
        }
        break;
    case MENU_SETTING_CUSTOM_DND:
        {
            SettingUI_OpenPage("OpenSubPage(dnd_custom)");
        }
        break;
    case MENU_PAGE_DND:
        {
            if (configParser_GetConfigInt(kszBroadsoftActive) != 0)
            {
                //BaseUI_CreateSubMenuWithSubPage(SubMenuData("OpenMenu(call_feature_list.xml)"), SubMenuData("OpenSubPage(dnd)"));
                SettingUI_OpenPage(SubMenuData("OpenSubPage(dnd)"));
            }
            else
            {
                //BaseUI_CreateSubMenu("OpenMenu(call_feature_list.xml)", "OpenSubPage(dnd)");
                SettingUI_OpenPage(SubMenuData("OpenSubPage(dnd)"));
            }
        }
        break;
    case MENU_PAGE_ALWAYS_FORWARD:
        {
//              BaseUI_CreateSubMenuWithSubPage(SubMenuData("OpenMenu(call_feature_list.xml)")
//                                                  , SubMenuData("OpenSubPage(call_forward_always_forward)"));
            SettingUI_OpenPage(SubMenuData("OpenSubPage(call_forward_always_forward)"));
        }
        break;
    case MENU_PAGE_BUSY_FORWARD:
        {
//              BaseUI_CreateSubMenuWithSubPage(SubMenuData("OpenMenu(call_feature_list.xml)")
//                                                  , SubMenuData("OpenSubPage(call_forward_busy_forward)"));
            SettingUI_OpenPage(SubMenuData("OpenSubPage(call_forward_busy_forward)"));
        }
        break;
    case MENU_PAGE_NOANSWER_FORWARD:
        {
//              BaseUI_CreateSubMenuWithSubPage(SubMenuData("OpenMenu(call_feature_list.xml)")
//                                                  , SubMenuData("OpenSubPage(call_forward_no_answer_forward)"));
            SettingUI_OpenPage(SubMenuData("OpenSubPage(call_forward_no_answer_forward)"));
        }
        break;
    case MENU_SETTING_TEXTMESSAGE:
        {
            SettingUI_OpenPage(SubMenuData("OpenPage(text_message_list.xml)", nIndex), false);
        }
        break;
    case MENU_SETTING_NEWMESSAGE:
        {
            return SettingUI_OpenPage(SubMenuData("OpenSubPage(text_message_new_message)"));
        }
        break;
    case MENU_SETTING_MSGINBOX:
        {
            return SettingUI_OpenPage(SubMenuData("OpenSubPage(text_message_inbox)"));
        }
        break;
    case MENU_SETTING_VOICEMAIL:
        {
            SettingUI_OpenSetVoiceMailPage(nIndex);
        }
        break;
    case MENU_ZEROSPTOUCH:
        {
            zero_Enter();
        }
        break;
    case MENU_SETTING_MY_STATUS:
        {
            SettingUI_OpenPage("OpenSubPage(my_status)");
        }
        break;
    case MENU_PAGE_BUDDIES:
        {
#ifndef _T48
            DM_EnterBSFTUCUI("OpenDirectoryPage(uc_buddies)");
#else
//#warning "TODO:DM_EnterDirectoryUI"
//              DM_EnterDirectoryUI(DIRECTORY_TYPE_BUDDIES, ALL_BUDDIES_ID);
#endif
        }
        break;
    case MENU_PAGE_NETWORK_FAVORITES:
        {
#ifndef _T48
            DM_EnterBSFTUCUI("OpenDirectoryPage(uc_favourite)");
#else
//#warning "TODO:DM_EnterDirectoryUI"
//              DM_EnterDirectoryUI(DIRECTORY_TYPE_BUDDIES, FAVOURITE_ID);
#endif
        }
        break;
    case CC_PAGE_ACD_LOGIN:
        {
            SettingUI_OpenPage("hoteling_login");
        }
        break;
    case CC_PAGE_ACD_STATUS:
        {
            SettingUI_OpenPage("initial_state");
        }
        break;
    case CC_PAGE_CALLCENTER_ACDSELECTSTATE:
        {
            SettingUI_OpenPage("acd_status_select");
        }
        break;
    case CC_PAGE_CALLCENTER_HOTELINGLOGIN:
    case CC_PAGE_CALLCENTER_HOTELINGLOGINACDOFF:
        {
            SettingUI_OpenPage("hoteling_login");
        }
        break;
    case CC_PAGE_CALLCENTER_DISPOSITIONCODE:
        {
            SettingUI_OpenPage("disposition_code");
        }
        break;
    case CC_PAGE_CALLCENTER_EMERGENCYESCALATION:
        {
            SettingUI_OpenPage("emergency_escalation");
        }
        break;
    case MENU_PAGE_SET_VOICEMAIL:
        {
            SettingUI_OpenSetVoiceMailPage(nIndex);
        }
        break;
    case MENU_PAGE_AUTOPCODEAUTH:
        {
#if IF_BUG_23826
            SettingUI_OpenPage("autop_code_authorized");
#endif
        }
        break;
    case PAGE_TYPE_HOTDESK:
        {
            SettingUI_OpenPage("hotdesk_base");
        }
        break;
#ifdef IF_FEATURE_MULTIFWD
    case MENU_MULTI_FORWARD:
        {
            if (!SettingUI_IsDlgOnTop(DLG_CDlgMultiForward))
            {
                SubMenuData objSubMenuData;
                objSubMenuData.strMenuAction = "OpenSubPage(MultiForward)";
                objSubMenuData.pBeforeSetSubpageData = &nIndex;
                SettingUI_OpenPage(objSubMenuData);
            }
        }
        break;
#endif
#if IF_FEATURE_METASWITCH_ACD
    case CC_PAGE_MTSWACD_MLHG:
    case CC_PAGE_MTSW_COMMPORTAL:
        {
            SettingUI_OpenPage("mtswacdmlghlist");
        }
        break;
#endif
    case MENU_PAGE_STATUS:
        {
            SettingUI_OpenPage(SubMenuData("OpenMenu(status_list.xml)"), true);
        }
        break;
    default:
        {
            chWARNING2(false, "CMenuController::EnterPageByType page type error!");
            return false;
        }
        break;
    }

    return true;
}

// 从Dsskey设置界面进入联系人模块的
// | pCallbackFun | 联系人模块的回调指针
bool CMenuController::EnterDirFormDsskey(CMenuCallback * pMenuCallback)
{
#ifndef _T48
    // 入口参数检查
    if (NULL == pMenuCallback)
    {
        return false;
    }

    // Parse the click-action info.
    yl::string strAction;
    yl::string strContent;
    if (!commonAPI_ParseActionInfo(&strAction, &strContent, "OpenDynamic(directory)"))
    {
        return false;
    }

    yl::string strClickActionCpy("OpenDynamic(directory)");
    if (strAction == kszOpenDynamicAction)
    {
        // If the menu is dynamic menu, we should translate dynamic
        // menu to concrete menu.
        if (g_pMenuGen != NULL)
        {
            strClickActionCpy = g_pMenuGen->Dynamic2Concrete("OpenDynamic(directory)");
        }
    }

    // Parse again.
    if (!commonAPI_ParseActionInfo(&strAction, &strContent, strClickActionCpy))
    {
        return false;
    }

    bool isSucceed = false;

    // 更改标志位
    m_bDsskeyJumpDirFalg = true;
    m_pMenuCallback      = pMenuCallback;

    // 点击Menu的情况
    if (strAction == kszOpenXmlAction)
    {
        // Enter Menu
        isSucceed = MyEnterMenuByString(strClickActionCpy, true);
    }
    else if (strAction == kszOpenDirectoryPage)
    {
        // Enter directory page.
        DM_EnterDirectoryUI(DIRECTORY_TYPE_LOCAL);
        isSucceed = true;
    }

    return isSucceed;
#endif
    return true;
}

// 从Dsskey设置界面退出联系人模块
bool CMenuController::ExitDirFormDsskey()
{
#ifndef _T48
    ClearDsskeyFlag();

    // Parse the click-action info.
    yl::string strAction;
    yl::string strContent;
    if (!commonAPI_ParseActionInfo(&strAction, &strContent, "OpenDynamic(directory)"))
    {
        return false;
    }

    yl::string strClickActionCpy("OpenDynamic(directory)");
    if (strAction == kszOpenDynamicAction)
    {
        // If the menu is dynamic menu, we should translate dynamic
        // menu to concrete menu.
        if (g_pMenuGen != NULL)
        {
            strClickActionCpy = g_pMenuGen->Dynamic2Concrete("OpenDynamic(directory)");
        }
    }

    // Parse again.
    if (!commonAPI_ParseActionInfo(&strAction, &strContent, strClickActionCpy))
    {
        return false;
    }

    bool isSucceed = false;

    if (strAction == kszOpenXmlAction)
    {
        DM_ExitDirectory();
        BaseUI_DestroyMenu();
    }
    else if (strAction == kszOpenDirectoryPage)
    {
        DM_ExitDirectory();
    }

    return isSucceed;
#endif
    return true;
}


bool CMenuController::EnterPageByAction(const yl::string & strClickAction,
                                        int nSelectIndex /* = 0 */)
{
#if defined(_T3X) || defined(_T4X)
    return SettingUI_OpenPage(strClickAction, nSelectIndex);
#else
    return false;
#endif
}

bool CMenuController::MyInit()
{
    // Instantiate the model.
    if (m_pMenuModel == NULL)
    {
        m_pMenuModel = new CMenuModel();

        // 注册消息.
        MyRegisterMessage();
    }

    return true;
}

bool CMenuController::MyFinal()
{
    // Release the model.
    if (m_pMenuModel != NULL)
    {
        delete m_pMenuModel;
        m_pMenuModel = NULL;

        // 取消消息注册.
        MyUnRegisterMessage();
    }
    return true;
}

bool CMenuController::MyEnterMenuByString(const yl::string & strClickAction,
        bool bTopWindow/* = false*/) const
{
    // Call the method in UI to enter some menu.
    return BaseUI_CreateMenu(strClickAction, bTopWindow);
}

bool CMenuController::MyEnterSubPageByString(const yl::string & strClickAction) const
{
    MENU_INFO("CMenuController::MyEnterSubPageByString(%s)",
              strClickAction.c_str());

    // Call the method in UI to enter some page.
    if (strClickAction == "OpenPage(help_list.xml)")
    {
        // 打开帮助页面.
#ifndef _T48
        return SettingUI_OpenHelpPage();
#endif
        return false;
    }
    else if (strClickAction == "OpenPage(zerosptouch)")
    {
        // open zerosptouch
        zero_Enter();
        return true;
    }
    else if (strClickAction == "OpenSubPage(text_message_new_message)")
    {
        return false;
//#warning "TODO:MessageUI_GotoNewMessageDlg"
//          return MessageUI_GotoNewMessageDlg();
    }
    else if (strClickAction.find("OpenSubPage(text_message_") != yl::string::npos)
    {
        return false;
//#warning "TODO:MessageUI_CreateSubPage"
//          return MessageUI_CreateSubPage(strClickAction);
    }
    else
    {
        return SettingUI_OpenPage(SubMenuData(strClickAction));
    }
}

bool CMenuController::MyEnterDirectoryPageByString(const yl::string & strClickAction) const
{
    if (m_bDsskeyJumpDirFalg)
    {
#ifndef _T48
        DM_SetSelectMode(true, m_pMenuCallback);
#endif
    }

    bool isSucc = false;
    if (strClickAction == "OpenDirectoryPage(local_contact_list.xml)")
    {
        // 进入本地联系人页面.
        DM_EnterDirectoryUI(DIRECTORY_TYPE_LOCAL);
        isSucc = true;
    }
#if defined(_T3X) || defined(_T4X)
    else if (strClickAction == "OpenDirectoryPage(blacklist_list.xml)")
    {
        // 进入黑名单
#ifndef _T48
        isSucc = DM_EnterLocalBlacklist();
#endif
    }
#endif
    else if (strClickAction == "OpenDirectoryPage(remote_contact_list.xml)")
    {
        // 进入远程联系人页面.
#ifndef _T48
        isSucc = DM_EnterRemoteDir();
#endif
    }
    else if (strClickAction == "OpenDirectoryPage(broadsoft_contact_list.xml)")
    {
        // 进入BroadSoft联系人页面.
#ifndef _T48
        isSucc = DM_EnterBroadSoftUI();
#endif
    }
    else if (strClickAction == "OpenDirectoryPage(ldpa_contact_list.xml)")
    {
        // 进入Ldap联系人页面.
#ifndef _T48
        isSucc = DM_EnterLDAPDir();
#endif
    }
    else if (strClickAction == "OpenDirectoryPage(uc_buddies)")
    {
        // 进入UC联系人All Buddies页面.
#ifndef _T48
        isSucc = DM_EnterBSFTUCUI(strClickAction);
#endif
    }
    else if (strClickAction == "OpenDirectoryPage(uc_favourite)")
    {
        // 进入UC联系人Favourite页面.
#ifndef _T48
        isSucc = DM_EnterBSFTUCUI(strClickAction);
#endif
    }
    else
    {
        MENU_INFO("CMenuController::MyEnterDirectoryPageByString Wrong ation[%s]",
                  strClickAction.c_str());
        isSucc = false;
    }

    return isSucc;
}

bool CMenuController::MyEnterCallLogPageByString(const yl::string & strClickAction) const
{
    // Call UI.
    if (strClickAction == "OpenCallLogPage(broadsoft_calllog_list.xml)")
    {
        return DM_EnterDirectoryUI(CALL_LOG_TYPE_BROADSOFT, kszAllContactId);
    }
    else if (strClickAction == "OpenCallLogPage(local_call_log_list.xml)")
    {
        return DM_EnterDirectoryUI(CALL_LOG_TYPE_LOCAL, kszAllContactId);
    }
#if IF_FEATURE_METASWITCH_CALLLOG
    else if (strClickAction == "OpenCallLogPage(metaswitch_call_log_list.xml)")
    {
        return DM_EnterDirectoryUI(CALL_LOG_TYPE_MTSW, kszAllContactId);
    }
#endif
    else
    {
        return false;
    }
}

bool CMenuController::MyEnterDialByString(const yl::string & strClickAction) const
{
    // Dial UI.
    talklogic_EnterDialUI("");
    return true;
}

// 清空Dsskey的标志位
void CMenuController::ClearDsskeyFlag()
{
    m_bDsskeyJumpDirFalg = false;
    m_pMenuCallback = NULL;
}

CMenuController::CMenuController()
    : m_pMenuModel(NULL)
    , m_bDsskeyJumpDirFalg(false)
    , m_pMenuCallback(NULL)
    , m_pMenuItemList(NULL)
{
    m_pMenuItemList = new MenuItemList();
}

CMenuController::~CMenuController()
{
    if (NULL != m_pMenuItemList)
    {
        delete m_pMenuItemList;
        m_pMenuItemList = NULL;
    }
}

void CMenuController::MyRegisterMessage()
{
    // 注册消息.
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED,
                          CONFIG_MSG_BCAST_CHANGED,
                          MyOnLdapMessage);
}

void CMenuController::MyUnRegisterMessage()
{
    // 取消消息注册.
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED,
                            CONFIG_MSG_BCAST_CHANGED,
                            MyOnLdapMessage);
}

// Static.
LRESULT CMenuController::MyOnLdapMessage(msgObject & msg)
{
    MENU_INFO("CMenuController::MyOnLdapMessage 0x[%x] 0x[%x]",
              msg.message, msg.wParam);
    BOOL bRet = FALSE;
    const msgObject & refMsg = msg;
    switch (refMsg.message)
    {
    case CONFIG_MSG_BCAST_CHANGED:
        switch (refMsg.wParam)
        {
        case ST_LDAP:
            // 重新获取设置LDAP名.
            {
                yl::string strLdapName = LDAP_GetName();
                if (strLdapName.empty())
                {
                    strLdapName = "LDAP";
                }
                if (g_pMenuTitleManager != NULL)
                {
                    g_pMenuTitleManager->SetTitle(
                        "OpenDirectoryPage(ldpa_contact_list.xml)", strLdapName);
                }
            }

#ifndef _T48
            // 如果网页LDAP发生改变, 只是转发消息.
            etl_NotifyApp(false,
                          LDAP_STATUS_CHANGED,
                          0,
                          Directory_IsLdapContactConfigured() ? 1 : 0);
#endif
            bRet = TRUE;
            break;

        default:
            break;
        }
        break;

    default:
        break;
    }

    return bRet;
}

}  // namespace NS_MENU
