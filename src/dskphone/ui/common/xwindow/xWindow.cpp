#include "xWindowManager.h"
#include "keymap.h"

#include "xListView.h"
#include "xControls.h"
#include "xValueHandler.h"

#include "gsCallMethod.h"
#include "commonapi/stringhelperapi.h"
#include "dsklog/log.h"
#include "ylstringalgorithm.h"
#include "ylalgorithm.h"
#include "xwindowcommon.h"
#include "activefonttext.h"
#include "modviewdump.h"
#include "overlengtheffectfactory.h"
#include "overlengtheffectbase.h"
#if IF_GESTURE_ENABLE
#include "gesture/include/gesturemanager.h"
#endif


IMPLEMENT_RUNTIME_FAMILY(xView);

EXPORT_VIEW_CLASS(xView);

void adjustRectByGravity(const chRect& rect, const int gravity, chRect& rectResult)
{
    int xOffset = rect.left - rectResult.left;
    int yOffset = rect.top - rectResult.top;
    switch (gravity & gravity_x_mask)
    {
    case gravity_left:
        xOffset += 0;
        break;
    case gravity_x_center:
        {
            // 1 pixel offset to right with odd cols
            int diff = rect.Width() - rectResult.Width();
            xOffset += diff / 2 + diff % 2;
        }
        break;
    case gravity_right:
        xOffset += rect.Width() - rectResult.Width();
        break;
    default:
        xOffset = 0;
    }
    switch (gravity & gravity_y_mask)
    {
    case gravity_top:
        yOffset += 0;
        break;
    case gravity_y_center:
        {
            // 1 pixel offset to bottom with odd lines
            int diff = rect.Height() - rectResult.Height();
            yOffset += diff / 2 + diff % 2;
        }
        break;
    case gravity_bottom:
        yOffset += rect.Height() - rectResult.Height();
        break;
    default:
        yOffset = 0;
    }
    rectResult.OffsetRect(xOffset, yOffset);
}

bool PopupAndroidBOOL(xml_node& node, const char* pszName, int& value)
{
    xml_attribute attr = node.attribute(pszName);
    yl::string strAttr = attr.as_string();

    node.remove_attribute(attr);

    if (strAttr.empty())
    {
        return false;
    }

    if (strAttr == XWINDOW_ENUM_TRUE)
    {
        value = xView::bool_true;
    }
    else if (strAttr == XWINDOW_ENUM_FALSE)
    {
        value = xView::bool_false;
    }
    else if (strAttr == XWINDOW_ENUM_AUTO)
    {
        value = xView::bool_auto;
    }
    else
    {
        XWINDOW_WARN("Unknow BOOL value:%s", strAttr.c_str());
        return false;
    }

    return true;
}

bool PopupAndroidBool(xml_node& node, const char* pszName, bool& value)
{
    xml_attribute attr = node.attribute(pszName);
    yl::string strAttr = attr.as_string();

    node.remove_attribute(attr);

    if (strAttr.empty())
    {
        return false;
    }

    if (strAttr == XWINDOW_ENUM_TRUE)
    {
        value = true;
    }
    else if (strAttr == XWINDOW_ENUM_FALSE)
    {
        value = false;
    }
    else
    {
        XWINDOW_WARN("Unknow bool value:%s", strAttr.c_str());
        return false;
    }

    return true;
}

bool PopupAndroidInt(xml_node& node, const char* pszName, int& value)
{
    xml_attribute attr = node.attribute(pszName);
    yl::string strAttr = attr.as_string();

    node.remove_attribute(attr);

    if (strAttr.empty())
    {
        return false;
    }

    value = atoi(strAttr.c_str());

    return true;
}

bool PopupAndroidDouble(xml_node& node, const char* pszName, double& value)
{
    xml_attribute attr = node.attribute(pszName);
    yl::string strAttr = attr.as_string();

    node.remove_attribute(attr);

    if (strAttr.empty())
    {
        return false;
    }

    value = atof(strAttr.c_str());

    return true;
}

bool PopupAndroidPixel(xml_node& node, const char* pszName, int& value)
{
    xml_attribute attr = node.attribute(pszName);
    yl::string strAttr = attr.as_string();

    node.remove_attribute(attr);

    if (strAttr.empty())
    {
        return false;
    }

    value = atoi(strAttr.c_str());

#if IF_SCREEN_SUPPORT_DEV_PIXEL_SCALE
    value = g_WorkStation.getScreenDevPixelX(value);
#endif

    return true;
}

bool PopupAndroidFont(xml_node& node, const char* pszName, int& value)
{
    xml_attribute attr = node.attribute(pszName);
    yl::string strAttr = attr.as_string();

    node.remove_attribute(attr);

    if (strAttr.empty())
    {
        return false;
    }

    value = atoi(strAttr.c_str());

#if IF_SCREEN_SUPPORT_DEV_PIXEL_SCALE
    value = g_WorkStation.getScreenDevPixelY(value);
#endif

    return true;
}

bool PopupAndroidString(xml_node& node, const char* pszName, yl::string& value)
{
    xml_attribute attr = node.attribute(pszName);

    value = attr.as_string();
    node.remove_attribute(attr);

    if (value.empty())
    {
        return false;
    }

    if (value[0] == '@')
    {
        size_t uPos = value.find('/');

        if (yl::string::npos != uPos)
        {
            value = value.substr(uPos + 1);
        }
    }

    return true;
}

bool PopupAndroidRect(xml_node& node, const char* pszName, chRect& value)
{
    xml_attribute attr = node.attribute(pszName);
    yl::string strAttr = attr.as_string();

    node.remove_attribute(attr);

    if (strAttr.empty())
    {
        return false;
    }

    int x = 0, y = 0, w = 0, h = 0;

    if (4 != sscanf(strAttr.c_str(), "%d,%d,%d,%d", &x, &y, &w, &h))
    {
        x = 0;
        y = 0;
        w = 0;
        h = 0;
    }

    value.SetRect(x, y, x + w, y + h);

    return true;
}

bool PopupAndroidColor(xml_node& node, const char* pszName, xColor& value)
{
    xml_attribute attr = node.attribute(pszName);
    yl::string strAttr = attr.as_string();
    int nSize = strAttr.size();

    if (nSize >= 7 && strAttr[0] == '#')
    {
        node.remove_attribute(attr);

        UINT c = 0;

        if (1 != sscanf(strAttr.c_str() + 1, "%x", &c))
        {
            c = 0;
        }

#if IF_PIXEL_ALPHA
        BYTE byAlpha = (9 == nSize) ? ((c & 0xFF000000) >> 24) : 0xFF;
#endif

        BYTE r = (c & 0x00FF0000) >> 16;
        BYTE g = (c & 0x0000FF00) >> 8;
        BYTE b = (c & 0x000000FF) >> 0;

#if IF_PIXEL_ALPHA
        xColor clrTmp(byAlpha, r, g, b);
#else
        xColor clrTmp(r, g, b);
#endif

        value = clrTmp;

        return true;
    }

    return false;
}

bool PopupAndroidDimension(xml_node& node, const char* pszName, int& value)
{
    xml_attribute attr = node.attribute(pszName);
    yl::string strAttr = attr.as_string();

    if (strAttr.empty())
    {
        node.remove_attribute(attr);
        return false;
    }

    if (strAttr == XWINDOW_ENUM_WRAP_CONTENT)
    {
        value = LayoutParams::wrap_content;
    }
    else if (strAttr == XWINDOW_ENUM_MATCH_PARENT || strAttr == XWINDOW_ENUM_FILL_PARENT)
    {
        value = LayoutParams::match_parent;
    }
    else
    {
        return PopupAndroidPixel(node, pszName, value);
    }

    node.remove_attribute(attr);

    return true;
}

bool PopupAndroidGravity(xml_node& node, const char* pszName, int& value)
{
    xml_attribute attr = node.attribute(pszName);
    yl::string strAttr = attr.as_string();

    node.remove_attribute(attr);

    if (strAttr.empty())
    {
        return false;
    }

    LIST_STRING listString;

    if (!commonAPI_splitStringBySeparator(strAttr, '|', listString))
    {
        return false;
    }

    LIST_STRING::iterator iter = listString.begin();
    int nOldValue = value;

    value = gravity_undefined;

    for (; iter != listString.end(); ++iter)
    {
        yl::string strValue = (*iter).trim_both();

        if (strValue == XWINDOW_ENUM_TOP)
        {
            value |= gravity_top;
        }
        else if (strValue == XWINDOW_ENUM_BOTTOM)
        {
            value |= gravity_bottom;
        }
        else if (strValue == XWINDOW_ENUM_CENTER_VERTICAL)
        {
            value |= gravity_y_center;
        }
        else if (strValue == XWINDOW_ENUM_LEFT)
        {
            value |= gravity_left;
        }
        else if (strValue == XWINDOW_ENUM_RIGHT)
        {
            value |= gravity_right;
        }
        else if (strValue == XWINDOW_ENUM_CENTER_HORIZONTAL)
        {
            value |= gravity_x_center;
        }
        else if (strValue == XWINDOW_ENUM_CENTER)
        {
            value = gravity_x_center |  gravity_y_center;
        }
        else
        {
            XWINDOW_WARN("Unknow layout_gravity value:%s", strValue.c_str());
        }
    }

    if (gravity_undefined == value)
    {
        value = nOldValue;
        return false;
    }

    return true;
}

bool PopupAndroidVisibility(xml_node& node, const char* pszName, int& value)
{
    xml_attribute attr = node.attribute(pszName);
    yl::string strAttr = attr.as_string();

    node.remove_attribute(attr);

    if (strAttr.empty())
    {
        return false;
    }

    if (strAttr == XWINDOW_ENUM_VISIBLE)
    {
        value = xView::visible;
    }
    else if (strAttr == XWINDOW_ENUM_INVISIBLE)
    {
        value = xView::invisible;
    }
    else if (strAttr == XWINDOW_ENUM_GONE)
    {
        value = xView::gone;
    }
    else
    {
        XWINDOW_WARN("Unknow visibility value:%s", strAttr.c_str());
        return false;
    }

    return true;
}

bool PopupAndroidOrientation(xml_node& node, const char* pszName, int& value)
{
    xml_attribute attr = node.attribute(pszName);
    yl::string strAttr = attr.as_string();

    node.remove_attribute(attr);

    if (strAttr.empty())
    {
        XWINDOW_WARN("Loss orientation");
        return false;
    }

    if (strAttr == XWINDOW_ENUM_VERTICAL)
    {
        value = xLinearLayout::orientation_vertical;
    }
    else if (strAttr == XWINDOW_ENUM_HORIZONTAL)
    {
        value = xLinearLayout::orientation_horizontal;
    }
    else
    {
        XWINDOW_WARN("Unknow orientation value:%s", strAttr.c_str());
        return false;
    }

    return true;
}

bool PopupAndroidPixmap(xml_node& node, const char* pszName, xPixmap& value)
{
    xml_attribute attr = node.attribute(pszName);
    yl::string strAttr = attr.as_string();

    node.remove_attribute(attr);

    if (strAttr.empty())
    {
        return false;
    }

    g_WorkStation.GetImageByPath(strAttr, value);

    return true;
}

bool PopupAndroidBrush(xml_node& node, const char* pszName, xBrush& value)
{
    xColor color;

    if (PopupAndroidColor(node, pszName, color))
    {
        value.setColor(color);
        return true;
    }

    xPixmap objBrush;

    if (!PopupAndroidPixmap(node, pszName, objBrush))
    {
        return false;
    }

    value.setPixmap(objBrush);

    return true;
}

bool PopupAndroidCorner(xml_node& node, const char* pszName, int& value)
{
    xml_attribute attr = node.attribute(pszName);
    yl::string strAttr = attr.as_string();

    node.remove_attribute(attr);

    if (strAttr.empty())
    {
        return false;
    }

    LIST_STRING listString;

    if (!commonAPI_splitStringBySeparator(strAttr, '|', listString))
    {
        return false;
    }

    LIST_STRING::iterator iter = listString.begin();
    int nOldValue = value;

    value = 0;

    for (; iter != listString.end(); ++iter)
    {
        yl::string strValue = (*iter).trim_both();

        if (XWINDOW_ENUM_LEFT_TOP == strValue)
        {
            value |= CORNER_TYPE_LEFT_TOP;
        }
        else if (XWINDOW_ENUM_RIGHT_TOP == strValue)
        {
            value |= CORNER_TYPE_RIGHT_TOP;
        }
        else if (XWINDOW_ENUM_LEFT_BOTTOM == strValue)
        {
            value |= CORNER_TYPE_LEFT_BOTTOM;
        }
        else if (XWINDOW_ENUM_RIGHT_BOTTOM == strValue)
        {
            value |= CORNER_TYPE_RIGHT_BOTTOM;
        }
        else if (XWINDOW_ENUM_ALL == strValue)
        {
            value |= CORNER_TYPE_ALL;
        }
        else
        {
            XWINDOW_WARN("Unknow rect_corner_type value:%s", strValue.c_str());
        }
    }

    if (0 == value)
    {
        value = nOldValue;
    }

    return true;
}

bool PopupAndroidOverLengthStyle(xml_node& node, const char* pszName, YLVector<int>& vecType)
{
    xml_attribute attr = node.attribute(pszName);
    yl::string strAttr = attr.as_string();

    node.remove_attribute(attr);

    if (strAttr.empty())
    {
        return false;
    }

    LIST_STRING listString;

    if (!commonAPI_splitStringBySeparator(strAttr, '|', listString))
    {
        return false;
    }

    LIST_STRING::iterator iter = listString.begin();

    for (; iter != listString.end(); ++iter)
    {
        yl::string strValue = (*iter).trim_both();

        if (strValue == XWINDOW_ENUM_DIRECT_CUT)
        {
            vecType.push_back(OVERLENGTH_TYPE_DIRECT_CUT);
        }
        else if (strValue == XWINDOW_ENUM_CUT_WITH_ELLIPSIS)
        {
            vecType.push_back(OVERLENGTH_TYPE_CUT_WITH_ELLIPSIS);
        }
        else if (strValue == XWINDOW_ENUM_SCROLL)
        {
            vecType.push_back(OVERLENGTH_TYPE_SCROLL);
        }
#if IF_TEXTVIEW_SUPPORT_VERTICAL_SCROLL
        else if (strValue == XWINDOW_ENUM_VSCROLL)
        {
            vecType.push_back(OVERLENGTH_TYPE_VSCROLL);
        }
#endif
        else
        {
            XWINDOW_WARN("Unknow overlength_type:%s", strValue.c_str());
        }
    }

    return true;
}

