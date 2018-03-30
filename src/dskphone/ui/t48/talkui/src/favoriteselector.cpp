
#include "favoriteselector.h"
#include "baseui/framelist.h"
#include "settingui/src/settingitembase.h"
#include "setting/include/common.h"
#include "directoryui/include/moddirectoryui.h"
#include "uicommon/messagebox/selectmessagebox.h"
#include "uikernel/languagehelper.h"
#include "presence/include/presence_inc.h"

#include "uicommon/qtcommon/qmisc.h"

namespace
{
#define FAVORITE_ID_LOCAL_CONTACT   "localdirectory"
#define FAVORITE_ID_LOCAL_CALLLOG   "history"
#define FAVORITE_ID_REMOTE_CONTACT  "remotedirectory"
#define FAVORITE_ID_LDAP_CONTACT    "ldap"
#define FAVORITE_ID_BST_CONTACT     "broadsoftdirectory"
#define FAVORITE_ID_NETWORK_CONTACT     "networkdirectory"
#define FAVORITE_ID_BST_CALLLOG     "networkcalllog"
#define FAVORITE_ID_UC_BUDDIES      "uc_buddies"            //uc_buddies
#define FAVORITE_ID_NETWORK_FAVORITE        "network_favorite"      //network_favorite   ucfavorite
#define FAVORITE_ID_PLCM_CONTACT    "plcmdirectory"
#define FAVORITE_ID_GAB_CONTACT     "gabdirectory"
#define FAVORITE_ID_PAB_CONTACT     "pabdirectory"
#if IF_FEATURE_METASWITCH_CALLLOG
#define FAVORITE_ID_MTSW_CALLLOG    "metaswitchcalllog"
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
#define FAVORITE_ID_MTSW_CONTACT    "metaswitchcontacts"
#endif
#define FAVORITE_ID_BLACKLIST       "blacklist"
#if IF_BUG_39755
#define FAVORITE_ID_BLUE_CONTACT    "mobilecontant"
#endif

}

CFavoriteSelector::CFavoriteSelector()
    : m_nSelIndex(0)
{
}

CFavoriteSelector::~CFavoriteSelector()
{
    m_vecFavorites.clear();
}

//弹出框回调函数
void CFavoriteSelector::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL != pMessageBox)
    {
        if (MessageBox_OK == pMessageBox->GetResult())
        {
            EnterSelectedFavorite();
        }
    }

}

void CFavoriteSelector::Init()
{
    //设置选中的序号是0
    m_nSelIndex = 0;

    //保存的是名称ID
    m_vecFavorites.clear();

    //保存的是 显示名称
    m_vecDisplayName.clear();

    DirShortcutListData listData;

    listData.m_nIndexFrom = 0;
    listData.m_nCountToGet = 100;
    listData.m_nTotalCount = 0;

    if (!Dir_GetShortcutList(&listData))
    {
        return;
    }

    YLList<DirShortcutData>::iterator iter = listData.m_listData.begin();

    for (; iter != listData.m_listData.end(); ++iter)
    {
        DirShortcutData & dirData = *iter;

        m_vecFavorites.push_back(dirData.m_strIdName);

        //保存显示名称
        //先设置为显示名称
        yl::string strDisplayName = dirData.m_strDisplayName;

        //如果获取到的显示名称 与 文件中的显示名称相同 那么就需要翻译   如果 不相同那么说明客户已经改了 那么就不需要翻译
        if (GetFavoriteShowName(dirData.m_strIdName) == strDisplayName)
        {
            strDisplayName = LANG_TRANSLATE(strDisplayName.c_str()).toUtf8().data();
        }

        m_vecDisplayName.push_back(strDisplayName);
    }
}

bool CFavoriteSelector::IsHadFavorites()
{
    return (!m_vecFavorites.empty());
}

bool CFavoriteSelector::IsCanDirectEnter()
{
    return (1 == m_vecFavorites.size());
}

void CFavoriteSelector::DirectEnterFavorite()
{
    if (m_vecFavorites.empty())
    {
        return;
    }

    EnterFavorite(m_vecFavorites[0]);
}

void CFavoriteSelector::EnterSelectedFavorite()
{
    int nSize = m_vecFavorites.size();

    if (m_nSelIndex < 0 || m_nSelIndex >= nSize)
    {
        return;
    }

    EnterFavorite(m_vecFavorites[m_nSelIndex]);
}

