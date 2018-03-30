#include "searchwidget.h"
#include "qtcommon/qmisc.h"
#include "baseui/t4xpicpath.h"
#include "contacts/directory/include/commondata.h"
#include "contacts/supersearch/include/modsupersearch.h"
#include "uicommon/uimanager/rectdefine.h"
#include "configiddefine/configiddefine.h"
#include "configparser/modconfigparser.h"
#include "commonunits/modcommonunits.h"

namespace
{
// 最多显示几个列表项
#define SEARCH_WIGDET_MAX_COUNT     6
// 列表的总高度和宽度
#define SEARCH_WIGDET_WITH          272
#define SEARCH_WIGDET_HEIGHT        336
// 一个列表项的高度
#define SEARCH_ITEM_HEIGHT          56
// 延迟搜索的计时器时间
#define SEARCH_TIMER_OUT            300
}

//////////////////////////////////////////////////////////////////////////
QString PresenceTypeToPic(_PRESENCE_TYPE eType)
{
    QString strPic = "";

    //是否显示新状态图标
    bool bNewIconMode = configParser_GetConfigInt(kszPresenceIconMode) == 1;

    switch (eType)
    {
    case _PRESENCE_TYPE_ONLINE:
        strPic = bNewIconMode ? PIC_ICON_PRESENCE_NEW_ONLINE : PIC_ICON_PRESENCE_ONLINE;
        break;
    case _PRESENCE_TYPE_CHATTY:
        strPic = PIC_ICON_PRESENCE_ONLINE;
        break; // 无
    case _PRESENCE_TYPE_AWAY:
        strPic = bNewIconMode ? PIC_ICON_PRESENCE_NEW_AWAY : PIC_ICON_PRESENCE_AWAY;
        break;
    case _PRESENCE_TYPE_EXTENDED_AWAY:
        strPic = PIC_ICON_PRESENCE_AWAY;
        break;// 无
    case _PRESENCE_TYPE_BUSY:
        strPic = bNewIconMode ? PIC_ICON_PRESENCE_NEW_BUSY : PIC_ICON_PRESENCE_BUSY;
        break;
    case _PRESENCE_TYPE_OFFLINE:
        strPic = bNewIconMode ? PIC_ICON_PRESENCE_NEW_OFFLINE : PIC_ICON_PRESENCE_OFFLINE;
        break;
    }

    return strPic;
}

////////////////////////////////////////////////////////////////////////////////////////////
CSearchTypeDelegate::CSearchTypeDelegate()
{
    m_bItemFocus = false;
    m_bDefaultFocus = false;
    m_dataSelect = SELECT_DATA_UPSERSEARCH;
}

CSearchTypeDelegate::~CSearchTypeDelegate(void)
{
}

void CSearchTypeDelegate::OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount)
{
    CFrameList* pList = (CFrameList *)pFrameList;
    if (NULL == pList || pList != m_pFrameList)
    {
        return;
    }

    ListCustomItem objData;
    int iMaxSize = GetShowData(objData, nDataIndex, nDataCount);

    pList->SetTotalDataCount(iMaxSize);

    int nItemIndex = 0;
    int nMaxPageTotalCount = pList->GetMaxPageItemCount();
    ListCustomItem::const_iterator itBegin = objData.begin();

    for (; nItemIndex < nMaxPageTotalCount && itBegin != objData.end(); ++nItemIndex, ++itBegin)
    {
        bool bAdd = false;
        CSearchListItemPtr pSearchItem = (CSearchListItemPtr)pList->GetItemByIndex(nItemIndex);

        if (NULL == pSearchItem)
        {
            bAdd = true;
            pSearchItem = new CSearchListItem;

            if (NULL == pSearchItem)
            {
                continue;
            }

            pSearchItem->SetItemHeight(SEARCH_ITEM_HEIGHT);
        }

        InitSearchItem(pSearchItem, *itBegin, nDataIndex + nItemIndex);

        if (bAdd && !pList->AddItem(pSearchItem))
        {
            pSearchItem->Release();
        }
    }

    int nItemAmount = pList->GetTotalItemCount();
    for (int i = nItemIndex; i < nItemAmount; ++i)
    {
        pList->DelItemByIndex(nItemIndex);
    }

    pList->update();
}

