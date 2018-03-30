#include "dlgsettingui.h"
#include "basesettinguidelegate.h"
// manager
#include "settinguimanager.h"
#include "settinguidialogmanager.h"
#include "settinguiwizardmanager.h"

#include "settinguilogic/src/settinguibaseitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settinguilogic/include/settingdefine.h"
#include "settingui/include/pageiddefine.h"
#include "settingactiontypemgr.h"
#include "setuidlgbasecache.h"
#include "setuicachefactory.h"
#include "settingadapterfactory.h"
#include "settinguiadapter.h"
#include "uicallbackfun.h"
#include "setuidatadefine.h"

#include "baseui/include/titlebase.h"
#include "baseui/include/comboboxlistitem.h"
#include "baseui/include/editlistitem.h"
#include "baseui/include/menulistitem.h"
#include "languagehelper/modlangtranslate.h"
#include "translateiddefine.h"
#include "dsklog/log.h"
#include "keymap.h"
#include "messagebox/modmessagebox.h"
#include "uimanager/moduimanager.h"
#include "common/common_data_define.h"
#include "xwindow-kernel/include/xsignal.h"
#include "xwindow/xWindowManager.h"
#include "xwindow/xSoftkeyBar.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "settingui/include/settinguidefine.h"
#if IS_COLOR
#include "baseui/include/modbaseui.h"
#endif

namespace
{
const char * SET_UI_ID_NORMAL_TITLE = "listTitle";
const char * SET_UI_ID_LIST_VIEW = "listView";

const char * SET_UI_XML_DLG_SETTINGUI = "setting/dlgsettingui.xml";

#define ACTION_SAVE "save"
}

CBaseDialog * CDlgSettingUI::CreateInstance()
{
    CDlgSettingUI * pDlgBaseSettingUI = new CDlgSettingUI(eWT_PageMenu);

    if (NULL != pDlgBaseSettingUI)
    {
        pDlgBaseSettingUI->loadContent(SET_UI_XML_DLG_SETTINGUI);
    }

    return pDlgBaseSettingUI;
}

CDlgSettingUI * CDlgSettingUI::GetDlgBaseSettingUI(CSettingDialogBase * pDlg)
{
    if (NULL == pDlg
            || SETTTING_DIALOG_TYPE_BASE != (SETTTING_DIALOG_TYPE_MASK & pDlg->GetDialogType()))
    {
        return NULL;
    }

    return static_cast<CDlgSettingUI *>(pDlg);
}

CDlgSettingUI::CDlgSettingUI(EWindowType eWndType /*= eWT_PageMenu*/)
    : CSettingDialogBase(this, eWndType)
    , m_pListView(NULL)
    , m_pSetAdapter(NULL)
    , m_nNeedReloadItemIndex(-1)
    , m_nValidIndex(-1)
    , m_nDefPageItemCount(-1)
{
    m_nDialogType = SETTTING_DIALOG_TYPE_BASE;
}

CDlgSettingUI::~CDlgSettingUI()
{
#if IF_SETTING_FRAME_DEBUG
    SETTINGUI_INFO("[CDlgSettingUI::~CDlgSettingUI()]");
#endif

    if (NULL != m_pListView)
    {
        m_pListView->ClearContent();
        m_pListView->SetAdapter(NULL);
    }

    UninitListViewAdapter();
}

bool CDlgSettingUI::LoadPageData(bool bReLoadData /*= false*/)
{
    bool bResult = CSettingDialogBase::LoadPageData(bReLoadData);

    if (NULL == m_pSetAdapter || NULL == m_pSettingUIDelegate)
    {
        return bResult;
    }

    if (m_pSettingUIDelegate->IsFirstBindShow())
    {
        m_pSetAdapter->Init();
    }

    return bResult;
}

void CDlgSettingUI::Init()
{
    CSettingDialogBase::Init();
}

void CDlgSettingUI::Uninit()
{
    CSettingDialogBase::Uninit();
}

