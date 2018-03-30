#include "iframelistdelegate.h"
#include <QLabel>
#include "framelistbase.h"
#include "qwidgetutility.h"

#define FRAME_ITEM_LEFT_MARGIN 0
#define FRAME_ITEM_RIGHT_MARGIN 0
#define FRAME_ITEM_TOP_MARGIN 0
#define FRAME_ITEM_SPACE 0

CFrameListBase::CFrameListBase(QWidget * parent)
    : QWidget(parent)
    , m_pLabNote(NULL)
    , m_nLeftMargin(FRAME_ITEM_LEFT_MARGIN)
    , m_nTopMargin(FRAME_ITEM_TOP_MARGIN)
    , m_nRightMargin(FRAME_ITEM_RIGHT_MARGIN)
    , m_nItemSpace(FRAME_ITEM_SPACE)
    , m_nContentLeft(0)
    , m_nContentRight(0)
    , m_bDefaultFocus(false)
    , m_bNoteFixPos(false)
    , m_pDelegate(NULL)
    , m_nFocusIndex(INVALID_ITEM_INDEX)
    , m_nPageItemCount(DEFAULT_PAGE_ITEM_COUNT)
    , m_nDataCount(0)
    , m_bAttachItem(false)
{

}

CFrameListBase::~CFrameListBase()
{
    if (NULL != m_pDelegate)
    {
        m_pDelegate->OnFrameListDetach(this);
        m_pDelegate = NULL;
    }
}

void CFrameListBase::SetRect(int nX, int nY, int nWidth, int nHeight)
{
    const QRect rect(nX, nY, nWidth, nHeight);

    SetRect(rect);
}

void CFrameListBase::SetRect(const QRect & rect)
{
    if (rect.isValid())
    {
        setGeometry(rect);

        AdjustChildWidget();

        update();
    }
}

void CFrameListBase::SetItemMargin(int nLeft, int nTop, int nRight)
{
    m_nLeftMargin = nLeft;
    m_nTopMargin = nTop;
    m_nRightMargin = nRight;
}

void CFrameListBase::SetItemSpace(int nSpace)
{
    m_nItemSpace = nSpace;
}

void CFrameListBase::SetContentPos(int nLeftMargin, int nRightMargin)
{
    m_nContentLeft = nLeftMargin;
    m_nContentRight = nRightMargin;
}

void CFrameListBase::SetDefaultFocus(bool bFocus)
{
    m_bDefaultFocus = bFocus;
}

void CFrameListBase::SetNote(const yl::string & strNote)
{
    // 后面改为toQString
    QString qsNote = QString::fromUtf8(strNote.c_str());

    SetNote(qsNote);
}

void CFrameListBase::SetNote(const QString & strNode)
{
    if (NULL == m_pLabNote)
    {
        if (strNode.isEmpty())
        {
            return;
        }

        m_pLabNote = new QLabel(this);

        if (NULL == m_pLabNote)
        {
            return;
        }
        QRect rcFrame = rect();
        // 设置显示文本宽度占大部分列表栏，才能正确居中
        m_pLabNote->setFixedWidth(rcFrame.width() - m_nLeftMargin - m_nRightMargin);
        m_pLabNote->setAlignment(Qt::AlignCenter);
        m_pLabNote->hide();
    }

    m_pLabNote->setText(strNode);
    RefreshNoteLabel();
}

void CFrameListBase::SetNoteFontColor(const QColor & color)
{
    if (NULL == m_pLabNote)
    {
        m_pLabNote = new QLabel(this);

        if (NULL == m_pLabNote)
        {
            return;
        }

        m_pLabNote->setAlignment(Qt::AlignCenter);
        m_pLabNote->hide();
    }

    QPalette objQPalette = m_pLabNote->palette();

    objQPalette.setColor(QPalette::Text, color);
    objQPalette.setColor(QPalette::WindowText, color);
    m_pLabNote->setPalette(objQPalette);
}

void CFrameListBase::SetNoteFontSize(int nFontSize, int nFontWeight/* = QFont::Normal*/)
{
    if (NULL == m_pLabNote)
    {
        m_pLabNote = new QLabel(this);

        if (NULL == m_pLabNote)
        {
            return;
        }

        m_pLabNote->setAlignment(Qt::AlignCenter);
        m_pLabNote->hide();
    }

    qWidgetUtility::setFontStyle(m_pLabNote, nFontSize, nFontWeight);
}

