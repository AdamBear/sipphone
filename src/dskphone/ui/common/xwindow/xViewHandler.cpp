#include "xViewHandler.h"
#include "xWindowManager.h"
//#include "hardwareDefine.h"
#include "commonapi/stringhelperapi.h"
#include "dsklog/log.h"


#if IF_DEBUG_PARSER
yl::string GetNodeAttrString(xml_node& node)
{
    yl::string strAttr = "";

    if (!node.empty())
    {
        xml_attribute attr = node.first_attribute();

        while (!attr.empty())
        {
            yl::string strName = attr.name();
            yl::string strValue = attr.as_string();

            strAttr = strAttr + strName + "=\"" + strValue + "\" ";

            attr = attr.next_attribute();
        }
    }

    return strAttr;
}
#endif

//////////////////////////////////////////////////////////////////////////
// class xXmlLoader
xXmlLoader::xXmlLoader()
{
    m_pHostStation = &g_WorkStation;
}

xXmlLoader::~xXmlLoader()
{
    m_pHostStation = NULL;
}

xWorkStation& xXmlLoader::ownerStation() const
{
    chASSERT(m_pHostStation != NULL);
    return *m_pHostStation;
}

void xXmlLoader::loadXmlContent(xml_node& node)
{
#if IF_DEBUG_PARSER
    XWINDOW_INFO("(@%s:%p)", node.name(), this);
#endif
    loadChildrenElements(node);
    loadAttributes(node);
}

void xXmlLoader::loadChildrenElements(xml_node& node)
{
    if (node.empty())
    {
        return;
    }

    xml_node nodeChild = node.first_child();

    while (!nodeChild.empty())
    {
        yl::string strName = nodeChild.name();

#if IF_DEBUG_PARSER
        XWINDOW_INFO("(%s)", strName.c_str());
#endif

        onLoadChildElement(nodeChild);

#if IF_DEBUG_PARSER
        if (!nodeChild.first_attribute().empty())
        {
            chASSERTx(0, "Unknow attribute:%s of %s", GetNodeAttrString(nodeChild).c_str(), nodeChild.name());
        }
#endif
        nodeChild = nodeChild.next_sibling();
    }
}

void xXmlLoader::loadAttributes(xml_node& node)             { }
void xXmlLoader::onLoadChildElement(xml_node& node)         { }

//////////////////////////////////////////////////////////////////////////
xViewInterface::~xViewInterface()                           { }

bool xViewInterface::onViewEvent(xView* pView, xViewEvent* pEvent)
{
    switch (pEvent->type)
    {
    case VIEW_KEY_PRESS:
        return onViewKeyPressedEvent(pView, (static_cast<xViewEvent_key*>(pEvent))->value_key);
    case VIEW_KEY_RELEASE:
        return onViewKeyReleasedEvent(pView, (static_cast<xViewEvent_key*>(pEvent))->value_key);
    case VIEW_KEY_LONG_PRESS:
        return onViewKeyLongPressedEvent(pView, (static_cast<xViewEvent_key*>(pEvent))->value_key);

#if IF_MULTI_TOUCH_ENABLE
    case VIEW_TOUCH_BEGAN:
    {
        xTouchEvent* pTouchEvent = static_cast<xTouchEvent*>(pEvent);

        return onViewTouchBeganEvent(pView, pTouchEvent->m_pAnchorView, pTouchEvent->m_listPoint);
    }
    break;
    case VIEW_TOUCH_MOVED:
    {
        xTouchEvent* pTouchEvent = static_cast<xTouchEvent*>(pEvent);

        return onViewTouchMovedEvent(pView, pTouchEvent->m_pAnchorView, pTouchEvent->m_listPoint);
    }
    break;
    case VIEW_TOUCH_ENDED:
    {
        xTouchEvent* pTouchEvent = static_cast<xTouchEvent*>(pEvent);

        return onViewTouchEndedEvent(pView, pTouchEvent->m_pAnchorView, pTouchEvent->m_listPoint);
    }
    break;
    case VIEW_TOUCH_CANCEL:
    {
        xTouchEvent* pTouchEvent = static_cast<xTouchEvent*>(pEvent);

        return onViewTouchCancelEvent(pView, pTouchEvent->m_pAnchorView, pTouchEvent->m_listPoint);
    }
    break;
#endif
#if IF_GESTURE_ENABLE
    case VIEW_GESTURE:
    {
        return onViewGestureEvent(pView, pEvent);
    }
    break;
#endif

    case VIEW_FOCUS_ENTER:
        return onViewFocusEvent(pView, true);
    case VIEW_FOCUS_LEAVE:
        return onViewFocusEvent(pView, false);
    case VIEW_FOCUS_MOVE:
        return onViewFocusMoveEvent(pView, (static_cast<xViewEvent_FocusMove*>(pEvent))->m_pNewFocus,
                                    (static_cast<xViewEvent_FocusMove*>(pEvent))->m_pOldFocus);
    case VIEW_ACTIVE:
        return onViewActiveEvent(pView, (static_cast<xViewEvent_bool*>(pEvent))->value_bool);
    case VIEW_PAINT_BACKGROUND:
        return onViewPaintBackground(pView, (static_cast<xViewEvent_paint*>(pEvent))->value_painter);
    case VIEW_PAINT_CONTENT:
        return onViewPaintContent(pView, (static_cast<xViewEvent_paint*>(pEvent))->value_painter);
#if IF_VIEW_MASK
    case VIEW_PAINT_MASK:
        return onViewPaintMask(pView, (static_cast<xViewEvent_paint*>(pEvent))->value_painter);
#endif

    default:
        return false;
    }
    return false;
}