void CDlgSettingUI::UpdateWnd(bool bUIReturn/* = false*/)
{
    if (NULL == m_pListView || NULL == m_pSetAdapter)
    {
        SETTINGUI_WARN("CDlgSettingUI::UpdateWnd() fail. ListView is NULL");
        return;
    }
#if IF_SETTING_DISPLAY_DEBUG
    SETTINGUI_INFO("----------------- CDlgSettingUI::UpdateWnd() reloadIdx[%d] ==========="
                   , m_nNeedReloadItemIndex);
#endif
    if (m_nNeedReloadItemIndex >= 0)
    {
        if (NULL != m_pSettingUIDelegate)
        {
            m_pSettingUIDelegate->LoadItemData(m_nNeedReloadItemIndex);
        }
        m_nNeedReloadItemIndex = -1;
    }

    m_pSetAdapter->RefreshListView();
    RefreshSoftkey();
}

void CDlgSettingUI::BindDelegate(CSettingUIDelegateBase * pDelegate)
{
    if (NULL == pDelegate || m_pSettingUIDelegate == pDelegate)
    {
        return;
    }

    if (NULL != m_pSetAdapter)
    {
        m_pSetAdapter->BindDelegate(pDelegate);
    }

    CSettingDialogBase::BindDelegate(pDelegate);
}

void CDlgSettingUI::UnBindDelegate()
{
    CSettingDialogBase::UnBindDelegate();

    if (NULL != m_pSetAdapter)
    {
        m_pSetAdapter->UnBindDelegate();
    }
}

void CDlgSettingUI::RefreshUI(bool bReLoadData /*= false*/)
{
    if (NULL == m_pSetAdapter || NULL == m_pSettingUIDelegate)
    {
        return;
    }

    if (m_pSettingUIDelegate->IsRefreshDataWhenTopDelegate())
    {
        bReLoadData = true;
        m_pSettingUIDelegate->SetRefreshDataWhenTopDelegate(false);
    }

    LoadPageData(bReLoadData);
#if IF_ONE_MORE_ITEM_WITHOUT_TITLE
    if (NULL != m_pTitle)
    {
        if (m_pSettingUIDelegate->GetTitleName().empty())
        {
            m_pTitle->Hide();
            m_pSettingUIDelegate->SetPageItemCount(m_nDefPageItemCount + 1);
        }
        else
        {
            m_pTitle->Show();
            m_pSettingUIDelegate->SetPageItemCount(m_nDefPageItemCount);
        }
    }
    else
#endif
    {
        m_pSettingUIDelegate->SetPageItemCount(m_nDefPageItemCount);
    }

    m_pSetAdapter->SetTotalCount(m_pSettingUIDelegate->GetTotalDataCount());

    UpdateWnd();

#if IF_SETTING_FRAME_DEBUG
    SETTINGUI_INFO("[CDlgSettingUI::RefreshUI] pageId[%s] listview[%p], tot[%d], pageItemNum[%d]"
                   , GetPageID().c_str(), m_pListView, m_pListView->GetTotalDataCount()
                   , m_pListView->GetPageItemCount());
#endif
}

void CDlgSettingUI::RefreshSoftkey()
{
    if (NULL != m_pSetAdapter)
    {
        m_pSetAdapter->RefreshSoftkey();
    }
}

void CDlgSettingUI::CachePageData()
{
    if (NULL == m_pSettingUIDelegate || NULL == m_pListView)
    {
        return;
    }

    CSetUIDlgBaseCache * pSetDlgBaseCache = g_DlgCache.GetDlgCache(m_pSettingUIDelegate->GetPageID());
    if (NULL != pSetDlgBaseCache)
    {
        pSetDlgBaseCache->DialogCache(m_pSettingUIDelegate, m_pListView);
    }
}

bool CDlgSettingUI::DoInitAfterCreateDialog(const yl::string & strPageID)
{
    return InitListViewAdapter(strPageID);
}

bool CDlgSettingUI::DoUninitBeforeDeleteDialog()
{
    return UninitListViewAdapter();
}

