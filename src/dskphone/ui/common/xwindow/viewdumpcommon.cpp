#include "viewdumpcommon.h"
#include "dsklog/log.h"
#include "xControls.h"
#include "xcombobox.h"
#include "xListView.h"
#include "xlistviewitem.h"
#include "xPreferenceLayout.h"
#include "modviewdump.h"
#include "xtableview.h"


#if IF_XWINDOW_DUMP_EXPORT
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool FormatIntData(void* pData, yl::string& strData)
{
    int* pIntData = (int*)pData;

    if (NULL != pIntData)
    {
        strData = commonAPI_FormatString("%d", *pIntData);

        return true;
    }

    return false;
}

bool FormatLongData(void* pData, yl::string& strData)
{
    long* pLongData = (long*)pData;

    if (NULL != pLongData)
    {
        strData = commonAPI_FormatString("%d", *pLongData);

        return true;
    }

    return false;
}

bool FormatBoolData(void* pData, yl::string& strData)
{
    bool* pBoolData = (bool*)pData;

    if (NULL != pBoolData)
    {
        strData = commonAPI_FormatString("%d", ((*pBoolData) ? 1 : 0));

        return true;
    }

    return false;
}

bool FormatRectData(void* pData, yl::string& strData)
{
    chRect* pRect = (chRect*)pData;

    if (NULL != pRect)
    {
        strData = commonAPI_FormatString("%d,%d,%d,%d", pRect->left, pRect->top, pRect->Width(), pRect->Height());

        return true;
    }

    return false;
}