void CFavoriteSelector::EnterFavorite(const yl::string & strFavoriteId)
{
    if (FAVORITE_ID_LOCAL_CONTACT == strFavoriteId)
    {
        DM_EnterDirectoryUI(DIRECTORY_TYPE_LOCAL, kszAllContactId);
    }
    else if (FAVORITE_ID_REMOTE_CONTACT == strFavoriteId)
    {
        DM_EnterDirectoryUI(DIRECTORY_TYPE_REMOTE, kszRootId);
    }
    else if (FAVORITE_ID_LDAP_CONTACT == strFavoriteId)
    {
        DM_EnterDirectoryUI(DIRECTORY_TYPE_LDAP, knInvalidRId);
    }
    else if (FAVORITE_ID_BST_CONTACT == strFavoriteId)
    {
        DM_EnterDirectoryUI(DIRECTORY_TYPE_BROADSOFT, kszAllContactId);
    }
    else if (FAVORITE_ID_NETWORK_CONTACT ==
             strFavoriteId)      //这个 用于 M7配置模板Bug修改 IF_M7_MODIFY  与POOL_ACTION_BST_CONTACT 是同一种类型  只是适用版本不同
    {
        DM_EnterDirectoryUI(DIRECTORY_TYPE_BROADSOFT, kszAllContactId);
    }
    else if (FAVORITE_ID_BST_CALLLOG == strFavoriteId)
    {
        DM_EnterDirectoryUI(CALL_LOG_TYPE_BROADSOFT, kszAllContactId);
    }
    else if (FAVORITE_ID_LOCAL_CALLLOG == strFavoriteId)
    {
        DM_EnterDirectoryUI(CALL_LOG_TYPE_LOCAL, kszAllContactId);
    }
    else if (FAVORITE_ID_UC_BUDDIES == strFavoriteId)
    {
        DM_EnterDirectoryUI(DIRECTORY_TYPE_BTUC, ALL_BUDDIES_ID);
    }
#if IF_BUG_PLCMDIR
    else if (FAVORITE_ID_PLCM_CONTACT == strFavoriteId)
    {
        DM_EnterDirectoryUI(DIRECTORY_TYPE_PLCM, kszPlcmDirGroupId);
    }
#endif
#if IF_FEATURE_GBBOOK
    else if (FAVORITE_ID_GAB_CONTACT == strFavoriteId)
    {
        DM_EnterDirectoryUI(DIRECTORY_TYPE_GENBAND, kszGabBookGroupId);
    }
    else if (FAVORITE_ID_PAB_CONTACT == strFavoriteId)
    {
        DM_EnterDirectoryUI(DIRECTORY_TYPE_GENBAND, kszAllContactId);
    }
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    else if (FAVORITE_ID_MTSW_CONTACT == strFavoriteId)
    {
        DM_EnterDirectoryUI(DIRECTORY_TYPE_METASWICTH, kszAllContactId);
    }
#endif
#if IF_FEATURE_METASWITCH_CALLLOG
    else if (FAVORITE_ID_MTSW_CALLLOG == strFavoriteId)
    {
        DM_EnterDirectoryUI(CALL_LOG_TYPE_MTSW, kszAllContactId);
    }
#endif
    else if (FAVORITE_ID_BLACKLIST == strFavoriteId)
    {
        DM_EnterDirectoryUI(DIRECTORY_TYPE_LOCAL, kszBlackListGroupId);
    }
#if IF_BUG_39755
    else if (FAVORITE_ID_BLUE_CONTACT == strFavoriteId)
    {
        DM_EnterDirectoryUI(DIRECTORY_TYPE_BLUETOOTH, kszAllContactId);
    }
#endif
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

yl::string CFavoriteSelector::GetFavoriteShowName(const yl::string & strFavoriteId)
{
    if (FAVORITE_ID_LOCAL_CONTACT == strFavoriteId)
    {
        return yl::string(TRID_LOCAL_DIRECTORY);
    }
    else if (FAVORITE_ID_LOCAL_CALLLOG == strFavoriteId)
    {
        return yl::string(TRID_HISTORY);
    }
    else if (FAVORITE_ID_REMOTE_CONTACT == strFavoriteId)
    {
        return yl::string(TRID_REMOTE_PHONEBOOK);
    }
    else if (FAVORITE_ID_LDAP_CONTACT == strFavoriteId)
    {
        return yl::string(TRID_LDAP);
    }
    //http://10.2.1.199/Bug.php?BugID=118616
    // 翻译需要改成和factory里面favorite.xml一样，不然不会翻译,修改favorite文件升级上来可能存在文件不统一，修改代码和favorite里面名字一样
    else if (FAVORITE_ID_BST_CONTACT == strFavoriteId)
    {
        return yl::string(TRID_FAVORITE_BSFT_DIR);
    }
    else if (FAVORITE_ID_NETWORK_CONTACT == strFavoriteId)
    {
        return yl::string(TRID_FAVORITE_BSFT_DIR);
    }
    else if (FAVORITE_ID_BST_CALLLOG == strFavoriteId)
    {
        return yl::string(TRID_NETWORK_CALLLOG);
    }
    else if (FAVORITE_ID_UC_BUDDIES == strFavoriteId)
    {
        return yl::string(TRID_BUDDIES);
    }
    else if (FAVORITE_ID_NETWORK_FAVORITE == strFavoriteId)
    {
        return yl::string(TRID_NETWORK_FAVOURITES);
    }
#if IF_BUG_PLCMDIR
    else if (FAVORITE_ID_PLCM_CONTACT == strFavoriteId)
    {
        return yl::string(TRID_FAVORITE_PLCM_DIR);
    }
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    else if (FAVORITE_ID_MTSW_CONTACT == strFavoriteId)
    {
        return yl::string(TRID_NETWORK_CONTACTS);
    }
#endif
#if IF_FEATURE_METASWITCH_CALLLOG
    else if (FAVORITE_ID_MTSW_CALLLOG == strFavoriteId)
    {
        return yl::string(TRID_NETWORK_CALLLIST);
    }
#endif
#if IF_FEATURE_GBBOOK
    else if (FAVORITE_ID_GAB_CONTACT == strFavoriteId)
    {
        return yl::string(TRID_GB_PABBOOK);
    }
    else if (FAVORITE_ID_PAB_CONTACT == strFavoriteId)
    {
        return yl::string(TRID_GB_GABBOOK);
    }
#endif
    else if (FAVORITE_ID_BLACKLIST == strFavoriteId)
    {
        return yl::string(TRID_BLACKLIST);
    }
#if IF_BUG_39755
    else if (FAVORITE_ID_BLUE_CONTACT == strFavoriteId)
    {
        return yl::string(TRID_MOBILE_CONTACTS);
    }
#endif
    else
    {
        return yl::string("");
    }

}

void CFavoriteSelector::OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount)
{
    CFrameList * pList = (CFrameList *)pFrameList;

    if (NULL == pList || pList != m_pFrameList)
    {
        return;
    }

    if (!m_pFrameList->IsCanReuseItem(CSettingRadioItem::GetSettingRadioItemType()))
    {
        m_pFrameList->ClearContent();
    }

    int nItemIndex = 0;
    int nSize = m_vecFavorites.size();
    int nMaxPageItemCount = m_pFrameList->GetMaxPageItemCount();

    m_pFrameList->SetTotalDataCount(nSize);

    for (int i = 0; nItemIndex < nMaxPageItemCount && i < nSize; ++i)
    {
        if (i < nDataIndex)
        {
            continue;
        }

        //  yl::string& strFavoriteId = m_vecFavorites[i];

        bool bAdd = false;
        CSettingRadioItem * pRadioItem = (CSettingRadioItem *)m_pFrameList->GetItemByIndex(nItemIndex);

        if (NULL == pRadioItem)
        {
            bAdd = true;
            pRadioItem = new CSettingRadioItem;

            if (NULL == pRadioItem)
            {
                continue;
            }

            pRadioItem->setGeometry(0, 0, 0, SELECT_POP_BOX_POS_ITEM_HEIGHT);
            pRadioItem->SetChangeStyleOnPress(true);
            pRadioItem->SetChangeStyleOnSelect(false);
        }

        int nItemDataIndex = i;

        pRadioItem->ClearContent();
        pRadioItem->CListItem::SetAction(this);

        //这边的名称在init 的时候已经做过处理了  用这个
        pRadioItem->SetLabText(toQString(m_vecDisplayName[i]));

        //pRadioItem->SetLabText(LANG_TRANSLATE(GetFavoriteShowName(strFavoriteId).c_str()));
        pRadioItem->SetDataIndex(nItemDataIndex);

        if (nItemDataIndex == m_nSelIndex)
        {
            pRadioItem->SetFocus(true);
        }
        else
        {
            pRadioItem->SetFocus(false);
        }

        if (bAdd && !m_pFrameList->AddItem(pRadioItem))
        {
            pRadioItem->Release();
        }

        ++nItemIndex;
    }

    int nItemAmount = m_pFrameList->GetTotalItemCount();

    for (int i = nItemIndex; i < nItemAmount; ++i)
    {
        m_pFrameList->DelItemByIndex(nItemIndex);
    }

    m_pFrameList->update();
}

void CFavoriteSelector::OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction)
{
    CFrameList * pList = (CFrameList *)pFrameList;

    if (NULL == pList || pList != m_pFrameList || NULL == pItem || LIST_ITEM_ACTION_SELECT != nAction)
    {
        return;
    }

    m_nSelIndex = pItem->GetDataIndex();
}
