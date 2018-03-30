#include "dlgtalkpool.h"

/////////////talkui////////////
#include "talkuicommonfunction.h"
#include "talkuiframemanager.h"
#include "dlgbasetalkui.h"
#include "modtalkui.h"

//////////////////talkuilogic///////////////
#include "talkuilogic/talkuilogicactiondefine.h"

////////////uimanager///////////
#include "uimanager/moduimanager.h"

// #include "uilogiccommon/include/uilogiclanghelper.h"

#include "directoryui/include/moddirectoryui.h"


namespace talkui
{
void CDlgTalkPool::talkPool_OpenPage(const yl::string & strAction)
{
    if (strAction == POOL_ACTION_LOCAL_CONTACT)
    {
        DM_EnterDirectoryUI(DIRECTORY_TYPE_LOCAL, knInvalidRId);
    }
    else if (strAction == POOL_ACTION_REMOTE_CONTACT)
    {
        //根据要求 进入的是目录界面
        DM_EnterDirectoryUI(DIRECTORY_TYPE_REMOTE, kszRootId);
    }
    else if (strAction == POOL_ACTION_LDAP_CONTACT)
    {
        DM_EnterDirectoryUI(DIRECTORY_TYPE_LDAP, knInvalidRId);
    }
    else if (strAction == POOL_ACTION_BST_CONTACT)
    {
#ifdef IF_FEATURE_DIR_OPT
        DM_EnterDirectoryUI(DIRECTORY_TYPE_BROADSOFT, kszAllContactId);
#else
        DM_EnterDirectoryUI(DIRECTORY_TYPE_BROADSOFT, knInvalidRId);
#endif
    }
    else if (strAction ==
             POOL_ACTION_NETWORK_CONTACT)      //这个 用于 M7配置模板Bug修改 IF_M7_MODIFY  与POOL_ACTION_BST_CONTACT 是同一种类型  只是适用版本不同
    {
#ifdef IF_FEATURE_DIR_OPT
        DM_EnterDirectoryUI(DIRECTORY_TYPE_BROADSOFT, kszAllContactId);
#else
        DM_EnterDirectoryUI(DIRECTORY_TYPE_BROADSOFT, knInvalidRId);
#endif
    }
    else if (strAction == POOL_ACTION_BST_CALLLOG)
    {
#if IF_DIR_UI_SWITCH_GROUP_IN_BROWSE
        DM_EnterDirectoryUI(CALL_LOG_TYPE_BROADSOFT);
#else
        DM_EnterDirectoryUI(CALL_LOG_TYPE_BROADSOFT, knInvalidRId);
#endif
    }
    else if (strAction == POOL_ACTION_LOCAL_CALLLOG)
    {
#if IF_DIR_UI_SWITCH_GROUP_IN_BROWSE
        DM_EnterDirectoryUI(CALL_LOG_TYPE_LOCAL);
#else
        DM_EnterDirectoryUI(CALL_LOG_TYPE_LOCAL, knInvalidRId);
#endif
    }
    else if (POOL_ACTION_UC_BUDDIES == strAction)
    {
#ifdef IF_FEATURE_DIR_OPT
        DM_EnterDirectoryUI(DIRECTORY_TYPE_BTUC, kszAllBuddiesId);
#else
        DM_EnterDirectoryUI(DIRECTORY_TYPE_BTUC, knInvalidRId);
#endif
    }
#if IF_BUG_PLCMDIR
    else if (POOL_ACTION_PLCM_CONTACT == strAction)
    {
        DM_EnterDirectoryUI(DIRECTORY_TYPE_PLCM, kszPlcmDirGroupId);
    }
#endif
#ifdef IF_FEATURE_PSTN
    else if (POOL_ACTION_FAVORITE_DIRECTORY == strAction)
    {
        DM_EnterDirectoryUI(DIRECTORY_TYPE_LOCAL, kszFavoriteGroupId);
    }
#endif
#if IF_FEATURE_GBBOOK
    else if (POOL_ACTION_GENBAND_CONTACT == strAction)
    {
        DM_EnterDirectoryUI(DIRECTORY_TYPE_GENBAND, kszAllContactId);
    }
    else if (POOL_ACTION_GAB_CONTACT == strAction)
    {
        DM_EnterDirectoryUI(DIRECTORY_TYPE_GENBAND, kszGabBookGroupId);
    }
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    else if (POOL_ACTION_MTSW_CONTACT == strAction)
    {
        DM_EnterDirectoryUI(DIRECTORY_TYPE_METASWICTH, knInvalidRId);
    }
#endif
#if IF_FEATURE_METASWITCH_CALLLOG
    else if (POOL_ACTION_MTSW_CALLLOG == strAction)
    {
        DM_EnterDirectoryUI(CALL_LOG_TYPE_MTSW);
    }
#endif
    else if (POOL_ACTION_MOBILE_CONTANT == strAction)
    {
        DM_EnterDirectoryUI(DIRECTORY_TYPE_BLUETOOTH, kszBluethoothGroupId);
    }
    // http://10.3.5.199/Bug.php?BugID=63134 不需要进入network favorite
    //else if(POOL_ACTION_NETWORK_FAVORITE == strAction)
    //{
    //  DM_EnterDirectoryUI(DIRECTORY_TYPE_BTUC, FAVOURITE_ID);
    //}
    else
    {
        // none
        DM_EnterDirectoryUI(DIRECTORY_TYPE_NONE);
    }
}

LRESULT CDlgTalkPool::OnTalkPoolMsg(msgObject & objMessage)
{
    TALKUI_INFO("CDlgTalkPool::OnTalkPoolMsg Msg Id[%d].", objMessage.message);

    switch (objMessage.message)
    {
    case REMOTE_PHONEBOOK_RELOAD:
    case LDAP_STATUS_CHANGED:
    case BROADSOFT_PHONEBOOK_RELOAD:
    case BLUETOOTH_CONTACT_RELOAD:
    case BROADSOFT_BUDDYLIST_GROUP_CHANGE:
        {
            CDlgTalkPool * pDlgTalkPoolUI = (CDlgTalkPool *)UIManager_GetPrivateDlg(DLG_TalkPool);

            if (pDlgTalkPoolUI != NULL)
            {
                pDlgTalkPoolUI->OnContactItemChange();
            }
        }
        break;
    default:
        break;
    }

    return FALSE;
}

//取得实例
CBaseDialog * CDlgTalkPool::CreateInstance()
{
    return new CDlgTalkPool();
}

CDlgTalkPool::CDlgTalkPool()
    : CDlgTalkMenuBase(eWT_Unknown, DLG_TalkPool)
    , m_iMenuDataCount(0)
    , m_bIsEnterFromIdle(false)
{

}

CDlgTalkPool::~CDlgTalkPool()
{

}

// 初始化
void CDlgTalkPool::Init()
{
    RegisterMsg();

    //设置显示title
    if (NULL != m_pTitle)
    {
        m_pTitle->SetText(_UILOGIC_LANG_TRAN(TRID_FAVOURITE_DIRECTORIES));
    }

    if (NULL != m_pPageInfo)
    {
        m_pPageInfo->hide(true);
    }


    if (NULL != m_pListView)
    {
        m_pListView->SetAdapter(&m_objTalkPoolAdapter);
        m_pListView->ClearContent();
    }

    m_objTalkPoolAdapter.Init();

    m_vecAction.clear();
}

// 释放数据，将窗.口清空回复到初始化状态
void CDlgTalkPool::Uninit()
{
    UnRegisterMsg();

    if (NULL != m_pListView)
    {
        m_pListView->ClearContent();
        m_pListView->SetAdapter(NULL);
    }

    m_objTalkPoolAdapter.Uninit();
}

void CDlgTalkPool::RegisterMsg()
{
    etl_RegisterMsgHandle(REMOTE_PHONEBOOK_RELOAD, REMOTE_PHONEBOOK_RELOAD,
                          &CDlgTalkPool::OnTalkPoolMsg);
    etl_RegisterMsgHandle(LDAP_STATUS_CHANGED, LDAP_STATUS_CHANGED, &CDlgTalkPool::OnTalkPoolMsg);
    etl_RegisterMsgHandle(BROADSOFT_PHONEBOOK_RELOAD, BROADSOFT_PHONEBOOK_RELOAD,
                          &CDlgTalkPool::OnTalkPoolMsg);
    etl_RegisterMsgHandle(BLUETOOTH_CONTACT_RELOAD, BLUETOOTH_CONTACT_RELOAD,
                          &CDlgTalkPool::OnTalkPoolMsg);
    etl_RegisterMsgHandle(BROADSOFT_BUDDYLIST_GROUP_CHANGE, BROADSOFT_BUDDYLIST_GROUP_CHANGE,
                          &CDlgTalkPool::OnTalkPoolMsg);
}

void CDlgTalkPool::UnRegisterMsg()
{
    etl_UnregisterMsgHandle(REMOTE_PHONEBOOK_RELOAD, REMOTE_PHONEBOOK_RELOAD,
                            &CDlgTalkPool::OnTalkPoolMsg);
    etl_UnregisterMsgHandle(LDAP_STATUS_CHANGED, LDAP_STATUS_CHANGED, &CDlgTalkPool::OnTalkPoolMsg);
    etl_UnregisterMsgHandle(BROADSOFT_PHONEBOOK_RELOAD, BROADSOFT_PHONEBOOK_RELOAD,
                            &CDlgTalkPool::OnTalkPoolMsg);
    etl_UnregisterMsgHandle(BLUETOOTH_CONTACT_RELOAD, BLUETOOTH_CONTACT_RELOAD,
                            &CDlgTalkPool::OnTalkPoolMsg);
    etl_UnregisterMsgHandle(BROADSOFT_BUDDYLIST_GROUP_CHANGE, BROADSOFT_BUDDYLIST_GROUP_CHANGE,
                            &CDlgTalkPool::OnTalkPoolMsg);
}

void CDlgTalkPool::OnContactItemChange()
{
    ReloadFrameListData();

    UpdateWnd();
}

yl::string CDlgTalkPool::GetNameByAction(const yl::string & strAction)
{
    if (strAction == POOL_ACTION_LOCAL_CONTACT)
    {
        return TRID_LOCAL_DIRECTORY;
    }
    else if (strAction == POOL_ACTION_LOCAL_CALLLOG)
    {
        return TRID_HISTORY;
    }
    else if (strAction == POOL_ACTION_REMOTE_CONTACT)
    {
        return TRID_REMOTE_PHONEBOOK;
    }
    else if (strAction == POOL_ACTION_LDAP_CONTACT)
    {
        return TRID_LDAP;
    }
    else if (strAction == POOL_ACTION_BST_CONTACT)
    {
        return TRID_NETWORK_DIRECTORIES;
    }
    else if (strAction == POOL_ACTION_NETWORK_CONTACT)
    {
        return TRID_NETWORK_DIRECTORIES;
    }
    else if (strAction == POOL_ACTION_BST_CALLLOG)
    {
        return TRID_NETWORK_CALLLOG;
    }
    else if (POOL_ACTION_UC_BUDDIES == strAction)
    {
        return TRID_BUDDIES;
    }
    else if (POOL_ACTION_NETWORK_FAVORITE == strAction)
    {
        return TRID_NETWORK_FAVOURITES;
    }
#if IF_BUG_PLCMDIR
#ifndef _WIN32
#warning
#endif
//      else if (strAction == POOL_ACTION_PLCM_CONTACT) return TRID_PHONEBOOK;
#endif
#ifdef IF_FEATURE_PSTN
    else if (strAction == POOL_ACTION_FAVORITE_DIRECTORY)
    {
        return TRID_FAVORITE_DIRECTORY;
    }
#endif
#if IF_FEATURE_GBBOOK
    else if (strAction == POOL_ACTION_GENBAND_CONTACT)
    {
        return TRID_GB_PABBOOK;
    }
    else if (strAction == POOL_ACTION_GAB_CONTACT)
    {
        return TRID_GB_GABBOOK;
    }
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    else if (strAction == POOL_ACTION_MTSW_CONTACT)
    {
        return TRID_NETWORK_CONTACTS;
    }
#endif
#if IF_FEATURE_METASWITCH_CALLLOG
    else if (strAction == POOL_ACTION_MTSW_CALLLOG)
    {
        return TRID_NETWORK_CALLLOG;
    }
#endif
    else if (POOL_ACTION_MOBILE_CONTANT == strAction)
    {
        return TRID_MOBILE_CONTACTS;
    }
    else
    {
        return "";
    }
}

void CDlgTalkPool::ReloadFrameListData()
{
    //重置成没有数据
    m_iMenuDataCount = 0;

    //清除之前的数据
    if (NULL != m_pListView)
    {
        m_pListView->ClearContent();
    }

    //清除保存的数据
    m_vecAction.clear();

    //这个用于给delegate 设置数据
    talkui_vector<yl::string> vecDisplayName;

    DirShortcutListData objlistData;
    objlistData.m_nIndexFrom = 0;
    objlistData.m_nCountToGet = 100;

    //获取显示的数据
    if (!Dir_GetShortcutList(&objlistData))
    {
        return;
    }

#if !IF_BUG_39755
    // http://10.2.1.199/Bug.php?BugID=92495
    if (ExistLocalDirectory(objlistData.m_listData) && Dir_IsDirConfiged(BLUETOOTH_DIR))
    {
        DirShortcutData objBlueDir;
        objBlueDir.m_strIdName = POOL_ACTION_MOBILE_CONTANT;
        objBlueDir.m_strDisplayName = TRID_MOBILE_CONTACTS;

        YLList<DirShortcutData>::iterator itbegin = objlistData.m_listData.begin();
        objlistData.m_listData.push_back(objBlueDir);
    }
#endif

    int iIndex = 0;

    YLList<DirShortcutData>::iterator it = objlistData.m_listData.begin();
    for (; it != objlistData.m_listData.end(); ++it)
    {
        iIndex++;
        char szIndex[8] = { 0 };
        sprintf(szIndex, "%d.", iIndex);
        yl::string strIndex = yl::string(szIndex);

        //先设置为显示名称
        yl::string strDisplayName = (*it).m_strDisplayName;

        //如果获取到的显示名称 与 文件中的显示名称相同 那么就需要翻译   如果 不相同那么说明客户已经改了 那么就不需要翻译
        if (GetNameByAction((*it).m_strIdName) == strDisplayName)
        {
            strDisplayName = _UILOGIC_LANG_TRAN(strDisplayName.c_str());
        }

        //加入到Action 队列中
        m_vecAction.push_back((*it).m_strIdName);

        //加上序号
        strDisplayName = strIndex + strDisplayName;
        //加入到 显示名称 队列中
        vecDisplayName.push_back(strDisplayName);
    }

    //获取读取到的菜单列表个数
    m_iMenuDataCount = m_vecAction.size();
    //如果没有读取到数据  要设置一个None显示
    if (0 >= m_iMenuDataCount)
    {
        //加入到Action 队列中
        m_vecAction.push_back("");
        //加入到 显示名称 队列中
        yl::string strNone = "1. ";
        strNone += _UILOGIC_LANG_TRAN(TRID_NONE);
        vecDisplayName.push_back(strNone);
    }

    //给控件设置数据
    m_objTalkPoolAdapter.SetDirInfo(vecDisplayName);

    UpdatePageInfo(m_iMenuDataCount);

    CArraySoftKeyBarData arraySoftkey;
    GetSoftkey(arraySoftkey);
    SetSoftKey(arraySoftkey);
}

//设置数据
void CDlgTalkPool::SetData(void * pData)
{
    UIManager_ResetReturnToIdle();
}

//获取Softkey数据
void CDlgTalkPool::GetSoftkey(CArraySoftKeyBarData & objSoftkeyData)
{
    CSoftKeyBarData softkeyBarData0;

    GetSoftkeyBarDataFromSoftKeyType(softkeyBarData0, ST_BACK);
    objSoftkeyData.push_back(softkeyBarData0);


    CSoftKeyBarData softkeyBarData1;
    objSoftkeyData.push_back(softkeyBarData1);

    CSoftKeyBarData softkeyBarData2;
    objSoftkeyData.push_back(softkeyBarData2);


    CSoftKeyBarData softkeyBarData3;

    //先判断是否有获取到数据
    if (0 >= m_iMenuDataCount)
    {
        objSoftkeyData.push_back(softkeyBarData3);
    }
    else
    {
        GetSoftkeyBarDataFromSoftKeyType(softkeyBarData3, ST_ENTER);
        objSoftkeyData.push_back(softkeyBarData3);
    }

}

bool CDlgTalkPool::onKeyPressedEvent(int key)
{
    if (PHONE_KEY_CANCEL == key)
    {
        OnSoftkeyPress(ST_BACK, false);
        return true;
    }
    else if (PHONE_KEY_OK == key)
    {
        OnSoftkeyPress(ST_ENTER, false);
        return true;
    }
//      else if (PHONE_KEY_UP == key)
//      {
//          m_pListView->OnNotifyScrollPrevious();
//
//          return true;
//      }
//      else if (PHONE_KEY_DOWN == key)
//      {
//          m_pListView->OnNotifyScrollNext();
//          return true;
//      }
    else if (PHONE_KEY_1 <= key && key <= PHONE_KEY_9)
    {
        int iSelectIndex = key - PHONE_KEY_1;

        OpenPageByIndex(iSelectIndex);

        return true;
    }

    return CDlgTalkMenuBase::onKeyPressedEvent(key);
}

//处理softkey 继承自 talkbaseui
bool CDlgTalkPool::OnSoftkeyPress(SoftKey_TYPE eSoftType, bool bLongPress)
{
    if (bLongPress)
    {
        return false;
    }

    TALKUI_INFO("CDlgTalkPool-OnSoftkeyAction(%d)", eSoftType);

    switch (eSoftType)
    {
    case ST_ENTER:
        {
            //先判断是否有获取到数据 有获取到数据才需要继续执行
            if (0 < m_iMenuDataCount)
            {
                if (NULL != m_pListView)
                {
                    //所选择的序号
                    //这个是获取 实际的data 序号   而GetFocusItemIndex 是获取的是当页的排序序号
                    int iSelectItemDataIndex = m_pListView->GetFocusItemDataIndex();

                    //打开相应的界面
                    OpenPageByIndex(iSelectItemDataIndex);
                }
            }

            return true;
        }
        break;
    case ST_BACK:
        {
            //Cancel退出事件
            talkui_ExitPool();

            return true;
        }
        break;
    default:
        {
            return false;
        }
        break;
    }

    return false;
}

void CDlgTalkPool::OpenPageByIndex(int iSelectIndex)
{
    //选择的 Action
    yl::string strSelectAction = "";

    //获取账号ID
    if (iSelectIndex >= 0 && iSelectIndex < m_vecAction.size())
    {
        strSelectAction = m_vecAction[iSelectIndex];

        TALKUI_INFO("CDlgTalkPool::OpenPageByIndex strAction(%s)",
                    strSelectAction.c_str());

        //从IDLE界面进入Favorites界面时应该返回上一层界面，不能直接回到IDLE界面
        //http://10.2.1.199/Bug.php?BugID=34152
//          if (!m_bIsEnterFromIdle)
//          {
//              // 需要先退出Pool界面再进入某个联系人界面
//              // http://10.2.1.199/Bug.php?BugID=33325
//              // 退出Pool界面
//              talkui_ExitPool();
//          }

        // 打开界面
        talkPool_OpenPage(strSelectAction);
    }
}

void CDlgTalkPool::setIsEnterFromIdle(bool bEnterFromIdle)
{
    m_bIsEnterFromIdle = bEnterFromIdle;
}


}       //namespace talkui


