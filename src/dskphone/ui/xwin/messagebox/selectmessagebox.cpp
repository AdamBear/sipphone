#include "selectmessagebox.h"
#include "keymap.h"
#include "xwindow/xWindowManager.h"
#include "xwindow/xSoftkeyBar.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "baseui/include/listviewitemfactory.h"
#include "baselibs/include/devicelib/phonedevice.h"

namespace
{
#define MSG_BOX_LAYOUT_FILE_SELECT "messagebox/selectmsgbox.xml"
#define MSG_BOX_LAYOUT_FILE_SELECT_WINDOW "messagebox/selectmsgbox_window.xml"

const char * MSGBOX_ID_SELECT_BOX_TITLE = "selectmsgboxtitle";
const char * MSGBOX_ID_SELECT_BOX_LIST = "selectmsgboxlist";
}

CSelectMessageBox::CSelectMessageBox()
    : m_pTitleBar(NULL)
    , m_pListView(NULL)
    , m_nFocusAction(-1)
{
}

CSelectMessageBox::~CSelectMessageBox()
{
    Uninit();
}

int CSelectMessageBox::GetFcousAction()
{
    return m_nFocusAction;
}

CMessageBoxBase * CSelectMessageBox::CreateSelectMsgBox(const MSGBOX_INFO & stMsgInfo)
{
    CSelectMessageBox * pSelectMsgBox = new CSelectMessageBox;

    if (NULL != pSelectMsgBox)
    {
        SELECTMSGBOXINFO* tSelectInfo = static_cast<SELECTMSGBOXINFO *>(stMsgInfo.extraData);

        SELECTBOX_STYLE eStyle = tSelectInfo->m_eStyle;

        if (SELECTBOX_STYLE_AUTO == eStyle)
        {
            if (PT_T52 == devicelib_GetPhoneType())
            {
                eStyle = SELECTBOX_STYLE_WINDOW;
            }
            else
            {
                eStyle = SELECTBOX_STYLE_OPTIONBOX;
            }
        }

        switch (eStyle)
        {
        case SELECTBOX_STYLE_OPTIONBOX:
            pSelectMsgBox->loadContent(MSG_BOX_LAYOUT_FILE_SELECT);
            break;
        case SELECTBOX_STYLE_WINDOW:
            pSelectMsgBox->loadContent(MSG_BOX_LAYOUT_FILE_SELECT_WINDOW);
            break;
        default:
            return NULL;
            break;
        }

        pSelectMsgBox->CreateWindowObject();

        pSelectMsgBox->SetID(stMsgInfo.iID);
        pSelectMsgBox->SetType(stMsgInfo.eType);
        pSelectMsgBox->SetExitTime(stMsgInfo.nMsTime);
        pSelectMsgBox->SetExtraData(stMsgInfo.extraData);

        if (!stMsgInfo.strTitle.empty())
        {
            pSelectMsgBox->SetTitleBar(stMsgInfo.strTitle);
        }

        pSelectMsgBox->SetSelectInfo(tSelectInfo);

        if (stMsgInfo.vecSoftkey.size() > 0)
        {
            pSelectMsgBox->SetSoftkey(stMsgInfo.vecSoftkey);
        }
        else
        {
            pSelectMsgBox->SetDefaultSoftKey(stMsgInfo.eType);
        }
    }

    return pSelectMsgBox;
}

void CSelectMessageBox::loadChildrenElements(xml_node & node)
{
    xFrameLayout::loadChildrenElements(node);

    m_pTitleBar = static_cast<xTextView *>(getViewById(MSGBOX_ID_SELECT_BOX_TITLE));
    m_pListView = static_cast<xListView *>(getViewById(MSGBOX_ID_SELECT_BOX_LIST));

    Init();
}

void CSelectMessageBox::Init()
{
    if (NULL != m_pListView)
    {
        m_pListView->SetAdapter(this);
        m_pListView->ClearContent();
    }

    m_vecSelectItemInfo.m_vecSelectMsgBoxItem.clear();
}

