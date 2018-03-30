#include "cuiregiontextscreen.h"
#include "keymap.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "extended/xmlbrowser/include/modxmlbrowser.h"
#include "modxmlbrowserui.h"
#include "xmlbrowseruicommon.h"
#include "settingui/src/pingandtraceresultitem.h"
#include "baseui/include/listviewitemfactory.h"
#include "xwindow/xListView.h"
#include "xwindow/xWindowManager.h"

#if XML_BROWSER_SUPPORT_UI

namespace
{
#define XML_REGION_TEXTSCREEN_XML           "xmlbrowserui/xmlregiontextscreen.xml"
#define ITEM_TEXTSCREEN_WIDTH               244

#define SCROLL_PAGE_COUNT                   1

#ifdef _CP920
#define TEXT_FONT_SIZE                      8
#else
#if IS_COLOR
#define TEXT_FONT_SIZE                      8
#else
#define TEXT_FONT_SIZE                      10
#endif
#endif
}

////////////////////////////////////////////////////////////////////////////////
CUIRegionTextScreen::CUIRegionTextScreen()
    : CUIRegionBaseListScreen()
    , m_nCurrentPage(0)
{
    m_vecShowText.clear();
}

CUIRegionTextScreen::~CUIRegionTextScreen()
{

}

void CUIRegionTextScreen::Update(void * pData)
{
    CUIRegionBaseListScreen::Update(pData);
    if (NULL == pData)
    {
        return;
    }

    XBTextScreenData * pNewData = static_cast<XBTextScreenData *>(pData);

    if (NULL == m_pTxtTitle
            || NULL == m_pListView || NULL == pNewData)
    {
        return;
    }
    m_pListView->focusable = false;
    m_stBaseData.m_strTitle = pNewData->stAttr.strTitle;
    m_stBaseData.m_bIsWrapTitle = pNewData->stAttr.bWrapTitle;
    LoadMenuItemData(pNewData);
    m_iQuitTime = pNewData->stAttr.iTimeOut;
    m_bLock = pNewData->stAttr.bLockIn;
    SetSoftkey(pNewData->vecSoftkey);
}

void CUIRegionTextScreen::SetDefaultSoftkey(VecSoftkeyParam & softkeyParam)
{
    softkeyParam.clear();

    softkeyParam.push_back(TypeToSoftInfo(XB_ST_EXIT));
    softkeyParam.push_back(TypeToSoftInfo(XB_ST_EMPTY));
    softkeyParam.push_back(TypeToSoftInfo(XB_ST_EMPTY));
    softkeyParam.push_back(TypeToSoftInfo(XB_ST_EMPTY));
}

bool CUIRegionTextScreen::LoadMenuItemData(XBTextScreenData * pTextScreenData)
{
    if (NULL == pTextScreenData || NULL == m_pTxtTitle
            || NULL == m_pListView)
    {
        return false;
    }

    GetShowText(pTextScreenData->strText, m_vecShowText);
    return true;
}

void CUIRegionTextScreen::InitListViewLayout(xListView * pListView)
{
    CUIRegionBaseListScreen::InitListViewLayout(pListView);
    if (NULL == pListView)
    {
        return;
    }

    pListView->SetTotalDataCount(GetTotalPage());
}

xListViewItem * CUIRegionTextScreen::PrepareItem(xListView * pListView, int nDataIndex,
        xListViewItem * pItem)
{
    if (NULL == pListView || pListView != m_pListView)
    {
        return NULL;
    }

    CPingAndTraceoutItemPtr pResultItem = CPingAndTraceoutItem::GetPingAndTraceoutListItem(pItem);

    if (NULL == pResultItem)
    {
        pResultItem = static_cast<CPingAndTraceoutItemPtr>(g_ItemFactory.ApplyItem(
                          CPingAndTraceoutItem::GetPingAndTraceoutItemType()));

        if (NULL == pResultItem)
        {
            return NULL;
        }
    }

    pResultItem->SetAction(this);

    return pResultItem;
}