void CFrameListBase::SetNoteFixRect(int nLeft, int nTop, int nWidth, int nHeight)
{
    m_bNoteFixPos = true;

    if (NULL == m_pLabNote)
    {
        m_pLabNote = new QLabel(this);

        if (NULL == m_pLabNote)
        {
            return;
        }

        m_pLabNote->setAlignment(Qt::AlignCenter);
        m_pLabNote->hide();
    }

    m_pLabNote->setFixedWidth(nWidth);
    m_pLabNote->setFixedHeight(nHeight);
    m_pLabNote->move(nLeft, nTop);
}

void CFrameListBase::SetDelegate(IFrameListDelegate * pDelegate, bool bLoadData/* = false*/)
{
    if (pDelegate != m_pDelegate)
    {
        if (NULL != m_pDelegate && m_pDelegate->IsFrameListAttach(this))
        {
            m_pDelegate->OnFrameListDetach(this);
        }

        m_pDelegate = pDelegate;

        if (NULL != m_pDelegate && !m_pDelegate->IsFrameListAttach(this))
        {
            m_pDelegate->OnFrameListAttach(this);
        }
    }

    if (bLoadData)
    {
        RefreshData(0);
    }
}

void CFrameListBase::ResetDelegate()
{
    m_pDelegate = NULL;
}

void CFrameListBase::ResetAction()
{
    VEC_LIST_ITEM::iterator iter = m_vecItem.begin();

    for (; iter != m_vecItem.end(); ++iter)
    {
        CListItemPtr pListItem = *iter;

        if (NULL != pListItem)
        {
            pListItem->SetAction(NULL);
        }
    }
}

void CFrameListBase::RefreshData(int nDataIndex)
{
    if (NULL != m_pDelegate && m_nPageItemCount > 0)
    {
        m_pDelegate->OnLoadData(this, nDataIndex, m_nPageItemCount);
    }
}

void CFrameListBase::SetMaxPageItemCount(int nMaxCount)
{
    if (nMaxCount < 0)
    {
        nMaxCount = 0;
    }

    m_nPageItemCount = nMaxCount;

    int nSize = m_vecItem.size();

    if (m_nPageItemCount >= nSize)
    {
        return;
    }

    int nIndex = m_nPageItemCount - 1;

    for (int i = m_nPageItemCount; i < nSize; ++i)
    {
        DelItemByIndex(nIndex);
    }
}

int CFrameListBase::GetMaxPageItemCount()
{
    return m_nPageItemCount;
}

int CFrameListBase::GetTotalItemCount() const
{
    return m_vecItem.size();
}

void CFrameListBase::SetTotalDataCount(int nTotalCount)
{
    bool bNotify = false;

    if (m_nDataCount != nTotalCount)
    {
        bNotify = true;
    }

    m_nDataCount = nTotalCount;

    if (bNotify)
    {
        OnDataCountChange();
    }
}

int CFrameListBase::GetTotalDataCount() const
{
    return m_nDataCount;
}

void CFrameListBase::SetItemAttach()
{
    m_bAttachItem = true;
}

bool CFrameListBase::IsItemAttach() const
{
    return m_bAttachItem;
}

int CFrameListBase::GetFocusItemIndex() const
{
    return m_nFocusIndex;
}

void CFrameListBase::SelectItemByIndex(int nIndex)
{
    if (INVALID_ITEM_INDEX == nIndex)
    {
        DefocusItem();
        return;
    }

    CListItemPtr pItem = GetItemByIndex(nIndex);

    if (NULL == pItem)
    {
        return;
    }

    DefocusItem();

    m_nFocusIndex = nIndex;
    pItem->SelectItem(true);
}

void CFrameListBase::FocusItemByIndex(int nIndex)
{
    if (INVALID_ITEM_INDEX == nIndex)
    {
        DefocusItem();
        update();
        return;
    }

    CListItemPtr pItem = GetItemByIndex(nIndex);

    if (NULL == pItem)
    {
        return;
    }

    DefocusItem();

    m_nFocusIndex = nIndex;
    pItem->FocusItem(true);
}

