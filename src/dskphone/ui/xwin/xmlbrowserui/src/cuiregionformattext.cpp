#include "cuiregionformattext.h"
#include "keymap.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "extended/xmlbrowser/include/modxmlbrowser.h"
#include "modxmlbrowserui.h"
#include "xmlbrowseruicommon.h"
#include "baseui/include/menulistitem.h"
#include "baseui/include/listviewitemfactory.h"
#include "xwindow/xListView.h"
#include "xwindow/xWindowManager.h"
#include "idlescreen/include/modidlescreen.h"
#include "idlescreenui/iconlist.h"
#include "idleuilogic/include/baseidlehandle.h"
#include "settinguilogic/include/modsetuilogic.h"
#include "xwindow/xSoftkeyBar.h"


#if XML_BROWSER_SUPPORT_UI

namespace
{
#define XML_REGION_FORMATTEXT_XML               "xmlbrowserui/xmlregionformat.xml"
#define XML_SCREENSAVERS_UI_LAYOUT_FILE         "screensaver/xmlscreensavers.xml"
#define XML_SCREENSAVERS_CLOCK                  "datetime"
#define XML_SCREENSAVERS_IMAGE                  "image"
#define XML_SCREENSAVERS_TITLEICON              "statusIconList"
#define XML_REGION_FORMATTEXT_LIST_HEAD         "listHead"
#define XML_REGION_FORMATTEXT_LIST_VIEW         "listView"
#define XML_REGION_FORMATTEXT_LIST_FOOT         "listFoot"
#define XML_SCREENSAVERS_FRAMETOP               "frmtop"
#define XML_SCREENSAVERS_FRAMEBOTTOM            "frmbottom"
#define XML_REGION_FORMATTEXT_LIST_ITEM_HEIGHT  19
#define XML_REGION_FORMATTEXT_ITEM_MAX          5
#define XML_REGION_FORMATTEXT_HEAD_MAX          2               // 暂只支持两行       
#define XML_REGION_FORMATTEXT_FOOT_MAX          1               // 暂只支持一行
}


////////////////////////////////////////////////////////////////////////////////
CUIRegionFormatText::CUIRegionFormatText(bool bScreenSaver/* = false*/)
    : CUIRegionBase()
    , m_pListHead(NULL)
    , m_pListView(NULL)
    , m_pListFoot(NULL)
    , m_pAdapterHead(NULL)
    , m_pAdapterFoot(NULL)
    , m_nHeadCount(0)
    , m_nFootCount(0)
    , m_nSelect(0)
    , m_bScreenSaver(bScreenSaver)
    , m_pClockLab(NULL)
    , m_pImage(NULL)
    , m_pTitleIcons(NULL)
    , m_pFrameTop(NULL)
    , m_pFrameBottom(NULL)
{
    if (m_bScreenSaver)
    {
        loadContent(XML_SCREENSAVERS_UI_LAYOUT_FILE);
    }
    else
    {
        loadContent(XML_REGION_FORMATTEXT_XML);
    }
}

CUIRegionFormatText::~CUIRegionFormatText()
{
    if (NULL != m_pAdapterHead)
    {
        delete m_pAdapterHead;
        m_pAdapterHead = NULL;
    }

    if (NULL != m_pAdapterFoot)
    {
        delete m_pAdapterFoot;
        m_pAdapterFoot = NULL;
    }

    if (NULL != m_pClockLab)
    {
        delete m_pClockLab;
        m_pClockLab = NULL;
    }

    if (NULL != m_pTitleIcons)
    {
        delete m_pTitleIcons;
        m_pTitleIcons = NULL;
    }
}

void CUIRegionFormatText::Update(void * pMenuData)
{
    if (NULL == pMenuData)
    {
        return;
    }

    if (m_bScreenSaver)
    {
        XBScreenSaverData * pNewData = static_cast<XBScreenSaverData *>(pMenuData);

        if (NULL == m_pListView || NULL == pNewData)
        {
            return;
        }

        LoadMenuItemData(pNewData);
    }
    else
    {
        XBFormattedTextData * pNewData = static_cast<XBFormattedTextData *>(pMenuData);

        if (NULL == m_pListHead || NULL == m_pListFoot || NULL == m_pListView || NULL == pNewData)
        {
            return;
        }

        LoadMenuItemData(pNewData);
        m_iQuitTime = pNewData->stAttr.iTimeOut;
        m_bLock = pNewData->stAttr.bLockIn;
        SetSoftkey(pNewData->vecSoftkey);
    }
}

