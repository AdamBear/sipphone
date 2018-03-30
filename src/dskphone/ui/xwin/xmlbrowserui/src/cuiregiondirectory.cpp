#include "cuiregiondirectory.h"
#include "keymap.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "extended/xmlbrowser/include/modxmlbrowser.h"
#include "modxmlbrowserui.h"
#include "xmlbrowseruicommon.h"
#include "baseui/include/menulistitem.h"
#include "baseui/include/listviewitemfactory.h"
#include "xwindow/xListView.h"
#include "xwindow/xWindowManager.h"
#include "imeedit/ximeedit.h"
#include "baseui/include/inputmethod.h"

namespace
{
#define MAX_LEN_XMLDIRECTORY_SEARCH   99  //xml联系人搜索最大长度
#define SEARCH_DELAY_TIME_MIN 0
#define SEARCH_DELAY_TIME_MAX 10000
#define XML_REGION_DIRECTORY_XML             "xmlbrowserui/xmlregiondirectory.xml"
#define XML_REGION_DIRECTORY_SEARCH_INPUT    "editKey"
#define XMLREGION_DIRECTORY_ITEM_HIGHT       25
#define XMLREGION_DIRECTORY_PAGE_ITEM_NUM_T54_T46    6
#define XMLREGION_DIRECTORY_PAGE_ITEM_NUM_T52        5
}

#if XML_BROWSER_SUPPORT_UI
////////////////////////////////////////////////////////////////////////////////
CUIRegionDirectory::CUIRegionDirectory()
    : CUIRegionBaseListScreen(false)
#if IF_FEATURE_XMLDIRECTORY
    , m_pInput(NULL)
    , m_iSearchDelay(SEARCH_DELAY_TIME_MAX)
    , m_bInputShow(false)
#endif
{
    m_vecItemData.clear();
    loadContent(XML_REGION_DIRECTORY_XML);
}

CUIRegionDirectory::~CUIRegionDirectory()
{
    UnInit();
}

void CUIRegionDirectory::Update(void * pDirectoryData)
{
    if (NULL == pDirectoryData)
    {
        return;
    }

    XBDirectoryData * pNewData = static_cast<XBDirectoryData *>(pDirectoryData);

    if (NULL == m_pTxtTitle
            || NULL == m_pListView || NULL == pNewData)
    {
        return;
    }

    m_stBaseData.m_strTitle = pNewData->stAttr.strTitle;
    m_stBaseData.m_bIsWrapTitle = pNewData->stAttr.bWrapTitle;
    m_stBaseData.m_nDefaultIndex = pNewData->stAttr.iDefaultIndex;
    LoadMenuItemData(pNewData);
    m_iQuitTime = pNewData->stAttr.iTimeOut;
    m_bLock = pNewData->stAttr.bLockIn;
#if IF_FEATURE_XMLDIRECTORY
    if (m_pInput != NULL)
    {
        if (!pNewData->stInputField.strParam.empty())
        {
            m_pInput->show();
            m_bInputShow = true;
            if (!pNewData->stInputField.strDefault.empty())
            {
                m_pInput->SetInitText(pNewData->stInputField.strDefault);
            }

            m_stBaseData.m_nDefaultIndex = INVALID_ITEM_INDEX;
        }
        else
        {
            m_bInputShow = false;
            m_pInput->hide();
            if (m_pListView != NULL)
            {
                m_pListView->setFocus();
            }
        }
    }
    m_iSearchDelay = configParser_GetConfigInt(kszXMLPhonebookSearchDelay);

    if (m_iSearchDelay < SEARCH_DELAY_TIME_MIN)
    {
        m_iSearchDelay = SEARCH_DELAY_TIME_MIN;
    }

    if (m_iSearchDelay > SEARCH_DELAY_TIME_MAX)
    {
        m_iSearchDelay = SEARCH_DELAY_TIME_MAX;
    }
    StopSearchTime();
    //保存搜索自定义softkey
    m_vecSearchSoftkey = pNewData->stInputField.vecSoftkey;
#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
    m_vecGlobalSoftkey = pNewData->vecSoftkey;
#endif
    if (m_pInput != NULL && !pNewData->stInputField.strParam.empty())
    {
        SetSoftkey(m_vecSearchSoftkey);
        return;
    }
#endif

    SetSoftkey(pNewData->vecSoftkey);

}

const VecSoftkeyParam & CUIRegionDirectory::GetFocusSoftkey()
{
    int nIndex = GetCurrentItemIndex();
    return GetSoftkeyByIndex(nIndex);
}

const VecSoftkeyParam & CUIRegionDirectory::GetSoftkeyByIndex(int nIndex)
{
    if (nIndex >= 0 && nIndex < m_vecItemData.size())
    {
        return m_vecItemData[nIndex].m_vecSoftkey;
    }
    return sVecSoftParam;
}

#if IF_FEATURE_XMLDIRECTORY