void CSelectMessageBox::Uninit()
{
    if (NULL != m_pListView)
    {
        m_pListView->ClearContent();
        m_pListView->SetAdapter(NULL);
    }

    m_vecSelectItemInfo.m_vecSelectMsgBoxItem.clear();
}

void CSelectMessageBox::SetDefaultSoftKey(int nType)
{
    VecMsgBoxSoftkey vecSoftKey;
    MsgBoxSoftkey stSoftKey;

    stSoftKey.m_title = _LANG(TRID_CANCEL);
    stSoftKey.m_result = MessageBox_Cancel;
    vecSoftKey.push_back(stSoftKey);

    stSoftKey.m_title = "";
    stSoftKey.m_result = MessageBox_ERROR;
    vecSoftKey.push_back(stSoftKey);
    vecSoftKey.push_back(stSoftKey);

    stSoftKey.m_title = _LANG(TRID_OK);
    stSoftKey.m_result = MessageBox_OK;
    vecSoftKey.push_back(stSoftKey);

    SetSoftkey(vecSoftKey);
}

bool CSelectMessageBox::OnSoftkeyAction(int nType, bool bLongPress)
{
    if (MessageBox_OK == nType)
    {
        ExitMessageBox(MessageBox_OK);
        return true;
    }

    return CMessageBoxBase::OnSoftkeyAction(nType, bLongPress);
}

void CSelectMessageBox::ExitMessageBox(MESSAGEBOX_RESULT eResult)
{
    if (NULL != m_pListView)
    {
        int nFocusIndex = m_pListView->GetFocusItemDataIndex();
        if (nFocusIndex >= 0
                && nFocusIndex < m_vecSelectItemInfo.m_vecSelectMsgBoxItem.size())
        {
            m_nFocusAction = m_vecSelectItemInfo.m_vecSelectMsgBoxItem[nFocusIndex].m_nAction;
        }
    }

    CMessageBoxBase::ExitMessageBox(eResult);
}

bool CSelectMessageBox::ProcessKeyEvent(int nKeyCode, bool bPress)
{
    if (bPress && NULL != m_pListView)
    {
        if (PHONE_KEY_UP == nKeyCode
                || PHONE_KEY_DOWN == nKeyCode
                || PHONE_KEY_FN1 == nKeyCode
                || PHONE_KEY_FN2 == nKeyCode
                || PHONE_KEY_FN3 == nKeyCode
                || PHONE_KEY_FN4 == nKeyCode)
        {
            return false;
        }
    }

    if (PHONE_KEY_OK == nKeyCode && bPress)
    {
        ExitMessageBox(MessageBox_OK);
        return true;
    }

    return CMessageBoxBase::ProcessKeyEvent(nKeyCode, bPress);
}

void CSelectMessageBox::SetTitleBar(const yl::string & strTitle)
{
    if (NULL == m_pTitleBar)
    {
        return;
    }

    m_pTitleBar->SetText(strTitle);
}

void CSelectMessageBox::SetSelectInfo(SELECTMSGBOXINFO * pSelectBoxInfo)
{
    if (NULL == pSelectBoxInfo)
    {
        return;
    }

    m_vecSelectItemInfo.m_vecSelectMsgBoxItem.clear();

    m_vecSelectItemInfo.m_SelectIndex = pSelectBoxInfo->m_SelectIndex;
    m_vecSelectItemInfo.m_vecSelectMsgBoxItem = pSelectBoxInfo->m_vecSelectMsgBoxItem;

    m_vecSelectItemInfo.m_bShowCheckIcon = pSelectBoxInfo->m_bShowCheckIcon;

    m_vecSelectItemInfo.m_eStyle = pSelectBoxInfo->m_eStyle;

    if (SELECTBOX_STYLE_AUTO == m_vecSelectItemInfo.m_eStyle)
    {
        if (PT_T52 == devicelib_GetPhoneType())
        {
            m_vecSelectItemInfo.m_eStyle = SELECTBOX_STYLE_WINDOW;
        }
        else
        {
            m_vecSelectItemInfo.m_eStyle = SELECTBOX_STYLE_OPTIONBOX;
        }
    }

}