void CUIRegionFormatText::SetDefaultSoftkey(VecSoftkeyParam & softkeyParam)
{
    softkeyParam.clear();

    softkeyParam.push_back(TypeToSoftInfo(XB_ST_EXIT));
    softkeyParam.push_back(TypeToSoftInfo(XB_ST_EMPTY));
    softkeyParam.push_back(TypeToSoftInfo(XB_ST_EMPTY));
    softkeyParam.push_back(TypeToSoftInfo(XB_ST_EMPTY));
}

int CUIRegionFormatText::GetCurrentItemIndex()
{
    if (NULL != m_pListView)
    {
        return -1;//m_pListView->GetFocusItemDataIndex();
    }
    return 0;
}

void CUIRegionFormatText::loadChildrenElements(xml_node & node)
{
    xFrameLayout::loadChildrenElements(node);

    m_pListView = static_cast<xListView *>(getViewById(XML_REGION_FORMATTEXT_LIST_VIEW));
    if (NULL != m_pListView)
    {
        m_pListView->SetAdapter(this);
    }

    if (m_bScreenSaver)
    {
        m_pImage = static_cast<xImageView*>(getViewById(XML_SCREENSAVERS_IMAGE));
        m_pFrameTop = static_cast<xFrameLayout*>(getViewById(XML_SCREENSAVERS_FRAMETOP));
        m_pFrameBottom = static_cast<xFrameLayout*>(getViewById(XML_SCREENSAVERS_FRAMEBOTTOM));
        if (m_pImage != NULL)
        {
            m_pImage->hide();
        }
        return;
    }

    m_pListHead = static_cast<xListView *>(getViewById(XML_REGION_FORMATTEXT_LIST_HEAD));
    if (NULL != m_pListHead)
    {
        m_pAdapterHead = new CFormatTextAdapter;
        if (NULL != m_pAdapterHead)
        {
            m_pListHead->SetAdapter(m_pAdapterHead);
        }
    }

    m_pListFoot = static_cast<xListView *>(getViewById(XML_REGION_FORMATTEXT_LIST_FOOT));
    if (NULL != m_pListFoot)
    {
        m_pAdapterFoot = new CFormatTextAdapter;
        if (NULL != m_pAdapterFoot)
        {
            m_pListFoot->SetAdapter(m_pAdapterFoot);
            m_pListFoot->layout_marginBottom = currentSoftKeyBar().layout_height;
        }
    }
}

bool CUIRegionFormatText::LoadMenuItemData(XBFormattedTextData * pData)
{
    if (NULL == pData || NULL == m_pListFoot || NULL == m_pListHead || NULL == m_pListView
            || NULL == m_pAdapterHead || NULL == m_pAdapterFoot)
    {
        return false;
    }

    stScrollData scrollData;
    VEC_SCROLL_ITEM_DATA    vecItemData;        //要显示内容

    // Header
    vecItemData.clear();
    List_Header_Zone::iterator iterHd = pData->listHeader.begin();
    for (; iterHd != pData->listHeader.end(); ++iterHd)
    {
        LineData * pLineData = (*iterHd);
        if (NULL == pLineData)
        {
            continue;
        }

        scrollData.m_strText = pLineData->strText;
        scrollData.nAlgin = TypeToAlignmentFlag(pLineData->iAlign);
        scrollData.m_Color = GetXMLTextColor(pLineData->strRGB, pLineData->eColor);
        scrollData.eFont = TypeToFontPointSize(pLineData->eFont);
        vecItemData.push_back(scrollData);
        m_nHeadCount++;
        if (XML_REGION_FORMATTEXT_HEAD_MAX == m_nHeadCount)
        {
            break;
        }
    }

    m_pAdapterHead->SetData(vecItemData);

    // Footer
    vecItemData.clear();
    List_Footer_Zone::iterator iterFt = pData->listFooter.begin();
    for (int i = 0; iterFt != pData->listFooter.end(); ++iterFt, ++i)
    {
        LineData * pLineData = (*iterFt);
        if (NULL == pLineData)
        {
            continue;
        }

        scrollData.m_strText = pLineData->strText;
        scrollData.nAlgin = TypeToAlignmentFlag(pLineData->iAlign);
        scrollData.m_Color = GetXMLTextColor(pLineData->strRGB, pLineData->eColor);
        scrollData.eFont = TypeToFontPointSize(pLineData->eFont);
        vecItemData.push_back(scrollData);
        m_nFootCount++;
        if (XML_REGION_FORMATTEXT_FOOT_MAX == m_nFootCount)
        {
            break;
        }
    }

    m_pAdapterFoot->SetData(vecItemData);

    // Content
    m_vecItemData.clear();
    List_Scroll_Zone & refListItem = pData->listScroll;
    List_Scroll_Zone::iterator itItem = refListItem.begin();
    for (; itItem != refListItem.end(); ++itItem)
    {
        LineData * pItem = (*itItem);
        if (NULL == pItem)
        {
            continue;
        }

        scrollData.m_strText = pItem->strText;
        scrollData.nAlgin = TypeToAlignmentFlag(pItem->iAlign);
        scrollData.m_Color = GetXMLTextColor(pItem->strRGB, pItem->eColor);
        scrollData.eFont = TypeToFontPointSize(pItem->eFont);
        m_vecItemData.push_back(scrollData);
    }

    return true;
}