bool CUIRegionDirectory::onFocusMoveEvent(xView * pNewFocus, xView * pOldFocus)
{
    if (pNewFocus == m_pInput || (pOldFocus == m_pInput && NULL != m_pListView
                                  && (m_pListView->hasFocus()) || m_pListView->containFocus()))
    {
        if (NULL != m_pListView && (!m_pListView->hasFocus() && !m_pListView->containFocus()))
        {
            m_pListView->SetItemFocusByIndex(INVALID_ITEM_INDEX);

        }
        // 焦点变化时dlgbrowser，会获取更新softkey

        return true;
    }

    return false;
}


void CUIRegionDirectory::slotOnIMEChanged(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData)
{
    if (NULL == m_pInput)
    {
        return;
    }

    yl::string strIME = m_pInput->GetImeName();

    if (strIME.empty())
    {
        strIME = "Abc";
    }

    configParser_SetConfigString(kszContactSearchDefaultIME, strIME.c_str(), CONFIG_LEVEL_PHONE);
}

void CUIRegionDirectory::OnKeyWordChanged(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData)
{
    StartSearchTime();
}


void CUIRegionDirectory::StopSearchTime()
{
    if (m_SearchTimer.IsTimerRuning())
    {
        printf("m_SearchTimer.KillTimer();\n");
        m_SearchTimer.KillTimer();
    }
}


void CUIRegionDirectory::StartSearchTime()
{
    StopSearchTime();
    printf("m_SearchTimer.SetTimer();%d\n", m_iSearchDelay);
#ifdef _WIN32
    m_SearchTimer.SetTimer(m_iSearchDelay, (void*)&m_SearchTimer, this, static_cast<msgEntry>(&CUIRegionDirectory::OnTimeOut));
#else
    m_SearchTimer.SetTimer(m_iSearchDelay, (void*)&m_SearchTimer, this, static_cast<mkMsgEntry>(&CUIRegionDirectory::OnTimeOut));
#endif
}

BOOL CUIRegionDirectory::OnTimeOut(msgObject& msg)
{
    Search();
    return TRUE;
}

void CUIRegionDirectory::Search()
{
    StopSearchTime();

    const yl::string & strSearchContent = m_pInput->GetText();
    if (!strSearchContent.empty())
    {
        xmlbrowser_OnAction(GetElementID(), XB_ACTION_SEARCH_PHONEBOOK,
                            (void *)strSearchContent.c_str());
    }
}

void CUIRegionDirectory::OnXMLUIAction(XB_SOFTKEY_TYPE eType)
{
    switch (eType)
    {
    case XB_ST_SEARCH:
        {
            if (NULL != m_pInput)
            {
                Search();
            }
        }
        break;
    default:
        break;
    }
}

VecSoftkeyParam & CUIRegionDirectory::GetSoftkey()
{
    if (m_bInputShow)
    {
        if (m_pInput->hasFocus())
        {
            SetSoftkey(m_vecSearchSoftkey);
        }
        else
        {
#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
            SetSoftkey(m_vecGlobalSoftkey);
#endif
        }
    }
    else
    {
        SetSoftkey(m_vecSearchSoftkey);
    }
    return CUIRegionBase::GetSoftkey();
}

#endif

void CUIRegionDirectory::SetDefaultSoftkey(VecSoftkeyParam & softkeyParam)
{
    softkeyParam.clear();
#if IF_FEATURE_XMLDIRECTORY
    if (m_pInput != NULL
            && m_pInput->hasFocus())
    {
        softkeyParam.push_back(TypeToSoftInfo(XB_ST_EXIT));
        softkeyParam.push_back(TypeToSoftInfo(XB_ST_CHANGEMODE));
        softkeyParam.push_back(TypeToSoftInfo(XB_ST_BACKSPACE));
        softkeyParam.push_back(TypeToSoftInfo(XB_ST_SEARCH));
    }
    else
#endif
    {
        softkeyParam.push_back(TypeToSoftInfo(XB_ST_DIAL));
        softkeyParam.push_back(TypeToSoftInfo(XB_ST_PREVIOUS));
        softkeyParam.push_back(TypeToSoftInfo(XB_ST_NEXT));
        softkeyParam.push_back(TypeToSoftInfo(XB_ST_EXIT));
    }
}

bool CUIRegionDirectory::LoadMenuItemData(XBDirectoryData * pDirectoryData)
{
    if (NULL == pDirectoryData || NULL == m_pTxtTitle
            || NULL == m_pListView)
    {
        return false;
    }

    DirecotryItemData vecData;
    vecData.m_bIsWrap = false;

    //解析数据
    List_Dir_Menu_Item::iterator it = pDirectoryData->listItem.begin();

    //显示序号
    int nIndex = 0;
    yl::string strIndex;
    m_vecItemData.clear();
    for (/* */; it != pDirectoryData->listItem.end(); it++)
    {
        vecData.m_strText = (*it)->strPrompt;
        vecData.m_nIndex = nIndex;

        nIndex++;

#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
        vecData.m_vecSoftkey = (*it)->vecSoftkey;
#endif
        m_vecItemData.push_back(vecData);
    }
    return true;
}