bool FormatWStringData(void* pData, yl::string& strData)
{
    yl::wstring* pString = (yl::wstring*)pData;

    if (NULL != pString)
    {
        strData = yl::to_utf8(*pString);

        return true;
    }

    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
CViewDumpClassBase::CViewDumpClassBase(const yl::string& strParentClass)
    : m_strParentClass(strParentClass)
{
}

CViewDumpClassBase::~CViewDumpClassBase()
{

}

bool CViewDumpClassBase::RegisterProperty(const yl::string& strName, FUN_GET_PROPERTY pFunGetProperty)
{
    MAP_DUMP_PROPERTY::iterator iter = m_mapProperty.find(strName);

    if (iter != m_mapProperty.end())
    {
        XWINDOW_ERR("Duplicated property:%s", strName.c_str());
        return false;
    }

#if IF_VIEW_DUMP_DEBUG
    printf("Register Property:%p %s %p \n", this, strName.c_str(), pFunGetProperty);
#endif

    m_mapProperty[strName] = pFunGetProperty;

    return true;
}

void CViewDumpClassBase::DumpProperty(xView* pView, xml_node& node)
{
    MAP_DUMP_PROPERTY::iterator iter = m_mapProperty.begin();

    for (; iter != m_mapProperty.end(); ++iter)
    {
        FUN_GET_PROPERTY pFunGetProperty = iter->second;
        yl::string& strName = iter->first;
        yl::string strValue;

        if (NULL != pFunGetProperty)
        {
            strValue = (*pFunGetProperty)(pView);
        }

        node.append_attribute(strName.c_str(), strValue.c_str());
    }
}

const yl::string& CViewDumpClassBase::GetParentClass() const
{
    return m_strParentClass;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// xView
DECLARE_VIEW_DUMP_BEGIN(xView)
DECLARE_VIEW_DUMP_PROPERTY_DATA(id, m_strId, VIEW_DUMP_DATA_TYPE_STRING)
DECLARE_VIEW_DUMP_PROPERTY_DATA(ltwh, m_rectLayout, VIEW_DUMP_DATA_TYPE_RECT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(focusable, focusable, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(disable, disable, VIEW_DUMP_DATA_TYPE_BOOL)
DECLARE_VIEW_DUMP_END

IMPLEMENT_VIEW_DUMP_BASE_BEGIN(xView)
IMPLEMENT_VIEW_DUMP_PROPERTY(id)
IMPLEMENT_VIEW_DUMP_PROPERTY(ltwh)
IMPLEMENT_VIEW_DUMP_PROPERTY(focusable)
IMPLEMENT_VIEW_DUMP_PROPERTY(disable)
IMPLEMENT_VIEW_DUMP_END(xView)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// xImageView
DECLARE_VIEW_DUMP_BEGIN(xImageView)
#if IF_INVERT_EFFECT
DECLARE_VIEW_DUMP_PROPERTY_DATA(invert, invert, VIEW_DUMP_DATA_TYPE_INT)
#endif
DECLARE_VIEW_DUMP_PROPERTY_DATA(src, m_strSrc, VIEW_DUMP_DATA_TYPE_STRING)
DECLARE_VIEW_DUMP_END

IMPLEMENT_VIEW_DUMP_BEGIN(xImageView, xView)
#if IF_INVERT_EFFECT
IMPLEMENT_VIEW_DUMP_PROPERTY(invert)
#endif
IMPLEMENT_VIEW_DUMP_PROPERTY(src)
IMPLEMENT_VIEW_DUMP_END(xImageView)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// xTextView
DECLARE_VIEW_DUMP_BEGIN(xTextView)
DECLARE_VIEW_DUMP_PROPERTY_DATA(text, m_strText, VIEW_DUMP_DATA_TYPE_STRING)
DECLARE_VIEW_DUMP_PROPERTY_DATA(painttext, m_strCachePaintString, VIEW_DUMP_DATA_TYPE_WSTRING)
DECLARE_VIEW_DUMP_PROPERTY_DATA(rect, m_rcText, VIEW_DUMP_DATA_TYPE_RECT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(gravity, gravity, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(align, align, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(overlength, overlength_style, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_END

IMPLEMENT_VIEW_DUMP_BEGIN(xTextView, xView)
IMPLEMENT_VIEW_DUMP_PROPERTY(text)
IMPLEMENT_VIEW_DUMP_PROPERTY(painttext)
IMPLEMENT_VIEW_DUMP_PROPERTY(rect)
IMPLEMENT_VIEW_DUMP_PROPERTY(gravity)
IMPLEMENT_VIEW_DUMP_PROPERTY(align)
IMPLEMENT_VIEW_DUMP_PROPERTY(overlength)
IMPLEMENT_VIEW_DUMP_END(xTextView)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// xButton
DECLARE_VIEW_DUMP_BEGIN(xButton)
#if IF_INVERT_EFFECT
DECLARE_VIEW_DUMP_PROPERTY_DATA(invert, m_bInvert, VIEW_DUMP_DATA_TYPE_INT)
#endif
DECLARE_VIEW_DUMP_PROPERTY_DATA(src, m_strSrc, VIEW_DUMP_DATA_TYPE_STRING)
DECLARE_VIEW_DUMP_END

IMPLEMENT_VIEW_DUMP_BEGIN(xButton, xTextView)
#if IF_INVERT_EFFECT
IMPLEMENT_VIEW_DUMP_PROPERTY(invert)
#endif
IMPLEMENT_VIEW_DUMP_PROPERTY(src)
IMPLEMENT_VIEW_DUMP_END(xButton)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// xScrollBar
DECLARE_VIEW_DUMP_BEGIN(xScrollBar)
#if IF_SCROLL_BAR_DENOTE_ITEM_POS
DECLARE_VIEW_DUMP_PROPERTY_DATA(pagecount, m_nPageCount, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(totalcount, m_nTotalCount, VIEW_DUMP_DATA_TYPE_INT)
#else
#if IF_SCROLL_BAR_DENOTE_CONTENT_POS
DECLARE_VIEW_DUMP_PROPERTY_DATA(pageheight, m_nScrollArea, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(contentheight, m_nScrollRange, VIEW_DUMP_DATA_TYPE_INT)
#endif
#endif
DECLARE_VIEW_DUMP_PROPERTY_DATA(position, m_nPosition, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(blockheight, m_nBlockHeight, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(blockpos, m_nBlockPos, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_END

IMPLEMENT_VIEW_DUMP_BEGIN(xScrollBar, xView)
#if IF_SCROLL_BAR_DENOTE_ITEM_POS
IMPLEMENT_VIEW_DUMP_PROPERTY(pagecount)
IMPLEMENT_VIEW_DUMP_PROPERTY(totalcount)
#else
#if IF_SCROLL_BAR_DENOTE_CONTENT_POS
IMPLEMENT_VIEW_DUMP_PROPERTY(pageheight)
IMPLEMENT_VIEW_DUMP_PROPERTY(contentheight)
#endif
#endif
IMPLEMENT_VIEW_DUMP_PROPERTY(position)
IMPLEMENT_VIEW_DUMP_PROPERTY(blockheight)
IMPLEMENT_VIEW_DUMP_PROPERTY(blockpos)
IMPLEMENT_VIEW_DUMP_END(xScrollBar)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// xProgressBar
#if IF_PROGRESS_BAR_ENABLE
DECLARE_VIEW_DUMP_BEGIN(xProgressBar)
DECLARE_VIEW_DUMP_PROPERTY_DATA(range, m_nRange, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(position, m_nPosition, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(paintpos, m_nPaintPosition, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(paintoffset, m_nPaintOffset, VIEW_DUMP_DATA_TYPE_INT)
#if IF_PROGRESSBAR_WITH_VERTICAL
DECLARE_VIEW_DUMP_PROPERTY_DATA(orientation, m_nOrientation, VIEW_DUMP_DATA_TYPE_INT)
#endif
DECLARE_VIEW_DUMP_END

IMPLEMENT_VIEW_DUMP_BEGIN(xProgressBar, xView)
IMPLEMENT_VIEW_DUMP_PROPERTY(range)
IMPLEMENT_VIEW_DUMP_PROPERTY(position)
IMPLEMENT_VIEW_DUMP_PROPERTY(paintpos)
IMPLEMENT_VIEW_DUMP_PROPERTY(paintoffset)
#if IF_PROGRESSBAR_WITH_VERTICAL
IMPLEMENT_VIEW_DUMP_PROPERTY(orientation)
#endif
IMPLEMENT_VIEW_DUMP_END(xProgressBar)
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// xSlideBar

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// xRadioButton
#if IF_RADIO_BUTTON_ENABLE
DECLARE_VIEW_DUMP_BEGIN(xRadioButton)
DECLARE_VIEW_DUMP_PROPERTY_DATA(rctext, m_rcText, VIEW_DUMP_DATA_TYPE_RECT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(text, m_strText, VIEW_DUMP_DATA_TYPE_STRING)
DECLARE_VIEW_DUMP_PROPERTY_DATA(rcicon, m_rcIcon, VIEW_DUMP_DATA_TYPE_RECT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(checked, m_bChecked, VIEW_DUMP_DATA_TYPE_BOOL)
DECLARE_VIEW_DUMP_END

IMPLEMENT_VIEW_DUMP_BEGIN(xRadioButton, xView)
IMPLEMENT_VIEW_DUMP_PROPERTY(rctext)
IMPLEMENT_VIEW_DUMP_PROPERTY(text)
IMPLEMENT_VIEW_DUMP_PROPERTY(rcicon)
IMPLEMENT_VIEW_DUMP_PROPERTY(checked)
IMPLEMENT_VIEW_DUMP_END(xRadioButton)
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// xLinearLayout
DECLARE_VIEW_DUMP_BEGIN(xLinearLayout)
DECLARE_VIEW_DUMP_PROPERTY_DATA(gravity, gravity, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(orientation, orientation, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_END

IMPLEMENT_VIEW_DUMP_BEGIN(xLinearLayout, xView)
IMPLEMENT_VIEW_DUMP_PROPERTY(gravity)
IMPLEMENT_VIEW_DUMP_PROPERTY(orientation)
IMPLEMENT_VIEW_DUMP_END(xLinearLayout)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// xFrameLayout

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// xScaleLayout
#if IF_SCALE_LAYOUT_ENABLE
DECLARE_VIEW_DUMP_BEGIN(xScaleLayout)
DECLARE_VIEW_DUMP_PROPERTY_DATA(scale_width, scale_width, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(scale_height, scale_height, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_END

IMPLEMENT_VIEW_DUMP_BEGIN(xScaleLayout, xView)
IMPLEMENT_VIEW_DUMP_PROPERTY(scale_width)
IMPLEMENT_VIEW_DUMP_PROPERTY(scale_height)
IMPLEMENT_VIEW_DUMP_END(xScaleLayout)
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// xRectView
#if IF_RECT_VIEW_ENABLE
DECLARE_VIEW_DUMP_BEGIN(xRectView)
DECLARE_VIEW_DUMP_PROPERTY_DATA(size, rect_corner_size, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(type, rect_corner_type, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_END

IMPLEMENT_VIEW_DUMP_BEGIN(xRectView, xView)
IMPLEMENT_VIEW_DUMP_PROPERTY(size)
IMPLEMENT_VIEW_DUMP_PROPERTY(type)
IMPLEMENT_VIEW_DUMP_END(xRectView)
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// xComboBox
DECLARE_VIEW_DUMP_BEGIN(xComboBox)
DECLARE_VIEW_DUMP_PROPERTY_METHOD(value, GetValue())
DECLARE_VIEW_DUMP_END

IMPLEMENT_VIEW_DUMP_BEGIN(xComboBox, xTextView)
IMPLEMENT_VIEW_DUMP_PROPERTY(value)
IMPLEMENT_VIEW_DUMP_END(xComboBox)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// xFlashingBox
#if IF_FLASHING_BOX_ENABLE
DECLARE_VIEW_DUMP_BEGIN(xFlashingBox)
DECLARE_VIEW_DUMP_PROPERTY_DATA(showTime, showTime, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(hideTime, hideTime, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_END

IMPLEMENT_VIEW_DUMP_BEGIN(xFlashingBox, xFrameLayout)
IMPLEMENT_VIEW_DUMP_PROPERTY(showTime)
IMPLEMENT_VIEW_DUMP_PROPERTY(hideTime)
IMPLEMENT_VIEW_DUMP_END(xFlashingBox)
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// xFlashingImageView
#if IF_FLASHING_IMAGE_VIEW_ENABLE
DECLARE_VIEW_DUMP_BEGIN(xFlashingImageView)
DECLARE_VIEW_DUMP_PROPERTY_DATA(showTime, m_nShowTime, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(interval, m_nFlashingInterval, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_END

IMPLEMENT_VIEW_DUMP_BEGIN(xFlashingImageView, xImageView)
IMPLEMENT_VIEW_DUMP_PROPERTY(showTime)
IMPLEMENT_VIEW_DUMP_PROPERTY(interval)
IMPLEMENT_VIEW_DUMP_END(xFlashingImageView)
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// xAnimationBox
#if IF_ANIMATION_BOX_ENABLE
DECLARE_VIEW_DUMP_BEGIN(xAnimationBox)
DECLARE_VIEW_DUMP_PROPERTY_DATA(interval, m_nInterval, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(currentPic, m_nCurrentPic, VIEW_DUMP_DATA_TYPE_INT);
DECLARE_VIEW_DUMP_PROPERTY_DATA(pics, m_strPics, VIEW_DUMP_DATA_TYPE_STRING);
DECLARE_VIEW_DUMP_END

IMPLEMENT_VIEW_DUMP_BEGIN(xAnimationBox, xView)
IMPLEMENT_VIEW_DUMP_PROPERTY(interval)
IMPLEMENT_VIEW_DUMP_PROPERTY(currentPic)
IMPLEMENT_VIEW_DUMP_PROPERTY(pics)
IMPLEMENT_VIEW_DUMP_END(xAnimationBox)
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// xPreferenceLayout

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// xListView
DECLARE_VIEW_DUMP_BEGIN(xListView)
DECLARE_VIEW_DUMP_PROPERTY_DATA(styleType, m_pStyle->m_nType, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(scrollType, m_pStyle->m_nScrollType, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(focusType, m_pStyle->m_nFocusType, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(pageItemCount, m_pStyle->m_nPageItemCount, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(contentPos, m_pStyle->m_nContentPos, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(contentHeight, m_pStyle->m_nContentHeight, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(pageHeight, m_pStyle->m_nPageHeight, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(itemTopMargin, m_pStyle->m_nItemTopMargin, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(itemBottomMargin, m_pStyle->m_nItemBottomMargin, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(itemLeftMargin, m_pStyle->m_nItemLeftMargin, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(itemRightMargin, m_pStyle->m_nItemRightMargin, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(itemWidth, m_pStyle->m_nItemWidth, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(itemHeight, m_pStyle->m_nItemHeight, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(itemSpace, m_pStyle->m_nItemSpace, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(scrollTopMargin, m_pStyle->m_nScrollCtrlTopMargin, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(scrollBottomMargin, m_pStyle->m_nScrollCtrlBottomMargin, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(scrollRightMargin, m_pStyle->m_nScrollCtrlRightMargin, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(fixPosFlag, m_pStyle->m_nFixPosFlag, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(itemStyle, m_pStyle->m_strItemStyle, VIEW_DUMP_DATA_TYPE_STRING)
DECLARE_VIEW_DUMP_PROPERTY_DATA(dataCount, m_nDataCount, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(focusIndex, m_nFocusIndex, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(attachItem, m_bAttachItem, VIEW_DUMP_DATA_TYPE_BOOL)
DECLARE_VIEW_DUMP_END

IMPLEMENT_VIEW_DUMP_BEGIN(xListView, xLinearLayout)
IMPLEMENT_VIEW_DUMP_PROPERTY(styleType)
IMPLEMENT_VIEW_DUMP_PROPERTY(scrollType)
IMPLEMENT_VIEW_DUMP_PROPERTY(focusType)
IMPLEMENT_VIEW_DUMP_PROPERTY(pageItemCount)
IMPLEMENT_VIEW_DUMP_PROPERTY(contentPos)
IMPLEMENT_VIEW_DUMP_PROPERTY(contentHeight)
IMPLEMENT_VIEW_DUMP_PROPERTY(pageHeight)
IMPLEMENT_VIEW_DUMP_PROPERTY(itemTopMargin)
IMPLEMENT_VIEW_DUMP_PROPERTY(itemBottomMargin)
IMPLEMENT_VIEW_DUMP_PROPERTY(itemLeftMargin)
IMPLEMENT_VIEW_DUMP_PROPERTY(itemRightMargin)
IMPLEMENT_VIEW_DUMP_PROPERTY(itemWidth)
IMPLEMENT_VIEW_DUMP_PROPERTY(itemHeight)
IMPLEMENT_VIEW_DUMP_PROPERTY(itemSpace)
IMPLEMENT_VIEW_DUMP_PROPERTY(scrollTopMargin)
IMPLEMENT_VIEW_DUMP_PROPERTY(scrollBottomMargin)
IMPLEMENT_VIEW_DUMP_PROPERTY(scrollRightMargin)
IMPLEMENT_VIEW_DUMP_PROPERTY(fixPosFlag)
IMPLEMENT_VIEW_DUMP_PROPERTY(itemStyle)
IMPLEMENT_VIEW_DUMP_PROPERTY(dataCount)
IMPLEMENT_VIEW_DUMP_PROPERTY(focusIndex)
IMPLEMENT_VIEW_DUMP_PROPERTY(attachItem)
IMPLEMENT_VIEW_DUMP_END(xListView)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// xListViewItem
DECLARE_VIEW_DUMP_BEGIN(xListViewItem)
DECLARE_VIEW_DUMP_PROPERTY_DATA(type, m_nType, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(index, m_nDataIndex, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(itemFocus, m_bFocus, VIEW_DUMP_DATA_TYPE_BOOL)
DECLARE_VIEW_DUMP_END

IMPLEMENT_VIEW_DUMP_BEGIN(xListViewItem, xFrameLayout)
IMPLEMENT_VIEW_DUMP_PROPERTY(type)
IMPLEMENT_VIEW_DUMP_PROPERTY(index)
IMPLEMENT_VIEW_DUMP_PROPERTY(itemFocus)
IMPLEMENT_VIEW_DUMP_END(xListViewItem)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// xTableView
#if IF_TABLE_VIEW_ENABLE
DECLARE_VIEW_DUMP_BEGIN(xTableView)
DECLARE_VIEW_DUMP_PROPERTY_DATA(column, GetTableViewStyle()->m_nColumn, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(colSpace, GetTableViewStyle()->m_nColSpace, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_PROPERTY_DATA(colOrientation, GetTableViewStyle()->m_nOrientation, VIEW_DUMP_DATA_TYPE_INT)
DECLARE_VIEW_DUMP_END

IMPLEMENT_VIEW_DUMP_BEGIN(xTableView, xListView)
IMPLEMENT_VIEW_DUMP_PROPERTY(column)
IMPLEMENT_VIEW_DUMP_PROPERTY(colSpace)
IMPLEMENT_VIEW_DUMP_PROPERTY(colOrientation)
IMPLEMENT_VIEW_DUMP_END(xTableView)
#endif

#endif