void CFrameListBase::SetItemFocusByIndex(int nIndex)
{
    if (INVALID_ITEM_INDEX == nIndex)
    {
        DefocusItem();
        return;
    }

    CListItemPtr pItem = GetItemByIndex(nIndex);

    if (NULL == pItem)
    {
        return;
    }

    DefocusItem();

    m_nFocusIndex = nIndex;
    pItem->SetFocus(true);
}

int CFrameListBase::GetFocusItemDataIndex()
{
    CListItemPtr pItem = GetItemByIndex(m_nFocusIndex);

    if (NULL != pItem)
    {
        return pItem->GetDataIndex();
    }

    return INVALID_ITEM_INDEX;
}

void CFrameListBase::SelectItemByDataIndex(int nIndex)
{
    if (INVALID_ITEM_INDEX == nIndex)
    {
        DefocusItem();
        return;
    }

    CListItemPtr pItem = GetItemByDataIndex(nIndex);

    if (NULL == pItem)
    {
        return;
    }

    DefocusItem();

    m_nFocusIndex = GetIndexByItem(pItem);
    pItem->SelectItem(true);
}

void CFrameListBase::FocusItemByDataIndex(int nIndex)
{
    if (INVALID_ITEM_INDEX == nIndex)
    {
        DefocusItem();
        return;
    }

    CListItemPtr pItem = GetItemByDataIndex(nIndex);

    if (NULL == pItem)
    {
        return;
    }

    DefocusItem();

    m_nFocusIndex = GetIndexByItem(pItem);
    pItem->FocusItem(true);
}

void CFrameListBase::DefocusItem()
{
    if (INVALID_ITEM_INDEX == m_nFocusIndex)
    {
        return;
    }

    CListItemPtr pFocusItem = GetItemByIndex(m_nFocusIndex);

    if (NULL != pFocusItem)
    {
        pFocusItem->SetFocus(false);
    }

    m_nFocusIndex = INVALID_ITEM_INDEX;
}

void CFrameListBase::FocusLastItem()
{
    int nSize = m_vecItem.size();
    int nLastIndex = INVALID_ITEM_INDEX;

    if (nSize > 0)
    {
        nLastIndex = nSize - 1;
    }

    FocusItemByIndex(nLastIndex);
}

bool CFrameListBase::AddItem(CListItemPtr pListItem, bool bRefresh/* = false*/)
{
    if (NULL == pListItem)
    {
        return false;
    }

    int nSize = m_vecItem.size();

    if (nSize >= m_nPageItemCount)
    {
        return false;
    }

    pListItem->setParent(this);
    pListItem->SetDelegate(this);
    pListItem->raise();
    pListItem->show();

    m_vecItem.push_back(pListItem);

    if (0 == nSize)
    {
        OnDataIndexChange(pListItem);
    }

    AdjustChildWidget();
    RefreshNoteLabel();

    if (m_bDefaultFocus && INVALID_ITEM_INDEX == m_nFocusIndex)
    {
        FocusItemByIndex(0);
    }

    if (bRefresh)
    {
        update();
    }

    return true;
}

void CFrameListBase::ClearContent(bool bRefresh/* = false*/)
{
    if (m_bAttachItem)
    {
        DetachAllItem(bRefresh);
        return;
    }

    ClearAllItem();

    ResetListData();

    OnClearContent();
    RefreshNoteLabel();

    if (bRefresh)
    {
        update();
    }
}

void CFrameListBase::DetachAllItem(bool bRefresh/* = false*/)
{
    if (!m_bAttachItem)
    {
        return;
    }

    m_bAttachItem = false;
    VEC_LIST_ITEM::iterator iter = m_vecItem.begin();

    for (; iter != m_vecItem.end(); ++iter)
    {
        CListItemPtr pListItem = *iter;

        if (NULL != pListItem)
        {
            pListItem->setParent(NULL);
            pListItem->SetDelegate(NULL);
        }
    }

    m_vecItem.clear();

    ResetListData();

    OnClearContent();
    RefreshNoteLabel();

    if (bRefresh)
    {
        update();
    }
}

void CFrameListBase::ResetListData()
{
    m_nFocusIndex = INVALID_ITEM_INDEX;
    m_nDataCount = 0;
}

