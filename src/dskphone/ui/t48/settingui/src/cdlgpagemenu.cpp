#include "cdlgpagemenu.h"
#include "setting/include/modmenu.h"
#include "uikernel/languagehelper.h"
#include "qtcommon/qmisc.h"
#include "keymap.h"
#include "setting/include/authoritymessagedefine.h"
#include "baselibs/include/etlmsghandler/modetlmsghandler.h"
#include "setting/include/modmenu.h"
#include "dsklog/log.h"
#include "directoryui/include/moddirectoryui.h"

#include <configiddefine.h>
#include "configparser/modconfigparser.h"

#define IF_DIRCTORY_LIST_IN_SETTINGUI   0

#if IF_DIRCTORY_LIST_IN_SETTINGUI
// 打开联系人菜单列表的Click Action.
static const char kszMenuCADirectory[] = "OpenMenu(directory_list.xml)";
// 打开本地联系人页面的Click Action.
static const char kszMenuCALocal[] = "OpenDirectoryPage(local_contact_list.xml)";
// 打开黑名单页面的Click Action.
static const char  kszMenuCABlacklist[] = "OpenDirectoryPage(blacklist_list.xml)";
// 打开远程联系人页面的Click Action.
static const char kszMenuCARemote[] = "OpenDirectoryPage(remote_contact_list.xml)";
// 打开BroadSoft远程联系人页面的Click Action.
static const char kszMenuCABroadSoft[] = "OpenDirectoryPage(broadsoft_contact_list.xml)";
// 打开Ldap联系人页面的Click Action.
static const char kszMenuCALdap[] = "OpenDirectoryPage(ldpa_contact_list.xml)";
// 打开UC联系人页面的Click Action.
static const char kszMenuCAUC[] = "OpenMenu(uc_contact_list.xml)";
#if IF_BUG_PLCMDIR
// 打开plcm联系人页面的Click Action.
static const char kszMenuCAPlcm[] = "OpenDirectoryPage(plcm_contact_list.xml)";
#endif
#endif


#define kszVoiceMaillistActionContent "voice_mail_list.xml"

//消息回调函数
static LRESULT OnPageMenuMsg(msgObject & objMessage)
{
    BASEUI_INFO("OnPageMenuMsg MsgID(%d)", objMessage.message);

    switch (objMessage.message)
    {
    case AUTHORITY_MESSAGE_AUTHORITY_CHANGED:
    {
        PageMenuList listMenu = g_PageMenuManager.GetinstanceList();
        PageMenuList::iterator iter = listMenu.begin();
        for (; iter != listMenu.end(); ++iter)
        {
            CDlgPageMenu * pDlgPageMenu = *iter;
            if (pDlgPageMenu == NULL)
            {
                chWARNING2(FALSE, "OnPageMenuMsg Menu is inexistence");
                continue;
            }

            // 刷新PageMenu
            pDlgPageMenu->UpdatePageMenuFromMSG();
        }
    }
    break;
    default:
        break;
    }
    return 0;
}

CBaseDialog * CDlgPageMenu::CreateInstance()
{
    return new CDlgPageMenu();
}

CDlgPageMenu::CDlgPageMenu(QWidget * parent /*= 0*/)
    : CDlgBaseSubPage(parent)
    , m_bResfreshOnReshow(false)
{
}

CDlgPageMenu::~CDlgPageMenu()
{
}

void CDlgPageMenu::Init()
{
    g_PageMenuManager.AddPageMenu(this);
}

void CDlgPageMenu::Uninit()
{
    g_PageMenuManager.DeletePageMenu(this);

    m_vecWidgetPair.clear();
    if (NULL != m_pListDelegate)
    {
        m_pListDelegate->ClearContent(true);
    }
}

bool CDlgPageMenu::IsAddIcon(yl::string strClickAction)
{
    if (strClickAction.empty())
    {
        return false;
    }

    //需要显示图标的三级菜单请在下面添加
    if (m_strSubPageAction == "OpenPage(text_message_list.xml)"
            /*|| m_strSubPageAction == xxx*/)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool CDlgPageMenu::OnAction(const QString & strAction)
{
    if (strAction == "enter")
    {
        //直接再创建一个页面
        emit openpage(SubMenuData(GetCurrentItemAction().toUtf8().data()));
        return true;
    }
#if IF_DIRCTORY_LIST_IN_SETTINGUI
    else if (strAction == "SearchDir")
    {
        QString strMenuAction = GetCurrentItemAction();

        /*if (kszMenuCALdap == strMenuAction)
        {
            DM_EnterSearch(DIRECTORY_TYPE_LDAP, knInvalidRId);
        }
        else if (kszMenuCALocal == strMenuAction)
        {
            DM_EnterSearch(DIRECTORY_TYPE_LOCAL, knInvalidRId);
        }
        else if (kszMenuCABlacklist == strMenuAction)
        {
            DM_EnterSearch(DIRECTORY_TYPE_LOCAL, kszBlackListGroupId);
        }
        else if (kszMenuCARemote == strMenuAction)
        {
            DM_EnterSearch(DIRECTORY_TYPE_REMOTE, kszRootId);
        }
        else if (kszMenuCABroadSoft == strMenuAction)
        {
            DM_EnterSearch(DIRECTORY_TYPE_BROADSOFT, knInvalidRId);
        }
        else if ("OpenMenu(uc_contact_list.xml)" == strMenuAction)
        {
        }
        else
        {
            return false;
        }*/

        return true;
    }
#endif
    return false;
}

void CDlgPageMenu::OnListItemClicked(QString strItemAction)
{
    emit openpage(SubMenuData(strItemAction.toUtf8().data()));
}

void CDlgPageMenu::SetContent(MenuItemList * pItemList)
{
    if (pItemList == NULL || m_pListDelegate == NULL)
    {
        return;
    }

    ClearAllContent();

    bool bAddIcon = IsAddIcon(m_strSubPageAction);
    MenuItemListIter iter = pItemList->begin();
    for (int i = 0; iter != pItemList->end(); ++iter, ++i)
    {
        MenuItemData * pData = *iter;
        if (pData == NULL)
        {
            continue;
        }

        QWidgetPair qpair(SETTING_ITEM_MENU);
        qpair.first =  LANG_TRANSLATE(
                           pData->m_strDisplayName.c_str()); //pData->m_bShouldBeTranslated ? LANG_TRANSLATE(pData->m_strDisplayName.c_str()) : toQString(pData->m_strDisplayName);
        qpair.third = toQString(pData->m_strClickAction.c_str());
        qpair.bAddIcon = bAddIcon;
        if (bAddIcon)
        {
            qpair.strIcon = pData->m_strIconName;
        }

        m_vecWidgetPair.push_back(qpair);
    }

    AddWidget(m_vecWidgetPair, true, false, true);
}

// 设置子页面的SoftKey
bool CDlgPageMenu::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
#if IF_DIRCTORY_LIST_IN_SETTINGUI
    if (m_strSubPageAction == kszMenuCADirectory)
    {
        objWndData[2].m_strSoftkeyAction = "SearchDir";
        objWndData[2].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_SEARCH);
    }
    else
