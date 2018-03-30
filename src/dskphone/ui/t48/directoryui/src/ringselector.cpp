#include <yllist.h>
#include "baseui/framelist.h"
#include "ringplayer.h"

#include "ringselector.h"

#include "directorycommon.h"
#include "translateiddefine.h"
#include "settingui/src/settingitembase.h"
#include "qtcommon/qmisc.h"
#include "uicommon/messagebox/modmessagebox.h"
#include "uikernel/languagehelper.h"
#include <resource/modresource.h>


typedef YLSmartPtr<BaseResItem> CRingDataPtr;
typedef YLList<CRingDataPtr> LIST_RING;
namespace
{
#define RING_SELECT_POP_BOX_ITEM_HEIGHT 47
}


CRingSelector::CRingSelector(void)
    : m_nId(0)
    , m_pRing(NULL)
    , m_nSelIndex(-1)
{
    m_bItemFocus = false;
    m_bDefaultFocus = false;
    m_pRing = new qRingPlayer(NULL);
}

CRingSelector::~CRingSelector(void)
{
    CMessageBoxBase * pTopMsgBox = MessageBox_GetTopMessageBox();

    if (NULL != pTopMsgBox && pTopMsgBox->GetEventFilter() == this)
    {
        pTopMsgBox->SetEventFilter(NULL);
    }

    if (NULL != m_pRing)
    {
        m_pRing->deleteLater();
        m_pRing = NULL;
    }
}

int CRingSelector::GetId() const
{
    return m_nId;
}

void CRingSelector::Init(int nId, const yl::string & strRingPath)
{
    m_vecRingName.clear();
    m_vecRingPath.clear();
    m_vecRingName.push_back(TRID_AUTO);
    m_vecRingPath.push_back("");

    m_nId = nId;
    m_nSelIndex = 0;

    yl::string strResType;
    LIST_RING lsRing;

    if (!Res_GetResType(DIR_RES_TYPE_RING, strResType)
            || 0 != res_getResItemsByResType(lsRing, strResType.c_str()))
    {
        return;
    }

    LIST_RING::iterator iter = lsRing.begin();

    for (int i = 0; iter != lsRing.end(); iter++, ++i)
    {
        CRingDataPtr & pItem = *iter;

        if (pItem.IsEmpty())
        {
            continue;
        }

        m_vecRingName.push_back(pItem->resItemDispLabel);
        m_vecRingPath.push_back(pItem->resPath);

        if (pItem->resPath == strRingPath)
        {
            m_nSelIndex = i + 1;
        }
    }

    if (m_nSelIndex < 0)
    {
        m_nSelIndex = 0;
    }
}

bool CRingSelector::GetRingPath(yl::string & strRingPath)
{
    if (m_nSelIndex < 0 || m_nSelIndex >= m_vecRingPath.size())
    {
        strRingPath = "";
        return false;
    }

    strRingPath = m_vecRingPath[m_nSelIndex];

    return true;
}

void CRingSelector::StopRingPlay()
{
    if (NULL != m_pRing)
    {
        m_pRing->StopRingPlay();
    }
}

void CRingSelector::OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount)
{
    int nFocus = -1;
    CFrameList * pList = (CFrameList *)pFrameList;

    if (NULL == pList || pList != m_pFrameList)
    {
        return;
    }

    if (NULL != m_pRing)
    {
        m_pRing->StopRingPlay();
    }

    if (!m_pFrameList->IsCanReuseItem(CSettingRadioItem::GetSettingRadioItemType()))
    {
        m_pFrameList->ClearContent();
    }

    int nItemIndex = 0;
    int nNameSize = m_vecRingName.size();
    int nPathSize = m_vecRingPath.size();
    int nSize = (nNameSize < nPathSize ? nNameSize : nPathSize);
    int nMaxPageItemCount = m_pFrameList->GetMaxPageItemCount();

    m_pFrameList->SetTotalDataCount(nSize);

    for (; nItemIndex < nMaxPageItemCount && nDataIndex < nSize; ++nItemIndex, ++nDataIndex)
    {
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

            pRadioItem->SetChangeStyleOnSelect(false);
            pRadioItem->setGeometry(0, 0, 0, RING_SELECT_POP_BOX_ITEM_HEIGHT);
        }

        pRadioItem->ClearContent();
        pRadioItem->CListItem::SetAction(this);
        pRadioItem->SetLabText(toQString(m_vecRingName[nDataIndex]));
        pRadioItem->SetDataIndex(nDataIndex);

        if (nDataIndex == m_nSelIndex)
        {
            nFocus = nDataIndex;
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
    //bug,http ://bugfree.yealink.com/Bug.php?BugID=131645
    //list类控件通过标识变量标识是否为焦点，而Qt控件不是同一方法，初步怀疑在过程中发生焦点转移，而list未能及时改变标识变量
    //用下面方法修复bug
    if (nFocus != -1)
    {
        ((CSettingRadioItem *)m_pFrameList->GetItemByIndex(nFocus))->SetFocus(true);
    }
    m_pFrameList->update();
}

void CRingSelector::OnFrameListAttach(CFrameListBase * pFrameList)
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

void CRingSelector::OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction)
{
    CFrameList * pList = (CFrameList *)pFrameList;

    if (NULL == pList || pList != m_pFrameList || NULL == pItem || LIST_ITEM_ACTION_SELECT != nAction)
    {
        return;
    }

    m_nSelIndex = pItem->GetDataIndex();

    if (NULL != m_pRing)
    {
        yl::string strRingPath;

        GetRingPath(strRingPath);

        if (strRingPath.empty())
        {
            m_pRing->StopRingPlay();
        }
        else
        {
            m_pRing->StartRingPlay(strRingPath, TIME_DELAY_PLAY_RING);
        }
    }
}

bool CRingSelector::ProcessKeyEvent(int nKeyCode, bool bPress, bool & bFilter)
{
    if (!bPress || NULL == m_pRing || !m_pRing->IsRinging())
    {
        return false;
    }

    if (PHONE_KEY_VOLUME_INCREASE != nKeyCode && PHONE_KEY_VOLUME_DECREASE != nKeyCode)
    {
        return false;
    }

    m_pRing->AdjustVolume(PHONE_KEY_VOLUME_INCREASE == nKeyCode);

    bFilter = true;
    return true;
}