void CSearchTypeDelegate::OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction)
{
    if (LIST_ITEM_ACTION_SELECT != nAction)
    {
        return;
    }

    CSearchTable* pList = (CSearchTable *)pFrameList;
    if (NULL == pList)
    {
        return ;
    }

    int iDataIndex = pItem->GetDataIndex();
    SearchItemData itemData = GetSearchData(iDataIndex);
    pList->SendClickEvent(iDataIndex, itemData.strNumber, itemData.varData);
}

// 设置用户自定义项列表
void CSearchTypeDelegate::SetCustomList(const ListCustomItem & listCustomItem)
{
    m_listCustomItem = listCustomItem;

    if (m_listCustomItem.size() > 0
            && GetSearchString() == ""
            && SELECT_DATA_UPSERSEARCH == m_dataSelect)
    {
        SetSelectData(SELECT_DATA_CUSTOMLIST);
    }
}

bool CSearchTypeDelegate::IsCustomListChanged(ListCustomItem & listCustomItem)
{
    if (listCustomItem.size() != m_listCustomItem.size())
    {
        return true;
    }

    ListCustomItem::iterator itTarget = listCustomItem.begin();
    ListCustomItem::iterator itSource = m_listCustomItem.begin();
    for (; itTarget != listCustomItem.end(); ++itTarget, ++itSource)
    {
        if ((*itTarget) != (*itSource))
        {
            return true;
        }
    }

    return false;
}

SearchItemData CSearchTypeDelegate::GetSearchData(int iDataIndex)
{
    ListCustomItem objData;
    GetShowData(objData, iDataIndex, 1);

    if (objData.size() == 1)
    {
        return *objData.begin();
    }

    return SearchItemData();
}

yl::string CSearchTypeDelegate::GetSearchString()
{
    CSearchTable* pSearchTable = (CSearchTable*)m_pFrameList;
    return (pSearchTable ? pSearchTable->GetSearchString() : "");
}

int CSearchTypeDelegate::GetShowData(ListCustomItem& objData, int nDataIndex, int nDataCount)
{
    if (SELECT_DATA_CUSTOMLIST == m_dataSelect)
    {
        return GetCustomListData(objData, nDataIndex, nDataCount);
    }

    if (SELECT_DATA_MERGE == m_dataSelect)
    {
        return GetMergeData(objData, nDataIndex, nDataCount);
    }

    return GetSuperSearchData(objData, nDataIndex, nDataCount);
}

// 获取自定义的数据
int CSearchTypeDelegate::GetCustomListData(ListCustomItem& objData, int nDataIndex, int nDataCount)
{
    ListCustomItem::iterator itr = m_listCustomItem.begin();
    for (int i = 0; itr != m_listCustomItem.end(); ++i, ++itr)
    {
        if (i < nDataIndex)
        {
            continue;
        }

        if (i >= nDataIndex + nDataCount)
        {
            break;
        }

        objData.push_back(*itr);
    }

    return m_listCustomItem.size();
}

int CSearchTypeDelegate::GetSuperSearchData(ListCustomItem& objData, int nDataIndex, int nDataCount)
{
    SuperSearchItemListData itemListData;
    itemListData.m_nTotalCount = 0;
    itemListData.m_nIndexFrom = nDataIndex;
    itemListData.m_nCountToGet = nDataCount;
    itemListData.m_strSearch = GetSearchString();
    SuperSearch_GetContactListData(itemListData, itemListData.m_strSearch);

    YLList<DirectorySearchData>::const_iterator itr = itemListData.m_listContact.begin();
    for (; itr != itemListData.m_listContact.end(); ++itr)
    {
        SearchItemData itemData;
        itemData.strDisplay = toQString((*itr).m_strName);
        itemData.strNumber = toQString((*itr).m_strNumber);
        itemData.strContactPic = toQString((*itr).m_strPic);
        itemData.strPresencePic = PresenceTypeToPic((*itr).m_eStatus);
        itemData.varData = QVariant(-1);
        objData.push_back(itemData);
    }

    return itemListData.m_nTotalCount;
}

