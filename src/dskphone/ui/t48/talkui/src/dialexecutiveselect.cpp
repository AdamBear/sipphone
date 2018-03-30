#include "dialexecutiveselect.h"
#include "broadsoft/executiveassistant/include/modexecutiveassistant.h"
#include "baseui/framelist.h"
#include "settingui/src/settingitembase.h"
#include "setting/include/common.h"
#include "directoryui/include/moddirectoryui.h"
#include "uicommon/messagebox/selectmessagebox.h"
#include "uikernel/languagehelper.h"
#include "presence/include/presence_inc.h"
#include "uicommon/qtcommon/qmisc.h"
#include "talk/talklogic/include/modtalklogic.h"

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT

CExecutiveSelector::CExecutiveSelector()
    : m_nSelIndex(0), m_iSessionId(0)
{
}

void CExecutiveSelector::Init(int iLineId, int iSessionId)
{
    m_nSelIndex = 0;
    m_iLineId = iLineId;
    m_iSessionId = iSessionId;
    m_vecData.clear();
    m_vecDspData.clear();

    CAssCommon dataInfo;
    if (BWExeAssis_GetData(&dataInfo, iLineId))
    {
        ExeAssisList & listData = dataInfo.listData;
        for (ExeAssisListIt it = listData.begin(); it != listData.end(); ++it)
        {
            m_vecData.push_back(it->ItemName.strExt);
            m_vecDspData.push_back(toQString(it->ItemName.strUserId));
        }
    }

    m_vecDspData.push_back(LANG_TRANSLATE(TRID_CANCEL_SELECT));
}


void CExecutiveSelector::OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount)
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
    // warring: size + 1 添加一项none用于取消选择
    int nSize = m_vecDspData.size();
    int nMaxPageItemCount = m_pFrameList->GetMaxPageItemCount();

    m_pFrameList->SetTotalDataCount(nSize);

    for (int i = 0; nItemIndex < nMaxPageItemCount && i < nSize; ++i)
    {
        if (i < nDataIndex)
        {
            continue;
        }

        bool bAdd = false;
        CSettingRadioItem * pRadioItem = (CSettingRadioItem *)m_pFrameList->GetItemByIndex(nItemIndex);

        if (NULL == pRadioItem)
        {
            bAdd = true;
            pRadioItem = new CSettingRadioItem;

            pRadioItem->setGeometry(0, 0, 0, SELECT_POP_BOX_POS_ITEM_HEIGHT);
            pRadioItem->SetChangeStyleOnPress(true);
            pRadioItem->SetChangeStyleOnSelect(false);
        }

        int nItemDataIndex = i;
        pRadioItem->ClearContent();
        pRadioItem->CListItem::SetAction(this);
        pRadioItem->SetLabText(m_vecDspData[i]);
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

void CExecutiveSelector::OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction)
{
    CFrameList * pList = (CFrameList *)pFrameList;
    if (NULL == pList
            || pList != m_pFrameList
            || NULL == pItem
            || LIST_ITEM_ACTION_SELECT != nAction)
    {
        return;
    }

    m_nSelIndex = pItem->GetDataIndex();
}

void CExecutiveSelector::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    TALKUI_INFO("CExecutiveSelector::MessageBoxCallBack ret=[%d] select=[%d]", pMessageBox->GetResult(),
                m_nSelIndex);
    if (MessageBox_OK == pMessageBox->GetResult())
    {
        int iSize = m_vecData.size();
        if (m_nSelIndex >= 0 && m_nSelIndex <= iSize)
        {
            bool bSelectNone = (m_nSelIndex == iSize);
            yl::string strNumber =  bSelectNone ? "" : m_vecData[m_nSelIndex];

            BWExeAssis_SetCallInitiationNumber(strNumber, m_iLineId);
            TALKUI_INFO("CExecutiveSelector none=[%d] strNumber(%s)", bSelectNone, strNumber.c_str());

            talklogic_UIEvent(m_iSessionId, TALK_ACTION_CALL_INITIATION, bSelectNone ? 0 : 1);
        }
    }
    else
    {
        m_vecData.clear();
    }
}
#endif