#endif
    {
        objWndData[2].m_strSoftkeyAction.clear();
        objWndData[2].m_strSoftkeyTitle.clear();
    }

    objWndData[3].m_strSoftkeyAction.clear();
    objWndData[3].m_strSoftkeyTitle.clear();

    return true;
}

void CDlgPageMenu::SetData(void * pData/* = NULL*/)
{
}

bool CDlgPageMenu::LoadPageData()
{
    MenuItemListData pagemenuItemListData = Menu_GetMenuListByClickAction(m_strSubPageAction);
    SetContent(pagemenuItemListData.m_pMenuItemList);
    return true;
}

bool CDlgPageMenu::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (pObject == NULL || pEvent == NULL)
    {
        return false;
    }

    if (pEvent->type() == QEvent::KeyPress)
    {
        QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;
        /*if (pKeyEvent->key() == PHONE_KEY_OK)
        {
            OnAction("enter");
            return true;
        }*/

        if (pKeyEvent->key() == PHONE_KEY_LEFT)
        {
            if (NULL != m_pListDelegate)
            {
                m_pListDelegate->GoPrePage();
                return true;
            }
        }

        if (pKeyEvent->key() == PHONE_KEY_RIGHT)
        {
            if (NULL != m_pListDelegate)
            {
                m_pListDelegate->GoNextPage();
                return true;
            }
        }

        if (m_strSubPageAction.find(kszVoiceMaillistActionContent) != yl::string::npos
                && (pKeyEvent->key() == PHONE_KEY_MESSAGE))
        {
#if IF_FEATURE_BROADSOFT_VOICEMAIL
            if (configParser_GetConfigInt(kszVoiceMailMessageKeyMode) == 1)
            {
                return true;
            }
#endif

            OnAction("enter");
            return true;
        }
    }

    return CDlgBaseSubPage::eventFilter(pObject, pEvent);
}

void CDlgPageMenu::UpdatePageMenuFromMSG()
{
    if (NULL == m_pGeneralFrame || m_pGeneralFrame->GetTotalDataCount() == 0)
    {
        return;
    }

    if (m_pListDelegate != NULL)
    {
        LoadPageData();
        if (!IsTopPage())
        {
            m_bResfreshOnReshow = true;
        }
    }
}

void CDlgPageMenu::OnReShow()
{
    if (!m_bResfreshOnReshow)
    {
        CDlgBaseSubPage::OnReShow();
        return;
    }
    else
    {
        UpdateWnd();
        m_bResfreshOnReshow = false;
    }
}


/***********************************************about class pamenumanager*************************************/
CPagemenuManager::CPagemenuManager()
{
    etl_RegisterMsgHandle(AUTHORITY_MESSAGE_AUTHORITY_CHANGED,
                          AUTHORITY_MESSAGE_AUTHORITY_CHANGED,
                          OnPageMenuMsg);
}

CPagemenuManager::~CPagemenuManager()
{
    etl_UnregisterMsgHandle(AUTHORITY_MESSAGE_AUTHORITY_CHANGED,
                            AUTHORITY_MESSAGE_AUTHORITY_CHANGED,
                            OnPageMenuMsg);
}

void CPagemenuManager::AddPageMenu(CDlgPageMenu * pPageMenu)
{
    if (NULL == pPageMenu)
    {
        return;
    }

    PageMenuList::iterator iter = PageMenuInstanceList.begin();
    for (; iter != PageMenuInstanceList.end(); ++iter)
    {
        if (*iter == pPageMenu)
        {
            return;
        }
    }

    PageMenuInstanceList.push_back(pPageMenu);
}

void CPagemenuManager::DeletePageMenu(CDlgPageMenu * pPageMenu)
{
    PageMenuList::iterator iter = PageMenuInstanceList.begin();
    while (iter != PageMenuInstanceList.end())
    {
        if (pPageMenu == *iter)
        {
            iter = PageMenuInstanceList.erase(iter);
            break;
        }
        else
        {
            ++iter;
        }
    }
}

PageMenuList & CPagemenuManager::GetinstanceList()
{
    return PageMenuInstanceList;
}