bool CDlgSettingUI::OnProcessAction(int iType, bool bPress, int nFocusDataIndex)
{
    bool bResult = false;
    if (NULL == m_pSettingUIDelegate)
    {
        return false;
    }

    if (m_pSettingUIDelegate->OnSoftkeyAction(iType, bPress, nFocusDataIndex))
    {
        return true;

    }

    if (ST_BACK == iType)
    {
        if (m_pSettingUIDelegate->OnBackAction())
        {
            FallBackToThePreviousInterface();
        }
        bResult = true;
    }
    else if (ST_SAVE == iType)
    {
        CachePageData();
        bResult = OnAction(g_pSettingActionTypeMgr->TransSoftkeyType2SettingActionType(ST_SAVE),
                           nFocusDataIndex);
    }
    else if (ST_ENTER == iType)
    {
        bResult = OnAction(g_pSettingActionTypeMgr->TransSoftkeyType2SettingActionType(ST_ENTER),
                           nFocusDataIndex);
    }
    else
    {
        return false;
    }

    return bResult;
}

void CDlgSettingUI::DealWithDataValid(CSettingUIDataValidInfo & dataValid)
{
    m_nValidIndex = dataValid.GetValidIndex();
    if (NULL != m_pSettingUIDelegate)
    {
        m_pSettingUIDelegate->SetFocusIndex(m_nValidIndex);
    }

    CSettingDialogBase::DealWithDataValid(dataValid);
}

void CDlgSettingUI::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL != pMessageBox && MSG_ID_VALID_DATA == pMessageBox->GetID())
    {
        if (NULL != m_pListView)
        {
            // 非法项当前未显示，定位到可以显示的地方
            int nValidIndex = m_nValidIndex;
            if (NULL != m_pSetAdapter && nValidIndex >= 0)
            {
                int nFirstViewIndex = m_pSetAdapter->GetPageDataIndex();
                int nLastViewIndex = nFirstViewIndex + m_pListView->GetPageItemCount();
                if (nValidIndex >= nLastViewIndex || nValidIndex < nFirstViewIndex)
                {
                    m_pSetAdapter->SetPageDataIndex(nValidIndex);
                    m_pListView->RefreshData(nValidIndex);
                }
            }
            // 更新非法项为焦点项
            m_pListView->FocusItemByDataIndex(nValidIndex);
            if (NULL != m_pSetAdapter && NULL != m_pSettingUIDelegate)
            {
                m_pSetAdapter->UpdateIndex();
                m_pSettingUIDelegate->SetPageDataIndex(m_pSetAdapter->GetPageDataIndex());
            }

            m_nValidIndex = -1;
        }
    }
    else
    {
        CSettingDialogBase::MessageBoxCallBack(pMessageBox);
    }
}

void CDlgSettingUI::UpdateFocus(const yl::string & strItemId, bool bFocus)
{
    if (NULL == m_pSettingUIDelegate || NULL == m_pListView
            || NULL == m_pSetAdapter)
    {
        return ;
    }

    CSettingUIPageDataBasePtr pPageData = m_pSettingUIDelegate->GetPageData();
    if (NULL != pPageData)
    {
        int nIndex = pPageData->GetItemDataIndexByItemId(strItemId);
        if (bFocus)
        {
            m_pSettingUIDelegate->SetFocusIndex(nIndex);
            RefreshUI();
        }
#if IF_SETTING_INTERFACE_DEBUG
        SETTINGUI_INFO("-=-=-=-=-=-==-=-=-=-=$$$ [CDlgSettingUI::UpdateFocus] set index[%d] focus[%d] ==="
                       , nIndex, bFocus);
#endif
    }
}

bool CDlgSettingUI::CanBeActivityDialog()
{
    if (NULL != m_pListView && 0 >= m_pListView->GetTotalDataCount())
    {
        return false;
    }

    return true;
}