bool CUIRegionFormatText::LoadMenuItemData(XBScreenSaverData * pData)
{
    if (NULL == pData || NULL == m_pListView
       )
    {
        return false;
    }


    ////////屏保图片、时钟、图标显示/////////////////
    if (pData->stSysTime.bShow)
    {
        if (m_pClockLab == NULL)
        {
            m_pClockLab = new xDateTimeView;
        }
        if (m_pClockLab == NULL)
        {
            return false;
        }
#if !IS_COLOR
        //黑白屏在data控件init前需要设置支持的字体
        m_pClockLab->SetDateTimeSize(10, 10);
#endif
        m_pClockLab->Init();
        m_pClockLab->SetTimeShowFormat(xDateTimeView::TIME_SHOW_FORMAT_HH_MM);
        m_pClockLab->SetDateTimeShowFormat(xDateTimeView::DATE_TIME_SHOW_FORMAT_TIME_DATE);
        m_pClockLab->SetDateTimeSize(TypeToFontPointSize(pData->stSysTime.eFont), TypeToFontPointSize(pData->stSysTime.eFont));
        m_pClockLab->SetTimeFormat(SettingUILogic_GetCurTimeFormatType());
        m_pClockLab->SetDateFormat(SettingUILogic_GetCurDateFormatType());
        m_pClockLab->SetTextColor(GetXMLTextColor(pData->stSysTime.strRGB, pData->stSysTime.eColor));
        //获取的时钟文本长度加上时间与日期的间隔
        m_pClockLab->layout_width = m_pClockLab->wrapContentWidth() + m_pClockLab->GetFont().textBound(" ").cx;
        m_pClockLab->layout_height = TypeToFontPointSize(pData->stSysTime.eFont);
        switch (pData->stSysTime.eVertical)
        {
        case IMAGE_VERTICAL_ALIGN_TOP:
            {
                if (m_pFrameTop != NULL)
                {
                    m_pFrameTop->addChildView(m_pClockLab);
                }
            }
            break;
        case IMAGE_VERTICAL_ALIGN_CENTER:
        case IMAGE_VERTICAL_ALIGN_BOTTOM:
            {
                if (m_pFrameBottom != NULL)
                {
                    m_pFrameBottom->addChildView(m_pClockLab);
                }
            }
            break;
        default:
            break;
        }

        FrameLayoutParams* layoutInParent = FrameLayoutParams::GetLayoutParams(m_pClockLab->m_pLayoutInParent);
        if (layoutInParent != NULL)
        {
            layoutInParent->layout_gravity = GetAlignByImageAlign(pData->stSysTime.eVertical, pData->stSysTime.eHorizontal);
        }

        m_pClockLab->show();
    }

    if (pData->stStatusIcon.bShow)
    {
        if (m_pTitleIcons == NULL)
        {
            m_pTitleIcons = new xLinearLayout;
        }
        if (m_pTitleIcons == NULL)
        {
            return false;
        }
        m_pTitleIcons->layout_width = wrap_content;
        m_pTitleIcons->layout_height = wrap_content;
        switch (pData->stStatusIcon.eVertical)
        {
        case IMAGE_VERTICAL_ALIGN_TOP:
            {
                if (m_pFrameTop != NULL)
                {
                    m_pFrameTop->addChildView(m_pTitleIcons);
                }
            }
            break;
        case IMAGE_VERTICAL_ALIGN_CENTER:
        case IMAGE_VERTICAL_ALIGN_BOTTOM:
            {
                if (m_pFrameBottom != NULL)
                {
                    m_pFrameBottom->addChildView(m_pTitleIcons);
                }
            }
            break;
        default:
            break;
        }
        FrameLayoutParams* layoutInParent = FrameLayoutParams::GetLayoutParams(m_pTitleIcons->m_pLayoutInParent);
        if (layoutInParent != NULL)
        {
            layoutInParent->layout_gravity = GetAlignByImageAlign(pData->stStatusIcon.eVertical, pData->stStatusIcon.eHorizontal);
        }

        m_pTitleIcons->show();
    }
#if IS_COLOR
    if (pData->listImage.front()->eLoadStatus == ILS_LOAD_OK)
    {
        m_pImage->layout_width = pData->listImage.front()->uWidth;
        m_pImage->layout_height = pData->listImage.front()->uHeight;
        FrameLayoutParams* layoutInParent = FrameLayoutParams::GetLayoutParams(m_pImage->m_pLayoutInParent);
        if (layoutInParent != NULL)
        {
            layoutInParent->layout_gravity = GetAlignByImageAlign(pData->listImage.front()->eVertical, pData->listImage.front()->eHorizontal);
        }
        m_pImage->show();
        xPixmap PixImage;
        PixImage.load(pData->listImage.front()->strImagePath);
        m_pImage->setBackground(PixImage);
        m_pImage->update(true);
    }
#endif

//////////////////////////////////////////////////////////////////
    stScrollData scrollData;
// Content
    m_vecItemData.clear();
    List_Scroll_Zone & refListItem = pData->listLine;
    List_Scroll_Zone::iterator itItem = refListItem.begin();
    for (; itItem != refListItem.end(); ++itItem)
    {
        LineData * pItem = (*itItem);
        if (NULL == pItem)
        {
            continue;
        }

        scrollData.m_strText = pItem->strText;
        scrollData.nAlgin = TypeToAlignmentFlag(pItem->iAlign);
        scrollData.m_Color = GetXMLTextColor(pItem->strRGB, pItem->eColor);
        scrollData.eFont = TypeToFontPointSize(pItem->eFont);
        m_vecItemData.push_back(scrollData);


    }

    return true;

}

