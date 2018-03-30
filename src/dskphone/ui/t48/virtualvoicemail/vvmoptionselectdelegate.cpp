#include "vvmoptionselectdelegate.h"
#include "vvmfileplayercontroller.h"

#include "baseui/framelist.h"
#include "settingui/src/settingitembase.h"
#include "uicommon/messagebox/selectmessagebox.h"

CVVMOptionSelectDelegate::CVVMOptionSelectDelegate(QObject * pParent)
    : QObject(pParent)
    , m_nSelIndex(0)
{

}

CVVMOptionSelectDelegate::~CVVMOptionSelectDelegate()
{

}

void CVVMOptionSelectDelegate::InitOptionData()
{
    connect(this, SIGNAL(DialogAction(QString)), &g_VVMController, SLOT(OnPlayerAction(QString)));
}

void CVVMOptionSelectDelegate::OnLoadData(CFrameListBase * pFrameList, int nDataIndex,
        int nDataCount)
{
    CFrameList * pList = (CFrameList *)pFrameList;

    if (NULL == pList || pList != m_pFrameList)
    {
        return;
    }

    if (!m_pFrameList->IsCanReuseItem(LIST_ITEM_TYPE_SETTING))
    {
        m_pFrameList->ClearContent();
    }

    bool bGetPairResult = false;
    const vecVVMItemPair & vecOptionData = g_VVMController.GetOptionData(bGetPairResult);
    if (!bGetPairResult)
    {
        return;
    }

    int nItemIndex = 0;
    int nSize = vecOptionData.size();
    int nMaxPageItemCount = m_pFrameList->GetMaxPageItemCount();
    m_pFrameList->SetTotalDataCount(nSize);

    for (int i = 0; nItemIndex < nMaxPageItemCount && i < nSize; ++i)
    {
        if (i < nDataIndex)
        {
            continue;
        }

        bool bAdd = false;
        CSettingItem * pItem = (CSettingItem *)m_pFrameList->GetItemByIndex(nItemIndex);

        if (NULL == pItem)
        {
            bAdd = true;
            pItem = new CSettingItem;

            if (NULL == pItem)
            {
                continue;
            }

            pItem->setGeometry(0, 0, 0, SELECT_POP_BOX_POS_ITEM_HEIGHT);
            pItem->SetChangeStyleOnPress(true);
            pItem->SetChangeStyleOnSelect(false);
        }

        int nItemDataIndex = i;

        pItem->ClearContent();
        pItem->CListItem::SetAction(this);

        pItem->SetLabText(vecOptionData[i].m_strTitle1);
        pItem->SetDataIndex(nItemDataIndex);

        if (bAdd && !m_pFrameList->AddItem(pItem))
        {
            pItem->Release();
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

void CVVMOptionSelectDelegate::OnItemAction(CFrameListBase * pFrameList, CListItem * pItem,
        int nAction)
{
    CFrameList * pList = (CFrameList *)pFrameList;

    if (NULL == pList || pList != m_pFrameList || NULL == pItem || LIST_ITEM_ACTION_SELECT != nAction)
    {
        return;
    }

    m_nSelIndex = pItem->GetDataIndex();
    MessageBox_RemoveMessageBox(this, MessageBox_OK);
}

void CVVMOptionSelectDelegate::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    bool bGetPairResult = false;
    const vecVVMItemPair & vecOptionData = g_VVMController.GetOptionData(bGetPairResult);
    if (!bGetPairResult)
    {
        return;
    }

    if (m_nSelIndex < vecOptionData.size() && MessageBox_OK == pMessageBox->GetResult())
    {
        emit DialogAction(vecOptionData[m_nSelIndex].m_strAction);
    }

    disconnect(this, SIGNAL(DialogAction(QString)), &g_VVMController, SLOT(OnPlayerAction(QString)));
}