bool CDlgSettingUI::IsShowVolumeBar(int& iVolumeType, int& iMin, int& iMax, yl::string& strPix)
{
    if (NULL != m_pSettingUIDelegate)
    {
        return m_pSettingUIDelegate->IsShowVolumeBar(iVolumeType, iMin, iMax, strPix);
    }

    return CSettingDialogBase::IsShowVolumeBar(iVolumeType, iMin, iMax, strPix);
}

/////////////////////////////////////////// xLinearLayout st ///////////////////////////////////////////
void CDlgSettingUI::loadChildrenElements(xml_node & node)
{
    xLinearLayout::loadChildrenElements(node);

    m_pListView = (xListView *)getViewById(SET_UI_ID_LIST_VIEW);

    BindTitle(getViewById("boxTitle"));
}

void CDlgSettingUI::loadAttributes(xml_node& node)
{
    xLinearLayout::loadAttributes(node);
    if (NULL != m_pListView)
    {
        m_nDefPageItemCount = m_pListView->GetPageItemCount();
    }
}

bool CDlgSettingUI::ProcessKeyEvent(int nKeyCode, bool bPress)
{
#ifdef IF_SETTING_WIZARD
    if (NULL != g_pSettingUIWizardManager && g_pSettingUIWizardManager->IsInWizard()
            && g_pSettingUIWizardManager->ProcessKeyEvent(nKeyCode, bPress))
    {
        return true;
    }
#endif
    if (NULL != m_pSettingUIDelegate)
    {
        return m_pSettingUIDelegate->ProcessKeyEvent(nKeyCode, bPress);
    }

    return false;
}

bool CDlgSettingUI::onKeyPressedEvent(int key)
{
#if IF_SETTING_FRAME_DEBUG
    SETTINGUI_INFO("################################ CDlgSettingUI::onKeyPressedEvent() dlgbasesetui[%p] m_pSettingUIDelegate[%p], key[%p] ===="
                   , this, m_pSettingUIDelegate, key);
#endif

    int nFocusDataIndex = (NULL != m_pListView) ? m_pListView->GetFocusItemDataIndex() : -1;
    if (NULL != m_pSettingUIDelegate)
    {
        // 上下切换焦点、点击OK时，先保存当前项数据到内存中
        if ((PHONE_KEY_OK == key || PHONE_KEY_UP == key || PHONE_KEY_DOWN == key)
                && NULL != m_pSetAdapter)
        {
            m_pSettingUIDelegate->SetItemDataValueByDataIndex(nFocusDataIndex
                    , m_pSetAdapter->GetItemValueByDataIndex(nFocusDataIndex));
        }

        if (PHONE_KEY_0 <= key && PHONE_KEY_9 >= key && NULL != m_pSetAdapter
                && m_pSetAdapter->OnNumberKeyPress(key))
        {
            return true;
        }

        // 处理顺序：界面Delegate -> OK Cancel默认操作 -> ListView
        if (m_pSettingUIDelegate->OnKeyEvent(key, true, nFocusDataIndex))
        {
            return true;
        }
    }

    if (PHONE_KEY_CANCEL == key && OnProcessAction(ST_BACK, false, nFocusDataIndex))
    {
        return true;
    }

    return xLinearLayout::onKeyPressedEvent(key);
}

bool CDlgSettingUI::OnSoftkeyAction(int iType, bool bLongPress)
{
    if (bLongPress)
    {
        return false;
    }

    int nFocusDataIndex = (NULL != m_pListView) ? m_pListView->GetFocusItemDataIndex() : -1;

    //更新数据
    if (NULL != m_pSettingUIDelegate)
    {
        if (ST_SAVE == iType || ST_START == iType || ST_ENTER == iType
#ifdef IF_SETTING_WIZARD
                || ST_FINISH == iType
#endif
                || ST_LOGIN == iType || ST_NEXT == iType || ST_BACK == iType || ST_ALL_LINES == iType)
        {
            yl::string strValue = "";

            if (NULL != m_pSetAdapter)
            {
                strValue = m_pSetAdapter->GetItemValueByDataIndex(nFocusDataIndex);
            }

            m_pSettingUIDelegate->SetItemDataValueByDataIndex(nFocusDataIndex, strValue);
        }

        if (m_pSettingUIDelegate->OnSoftkeyAction(iType, bLongPress, nFocusDataIndex))
        {
            return true;
        }
    }
    return false;
}