void CUIRegionFormatText::InitListViewLayout(xListView * pListView)
{
    if (NULL == pListView)
    {
        return;
    }

    xListViewStyle* pStyle = pListView->GetStyle();

    if (NULL != pStyle)
    {
        pStyle->SetItemHeight(0);
        if (m_bScreenSaver)
        {
            //pStyle->SetPageItemCount(m_vecItemData.size());
        }
        else
        {
            pStyle->SetPageItemCount(8);
        }
        pStyle->SetPageHeight(271);
        pStyle->SetContentHeight(272);
#if IF_XLISTVIEW_ITEM_SPACE_BY_LINE
        pStyle->SetItemLineIgnoreEnd(true);
#endif
    }

    pListView->SetTotalDataCount(m_vecItemData.size());
}

xListViewItem * CUIRegionFormatText::PrepareItem(xListView * pListView, int nDataIndex,
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

bool CUIRegionFormatText::RefreshItem(xListView * pListView, xListViewItem * pItem)
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
        const stScrollData & stMenuData = m_vecItemData[nDataIndex];
        pMenuItem->SetGravity(stMenuData.nAlgin);
        pMenuItem->SetMenu(stMenuData.m_strText);
        pMenuItem->SetColor(stMenuData.m_Color);
        pMenuItem->SetFontSize(stMenuData.eFont);
    }

    return true;
}


bool CUIRegionFormatText::GetItemHeight(xListView * pListView, int nDataIndex, int& nItemHeight)
{
    if ((nDataIndex < 0) || (m_vecItemData.size() < nDataIndex + 1))
    {
        return false;
    }
    nItemHeight = m_vecItemData[nDataIndex].eFont;
    return true;
}

void CUIRegionFormatText::OnItemAction(xListView * pListView, xListViewItem * pItem, int nAction)
{
    if (NULL == pListView || pListView != m_pListView || NULL == pItem)
    {
        return;
    }

    if (LIST_VIEW_ITEM_ACTION_FOCUS == nAction)
    {
        //xListViewAdapter::OnItemAction(pListView, pItem, nAction);
        //UpdateIndex();
        //m_nSelect = pItem->GetDataIndex();
        return;
    }
}

void CUIRegionFormatText::UpdateContent()
{
    //RelayoutByContent();
    RefreshListView();
    if (NULL != m_pListView)
    {
        if (1/*m_bScreenSaver*/)
        {
            m_pListView->FocusItemByDataIndex(-1);
        }
    }
    if (NULL != m_pAdapterHead)
    {
        m_pAdapterHead->RefreshListView();
    }
    if (NULL != m_pAdapterFoot)
    {
        m_pAdapterFoot->RefreshListView();
    }
}