bool CFrameListBase::DelItemByIndex(int nIndex, bool bRefresh/* = false*/)
{
    int nSize = m_vecItem.size();

    if (nIndex < 0 || nIndex >= nSize)
    {
        return false;
    }

    CListItemPtr pListItem = m_vecItem[nIndex];

    if (NULL != pListItem)
    {
        if (m_bAttachItem)
        {
            pListItem->setParent(NULL);
            pListItem->SetDelegate(NULL);
        }
        else
        {
            pListItem->Release();
        }
    }

    m_vecItem.removeAt(nIndex);

    if (m_vecItem.size() <= 0 && m_bAttachItem)
    {
        m_bAttachItem = false;
    }

    AdjustChildWidget();
    RefreshNoteLabel();

    if (0 == m_vecItem.size())
    {
        m_nFocusIndex = INVALID_ITEM_INDEX;
        OnClearContent();
    }
    else if (nIndex <= m_nFocusIndex)
    {
        if (m_bDefaultFocus)
        {
            // 重新选中一行
            int nFocusIndex = m_nFocusIndex - 1;

            if (nFocusIndex < 0)
            {
                nFocusIndex = 0;
            }

            FocusItemByIndex(nFocusIndex);
        }
        else
        {
            m_nFocusIndex = INVALID_ITEM_INDEX;
        }
    }

    if (bRefresh)
    {
        update();
    }

    return true;
}

bool CFrameListBase::DelItemByDataIndex(int nIndex, bool bRefresh/* = false*/)
{
    CListItemPtr pItem = GetItemByDataIndex(nIndex);

    if (NULL == pItem)
    {
        return false;
    }

    int nDelIndex = GetIndexByItem(pItem);

    return DelItemByIndex(nDelIndex, bRefresh);
}

CListItemPtr CFrameListBase::GetItemByIndex(int nIndex)
{
    if (INVALID_ITEM_INDEX == nIndex)
    {
        return NULL;
    }

    int nSize = m_vecItem.size();

    if (nIndex < 0 || nIndex >= nSize)
    {
        return NULL;
    }

    return m_vecItem[nIndex];
}

CListItemPtr CFrameListBase::GetItemByDataIndex(int nIndex)
{
    if (INVALID_ITEM_INDEX == nIndex)
    {
        return NULL;
    }

    VEC_LIST_ITEM::iterator iter = m_vecItem.begin();

    for (; iter != m_vecItem.end(); ++iter)
    {
        CListItemPtr pItem = *iter;

        if (NULL != pItem && pItem->GetDataIndex() == nIndex)
        {
            return pItem;
        }
    }

    return NULL;
}

CListItemPtr CFrameListBase::GetFocusItem()
{
    if (INVALID_ITEM_INDEX == m_nFocusIndex)
    {
        return NULL;
    }

    return GetItemByIndex(m_nFocusIndex);
}

int CFrameListBase::GetIndexByItem(CListItemPtr pListItem)
{
    if (NULL == pListItem)
    {
        return INVALID_ITEM_INDEX;
    }

    int nIndex = INVALID_ITEM_INDEX;
    VEC_LIST_ITEM::iterator iter = m_vecItem.begin();

    for (int i = 0; iter != m_vecItem.end(); ++i, ++iter)
    {
        CListItemPtr pTmpItem = *iter;

        if (pTmpItem == pListItem)
        {
            nIndex = i;
            break;
        }
    }

    return nIndex;
}

int CFrameListBase::GetIndexByDataIndex(int nDataIndex)
{
    int nIndex = INVALID_ITEM_INDEX;
    VEC_LIST_ITEM::iterator iter = m_vecItem.begin();

    for (int i = 0; iter != m_vecItem.end(); ++i, ++iter)
    {
        CListItemPtr pTmpItem = *iter;

        if (NULL != pTmpItem && pTmpItem->GetDataIndex() == nDataIndex)
        {
            nIndex = i;
            break;
        }
    }

    return nIndex;
}

IFrameListDelegate * CFrameListBase::GetFrameDelegate() const
{
    return m_pDelegate;
}