void CSelectMessageBox::SetWidgetFocus()
{
    if (-1 != m_vecSelectItemInfo.m_SelectIndex)
    {
        SetFocusIndex(m_vecSelectItemInfo.m_SelectIndex);
        SetContentPos(m_vecSelectItemInfo.m_SelectIndex);
    }

    RefreshListView();
}

//-----------listView----------------------
void CSelectMessageBox::InitListViewLayout(xListView * pListView)
{
    if (NULL == pListView)
    {
        return;
    }

    pListView->SetTotalDataCount(m_vecSelectItemInfo.m_vecSelectMsgBoxItem.size());
}

xListViewItem * CSelectMessageBox::PrepareItem(xListView * pListView, int nDataIndex,
        xListViewItem * pItem)
{
    POPUPBOX_INFO("CSelectMessageBox::PrepareItem nDataIndex = %d", nDataIndex);

    if (NULL == pListView || pListView != m_pListView)
    {
        return NULL;
    }

    if (NULL != pItem && pItem->GetType() == LIST_ITEM_SUB_TYPE_SELECT)
    {
        return pItem;
    }

    xListViewItem * pNewItem = g_ItemFactory.ApplyItem(LIST_ITEM_SUB_TYPE_SELECT);

    if (NULL != pNewItem)
    {
        pNewItem->SetAction(this);
    }

    return pNewItem;
}

bool CSelectMessageBox::RefreshItem(xListView * pListView, xListViewItem * pItem)
{
    if (NULL == pListView || pListView != m_pListView)
    {
        return false;
    }

    CSelectItem * pSelectItem = CSelectItem::GetSelectItem(pItem);

    if (NULL == pSelectItem)
    {
        POPUPBOX_ERR("Get Select item fail in refresh.");
        return false;
    }

    int nDataIndex = pSelectItem->GetDataIndex();

    //加载数据
    if (0 <= nDataIndex && nDataIndex < m_vecSelectItemInfo.m_vecSelectMsgBoxItem.size())
    {
        yl::string strTitle = m_vecSelectItemInfo.m_vecSelectMsgBoxItem[nDataIndex].m_strTitle;
        if (SELECTBOX_STYLE_WINDOW == m_vecSelectItemInfo.m_eStyle)
        {
            yl::string strIndex;

            char szTmp[8];
            sprintf(szTmp, "%d", (nDataIndex + 1));
            strIndex = szTmp;
            strIndex = strIndex + ".";
            strTitle = strIndex + strTitle;
        }

        pSelectItem->SetSelectText(strTitle
                                   , m_vecSelectItemInfo.m_bShowCheckIcon
                                   , (nDataIndex == m_vecSelectItemInfo.m_SelectIndex));
    }

    return true;

}

void CSelectMessageBox::OnCleanContent(xListView * pListView)
{
    if (NULL == pListView || pListView != m_pListView)
    {
        return;
    }

    ResetIndex();
}

void CSelectMessageBox::OnItemAction(xListView * pListView, xListViewItem * pItem, int nAction)
{
    xListViewAdapter::OnItemAction(pListView, pItem, nAction);

    POPUPBOX_INFO("CSelectMessageBox::OnItemAction(xListView:%p Item:%p Action:%d)",
                  pListView, pItem, nAction);

    if (NULL == pListView || pListView != m_pListView || NULL == pItem)
    {
        return;
    }

    if (LIST_VIEW_ITEM_ACTION_FOCUS == nAction)
    {
        UpdateIndex();

        return;
    }
}
//-----------listView----------------------