bool PopupAndroidArray(xml_node& node, const char* pszName, YLVector<int>& vec)
{
    xml_attribute attr = node.attribute(pszName);
    yl::string strAttr = attr.as_string();

    strAttr.trim_both();

    if (strAttr.empty())
    {
        return false;
    }

    LIST_STRING listString;

    if (!commonAPI_splitStringBySeparator(strAttr, ',', listString))
    {
        return false;
    }

    LIST_STRING::iterator iter = listString.begin();

    for (; iter != listString.end(); ++iter)
    {
        yl::string strValue = (*iter).trim_both();

        vec.push_back(atoi(strValue.c_str()));
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////
// class XWINDOW_EXPORT xViewIterator
xViewIterator::xViewIterator(xView* pView)
{
    m_pHostView = pView;
}

bool xViewIterator::hasData() const
{
    return m_pHostView != NULL;
}
xViewIterator& xViewIterator::operator++()
{
    m_pHostView = m_pHostView->m_pNext;
    return *this;
}
xViewIterator& xViewIterator::operator--()
{
    m_pHostView = m_pHostView->m_pPrev;
    return *this;
}
xViewIterator xViewIterator::operator + (int iOffset) const
{
    xView* pView = m_pHostView;
    while (iOffset > 0 && pView != NULL)
    {
        pView = pView->m_pNext;
        -- iOffset;
    }
    while (iOffset < 0 && pView != NULL)
    {
        pView = pView->m_pPrev;
        ++ iOffset;
    }
    return  xViewIterator(pView);
}

//////////////////////////////////////////////////////////////////////////
// class XWINDOW_EXPORT xViewTree
xViewTree::xViewTree()
    : m_pRootView(NULL)
    , m_pParent(NULL)
    , m_pNext(NULL)
    , m_pPrev(NULL)
    , m_pChild(NULL)
    , m_pLastChild(NULL)
    , m_nChildreCount(0)
{

}

xView* xViewTree::getRootView()
{
    if (m_pRootView == NULL)
    {
        if (NULL != m_pParent)
        {
            XWINDOW_WARN("Layzy for getRootView()");
        }

        xView* pView = static_cast<xView*>(this);
        while (pView->m_pParent != NULL)
        {
            chASSERT(pView->m_pWindow == NULL);
            pView = pView->m_pParent;
        }
        m_pRootView = pView;
    }
    return m_pRootView;
}

void xViewTree::setRootView(xView* pRootView)
{
    if (m_pRootView != pRootView)
    {
        m_pRootView = pRootView;
        xView* pChildView = m_pChild;
        while (pChildView != NULL)
        {
            pChildView->setRootView(m_pRootView);
            pChildView = pChildView->m_pNext;
        }
    }
}

void xViewTree::push_child(xView* pChild)
{
    chASSERT(pChild != NULL && pChild->m_pParent == NULL);
    chASSERT(getRootView() != pChild); // loop check
    if (m_pChild == NULL)
    {
        m_pLastChild = m_pChild = pChild;
        m_pChild->m_pPrev = m_pChild->m_pNext = NULL;
        m_nChildreCount = 1;
    }
    else
    {
        pChild->m_pNext = NULL;
        pChild->m_pPrev = m_pLastChild;
        m_pLastChild->m_pNext = pChild;
        m_pLastChild = pChild;
        m_nChildreCount ++;
    }
    pChild->m_pParent = static_cast<xViewGroup*>(this);
    pChild->setRootView(getRootView());
}

void xViewTree::insert_child(int iIndex, xView* pChild)
{
    chASSERT(iIndex >= 0 && iIndex <= m_nChildreCount);
    chASSERT(pChild != NULL && pChild->m_pParent == NULL);
    chASSERT(getRootView() != pChild); // loop check
    if (m_pChild == NULL)
    {
        m_pLastChild = m_pChild = pChild;
        m_pChild->m_pPrev = m_pChild->m_pNext = NULL;
        m_nChildreCount = 1;
    }
    else
    {
        if (iIndex == 0)
        {
            pChild->m_pPrev = NULL;
            pChild->m_pNext = m_pChild;
            m_pChild->m_pPrev = pChild;
            m_pChild = pChild;
        }
        else if (iIndex == m_nChildreCount)
        {
            pChild->m_pNext = NULL;
            pChild->m_pPrev = m_pLastChild;
            m_pLastChild->m_pNext = pChild;
            m_pLastChild = pChild;
        }
        else
        {
            xView* pNext = m_pChild->m_pNext;
            while (--iIndex > 0)
            {
                pNext = pNext->m_pNext;
            }
            xView* pPrev = pNext->m_pPrev;
            pChild->m_pNext = pNext;
            pChild->m_pPrev = pPrev;
            pPrev->m_pNext = pChild;
            pNext->m_pPrev = pChild;
        }
        m_nChildreCount ++;
    }
    pChild->m_pParent = static_cast<xViewGroup*>(this);
    pChild->setRootView(getRootView());
}

void xViewTree::popup_child(xView* pChild)
{
    chASSERT(pChild->m_pParent == this);
    pChild->m_pParent = NULL;
    pChild->setRootView(NULL);
    if (m_nChildreCount == 1)
    {
        chASSERT(pChild == m_pChild && pChild == m_pLastChild);
        m_pChild = m_pLastChild = NULL;
    }
    else if (pChild == m_pChild)
    {
        m_pChild = m_pChild->m_pNext;
        pChild->m_pNext = NULL;
        m_pChild->m_pPrev = NULL;
    }
    else if (pChild == m_pLastChild)
    {
        m_pLastChild = m_pLastChild->m_pPrev;
        m_pLastChild->m_pNext = NULL;
        pChild->m_pPrev = NULL;
    }
    else
    {
        xView* pPrev = pChild->m_pPrev;
        xView* pNext = pChild->m_pNext;
        pPrev->m_pNext = pNext;
        pNext->m_pPrev = pPrev;
        pChild->m_pPrev = pChild->m_pNext = NULL;
    }
    -- m_nChildreCount;
}

int xViewTree::get_child_index(xView* pChild) const
{
    int iIndex = 0;
    xView* pView = m_pChild;
    while (pView != pChild)
    {
        if (pView == NULL)
        {
            return -1;
        }
        ++ iIndex;
        pView = pView->m_pNext;
    }
    return iIndex;
}

xView* xViewTree::child_at(int iIndex) const
{
    chASSERT(iIndex >= 0 && iIndex < m_nChildreCount);
    xView* pView = m_pChild;
    while (iIndex > 0)
    {
        pView = pView->m_pNext;
        -- iIndex;
    }
    return pView;
}

//////////////////////////////////////////////////////////////////////////
LayoutParams::LayoutParams()
{
    layout_width = layout_height = LayoutParams::match_parent;
    layout_marginLeft = layout_marginTop = layout_marginRight = layout_marginBottom = 0;
    paddingLeft = paddingRight = paddingTop = paddingBottom = 0;
}

LayoutParams::~LayoutParams()
{
}

void LayoutParams::loadAttributes(xml_node& node)
{
    int layout_margin = -1;

    PopupAndroidPixel(node, XWINDOW_NAME_LAYOUT_MARGIN, layout_margin);

    if (layout_margin != -1)
    {
        layout_marginLeft = layout_marginTop = layout_marginRight = layout_marginBottom = layout_margin;
    }

    PopupAndroidPixel(node, XWINDOW_NAME_LAYOUT_MARGIN_LEFT, layout_marginLeft);
    PopupAndroidPixel(node, XWINDOW_NAME_LAYOUT_MARGIN_RIGHT, layout_marginRight);
    PopupAndroidPixel(node, XWINDOW_NAME_LAYOUT_MARGIN_TOP, layout_marginTop);
    PopupAndroidPixel(node, XWINDOW_NAME_LAYOUT_MARGIN_BOTTOM, layout_marginBottom);

    PopupAndroidDimension(node, XWINDOW_NAME_LAYOUT_HEIGHT, layout_height);
    PopupAndroidDimension(node, XWINDOW_NAME_LAYOUT_WIDTH, layout_width);

    int padding = -1;

    PopupAndroidPixel(node, XWINDOW_NAME_PADDING, padding);

    if (padding != -1)
    {
        paddingLeft = paddingRight = paddingTop = paddingBottom = padding;
    }

    PopupAndroidPixel(node, XWINDOW_NAME_PADDING_LEFT, paddingLeft);
    PopupAndroidPixel(node, XWINDOW_NAME_PADDING_RIGHT, paddingRight);
    PopupAndroidPixel(node, XWINDOW_NAME_PADDING_TOP, paddingTop);
    PopupAndroidPixel(node, XWINDOW_NAME_PADDING_BOTTOM, paddingBottom);
}

//////////////////////////////////////////////////////////////////////////
LayoutParamsInParent::LayoutParamsInParent(int nType)
    : m_nType(nType)
{
    m_pHostView = NULL;
}

LayoutParamsInParent::~LayoutParamsInParent()
{

}

void LayoutParamsInParent::loadAttributes(xml_node& node)
{

}

LayoutParamsInParent* LayoutParamsInParent::GetLayoutParams(LayoutParamsInParent* pLayout)
{
    if (NULL != pLayout && LAYOUT_PARAMS_IN_PARENT_TYPE_BASE == pLayout->m_nType)
    {
        return pLayout;
    }

    return NULL;
}

//////////////////////////////////////////////////////////////////////////
//
xView::xView()
    : m_strId("")
    ,  m_pHandler(NULL)
    , visibility(xView::visible)    // invisible=0, visible=1, gone=2
    , focusable(bool_auto)         // bool_auto=auto bool_false=no-focus bool_true=strong-focus
    , m_bFocusPresent(false)
    , clickable(false)
    , overlap(ownerStation().profile().m_bOverlapMode)
#if IF_MULTI_TOUCH_ENABLE
    , touchable(false)
#endif
    , disable(false)
    , m_pWindow(NULL)
    , m_pExternAttributes(NULL)
    , m_pLayoutInParent(NULL)
    , m_pStyle(NULL)
    , m_bClosed(false)
    , m_nRelayoutAction(RELAYOUT_ACTION_NONE)
    , m_bAppear(false)
    , m_bDisable(false)
#if IF_VIEW_MASK
    , mask(false)
#endif
    , m_nAsyncListType(VIEW_ASYNC_LIST_TYPE_NONE)
#if IF_VIEW_RENDER
    , m_pRenderer(NULL)
#endif
{
    REGISTER_VIEW_DUMP_CLASS(xView)

    m_mapProperty.clear();
}

// way to destroy view
// 1.pView->close();
// 2.delete pView/View object on stack;
// 3.close then delete/view on stack
xView::~xView()
{
#if IF_VIEW_RENDER
    ReleaseRenderer();
#endif
#if IF_XKERNEL_THREAD_CHECK
    chASSERT(m_pHostStation->IsCalledByThisThread());
#endif
    if (!m_bClosed)
    {
        // way of 2
        // support delete root view only
        chASSERT(m_pParent == NULL);
        m_pHostStation->close_helper(this, false);
    }
    else if (IsInDeleteList())
    {
        // way 3
        m_pHostStation->m_listDeleteViews.remove(this);
        ResetInDeleteList();
    }
    else
    {
        // way 1

    }

    if (IsInRedrawList())
    {
        XWINDOW_ERR("xView::~xView, View[%s] is in refrawlist", m_strId.c_str());
        m_pHostStation->deleteRedrawRegion(this);
    }

    freeLayoutInParent();

    deleteChildrenViews();

    chASSERT(VIEW_ASYNC_LIST_TYPE_NONE == m_nAsyncListType);

    if (m_pHandler != NULL)
    {
        delete m_pHandler;
        m_pHandler = NULL;
    }
}

#if IF_VIEW_RENDER
void xView::ReleaseRenderer()
{
    if (NULL != m_pRenderer)
    {
        delete m_pRenderer;

        m_pRenderer = NULL;
    }
}
#endif

void xView::showValue(const yl::string& value)
{
    XWINDOW_WARN("noop of showValue(%s)", value.c_str());
}

#if IF_XWINDOW_DEBUG
void xView::dump(int iLevel)
{
    yl::string strSpace = "";

    if (iLevel > 0)
    {
        strSpace = yl::string(iLevel * 2, ' ');
    }

    XWINDOW_INFO("%s%c%s(%d,%d -> %d,%d)@%dx%d v:%d",
                 strSpace.c_str(), hasChild() ? '+' : '-', m_strId.c_str(),
                 m_rectLayout.left, m_rectLayout.top, m_rectLayout.right, m_rectLayout.bottom,
                 m_rectLayout.right, m_rectLayout.bottom, visibility, viewNick().c_str());

    for (xViewIterator it(m_pChild); it.hasData(); ++it)
    {
        it->dump(iLevel + 1);
    }
}

yl::string xView::viewNick()
{
    yl::string strResult;

    for (xView* pView = this; pView != NULL; pView = pView->m_pParent)
    {
        yl::string strViewName = ".";

        if (NULL != pView->m_pParent)
        {
            strViewName += commonAPI_FormatString("%d", pView->m_pParent->get_child_index(pView));
        }
        else
        {
            strViewName = "0";
        }

        if (!pView->m_strId.empty() && pView->m_strId[0] != '@')
        {
            strViewName = strViewName + "(" + pView->m_strId + ")";
        }

        strResult = strViewName + strResult;
    }

    return strResult;
}
#endif

xView* xView::getViewById(const yl::string& _id)
{
    if (m_strId == _id)
    {
        return this;
    }
    for (xViewIterator it = xViewTree::child_head(); it.hasData(); ++it)
    {
        xView* pResult = it->getViewById(_id);
        if (pResult != NULL)
        {
            return pResult;
        }
    }
    return NULL;
}

dataWindowObject* xView::getHostWindow()
{
    return m_pRootView != NULL ? m_pRootView->m_pWindow : NULL;
}

namespace
{
int _getViewById(xView* pView, const yl::string& id, xView**& outArray, int count /*= -1*/)
{
    int found_count = 0;
    if (pView->m_strId == id && count != 0)
    {
        if (outArray != NULL)
        {
            *outArray = pView;
            ++outArray;
        }
        ++found_count;
    }

    for (xViewIterator it = pView->child_head(); it.hasData(); ++it)
    {
        found_count += _getViewById(&*it, id, outArray, count - found_count);
        if (count == found_count)
        {
            break;
        }
    }
    return found_count;
}
}//namespace

int xView::getViewById(const yl::string& id, xView** outArray, int count /*= -1*/)
{
    return _getViewById(this, id, outArray, count);
}

ViewDataFrame xView::getProperty(const yl::string& strName)
{
    ViewPropertys::iterator iter = m_mapProperty.find(strName);

    if (iter != m_mapProperty.end())
    {
        return iter->second;
    }

    return ViewDataFrame(NULL);
}

void xView::setProperty(const yl::string& strName, ViewDataFrame pDataFrame)
{
    ViewDataFrame pOldDataFrame = getProperty(strName);

    if (pOldDataFrame.Get() == pDataFrame.Get())
    {
        return;
    }

    m_mapProperty[strName] = pDataFrame;
}

void xView::releaseProperty(const yl::string& strName)
{
    ViewPropertys::iterator iter = m_mapProperty.find(strName);

    if (iter != m_mapProperty.end())
    {
        m_mapProperty.erase(iter);
    }
}

bool xView::hasProperty(const yl::string& strName)
{
    ViewPropertys::iterator iter = m_mapProperty.find(strName);

    if (iter != m_mapProperty.end())
    {
        return true;
    }

    return false;
}

class StringExtraData : public ExtraData
{
public:
    StringExtraData() {}
    virtual ~StringExtraData() {}

public:
    yl::string m_strData;
};

bool xView::setStringProperty(const yl::string& strName, const yl::string& strValue)
{
    ViewDataFrame pDataFrame = getProperty(strName);

    if (pDataFrame.IsEmpty())
    {
        StringExtraData* pExtraData = new StringExtraData;

        if (NULL == pExtraData)
        {
            return false;
        }

        pExtraData->m_strData = strValue;

        pDataFrame = ViewDataFrame(pExtraData);

        setProperty(strName, pDataFrame);
    }
    else
    {
        StringExtraData* pExtraData = static_cast<StringExtraData*>(pDataFrame.Get());

        pExtraData->m_strData = strValue;
    }

    return true;
}

yl::string xView::getStringProperty(const yl::string& strName, const yl::string& strDef)
{
    ViewDataFrame pDataFrame = getProperty(strName);

    if (pDataFrame.IsEmpty())
    {
        return strDef;
    }

    StringExtraData* pExtraData = static_cast<StringExtraData*>(pDataFrame.Get());

    return pExtraData->m_strData;
}

void xView::setStyle(const yl::string& strStyle)
{
    if (strStyle.empty())
    {
        m_pStyle = NULL;
        return;
    }

    m_pStyle = xStyle::getObjectByName(strStyle);
}

void xView::setGeometry(const chRect& rect)
{
    chASSERT(m_pParent == NULL && m_pWindow != NULL);

    ownerStation().LayoutViewInBoundRect(this, rect);
}

chRect xView::geometry() const
{
    return m_rectLayout;
}

chRect xView::clientRect() const
{
    chRect rcClient = clientGeometry();

    rcClient.OffsetRect(-m_rectLayout.left, -m_rectLayout.top);

    return rcClient;
}


chRect xView::contentRect() const
{
    chRect rcContent = contentGeometry();

    rcContent.OffsetRect(-m_rectLayout.left, -m_rectLayout.top);

    return rcContent;
}

chRect xView::clientGeometry() const
{
    return m_rectLayout;
}

chRect xView::contentGeometry() const
{
    chRect rcContent = m_rectLayout;

    rcContent.DeflateRect(paddingLeft, paddingTop, paddingRight, paddingBottom);

    return rcContent;
}

bool xView::checkExposed(bool bInclueChildren) const
{
    if (m_bAppear)
    {
        xWorkStation& station = ownerStation();
        xRegion region(station.getVisibleRect(this));
        if (!region.empty())
        {
            ViewOrWindow vwView(const_cast<xView*>(this));
            ViewOrWindow vwNext = station.getNextViewOrWindow(vwView, bInclueChildren);
            return station.upwardUpdateFrom(vwNext, region, true);
        }
    }
    return false;
}

void xView::close()
{
    ownerStation().close_helper(this, true);
}

void xView::show(int nShowCmd/* = SHOW_CMD_DEFAULT*/)
{
    bool bParentContentChanged = visibility == xView::gone;
    ownerStation().show_helper(this, nShowCmd);
    if (bParentContentChanged && m_pParent != NULL)
    {
        m_pParent->onChildrenLayoutChanged();
    }
}

void xView::hide(bool bGone)
{
    bool bParentContentChanged = bGone && visibility != xView::gone;
    ownerStation().hide_helper(this, bGone);
    if (bParentContentChanged && m_pParent != NULL)
    {
        m_pParent->onChildrenLayoutChanged();
    }
}

void xView::update(bool bUpdateChildren)
{
    if (m_bAppear)
    {
        if (bUpdateChildren)
        {
            m_pHostStation->exposeView(this, true);
        }
        else
        {
            m_pHostStation->update_helper(this);
        }
    }
}

void xView::update(const chRect& rect, bool bUpdateChildren/* = false*/)
{
    if (m_bAppear)
    {
        if (bUpdateChildren)
        {
            m_pHostStation->exposeView(this, rect, true);
        }
        else
        {
            m_pHostStation->update_helper(this, rect);
        }
    }
}

bool xView::setFocus(int nFocusReason/* = FOCUS_REASON_DEFAULT*/)
{
    xView* pFocusView = NULL;

    if (FOCUS_REASON_DEFAULT == nFocusReason || FOCUS_REASON_RIGHT == nFocusReason
            || FOCUS_REASON_DOWN == nFocusReason)
    {
        pFocusView = xWorkStation::getStrongFocusChild(this, true);
    }
    else if (FOCUS_REASON_LEFT == nFocusReason || FOCUS_REASON_UP == nFocusReason)
    {
        pFocusView = xWorkStation::getStrongFocusChild(this, false);
    }
    else
    {
        return false;
    }

    if (NULL == pFocusView)
    {
        pFocusView = getDefaultFocusView();
    }

    if (xWorkStation::checkViewFocusble(pFocusView))
    {
        ownerStation().setFocus_helper(pFocusView);
        return true;
    }

    return false;
}

xView* xView::getDefaultFocusView()
{
    return this;
}

bool xView::hasFocus() const
{
    return ownerStation().isFocusView_helper(this);
}

bool xView::hasFocused() const
{
    xWorkStation& objStation = ownerStation();

    return (objStation.isFocusView_helper(this) && objStation.isFocusedView_helper(this));
}

bool xView::underFocus() const
{
    xView* pFocus = ownerStation().getFocusView();
    return pFocus == this || xWorkStation::containChildView(pFocus, this);
}

bool xView::underFocused() const
{
    xWorkStation& objStation = ownerStation();
    xView* pFocus = objStation.getFocusView();

    if (NULL == pFocus || !objStation.isFocusedView_helper(pFocus))
    {
        return false;
    }

    return (pFocus == this || xWorkStation::containChildView(pFocus, this));
}

bool xView::containFocus() const
{
    xView* pFocus = ownerStation().getFocusView();
    return pFocus == this || xWorkStation::containChildView(this, pFocus);
}

bool xView::containFocused() const
{
    xWorkStation& objStation = ownerStation();
    xView* pFocus = objStation.getFocusView();

    if (NULL == pFocus || !objStation.isFocusedView_helper(pFocus))
    {
        return false;
    }

    return (pFocus == this || xWorkStation::containChildView(this, pFocus));
}

#if IF_MULTI_TOUCH_ENABLE
void xView::setTouchable(bool bTouchable)
{
    touchable = bTouchable;

    for (xViewIterator it = child_head(); it.hasData(); ++it)
    {
        xView* pChild = it.operator ->();

        if (NULL != pChild)
        {
            pChild->setTouchable(bTouchable);
        }
    }
}
#endif

void xView::activateWindow()
{
    chASSERT(m_pParent == NULL && m_pWindow != NULL);
    ownerStation().setActiveWindowObject(m_pWindow);
}
bool xView::isActiveWindow()
{
    return ownerStation().getActiveWindow() == this;
}

#if IF_VIEW_MASK
void xView::setMask(bool bMask, bool bUpdate/* = true*/)
{
    if (mask != bMask)
    {
        mask = bMask;

        if (bUpdate)
        {
            update(true);
        }
    }
}
#endif

void xView::setBackground(const xColor& col)
{
    background.setColor(col);
    update(false);
}

void xView::setBackground(const xPixmap& pixmap)
{
    background.setPixmap(pixmap);
    update(false);
}

void xView::setDisable(bool bDisable)
{
    // update attribute
    disable = bDisable;

    // update status
    // use parent status at enable action
    if (m_pParent != NULL && !bDisable)
    {
        bDisable = m_pParent->m_bDisable;
    }
    if (m_bDisable != (UINT)bDisable)
    {
        m_pHostStation->recursiveViewDisable(this, bDisable);
        update(false);
    }
}

void xView::setOverlapMode(bool bOverlap)
{
    if (overlap != bOverlap)
    {
        overlap = bOverlap;
        if (m_pWindow != NULL)
        {
            m_pWindow->overlap = bOverlap;
        }
        update(false);
    }
}

void xView::setViewHandler(xViewHandler* pHandler)
{
    chASSERT(pHandler != NULL && pHandler->m_pNextHandler == NULL);
    if (m_pHandler == NULL)
    {
        m_pHandler = pHandler;
    }
    else
    {
        xViewHandler* pLastHandler = m_pHandler;

        while (NULL != pLastHandler)
        {
            if (pLastHandler == pHandler)
            {
                chASSERT(false);
            }

            if (NULL == pLastHandler->m_pNextHandler)
            {
                pLastHandler->m_pNextHandler = pHandler;
                break;
            }

            pLastHandler = pLastHandler->m_pNextHandler;
        }

    }
    pHandler->m_pHostView = this;
    pHandler->onViewConnected();
}

void xView::loadXmlContent(xml_node& node)
{
    PopupAndroidString(node, XWINDOW_NAME_ID, m_strId);

    // TBD: load m_pExternAttributes here
    // ???

    xXmlLoader::loadXmlContent(node);

    if (m_pParent != NULL || node.attributes_begin() != node.attributes_end())
    {
        LayoutParamsInParent* pLayoutInParent = getLayoutInParent();
        if (pLayoutInParent != NULL)
        {
            pLayoutInParent->loadAttributes(node);
        }
    }

    for (xViewGroup* pParent = m_pParent;
            pParent != NULL && node.attributes_begin() != node.attributes_end();
            pParent = pParent->m_pParent)
    {
        pParent->loadAttributesForContentView(this, node);
    }

#if IF_DEBUG_PARSER
    chASSERTx((node.empty() || node.first_attribute().empty()), "Unknow attribute:%s of %s",
              GetNodeAttrString(node).c_str(), viewNick().c_str());
#endif
}

void xView::loadAttributes(xml_node& node)
{
    LayoutParams::loadAttributes(node);

    PopupAndroidBrush(node, XWINDOW_NAME_BACKGROUND, background);

    yl::string style;

    PopupAndroidString(node, XWINDOW_NAME_STYLE, style);

    if (!style.empty())
    {
        m_pStyle = xStyle::getObjectByName(style);
    }

    PopupAndroidBOOL(node, XWINDOW_NAME_FOCUSABLE, focusable);
    PopupAndroidBool(node, XWINDOW_NAME_FOCUS_PRESENT, m_bFocusPresent);
    PopupAndroidBool(node, XWINDOW_NAME_CLICKABLE, clickable);
    PopupAndroidBool(node, XWINDOW_NAME_OVERLAP, overlap);
    PopupAndroidBool(node, XWINDOW_NAME_DISABLE, disable);
#if IF_MULTI_TOUCH_ENABLE
    PopupAndroidBool(node, XWINDOW_NAME_TOUCHABLE, touchable);
#endif
#if IF_VIEW_MASK
    PopupAndroidBool(node, XWINDOW_NAME_MASK, mask);
#endif
    if (disable)
    {
        // sync children m_bDisable with this
        m_pHostStation->recursiveViewDisable(this, disable);
    }

    PopupAndroidVisibility(node, XWINDOW_NAME_VISIBLITY, visibility);

#if IF_MOVE_FOCUS_BY_KEY
    // 下一个焦点不能是自己
    yl::string nextFocusUp;

    PopupAndroidString(node, XWINDOW_NAME_NEXT_FOCUS_UP, nextFocusUp);

    if (!nextFocusUp.empty())
    {
        setStringProperty(XWINDOW_NAME_NEXT_FOCUS_UP, nextFocusUp);

        chASSERT(this->m_strId != nextFocusUp);
    }

    yl::string nextFocusDown;

    PopupAndroidString(node, XWINDOW_NAME_NEXT_FOCUS_DOWN, nextFocusDown);

    if (!nextFocusDown.empty())
    {
        setStringProperty(XWINDOW_NAME_NEXT_FOCUS_DOWN, nextFocusDown);

        chASSERT(this->m_strId != nextFocusDown);
    }

    yl::string nextFocusLeft;

    PopupAndroidString(node, XWINDOW_NAME_NEXT_FOCUS_LEFT, nextFocusLeft);

    if (!nextFocusLeft.empty())
    {
        setStringProperty(XWINDOW_NAME_NEXT_FOCUS_LEFT, nextFocusLeft);

        chASSERT(this->m_strId != nextFocusLeft);
    }

    yl::string nextFocusRight;

    PopupAndroidString(node, XWINDOW_NAME_NEXT_FOCUS_RIGHT, nextFocusRight);

    if (!nextFocusRight.empty())
    {
        setStringProperty(XWINDOW_NAME_NEXT_FOCUS_RIGHT, nextFocusRight);

        chASSERT(this->m_strId != nextFocusRight);
    }

#if IF_PC_ENABLE
    yl::string nextFocusPrevious;

    PopupAndroidString(node, XWINDOW_NAME_NEXT_FOCUS_PREVIOUS, nextFocusPrevious);

    if (!nextFocusPrevious.empty())
    {
        setStringProperty(XWINDOW_NAME_NEXT_FOCUS_PREVIOUS, nextFocusPrevious);

        chASSERT(m_strId != nextFocusPrevious);
    }

    yl::string nextFocusNext;

    PopupAndroidString(node, XWINDOW_NAME_NEXT_FOUCS_NEXT, nextFocusNext);

    if (!nextFocusNext.empty())
    {
        setStringProperty(XWINDOW_NAME_NEXT_FOUCS_NEXT, nextFocusNext);

        chASSERT(m_strId != nextFocusNext);
    }
#endif
#endif

#if IF_VIEW_USER_ACTION
    yl::string strOnClick;

    PopupAndroidString(node, XWINDOW_NAME_ON_CLICK, strOnClick);

    if (!strOnClick.empty())
    {
        setStringProperty(XWINDOW_NAME_ON_CLICK, strOnClick);
    }
#endif

#if IF_VIEW_RENDER
    yl::string renderer;

    PopupAndroidString(node, XWINDOW_NAME_RENDERER, renderer);

    if (!renderer.empty())
    {
        ReleaseRenderer();

        m_pRenderer = new xViewRenderer;

        if (NULL != m_pRenderer)
        {
            m_pRenderer->LoadContent(renderer);
        }
    }
#endif
}

void xView::onLoadChildElement(xml_node& nodeChild)
{
    if (nodeChild.empty())
    {
        return;
    }

    yl::string strName = nodeChild.name();
    xViewHandler* pHandler = xViewHandler::getObjectByName(strName);
    if (pHandler != NULL)
    {
        setViewHandler(pHandler);
        pHandler->loadXmlContent(nodeChild);
    }
    else
    {
#if IF_VIEW_RENDER
        if (strName == "Renderer")
        {
            ReleaseRenderer();

            m_pRenderer = new xViewRenderer;

            if (NULL != m_pRenderer)
            {
                m_pRenderer->LoadElement(nodeChild);
            }

            return;
        }
#endif

        m_pHostStation->signalCustomElement.emitSignal((WPARAM)this, (LPARAM)&nodeChild, 0, NULL);
    }
}

#if IF_GESTURE_ENABLE
void xView::grabGesture(int nGestureType)
{
    if (g_GestureManager.RegisterGesture(this, nGestureType))
    {
        touchable = true;
    }
}

void xView::ungrabGesture(int nGestureType)
{
    g_GestureManager.UnRegisterGesture(this, nGestureType);
}

void xView::clearGesture()
{
    g_GestureManager.ClearGesture(this);
}
#endif

bool xView::onEvent(xViewEvent* pEvent)
{
    switch (pEvent->type)
    {
    case VIEW_KEY_PRESS:
        return onKeyPressedEvent((static_cast<xViewEvent_key*>(pEvent))->value_key);
    case VIEW_KEY_RELEASE:
        return onKeyReleasedEvent((static_cast<xViewEvent_key*>(pEvent))->value_key);
    case VIEW_KEY_LONG_PRESS:
        return onKeyLongPressedEvent((static_cast<xViewEvent_key*>(pEvent))->value_key);

#if IF_MULTI_TOUCH_ENABLE
    case VIEW_TOUCH_BEGAN:
        {
            xTouchEvent* pTouchEvent = static_cast<xTouchEvent*>(pEvent);

            return onTouchBeganEvent(pTouchEvent->m_pAnchorView, pTouchEvent->m_listPoint);
        }
        break;
    case VIEW_TOUCH_MOVED:
        {
            xTouchEvent* pTouchEvent = static_cast<xTouchEvent*>(pEvent);

            return onTouchMovedEvent(pTouchEvent->m_pAnchorView, pTouchEvent->m_listPoint);
        }
        break;
    case VIEW_TOUCH_ENDED:
        {
            xTouchEvent* pTouchEvent = static_cast<xTouchEvent*>(pEvent);

            return onTouchEndedEvent(pTouchEvent->m_pAnchorView, pTouchEvent->m_listPoint);
        }
        break;
    case VIEW_TOUCH_CANCEL:
        {
            xTouchEvent* pTouchEvent = static_cast<xTouchEvent*>(pEvent);

            return onTouchCancelEvent(pTouchEvent->m_pAnchorView, pTouchEvent->m_listPoint);
        }
        break;
#endif
#if IF_GESTURE_ENABLE
    case VIEW_GESTURE:
        {
            return onGestureEvent(static_cast<xGestureEvent*>(pEvent));
        }
        break;
#endif

    case VIEW_FOCUS_CUSTOM:
        return onFocusCustom((static_cast<xViewEvent_int*>(pEvent))->value_int);
    case VIEW_FOCUS_ENTER:
        return onFocusEvent(true);
    case VIEW_FOCUS_LEAVE:
        return onFocusEvent(false);
    case VIEW_FOCUS_MOVE:
        return onFocusMoveEvent((static_cast<xViewEvent_FocusMove*>(pEvent))->m_pNewFocus,
                                (static_cast<xViewEvent_FocusMove*>(pEvent))->m_pOldFocus);
    case VIEW_ACTIVE:
        return onActiveEvent((static_cast<xViewEvent_bool*>(pEvent))->value_bool);
    case VIEW_PAINT_BACKGROUND:
        return onPaintBackground((static_cast<xViewEvent_paint*>(pEvent))->value_painter);
    case VIEW_PAINT_CONTENT:
        return onPaintContent((static_cast<xViewEvent_paint*>(pEvent))->value_painter);
#if IF_VIEW_MASK
    case VIEW_PAINT_MASK:
        return onPaintMask((static_cast<xViewEvent_paint*>(pEvent))->value_painter);
#endif

    default:
        return false;
    }
    return false;
}

bool xView::onKeyPressedEvent(int /*key*/)
{
    return false;
}
bool xView::onKeyReleasedEvent(int /*key*/)
{
    return false;
}
bool xView::onKeyLongPressedEvent(int /*key*/)
{
    return false;
}

#if IF_MULTI_TOUCH_ENABLE
bool xView::onTouchBeganEvent(xView* pAnchorView, LIST_RAW_TOUCH_POINT& lsTouchPoint)
{
    return false;
}
bool xView::onTouchMovedEvent(xView* pAnchorView, LIST_RAW_TOUCH_POINT& lsTouchPoint)
{
    return false;
}
bool xView::onTouchEndedEvent(xView* pAnchorView, LIST_RAW_TOUCH_POINT& lsTouchPoint)
{
    return false;
}
bool xView::onTouchCancelEvent(xView* pAnchorView, LIST_RAW_TOUCH_POINT& lsTouchPoint)
{
    return false;
}
#endif

#if IF_GESTURE_ENABLE
bool xView::onGestureEvent(xGestureEvent* pEvent)
{
    return false;
}
#endif

bool xView::onActiveEvent(bool /*bActive*/)
{
    return false;
}
bool xView::onFocusCustom(int nFocusReason)
{
    return false;
}
bool xView::onFocusEvent(bool /*bFocusIn*/)
{
    return false;
}
bool xView::onFocusMoveEvent(xView* pNewFocus, xView* pOldFocus)
{
    return false;
}

int xView::wrapContentWidth()
{
    chASSERT(FALSE);
    return 0;
}
int xView::wrapContentHeight()
{
    chASSERT(FALSE);
    return 0;
}
void xView::LayoutContentInRect(const chRect&)          { }

int xView::wrapFrameWidth()
{
    return layout_width > 0 ? layout_width : (paddingLeft + wrapContentWidth() + paddingRight);
}

int xView::wrapFrameHeight()
{
    return layout_height > 0 ? layout_height : (paddingTop + wrapContentHeight() + paddingBottom);
}

int xView::wrapBoundWidth()
{
    return layout_marginLeft + wrapFrameWidth() + layout_marginRight;
}

int xView::wrapBoundHeight()
{
    return layout_marginTop + wrapFrameHeight() + layout_marginBottom;
}

#define VIEW_FIXED_SIZE(pView)      (pView->layout_width > 0 && pView->layout_height > 0)
#define VIEW_WRAP_CONTENT(pView)    (pView->layout_width == LayoutParams::wrap_content || pView->layout_height == LayoutParams::wrap_content)

void xView::onContentChanged()
{
    // No need re-layout without root window created
    if (getHostWindow() != NULL)
    {
        // 如果有重新布局请求，则刷新区域的计算都放到布局之后
        if (!relaoutWrapContentParent())
        {
            ownerStation().update_helper(this);
        }
    }
}

bool xView::relaoutWrapContentParent()
{
    // DO NOT re-layout without root window created
    chASSERT(getHostWindow() != NULL);
    // up seek the last wrap_content parent view and flag the views between this
    xView* pLastWrapContentView = NULL;
    xView* pViewToBeRelayout = NULL;
    for (xView* pStretchView = this; pStretchView != NULL; pStretchView = pStretchView->m_pParent)
    {
        if (pStretchView->IsForceRelayout())
        {
            pViewToBeRelayout = pStretchView;
            pLastWrapContentView = NULL;
            break;
        }
        if (VIEW_WRAP_CONTENT(pStretchView))
        {
            pViewToBeRelayout = pStretchView;
            pLastWrapContentView = pStretchView;
        }
        else if (VIEW_FIXED_SIZE(pStretchView))
        {
            break;
        }
    }
    if (pViewToBeRelayout != NULL)
    {
        // flag all views from this to pViewToBeRelayout
        for (xView* pView = this; pView != pViewToBeRelayout; pView = pView->m_pParent)
        {
            pView->m_nRelayoutAction |= RELAYOUT_ACTION_FORCE;
        }
    }
    return m_pHostStation->asyncRelayoutViewBound(pLastWrapContentView);
}

int xView::measureWidthWithBound(int widthBound)
{
    int result = widthBound;
    switch (layout_width)
    {
    // case 0:  // weight mode
    case LayoutParams::match_parent:
        break;
    case LayoutParams::wrap_content:
        result = wrapBoundWidth();
        break;
    default:
        chWARNING(layout_width > 0);
        result = layout_marginLeft + layout_width + layout_marginRight;
        break;
    }
    return result;
}

int xView::measureHeightWithBound(int heightBound)
{
    int result = heightBound;
    switch (layout_height)
    {
    // case 0:  // weight mode
    case LayoutParams::match_parent:
        break;
    case LayoutParams::wrap_content:
        result = wrapBoundHeight();
        break;
    default:
        chWARNING(layout_height > 0);
        result = layout_marginTop + layout_height + layout_marginBottom;
        break;
    }
    return result;
}

void xView::LayoutInBoundRect(const chRect& rectBound)
{
    chRect rectFrame = DeflateRect(rectBound, layout_marginLeft, layout_marginTop,
                                   layout_marginRight, layout_marginBottom);
    bool bForceRelayout = IsForceRelayout();

    ResetRelayoutAction();

    if (bForceRelayout || rectFrame != m_rectLayout)
    {
        chRect rectOld = m_rectLayout;

        m_pHostStation->viewRectChanged(this, rectOld, rectFrame);
        m_rectLayout = rectFrame;

        if (IsInRelayoutList())
        {
            m_pHostStation->m_listRelayoutViews.remove(this);
            ResetInRelayoutList();
        }
        if (m_pWindow != NULL)
        {
            m_pWindow->rect = rectFrame;
        }

        onRectChanged(rectOld);

        if (m_rectLayout != rectFrame)
        {
            XWINDOW_ERR("DO not reset m_rectLayout in onRectChanged Id:%s", m_strId.c_str());
        }

        chRect rectContentBound = DeflateRect(rectFrame, paddingLeft, paddingTop, paddingRight, paddingBottom);
        LayoutContentInRect(rectContentBound);
    }
}

bool xView::onPaintBackground(xPainter& p)
{
    if (NULL != m_pStyle && m_pStyle->onViewPaintBackground(this, p))
    {
        return true;
    }

    chRect rcClient = clientRect();

#if IF_DEBUG_PAINTER
    XWINDOW_INFO("onPaintBackground id:%s Rect:%d %d %d %d", m_strId.c_str(), rcClient.left,
                 rcClient.top, rcClient.right, rcClient.bottom);
#endif

    PaintBackground(p, rcClient);

    return true;
}

void xView::PaintBackground(xPainter& p, const chRect& rcBG)
{
    if (overlap)
    {
        if (!background.isNull())
        {
            p.blendRect(rcBG, background);
        }
    }
    else
    {
        // overwrite mode
        if (!background.isNull())
        {
            p.fillRect(rcBG, background);
        }
        else
        {
            p.fillRect(rcBG, ownerStation().profile().m_clrViewDefaultBG);
        }
    }
}

bool xView::onPaintContent(xPainter& p)
{
    if (NULL != m_pStyle && m_pStyle->onViewPaintContent(this, p))
    {
        return true;
    }

#if IF_VIEW_RENDER
    if (NULL != m_pRenderer)
    {
        m_pRenderer->Render(p, this);

        return true;
    }
#endif

    return false;
}

#if IF_VIEW_MASK
bool xView::onPaintMask(xPainter& p)
{
    if (NULL != m_pStyle && m_pStyle->onViewPaintMask(this, p))
    {
        return true;
    }

    return false;
}
#endif

bool xView::onRectChanged(const chRect& rectOld)
{
    if (NULL != m_pStyle)
    {
        return m_pStyle->onViewRectChanged(this, rectOld);
    }

    return false;
}

void xView::onClick()
{
#if IF_VIEW_USER_ACTION
    yl::string strAction = getStringProperty(XWINDOW_NAME_ON_CLICK, "");

    if (!strAction.empty())
    {
        call_text_method(strAction.c_str());
    }
#endif
}

xView* xView::LoadView(const yl::string& strFielName)
{
    return g_WorkStation.LoadView(strFielName);
}

xPixmap xView::LoadPixmap(const yl::string& strFielName)
{
    return g_WorkStation.LoadPixmap(strFielName);
}

int xView::GetFocusReasonByKey(int nKey)
{
    if (PHONE_KEY_LEFT == nKey)
    {
        return FOCUS_REASON_LEFT;
    }
    else if (PHONE_KEY_RIGHT == nKey)
    {
        return FOCUS_REASON_RIGHT;
    }
    else if (PHONE_KEY_UP == nKey)
    {
        return FOCUS_REASON_UP;
    }
    else if (PHONE_KEY_DOWN == nKey)
    {
        return FOCUS_REASON_DOWN;
    }

    return FOCUS_REASON_NONE;
}

void xView::setLayoutInParent(LayoutParamsInParent* pLayoutInParent)
{
    if (NULL != m_pLayoutInParent)
    {
        freeLayoutInParent();
    }

    m_pLayoutInParent = pLayoutInParent;
}

LayoutParamsInParent* xView::getLayoutInParent()
{
    if (NULL == m_pParent)
    {
        // root view only
        if (NULL != m_pLayoutInParent && NULL == RootLayoutParams::GetLayoutParams(m_pLayoutInParent))
        {
            XWINDOW_WARN("RootLayoutParams::GetLayoutParams fail in getLayoutInParent");
            setLayoutInParent(NULL);
            chASSERT(false);
        }

        if (NULL == m_pLayoutInParent)
        {
            m_pLayoutInParent = new RootLayoutParams();
            m_pLayoutInParent->m_pHostView = this;
        }
    }
    else
    {
        if (NULL != m_pLayoutInParent && !m_pParent->checkLayout(m_pLayoutInParent))
        {
            XWINDOW_WARN("checkLayout fail in getLayoutInParent");
            setLayoutInParent(NULL);
        }

        if (NULL == m_pLayoutInParent)
        {
            m_pLayoutInParent = m_pParent->allocLayout();

            if (NULL != m_pLayoutInParent)
            {
                // for ScrollLayout
                m_pLayoutInParent->m_pHostView = this;
            }
        }
    }
    return m_pLayoutInParent;
}

bool xView::releaseFocusInWindow()
{
    xView* pRootView = getRootView();

    if (NULL == pRootView)
    {
        return false;
    }

    dataWindowObject* pWindow = pRootView->m_pWindow;

    if (NULL == pWindow)
    {
        return false;
    }

    if (pWindow->focusView == this || xWorkStation::containChildView(this, pWindow->focusView))
    {
        xViewGroup* pParentView = m_pParent;
        xView* pFocusView = NULL;
        int nLoop = 0;

        while (NULL != pParentView)
        {
            DEAD_LOOP_BREAK(nLoop)

            if (pParentView->m_bAppear && !pParentView->m_bDisable && xView::bool_false != pParentView->focusable)
            {
                pFocusView = pParentView;
                break;
            }

            pParentView = pParentView->m_pParent;
        }

        pWindow->focusView = pFocusView;

        return true;
    }

    return false;
}

void xView::freeLayoutInParent()
{
    if (m_pLayoutInParent != NULL)
    {
        delete m_pLayoutInParent;
        m_pLayoutInParent = NULL;
    }
}

void xView::deleteChildrenViews()
{
    if (m_pChild != NULL)
    {
        while (m_pChild != NULL)
        {
            xView* pChildView = m_pChild;
            xViewTree::popup_child(pChildView);

            chASSERT(pChildView->m_bClosed);
            delete pChildView;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// class ViewGroup : public View
xViewGroup::xViewGroup()
{
}

xViewGroup::~xViewGroup()
{
}

void xViewGroup::loadContent(const yl::string& strFileName)
{
    if (NULL == m_pHostStation)
    {
        return;
    }

    xml_document doc;

    if (!m_pHostStation->LoadLayout(strFileName, doc))
    {
        return;
    }

#if IF_DEBUG_PARSER
    XWINDOW_INFO("loadContent:%s", strFileName.c_str());
#endif

    xml_node nodeRoot = doc.document_element();

    if (!nodeRoot.empty())
    {
        loadXmlContent(nodeRoot);
    }
}

void xViewGroup::replaceChildView(const yl::string& id, xView* newChild)
{
    xView* oldChild = getViewById(id);
    if (oldChild != NULL)
    {
        bool bChildVisible = oldChild->m_bAppear;

        // copy all profile except tree info
        xViewTree treeNewChild = *newChild;
        *newChild = *oldChild;
        *(static_cast<xViewTree*>(newChild)) = treeNewChild;
        oldChild->setLayoutInParent(NULL);

        xViewGroup* pParent = oldChild->m_pParent;
        int iIndex = pParent->get_child_index(oldChild);
        if (bChildVisible)
        {
            m_pHostStation->releaseFocusOnViewDisappear(oldChild);
            m_pHostStation->unexposeView(oldChild);
            m_pHostStation->recursiveViewAppear(oldChild, false);
        }

        pParent->popup_child(oldChild);
        delete oldChild;

        pParent->insert_child(iIndex, newChild);
        if (bChildVisible)
        {
            m_pHostStation->recursiveViewAppear(newChild, true);
            m_pHostStation->exposeView(newChild, false);
        }
        pParent->onChildrenLayoutChanged();
    }
}

void xViewGroup::addChildView(xView* view)
{
    xViewTree::push_child(view);
    view->getLayoutInParent();      // speed up: new m_pLayoutInParent for later used

    OnChildAppend(view);
}

void xViewGroup::insertChildView(int iIndex, xView* view)
{
    xViewTree::insert_child(iIndex, view);
    view->getLayoutInParent();      // speed up: new m_pLayoutInParent for later used

    OnChildAppend(view);
}

void xViewGroup::OnChildAppend(xView* pView)
{
    if (m_bAppear && xView::visible == pView->visibility)
    {
        m_pHostStation->recursiveViewAppear(pView, true);

        if (!pView->m_rectLayout.IsEmpty())
        {
            m_pHostStation->exposeView(pView, false);
        }
    }
    onChildrenLayoutChanged();
}

void xViewGroup::popupChildView(xView* pView)
{
    if (pView->m_bAppear)
    {
        m_pHostStation->releaseFocusOnViewDisappear(pView);
        m_pHostStation->unexposeView(pView);
        m_pHostStation->recursiveViewAppear(pView, false);
    }
    else
    {
        pView->releaseFocusInWindow();
    }

    pView->freeLayoutInParent();
    xViewTree::popup_child(pView);
    onChildrenLayoutChanged();
}

void xViewGroup::closeAllChildren()
{
    xView* pRootView = getRootView();
    if (pRootView->m_pWindow != NULL && xWorkStation::containChildView(this, pRootView->m_pWindow->focusView))
    {
        this->setFocus();
        pRootView->m_pWindow->focusView = this;
    }

    for (xView* pChild = m_pChild; pChild != NULL; pChild = pChild->m_pNext)
    {
        if (pChild->m_bAppear)
        {
            m_pHostStation->recursiveViewAppear(pChild, false);
        }

        pChild->m_bClosed = true;
        m_pHostStation->recursiveViewClosed(pChild);
    }

    this->deleteChildrenViews();
    onChildrenLayoutChanged();
}

void xViewGroup::moveChildToTop(xView* pChild)
{
    chASSERT(pChild->m_pParent == this);
    if (pChild != m_pLastChild)
    {
        xViewTree::popup_child(pChild);
        xViewTree::push_child(pChild);
        ownerStation().asyncRelayoutViewBound(pChild);
    }
}

void xViewGroup::moveChildToBottom(xView* pChild)
{
    chASSERT(pChild->m_pParent == this);
    if (pChild != m_pChild)
    {
        xViewTree::popup_child(pChild);
        xViewTree::insert_child(0, pChild);
        ownerStation().asyncRelayoutViewBound(pChild);
    }
}

void xViewGroup::onChildrenLayoutChanged()
{
    if (getHostWindow() != NULL)
    {
        if (!xView::relaoutWrapContentParent())
        {
            ownerStation().asyncRelayoutViewContent(this);
        }
    }
}

void xViewGroup::onLoadChildElement(xml_node& nodeChild)
{
    if (nodeChild.empty())
    {
        return;
    }

    yl::string strName = nodeChild.name();
    xView* pView = xView::getObjectByName(strName);

    if (NULL != pView)
    {
        // add to tree for getRootView() in loadXmlContent
        xViewTree::push_child(pView);

        pView->getLayoutInParent();     // speed up: new m_pLayoutInParent for later used

        // TBD: child should inherit m_pExternAttributes of parents-family
        // 以后需要实现View可以使用外部的字体/字号/颜色/Padding等风格参数，并且这些参数可以被子View默认继承

        pView->loadXmlContent(nodeChild);

        OnChildAppend(pView);
#if IF_DEBUG_PARSER
        if (!nodeChild.first_attribute().empty())
        {
            XWINDOW_WARN("Unknow attributes:{%s} of %s",
                         GetNodeAttrString(nodeChild).c_str(), pView->viewNick().c_str());
        }
#endif
    }
    else
    {
        xView::onLoadChildElement(nodeChild);
    }
}

//////////////////////////////////////////////////////////////////////////
// class ImageView : public View
#if IF_IMAGE_VIEW_ENABLE
EXPORT_VIEW_CLASS(xImageView);

xImageView::xImageView()
{
    REGISTER_VIEW_DUMP_CLASS(xImageView)

#if IF_XWINDOW_DUMP_EXPORT
    m_strSrc = "";
#endif
#if IF_IMAGE_VIEW_BORDER
    m_nBorderWidth = 0;
    m_clrBorder = xColor(0, 0, 0);
#endif
#if IF_INVERT_EFFECT
    invert = xView::bool_false;
#endif
    // background.setColor(xColor(255, 255, 255));
}

#if IF_IMAGE_VIEW_KEEP_PIXMAP_RATIO
bool xImageView::GetRatioPixmap(const xPixmap& pmSrc, const chRect& rcDst, xPixmap& pmResult)
{
    const xGraphicPtr& pGraphic = pmSrc.GetGraphic();

    if (pGraphic.IsEmpty())
    {
        return false;
    }

    int nPixWidth = pmSrc.width();
    int nPixHeight = pmSrc.height();
    int nDstWidth = rcDst.Width();
    int nDstHeight = rcDst.Height();

    if ((nPixWidth <= nDstWidth && nPixHeight <= nDstHeight)
            || pmSrc.isNull() || 0 == nPixWidth || 0 == nPixHeight)
    {
        return false;
    }

    // Find the ideal x / y scaling ratio to fit \a source into \a target.
    double dXRatio = (double)nDstWidth / (double)nPixWidth;
    double dYRatio = (double)nDstHeight / (double)nPixHeight;
    dXRatio = dYRatio = chMIN(dXRatio, dYRatio);
    int nScaleWidth = nPixWidth * dXRatio;
    int nScaleHeight = nPixHeight * dYRatio;

    pmResult.Create(nScaleWidth, nScaleHeight, pGraphic->GetFormat());
    pmResult.ScaleCopy(pmSrc);

    return true;
}
#endif

#if IF_IMAGE_VIEW_KEEP_PIXMAP_RATIO
bool xImageView::setPixmap(const yl::string& file, bool bKeepRatio /*= false*/)
#else
bool xImageView::setPixmap(const yl::string& file)
#endif
{
    if (NULL == m_pHostStation)
    {
        return false;
    }

#if IF_XWINDOW_DUMP_EXPORT
    m_strSrc = file;
#endif

    m_pHostStation->GetImageByPath(file, src);
#if IF_IMAGE_VIEW_KEEP_PIXMAP_RATIO
    if (bKeepRatio)
    {
        xPixmap pix;
        if (GetRatioPixmap(src, contentRect(), pix))
        {
            src = pix;
        }
    }
#endif

    onContentChanged();

    return true;
}
#if IF_IMAGE_VIEW_KEEP_PIXMAP_RATIO
bool xImageView::setPixmap(const xPixmap& pixmap, bool bKeepRatio /*= false*/)
#else
bool xImageView::setPixmap(const xPixmap& pixmap)
#endif
{
#if IF_XWINDOW_DUMP_EXPORT
    m_strSrc = "";
#endif

#if IF_IMAGE_VIEW_KEEP_PIXMAP_RATIO
    if (!bKeepRatio || !GetRatioPixmap(pixmap, contentRect(), src))
#endif
    {
        src = pixmap;
    }

    onContentChanged();
    return true;
}

void xImageView::showValue(const yl::string& src)
{
    setPixmap(src);
}

#if IF_INVERT_EFFECT
void xImageView::setInvert(bool bInvert/* = true*/)
{
    if (bInvert != (bool)invert)
    {
        invert = bInvert ? xView::bool_true : xView::bool_false;
        onContentChanged();
    }
}
#endif

void xImageView::loadAttributes(xml_node& node)
{
    xView::loadAttributes(node);

#if IF_IMAGE_VIEW_SUPPORT_GRAVITY
    PopupAndroidGravity(node, XWINDOW_NAME_GRAVITY, gravity);
#endif
    PopupAndroidPixmap(node, XWINDOW_NAME_SRC, src);
#if IF_INVERT_EFFECT
    PopupAndroidBOOL(node, XWINDOW_NAME_INVERT, invert);
#endif

#if IF_IMAGE_VIEW_BORDER
    PopupAndroidInt(node, XWINDOW_NAME_BORDER_WIDTH, m_nBorderWidth);
    PopupAndroidColor(node, XWINDOW_NAME_BORDER_COLOR, m_clrBorder);
#endif
}

int xImageView::wrapContentWidth()
{
    return src.width();
}

int xImageView::wrapContentHeight()
{
    return src.height();
}

bool xImageView::onPaintContent(xPainter& p)
{
    chRect rcContent = contentRect();

    if (!src.isNull())
    {
#if IF_IMAGE_VIEW_SUPPORT_GRAVITY
        int nPixmapWidth = src.width();
        int nPixmapHeight = src.height();
        if (nPixmapWidth != rcContent.Width()
                || nPixmapHeight != rcContent.Height())
        {
            chRect rcDst = chRect(0, 0, nPixmapWidth, nPixmapHeight);
            adjustRectByGravity(rcContent, gravity, rcDst);
            rcContent = rcDst;
        }
#endif
        if (overlap)
        {
            p.blendRect(rcContent, src);
        }
        else
        {
            p.drawItemPixmap(rcContent, src);
        }

#if IF_INVERT_EFFECT
        if (invert == xView::bool_true)
        {
            p.invertRect(rcContent);
        }
#endif
    }

#if IF_IMAGE_VIEW_BORDER
    if (m_nBorderWidth > 0)
    {
        p.setLineType(LINE_TYPE_SOLID);
        p.setLineWidth(m_nBorderWidth);
        p.drawRect(rcContent, m_clrBorder);
    }
#endif

    return true;
}
#endif

//////////////////////////////////////////////////////////////////////////
// class TextView : public View
EXPORT_VIEW_CLASS(xTextView);

#define HIGH_LIGHT_TEXT_DEFAULT_COLOR xColor(0xff, 0x9f, 0x00)
#if IF_TEXTVIEW_SUPPORT_HIGH_LIGHT_TEXT
xColor xTextView::s_clrHighlightText = HIGH_LIGHT_TEXT_DEFAULT_COLOR;
#endif

xTextView::xTextView()
    : overlength_style(OVERLENGTH_TYPE_DIRECT_CUT)
#if IF_TEXTVIEW_SUPPORT_HIGH_LIGHT_TEXT
    , m_strHighlightKeyword("")
#endif
    , m_bMultiLineEnable(false)
    , m_pOverLengthEffect(NULL)
    , m_bNewLineByWord(true)
    , m_bContainRTLChar(false)
#if IF_TEXTVIEW_SUPPORT_ACTIVE_FONT
    , m_pActiveFont(NULL)
#endif
{
    REGISTER_VIEW_DUMP_CLASS(xTextView)

    gravity = gravity_left | gravity_top;
    align = align_left;
    wrapLimited = WRAP_LIMITED_DEFAULT;
    wrapLimitedHeight = WRAP_LIMITED_DEFAULT;
    m_cacheTextSize = chSize(-1, -1);
    // background.setColor(xColor(255, 255, 255));
    SLOT_CONNECT(this, signalTextChanged, this, xTextView::slotOnTextChange);
    SLOT_CONNECT(this, signalAppear, this, xTextView::OnViewAppear);
}

xTextView::~xTextView()
{
    DeleteOverLengthEffect();

    SLOT_DISCONNECT(this, signalAppear, this, xTextView::OnViewAppear);
    SLOT_DISCONNECT(this, signalTextChanged, this, xTextView::slotOnTextChange);

#if IF_TEXTVIEW_SUPPORT_ACTIVE_FONT
    if (NULL != m_pActiveFont)
    {
        delete m_pActiveFont;
        m_pActiveFont = NULL;
    }
#endif
}

void xTextView::InitOverLengthEffect()
{
    g_OverLengthEffectFactory.RegisterOverLengthEffect(COverLengthEllipsisEffect::GetOverLengthEllipsisEffectType(), &COverLengthEllipsisEffect::CreateOverLengthEllipsisEffect);
    g_OverLengthEffectFactory.RegisterOverLengthEffect(CHorizontalScrollEffect::GetHorizontalScrollEffectType(), &CHorizontalScrollEffect::CreateHorizontalScrollEffect);
#if IF_TEXTVIEW_SUPPORT_VERTICAL_SCROLL
    g_OverLengthEffectFactory.RegisterOverLengthEffect(CVerticalScrollEffect::GetVerticalScrollEffectType(), &CVerticalScrollEffect::CreateVerticalScrollEffect);
#endif
}

void xTextView::SetInitText(const yl::string& strText)
{
    xTextView::SetText(strText);
}

void xTextView::SetText(const yl::string& src)
{
    if (!g_WorkStation.SetupTextTran(this, src))
    {
        SetTranText(src);
    }
}

void xTextView::SetTranText(const yl::string& strText)
{
    if (m_strText != strText)
    {
        m_strText = strText;

        if (NULL != m_pHostStation)
        {
            bool bContainRTLChar = m_bContainRTLChar;

            m_bContainRTLChar = m_pHostStation->IsRTLText(m_strText);

            if (bContainRTLChar != m_bContainRTLChar)
            {
                OnDirectionChange();
            }
        }

        OnTextChange();
    }
}

void xTextView::SetTextColor(const xColor& objColor)
{
    textColor = objColor;
}

void xTextView::SetWrapLimited(int nWrapLimited)
{
    wrapLimited = nWrapLimited;
}

void xTextView::SetWrapLimitedHeight(int nWrapLimitedHeight)
{
    wrapLimitedHeight = nWrapLimitedHeight;
}

void xTextView::SetFont(const xFont& _font)
{
    font = _font;

    OnTextChange();
}

void xTextView::SetFontPointSize(int nPointSize)
{
    font.setPointSize(nPointSize);

    OnTextChange();
}

void xTextView::SetFontBold(bool bBold)
{
    font.setBold(bBold);

    OnTextChange();
}

void xTextView::SetFontFamily(const yl::string& strFamily)
{
    font.setFamily(strFamily);

    OnTextChange();
}

void xTextView::OnTextChange()
{
    clearCacheTextSize();

    signalTextChanged.emitSignal(0, 0, 0, this);

    onContentChanged();
}

void xTextView::OnDirectionChange()
{

}

void xTextView::showValue(const yl::string& src)
{
    SetText(src);
}

void xTextView::SetMultiLineEnable(bool bMultiLine)
{
    m_bMultiLineEnable = bMultiLine;

    OnTextChange();
}

void xTextView::loadAttributes(xml_node& node)
{
    xView::loadAttributes(node);

    PopupAndroidGravity(node, XWINDOW_NAME_GRAVITY, gravity);
    PopupAndroidGravity(node, XWINDOW_NAME_ALIGN, align);

    PopupAndroidString(node, XWINDOW_NAME_TEXT, m_strText);

    if (!m_strText.empty())
    {
        xWorkStation& currentStation = g_WorkStation;
        yl::string strText = currentStation.Tran(m_strText);

        if (strText != m_strText)
        {
            currentStation.SetupTextTran(this, m_strText);
        }
        else
        {
            m_bContainRTLChar = currentStation.IsRTLText(strText);
        }
    }

    PopupAndroidColor(node, XWINDOW_NAME_TEXT_COLOR, textColor);

    int textSize = -1;

    if (PopupAndroidFont(node, XWINDOW_NAME_TEXT_SIZE, textSize))
    {
        SetFontPointSize(textSize);
    }

    bool bBold = false;

    if (PopupAndroidBool(node, XWINDOW_NAME_TEXT_BOLD, bBold))
    {
        SetFontBold(bBold);
    }

    PopupAndroidBool(node, XWINDOW_NAME_MULTILINE, m_bMultiLineEnable);

    PopupAndroidPixel(node, XWINDOW_NAME_WRAPLIMITED, wrapLimited);

#if IF_TEXTVIEW_SUPPORT_ACTIVE_FONT
    VEC_TEXTVIEW_INT vecActiveFonts;
    PopupAndroidArray(node, XWINDOW_NAME_ACTIVE_FONTS, vecActiveFonts);
    //排列可用字体，并优先选择对应的字体
    int nSize = vecActiveFonts.size();
    if (nSize > 0)
    {
        if (NULL == m_pActiveFont)
        {
            m_pActiveFont = new CActiveFontText();
        }

        if (NULL != m_pActiveFont)
        {
            m_pActiveFont->AddActiveFontVec(vecActiveFonts);
        }

        SetFontFitSize();
    }
#endif

    VEC_TEXTVIEW_INT vecType;
    PopupAndroidOverLengthStyle(node, XWINDOW_NAME_OVER_LENGTH_STYLE, vecType);

    //如果不配置默认为OVERLENGTH_TYPE_CUT_WITH_ELLIPSIS
    if (vecType.size() <= 0)
    {
        vecType.push_back(OVERLENGTH_TYPE_CUT_WITH_ELLIPSIS);
    }

    bool bEllipsis = false;
    for (int i = 0; i < vecType.size(); ++i)
    {
        OVERLENGTH_TYPE eType = (OVERLENGTH_TYPE)vecType[i];
        m_pOverLengthEffect = GetEffectByType(eType);
        if (NULL != m_pOverLengthEffect)
        {
            m_pOverLengthEffect->loadAttributes(node);
        }

        if (OVERLENGTH_TYPE_CUT_WITH_ELLIPSIS == eType)
        {
            bEllipsis = true;;
        }
    }

    if (bEllipsis)
    {
        m_pOverLengthEffect = GetEffectByType(OVERLENGTH_TYPE_CUT_WITH_ELLIPSIS);
    }

    if (NULL != m_pOverLengthEffect)
    {
        overlength_style = m_pOverLengthEffect->GetType();
    }
}

chSize& xTextView::cacheTextSize()
{
    if (m_cacheTextSize.cx == -1)
    {
        m_cacheTextSize = font.textBound(m_strText);
    }
    return m_cacheTextSize;
}

int xTextView::wrapContentWidth()
{
    int nWrapWidth = 0;

#if IF_TEXTVIEW_SUPPORT_OVERLENGTH_WRAPCONTENT
    if (NULL != m_pOverLengthEffect && m_pOverLengthEffect->wrapContentWidth(nWrapWidth))
    {
        return nWrapWidth;
    }
#endif

    nWrapWidth = cacheTextSize().cx;

    if (layout_width == xView::wrap_content && nWrapWidth > wrapLimited && wrapLimited != WRAP_LIMITED_DEFAULT)
    {
        nWrapWidth = wrapLimited;
    }

    return nWrapWidth;
}

bool xTextView::clearCacheTextSize()
{
    m_cacheTextSize = chSize(-1, -1);
    m_cachePaintTextSize = chSize(-1, -1);
    return true;
}

chRect xTextView::GetViewPort()
{
    chRect rect = clientRect();
    if (NULL == m_pOverLengthEffect)
    {
        return rect;
    }

    chSize szOffset = m_pOverLengthEffect->GetOffset();

    return rect.OffsetRect(szOffset);
}

bool xTextView::onRectChanged(const chRect& rectOld)
{
    if (!xView::onRectChanged(rectOld) || m_rcText.IsEmpty())
    {
        SetTextRect(contentRect());
    }

    return true;
}

bool xTextView::onPaintContent(xPainter& p)
{
#if IF_TEXTVIEW_SUPPORT_OVERLENGTH_WRAPCONTENT
    if (NULL != m_pOverLengthEffect && m_pOverLengthEffect->onPaintContent(p))
    {
        return true;
    }
#endif

    DrawText(p, textColor);

    return true;
}

#if IF_TEXTVIEW_SUPPORT_HIGH_LIGHT_TEXT
void xTextView::SetTextHightLightColor(const xColor& objColor)
{
    s_clrHighlightText = objColor;
}
#endif

void xTextView::DrawText(xPainter& p, const chRect& rcContent, const xColor& objColor)
{
    if (!m_strText.empty() && NULL != m_pHostStation)
    {
        chRect rectTextBound(chPoint(0, 0), CachePaintTextSize());

        adjustRectByGravity(rcContent, gravity, rectTextBound);

        rectTextBound = rcContent & rectTextBound;

        p.drawText(rectTextBound, GetStringToPaint().c_str(), align, font, objColor);
    }
}

void xTextView::DrawText(xPainter& p, const xColor& objColor)
{
    if (m_strText.empty())
    {
        return;
    }

    // draw text
    chPoint ptViewPort = GetViewPort().TopLeft();
    chRect rcTextBound(chPoint(0, 0), CachePaintTextSize());

    int nGravity = gravity;
    int nAlign = align;
    //滚动时根据滚动方向定义对齐方式
    if (NULL != m_pOverLengthEffect && (IsOverLength() || IsOverHeight()))
    {
        int nNewGravity = m_pOverLengthEffect->GetGravity();
        if (gravity_undefined != nNewGravity)
        {
            nGravity = GetOverLengthEffectAlignment(nGravity, nNewGravity);
        }

        int nNewAlign = m_pOverLengthEffect->GetAlign();
        if (gravity_undefined != nNewAlign)
        {
            nAlign = GetOverLengthEffectAlignment(nAlign, nNewAlign);
        }
    }

    adjustRectByGravity(m_rcText, nGravity, rcTextBound);

    rcTextBound = m_rcText & rcTextBound;

    xRegion regOld = p.GetClipRegion();
    bool bRestClip = false;
    if (m_rcText != contentRect())
    {
        bRestClip = true;
        chRect rcGeometry = geometry();
        chRect rcReal(rcTextBound);
        rcReal.OffsetRect(rcGeometry.left, rcGeometry.top);
        p.setClipRect(rcReal, CHIP_OPERATION_INTERSECT);
    }

    yl::string strContent = GetStringToPaint();

    p.drawTextEx(rcTextBound, strContent.c_str(), nAlign, ptViewPort, font, objColor);

#if IF_TEXTVIEW_SUPPORT_HIGH_LIGHT_TEXT
    yl::string strTemp = strContent;
    chPoint ptKey = ptViewPort;
    if (!m_strHighlightKeyword.empty())
    {
        yl::string::size_type nOffset = strContent.find(m_strHighlightKeyword.c_str());
        if (nOffset != yl::string::npos)
        {
            strTemp = strContent.substr(nOffset);

            int nLenFull = font.textBound(strContent.c_str()).cx;
            int nHightLight = font.textBound(strTemp.c_str()).cx;
            int nOffsetHightLight = nLenFull - nHightLight;
            rcTextBound.left += nOffsetHightLight;

            p.drawTextEx(rcTextBound, m_strHighlightKeyword.c_str(), nAlign, ptViewPort, font, s_clrHighlightText);
        }
    }
#endif

    if (bRestClip)
    {
        p.SetClipRegion(regOld);
    }
}

int xTextView::GetOverLengthEffectAlignment(int nNormalAlignment, int nEffectAlignment)
{
    int nResult = gravity_undefined;
    int nYGravity = nEffectAlignment & gravity_y_mask;
    int nXGravity = nEffectAlignment & gravity_x_mask;
    if (gravity_undefined != nYGravity && gravity_undefined != nXGravity)
    {
        nResult = nEffectAlignment;
    }
    else if (gravity_undefined != nYGravity)
    {
        nResult = nNormalAlignment & gravity_x_mask;
        nResult = nResult | nYGravity;
    }
    else
    {
        nResult = nNormalAlignment & gravity_y_mask;
        nResult = nResult | nXGravity;
    }

    return nResult;
}

void xTextView::slotOnTextChange(xSignal* sender, WPARAM wParam, LPARAM lParam, int /*nDataBytes*/, LPVOID /*pData*/)
{
    OnStartOverLengthEffect();
}

void xTextView::setWText(const yl::wstring& wText)
{
    SetText(yl::to_utf8(wText));
}

int xTextView::TextLength()
{
    return CacheWText().size();
}

yl::wstring& xTextView::CacheWText()
{
    if (m_strCacheWText.empty())
    {
        m_strCacheWText = yl::to_utf16(m_strText);
    }

    return m_strCacheWText;
}

chSize xTextView::CachePaintTextSize()
{
    if (m_cachePaintTextSize.cx == -1)
    {
        m_cachePaintTextSize = font.textBound(GetStringToPaint());
    }
    return m_cachePaintTextSize;
}

yl::string xTextView::GetStringToPaint()
{
    if (!m_strCachePaintString.empty())
    {
        return yl::to_utf8(m_strCachePaintString);
    }

    yl::wstring wstr = CacheWText();

    if (!IsOverLength() && !IsOverHeight())
    {
        m_strCachePaintString = wstr;

        if (m_bContainRTLChar && NULL != m_pHostStation)
        {
            m_pHostStation->GetRTLShowWText(m_strCachePaintString);
        }

        return yl::to_utf8(m_strCachePaintString);
    }

    //如果是多行，则获取多行文本传递到超长风格中
    if (m_bMultiLineEnable)
    {
        wstr = GetMultiLineString(wstr);
    }

    if (NULL != m_pOverLengthEffect)
    {
        m_strCachePaintString = m_pOverLengthEffect->GetStringToPaint(m_rcText, wstr);

#if IF_TEXTVIEW_SUPPORT_VERTICAL_SCROLL
        if (OVERLENGTH_TYPE_VSCROLL == m_pOverLengthEffect->GetType() && !m_strCachePaintString.empty())
        {
            CVerticalScrollEffect* pVScroll = static_cast<CVerticalScrollEffect*>(m_pOverLengthEffect);
            pVScroll->PrepareLineSize(m_strCachePaintString, gravity);
        }
#endif
    }
    else
    {
        m_strCachePaintString = wstr;
    }

    if (m_bContainRTLChar && NULL != m_pHostStation)
    {
        m_pHostStation->GetRTLShowWText(m_strCachePaintString);
    }

    return yl::to_utf8(m_strCachePaintString);
}

void xTextView::OnViewAppear(xSignal* sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData)
{
    if (0 == wParam && NULL != m_pOverLengthEffect)
    {
        m_pOverLengthEffect->onEndEffect();
    }
    else if (1 == wParam)
    {
        OnStartOverLengthEffect();
    }
}

COverLengthEffectBase* xTextView::FindEffectByType(int nType)
{
    int nSize = m_vecOverLengthEffect.size();

    for (int i = 0; i < nSize; ++i)
    {
        COverLengthEffectBase* pEffect = m_vecOverLengthEffect[i];
        if (NULL != pEffect && pEffect->GetType() == nType)
        {
            return pEffect;
        }
    }

    return NULL;
}

COverLengthEffectBase* xTextView::GetEffectByType(int nType)
{
    COverLengthEffectBase* pEffect = FindEffectByType(nType);
    if (NULL == pEffect)
    {
        pEffect = g_OverLengthEffectFactory.ApplyOverLengthEffect(nType);
        if (NULL != pEffect)
        {
            pEffect->SetTextView(this);
            m_vecOverLengthEffect.push_back(pEffect);
        }
    }

    return pEffect;
}

bool xTextView::SetOverLengthStyle(int nType)
{
    //超长效果不改变
    if (overlength_style == nType)
    {
        return false;
    }

    overlength_style = nType;
    m_strCachePaintString.clear();
    m_cachePaintTextSize = chSize(-1, -1);

    //结束当前的超长效果
    if (NULL != m_pOverLengthEffect)
    {
        m_pOverLengthEffect->onEndEffect();
        m_pOverLengthEffect = NULL;
    }

    //新效果为直接裁剪
    if (OVERLENGTH_TYPE_DIRECT_CUT == nType)
    {
        return true;
    }

    m_pOverLengthEffect = GetEffectByType(nType);

    if (NULL != m_pOverLengthEffect)
    {
        OnStartOverLengthEffect();
    }

    return true;
}

static const yl::wstring HIDEABLE_CHARS = L" \t";
static const yl::wstring CUTABLE_CHARS = HIDEABLE_CHARS + L"-,.;)}]!?>";

yl::wstring xTextView::GetEllipsisText(const yl::wstring& wstr)
{
    COverLengthEffectBase* pEllipsis = GetEffectByType(OVERLENGTH_TYPE_CUT_WITH_ELLIPSIS);

    if (NULL != pEllipsis)
    {
        return pEllipsis->GetStringToPaint(m_rcText, wstr);
    }

    return wstr;
}

yl::wstring xTextView::GetEllipsisTextByFont(const yl::wstring& wstr, const xFont& font)
{
    COverLengthEllipsisEffect* pEllipsis = (COverLengthEllipsisEffect*)GetEffectByType(OVERLENGTH_TYPE_CUT_WITH_ELLIPSIS);

    if (NULL != pEllipsis)
    {
        return pEllipsis->GetEllipsisEffectText(m_rcText, wstr, font);
    }

    return wstr;
}

yl::wstring xTextView::GetAutoReturnText(const yl::wstring& wstr)
{
    return GetMultiLineString(wstr);
}

yl::wstring xTextView::GetMultiLineString(const yl::wstring& wstr)
{
    yl::wstring result;
    typedef YLVector<yl::wstring> LINES;
    YLVector<yl::wstring> vecLines = yl::string_split<yl::wstring::value_type>(wstr, L"\n", true);

    m_vecAutoReturnLocation.clear();
    m_vecAutoReturnLocation.push_back(0);
    m_vecLineBreakLocation.clear();
    int iRow = 0;
    int iFirstCharPos = 0;
    for (LINES::iterator begin = vecLines.begin(); begin != vecLines.end(); ++begin)
    {
        yl::wstring& str = *begin;
        while (!str.empty())
        {
            if (m_rcText.Width() < font.textBound(yl::to_utf8(str)).cx)
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

                    if (nTextSize <= m_rcText.Width())
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

                //如果结尾的字符不能换行
                if (CUTABLE_CHARS.find_first_of(strLine.back()) == yl::wstring::npos)
                {
                    yl::wstring::size_type cutPos = strLine.find_last_of(CUTABLE_CHARS);
                    if (cutPos != yl::wstring::npos)
                    {
                        strLine.resize(cutPos + 1);
                    }
                }

                result += strLine;
                result.push_back('\n');
                m_vecAutoReturnLocation.push_back(result.size());
                str.erase(0, strLine.size());
            }
            else
            {
                result += str;
                result.push_back('\n');

                m_vecLineBreakLocation.push_back(result.size());
                break;
            }
        }
    }

    //去掉最后一行的\n
    if (!result.empty() && result.back() == '\n')
    {
        result.pop_back();
    }
    if (m_vecLineBreakLocation.size() > 0)
    {
        m_vecLineBreakLocation.removeAt(m_vecLineBreakLocation.size() - 1);
    }

    return result;
}

bool xTextView::EnableScroll(bool bEnabled)
{
    //将所有滚动效果都设置为可用/不可用
    for (int i = 0; i < m_vecOverLengthEffect.size(); ++i)
    {
        COverLengthEffectBase* pEffect = m_vecOverLengthEffect[i];
        if (NULL != pEffect && (OVERLENGTH_TYPE_SCROLL == pEffect->GetType()
#if IF_TEXTVIEW_SUPPORT_VERTICAL_SCROLL
                                || OVERLENGTH_TYPE_VSCROLL == pEffect->GetType()
#endif
                               ))
        {
            COverLengthScrollEffectBase* pScrollEffect = static_cast<COverLengthScrollEffectBase*>(pEffect);
            pScrollEffect->SetScrollEnable(bEnabled);
        }
    }

    return true;
}

void xTextView::SetScrollDirection(int nScrollDirection)
{
    COverLengthEffectBase* pEffect = GetEffectByType(OVERLENGTH_TYPE_SCROLL);
    if (NULL != pEffect)
    {
        COverLengthScrollEffectBase* pScrollEffect = static_cast<COverLengthScrollEffectBase*>(pEffect);
        pScrollEffect->SetScrollDirection(nScrollDirection);
    }
}

void xTextView::SetTextRect(const chRect& rcText)
{
    if (!m_rcText.IsSameSize(rcText))
    {
        m_rcText = rcText;

        OnStartOverLengthEffect();
    }
}

int xTextView::GetNumOfAutoReturnAdd(int iStPos, int iEnPos)
{
    int iNum = 0;
    if (iStPos < iEnPos)
    {
        int iLow = 0;
        int iUp = m_vecAutoReturnLocation.size() - 1;
        if (iUp < 0)
        {
            return 0;
        }
        int lower_pos = 0;
        int upper_pos = iUp;
        int nLoop = 0;
        //二分查找下限
        while (1)
        {
            DEAD_LOOP_BREAK(nLoop);
            int mid_pos = (lower_pos + upper_pos) >> 1;
            if (iStPos >= m_vecAutoReturnLocation[mid_pos])
            {
                iLow = mid_pos;
                break;
            }
            upper_pos = mid_pos - 1;
            if (upper_pos <= lower_pos)
            {
                iLow = upper_pos;
                break;
            }
        }
        // 二分查找上限
        lower_pos = 0;
        upper_pos = iUp;
        nLoop = 0;
        while (1)
        {
            DEAD_LOOP_BREAK(nLoop);
            int mid_pos = (lower_pos + upper_pos) >> 1;
            if (iEnPos <= m_vecAutoReturnLocation[mid_pos])
            {
                iUp = mid_pos;
                break;
            }
            lower_pos = mid_pos + 1;
            if (lower_pos >= upper_pos)
            {
                iUp = lower_pos;
                break;
            }
        }

        iNum = iUp - iLow;
    }

    return iNum;
}

int xTextView::wrapContentHeight()
{
    int nWrapWidth = 0;

#if IF_TEXTVIEW_SUPPORT_OVERLENGTH_WRAPCONTENT
    if (NULL != m_pOverLengthEffect && m_pOverLengthEffect->wrapContentHeight(nWrapWidth))
    {
        return nWrapWidth;
    }
#endif

    int nWrapHeight = CachePaintTextSize().cy;

    if (xView::wrap_content == layout_height && nWrapHeight > wrapLimitedHeight && wrapLimitedHeight != WRAP_LIMITED_DEFAULT)
    {
        nWrapHeight = wrapLimitedHeight;
    }

    return nWrapHeight;
}

void xTextView::OnStartOverLengthEffect()
{
#if IF_TEXTVIEW_SUPPORT_ACTIVE_FONT
    SetFontFitSize();
#endif

    m_cacheTextSize = chSize(-1, -1);
    m_strCachePaintString.clear();
    m_cachePaintTextSize = chSize(-1, -1);
    m_strCacheWText.clear();

    //如果没有超长效果或者没有超长或者超高
    if (NULL != m_pOverLengthEffect)
    {
        m_pOverLengthEffect->onEndEffect();
        if (IsOverLength() || IsOverHeight())
        {
            if (!m_pOverLengthEffect->OnStartEffect())
            {
                update();
            }
        }
    }
}

void xTextView::SetScrollEffectInfoByType(int eType, int nScrollTickTime, int nScrollShift, int nScrollRoundDuration, int nScrollLineDuration)
{
#if IF_TEXTVIEW_SUPPORT_VERTICAL_SCROLL
    if (OVERLENGTH_TYPE_VSCROLL != eType && OVERLENGTH_TYPE_SCROLL != eType)
#else
    if (OVERLENGTH_TYPE_SCROLL != eType)
#endif
    {
        return ;
    }

    COverLengthEffectBase* pEffect = FindEffectByType(eType);
    if (NULL != pEffect)
    {
        COverLengthScrollEffectBase* pScroll = static_cast<COverLengthScrollEffectBase*>(pEffect);
        pScroll->SetScrollTickTime(nScrollTickTime);
        pScroll->SetScrollShift(nScrollShift);
        pScroll->SetScrollRoundDuration(nScrollRoundDuration);
        pScroll->SetScrollLineDuration(nScrollLineDuration);
    }
}

#if IF_TEXTVIEW_SUPPORT_ACTIVE_FONT
void xTextView::SetFontFitSize()
{
    // 设置了自动字体时，匹配合适的字体
    if (NULL != m_pActiveFont)
    {
        int nFontSize = m_pActiveFont->GetFitFontInDynamicFont(m_rcText, m_strText);
        if (nFontSize > 0 && nFontSize != font.getPointSize())
        {
            SetFontPointSize(nFontSize);
        }
    }
}
#endif

void xTextView::DeleteOverLengthEffect()
{
    for (int i = 0; i < m_vecOverLengthEffect.size(); ++i)
    {
        COverLengthEffectBase* pEffect = m_vecOverLengthEffect[i];
        if (NULL != pEffect)
        {
            if (m_pOverLengthEffect == pEffect)
            {
                m_pOverLengthEffect = NULL;
            }

            delete pEffect;
            pEffect = NULL;
        }
    }

    m_vecOverLengthEffect.clear();
    if (NULL != m_pOverLengthEffect)
    {
        delete m_pOverLengthEffect;
        m_pOverLengthEffect = NULL;
    }
}

//////////////////////////////////////////////////////////////////////////
// class xButton : public xTextView
EXPORT_VIEW_CLASS(xButton);

xButton::xButton()
#if IF_BUTTON_SUPPORT_PRESS_STATE
    : m_bPressed(false)
#endif
#if IF_MULTI_TOUCH_ENABLE
    , m_nTouchPointID(TOUCH_POINT_INVALID_ID)
#endif
{
    REGISTER_VIEW_DUMP_CLASS(xButton)

    gravity = gravity_center;
#if IF_INVERT_EFFECT
    m_bInvert = false;
#endif
#if IF_MULTI_TOUCH_ENABLE
    touchable = true;
#endif
}

xButton::~xButton()
{
}

void xButton::SetPixmap(const yl::string& strPixmap)
{
    src.setNull();

#if IF_XWINDOW_DUMP_EXPORT
    m_strSrc = strPixmap;
#endif

    if (NULL != m_pHostStation)
    {
        m_pHostStation->GetImageByPath(strPixmap, src);
    }

    onContentChanged();
}

#if IF_BUTTON_SUPPORT_DISABLE_STATE
void xButton::SetDisableTextColor(const xColor& color)
{
    m_clrDisable = color;

    if (disable)
    {
        update();
    }
}

void xButton::SetDisablePixmap(const yl::string& strDisable)
{
    m_pmDisable.setNull();

    if (NULL != m_pHostStation)
    {
        m_pHostStation->GetImageByPath(strDisable, m_pmDisable);
    }

    if (disable)
    {
        onContentChanged();
    }
}
#endif

#if IF_BUTTON_SUPPORT_FOCUS_STATE
void xButton::SetFocusTextColor(const xColor& color)
{
    m_clrFoucs = color;

    if (hasFocus())
    {
        update();
    }
}

void xButton::SetFocusPixmap(const yl::string& strPixmap)
{
    m_pmFocus.setNull();

    if (NULL != m_pHostStation)
    {
        m_pHostStation->GetImageByPath(strPixmap, m_pmFocus);
    }

    if (hasFocus())
    {
        onContentChanged();
    }
}
#endif

#if IF_BUTTON_SUPPORT_PRESS_STATE
void xButton::SetPressTextColor(const xColor& color)
{
    m_clrPress = color;

    if (m_bPressed)
    {
        update();
    }
}

void xButton::SetPressPixmap(const yl::string& strPixmap)
{
    m_pmPress.setNull();

    if (NULL != m_pHostStation)
    {
        m_pHostStation->GetImageByPath(strPixmap, m_pmPress);
    }

    if (m_bPressed)
    {
        onContentChanged();
    }
}

bool xButton::onFocusEvent(bool bFocusIn)
{
    if (!bFocusIn && m_bPressed)
    {
        m_bPressed = false;
        update();
    }

    return xTextView::onFocusEvent(bFocusIn);
}

bool xButton::onKeyPressedEvent(int key)
{
    if (PHONE_KEY_OK != key || !hasFocus())
    {
        return false;
    }

    m_bPressed = true;
    update();

    return false;
}

bool xButton::onKeyReleasedEvent(int key)
{
    if (PHONE_KEY_OK != key || !hasFocus())
    {
        return false;
    }

    m_bPressed = false;
    update();

    return false;
}
#endif // #if IF_BUTTON_SUPPORT_PRESS_STATE

#if IF_MULTI_TOUCH_ENABLE
bool xButton::onTouchBeganEvent(xView* pAnchorView, LIST_RAW_TOUCH_POINT& lsTouchPoint)
{
    if (!clickable || pAnchorView != this || lsTouchPoint.size() <= 0)
    {
        return false;
    }

    ST_TOUCH_POINT* pTouchPoint = lsTouchPoint.back();

    if (NULL == pTouchPoint)
    {
        return false;
    }

    m_nTouchPointID = pTouchPoint->nID;
    m_bPressed = true;

    update();

    return true;
}

bool xButton::onTouchMovedEvent(xView* pAnchorView, LIST_RAW_TOUCH_POINT& lsTouchPoint)
{
    if (!m_bPressed || pAnchorView != this || TOUCH_POINT_INVALID_ID == m_nTouchPointID)
    {
        return false;
    }

    ST_TOUCH_POINT* pTouchPoint = FindRawPointByID(lsTouchPoint, m_nTouchPointID);

    if (NULL == pTouchPoint)
    {
        return false;
    }

    chPoint ptTouch(pTouchPoint->nPosX, pTouchPoint->nPosY);

    if (!m_rectLayout.PtInRect(ptTouch))
    {
        m_nTouchPointID = TOUCH_POINT_INVALID_ID;
        m_bPressed = false;

        update();
    }

    return true;
}

bool xButton::onTouchEndedEvent(xView* pAnchorView, LIST_RAW_TOUCH_POINT& lsTouchPoint)
{
    if (!m_bPressed || pAnchorView != this || TOUCH_POINT_INVALID_ID == m_nTouchPointID)
    {
        return false;
    }

    ST_TOUCH_POINT* pTouchPoint = FindRawPointByID(lsTouchPoint, m_nTouchPointID);

    if (NULL == pTouchPoint)
    {
        return false;
    }

    m_nTouchPointID = TOUCH_POINT_INVALID_ID;
    m_bPressed = false;
    update();

    if (hasFocused())
    {
        onClick();
        signalClicked.emitSignal(0, 0, 0, NULL);
    }
    else
    {
        g_WorkStation.CheckViewActiveAndFocus(this, this);
#if IF_VIEW_ASYNC_CLICK
        g_WorkStation.AsyncClick(this);
#endif
    }

    return true;
}

bool xButton::onTouchCancelEvent(xView* pAnchorView, LIST_RAW_TOUCH_POINT& lsTouchPoint)
{
    if (!m_bPressed || pAnchorView != this || TOUCH_POINT_INVALID_ID == m_nTouchPointID)
    {
        return false;
    }

    ST_TOUCH_POINT* pTouchPoint = FindRawPointByID(lsTouchPoint, m_nTouchPointID);

    if (NULL == pTouchPoint)
    {
        return false;
    }

    m_nTouchPointID = TOUCH_POINT_INVALID_ID;
    m_bPressed = false;

    update();

    return true;
}
#endif

void xButton::loadAttributes(xml_node& node)
{
    xTextView::loadAttributes(node);

    PopupAndroidPixmap(node, XWINDOW_NAME_SRC, src);

#if IF_BUTTON_SUPPORT_DISABLE_STATE
    PopupAndroidColor(node, XWINDOW_NAME_COLOR_DISABLE, m_clrDisable);
    PopupAndroidPixmap(node, XWINDOW_NAME_PIXMAP_DISABLE, m_pmDisable);
#endif

#if IF_BUTTON_SUPPORT_FOCUS_STATE
    PopupAndroidColor(node, XWINDOW_NAME_COLOR_FOCUS, m_clrFoucs);
    PopupAndroidPixmap(node, XWINDOW_NAME_PIXMAP_FOCUS, m_pmFocus);
#endif

#if IF_BUTTON_SUPPORT_PRESS_STATE
    PopupAndroidColor(node, XWINDOW_NAME_COLOR_PRESS, m_clrPress);
    PopupAndroidPixmap(node, XWINDOW_NAME_PIXMAP_PRESS, m_pmPress);
#endif

#if IF_INVERT_EFFECT
    PopupAndroidBool(node, XWINDOW_NAME_INVERT, m_bInvert);
#endif
}

int xButton::wrapContentWidth()
{
#if IF_BUTTON_SUPPORT_DISABLE_STATE
    if (disable && !m_pmDisable.isNull())
    {
        return m_pmDisable.width();
    }
#endif

#if IF_BUTTON_SUPPORT_PRESS_STATE
    if (m_bPressed && !m_pmPress.isNull())
    {
        return m_pmPress.width();
    }
#endif

#if IF_BUTTON_SUPPORT_FOCUS_STATE
    if (hasFocus() && !m_pmFocus.isNull())
    {
        return m_pmFocus.width();
    }
#endif

    if (!src.isNull())
    {
        return src.width();
    }
    else
    {
        return xTextView::wrapContentWidth();
    }
}

int xButton::wrapContentHeight()
{
#if IF_BUTTON_SUPPORT_DISABLE_STATE
    if (disable && !m_pmDisable.isNull())
    {
        return m_pmDisable.height();
    }
#endif

#if IF_BUTTON_SUPPORT_PRESS_STATE
    if (m_bPressed && !m_pmPress.isNull())
    {
        return m_pmPress.height();
    }
#endif

#if IF_BUTTON_SUPPORT_FOCUS_STATE
    if (hasFocus() && !m_pmFocus.isNull())
    {
        return m_pmFocus.height();
    }
#endif

    if (!src.isNull())
    {
        return src.height();
    }
    else
    {
        return xTextView::wrapContentHeight();
    }
}

void xButton::PaintPixmap(xPainter& p, const chRect& rcDst)
{
    xPixmap pmTmp;

#if IF_BUTTON_SUPPORT_DISABLE_STATE
    if (disable && !m_pmDisable.isNull())
    {
        pmTmp = m_pmDisable;
    }
#endif

#if IF_BUTTON_SUPPORT_PRESS_STATE
    if (pmTmp.isNull() && m_bPressed && !m_pmPress.isNull())
    {
        pmTmp = m_pmPress;
    }
#endif

#if IF_BUTTON_SUPPORT_FOCUS_STATE
    if (pmTmp.isNull() && hasFocus() && !m_pmFocus.isNull())
    {
        pmTmp = m_pmFocus;
    }
#endif

    if (pmTmp.isNull())
    {
        pmTmp = src;

        if (pmTmp.isNull())
        {
            return;
        }
    }

    chRect rect(0, 0, pmTmp.width(), pmTmp.height());

    adjustRectByGravity(rcDst, gravity, rect);

    rect = rcDst & rect;

    if (overlap)
    {
        p.blendRect(rect, pmTmp);
    }
    else
    {
        p.drawItemPixmap(rect, pmTmp);
    }

#if IF_INVERT_EFFECT
    if (m_bInvert)
    {
        p.invertRect(rect);
    }
#endif
}

void xButton::PaintBtnText(xPainter& p)
{
    xColor clrText = textColor;

#if IF_BUTTON_SUPPORT_DISABLE_STATE
    if (disable)
    {
        clrText = m_clrDisable;
    }
#endif

#if IF_BUTTON_SUPPORT_PRESS_STATE
    if (m_bPressed && !m_pmPress.isNull())
    {
        clrText = m_clrPress;
    }
#endif

#if IF_BUTTON_SUPPORT_FOCUS_STATE
#if IF_BUTTON_SUPPORT_PRESS_STATE
    if (!m_bPressed && hasFocus() && !m_pmFocus.isNull())
#else
    if (hasFocus() && !m_pmFocus.isNull())
#endif
    {
        clrText = m_clrFoucs;
    }
#endif

    DrawText(p, clrText);
}

void xButton::Clear()
{
    bool bUpdate = !src.isNull();

    src.setNull();

#if IF_XWINDOW_DUMP_EXPORT
    m_strSrc = "";
#endif

    if (!m_strText.empty())
    {
        SetText("");
    }
    else if (bUpdate)
    {
        onContentChanged();
    }
}

bool xButton::onPaintContent(xPainter& p)
{
    if (NULL != m_pStyle && m_pStyle->onViewPaintContent(this, p))
    {
        return true;
    }

    chRect rcContent = contentRect();

    PaintPixmap(p, rcContent);

    PaintBtnText(p);

    return true;
}

//////////////////////////////////////////////////////////////////////////
EXPORT_VIEW_CLASS(xScrollBar);

xScrollBar::xScrollBar()
{
    REGISTER_VIEW_DUMP_CLASS(xScrollBar)

#if IF_SCROLL_BAR_DENOTE_ITEM_POS
    m_nPageCount = 0;
    m_nTotalCount = 0;
#else
#if IF_SCROLL_BAR_DENOTE_CONTENT_POS
    m_nScrollArea = 0;
    m_nScrollRange = 0;
#endif
#endif
    m_nPosition = 0;
    m_nBlockHeight = 0;
    m_nBlockPos = 0;
}

xScrollBar::~xScrollBar()
{

}

bool xScrollBar::onRectChanged(const chRect& rectOld)
{
    bool bResult = xView::onRectChanged(rectOld);

    AdjustBlock();

    return bResult;
}

void xScrollBar::SetRect(const chRect& rect)
{
    LayoutInBoundRect(rect);

    AdjustBlock();
}

void xScrollBar::SetScrollInfo(int nPageRange, int nRange, int nPos)
{
    chASSERT(nRange >= 0 && nPageRange >= 0 && nPos >= 0);

#if IF_SCROLL_BAR_DENOTE_ITEM_POS
    m_nPageCount = nPageRange;
    m_nTotalCount = nRange;
#else
#if IF_SCROLL_BAR_DENOTE_CONTENT_POS
    m_nScrollArea = nPageRange;
    m_nScrollRange = nRange;
#endif
#endif

    m_nPosition = nPos;

    AdjustBlock();

    update();
}

void xScrollBar::SetScrollPos(int nPos)
{
    m_nPosition = nPos;

#if IF_SCROLL_BAR_DENOTE_ITEM_POS
    if (m_nTotalCount <= 1)
    {
        return;
    }

    m_nBlockPos = (height() - m_nBlockHeight) * m_nPosition / (m_nTotalCount - 1);
#else
#if IF_SCROLL_BAR_DENOTE_CONTENT_POS
    int nRange = m_nScrollRange - m_nScrollArea;

    if (nRange <= 0)
    {
        return;
    }

    m_nBlockPos = (height() - m_nBlockHeight) * m_nPosition / nRange;
#endif
#endif

    update();
}

int xScrollBar::GetMinBlockHeight()
{
#if IF_SCROLL_BAR_SUPPORT_IMAGE
    if (m_pmBlock.isNull())
    {
        return 0;
    }

#if IF_PIXMAP_PARTLY_SHARED
    return m_pmBlock.GetPixmapRect().Height();
#else
    return m_pmBlock.height();
#endif
#else
    return 1;
#endif
}

void xScrollBar::AdjustBlock()
{
#if IF_SCROLL_BAR_DENOTE_ITEM_POS
    if (m_nTotalCount > m_nPageCount && m_nPageCount > 0)
    {
        int nMinBlockHeight = GetMinBlockHeight();
        int nScrollBarHeight = height();

        m_nBlockHeight = nScrollBarHeight * m_nPageCount / m_nTotalCount;

        if (m_nBlockHeight < nMinBlockHeight)
        {
            m_nBlockHeight = nMinBlockHeight;
        }

        m_nBlockPos = (nScrollBarHeight - m_nBlockHeight) * m_nPosition / (m_nTotalCount - 1);
    }
    else
    {
        m_nBlockHeight = 0;
        m_nBlockPos = 0;
    }
#else
#if IF_SCROLL_BAR_DENOTE_CONTENT_POS
    int nRange = m_nScrollRange - m_nScrollArea;

    if (nRange > 0 && m_nScrollRange > 0 && m_nScrollArea > 0)
    {
        int nMinBlockHeight = GetMinBlockHeight();
        int nScrollBarHeight = height();

        m_nBlockHeight = nScrollBarHeight * m_nScrollArea / m_nScrollRange;

        if (m_nBlockHeight < nMinBlockHeight)
        {
            m_nBlockHeight = nMinBlockHeight;
        }

        m_nBlockPos = (nScrollBarHeight - m_nBlockHeight) * m_nPosition / nRange;
    }
#endif
#endif
}

void xScrollBar::loadAttributes(xml_node& node)
{
    xView::loadAttributes(node);

#if IF_SCROLL_BAR_DENOTE_ITEM_POS
    PopupAndroidInt(node, XWINDOW_NAME_PAGE, m_nPageCount);
#endif

#if IF_SCROLL_BAR_SUPPORT_IMAGE
    PopupAndroidPixmap(node, XWINDOW_NAME_BLOCK, m_pmBlock);
#endif
}

#if IF_SCROLL_BAR_SUPPORT_IMAGE
void xScrollBar::SetBlockPixmap(const yl::string& strBlock)
{
    if (NULL != m_pHostStation)
    {
        m_pHostStation->GetImageByPath(strBlock, m_pmBlock);
    }
}

bool xScrollBar::GetBlockRect(chRect& rcBlock)
{
    if (m_pmBlock.isNull() || m_nBlockHeight <= 0)
    {
        return false;
    }

    chRect rcContent = contentRect();
    int nBlockPicWidth = m_pmBlock.GetRealWidth();
    int nBlockLeft = (rcContent.Width() - nBlockPicWidth) >> 1;

    rcBlock.SetRect(nBlockLeft, m_nBlockPos, nBlockLeft + nBlockPicWidth, m_nBlockPos + m_nBlockHeight);

    return true;
}
#endif

bool xScrollBar::onPaintContent(xPainter& p)
{
#if IF_SCROLL_BAR_SUPPORT_IMAGE
    chRect rcBlock;

    if (!GetBlockRect(rcBlock))
    {
        return false;
    }

    if (overlap)
    {
        p.blendRect(rcBlock, m_pmBlock);
    }
    else
    {
        p.drawItemPixmap(rcBlock, m_pmBlock);
    }
#else
#if IF_SCROLL_BAR_DENOTE_ITEM_POS
    if (m_nPageCount <= 0)
    {
        return false;
    }

    if (m_nTotalCount <= m_nPageCount)
    {
        // full pages
        p.fillRect(contentRect(), xColor(0, 0, 0));
    }
    else
    {
        chRect rect = contentRect();
        p.drawRect(rect, xColor(0, 0, 0));

        // slider block
        int nBlockHeight = m_nBlockHeight;
        nBlockHeight = chMAX(nBlockHeight, 2);
        rect.ResizeHeight(nBlockHeight);

        rect.OffsetRect(0, m_nBlockPos - rect.top) ;
        p.fillRect(rect, xColor(0, 0, 0));
    }
#endif
#endif

    return true;
}

#if IF_SCROLL_BAR_SUPPORT_IMAGE
bool xScrollBar::onPaintBackground(xPainter& p)
{
    if ((!background.isNull()) && (!background.isColor()))
    {
        chRect rcClient = clientRect();
        int nBmpWidth = background.pixmap().GetRealWidth();

        if (nBmpWidth < rcClient.Width())
        {
            if (NULL != m_pStyle && m_pStyle->onViewPaintBackground(this, p))
            {
                return true;
            }

            int nOffset = (rcClient.Width() - nBmpWidth) >> 1;

            rcClient.left += nOffset;
            rcClient.right = rcClient.left + nBmpWidth;

            PaintBackground(p, rcClient);

            return true;
        }
    }

    return xView::onPaintBackground(p);
}
#endif

//////////////////////////////////////////////////////////////////////////
#if IF_PROGRESS_BAR_ENABLE
EXPORT_VIEW_CLASS(xProgressBar);

xProgressBar::xProgressBar()
    : m_nRange(100)
    , m_nPosition(0)
    , m_nPaintOffset(0)
    , m_nPaintPosition(0)
    , m_nBarWidth(0)
{
    REGISTER_VIEW_DUMP_CLASS(xProgressBar)

#if IF_PROGRESSBAR_WITH_VERTICAL
    m_nOrientation = PROGRESSBAR_HORIZONTAL;
#endif
}

xProgressBar::~xProgressBar()
{
}

void xProgressBar::SetRange(int nRange)
{
    if (nRange <= 0 || nRange < m_nPosition)
    {
        return;
    }

    m_nRange = nRange;

    SetPaintPosition();

    update();
}

void xProgressBar::SetPosition(int nPosition, bool bNotify/* = false*/)
{
    if (nPosition > m_nRange)
    {
        nPosition = m_nRange;
    }

    if (nPosition < 0)
    {
        nPosition = 0;
    }

    if (nPosition == m_nPosition)
    {
        return;
    }

    m_nPosition = nPosition;

    SetPaintPosition();

    update();

    if (bNotify)
    {
        signalSliderValueChanged.emitSignal(0, 0, 0, NULL);
    }
}

void xProgressBar::SetPaintPosition()
{
    if (m_nRange <= 0 || m_nPosition < 0)
    {
        return;
    }

    chRect rcContent(clientRect());

#if IF_PROGRESSBAR_WITH_VERTICAL
    if (PROGRESSBAR_VERTICAL == m_nOrientation)
    {
        int nCount = rcContent.Height() - m_nPaintOffset;
        float fShowPercent = ((float)(m_nRange - m_nPosition)) / m_nRange;
        m_nPaintPosition = (int)(fShowPercent * nCount);
    }
    else if (PROGRESSBAR_HORIZONTAL == m_nOrientation)
    {
        int nCount = rcContent.Width() - m_nPaintOffset;
        float fShowPercent = ((float)m_nPosition) / m_nRange;
        m_nPaintPosition = (int)(fShowPercent * nCount);
    }
#else
    int nCount = rcContent.Width() - m_nPaintOffset;
    float fShowPercent = ((float)m_nPosition) / m_nRange;
    m_nPaintPosition = (int)(fShowPercent * nCount);
#endif

}

void xProgressBar::OnPositionChange(bool bAdd)
{
    int nPosition = m_nPosition;

    if (bAdd)
    {
        nPosition += 1;
    }
    else
    {
        nPosition -= 1;
    }

    SetPosition(nPosition, true);
}

void xProgressBar::loadAttributes(xml_node& node)
{
    xView::loadAttributes(node);

    PopupAndroidInt(node, XWINDOW_NAME_RANGE, m_nRange);
    PopupAndroidBrush(node, XWINDOW_NAME_BG, m_brushBG);
    PopupAndroidBrush(node, XWINDOW_NAME_BAR, m_brushBar);

#if IF_PROGRESSBAR_WITH_VERTICAL
    PopupAndroidOrientation(node, XWINDOW_NAME_ORIENTATION, m_nOrientation);
#endif

    SetPaintPosition();
}

#if IF_PROGRESSBAR_WITH_VERTICAL
void xProgressBar::PaintVertical(xPainter& p, chRect& rcContent)
{
    SetPaintPosition();

    PaintVerticalBG(p, rcContent);

    PaintVerticalBar(p, rcContent);
}

void xProgressBar::PaintVerticalBG(xPainter& p, chRect& rcContent)
{
    if (m_brushBG.isNull())
    {
        return;
    }

    xPixmap pmBG = m_brushBG.pixmap();
    if (m_nBarWidth <= 0 && !pmBG.isNull())
    {
        m_nBarWidth = pmBG.GetRealHeight();
    }

    if (m_nBarWidth <= 0)
    {
        return;
    }

    int nBGLeft = (rcContent.Width() - m_nBarWidth) >> 1;
    chRect rcBG(nBGLeft, 0, nBGLeft + m_nBarWidth, rcContent.Height());

    if (!pmBG.isNull())
    {
        p.fillRect(rcBG, pmBG);
    }
    else
    {
        xColor colBG = m_brushBG.GetColor();
        p.blendRect(rcBG, colBG);
    }
}

void xProgressBar::PaintVerticalBar(xPainter& p, chRect& rcContent)
{
    if (m_brushBar.isNull())
    {
        return;
    }

    xPixmap pmBar = m_brushBar.pixmap();
    if (m_nBarWidth <= 0 && !pmBar.isNull())
    {
        m_nBarWidth = pmBar.GetRealHeight();
    }

    if (m_nBarWidth <= 0)
    {
        return;
    }

    int nBarLeft = (rcContent.Width() - m_nBarWidth) >> 1;
    int nBarTop = m_nPaintPosition + (m_nPaintOffset >> 1);
    chRect rcBar(nBarLeft, nBarTop, nBarLeft + m_nBarWidth, rcContent.bottom);

    if (!pmBar.isNull())
    {
        p.fillRect(rcBar, pmBar);
    }
    else
    {
        xColor colBar = m_brushBar.GetColor();
        p.blendRect(rcBar, colBar);
    }
}
#endif

void xProgressBar::PaintHorizontal(xPainter& p, chRect& rcContent)
{
    SetPaintPosition();

    PaintHorizontalBG(p, rcContent);

    PaintHorizontalBar(p, rcContent);
}

void xProgressBar::PaintHorizontalBar(xPainter& p, chRect& rcContent)
{
    if (m_brushBar.isNull())
    {
        return;
    }

    xPixmap pmBar = m_brushBar.pixmap();
    if (m_nBarWidth <= 0 && !pmBar.isNull())
    {
        m_nBarWidth = pmBar.GetRealHeight();
    }

    if (m_nBarWidth <= 0)
    {
        return;
    }

    int nBarTop = (rcContent.Height() - m_nBarWidth) >> 1;
    int nBarRight = m_nPaintPosition + (m_nPaintOffset >> 1);
    chRect rcBar(0, nBarTop, nBarRight, m_nBarWidth + nBarTop);
    if (!pmBar.isNull())
    {
        p.fillRect(rcBar, pmBar);
    }
    else
    {
        xColor colBar = m_brushBar.GetColor();
        p.blendRect(rcBar, colBar);
    }
}

void xProgressBar::PaintHorizontalBG(xPainter& p, chRect& rcContent)
{
    if (m_brushBG.isNull())
    {
        return;
    }

    xPixmap pmBG = m_brushBG.pixmap();
    if (m_nBarWidth <= 0 && !pmBG.isNull())
    {
        m_nBarWidth = pmBG.GetRealHeight();
    }

    if (m_nBarWidth <= 0)
    {
        return;
    }

    int nHeight = m_nBarWidth;
    int nBGTop = (rcContent.Height() - nHeight) >> 1;
    chRect rcBG(0, nBGTop, rcContent.Width(), nHeight + nBGTop);
    if (!pmBG.isNull())
    {
        p.fillRect(rcBG, pmBG);
    }
    else
    {
        xColor colBG = m_brushBG.GetColor();
        p.blendRect(rcBG, colBG);
    }
}

bool xProgressBar::onPaintContent(xPainter& p)
{
    if (m_nPosition < 0 || m_nRange <= 0)
    {
        return true;
    }

    SetPaintPosition();

    chRect rcContent(clientRect());

#if IF_PROGRESSBAR_WITH_VERTICAL
    if (PROGRESSBAR_VERTICAL == m_nOrientation)//垂直方向
    {
        PaintVertical(p, rcContent);
    }
    else if (PROGRESSBAR_HORIZONTAL == m_nOrientation) //水平方向
    {
        PaintHorizontal(p, rcContent);
    }
#else
    PaintHorizontal(p, rcContent);
#endif

    return true;
}
#endif

//////////////////////////////////////////////////////////////////////////
#if IF_SLIDE_BAR_ENABLE
EXPORT_VIEW_CLASS(xSlideBar);

xSlideBar::xSlideBar()
{
    REGISTER_VIEW_DUMP_CLASS(xSlideBar)
}

xSlideBar::~xSlideBar()
{
#if IF_GESTURE_ENABLE
    clearGesture();
#endif
}

void xSlideBar::SetSlideBarIcon(xPixmap& pmIcon)
{
    m_pmIcon = pmIcon;

    SetIconChange();
}

void xSlideBar::SetIconChange()
{
#if IF_PROGRESSBAR_WITH_VERTICAL
    if (PROGRESSBAR_HORIZONTAL == m_nOrientation)
    {
        SetPaintOffset(m_pmIcon.width());
    }
    else if (PROGRESSBAR_VERTICAL == m_nOrientation)
    {
        SetPaintOffset(m_pmIcon.height());
    }
#else
    SetPaintOffset(m_pmIcon.width());
#endif

    SetPaintPosition();
}

void xSlideBar::loadAttributes(xml_node& node)
{
    xProgressBar::loadAttributes(node);

    PopupAndroidPixmap(node, XWINDOW_NAME_SLIDE_ICON, m_pmIcon);

    SetIconChange();
}

bool xSlideBar::onPaintContent(xPainter& p)
{
    if (m_nPosition < 0 || m_nRange <= 0)
    {
        return true;
    }
    chRect rcContent(clientRect());

    int nIconHeight = 0;
    int nIconWidth = 0;
    if (!m_pmIcon.isNull())
    {
        nIconWidth = m_pmIcon.width();
        nIconHeight = m_pmIcon.height();
    }

#if IF_PROGRESSBAR_WITH_VERTICAL
    if (PROGRESSBAR_VERTICAL == m_nOrientation)//垂直方向
    {
        PaintVertical(p, rcContent);

        //绘制图标
        if (!m_pmIcon.isNull())
        {
            int nPositionLeft = (rcContent.Width() - nIconWidth) >> 1;
            int nPositionRight =  nPositionLeft + nIconWidth;
            int nPositionBottom = m_nPaintPosition + nIconHeight;
            chRect rcPosition(nPositionLeft, m_nPaintPosition, nPositionRight, nPositionBottom);
            p.fillRect(rcPosition, m_pmIcon);
        }
    }
    else if (PROGRESSBAR_HORIZONTAL == m_nOrientation)
        //水平方向
    {
        PaintHorizontal(p, rcContent);

        //绘制图标,
        if (!m_pmIcon.isNull())
        {
            int nPositionTop = (rcContent.Height() - nIconHeight) >> 1;
            int nPositionBottom = nPositionTop + nIconHeight;
            int nPositionRight = m_nPaintPosition + nIconWidth;
            chRect rcPosition(m_nPaintPosition, nPositionTop, nPositionRight, nPositionBottom);
            p.fillRect(rcPosition, m_pmIcon);
        }
    }
#else
    PaintHorizontal(p, rcContent);

    //绘制图标,
    if (!m_pmIcon.isNull())
    {
        int nPositionTop = (rcContent.Height() - nIconHeight) >> 1;
        int nPositionBottom = nPositionTop + nIconHeight;
        int nPositionRight = m_nPaintPosition + nIconWidth;
        chRect rcPosition(m_nPaintPosition, nPositionTop, nPositionRight, nPositionBottom);
        p.fillRect(rcPosition, m_pmIcon);
    }
#endif

    return true;
}

#if IF_GESTURE_ENABLE
void xSlideBar::EnableSlide(bool bEnable)
{
    if (bEnable)
    {
        grabGesture(GESTURE_TYPE_PAN);
        grabGesture(GESTURE_TYPE_TAP);
    }
    else
    {
        ungrabGesture(GESTURE_TYPE_PAN);
        ungrabGesture(GESTURE_TYPE_TAP);
    }
}

void xSlideBar::checkFocus()
{
    xView* pFocusView = ownerStation().getFocusView();

    if (NULL != pFocusView && (pFocusView == this || xWorkStation::containChildView(pFocusView, this)))
    {
        return;
    }

    signalClicked.emitSignal(0, 0, 0, NULL);
}

bool xSlideBar::onGestureEvent(xGestureEvent* pEvent)
{
    if (NULL == pEvent)
    {
        return false;
    }

    xGesturePtr pGesture = pEvent->GetGestureByType(GESTURE_TYPE_PAN);
    xPanGesture* pPanGesture = xPanGesture::GetPanGesture(pGesture.Get());

    if (NULL != pPanGesture)
    {
        if (GESTURE_STATE_START == pPanGesture->GetState())
        {
            checkFocus();
        }

        const chPoint& ptHotSpot = pPanGesture->GetHotSpot();
        const chPoint& ptOffset = pPanGesture->GetOffset();
        chPoint ptTouch(ptHotSpot.x + ptOffset.x, ptHotSpot.y + ptOffset.y);

        onTouchPoint(ptTouch);

        return true;
    }

    pGesture = pEvent->GetGestureByType(GESTURE_TYPE_TAP);

    xTapGesture* pTapGesture = xTapGesture::GetTapGesture(pGesture.Get());

    if (NULL != pTapGesture)
    {
        if (GESTURE_STATE_START == pTapGesture->GetState())
        {
            checkFocus();
        }

        const chPoint& ptHotSpot = pTapGesture->GetHotSpot();

        onTouchPoint(ptHotSpot);

        return true;
    }

    return true;
}

void xSlideBar::onTouchPoint(const chPoint& ptTouch)
{
    chRect rcSlideBar = contentGeometry();
    int nPosition = 0;

    if (xProgressBar::PROGRESSBAR_VERTICAL == m_nOrientation)
    {
        int nHeight = rcSlideBar.Height();

        if (nHeight <= 0)
        {
            return;
        }

        if (ptTouch.y <= rcSlideBar.top)
        {
            nPosition = m_nRange;
        }
        else if (ptTouch.y >= rcSlideBar.bottom)
        {
            nPosition = 0;
        }
        else
        {
            nPosition = (rcSlideBar.bottom - ptTouch.y) * m_nRange / nHeight;
        }
    }
    else if (xProgressBar::PROGRESSBAR_HORIZONTAL == m_nOrientation)
    {
        int nWidth = rcSlideBar.Width();

        if (nWidth <= 0)
        {
            return;
        }

        if (ptTouch.x <= rcSlideBar.left)
        {
            nPosition = 0;
        }
        else if (ptTouch.x >= rcSlideBar.right)
        {
            nPosition = m_nRange;
        }
        else
        {
            nPosition = (ptTouch.x - rcSlideBar.left) * m_nRange / nWidth;
        }
    }
    else
    {
        return;
    }

    SetPosition(nPosition, true);
}
#endif
#endif

//////////////////////////////////////////////////////////////////////////
#if IF_RADIO_BUTTON_ENABLE
EXPORT_VIEW_CLASS(xRadioButton);

xRadioButton::xRadioButton()
    : m_bContainRTLChar(false)
    , m_bChecked(false)
{
    REGISTER_VIEW_DUMP_CLASS(xRadioButton)
}

xRadioButton::~xRadioButton()
{
}

void xRadioButton::SetChecked(bool bChecked)
{
    if (m_bChecked == bChecked)
    {
        return;
    }

    m_bChecked = bChecked;

    update();
}

void xRadioButton::SetIcon(const yl::string& strOn, const yl::string& strOff)
{
    m_strIconOn = strOn;
    m_strIconOff = strOff;
}

void xRadioButton::SetText(const yl::string& strText)
{
    if (!ownerStation().SetupTextTran(this, strText))
    {
        m_strText = strText;
        m_bContainRTLChar = ownerStation().IsRTLText(m_strText);
    }
}

void xRadioButton::SetTranText(const yl::string& strText)
{
    m_strText = strText;
    m_bContainRTLChar = ownerStation().IsRTLText(m_strText);
}

void xRadioButton::loadAttributes(xml_node& node)
{
    xView::loadAttributes(node);

    int textSize = 0;

    PopupAndroidRect(node, XWINDOW_NAME_RC_ICON, m_rcIcon);
    PopupAndroidString(node, XWINDOW_NAME_ON, m_strIconOn);
    PopupAndroidString(node, XWINDOW_NAME_OFF, m_strIconOff);

    PopupAndroidRect(node, XWINDOW_NAME_RC_TEXT, m_rcText);
    PopupAndroidInt(node, XWINDOW_NAME_TEXT_SIZE, textSize);

    if (textSize != 0)
    {
        m_objFont.setPointSize(textSize);
    }

    PopupAndroidColor(node, XWINDOW_NAME_TEXT_COLOR, m_objTextColor);
    PopupAndroidString(node, XWINDOW_NAME_TEXT, m_strText);

    if (!m_strText.empty())
    {
        xWorkStation& currentStation = g_WorkStation;
        yl::string strText = currentStation.Tran(m_strText);

        if (strText != m_strText)
        {
            currentStation.SetupTextTran(this, strText);
        }
        else
        {
            m_bContainRTLChar = currentStation.IsRTLText(m_strText);
        }
    }
}

bool xRadioButton::onPaintContent(xPainter& p)
{
    if (NULL == m_pHostStation)
    {
        return false;
    }

    yl::string strIcon = m_strIconOff;

    if (m_bChecked && !m_strIconOn.empty())
    {
        strIcon = m_strIconOn;
    }

    if (!strIcon.empty())
    {
        xPixmap pmIcon(strIcon);

        p.drawItemPixmap(m_rcIcon, pmIcon);
    }

    if (!m_strText.empty())
    {
        yl::string strShow = CutOffText(m_strText, m_objFont, m_rcText.Width());

        if (m_bContainRTLChar)
        {
            m_pHostStation->GetRTLShowText(strShow, strShow);
        }

        p.drawText(m_rcText, strShow.c_str(), (gravity_left | gravity_y_center), m_objFont, m_objTextColor);
    }

    return true;
}
#endif

//////////////////////////////////////////////////////////////////////////
LinearLayoutParams::LinearLayoutParams()
    : LayoutParamsInParent(LINEAR_LAYOUT_PARAMS)
{
    layout_gravity = gravity_undefined;
    layout_weight = 0;
}

LinearLayoutParams* LinearLayoutParams::GetLayoutParams(LayoutParamsInParent* pLayout)
{
    if (NULL != pLayout && LINEAR_LAYOUT_PARAMS == pLayout->m_nType)
    {
        return static_cast<LinearLayoutParams*>(pLayout);
    }

    return NULL;
}

void LinearLayoutParams::loadAttributes(xml_node& node)
{
    LayoutParamsInParent::loadAttributes(node);

    PopupAndroidGravity(node, XWINDOW_NAME_LAYOUT_GRAVITY, layout_gravity);
    PopupAndroidInt(node, XWINDOW_NAME_LAYOUT_WEIGHT, layout_weight);
}

//////////////////////////////////////////////////////////////////////////
EXPORT_VIEW_CLASS(xLinearLayout);

xLinearLayout::xLinearLayout()
{
    REGISTER_VIEW_DUMP_CLASS(xLinearLayout)

    gravity = gravity_top_left;
    orientation = xLinearLayout::orientation_horizontal;
}

void xLinearLayout::loadAttributes(xml_node& node)
{
    xViewGroup::loadAttributes(node);

    PopupAndroidGravity(node, XWINDOW_NAME_GRAVITY, gravity);
    PopupAndroidOrientation(node, XWINDOW_NAME_ORIENTATION, orientation);
}

int xLinearLayout::wrapContentWidth()
{
    int result = 0;
    for (xViewIterator it = xViewTree::child_head(); it.hasData(); ++it)
    {
        xView* viewChild = &(*it);
        if (viewChild->visibility == xView::gone)
        {
            continue;
        }

        int childValue = viewChild->wrapBoundWidth();
        if (orientation == xLinearLayout::orientation_horizontal)
        {
            // orientation_horizontal
            result += childValue;
        }
        else
        {
            result = chMAX(result, childValue);
        }
    }
    return result;
}

int xLinearLayout::wrapContentHeight()
{
    int result = 0;
    for (xViewIterator it = xViewTree::child_head(); it.hasData(); ++it)
    {
        xView* viewChild = &(*it);
        if (viewChild->visibility == xView::gone)
        {
            continue;
        }

        int childValue = viewChild->wrapBoundHeight();
        if (orientation == xLinearLayout::orientation_horizontal)
        {
            // orientation_horizontal
            result = chMAX(result, childValue);
        }
        else
        {
            result += childValue;
        }
    }
    return result;
}

void xLinearLayout::LayoutContentInRect(const chRect& rectParentContent)
{
    if (orientation == xLinearLayout::orientation_horizontal)
    {
        // orientation_horizontal
        int nFixedSpace = 0;
        int nMatchParentCount = 0;
        int nTotalWeight = 0;

        for (xViewIterator it = xViewTree::child_head(); it.hasData(); ++it)
        {
            xView* viewChild = &(*it);

            if (NULL == viewChild || xView::gone == viewChild->visibility)
            {
                continue;
            }

            LinearLayoutParams* layoutInParent = LinearLayoutParams::GetLayoutParams(viewChild->m_pLayoutInParent);
            chASSERT(layoutInParent != NULL);

            if (NULL == layoutInParent)
            {
                continue;
            }

            if (layoutInParent->layout_weight != 0)
            {
#if IF_DEBUG_VIEW_MGR
                if (viewChild->layout_width != 0)
                {
                    XWINDOW_WARN("%s@ use width=0 for better performence", viewChild->viewNick().c_str());
                }
#endif
                nTotalWeight += layoutInParent->layout_weight;
            }
            else if (viewChild->layout_width == LayoutParams::match_parent)
            {
#if IF_DEBUG_VIEW_MGR
                if (nMatchParentCount != 0)
                {
                    chASSERTx(FALSE, "%s@ too many match_parent in layout", viewChild->viewNick().c_str());
                }
#endif
                nMatchParentCount ++;
            }
            else
            {
                nFixedSpace += viewChild->wrapBoundWidth();
            }
        }
#if IF_DEBUG_VIEW_MGR
        if (nMatchParentCount != 0 && nTotalWeight != 0)
        {
            chASSERTx(FALSE, "%s@ both match_parent and weight in child layout", viewNick().c_str());
        }
#endif
        int nExpandSpace = 0;
        if ((nMatchParentCount != 0 || nTotalWeight != 0) && nFixedSpace <= rectParentContent.Width())
        {
            nExpandSpace = rectParentContent.Width() - nFixedSpace;
#if IF_DEBUG_VIEW_MGR
            chWARNING(nExpandSpace > 0);
#endif
        }

        chRect rectChildrenBound = ResizeWidth(rectParentContent, nFixedSpace + nExpandSpace);
        adjustRectByGravity(rectParentContent, gravity & gravity_x_mask, rectChildrenBound);

        int nLeftWeight = nTotalWeight;
        int nLeftSpace = nExpandSpace;

        int xPos = rectChildrenBound.left;
        for (xViewIterator it = xViewTree::child_head(); it.hasData(); ++it)
        {
            xView* viewChild = &(*it);

            if (NULL == viewChild || xView::gone == viewChild->visibility)
            {
                continue;
            }

            LinearLayoutParams* layoutInParent = LinearLayoutParams::GetLayoutParams(viewChild->m_pLayoutInParent);
            chASSERT(layoutInParent != NULL);

            if (NULL == layoutInParent)
            {
                continue;
            }

            int heightChild = viewChild->measureHeightWithBound(rectChildrenBound.Height());
            int widthChild = 0;
            if (layoutInParent->layout_weight != 0)
            {
                widthChild = nLeftSpace * layoutInParent->layout_weight / nLeftWeight;
                nLeftWeight -= layoutInParent->layout_weight;
                nLeftSpace -= widthChild;
            }
            else if (viewChild->layout_width == LayoutParams::match_parent)
            {
                widthChild = nExpandSpace;
            }
            else
            {
                widthChild = viewChild->wrapBoundWidth();
            }
            chRect childBound = rectLTWH(xPos, rectChildrenBound.top, widthChild, heightChild);

            // child layout_gravity will rewrite parent's gravity if layout_gravity exist
            adjustRectByGravity(rectChildrenBound, gravity & gravity_y_mask, childBound);
            adjustRectByGravity(rectChildrenBound, layoutInParent->layout_gravity & gravity_y_mask, childBound);

            viewChild->LayoutInBoundRect(childBound);
            xPos = childBound.right;
        }
    }
    else
    {
        int nFixedSpace = 0;
        int nMatchParentCount = 0;
        int nTotalWeight = 0;
        for (xViewIterator it = xViewTree::child_head(); it.hasData(); ++it)
        {
            xView* viewChild = &(*it);

            if (NULL == viewChild || xView::gone == viewChild->visibility)
            {
                continue;
            }

            LinearLayoutParams* layoutInParent = LinearLayoutParams::GetLayoutParams(viewChild->m_pLayoutInParent);
            chASSERT(layoutInParent != NULL);

            if (NULL == layoutInParent)
            {
                continue;
            }

            if (layoutInParent->layout_weight != 0)
            {
#if IF_DEBUG_VIEW_MGR
                if (viewChild->layout_height != 0)
                {
                    XWINDOW_WARN("%s@ use height=0 for better performence", viewChild->viewNick().c_str());
                }
#endif
                nTotalWeight += layoutInParent->layout_weight;
            }
            else if (viewChild->layout_height == LayoutParams::match_parent)
            {
#if IF_DEBUG_VIEW_MGR
                if (nMatchParentCount != 0)
                {
                    chASSERTx(FALSE, "%s@ too many match_parent in layout", viewChild->viewNick().c_str());
                }
#endif
                nMatchParentCount ++;
            }
            else
            {
                nFixedSpace += viewChild->wrapBoundHeight();
            }
        }
#if IF_DEBUG_VIEW_MGR
        if (nMatchParentCount != 0 && nTotalWeight != 0)
        {
            chASSERTx(FALSE, "%s@ both match_parent and weight in child layout", viewNick().c_str());
        }
#endif
        int nExpandSpace = 0;
        if ((nMatchParentCount != 0 || nTotalWeight != 0) && nFixedSpace <= rectParentContent.Height())
        {
            nExpandSpace = rectParentContent.Height() - nFixedSpace;
            chWARNING(nExpandSpace > 0);
        }

        int nLeftWeight = nTotalWeight;
        int nLeftSpace = nExpandSpace;

        chRect rectChildrenBound = ResizeHeight(rectParentContent, nFixedSpace + nExpandSpace);
        adjustRectByGravity(rectParentContent, gravity & gravity_y_mask, rectChildrenBound);

        int yPos = rectChildrenBound.top;
        for (xViewIterator it = xViewTree::child_head(); it.hasData(); ++it)
        {
            xView* viewChild = &(*it);

            if (NULL == viewChild || xView::gone == viewChild->visibility)
            {
                continue;
            }

            LinearLayoutParams* layoutInParent = LinearLayoutParams::GetLayoutParams(viewChild->m_pLayoutInParent);
            chASSERT(layoutInParent != NULL);

            if (NULL == layoutInParent)
            {
                continue;
            }

            int widthChild = viewChild->measureWidthWithBound(rectChildrenBound.Width());
            int heightChild = 0;
            if (layoutInParent->layout_weight != 0)
            {
                heightChild = nLeftSpace * layoutInParent->layout_weight / nLeftWeight;
                nLeftWeight -= layoutInParent->layout_weight;
                nLeftSpace -= heightChild;
            }
            else if (viewChild->layout_height == LayoutParams::match_parent)
            {
                heightChild = nExpandSpace;
            }
            else
            {
                heightChild = viewChild->wrapBoundHeight();
            }
            chRect childBound = rectLTWH(rectChildrenBound.left, yPos, widthChild, heightChild);

            // child layout_gravity will rewrite parent's gravity if layout_gravity exist
            adjustRectByGravity(rectChildrenBound, gravity & gravity_x_mask, childBound);
            adjustRectByGravity(rectChildrenBound, layoutInParent->layout_gravity & gravity_x_mask, childBound);
            viewChild->LayoutInBoundRect(childBound);
            yPos = childBound.bottom;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
FrameLayoutParams::FrameLayoutParams()
    : LayoutParamsInParent(FRAME_LAYOUT_PARAMS)
{
    layout_gravity = gravity_undefined;
}

FrameLayoutParams* FrameLayoutParams::GetLayoutParams(LayoutParamsInParent* pLayout)
{
    if (NULL != pLayout && FRAME_LAYOUT_PARAMS == pLayout->m_nType)
    {
        return static_cast<FrameLayoutParams*>(pLayout);
    }

    return NULL;
}

void FrameLayoutParams::loadAttributes(xml_node& node)
{
    LayoutParamsInParent::loadAttributes(node);

    PopupAndroidGravity(node, XWINDOW_NAME_LAYOUT_GRAVITY, layout_gravity);
}

//////////////////////////////////////////////////////////////////////////
EXPORT_VIEW_CLASS(xFrameLayout);

xFrameLayout::xFrameLayout()
{
    REGISTER_VIEW_DUMP_CLASS(xFrameLayout)
}

void xFrameLayout::LayoutContentInRect(const chRect& rectParentContent)
{
    for (xViewIterator it = xViewTree::child_head(); it.hasData(); ++it)
    {
        xView* viewChild = &(*it);

        if (NULL == viewChild || xView::gone == viewChild->visibility)
        {
            continue;
        }

        FrameLayoutParams* layoutInParent = FrameLayoutParams::GetLayoutParams(viewChild->m_pLayoutInParent);
        chASSERT(layoutInParent != NULL);

        if (NULL == layoutInParent)
        {
            continue;
        }

        int widthChild = viewChild->measureWidthWithBound(rectParentContent.Width());
        int heightChild = viewChild->measureHeightWithBound(rectParentContent.Height());

        chRect childBound = rectLTWH(rectParentContent.left, rectParentContent.top, widthChild, heightChild);
        adjustRectByGravity(rectParentContent, layoutInParent->layout_gravity, childBound);
        viewChild->LayoutInBoundRect(childBound);
    }
}

int xFrameLayout::wrapContentWidth()
{
    int result = 0;
    for (xViewIterator it = xViewTree::child_head(); it.hasData(); ++it)
    {
        xView* viewChild = &(*it);
        if (viewChild->visibility == xView::gone)
        {
            continue;
        }

        int childValue = viewChild->wrapBoundWidth();
        result = chMAX(result, childValue);
    }
    return result;
}

int xFrameLayout::wrapContentHeight()
{
    int result = 0;
    for (xViewIterator it = xViewTree::child_head(); it.hasData(); ++it)
    {
        xView* viewChild = &(*it);
        if (viewChild->visibility == xView::gone)
        {
            continue;
        }

        int childValue = viewChild->wrapBoundHeight();
        result = chMAX(result, childValue);
    }
    return result;
}

//////////////////////////////////////////////////////////////////////////
#if IF_SCALE_LAYOUT_ENABLE
ScaleLayoutParams::ScaleLayoutParams()
    : LayoutParamsInParent(SCALE_LAYOUT_PARAMS)
{

}

ScaleLayoutParams* ScaleLayoutParams::GetLayoutParams(LayoutParamsInParent* pLayout)
{
    if (NULL != pLayout && SCALE_LAYOUT_PARAMS == pLayout->m_nType)
    {
        return (ScaleLayoutParams*)pLayout;
    }

    return NULL;
}

void ScaleLayoutParams::loadAttributes(xml_node& node)
{
    LayoutParamsInParent::loadAttributes(node);

    PopupAndroidRect(node, XWINDOW_NAME_SCALE_LTWH, scale_rect);

    // set match_parent to child views
    chASSERT(m_pHostView->layout_width == LayoutParams::match_parent);
    chASSERT(m_pHostView->layout_height == LayoutParams::match_parent);
    m_pHostView->layout_width = LayoutParams::match_parent;
    m_pHostView->layout_height = LayoutParams::match_parent;
}

EXPORT_VIEW_CLASS(xScaleLayout);
xScaleLayout::xScaleLayout()
{
    REGISTER_VIEW_DUMP_CLASS(xScaleLayout)
    scale_width = scale_height = 100;
}

void xScaleLayout::loadAttributes(xml_node& node)
{
    xViewGroup::loadAttributes(node);

    PopupAndroidInt(node, XWINDOW_NAME_SCALE_HEIGHT, scale_height);
    PopupAndroidInt(node, XWINDOW_NAME_SCALE_WIDTH, scale_width);
}

void xScaleLayout::LayoutContentInRect(const chRect& rectParentContent)
{
    double xScaleParam = rectParentContent.Width() / (scale_width * 1.0);
    double yScaleParam = rectParentContent.Height() / (scale_height * 1.0);

    for (xViewIterator it = xViewTree::child_head(); it.hasData(); ++it)
    {
        xView* viewChild = &(*it);

        if (NULL == viewChild || viewChild->visibility != xView::visible)
        {
            continue;
        }

        ScaleLayoutParams* layoutInParent = ScaleLayoutParams::GetLayoutParams(viewChild->m_pLayoutInParent);
        chASSERT(layoutInParent != NULL);

        if (NULL == layoutInParent)
        {
            continue;
        }

        int l = (int)(layoutInParent->scale_rect.left * xScaleParam);
        int t = (int)(layoutInParent->scale_rect.top * yScaleParam);
        int r = (int)(layoutInParent->scale_rect.right * xScaleParam);
        int b = (int)(layoutInParent->scale_rect.bottom * yScaleParam);

        viewChild->LayoutInBoundRect(chRect(l, t, r, b));
    }
}
#endif

//////////////////////////////////////////////////////////////////////////
#if IF_RECT_VIEW_ENABLE
xRectView::xRectView()
    : rect_corner_size(0), rect_corner_type(0), rect_color(0, 0, 0)
{

}

void xRectView::loadAttributes(xml_node& node)
{
    xView::loadAttributes(node);

    PopupAndroidInt(node, XWINDOW_NAME_RECT_CORNER_SIZE, rect_corner_size);
    PopupAndroidColor(node, XWINDOW_NAME_RECT_COLOR, rect_color);
    PopupAndroidCorner(node, XWINDOW_NAME_RECT_CORNER_TYPE, rect_corner_type);
}

bool xRectView::onPaintContent(xPainter& painter)
{
    int roundPix = rect_corner_size;
    chRect rectLogic = rectLTWH(0, 0, width(), height());
    const xColor& color = rect_color;
    if (roundPix < 0)
    {
        // 小于0，使用默认值
        roundPix = 3;
    }

    if (rectLogic.Width() < roundPix || rectLogic.Height() < roundPix)
    {
        painter.drawRect(rectLogic, color);

        return true;
    }

    chRect rect = rectLogic;

    rect.DeflateRect(0, 0, 1, 1);

    painter.drawRoundedRect(rect, roundPix, roundPix, color, rect_corner_type);

    return true;
}

void xRectView::LayoutContentInRect(const chRect& rectFrame)
{
    for (xViewIterator it = xViewTree::child_head(); it.hasData(); ++it)
    {
        xView* viewChild = &(*it);
        if (viewChild->visibility == xView::gone)
        {
            continue;
        }

        chRect childFrame = rectFrame;
        viewChild->LayoutInBoundRect(childFrame.DeflateRect(1, 1));
    }
}
#endif

//////////////////////////////////////////////////////////////////////////
RootLayoutParams::RootLayoutParams()
    : LayoutParamsInParent(ROOT_LAYOUT_PARAMS)
{
    activable = true;
    layout_gravity = gravity_undefined;
}

void RootLayoutParams::loadAttributes(xml_node& node)
{
    LayoutParamsInParent::loadAttributes(node);

    PopupAndroidGravity(node, XWINDOW_NAME_LAYOUT_GRAVITY, layout_gravity);
    PopupAndroidBool(node, XWINDOW_NAME_ACTIVABLE, activable);
    PopupAndroidRect(node, XWINDOW_NAME_LOGIC_LTWH, logic_ltwh);
}

void RootLayoutParams::RelayoutRootView()
{
    chASSERT(m_pHostView != NULL && m_pHostView->m_pParent == NULL);
    xWorkStation* pHostStation = m_pHostView->m_pHostStation;

    if (NULL == pHostStation)
    {
        return;
    }

    int wScreen = pHostStation->width();
    int hScreen = pHostStation->height();
    chRect rectBound;

    if (!logic_ltwh.IsEmpty())
    {
        int wLogic = pHostStation->profile().m_sizeLogic.cx;
        int hLogic = pHostStation->profile().m_sizeLogic.cy;
        rectBound.left = logic_ltwh.left * wScreen / wLogic;
        rectBound.right = logic_ltwh.right * wScreen / wLogic;
        rectBound.top = logic_ltwh.top * hScreen / hLogic;
        rectBound.bottom = logic_ltwh.bottom * hScreen / hLogic;
    }
    else
    {
        rectBound.left = 0;
        if (m_pHostView->LayoutParams::layout_width == LayoutParams::match_parent)
        {
            rectBound.right = wScreen;
        }
        else
        {
            rectBound.right = m_pHostView->wrapBoundWidth();
        }
        rectBound.top = 0;
        if (m_pHostView->LayoutParams::layout_height == LayoutParams::match_parent)
        {
            rectBound.bottom = hScreen;
        }
        else
        {
            rectBound.bottom = m_pHostView->wrapBoundHeight();
        }
        if (layout_gravity != gravity_undefined)
        {
            adjustRectByGravity(chRect(0, 0, wScreen, hScreen), layout_gravity, rectBound);
        }
    }
    pHostStation->LayoutViewInBoundRect(m_pHostView, rectBound);
}

void RootLayoutParams::RelayoutRootView(xView* pRootView)
{
    chASSERT(pRootView != NULL && pRootView->m_pParent == NULL && pRootView->m_pWindow != NULL);

    if (NULL == pRootView)
    {
        return;
    }

#if IF_DEBUG_VIEW_MGR
    XWINDOW_INFO("Begin RelayoutRootView RootView:%s", pRootView->viewNick().c_str());
#endif

    RootLayoutParams* pLayoutInParent = RootLayoutParams::GetLayoutParams(pRootView->m_pLayoutInParent);
    chASSERT(pLayoutInParent != NULL);

    if (NULL != pLayoutInParent)
    {
        pLayoutInParent->RelayoutRootView();
    }
}

RootLayoutParams* RootLayoutParams::GetLayoutParams(LayoutParamsInParent* pLayout)
{
    if (NULL != pLayout && ROOT_LAYOUT_PARAMS == pLayout->m_nType)
    {
        return static_cast<RootLayoutParams*>(pLayout);
    }

    return NULL;
}