bool CUIRegionTextScreen::RefreshItem(xListView * pListView, xListViewItem * pItem)
{
    if (NULL == pListView || pListView != m_pListView)
    {
        return false;
    }

    //设置相应数据
    CPingAndTraceoutItemPtr pResultItem = CPingAndTraceoutItem::GetPingAndTraceoutListItem(pItem);

    if (NULL == pResultItem)
    {
        return false;
    }

    pResultItem->SetAction(this);

    int nDataIndex = pItem->GetDataIndex();
    yl::string strContent = GetValueByIndex(nDataIndex);
    pResultItem->SetValue(strContent);

    return true;
}

bool CUIRegionTextScreen::OnKeyPress(int nKey)
{
    if (CUIRegionBaseListScreen::OnKeyPress(nKey))
    {
        return true;
    }

    if (NULL == m_pListView || NULL == m_pScroll)
    {
        return false;
    }

    int nTotalPage = GetTotalPage();

    if (nTotalPage <= 1)
    {
        return false;
    }

    switch (nKey)
    {
    case PHONE_KEY_UP:
        {
            m_nCurrentPage--;
            if (m_nCurrentPage < 0)
            {
                m_nCurrentPage = nTotalPage - 1;
            }
            RefreshListView();
            m_pScroll->SetScrollInfo(SCROLL_PAGE_COUNT, nTotalPage, m_nCurrentPage);
            return true;
        }
        break;
    case PHONE_KEY_DOWN:
        {
            m_nCurrentPage++;
            if (m_nCurrentPage >= nTotalPage)
            {
                m_nCurrentPage = 0;
            }
            RefreshListView();
            m_pScroll->SetScrollInfo(SCROLL_PAGE_COUNT, nTotalPage, m_nCurrentPage);
            return true;
        }
        break;
    default:
        break;
    }
    return false;
}

yl::string CUIRegionTextScreen::GetValueByIndex(int nIndex)
{
    yl::string strValue = "";
    int nRealIndex = m_nCurrentPage + nIndex;
    if (nRealIndex < m_vecShowText.size() && nRealIndex >= 0)
    {
        strValue = m_vecShowText[nRealIndex];
    }

    return strValue;
}

int CUIRegionTextScreen::GetTotalPage()
{
    if (NULL == m_pListView)
    {
        return 0;
    }

    return m_vecShowText.size();
}

void CUIRegionTextScreen::GetShowText(const yl::string & strText, VEC_TEXT & vecText)
{
    if (strText.empty())
    {
        return;
    }

    vecText.clear();
    xFont font;
    font.setPointSize(TEXT_FONT_SIZE);

    yl::wstring wstr = yl::to_utf16(strText);
    typedef YLVector<yl::wstring> LINES;
    YLVector<yl::wstring> vecLines = yl::string_split<yl::wstring::value_type>(wstr, L"\n", true);

    int iRow = 0;
    int iFirstCharPos = 0;
    for (LINES::iterator begin = vecLines.begin(); begin != vecLines.end(); ++begin)
    {
        yl::wstring & str = *begin;
        while (!str.empty())
        {
            if (ITEM_TEXTSCREEN_WIDTH < font.textBound(yl::to_utf8(str)).cx)
            {
                yl::wstring::size_type upper_pos = str.size();
                yl::wstring::size_type lower_pos = 0;
                yl::wstring::size_type mid_pos = (upper_pos + lower_pos) >> 1;
                yl::string paintText;
                int nLoop = 0;
                //二分查找合适位置
                while (lower_pos != mid_pos)
                {
                    DEAD_LOOP_BREAK(nLoop);

                    paintText = yl::to_utf8(str.substr(0, mid_pos));
                    int nTextSize = font.textBound(paintText).cx;

                    if (nTextSize <= ITEM_TEXTSCREEN_WIDTH)
                    {
                        lower_pos = mid_pos;
                    }
                    else
                    {
                        upper_pos = mid_pos;
                    }

                    mid_pos = (upper_pos + lower_pos) >> 1;
                }

                //分词换行
                yl::wstring strLine(str, 0, lower_pos);

                //一个字符都显示不下
                if (strLine.empty())
                {
                    break;
                }

                vecText.push_back(yl::to_utf8(strLine));
                str.erase(0, strLine.size());
            }
            else
            {
                vecText.push_back(yl::to_utf8(str));
                break;
            }
        }
    }
}

void CUIRegionTextScreen::RelayoutByContent()
{
    CUIRegionBaseListScreen::RelayoutByContent();
}

#endif