int CSearchTypeDelegate::GetMergeData(ListCustomItem& objData, int nDataIndex, int nDataCount)
{
    int iMaxSize = 0;

    // 先取穿梭的数据
    ListCustomItem objCustomData;
    iMaxSize = GetCustomListData(objCustomData, nDataIndex, nDataCount);

    // 再取搜索的数据
    ListCustomItem objSuperData;
    int iRemainSize = nDataCount - objCustomData.size();
    iRemainSize = (iRemainSize > 0) ? iRemainSize : 0;
    int iRemainIndex = nDataIndex - m_listCustomItem.size();
    iRemainIndex = (iRemainIndex > 0) ? iRemainIndex : 0;
    // 即使该页不需要搜索数据也要调接口，目的为得到数量支持滚动条的显示
    iMaxSize += GetSuperSearchData(objSuperData, iRemainIndex, iRemainSize);

    objData = objCustomData;

    // 需要搜索数据才放入目标容器
    if (iRemainSize != 0)
    {
        ListCustomItem::iterator itr = objSuperData.begin();
        for (; itr != objSuperData.end(); ++itr)
        {
            objData.push_back(*itr);
        }
    }

    return iMaxSize;
}

void CSearchTypeDelegate::InitSearchItem(CSearchListItemPtr pSearchItem, const SearchItemData& objItemData, int nIndex)
{
    if (NULL == pSearchItem)
    {
        return ;
    }

    pSearchItem->ClearContent();
    pSearchItem->SetAction(this);
    pSearchItem->SetDataIndex(nIndex);

    // 设置名字和号码
    QString strDisplayName = objItemData.strDisplay;
    QString strLabelNumber = objItemData.strNumber;

    if (strDisplayName.isEmpty())
    {
        strDisplayName = strLabelNumber;
    }

    if (strDisplayName != strLabelNumber)
    {
        pSearchItem->SetTitleRect(58, 9, 160, 20);
        pSearchItem->SetValueRect(58, 29, 160, 20);
        pSearchItem->SetValueFontSize(14);

        pSearchItem->SetTitle(strDisplayName, false);
        pSearchItem->SetValue(strLabelNumber);
    }
    else
    {
        pSearchItem->SetTitleRect(58, 0, 160, 50);

        pSearchItem->SetTitle(strDisplayName, false);
        pSearchItem->SetValue(toQString(""));
    }

    pSearchItem->SetValueColor(Qt::white);
    pSearchItem->SetTitleColor(Qt::white);

    // 设置头像
    yl::string strPicTemp = fromQString(objItemData.strContactPic);
    commonUnits_Hide2Real(strPicTemp, PHFILE_TYPE_CONTACT_IMAGE);
    pSearchItem->SetHeadPortraitRect(6, 7, 39, 39);
    pSearchItem->SetHeadPortrait(toQString(strPicTemp));

    // 设置状态图标
    if (configParser_GetConfigInt(kszPresenceIconMode) == 1)
    {
        pSearchItem->SetStatePicRect(30, 30, 16, 16);
    }
    else
    {
        pSearchItem->SetStatePicRect(37, 38, 16, 16);
    }

    pSearchItem->SetStatePicutre(objItemData.strPresencePic);
}

////////////////////////////////////////////////////////////////////////////////////
CSearchTable::CSearchTable(QWidget * parent)
    : CFrameList(parent)
    , m_iDataIndex(-1)
{
    m_pSearchDelegate = new CSearchTypeDelegate;
    SetDelegate(m_pSearchDelegate);

    InitWidget();
    ClearCustomList();
}

CSearchTable::~CSearchTable()
{
    if (NULL != m_pSearchDelegate)
    {
        delete m_pSearchDelegate;
        m_pSearchDelegate = NULL;
    }

    SetDelegate(NULL);
    m_tmrSearch.stop();
}

void CSearchTable::InitWidget()
{
    setObjectName("SearchTable");

    SetScrollShow(SCROLL_SHOW_ONE_PAGE);
    SetScrollViewType(SCROLL_VIEW_TYPE_PAGE);
    SetMaxPageItemCount(SEARCH_WIGDET_MAX_COUNT);
    SetItemClickBG(PIC_DIRECTORY_SEARCH_ITEM_SEL_TOP);
    setGeometry(0, 0, SEARCH_WIGDET_WITH, SEARCH_WIGDET_HEIGHT);

    // 初始化搜索计时器.
    connect(&m_tmrSearch, SIGNAL(timeout()), this, SLOT(OnTimeroutSearch()));
}