bool xViewInterface::onViewKeyPressedEvent(xView*, int)
{
    return false;
}
bool xViewInterface::onViewKeyReleasedEvent(xView*, int)
{
    return false;
}
bool xViewInterface::onViewKeyLongPressedEvent(xView*, int)
{
    return false;
}

#if IF_MULTI_TOUCH_ENABLE
bool xViewInterface::onViewTouchBeganEvent(xView* pView, xView* pAnchorView, LIST_RAW_TOUCH_POINT& lsTouchPoint)
{
    return false;
}
bool xViewInterface::onViewTouchMovedEvent(xView* pView, xView* pAnchorView, LIST_RAW_TOUCH_POINT& lsTouchPoint)
{
    return false;
}
bool xViewInterface::onViewTouchEndedEvent(xView* pView, xView* pAnchorView, LIST_RAW_TOUCH_POINT& lsTouchPoint)
{
    return false;
}
bool xViewInterface::onViewTouchCancelEvent(xView* pView, xView* pAnchorView, LIST_RAW_TOUCH_POINT& lsTouchPoint)
{
    return false;
}
#endif

bool xViewInterface::onViewActiveEvent(xView*, bool)
{
    return false;
}
bool xViewInterface::onViewFocusEvent(xView*, bool)
{
    return false;
}
bool xViewInterface::onViewFocusMoveEvent(xView*, xView*, xView*)
{
    return false;
}

bool xViewInterface::onViewPaintContent(xView*, xPainter&)
{
    return false;
}
bool xViewInterface::onViewPaintBackground(xView*, xPainter&)
{
    return false;
}
#if IF_VIEW_MASK
bool xViewInterface::onViewPaintMask(xView*, xPainter&)
{
    return false;
}
#endif

bool xViewInterface::onViewHide(xView* pView)
{
    return false;
}
bool xViewInterface::onViewClose(xView* pView)
{
    return false;
}

bool xViewInterface::onViewRectChanged(xView* pView, const chRect& rectOld)
{
    return false;
}

#if IF_GESTURE_ENABLE
bool xViewInterface::onViewGestureEvent(xView* pView, xViewEvent* pGestureEvent)
{
    return false;
}
#endif

//////////////////////////////////////////////////////////////////////////
// class xStyle
IMPLEMENT_RUNTIME_FAMILY(xStyle);

IMPLEMENT_RUNTIME_FAMILY(xViewHandler);
xViewHandler::xViewHandler(int nType/* = VIEW_HANDLE_TYPE_BASE*/)
    : m_nType(nType)
    , m_pHostView(NULL)
    , m_pNextHandler(NULL)
{
}

xViewHandler::~xViewHandler()
{
    m_pHostView = NULL;
    if (m_pNextHandler != NULL)
    {
        delete m_pNextHandler;
        m_pNextHandler = NULL;
    }
}

bool xViewHandler::dispatchViewEventOnHandler(xView* pTargetView, xViewEvent* pEvent)
{
    for (xViewHandler* pHandler = pTargetView->m_pHandler; pHandler != NULL; pHandler = pHandler->m_pNextHandler)
    {
        if (pHandler->onViewEvent(pTargetView, pEvent))
        {
            return true;
        }
    }
    return false;
}
