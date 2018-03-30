#include "settinguidialogmanager.h"
#include "delegatefactory.h"
#include "uicallbackfun.h"
#include "settinguimanager.h"
#include "basesettingdialog.h"
#include "basesettinguidelegate.h"
#include "settingdialogfactory.h"
#include "setuidatadefine.h"
#include "settinguilogic/include/settingdefine.h"
#include "settingui/include/pageiddefine.h"

#include "xwindow/xWindowManager.h"
#include "uimanager/moduimanager.h"
#include "uimanager/uimanager_common.h"
#include "commonapi/stringhelperapi.h"
#include "dsklog/log.h"

CSettingUIDialogManager * CSettingUIDialogManager::s_SettingUIDialogMgr = NULL;

namespace
{
const int g_iDefaultFallBackingMark = 17;
}

CSettingUIDialogManager * CSettingUIDialogManager::GetInstance()
{
    if (NULL == s_SettingUIDialogMgr)
    {
        s_SettingUIDialogMgr = new CSettingUIDialogManager();
    }

    return s_SettingUIDialogMgr;
}

void CSettingUIDialogManager::ReleaseInstance()
{
    if (NULL != s_SettingUIDialogMgr)
    {
        delete s_SettingUIDialogMgr;
        s_SettingUIDialogMgr = NULL;
    }
}

CSettingUIDialogManager::CSettingUIDialogManager()
{
}

CSettingUIDialogManager::~CSettingUIDialogManager()
{
}

void CSettingUIDialogManager::Init()
{
}

void CSettingUIDialogManager::UnInit()
{
}

bool CSettingUIDialogManager::OpenPage(const yl::string & strPageID, bool bRefresh /*= true*/)
{
    PageOpenData opPageData(strPageID);
    return OpenPage(opPageData, bRefresh);
}

bool CSettingUIDialogManager::OpenPage(const PageOpenData & openPageData, bool bRefresh /*= true*/)
{
    yl::string strPageID = openPageData.strPageAction;

    // 获取Delegate
    CSettingUIDelegateBase * pSetUIDelegate = g_pSettingUIManager->GetPageDelegate(strPageID);
#if IF_SETTING_BACK_DEBUG
    printf("------ [CSettingUIDialogManager::OpenPage] pageId[%s], delegate[%p] =======\n"
           , strPageID.c_str(), pSetUIDelegate);
#endif
    if (NULL != pSetUIDelegate)
    {
        SETTINGUI_INFO("[CSettingUIDialogManager::OpenPage] page[%s] has exist, do not need create again!!!",
                       strPageID.c_str());
        return false;
    }
    else
    {
        pSetUIDelegate = g_DelegateFactory.GetDelegate(strPageID);
        if (NULL == pSetUIDelegate
                && NULL == (pSetUIDelegate = g_DelegateFactory.GetDefaultDelegate()))
        {
            return false;
        }
        pSetUIDelegate->SetPageID(strPageID);
        // 添加到Delegate栈中
        g_pSettingUIManager->AddDelegate(pSetUIDelegate);
    }

    // 创建界面
    CSettingDialogBase * pDlg = GetDialog(strPageID);
    if (NULL == pDlg)
    {
        return false;
    }
    pDlg->UnBindDelegate();
    pDlg->DoInitAfterCreateDialog(strPageID);
    // 界面绑定Delegate
    pDlg->BindDelegate(pSetUIDelegate);

    // 附加操作
    pDlg->InitExtraData(openPageData.pExtraData);

#if IF_SETTING_FRAME_DEBUG
    SETTINGUI_INFO("[CSettingUIDialogManager::OpenPage] dlg[%p], delegate[%p], pageId[%s]"
                   , pDlg, pSetUIDelegate, strPageID.c_str());
#endif

    if (bRefresh)
    {
        pDlg->RefreshUI(true);
    }
    else
    {
        pDlg->LoadPageData(true);
    }

    return true;
}