bool CFrameListBase::IsCanReuseItem(int nItemType)
{
    if (0 == m_vecItem.size())
    {
        return true;
    }

    CListItemPtr pItem = m_vecItem[0];

    if (NULL != pItem && pItem->GetType() == nItemType)
    {
        return true;
    }

    return false;
}

bool CFrameListBase::IsAllSameItem(int nItemType)
{
    VEC_LIST_ITEM::iterator iter = m_vecItem.begin();

    for (; iter != m_vecItem.end(); ++iter)
    {
        CListItemPtr pItem = *iter;

        if (NULL == pItem || nItemType != pItem->GetType())
        {
            return false;
        }
    }

    return true;
}

bool CFrameListBase::IsHadConflictItem(int nItemType, int nMaskType)
{
    if (0 == m_vecItem.size())
    {
        return false;
    }

    CListItemPtr pItem = m_vecItem[0];

    if (NULL != pItem && (pItem->GetType() & nMaskType) == nItemType)
    {
        return false;
    }

    return true;
}

void CFrameListBase::GetContentPos(int & nLeftMargin, int & nRightMargin)
{
    nLeftMargin = m_nContentLeft;
    nRightMargin = m_nContentRight;
}

VEC_LIST_ITEM & CFrameListBase::GetListItem()
{
    return m_vecItem;
}

void CFrameListBase::ClearAllItem()
{
    VEC_LIST_ITEM::iterator iter = m_vecItem.begin();

    for (; iter != m_vecItem.end(); ++iter)
    {
        CListItemPtr pItem = *iter;

        if (NULL != pItem)
        {
            pItem->Release();
        }
    }

    m_vecItem.clear();
}

void CFrameListBase::SwitchPage(int nPageNo)
{
    int nMaxPageNo = (m_nDataCount + m_nPageItemCount - 1) / m_nPageItemCount - 1;

    if (INVALID_FRAME_LIST_PAGE_NO == nPageNo
            || nPageNo > nMaxPageNo)
    {
        return ;
    }

    int nIndex = nPageNo * m_nPageItemCount;

    m_pDelegate->OnLoadData(this, nIndex, m_nPageItemCount);
}

bool CFrameListBase::OnLastItemNext()
{
    if (NULL == m_pDelegate)
    {
        return false;
    }

    return m_pDelegate->OnLastItemNext();
}

bool CFrameListBase::OnFirstItemPrevious()
{
    if (NULL == m_pDelegate)
    {
        return false;
    }

    return m_pDelegate->OnFirstItemPrevious();
}

// 向前翻页
bool CFrameListBase::OnPrePage()
{
    if (NULL == m_pDelegate || m_nPageItemCount <= 0)
    {
        return false;
    }

    int nPageNo = GetCurPage();

    if (INVALID_FRAME_LIST_PAGE_NO == nPageNo)
    {
        return false;
    }

    // 有前一页数据
    if (0 != nPageNo)
    {
        int nIndex = (nPageNo - 1) * m_nPageItemCount;

        m_pDelegate->OnLoadData(this, nIndex, m_nPageItemCount);
        return true;
    }

    // 处理完第一页翻页
    if (OnFirstItemPrevious())
    {
        return true;
    }

    int nMaxPageNo = (m_nDataCount + m_nPageItemCount - 1) / m_nPageItemCount - 1;

    // 只有一页，不需要重新加载数据
    if (nPageNo == nMaxPageNo)
    {
        return false;
    }

    // 循环到最后一页
    int nIndex = nMaxPageNo * m_nPageItemCount;
    int nCount = m_nDataCount - nIndex;

    if (nCount > m_nPageItemCount)
    {
        nCount = m_nPageItemCount;
    }

    m_pDelegate->OnLoadData(this, nIndex, nCount);

    return true;
}

