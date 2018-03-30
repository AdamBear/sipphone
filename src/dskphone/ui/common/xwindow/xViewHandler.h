#ifndef __X_VIEW_HANDLER_H__
#define __X_VIEW_HANDLER_H__

#include "xGraphic.h"
#include "xViewEvent.h"
#include <xmlparser/xmlparser.hpp>
#include "xwindow-kernel/include/xthreadtimer.h"


#define DECALRE_RUNTIME_FAMILY(clsName) \
    class clsName;\
    typedef clsName* (*methodGetObjectOf##clsName)();\
    class XWINDOW_EXPORT ClassNode_##clsName    {\
    public:\
        ClassNode_##clsName(LPCSTR lpszClassName, methodGetObjectOf##clsName pMethod);\
        static clsName* getObjectByName(const yl::string& strClassName);\
    public:\
        yl::string                          m_strClassName;\
        methodGetObjectOf##clsName          m_pMethodGetObject;\
        ClassNode_##clsName*                m_pNextNode;\
    }

#define DECALRE_GET_OBJECT_METHOD(clsName) \
    public:\
    static clsName* getObjectByName(const yl::string& name)

#define IMPLEMENT_RUNTIME_FAMILY(clsName) \
    static ClassNode_##clsName* s_pFirstClassNode_##clsName = NULL;\
    ClassNode_##clsName::ClassNode_##clsName(LPCSTR lpszClassName, methodGetObjectOf##clsName pMethod) {\
        m_strClassName = lpszClassName[0] == 'x' ? lpszClassName + 1 : lpszClassName;\
        m_pMethodGetObject = pMethod;\
        m_pNextNode = NULL;\
        if(s_pFirstClassNode_##clsName == NULL) {\
            s_pFirstClassNode_##clsName = this;\
        } else {\
            ClassNode_##clsName* pLastNode = s_pFirstClassNode_##clsName;\
            while(pLastNode->m_pNextNode != NULL) {\
                pLastNode = pLastNode->m_pNextNode;\
            }\
            pLastNode->m_pNextNode = this;\
        }\
    }\
    clsName* ClassNode_##clsName::getObjectByName(const yl::string& strClassName) {\
        for(ClassNode_##clsName* pNode = s_pFirstClassNode_##clsName; pNode != NULL; pNode = pNode->m_pNextNode) {\
            if(pNode->m_strClassName == strClassName) {\
                return pNode->m_pMethodGetObject();\
            }\
        }\
        return NULL;\
    }\
    clsName* clsName::getObjectByName(const yl::string& name) {\
        return ClassNode_##clsName::getObjectByName(name); \
    }

#if IF_DEBUG_PARSER
extern yl::string GetNodeAttrString(xml_node& node);
#endif

class xWorkStation;
class XWINDOW_EXPORT xXmlLoader
{
public:
    xWorkStation& ownerStation() const;
public:
    virtual void loadXmlContent(xml_node& node);
    virtual void loadChildrenElements(xml_node& node);
    virtual void loadAttributes(xml_node& node);
    virtual void onLoadChildElement(xml_node& nodeChild);
public:
    xXmlLoader();
    virtual ~xXmlLoader();
public:
    xWorkStation*           m_pHostStation;
};

class XWINDOW_EXPORT xViewInterface
{
public:
    virtual bool onViewEvent(xView* pView, xViewEvent* pEvent);

    virtual bool onViewKeyPressedEvent(xView* pView, int key);
    virtual bool onViewKeyReleasedEvent(xView* pView, int key);
    virtual bool onViewKeyLongPressedEvent(xView* pView, int key);

#if IF_MULTI_TOUCH_ENABLE
    virtual bool onViewTouchBeganEvent(xView* pView, xView* pAnchorView, LIST_RAW_TOUCH_POINT& lsTouchPoint);
    virtual bool onViewTouchMovedEvent(xView* pView, xView* pAnchorView, LIST_RAW_TOUCH_POINT& lsTouchPoint);
    virtual bool onViewTouchEndedEvent(xView* pView, xView* pAnchorView, LIST_RAW_TOUCH_POINT& lsTouchPoint);
    virtual bool onViewTouchCancelEvent(xView* pView, xView* pAnchorView, LIST_RAW_TOUCH_POINT& lsTouchPoint);
#endif

    virtual bool onViewActiveEvent(xView* pView, bool bActive);
    virtual bool onViewFocusEvent(xView* pView, bool bFocusIn);
    virtual bool onViewFocusMoveEvent(xView* pView, xView* pNewFocus, xView* pOldFocus);

    virtual bool onViewPaintBackground(xView* pView, xPainter&);
    virtual bool onViewPaintContent(xView* pView, xPainter& painter);
#if IF_VIEW_MASK
    virtual bool onViewPaintMask(xView*, xPainter&);
#endif

    virtual bool onViewHide(xView* pView);
    virtual bool onViewClose(xView* pView);

    virtual bool onViewRectChanged(xView* pView, const chRect& rectOld);

#if IF_GESTURE_ENABLE
    virtual bool onViewGestureEvent(xView* pView, xViewEvent* pGestureEvent);
#endif

public:
    virtual ~xViewInterface();
};


DECALRE_RUNTIME_FAMILY(xStyle);
#define EXPORT_STYLE_CLASS(name) \
    static xStyle* method_of_get_##name() {\
    static name tmp_obj_of_##name; return &tmp_obj_of_##name;\
} DECLARE_EXPORT ClassNode_xStyle s_Node_Of_##name(#name, method_of_get_##name)

class XWINDOW_EXPORT xStyle : public xViewInterface
{
    DECALRE_GET_OBJECT_METHOD(xStyle);
};

DECALRE_RUNTIME_FAMILY(xViewHandler);
#define EXPORT_VIEW_HANDLER_CLASS(name) \
    static xViewHandler* method_of_get_##name() {\
        return new name();\
} DECLARE_EXPORT ClassNode_xViewHandler s_Node_Of_##name(#name, method_of_get_##name)

// ViewHandle类型(每个继承类使用一个字节来区分)
enum
{
    VIEW_HANDLE_TYPE_BASE               = 0x00000000,
    VIEW_HANDLE_TYPE_MARK               = 0xFF000000,
    VIEW_HANDLE_TYPE_TEXT               = 0x01000000,
    VIEW_HANDLE_TYPE_GIF_IMAGE          = 0x04000000,
    VIEW_HANDLE_TYPE_BTN_DISABLE_BG     = 0x05000000,
    VIEW_HANDLE_TYPE_VALUE              = 0x06000000,
};

class XWINDOW_EXPORT xViewHandler : public xXmlLoader
    , public xViewInterface
{
    DECALRE_GET_OBJECT_METHOD(xViewHandler);
public:
    static bool dispatchViewEventOnHandler(xView* pView, xViewEvent* pEvent);
    virtual void onViewConnected()              { }
    virtual void onViewDisconnected()           { }
public:
    explicit xViewHandler(int nType = VIEW_HANDLE_TYPE_BASE);
    virtual ~xViewHandler();
public:
    int                         m_nType;
    xView*                      m_pHostView;
    xViewHandler*               m_pNextHandler;
};

#endif // __X_VIEW_HANDLER_H__