bool CSettingUIDialogManager::OpenPageOrderByPageID(const yl::string & strPageIDList)
{
    // strPageIDList格式："id1, focusItemId1; id2, focusItemId2; id3, focusItemId3"
    // 去掉首尾空格、';'号
    yl::string strList = commonAPI_TrimString(strPageIDList, " ");
    strList = commonAPI_TrimString(strList, ";");
    LIST_STRING listPageIDs;
    LIST_STRING listPageItemId;
    if (commonAPI_splitStringBySeparator(strList, ';', listPageIDs)
            && 0 < listPageIDs.size())
    {
        LIST_STRING::ListIterator iter = listPageIDs.begin();
        yl::string strFirstPageID;
        yl::string strFirstItemID;
        yl::string strFirst;
        int nCount = 0;
        CSettingDialogBase * pPreDlg = NULL;
        yl::string strPreDlgFocusItemId = "";
        do
        {
            strFirst = (iter != listPageIDs.end()) ? (*iter) : "";
            ++iter;

            listPageItemId.clear();
            if (!commonAPI_splitStringBySeparator(strFirst, ',', listPageItemId))
            {
                break;
            }

            strFirstPageID = GetValueFromListByIndex(listPageItemId, 1);
            strFirstPageID = commonAPI_TrimString(strFirstPageID, " ");
            strFirstItemID = GetValueFromListByIndex(listPageItemId, 2);
            strFirstItemID = commonAPI_TrimString(strFirstItemID, " ");
#if IF_SETTING_INTERFACE_DEBUG
            SETTINGUI_INFO("-=-=-=-=-=-==-=-=-=-=$$$ [CSettingUIDialogManager::OpenPageOrderByPageID] dlg[%s], focusItem[%s] ==="
                           , strFirstPageID.c_str(), strFirstItemID.c_str());
#endif

            if (strFirstPageID.empty())
            {
                break;
            }

            // 查找/创建First界面
            CSettingDialogBase * pDlg = GetTopActiveDialog(strFirstPageID);
            if (NULL != pDlg && 0 == pDlg->GetPageID().compare(strFirstPageID))
            {
#if IF_SETTING_INTERFACE_DEBUG
                SETTINGUI_INFO("-=-=-=-=-=-==-=-=-=-=$$$ [CSettingUIDialogManager::OpenPageOrderByPageID] find dialog[%s] and set active ==="
                               , strFirstPageID.c_str());
#endif
                pDlg->DialogSetActive();
            }
            else
            {
                pDlg = NULL;
                if (OpenPage(strFirstPageID, false))
                {
                    pDlg = GetTopActiveDialog(strFirstPageID);
                    if (NULL != pDlg && 0 != pDlg->GetPageID().compare(strFirstPageID))
                    {
                        pDlg = NULL;
                    }

                    if (NULL != pDlg)
                    {
#if IF_SETTING_INTERFACE_DEBUG
                        SETTINGUI_INFO("-=-=-=-=-=-==-=-=-=-=$$$ [CSettingUIDialogManager::OpenPageOrderByPageID] dialog[%s] create ==="
                                       , strFirstPageID.c_str());
#endif
                        pDlg->DialogSetActive();
                    }
                }
            }
            // 设置Second项为First界面焦点项
            if (NULL == pDlg)
            {
#if IF_SETTING_INTERFACE_DEBUG
                SETTINGUI_INFO("-=-=-=-=-=-==-=-=-=-=$$$ [CSettingUIDialogManager::OpenPageOrderByPageID] dialog[%s] create error ==="
                               , strFirstPageID.c_str());
#endif
                break;
            }
            if (NULL != pPreDlg && pPreDlg != pDlg)
            {
                pPreDlg->UpdateFocus(strPreDlgFocusItemId, false);
            }
            pPreDlg = pDlg;
            strPreDlgFocusItemId = strFirstItemID;
            pDlg->UpdateFocus(strFirstItemID, true);

            ++nCount;
        }
        while (iter != listPageIDs.end());

        if (nCount == listPageIDs.size())
        {
            return true;
        }
#if IF_SETTING_INTERFACE_DEBUG
        SETTINGUI_INFO("-=-=-=-=-=-==-=-=-=-=$$$ [CSettingUIDialogManager::OpenPageOrderByPageID] need open [%d] page, just open [%d] page ==="
                       , listPageIDs.size(), nCount);
#endif
    }

    return false;
}

void CSettingUIDialogManager::FallBackToThePreviousInterface(CSettingDialogBase * pCurDlg)
{
    if (NULL != pCurDlg)
    {
        if (g_iDefaultFallBackingMark == m_mapDlgFallBacking[pCurDlg])
        {
            return;
        }
        m_mapDlgFallBacking[pCurDlg] = g_iDefaultFallBackingMark;

#if IF_SETTING_BACK_DEBUG
        printf("------ [CSettingUIDialogManager::FallBackToThePreviousInterface] pageId[%s] =======\n"
               , pCurDlg->GetPageID().c_str());
#endif

        //先删除当前Delegate,在判断是否需要退出界面
        bool bOneDelegate = pCurDlg->IsOnlyBindOneDelegate();
        CSettingUIDelegateBase * pCurDelegate = pCurDlg->GetBindDelegate();
        pCurDlg->DoUninitBeforeDeleteDialog();
        pCurDlg->UnBindDelegate();
        g_pSettingUIManager->DelDelegate(pCurDelegate);

        if (bOneDelegate)
        {
            ReleasePrivateDialog(pCurDlg);
        }
        else
        {
            CSettingUIDelegateBasePtr pPreviousDelegate = g_pSettingUIManager->GetTopDelegate();
            if (NULL != pPreviousDelegate)
            {
                pCurDlg->DoInitAfterCreateDialog(pPreviousDelegate->GetPageID());
                pCurDlg->BindDelegate(pPreviousDelegate);
                pCurDlg->RefreshUI();
            }
        }

        m_mapDlgFallBacking[pCurDlg] = 0;
    }
}

