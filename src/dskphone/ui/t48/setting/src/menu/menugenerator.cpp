#include "menugenerator.h"

#include "ETLLib/ETLLib.hpp"
// #include "CommonAPI/CommonAPI.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#ifdef  _T48
// #include "directory/moddirinterface.h"
#else
#include "Directory/ContactBroadSoftModel.h"
#include "Directory/ContactGroupController.h"
#include "Directory/ContactRemoteModel.h"
#include "Directory/modDirectory.h"
#endif
#include "contacts/ldaplogic/include/modldap.h"
#ifndef _T48
#include "CallLog/BSCallLogModel.h"
#include "CallLog/modCallLog.h"
#endif
#include "path_define.h"
#include "setting/include/modmenu.h"
#include "menutitlemanager.h"
#include "talk/fwd_dnd/include/modfwddnd.h"
#include "menucontroller.h"

#ifndef _T48
#include "Directory/ContactBuddyListModel.h"
#endif

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
#include "record/include/modrecord.h"
#endif

#if IF_FEATURE_GENBAND_E911
#include "e911address/include/mode911address.h"
#endif

namespace NS_MENU
{
//////////////////////////////////////////////////////////////////////////
// Class CMenuGenerator.
CMenuGenerator * CMenuGenerator::m_pInst = NULL;

// 最大buffer长度.
static const int knMaxBufferLen = 260;

// menu主菜单
static const char kszActionMenu[] = "OpenMenu(menu_list.xml)";

//
static const char kszActionStatus[] = "OpenMenu(status_list.xml)";
// Zero各界面的点击Action
static const char kszZeroActionStatus[] = "OpenSubPage(zero_status_list.xml)";
static const char kszZeroActionNetwork[] = "OpenPage(zero_network_list.xml)";
// Zero标题
static const char kszZeroTitleStatus[] = "Status";
static const char kszZeroTitleNetwork[] = "Network";

// 将Call Feature总菜单定义.
static const char kszMenuCACallFeature[] = "OpenMenu(call_feature_list.xml)";
static const char kszPageCACallFeature[] = "OpenPage(call_feature_list.xml)";


// 以下为Call Feature菜单的定义内容.
// Call Forward菜单项的id名.
static const char kszCFMenuItemIdNameCallForward[] = "call_forward";
// Call forward菜单项的显示名.
static const char kszCFMenuItemDispNameCallForward[] = "Call Forward";
// Custom Call forward菜单项的Click Action.
static const char kszCFMenuItemClickActionCustomCallForward[] =
    "OpenSubPage(call_forward_for_custom)";
// Call forward菜单项的Click Action.
static const char kszCFMenuItemClickActionCallForward[] = "OpenPage(call_forward_list.xml)";

// Call Waiting菜单项的id名.
static const char kszCFMenuItemIdNameCallWaiting[] = "call_waiting";
// Call Waiting菜单项的显示名.
static const char kszCFMenuItemDispNameCallWaiting[] = "Call Waiting";

// Call Waiting菜单项的Click Action.
static const char kszCFMenuItemClickActionCallWaiting[] = "OpenSubPage(call_waiting)";

// Auto Answer菜单项的id名.
static const char kszCFMenuItemIdNameAutoAnswer[] = "auto_answer";
// Auto Answer菜单项的显示名.
static const char kszCFMenuItemDispNameAutoAnswer[] = "Auto Answer";

// Auto Answer菜单项的Click Action.
#ifdef IF_FEATURE_AUTO_ANSWER_TIMER
static const char kszCFMenuItemClickActionAutoAnswer[] = "OpenPage(auto_answer_list_option.xml)";
#else
static const char kszCFMenuItemClickActionAutoAnswer[] = "OpenSubPage(auto_answer)";
#endif

// DSS Keys菜单项的id名.
static const char kszCFMenuItemIdNameDssKey[] = "dss_keys";
// DSS Keys菜单项的显示名.
static const char kszCFMenuItemDispNameDssKey[] = "DSS Keys";
// DSS Keys菜单项的Click Action.
static const char kszCFMenuItemClickActionDssKey[] = "OpenSubPage(dsskey_frame)";

// DND菜单项的id名.
static const char kszCFMenuItemIdNameDnd[] = "dnd";
// DND菜单项的显示名.
static const char kszCFMenuItemDispNameDnd[] = "DND";

// DND菜单项的Click Action.
static const char kszCFMenuItemClickActionDnd[] = "OpenSubPage(dnd)";
// Custom DND菜单项的Click Action.
static const char kszCFMenuItemClickActionCustomDnd[] = "OpenSubPage(dnd_custom)";


// Intercom菜单项的id名.
static const char kszCFMenuItemIdNameIntercom[] = "intercom";
// Intercom菜单项的显示名.
static const char kszCFMenuItemDispNameIntercom[] = "Intercom";

// Intercom菜单项的Click Action.
static const char kszCFMenuItemClickActionIntercom[] = "OpenSubPage(intercom)";

// Anonymous Call菜单项的id名.
static const char kszCFMenuItemIdNameAnonymousCall[] = "anonymous_call";
// Anonymous Call菜单项的显示名.
static const char kszCFMenuItemDispNameAnonymousCall[] = "Anonymous";
// Anonymous Call菜单项的Click Action.
static const char kszCFMenuItemClickActionAnonymousCall[] = "OpenPage(anonymous_call_list.xml)";

// Call Control菜单项的id名.
static const char kszCFMenuItemIdNameCallControl[] = "callControl";
// Call Control菜单项的显示名.
static const char kszCFMenuItemDispNameCallControl[] = TRID_CALL_CONTROL;
// Call Control菜单项的Click Action.
static const char kszCFMenuItemClickActionCallControl[] = "OpenSubPage(callControl)";

// Wifi的id名
static const char kszBasicSettingItemIdNameWifi[] = "wifi";
// Advanced菜单项的id名.
static const char kszCFMenuItemIdNameAdvanced[] = "advanced";
// Advanced菜单项的显示名.
static const char kszCFMenuItemDispNameAdvanced[] = "Others";
// Advanced菜单项的Click Action.
static const char kszCFMenuItemClickActionAdvanced[] = "OpenMenu(advanced_call_feature_list.xml)";

static const char kszMenuCABasicSetting[] = "OpenMenu(basic_setting_list.xml)";
static const char kszPageCABasicSetting[] = "OpenPage(basic_setting_list.xml)";

static const char kszBasicItemClickActionResetLocal[] = "OpenSubPage(basic_reset_config)";
static const char kszBasicItemResetLocal[] = "basic_reset_config";
static const char kszBasicDispNameResetLocal[] = TRID_RESET_CONFIGURATION;

static const char kszMenuItemIdNameMyStaus[] = "mystatus";
static const char kszMenuItemDispNameMyStaus[] = TRID_MY_STATUS;
static const char kszMenuItemClickActionMyStaus[] = "OpenSubPage(my_status)";

static const char kszMenuItemClickActionForwardOption[] = "OpenSubPage(call_forward_option)";

//动态生成status菜单
static const char kszMenuCAStatus[] = "OpenMenu(status_list.xml)";
static const char kszMenuNameStatus[] = "Status";

static const char kszMenuItemIdNameStatusRTP[] = "RTPStatus";
static const char kszMenuItemDispNameStatusRTP[] = "RTP Status";
static const char kszMenuItemClickActionStatusRTP[] = "OpenSubPage(status_rtp)";

//动态生成advance settingcai菜单: 是否包含reset local configration
static const char kszMenuCAAdvancedSetting[] = "OpenMenu(advanced_setting_list.xml)";
static const char kszMenuNameAdvancedSetting[] = TRID_RESET_AND_REBOOT;

static const char kszMenuItemIdNameResetDefault[] = "reset_local_config";
static const char kszMenuItemDispNameResetDefault[] = TRID_RESET_LOCAL_SETTINGS;
static const char kszMenuItemClickActionResetDefault[] = "OpenSubPage(reset_default)";

//Message
static const char kszMenuMessage[] = "OpenMenu(message_list.xml)";
static const char kszMenuItemIdNameVoiceMail[] = "voice_mail";
static const char kszMenuItemDispNameVoiceMail[] = TRID_VOICE_MAIL;
static const char kszMenuItemClickActionVoiceMail[] = "OpenPage(voice_mail_list.xml)";
static const char kszMenuItemIdNameSMS[] = "text_message";
static const char kszMenuItemDispNameSMS[] = TRID_TEXT_MESSAGE;
static const char kszMenuItemClickActionSMS[] = "OpenPage(text_message_list.xml)";

static const char kszMenuItemIdNameMTSWVoiceMailList[] = "metaswitch_voice_mail_list";
static const char kszMenuItemDispNameMTSWVoiceMailList[] = TRID_MESSAGESLIST;
static const char kszMenuItemClickActionMTSWVoiceMailList[] =
    "OpenSubPage(metaswitch_voice_mail_list)";

//USB
static const char kszMenuUSBInterface[] = "usb_record";
static const char kszMenuDispNameUSBInterface[] = TRID_USB_RECORD;
static const char kszMenuClickActionUSBInterface[] = "OpenMenu(usb_record.xml)";
static const char kszMenuUSBIcon[] = "menu_usb_normal.png";
static const char kszMenuUSBIconSelected[] = "menu_usb_selected.png";

static const char kszUSBItemClickActionNone[] = "OpenSubPage(browse_usb_storage)";
static const char kszUSBItemNone[] = "browse_usb_storage";
static const char kszUSBDispNameUSBNone[] = TRID_NONE;

static const char kszUSBItemClickActionIdleRecord[] = "OpenSubPage(usb_idle_record)";
static const char kszUSBItemIdleRecord[] = "usb_idle_record";
static const char kszUSBDispNameIdleRecord[] = TRID_USB_IDLE_RECORD;

//diagnostics
static const char kszCFMenuItemIdNameDiagnostics[] = "diagnostics";
// diagnostics菜单项的显示名.
static const char kszCFMenuItemDispNameDiagnostics[] = TRID_DIAGNOSTICS;
// diagnostics菜单项的Click Action.
static const char kszCFMenuItemClickActionDiagnostics[] = "OpenPage(diagnostics_list.xml)";

//NetworkList
static const char kszMenuNetworkList[] = "OpenPage(network_list.xml)";

//////////////////////////////////////////////////////////////////////////
// Static.
CMenuGenerator * CMenuGenerator::GetInstance()
{
    // 实例化单键.
    if (m_pInst == NULL)
    {
        m_pInst = new CMenuGenerator();
        if (m_pInst != NULL)
        {
            m_pInst->MyInit();
        }
    }
    return m_pInst;
}

// Static.
void CMenuGenerator::ReleaseInstance()
{
    // 销毁实例.
    if (m_pInst != NULL)
    {
        m_pInst->MyFinal();

        delete m_pInst;
        m_pInst = NULL;
    }
}

bool CMenuGenerator::GetMenuListByClickAction(
    MenuItemListData * pData, const yl::string & strClickAction)
{
    if (pData == NULL || strClickAction.empty())
    {
        return false;
    }

    // 每次获取菜单之前, 都要将以前的菜单清空.
    MyClearMenuList();

    pData->m_strClickAction = strClickAction;

    if (strClickAction == kszMenuCACallFeature
            || strClickAction == kszPageCACallFeature)  // Call Feature菜单.
    {
        pData->m_pMenuItemList = MyGenCallFeatureMenuItemList();
    }
    else if (strClickAction == kszMenuCABasicSetting
             || strClickAction == kszPageCABasicSetting)
    {
        pData->m_pMenuItemList = MyGenBasicSettingMenuItemList();
    }
    else if (strClickAction == kszMenuCAStatus)
    {
        pData->m_pMenuItemList = MyGenStatusMenuItemList();
    }
    else if (strClickAction == kszMenuCAAdvancedSetting)
    {
        pData->m_pMenuItemList = MyGenAdvanceSettingMenuItemList();
    }
    else if (strClickAction == kszMenuMessage)
    {
        pData->m_pMenuItemList = MyGenMessageMenuList();
    }
    else if (strClickAction == kszMenuNetworkList)
    {
        pData->m_pMenuItemList = MyGenNetworkList();
    }
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    else if (strClickAction == kszActionMenu)
    {
        pData->m_pMenuItemList = MyGenerateMenuItemList();
    }
    else if (strClickAction == kszMenuClickActionUSBInterface)
    {
        pData->m_pMenuItemList = MyGenerateUSBItemList();
    }
#endif
    else
    {
        // Nothing to do.
    }

    return pData->m_pMenuItemList != NULL;
}

bool CMenuGenerator::MayDynamicCreate(const yl::string & strClickAction) const
{
    const DynamicMenuData * pMenuData = MyFindDynamicMenu(strClickAction);
    return pMenuData != NULL ? pMenuData->m_isDynamic : false;
}

CMenuGenerator::CMenuGenerator()
    : m_pMenuList(NULL)
{

}

CMenuGenerator::~CMenuGenerator()
{

}

bool CMenuGenerator::MyInit()
{
    // 实例化menu链表.
    if (m_pMenuList == NULL)
    {
        m_pMenuList = new MenuItemList();
    }
    return m_pMenuList != NULL && MyInitDynamicMap();
}

bool CMenuGenerator::MyFinal()
{
    // 清除menu链表.
    if (m_pMenuList != NULL)
    {
        // 清空menu链表.
        MyClearMenuList();

        delete m_pMenuList;
        m_pMenuList = NULL;
    }

    // 清空动态菜单列表.
    bool isSucceed = MyFinalDynamicMap();

    return m_pMenuList == NULL && isSucceed;
}

bool CMenuGenerator::MyInitDynamicMap()
{
    yl::string strDynamicList[] = { kszMenuCACallFeature,
                                    kszPageCACallFeature, kszMenuCABasicSetting
                                    , kszPageCABasicSetting, kszMenuCAStatus, kszMenuCAAdvancedSetting, kszMenuMessage, kszMenuNetworkList
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
                                    , kszActionMenu, kszMenuClickActionUSBInterface
#endif
                                  };

    // 插入各个动态菜单.
    bool isSucceed = true;
    for (int i = 0;
            i < sizeof(strDynamicList) / sizeof(strDynamicList[0]);
            ++i)
    {
        if (!MyAddDynamicMenu(strDynamicList[i]))
        {
            isSucceed = false;
        }
    }

    // 生成菜单标题.
    if (g_pMenuTitleManager != NULL)
    {
        g_pMenuTitleManager->SetTitle(kszActionStatus, kszZeroTitleStatus);
        //由于Zero使用的Status,Network配置文件改变，这里要单独设置其标题
        g_pMenuTitleManager->SetTitle(kszZeroActionStatus, kszZeroTitleStatus);
        g_pMenuTitleManager->SetTitle(kszZeroActionNetwork, kszZeroTitleNetwork);
        g_pMenuTitleManager->SetTitle(kszMenuCAStatus, kszMenuNameStatus);
    }

    return isSucceed;
}

bool CMenuGenerator::MyFinalDynamicMap()
{
    // 清空动态菜单.
    DynamicMenuMapIter iter = m_mapDynamicMenu.begin();
    while (iter != m_mapDynamicMenu.end())
    {
        if (iter->second != NULL)
        {
            delete iter->second;
            iter->second = NULL;
        }
    }  // while
    m_mapDynamicMenu.clear();
    return true;
}

bool CMenuGenerator::MyAddDynamicMenu(const yl::string & strClickAction)
{
    // 如果已经存在, 则更改.
    DynamicMenuData * pMenuData = NULL;
    if ((pMenuData = MyFindDynamicMenu(strClickAction)) != NULL)
    {
        pMenuData->m_isDynamic = true;
    }
    else
    {
        // 没有, 则新创建.
        if (DynamicMenuData * pNewMenuData = new DynamicMenuData())
        {
            pNewMenuData->m_isDynamic = true;
            return m_mapDynamicMenu.insert(
                       std::make_pair(strClickAction, pNewMenuData)).second;
        }
    }
    return false;
}

bool CMenuGenerator::MyRemoveDynamicMenu(const yl::string & strClickAction)
{
    // 某个动态菜单.
    DynamicMenuData * pMenuData = MyFindDynamicMenu(strClickAction);
    if (pMenuData != NULL)
    {
        pMenuData->m_isDynamic = false;
        return true;
    }
    return false;
}

CMenuGenerator::DynamicMenuData * CMenuGenerator::MyFindDynamicMenu(
    const yl::string & strClickAction)
{
    // 查找某个动态菜单.
    DynamicMenuMapIter iter = m_mapDynamicMenu.find(strClickAction);
    return iter != m_mapDynamicMenu.end() ? iter->second : NULL;
}

const CMenuGenerator::DynamicMenuData * CMenuGenerator::MyFindDynamicMenu(
    const yl::string & strClickAction) const
{
    // 查找某个动态菜单.
    DynamicMenuMapCIter citer = m_mapDynamicMenu.find(strClickAction);
    return citer != m_mapDynamicMenu.end() ? citer->second : NULL;
}

bool CMenuGenerator::MyClearMenuList()
{
    // 清空菜单列表.
    if (m_pMenuList != NULL)
    {
        MenuItemList::iterator iter = m_pMenuList->begin();
        while (iter != m_pMenuList->end())
        {
            if (*iter != NULL)
            {
                delete *iter;
                *iter = NULL;
            }
            iter = m_pMenuList->erase(iter);
        }  // while.
    }
    return true;
}

void CMenuGenerator::LoadFixMenuItemlistInFile(const yl::string & strClickAction)
{
    MenuItemList * itemList = g_pMenuController->GetFixMenuListInFileByClickAction(strClickAction);
    if (NULL == itemList)
    {
        return;
    }

    // 是否禁用wifi功能
    bool bForbidWifi = (configParser_GetConfigInt(kszForbidWifi) == 1);

    bool bForbidFwd = !fwd_IsActive();

#if IF_FEATURE_METASWITCH
    bool bForbidMtswFeature = (configParser_GetConfigInt(kszMTSWActive) == 1);
#endif

    MenuItemList::iterator iter = itemList->begin();
    MenuItemList::iterator iterEnd = itemList->end();
    for (; iter != iterEnd; ++iter)
    {
        //为了避免逻辑层MenuModel传下来的指针在MyClearMenuList()不被删除，
        //此处新建指针。
        MenuItemData * pMenuItem = new MenuItemData();
        *pMenuItem = **iter;
        // 增加Advanced菜单项.
        if (NULL != pMenuItem)
        {
            if (bForbidWifi && pMenuItem->m_strIdName == kszBasicSettingItemIdNameWifi)
            {
                //隐藏wifi
                continue;
            }

            if (bForbidFwd && pMenuItem->m_strIdName == "FWD_international")
            {
                continue;
            }

#if IF_FEATURE_METASWITCH
            if (bForbidMtswFeature && pMenuItem->m_strIdName == "callCompletion")
            {
                continue;
            }
#endif

            if (m_pMenuList != NULL)
            {
                m_pMenuList->push_back(pMenuItem);

                // 设置标题.
                if (g_pMenuTitleManager != NULL)
                {
                    g_pMenuTitleManager->SetTitle(pMenuItem->m_strClickAction,
                                                  pMenuItem->m_strDisplayName);
                }
            }
        }
    }
}

MenuItemList * CMenuGenerator::MyGenCallFeatureMenuItemList()
{
    bool bBroadsoftActive = configParser_GetConfigInt(kszBroadsoftActive) != 0
                            && configParser_GetConfigInt(kszXSISwitch) != 0;

    if (bBroadsoftActive)
    {
        // 生成Call Control菜单项.
        MyGenCFMenuItemCallControl();
    }
    else
    {
        if (fwd_IsActive())
        {
            // 生成Call Forward菜单项.
            MyGenCFMenuItemCallForward();
        }
    }

#ifdef IF_FEATURE_BROADSOFT_CALL_WAITING
    if (configParser_GetConfigInt(kszBroadsoftActive) != 1
            || configParser_GetConfigInt(kszCallWaitingMode) != 1)
    {
        // 生成Call Waiting菜单项.
        MyGenCFMenuItemCallWaiting();
    }
#else
    // 生成Call Waiting菜单项.
    MyGenCFMenuItemCallWaiting();
#endif

    // 生成Auto Answer菜单项.
    MyGenCFMenuItemAntoAnswer();

    // 生成DSS Keys菜单项.
    MyGenCFMenuItemDssKey();

    if (!bBroadsoftActive)
    {
        if (dnd_IsActive())
        {
            // 生成DND菜单项.
            MyGenCFMenuItemDnd();
        }
    }

    // 生成Intercom菜单项.
    MyGenCFMenuItemIntercom();
#if IF_BUG_30078 || IF_FEATURE_METASWITCH
    if (!bBroadsoftActive
#if IF_FEATURE_METASWITCH
            && configParser_GetConfigInt(kszMTSWActive) == 0
#endif
       )
    {
        // 生成Anonymous Call菜单项.
        MyGenCFMenuItemAnonymousCall();
    }
#else
    // 生成Anonymous Call菜单项.
    MyGenCFMenuItemAnonymousCall();
#endif

    // 生成Advanced菜单项.
    MyGenCFMenuItemAdvanced();

    //生成Diagnostics菜单项
    MyGenCFMenuItemDiagnostics();

    return m_pMenuList;
}

// 生成Call Control菜单项.
void CMenuGenerator::MyGenCFMenuItemCallControl()
{
    // 增加Call Control菜单项.
    if (MenuItemData * pMenuItem = new MenuItemData())
    {
        // 设置菜单的id-name.
        pMenuItem->m_strIdName.append(kszCFMenuItemIdNameCallControl);

        // 设置菜单的显示名.
        pMenuItem->m_strDisplayName.append(kszCFMenuItemDispNameCallControl);

        // 设置菜单的icon全路径
        pMenuItem->m_strIconName = Menu_GetDefaultIcon(eMDIT_CallControlDeftMenuItemIcon);

        // 设置菜单的click action.
        pMenuItem->m_strClickAction.append(kszCFMenuItemClickActionCallControl);

        if (m_pMenuList != NULL)
        {
            m_pMenuList->push_back(pMenuItem);

            // 设置标题.
            if (g_pMenuTitleManager != NULL)
            {
                g_pMenuTitleManager->SetTitle(kszCFMenuItemClickActionCallControl,
                                              pMenuItem->m_strDisplayName);
            }
        }
    }
}

void CMenuGenerator::MyGenCFMenuItemCallForward()
{
    // 增加Call Forward菜单项.
    if (MenuItemData * pMenuItem = new MenuItemData())
    {
        yl::string strCallForwardAction = (FWDDND_MODE_ACCOUNT == fwd_GetMode())
                                          ? kszCFMenuItemClickActionCustomCallForward
                                          : kszMenuItemClickActionForwardOption;

        // 设置菜单的id-name.
        pMenuItem->m_strIdName.append(kszCFMenuItemIdNameCallForward);

        // 设置菜单的显示名.
        pMenuItem->m_strDisplayName.append(kszCFMenuItemDispNameCallForward);

        // 设置菜单的icon全路径
        pMenuItem->m_strIconName = Menu_GetDefaultIcon(eMDIT_CallForwardDeftMenuItemIcon);

        // 设置菜单的click action.
        pMenuItem->m_strClickAction.append(strCallForwardAction);

        if (m_pMenuList != NULL)
        {
            m_pMenuList->push_back(pMenuItem);

        }
    }
}

void CMenuGenerator::MyGenCFMenuItemCallWaiting()
{
    // 增加Call Waiting菜单项.
    if (MenuItemData * pMenuItem = new MenuItemData())
    {
        // 设置菜单的id-name.
        pMenuItem->m_strIdName.append(kszCFMenuItemIdNameCallWaiting);

        // 设置菜单的显示名.
        pMenuItem->m_strDisplayName.append(kszCFMenuItemDispNameCallWaiting);

        // 设置菜单的icon全路径
        pMenuItem->m_strIconName = Menu_GetDefaultIcon(eMDIT_CallWaitingDeftMenuItemIcon);

        // 设置菜单的click action.
        pMenuItem->m_strClickAction.append(kszCFMenuItemClickActionCallWaiting);

        if (m_pMenuList != NULL)
        {
            m_pMenuList->push_back(pMenuItem);

            // 设置标题.
            if (g_pMenuTitleManager != NULL)
            {
                g_pMenuTitleManager->SetTitle(kszCFMenuItemClickActionCallWaiting,
                                              pMenuItem->m_strDisplayName);
            }
        }
    }
}

void CMenuGenerator::MyGenCFMenuItemAntoAnswer()
{
    // 增加AutoAnswer菜单项.
    if (MenuItemData * pMenuItem = new MenuItemData())
    {
        // 设置菜单的id-name.
        pMenuItem->m_strIdName.append(kszCFMenuItemIdNameAutoAnswer);

        // 设置菜单的显示名.
        pMenuItem->m_strDisplayName.append(kszCFMenuItemDispNameAutoAnswer);

        // 设置菜单的icon全路径
        pMenuItem->m_strIconName = Menu_GetDefaultIcon(eMDIT_AutoAnswerDeftMenuItemIcon);

        // 设置菜单的click action.
        pMenuItem->m_strClickAction.append(kszCFMenuItemClickActionAutoAnswer);

        if (m_pMenuList != NULL)
        {
            m_pMenuList->push_back(pMenuItem);

            // 设置标题.
            if (g_pMenuTitleManager != NULL)
            {
                g_pMenuTitleManager->SetTitle(kszCFMenuItemClickActionAutoAnswer,
                                              pMenuItem->m_strDisplayName);
            }
        }
    }
}

void CMenuGenerator::MyGenCFMenuItemDssKey()
{
    // 增加Dss Key菜单项.
    if (MenuItemData * pMenuItem = new MenuItemData())
    {
        // 设置菜单的id-name.
        pMenuItem->m_strIdName.append(kszCFMenuItemIdNameDssKey);

        // 设置菜单的显示名.
        pMenuItem->m_strDisplayName.append(kszCFMenuItemDispNameDssKey);

        // 设置菜单的icon全路径
        pMenuItem->m_strIconName = Menu_GetDefaultIcon(eMDIT_DssKeyDeftMenuItemIcon);

        // 设置菜单的click action.
        pMenuItem->m_strClickAction.append(kszCFMenuItemClickActionDssKey);

        if (m_pMenuList != NULL)
        {
            m_pMenuList->push_back(pMenuItem);

            // 设置标题.
            if (g_pMenuTitleManager != NULL)
            {
                g_pMenuTitleManager->SetTitle(kszCFMenuItemClickActionDssKey,
                                              pMenuItem->m_strDisplayName);
            }
        }
    }
}

void CMenuGenerator::MyGenCFMenuItemDnd()
{
    // 增加DND菜单项.
    if (MenuItemData * pMenuItem = new MenuItemData())
    {
        yl::string strDndClickAction = (FWDDND_MODE_ACCOUNT == dnd_GetMode())
                                       ? kszCFMenuItemClickActionCustomDnd
                                       : kszCFMenuItemClickActionDnd;

        // 设置菜单的id-name.
        pMenuItem->m_strIdName.append(kszCFMenuItemIdNameDnd);

        // 设置菜单的显示名.
        pMenuItem->m_strDisplayName.append(kszCFMenuItemDispNameDnd);

        // 设置菜单的icon全路径
        pMenuItem->m_strIconName = Menu_GetDefaultIcon(eMDIT_DNDDeftMenuItemIcon);

        // 设置菜单的click action.
        pMenuItem->m_strClickAction.append(strDndClickAction);

        if (m_pMenuList != NULL)
        {
            m_pMenuList->push_back(pMenuItem);

            // 设置标题.
            if (g_pMenuTitleManager != NULL)
            {
                g_pMenuTitleManager->SetTitle(strDndClickAction,
                                              pMenuItem->m_strDisplayName);
            }
        }
    }
}

void CMenuGenerator::MyGenCFMenuItemIntercom()
{
    // 增加Intercom菜单项.
    if (MenuItemData * pMenuItem = new MenuItemData())
    {
        // 设置菜单的id-name.
        pMenuItem->m_strIdName.append(kszCFMenuItemIdNameIntercom);

        // 设置菜单的显示名.
        pMenuItem->m_strDisplayName.append(kszCFMenuItemDispNameIntercom);

        // 设置菜单的icon全路径
        pMenuItem->m_strIconName = Menu_GetDefaultIcon(eMDIT_IntercomDeftMenuItemIcon);

        // 设置菜单的click action.
        pMenuItem->m_strClickAction.append(kszCFMenuItemClickActionIntercom);

        if (m_pMenuList != NULL)
        {
            m_pMenuList->push_back(pMenuItem);

            // 设置标题.
            if (g_pMenuTitleManager != NULL)
            {
                g_pMenuTitleManager->SetTitle(kszCFMenuItemClickActionIntercom,
                                              pMenuItem->m_strDisplayName);
            }
        }
    }
}

void CMenuGenerator::MyGenCFMenuItemAnonymousCall()
{
    // 增加Anonymous Call菜单项.
    if (MenuItemData * pMenuItem = new MenuItemData())
    {
        // 设置菜单的id-name.
        pMenuItem->m_strIdName.append(kszCFMenuItemIdNameAnonymousCall);

        // 设置菜单的显示名.
        pMenuItem->m_strDisplayName.append(kszCFMenuItemDispNameAnonymousCall);

        // 设置菜单的icon全路径
        pMenuItem->m_strIconName = Menu_GetDefaultIcon(eMDIT_AnonymouseCallDeftMenuItemIcon);

        // 设置菜单的click action.
        pMenuItem->m_strClickAction.append(kszCFMenuItemClickActionAnonymousCall);

        if (m_pMenuList != NULL)
        {
            m_pMenuList->push_back(pMenuItem);

            // 设置标题.
            if (g_pMenuTitleManager != NULL)
            {
                g_pMenuTitleManager->SetTitle(kszCFMenuItemClickActionAnonymousCall,
                                              pMenuItem->m_strDisplayName);
            }
        }
    }
}

void CMenuGenerator::MyGenCFMenuItemAdvanced()
{
    LoadFixMenuItemlistInFile(kszCFMenuItemClickActionAdvanced);
}

MenuItemList * CMenuGenerator::MyGenBasicSettingMenuItemList()
{
    LoadFixMenuItemlistInFile(kszMenuCABasicSetting);

    //http://10.2.1.199/Bug.php?BugID=92956
//  if (configParser_GetConfigInt(kszEnableUC) == 1
//      && configParser_GetConfigInt(kszBroadsoftActive) == 1)
//  {
//      MyGenCFMenuItemBasicSettingMyStatus();
//  }

#if IF_FEATURE_GENBAND_E911
    if (!e911_IsGenbandE911On())
    {
        yl::string strIdName = "e911_location";
        RemoveItemByIdName(strIdName);
    }
#endif

    if (configParser_GetConfigInt(kszAutoProvisionCustomProtect) == 1
            && configParser_GetConfigInt(kszShowResetLocalInBasic) == 1)
    {
        MyGenBasicSettingMenuItemResetLocal();
    }

    return m_pMenuList;
}

void CMenuGenerator::MyGenCFMenuItemBasicSettingMyStatus()
{
    // 增加Broadsoft Directory菜单项.
    if (MenuItemData * pMenuItem = new MenuItemData())
    {
        // 设置菜单的id-name.
        pMenuItem->m_strIdName.append(kszMenuItemIdNameMyStaus);

        // 设置菜单的显示名.
        pMenuItem->m_strDisplayName.append(kszMenuItemDispNameMyStaus);

        // 设置菜单的icon全路径
        pMenuItem->m_strIconName = Menu_GetDefaultIcon(eMDIT_MyStatusDeftMenuItemIcon);

        // 设置菜单的click action.
        pMenuItem->m_strClickAction.append(kszMenuItemClickActionMyStaus);

        if (m_pMenuList != NULL)
        {
            m_pMenuList->push_back(pMenuItem);

            // 设置标题.
            if (g_pMenuTitleManager != NULL)
            {
                g_pMenuTitleManager->SetTitle(kszMenuItemClickActionMyStaus,
                                              pMenuItem->m_strDisplayName);
            }
        }
    }
}

MenuItemList * CMenuGenerator::MyGenStatusMenuItemList()
{
    //先加载固定菜单
    LoadFixMenuItemlistInFile(kszMenuCAStatus);

    //RTCP VOICE Quality Report开启的时候添加相应菜单
    if (0 != configParser_GetConfigInt(kszDisplayVQReportOnUI))
    {
        MyGenCFMenuItemStatusRTP();
    }

    return m_pMenuList;
}

void CMenuGenerator::MyGenCFMenuItemStatusRTP()
{
    if (MenuItemData * pMenuItem = new MenuItemData())
    {
        pMenuItem->m_strIdName.append(kszMenuItemIdNameStatusRTP);

        pMenuItem->m_strDisplayName.append(kszMenuItemDispNameStatusRTP);

        pMenuItem->m_strClickAction.append(kszMenuItemClickActionStatusRTP);

        if (m_pMenuList != NULL)
        {
            m_pMenuList->push_back(pMenuItem);

            // 设置标题.
            if (g_pMenuTitleManager != NULL)
            {
                g_pMenuTitleManager->SetTitle(kszMenuItemClickActionStatusRTP,
                                              pMenuItem->m_strDisplayName);
            }
        }
    }
}

MenuItemList * CMenuGenerator::MyGenAdvanceSettingMenuItemList()
{
    LoadFixMenuItemlistInFile(kszMenuCAAdvancedSetting);


    // 同其他机型，该项放到Reset Config项中
//  if (configParser_GetConfigInt(kszAutoProvisionCustomProtect) != 0)
//  {
//      MyGenAdvanceSettingMenuItemResetDefault();
//  }

    return m_pMenuList;
}

void CMenuGenerator::MyGenAdvanceSettingMenuItemResetDefault()
{
    if (MenuItemData * pMenuItem = new MenuItemData())
    {
        pMenuItem->m_strIdName.append(kszMenuItemIdNameResetDefault);

        pMenuItem->m_strDisplayName.append(kszMenuItemDispNameResetDefault);

        pMenuItem->m_strClickAction.append(kszMenuItemClickActionResetDefault);

        if (m_pMenuList != NULL)
        {
            m_pMenuList->push_back(pMenuItem);

            // 设置标题.
            if (g_pMenuTitleManager != NULL)
            {
                g_pMenuTitleManager->SetTitle(kszMenuItemClickActionResetDefault,
                                              pMenuItem->m_strDisplayName);
            }
        }
    }
}

MenuItemList * CMenuGenerator::MyGenMessageMenuList()
{
    MyGenMessageMenuItemVoiceMail();

#if IF_FEATURE_METASWITCH_VOICEMAIL
    //是否开启metaswitch voicemail功能
    if (configParser_GetConfigInt(kszMetaSwitchActive) == 1
            && configParser_GetConfigInt(kszMTSWCommportalEnable) == 1
            && configParser_GetConfigInt(kszMTSWVMListEnable) == 1)
    {
        MyGenMessageMenuItemMTSWVoiceMail();
    }
#endif

    if (configParser_GetConfigInt(kszSMSEnable) == 1)
    {
        MyGenMessageMenuItemSMS();
    }

    return m_pMenuList;
}

void CMenuGenerator::MyGenMessageMenuItemVoiceMail()
{
    if (MenuItemData * pMenuItem = new MenuItemData())
    {
        pMenuItem->m_strIdName.append(kszMenuItemIdNameVoiceMail);

        pMenuItem->m_strDisplayName.append(kszMenuItemDispNameVoiceMail);

        pMenuItem->m_strClickAction.append(kszMenuItemClickActionVoiceMail);

        if (m_pMenuList != NULL)
        {
            m_pMenuList->push_back(pMenuItem);

            // 设置标题.
            if (g_pMenuTitleManager != NULL)
            {
                g_pMenuTitleManager->SetTitle(kszMenuItemClickActionVoiceMail,
                                              pMenuItem->m_strDisplayName);
            }
        }
    }
}

#if IF_FEATURE_METASWITCH_VOICEMAIL
void CMenuGenerator::MyGenMessageMenuItemMTSWVoiceMail()
{
    if (MenuItemData * pMenuItem = new MenuItemData())
    {
        pMenuItem->m_strIdName.append(kszMenuItemIdNameMTSWVoiceMailList);

        pMenuItem->m_strDisplayName.append(kszMenuItemDispNameMTSWVoiceMailList);

        pMenuItem->m_strClickAction.append(kszMenuItemClickActionMTSWVoiceMailList);

        if (m_pMenuList != NULL)
        {
            m_pMenuList->push_back(pMenuItem);

            // 设置标题.
            if (g_pMenuTitleManager != NULL)
            {
                g_pMenuTitleManager->SetTitle(kszMenuItemClickActionMTSWVoiceMailList,
                                              pMenuItem->m_strDisplayName);
            }
        }
    }
}
#endif

/****************Diagnostics****************************/
// 生成 Diagnostics菜单项.
void CMenuGenerator::MyGenCFMenuItemDiagnostics()
{
    // 增加Advanced菜单项.
    if (MenuItemData * pMenuItem = new MenuItemData())
    {
        // 设置菜单的id-name.
        pMenuItem->m_strIdName.append(kszCFMenuItemIdNameDiagnostics);

        // 设置菜单的显示名.
        pMenuItem->m_strDisplayName.append(kszCFMenuItemDispNameDiagnostics);

        // 设置菜单的icon全路径
        pMenuItem->m_strIconName = Menu_GetDefaultIcon(eMDIT_DeftMenuIconPath);

        // 设置菜单的click action.
        pMenuItem->m_strClickAction.append(kszCFMenuItemClickActionDiagnostics);

        if (m_pMenuList != NULL)
        {
            m_pMenuList->push_back(pMenuItem);

            // 设置标题.
            if (g_pMenuTitleManager != NULL)
            {
                g_pMenuTitleManager->SetTitle(kszCFMenuItemClickActionDiagnostics,
                                              pMenuItem->m_strDisplayName);
            }
        }
    }
}

void CMenuGenerator::MyGenMessageMenuItemSMS()
{
    if (MenuItemData * pMenuItem = new MenuItemData())
    {
        pMenuItem->m_strIdName.append(kszMenuItemIdNameSMS);

        pMenuItem->m_strDisplayName.append(kszMenuItemDispNameSMS);

        pMenuItem->m_strClickAction.append(kszMenuItemClickActionSMS);

        if (m_pMenuList != NULL)
        {
            m_pMenuList->push_back(pMenuItem);

            // 设置标题.
            if (g_pMenuTitleManager != NULL)
            {
                g_pMenuTitleManager->SetTitle(kszMenuItemClickActionSMS,
                                              pMenuItem->m_strDisplayName);
            }
        }
    }
}

void CMenuGenerator::RemoveItemByIdName(const yl::string & strIdName)
{
    if (m_pMenuList == NULL)
    {
        return;
    }

    MenuItemListIter iter = m_pMenuList->begin();
    for (; iter != m_pMenuList->end(); ++iter)
    {
        if (NULL != *iter
                && strcmp((*iter)->m_strIdName.c_str(), strIdName.c_str()) == 0)
        {
            delete *iter;
            *iter = NULL;
            m_pMenuList->erase(iter);
            break;
        }
    }
}

MenuItemList * CMenuGenerator::MyGenNetworkList()
{
    LoadFixMenuItemlistInFile(kszMenuNetworkList);

    if (configParser_GetConfigInt(kszForbidVPN) == 1)
    {
        yl::string strIdName = "vpn";
        RemoveItemByIdName(strIdName);
    }

    return m_pMenuList;
}

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
MenuItemList * CMenuGenerator::MyGenerateMenuItemList()
{
    LoadFixMenuItemlistInFile(kszActionMenu);
#if 0//zzl 无论是否连接上都显示USB图标
    if (modRecord_IsStorageConnect())
#endif
    {
        MyGenerateUSBMenu();
    }

    return m_pMenuList;
}

void CMenuGenerator::MyGenerateUSBMenu()
{
    if (MenuItemData * pMenuItem = new MenuItemData())
    {
        pMenuItem->m_strIdName.append(kszMenuUSBInterface);

        pMenuItem->m_strDisplayName.append(kszMenuDispNameUSBInterface);

        pMenuItem->m_strClickAction.append(kszMenuClickActionUSBInterface);

        yl::string strIconPath = Menu_GetDefaultIcon(eMDIT_DeftMenuIconPath);

        pMenuItem->m_strIconName.append(strIconPath.c_str()).append(kszMenuUSBIcon);

        pMenuItem->m_strSelectedIconName.append(strIconPath.c_str()).append(kszMenuUSBIconSelected);

        if (m_pMenuList != NULL)
        {
            m_pMenuList->push_back(pMenuItem);

            // 设置标题.
            if (g_pMenuTitleManager != NULL)
            {
                g_pMenuTitleManager->SetTitle(kszMenuClickActionUSBInterface,
                                              pMenuItem->m_strDisplayName);
            }
        }
    }
}

void CMenuGenerator::MyGenerateUSBIdleReocrd()
{
    if (MenuItemData * pMenuItem = new MenuItemData())
    {
        // 设置菜单的id-name.
        pMenuItem->m_strIdName.append(kszUSBItemIdleRecord);

        // 设置菜单的显示名.
        pMenuItem->m_strDisplayName.append(kszUSBDispNameIdleRecord);

        // 设置菜单的icon全路径
        pMenuItem->m_strIconName = Menu_GetDefaultIcon(eMDIT_DeftMenuIconPath);

        // 设置菜单的click action.
        pMenuItem->m_strClickAction.append(kszUSBItemClickActionIdleRecord);

        if (m_pMenuList != NULL)
        {
            m_pMenuList->push_back(pMenuItem);

            // 设置标题.
            if (g_pMenuTitleManager != NULL)
            {
                g_pMenuTitleManager->SetTitle(kszUSBItemClickActionIdleRecord,
                                              kszUSBDispNameIdleRecord);
            }
        }
    }
}

MenuItemList * CMenuGenerator::MyGenerateUSBItemList()
{
    if (modRecord_IsStorageConnect())
    {
        LoadFixMenuItemlistInFile(kszMenuClickActionUSBInterface);

        if (0)
        {
            MyGenerateUSBIdleReocrd();
        }
    }
    else if (MenuItemData * pMenuItem = new MenuItemData())
    {
        // 设置菜单的id-name.
        pMenuItem->m_strIdName.append(kszUSBItemNone);

        // 设置菜单的显示名.
        pMenuItem->m_strDisplayName.append(kszUSBDispNameUSBNone);

        // 设置菜单的icon全路径
        pMenuItem->m_strIconName = Menu_GetDefaultIcon(eMDIT_DeftMenuIconPath);

        // 设置菜单的click action.
        pMenuItem->m_strClickAction.append(kszUSBItemClickActionNone);

        if (m_pMenuList != NULL)
        {
            m_pMenuList->push_back(pMenuItem);

            // 设置标题.
            if (g_pMenuTitleManager != NULL)
            {
                g_pMenuTitleManager->SetTitle(kszUSBItemClickActionNone,
                                              kszMenuDispNameUSBInterface);
            }
        }
    }

    return m_pMenuList;
}
#endif

void CMenuGenerator::MyGenBasicSettingMenuItemResetLocal()
{
    if (MenuItemData* pMenuItem = new MenuItemData())
    {
        // 设置菜单的id-name.
        pMenuItem->m_strIdName.append(kszBasicItemResetLocal);

        // 设置菜单的显示名.
        pMenuItem->m_strDisplayName.append(kszBasicDispNameResetLocal);

        // 设置菜单的icon全路径
        pMenuItem->m_strIconName = Menu_GetDefaultIcon(eMDIT_DeftMenuIconPath);

        // 设置菜单的click action.
        pMenuItem->m_strClickAction.append(kszBasicItemClickActionResetLocal);

        if (m_pMenuList != NULL)
        {
            m_pMenuList->push_back(pMenuItem);

            // 设置标题.
            if (g_pMenuTitleManager != NULL)
            {
                g_pMenuTitleManager->SetTitle(kszBasicItemClickActionResetLocal,
                                              kszBasicDispNameResetLocal);
            }
        }
    }
}

}  // namespace NS_MENU