bool CDlgSettingUI::onPaintContent(xPainter & p)
{
    bool bResult = xLinearLayout::onPaintContent(p);

    if (NULL != m_pSettingUIDelegate)
    {
        m_pSettingUIDelegate->PaintContent(p);
    }

    return bResult;
}

bool CDlgSettingUI::onFocusEvent(bool bFocusIn)
{
    if (NULL != m_pSettingUIDelegate)
    {
        return m_pSettingUIDelegate->FocusEvent(bFocusIn);
    }

    return xLinearLayout::onFocusEvent(bFocusIn);
}

#if IS_COLOR
xBrush CDlgSettingUI::GetBackGroundBrush()
{
    xBrush brushBG;

    BaseUI_GetNormalDialogBG(brushBG);

    return brushBG;
}
#endif

/////////////////////////////////////////// xLinearLayout en ///////////////////////////////////////////
/////////////////////////////////////////// 界面属性 st ///////////////////////////////////////////

xView * CDlgSettingUI::GetTitleView() const
{
    if (NULL == m_pTitle)
    {
        return NULL;
    }

    return m_pTitle->GetTitleView();
}

xListView * CDlgSettingUI::GetListView() const
{
    return m_pListView;
}

CSettingUIAdapter * CDlgSettingUI::GetListViewAdapter() const
{
    return m_pSetAdapter;
}

bool CDlgSettingUI::InitListViewAdapter(const yl::string & strPageId)
{
    if (NULL == m_pListView)
    {
        SETTINGUI_WARN("[CDlgSettingUI::InitListViewAdapter] listview is null");
        return false;
    }

#if IF_SETTING_FRAME_DEBUG
    SETTINGUI_INFO("--------------- [CDlgSettingUI::InitListViewAdapter] listview[%p] ===="
                   , m_pListView);
#endif

    UninitListViewAdapter();

    m_pSetAdapter = g_SettingAdapterFactory.GetSettingAdapter(strPageId);
    if (NULL == m_pSetAdapter)
    {
        m_pSetAdapter = g_SettingAdapterFactory.GetDefaultSettingAdapter();
    }
    if (NULL != m_pSetAdapter)
    {
        m_pListView->SetAdapter(m_pSetAdapter);
    }

    return true;
}

bool CDlgSettingUI::UninitListViewAdapter()
{
    // 先清除原来的Adapter
    if (NULL != m_pSetAdapter)
    {
        m_pSetAdapter->UnBindDelegate();
        delete m_pSetAdapter;
        m_pSetAdapter = NULL;
    }

    return true;
}

bool CDlgSettingUI::IsSetActiveWhileAddToBigWnd()
{
    if (NULL != m_pSettingUIDelegate)
    {
        return m_pSettingUIDelegate->IsSetActiveWndWhileCreate();
    }

    return false;
}

bool CDlgSettingUI::onActiveEvent(bool bActive)
{
    if (bActive && NULL != m_pSettingUIDelegate)
    {
        if (m_pSettingUIDelegate->IsRefreshWndWhileActive())
        {
            m_pSettingUIDelegate->SetRefreshWndWhileActive(false);
            RefreshUI();
        }
        else if (m_pSettingUIDelegate->IsRefreshDataWhenTopDelegate())
        {
            m_pSettingUIDelegate->SetRefreshDataWhenTopDelegate(false);
            RefreshUI(true);
        }
        else if (NULL != m_pListView)
        {
            int nFocusIndex = m_pListView->GetFocusItemIndex();
            m_pListView->FocusItemByIndex(nFocusIndex);
        }
    }
    else if (!bActive && NULL != m_pListView)
    {
        // 窗口层叠时，防止焦点效果没有及时刷新
        m_pListView->update(true);
    }

    return xLinearLayout::onActiveEvent(bActive);
}
