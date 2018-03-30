#include "transfernumselector.h"

#include "baseui/framelist.h"
#include "diruihelper.h"
#include "baseui/detaillistitem.h"
#include "translateiddefine.h"
#include "settingui/src/settingitembase.h"
#include "commonunits/modcommonunits.h"
#include "messagebox/modmessagebox.h"
#include "messagebox/selectmessagebox.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"

namespace
{
#define TRANSFER_NUM_SELECT_POP_BOX_ITEM_HEIGHT 47
}

CTransferNumSelector::CTransferNumSelector()
    : m_nSelIndex(-1)
    , m_eTranNumType(TRAN_NUM_DIR)
{
    m_bItemFocus = false;
    m_bDefaultFocus = false;
}

CTransferNumSelector::~CTransferNumSelector()
{
    CMessageBoxBase * pTopMsgBox = MessageBox_GetTopMessageBox();

    if (NULL != pTopMsgBox && pTopMsgBox->GetEventFilter() == this)
    {
        pTopMsgBox->SetEventFilter(NULL);
    }
}

void CTransferNumSelector::Init(TRAN_NUM_TYPE eTranNumType)
{
    m_nSelIndex = 0;
    m_eTranNumType = eTranNumType;
}

bool CTransferNumSelector::GetNumber(yl::string & strNumber) const
{
    yl::string strName;

    bool ret = true;
    if (m_eTranNumType == TRAN_NUM_DIR)
    {
        ret = CDirUIHelper::GetAttributeByIndex(m_nSelIndex, strName, strNumber);
    }
    else
    {
        strNumber = m_strNumber;
    }
    DIRUI_INFO("ret:%d name:%s number:%s", ret, strName.c_str(), strNumber.c_str());
    return ret;
}

const yl::string & CTransferNumSelector::GetName() const
{
    return m_strName;
}

void CTransferNumSelector::SetName(const yl::string & strName)
{
    m_strName = strName;
}

const yl::string & CTransferNumSelector::GetNumber() const
{
    return m_strNumber;
}

void CTransferNumSelector::SetNumber(const yl::string & strNumber)
{
    m_strNumber = strNumber;
}

void CTransferNumSelector::OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount)
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

    yl::string strValue;
    yl::string strName;
    int nItemIndex = 0;
    int nSize = m_eTranNumType == TRAN_NUM_DIR ? CDirUIHelper::GetAttributeSize() : 1;
    int nMaxPageItemCount = m_pFrameList->GetMaxPageItemCount();

    m_pFrameList->SetTotalDataCount(nSize);

    for (; nItemIndex < nMaxPageItemCount && nDataIndex < nSize; ++nItemIndex, ++nDataIndex)
    {
        if (m_eTranNumType == TRAN_NUM_DIR)
        {
            if (!CDirUIHelper::GetAttributeByIndex(nDataIndex, strName, strValue))
            {
                break;
            }
        }
        else
        {
            strName = m_strName;
            strValue = m_strNumber;
        }

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
            pRadioItem->setObjectName(QString("RadioItem%1").arg(nItemIndex));
            pRadioItem->SetChangeStyleOnSelect(false);
            pRadioItem->setGeometry(0, 0, 0, TRANSFER_NUM_SELECT_POP_BOX_ITEM_HEIGHT);
        }

        pRadioItem->ClearContent();
        pRadioItem->CListItem::SetAction(this);
        QString strTitle = LANG_TRANSLATE(strName.c_str());
        QString strLabelValue = toQString(strValue, true);
        if (toQString(strName, true) == strLabelValue
                || strTitle.isEmpty())
        {
            pRadioItem->SetLabText(strLabelValue);
        }
        else
        {
            pRadioItem->SetLabText(strTitle + QLatin1String(":") + strLabelValue);
        }

        pRadioItem->SetDataIndex(nDataIndex);

        if (nDataIndex == m_nSelIndex)
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
    }

    int nItemAmount = m_pFrameList->GetTotalItemCount();

    for (int i = nItemIndex; i < nItemAmount; ++i)
    {
        m_pFrameList->DelItemByIndex(nItemIndex);
    }

    m_pFrameList->update();
}

void CTransferNumSelector::OnFrameListAttach(CFrameListBase * pFrameList)
{
    CFrameListDelegate::OnFrameListAttach(pFrameList);

    if (NULL == m_pFrameList)
    {
        return;
    }

    int nMaxPageItemCount = m_pFrameList->GetMaxPageItemCount();
    int nPageDataIndex = 0;
    int nFocusIndex = 0;

    if (nMaxPageItemCount > 0)
    {
        nFocusIndex = m_nSelIndex % nMaxPageItemCount;
        nPageDataIndex = m_nSelIndex - nFocusIndex;
    }

    SetPageDataIndex(nPageDataIndex);
    SetFocusIndex(nFocusIndex);
}

void CTransferNumSelector::OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction)
{
    CFrameList * pList = (CFrameList *)pFrameList;

    if (NULL == pList || pList != m_pFrameList || NULL == pItem || LIST_ITEM_ACTION_SELECT != nAction)
    {
        return;
    }

    m_nSelIndex = pItem->GetDataIndex();
}

bool CTransferNumSelector::ProcessKeyEvent(int nKeyCode, bool bPress, bool & bFilter)
{
    if (!bPress)
    {
        return false;
    }

    if (PHONE_KEY_POUND != nKeyCode)
    {
        return false;
    }

    CMessageBoxBase * pTopMsgBox = MessageBox_GetTopMessageBox();

    if (NULL != pTopMsgBox
            && pTopMsgBox == m_pFrameList->parent()
            && pTopMsgBox->GetID() == DIR_UI_ACTION_TRANSFER_NUMBER_CHOOSE)
    {
        MessageBox_RemoveTopMessageBox(MessageBox_Send);
    }

    bFilter = true;
    return true;
}