bool CFrameListBase::OnNextPage()
{
    if (NULL == m_pDelegate || m_nPageItemCount <= 0)
    {
        return false;
    }

    int nPageNo = GetCurPage();

    if (INVALID_FRAME_LIST_PAGE_NO == nPageNo)
    {
        return false;
    }

    int nMaxPageNo = (m_nDataCount + m_nPageItemCount - 1) / m_nPageItemCount - 1;

    // 只有一页，不需要重新加载数据
    if (0 == nPageNo && nPageNo == nMaxPageNo)
    {
        return false;
    }

    // 有下一页数据
    if (nPageNo != nMaxPageNo)
    {
        int nIndex = (nPageNo + 1) * m_nPageItemCount;
        int nCount = m_nDataCount - nIndex;

        if (nCount > m_nPageItemCount)
        {
            nCount = m_nPageItemCount;
        }

        m_pDelegate->OnLoadData(this, nIndex, nCount);
        return true;
    }

    // 处理完最后一页翻页
    if (OnLastItemNext())
    {
        return true;
    }

    // 循环到第一页
    if (m_nDataCount > m_nPageItemCount)
    {
        m_pDelegate->OnLoadData(this, 0, m_nPageItemCount);
    }
    else
    {
        m_pDelegate->OnLoadData(this, 0, m_nDataCount);
    }

    return true;
}

int CFrameListBase::GetCurPage()
{
    if (0 == m_vecItem.size() || m_nPageItemCount <= 0)
    {
        return INVALID_FRAME_LIST_PAGE_NO;
    }

    CListItemPtr pFirstItem = m_vecItem[0];

    if (NULL == pFirstItem)
    {
        return INVALID_FRAME_LIST_PAGE_NO;
    }

    int nDataIndex = pFirstItem->GetDataIndex();

    if (nDataIndex < 0)
    {
        return INVALID_FRAME_LIST_PAGE_NO;
    }

    int nPageNo = (nDataIndex + m_nPageItemCount) / m_nPageItemCount - 1;

    return nPageNo;
}

// 向前滚动一项
bool CFrameListBase::OnPreItem(bool bScrollByPage)
{
    if (NULL == m_pDelegate || m_nPageItemCount <= 0 || m_nDataCount <= 0)
    {
        return false;
    }

    CListItemPtr pFirstItem = GetItemByIndex(0);

    if (NULL == pFirstItem)
    {
        return false;
    }

    CListItemPtr pFocusItem = GetFocusItem();

    // 从新获取焦点时，前一项定为最后一项
    if (NULL == pFocusItem)
    {
        FocusLastItem();
        return true;
    }

    int nBeginDataIndex = pFirstItem->GetDataIndex();

    if (nBeginDataIndex < 0)
    {
        return false;
    }

    int nPreItemIndex = m_nFocusIndex - 1;

    // 非第一项的前一项时，往前移一项
    if (nPreItemIndex >= 0)
    {
        FocusItemByIndex(nPreItemIndex);
        return true;
    }

    // 第一项的前一项，重新定位
    int nFocusIndex = pFocusItem->GetDataIndex() - 1;

    // 已经是数据的第一项
    if (nFocusIndex < 0)
    {
        if (OnFirstItemPrevious())
        {
            return true;
        }

        // 有多页数据，重新加载最后一页
        if (m_nDataCount > m_nPageItemCount)
        {
            int nRemainder = m_nDataCount % m_nPageItemCount;

            if (bScrollByPage && 0 != nRemainder)
            {
                nBeginDataIndex = m_nDataCount - nRemainder/* + 1 - 1*/;
            }
            else
            {
                nBeginDataIndex = m_nDataCount - m_nPageItemCount/* + 1 - 1*/;
            }

            m_pDelegate->OnLoadData(this, nBeginDataIndex, m_nPageItemCount);
        }

        FocusLastItem();
        return true;
    }

    if (bScrollByPage)
    {
        OnPrePage();
        FocusLastItem();
    }
    else
    {
        // 前移一项
        m_pDelegate->OnLoadData(this, nFocusIndex, m_nPageItemCount);
        FocusItemByIndex(0);
    }

    return true;
}

