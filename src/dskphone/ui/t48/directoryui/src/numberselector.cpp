#include "baseui/framelist.h"
#include "numberselector.h"

#include "diruihelper.h"
#include "baseui/detaillistitem.h"
#include "commonunits/modcommonunits.h"
#include "messagebox/modmessagebox.h"
#include "messagebox/selectmessagebox.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"


namespace
{
#define NUMBER_SELECT_POP_BOX_ITEM_HEIGHT 47

#define NUMBER_SELECT_POP_BOX_ITEM_CONTENT_TOP 0
#define NUMBER_SELECT_POP_BOX_ITEM_CONTENT_HEIGHT 43

#define NUMBER_SELECT_POP_BOX_ITEM_VALUE_LEFT 140
#define NUMBER_SELECT_POP_BOX_ITEM_VALUE_WIDTH 210

#define NUMBER_SELECT_POP_BOX_ITEM_TITLE_LEFT 12
#define NUMBER_SELECT_POP_BOX_ITEM_TITLE_WEIGHT 120
}


CNumberSelector::CNumberSelector()
    : m_nSelIndex(-1)
{
    m_bItemFocus = false;
    m_bDefaultFocus = false;
}

CNumberSelector::~CNumberSelector(void)
{
}

bool CNumberSelector::GetNumber(yl::string & strNumber) const
{
    yl::string strName;

    return CDirUIHelper::GetAttributeByIndex(m_nSelIndex, strName, strNumber);
}

const yl::string & CNumberSelector::GetName() const
{
    return m_strName;
}

void CNumberSelector::SetName(const yl::string & strName)
{
    m_strName = strName;
}

void CNumberSelector::OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount)
{
    CFrameList * pList = (CFrameList *)pFrameList;

    if (NULL == pList || pList != m_pFrameList)
    {
        return;
    }

    if (!m_pFrameList->IsCanReuseItem(CDetailListItem::GetDetailListItemType()))
    {
        return;
    }

    int nMaxPageTotalCount = m_pFrameList->GetMaxPageItemCount();
    int nSize = CDirUIHelper::GetAttributeSize();
    yl::string strValue;
    yl::string strName;
    int nItemIndex = 0;

    m_pFrameList->SetTotalDataCount(nSize);

    bool bResetNumName = false; //是否要重新组合Numeber翻译
    bool bDisplayNumber = configParser_GetConfigInt(kszLdapNumberDisplayMode) ==
                          0; //number的属性名称显示为Number
    if (DIRECTORY_TYPE_LDAP == CDirUIHelper::GetAttributeType()
            && nSize > 1
            && bDisplayNumber)
    {
        bResetNumName = true;
    }

    for (; nItemIndex < nMaxPageTotalCount && nDataIndex < nSize; ++nItemIndex, ++nDataIndex)
    {
        if (!CDirUIHelper::GetAttributeByIndex(nDataIndex, strName, strValue))
        {
            break;
        }

        bool bAdd = false;
        CDetailListItemPtr pDetailItem = (CDetailListItemPtr)m_pFrameList->GetItemByIndex(nItemIndex);

        if (NULL == pDetailItem)
        {
            bAdd = true;
            pDetailItem = new CDetailListItem;

            if (NULL == pDetailItem)
            {
                continue;
            }
            pDetailItem->setObjectName(QString("DetailItem%1").arg(nItemIndex));
            pDetailItem->SetItemHeight(NUMBER_SELECT_POP_BOX_ITEM_HEIGHT);
            pDetailItem->SetTitleRect(NUMBER_SELECT_POP_BOX_ITEM_TITLE_LEFT,
                                      NUMBER_SELECT_POP_BOX_ITEM_CONTENT_TOP,
                                      NUMBER_SELECT_POP_BOX_ITEM_TITLE_WEIGHT, NUMBER_SELECT_POP_BOX_ITEM_CONTENT_HEIGHT);

            pDetailItem->SetValueRect(NUMBER_SELECT_POP_BOX_ITEM_VALUE_LEFT,
                                      NUMBER_SELECT_POP_BOX_ITEM_CONTENT_TOP,
                                      NUMBER_SELECT_POP_BOX_ITEM_VALUE_WIDTH, NUMBER_SELECT_POP_BOX_ITEM_CONTENT_HEIGHT);
        }

        pDetailItem->ClearContent();
        pDetailItem->SetAction(this);
        pDetailItem->SetDataIndex(nDataIndex);

        if (bResetNumName)
        {
            QString strTitle = LANG_TRANSLATE(TRID_NUMBER);
            strTitle = strTitle + toQString(fmtString("%d", nItemIndex + 1));
            pDetailItem->SetTitle(strTitle);
        }
        else if (DIRECTORY_TYPE_LDAP == CDirUIHelper::GetAttributeType())
        {
            //不用翻译直接使用服务器推送属性
            QString strTitle = toQString(strName.c_str());
            pDetailItem->SetTitle(strTitle);
        }
        else
        {
            pDetailItem->SetTitle(LANG_TRANSLATE(strName.c_str()));
        }

        pDetailItem->SetValue(toQString(strValue, true));

        if (bAdd && !m_pFrameList->AddItem(pDetailItem))
        {
            pDetailItem->Release();
        }
    }

    int nItemAmount = m_pFrameList->GetTotalItemCount();

    for (int i = nItemIndex; i < nItemAmount; ++i)
    {
        m_pFrameList->DelItemByIndex(nItemIndex);
    }

    m_pFrameList->update();
}

void CNumberSelector::OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction)
{
    CFrameList * pList = (CFrameList *)pFrameList;

    if (NULL == pList || pList != m_pFrameList || NULL == pItem || LIST_ITEM_ACTION_SELECT != nAction)
    {
        return;
    }

    m_nSelIndex = pItem->GetDataIndex();

    CMessageBoxBase * pTopMsgBox = MessageBox_GetTopMessageBox();

    if (NULL != pTopMsgBox && pTopMsgBox == pFrameList->parent())
    {
        MessageBox_RemoveTopMessageBox(MessageBox_OK);
    }
}