void CUIRegionDirectory::InitListViewLayout(xListView * pListView)
{
    if (NULL == pListView)
    {
        return;
    }
    xListViewStylePtr pStyle = pListView->GetStyle();
    if (NULL == pStyle)
    {
        return;
    }

    int nItemHeight = GetItemHeight();
    if (0 == nItemHeight)
    {
        return;
    }

    pListView->SetTotalDataCount(m_vecItemData.size());
}

xListViewItem * CUIRegionDirectory::PrepareItem(xListView * pListView, int nDataIndex,
        xListViewItem * pItem)
{
    if (NULL == pListView || pListView != m_pListView)
    {
        return NULL;
    }

    CMenuListItemPtr pMenuItem = CMenuListItem::GetMenuListItem(pItem);

    if (NULL == pMenuItem)
    {
        pMenuItem = static_cast<CMenuListItemPtr>(g_ItemFactory.ApplyItem(
                        CMenuListItem::GetMenuItemType()));
    }


    return pMenuItem;
}

bool CUIRegionDirectory::RefreshItem(xListView * pListView, xListViewItem * pItem)
{
    if (NULL == pListView || pListView != m_pListView)
    {
        return false;
    }

    CMenuListItemPtr pMenuItem = CMenuListItem::GetMenuListItem(pItem);

    if (NULL == pMenuItem)
    {
        return false;
    }

    pMenuItem->SetAction(this);

    int nDataIndex = pItem->GetDataIndex();

    if (nDataIndex >= 0 && nDataIndex < m_vecItemData.size())
    {
        const DirecotryItemData & stMenuData = m_vecItemData[nDataIndex];
        pMenuItem->SetShowIndex(true);
        pMenuItem->SetMenu(stMenuData.m_strText);
    }

    return true;
}

void CUIRegionDirectory::loadChildrenElements(xml_node & node)
{
    CUIRegionBaseListScreen::loadChildrenElements(node);
#if IF_FEATURE_XMLDIRECTORY
    m_pInput = static_cast<xEditTextView *>(getViewById(XML_REGION_DIRECTORY_SEARCH_INPUT));
#endif
}


void CUIRegionDirectory::RelayoutByContent()
{
    if (NULL == m_pTxtTitle
            || NULL == m_pListView)
    {
        return;
    }

    m_pTxtTitle->SetText(m_stBaseData.m_strTitle);

    if (m_stBaseData.m_bIsWrapTitle)
    {
        m_pTxtTitle->SetMultiLineEnable(true);
    }
    else
    {
        m_pTxtTitle->SetMultiLineEnable(false);
        m_pTxtTitle->SetOverLengthStyle(OVERLENGTH_TYPE_CUT_WITH_ELLIPSIS);
    }
}

bool CUIRegionDirectory::OnKeyPress(int nKey)
{
#if IF_FEATURE_XMLDIRECTORY
    StopSearchTime();
#endif

    if (NULL == m_pListView)
    {
        return false;
    }

#if IF_FEATURE_XMLDIRECTORY
    switch (nKey)
    {
    case PHONE_KEY_OK:
        {
            if (m_pInput->hasFocus())
            {
                Search();
                return true;
            }
        }
        break;
    case PHONE_KEY_LEFT:
    case PHONE_KEY_RIGHT:
        {
            return false;
        }
        break;

    default:
        {
            if (nKey == PHONE_KEY_STAR
                    || nKey == PHONE_KEY_POUND
                    || (nKey >= PHONE_KEY_0
                        && nKey <= PHONE_KEY_9))
            {
                if (NULL == m_pListView || NULL == m_pInput || m_pInput->hasFocus())
                {
                    return false;
                }

                m_pListView->FocusItemByIndex(INVALID_ITEM_INDEX);
                m_pInput->setFocus();

                //g_WorkStation.xWorkStation::dispatchKeypadEvent(nKey, true);
                return false;
            }
        }
        break;
    }

    if (CUIRegionBaseListScreen::OnKeyPress(nKey))
    {
        return true;
    }
#endif
    return false;
}


void CUIRegionDirectory::Init()
{
#if IF_FEATURE_XMLDIRECTORY
    if (NULL != m_pInput)
    {
        yl::string strSupportIME = IME_Name;
        yl::string strIME = configParser_GetConfigString(kszXmlInputStringDefaultIME);
        IME_InstallIME(m_pInput, strSupportIME.c_str());

        m_pInput->SetIme(strIME);
        m_pInput->SetMaxLength(MAX_LEN_XMLDIRECTORY_SEARCH);

        SLOT_CONNECT(m_pInput, signalTextConfirmChange, this, CUIRegionDirectory::OnKeyWordChanged);
        SLOT_CONNECT(m_pInput, signalImeChanged, this, CUIRegionDirectory::slotOnIMEChanged);
    }
#endif
}


void CUIRegionDirectory::UnInit()
{
#if IF_FEATURE_XMLDIRECTORY
    StopSearchTime();
    if (NULL != m_pInput)
    {
        IME_UninstallIME(m_pInput);
        SLOT_DISCONNECT(m_pInput, signalTextConfirmChange, this, CUIRegionDirectory::OnKeyWordChanged);
        SLOT_DISCONNECT(m_pInput, signalImeChanged, this, CUIRegionDirectory::slotOnIMEChanged);
    }
#endif
}

#endif