void CUIRegionFormatText::RelayoutByContent()
{
    //if (m_pListHead != NULL)
    //{
    //    if (0 >= m_nHeadCount)
    //    {
    //        m_pListHead->hide();
    //    }
    //    else
    //    {
    //        m_pListHead->layout_height = m_nHeadCount * XML_REGION_FORMATTEXT_LIST_ITEM_HEIGHT;
    //        m_pListHead->SetPageItemCount(m_nHeadCount);
    //    }
    //}

    //if (m_pListFoot != NULL)
    //{
    //    if (0 >= m_nFootCount)
    //    {
    //        m_pListFoot->hide();
    //    }
    //}

    //int nListCount = XML_REGION_FORMATTEXT_ITEM_MAX - m_nHeadCount - m_nFootCount;
    //if (m_bScreenSaver)
    //{
    //    m_pListView->SetPageItemCount(m_vecItemData.size());
    //    ownerStation().asyncRelayoutViewBound(this);
    //    return;
    //}
    //m_pListView->layout_marginTop = m_nHeadCount * XML_REGION_FORMATTEXT_LIST_ITEM_HEIGHT;
    //m_pListView->layout_height = nListCount * XML_REGION_FORMATTEXT_LIST_ITEM_HEIGHT;
    //m_pListView->SetPageItemCount(nListCount);
    //ownerStation().asyncRelayoutViewBound(this);
}


bool CUIRegionFormatText::OnKeyPress(int nKey)
{

    if ((PHONE_KEY_UP == nKey) || (PHONE_KEY_DOWN == nKey))
    {
        if (m_pListView == NULL)
        {
            return false;
        }

        if (PHONE_KEY_UP == nKey)
        {
            m_pListView->OnNotifyScrollPrevious();
        }
        else if (PHONE_KEY_DOWN == nKey)
        {
            m_pListView->OnNotifyScrollNext();
        }
    }
    return CUIRegionBase::OnKeyPress(nKey);
}

bool CUIRegionFormatText::IsRGB(yl::string strRGB, xColor & ColorRGB)
{
    int iRed, iGreen, iBlue;
    strRGB.trim_both();
    strRGB.to_upper();

    if (sscanf(strRGB.c_str(), "%d,%d,%d", &iRed, &iGreen, &iBlue) == 3)
    {
        //直接后面判断三个值是否正确
    }
    else if (sscanf(strRGB.c_str(), "RGB(%d,%d,%d)", &iRed, &iGreen, &iBlue) == 3)
    {
        //直接后面判断三个值是否正确
    }
    else
    {
        return false;
    }

    if (iRed > 256 || iRed < 0)
    {
        return false;
    }
    if (iGreen > 256 || iGreen < 0)
    {
        return false;
    }
    if (iBlue > 256 || iBlue < 0)
    {
        return false;
    }
#if IF_ONLY_USE_GRADATION
#else
    ColorRGB.r = iRed;
    ColorRGB.b = iGreen;
    ColorRGB.b = iBlue;
#endif
    return true;
}

xColor CUIRegionFormatText::GetXMLTextColor(yl::string strRGB, TEXT_FONT_COLOR eColorType)
{
    xColor ColorText;
    if (IsRGB(strRGB, ColorText))
    {
        return ColorText;
    }
    else
    {
        return TypeToColor(eColorType);
    }
}

int CUIRegionFormatText::GetAlignByImageAlign(IMAGE_VERTICAL_ALIGN eVertical,
        IMAGE_HORIZONTAL_ALIGN eHorizontal)
{
    int nAlign = 0;
    // botton
    if (IMAGE_VERTICAL_ALIGN_BOTTOM == eVertical)
    {
        nAlign = nAlign | align_bottom;
    }
    // middle
    else if (IMAGE_VERTICAL_ALIGN_CENTER == eVertical)
    {
        nAlign = nAlign | align_v_center;
    }
    else
    {
        nAlign = nAlign | align_top;
    }

    // right
    if (IMAGE_HORIZONTAL_ALIGN_RIGHT == eHorizontal)
    {
        nAlign = nAlign | align_right;
    }
    // center
    else if (IMAGE_HORIZONTAL_ALIGN_CENTER == eHorizontal)
    {
        nAlign = nAlign | align_h_center;
    }
    else
    {
        nAlign = nAlign | align_left;
    }

    return nAlign;
}

#endif