// 向后滚动一项
bool CFrameListBase::OnNextItem(bool bScrollByPage)
{
    if (NULL == m_pDelegate || m_nPageItemCount <= 0 || m_nDataCount <= 0)
    {
        return false;
    }

    CListItemPtr pFirstItem = GetItemByIndex(0);

    if (NULL == pFirstItem)
    {
        return false;
    }

    CListItemPtr pFocusItem = GetFocusItem();

    // 从新获取焦点时，后一项定为第一项
    if (NULL == pFocusItem)
    {
        FocusItemByIndex(0);
        return false;
    }

    int nBeginIndex = pFirstItem->GetDataIndex();

    if (nBeginIndex < 0)
    {
        return false;
    }

    int nNextItemIndex = m_nFocusIndex + 1;
    int nItemCount = m_vecItem.size();

    // 非最后项的后一项时，往后移一项
    if (nNextItemIndex < nItemCount)
    {
        FocusItemByIndex(nNextItemIndex);
        return true;
    }

    // 最后项的后一项，重新定位
    int nFocusIndex = pFocusItem->GetDataIndex() + 1;

    // 已经是数据的最后项
    if (nFocusIndex >= m_nDataCount)
    {
        if (OnLastItemNext())
        {
            return true;
        }

        // 有多页数据，重新加载第一页
        if (m_nDataCount > m_nPageItemCount)
        {
            m_pDelegate->OnLoadData(this, 0, m_nPageItemCount);
        }

        FocusItemByIndex(0);
        return true;
    }

    if (bScrollByPage)
    {
        OnNextPage();
        FocusItemByIndex(0);
    }
    else
    {
        // 后移一项
        nBeginIndex += 1;
        m_pDelegate->OnLoadData(this, nBeginIndex, m_nPageItemCount);
        FocusLastItem();
    }

    return true;
}

void CFrameListBase::AdjustChildWidget()
{
    QRect rcFrame = geometry();

    int nItemTop = rcFrame.top() + m_nTopMargin;
    int nItemLeft = rcFrame.left() + m_nLeftMargin;
    int nItemWidth = rcFrame.width() - m_nLeftMargin - m_nRightMargin;

    if (NULL != m_pLabNote)
    {
        m_pLabNote->setGeometry(nItemLeft, 0, nItemWidth, rcFrame.height());
    }

    AdjustChildItem(nItemLeft, nItemTop, nItemWidth);
}

void CFrameListBase::AdjustChildItem(int nLeft, int nTop, int nWidth)
{
    VEC_LIST_ITEM::iterator iter = m_vecItem.begin();

    for (; iter != m_vecItem.end(); ++iter)
    {
        CListItemPtr pItem = *iter;

        if (NULL == pItem)
        {
            continue;
        }

        QRect rcItem = pItem->rect();
        int nItemHeight = rcItem.height();

        if (0 == nWidth)
        {
            rcItem.setRect(nLeft, nTop, rcItem.width(), nItemHeight);
        }
        else
        {
            rcItem.setRect(nLeft, nTop, nWidth, nItemHeight);
        }

        nTop += (nItemHeight + m_nItemSpace);
        pItem->setGeometry(rcItem);
    }
}

void CFrameListBase::RefreshNoteLabel()
{
    if (NULL == m_pLabNote)
    {
        return;
    }

    if (m_pLabNote->text().isEmpty() || 0 != m_vecItem.size())
    {
        m_pLabNote->hide();
        return;
    }

    m_pLabNote->show();
    m_pLabNote->raise();
}

void CFrameListBase::OnDataIndexChange(CListItem * pItem)
{

}

void CFrameListBase::OnItemFocusChange(CListItem * pItem, bool bPreFocus)
{
    if (NULL == pItem)
    {
        return;
    }

    CListItemPtr pFocusItem = GetItemByIndex(m_nFocusIndex);

    // 防止递归死循环，只处理获得焦点状态
    if (!pItem->IsFocus())
    {
        if (NULL != pFocusItem && !pFocusItem->IsFocus())
        {
            m_nFocusIndex = INVALID_ITEM_INDEX;
        }
        return;
    }

    if ((INVALID_ITEM_INDEX == m_nFocusIndex && NULL == pFocusItem)
            || (NULL != pFocusItem && pFocusItem != pItem))
    {
        m_nFocusIndex = GetIndexByItem(pItem);
    }

    int nSize = m_vecItem.size();

    for (int i = 0; i < nSize; ++i)
    {
        CListItemPtr pTmpItem = m_vecItem[i];

        if (NULL == pTmpItem || pTmpItem == pItem)
        {
            continue;
        }

        pTmpItem->FocusItem(false);
    }
}

void CFrameListBase::OnDataCountChange()
{

}

void CFrameListBase::OnClearContent()
{
    if (NULL != m_pDelegate)
    {
        m_pDelegate->OnCleanContent(this);
    }
}