// 直接设置用户自定义项列表
void CSearchTable::SetCustomList(const ListCustomItem & listCustomItem)
{
    if (NULL != m_pSearchDelegate)
    {
        m_pSearchDelegate->SetCustomList(listCustomItem);

        if (m_strSearch == "")
        {
            RefreshData(0);
        }
    }
}

bool CSearchTable::IsCustomListChanged(ListCustomItem & listCustomItem)
{
    return m_pSearchDelegate ? m_pSearchDelegate->IsCustomListChanged(listCustomItem) : true;
}

// 清空用户自定义项列表
void CSearchTable::ClearCustomList()
{
    if (NULL != m_pSearchDelegate)
    {
        ListCustomItem listCustomItem;
        m_pSearchDelegate->SetCustomList(listCustomItem);
    }
}

void CSearchTable::ClearData(bool bClearCustomList/* = true*/)
{
    m_tmrSearch.stop();
    m_strSearch = "";

    if (bClearCustomList)
    {
        ClearCustomList();
    }

    // 界面刷空
    SetItemAttach();
    DetachAllItem(true);
}

void CSearchTable::SetMergeData(bool bMergeData)
{
    if (NULL != m_pSearchDelegate)
    {
        SELECT_DATA nType = bMergeData ? SELECT_DATA_MERGE : SELECT_DATA_UPSERSEARCH;
        m_pSearchDelegate->SetSelectData(nType);
    }
}

bool CSearchTable::IsSelectItem()
{
    return (-1 != m_iDataIndex);
}

SearchItemData CSearchTable::GetCurrentItemContent()
{
    if (NULL != m_pSearchDelegate && IsSelectItem())
    {
        return m_pSearchDelegate->GetSearchData(m_iDataIndex);
    }

    // 返回空字符串
    return SearchItemData();
}

void CSearchTable::SendClickEvent(int iDataIndex, const QString& strNumber, const QVariant& varData)
{
    m_iDataIndex = iDataIndex;

    // 发出Click点击事件
    emit clickItem(strNumber, varData);

    m_iDataIndex = -1;
}

// 按照所选字符串进行搜索第N页的内容
void CSearchTable::Search(LPCSTR lpszSearchText)
{
    TALKUI_INFO("CSearchTable::Search [%s][%s][%d]", m_strSearch.toUtf8().data(), lpszSearchText, IsSelectItem());

    // 选中设置编辑框文本改变导致调用此接口，此场景不处理
    if (IsSelectItem())
    {
        return ;
    }

    // 清数据
    ClearData(!m_strSearch.isEmpty());

    m_strSearch = lpszSearchText;

    if (NULL != m_pSearchDelegate && !m_strSearch.isEmpty())
    {
        m_pSearchDelegate->SetSelectData(SELECT_DATA_UPSERSEARCH);
    }

    // 重新启动计时器.
    m_tmrSearch.start(SEARCH_TIMER_OUT);
}

// 搜索逻辑会卡住 所以这边判断一下key值
void CSearchTable::ShowSearchPageByKeyAnIndex(const yl::string & strKey, int nPageIndex)
{
    // 搜索返回的内容,从头开始显示
    if (strKey == fromQString(m_strSearch))
    {
        RefreshData(0);
    }
}

yl::string CSearchTable::GetSearchString()
{
    return m_strSearch.toUtf8().data();
}

void CSearchTable::OnTimeroutSearch()
{
    TALKUI_INFO("CSearchTable::OnTimeroutSearch begin");

    // 首先停掉计时器.
    if (m_tmrSearch.isActive())
    {
        m_tmrSearch.stop();
    }

    SuperSearchItemListData objSearchListData;
    objSearchListData.m_strSearch = fromQString(m_strSearch);
    objSearchListData.m_nCountToGet = SEARCH_WIGDET_MAX_COUNT;
    objSearchListData.m_nIndexFrom = 0;
    objSearchListData.m_nTotalCount = 0;

    bool bSucess = SuperSearch_ReqSearch(objSearchListData.m_strSearch);
    TALKUI_INFO("SuperSearch_ReqSearch [%s] result %s ", objSearchListData.m_strSearch.c_str(), bSucess ? "true" : "false");
}