void CSettingUIDialogManager::DropOutCurrentPage(CSettingDialogBase * pCurDlg,
        bool bDelDlg /*= true*/)
{
    if (NULL == pCurDlg)
    {
        return;
    }

#if IF_SETTING_BACK_DEBUG
    SETTINGUI_INFO("CSettingUIDialogManager::DropOutCurrentPage] pageId[%s]",
                   pCurDlg->GetPageID().c_str());
#endif

    CSettingUIDelegateBasePtr pDelegate = pCurDlg->GetBindDelegate();

    pCurDlg->DoUninitBeforeDeleteDialog();
    pCurDlg->UnBindDelegate();

    g_pSettingUIManager->DelDelegateRange(pDelegate, pCurDlg->GetFirstBindDelegate(), true);

    pCurDlg->ClearFirstBindDelegate();
}

void CSettingUIDialogManager::ReleasePageResource(CSettingDialogBase * pDlg)
{
    if (NULL != pDlg)
    {
        CSettingUIDelegateBasePtr pDelegate = pDlg->GetBindDelegate();
        pDlg->DoUninitBeforeDeleteDialog();
        pDlg->UnBindDelegate();
        g_pSettingUIManager->DelDelegate(pDelegate);
        pDlg->ClearFirstBindDelegate();
    }
}

CSettingDialogBase * CSettingUIDialogManager::GetDialog(const yl::string & strPageID)
{
#if IF_SETTING_FRAME_DEBUG
    SETTINGUI_INFO("--- [CSettingUIDialogManager::GetDialog] pageId[%s]", strPageID.c_str());
#endif

    CSettingDialogBase * pDlg = g_SettingDialogFactory.GetSettingDialog(strPageID);
    if (NULL == pDlg)
    {
        pDlg = (CSettingDialogBase *)UIManager_GetPrivateDlg(DLG_SETTINGUI);
        if (NULL == pDlg || pDlg != UIManager_GetTopWindow())
        {
            pDlg = g_SettingDialogFactory.GetDefaultSettingDialog();
        }
    }

    return pDlg;
}

////////////////////////////////////// 内部调用函数 st ///////////////////////////
// 获取顶层活动窗口
CSettingDialogBase * CSettingUIDialogManager::GetTopActiveDialog(const yl::string &
        strPageId /*= ""*/)
{
    CBaseDialog * pBaseDlg = UIManager_GetTopWindow();

    if (eWT_PageMenu == pBaseDlg->GetWindowType())
    {
        return static_cast<CSettingDialogBase *>(pBaseDlg);
    }

    return NULL;
}

void CSettingUIDialogManager::ReleasePrivateDialog(CSettingDialogBase * pDlg)
{
    if (NULL != pDlg)
    {
        RemoveFallBackingMark(pDlg);
        CBaseDialog * pBaseDlg = static_cast<CBaseDialog *>(pDlg);
        if (NULL != pBaseDlg)
        {
            UIManager_PutPrivateDlg(pBaseDlg);
        }
    }
}


void CSettingUIDialogManager::RemoveFallBackingMark(CSettingDialogBase * pDlg)
{
    if (NULL != pDlg)
    {
        MAP_DELEGATE_FALLBACKING::iterator it = m_mapDlgFallBacking.find(pDlg);
        if (it != m_mapDlgFallBacking.end())
        {
            m_mapDlgFallBacking.erase(it);
#if IF_SETTING_BACK_DEBUG
            SETTINGUI_INFO("############## [CSettingUIDialogManager::RemoveFallBackingMark] remove fall backing mark dlg[%p], after size[%d] ========"
                           , pDlg, m_mapDlgFallBacking.size());
#endif
        }
    }
}


yl::string CSettingUIDialogManager::GetValueFromListByIndex(LIST_STRING& listValue,
        int nIndex)
{
    LIST_STRING::ListIterator iter = listValue.begin();
    for (int nCnt = 1; iter != listValue.end(); ++iter, ++nCnt)
    {
        if (nCnt == nIndex)
        {
            return (*iter);
        }
    }

    return "";
}

void CSettingUIDialogManager::DropSettingUI(const yl::string & strPageId /*= ""*/)
{
    CSettingDialogBase * pDlg = GetTopActiveDialog(strPageId);
    if (NULL == pDlg)
    {
        return;
    }
    DropOutCurrentPage(pDlg);
    ReleasePrivateDialog(pDlg);
}

////////////////////////////////////// 内部调用函数 en ///////////////////////////

