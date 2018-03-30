#ifndef __XWINDOW_H__
#define __XWINDOW_H__

#include <ylhashmap.h>
#include <ylsmartptr.h>
#include "xViewHandler.h"
#include "xviewrenderer.h"
#include "xwindow-kernel/include/xsignal.h"
#include "overlengtheffectbase.h"
#include "ylvector.h"
#include "viewdumpcommon.h"
#if IF_GESTURE_ENABLE
#include "gesture/include/gesture.h"
#endif

DECALRE_RUNTIME_FAMILY(xView);
#define EXPORT_VIEW_CLASS(name) \
    static xView* method_of_get_##name() {\
        xView* child = new name();\
        child->m_strId = commonAPI_FormatString("@%s:%p", #name + 1, child);\
        return child;\
} ClassNode_xView s_Node_Of_##name(#name, method_of_get_##name)

//////////////////////////////////////////////////////////////////////////
class xView;
class XWINDOW_EXPORT xViewIterator
{
public:
    bool hasData() const;
public:
    xViewIterator& operator++();
    xViewIterator& operator--();
    xViewIterator operator + (int iOffset) const;
public:
    xView* operator->()
    {
        return m_pHostView;
    }
    xView& operator*()
    {
        return *m_pHostView;
    }
public:
    explicit xViewIterator(xView* pView);
private:
    xView*          m_pHostView;
};

class xViewGroup;
class XWINDOW_EXPORT xViewTree
{
    friend class xViewIterator;
public:
    inline bool hasChild() const
    {
        return m_pChild != NULL;
    }

    inline int childrenCount() const
    {
        return m_nChildreCount;
    }

    inline xViewGroup* parent() const
    {
        return m_pParent;
    }

    xView* getRootView();

    void push_child(xView* pChild);
    void insert_child(int iIndex, xView* pChild);
    void popup_child(xView* pChild);
    int get_child_index(xView* pChild) const;
    xView* child_at(int iIndex) const;
public:
    xViewIterator child_head() const
    {
        return xViewIterator(m_pChild);
    }
    xViewIterator child_tail() const
    {
        return xViewIterator(m_pLastChild);
    }
private:
    void setRootView(xView* pRootView);
protected:
    xViewTree();
public:
    xViewGroup*     m_pParent;
    xView*          m_pNext;
    xView*          m_pPrev;
    xView*          m_pChild;
    xView*          m_pLastChild;
    xView*          m_pRootView;
    int             m_nChildreCount;
};

#define TML_SYSTEM              5
#define TML_USER                10
#define TML_DEFAULT             20

XWINDOW_EXPORT void adjustRectByGravity(const chRect& rect, const int gravity, chRect& rectResult);
XWINDOW_EXPORT bool PopupAndroidBOOL(xml_node& node, const char* pszName, int& value);
XWINDOW_EXPORT bool PopupAndroidBool(xml_node& node, const char* pszName, bool& value);
XWINDOW_EXPORT bool PopupAndroidInt(xml_node& node, const char* pszName, int& value);
XWINDOW_EXPORT bool PopupAndroidDouble(xml_node& node, const char* pszName, double& value);
XWINDOW_EXPORT bool PopupAndroidPixel(xml_node& node, const char* pszName, int& value);
XWINDOW_EXPORT bool PopupAndroidFont(xml_node& node, const char* pszName, int& value);
XWINDOW_EXPORT bool PopupAndroidString(xml_node& node, const char* pszName, yl::string& value);
XWINDOW_EXPORT bool PopupAndroidRect(xml_node& node, const char* pszName, chRect& value);
XWINDOW_EXPORT bool PopupAndroidColor(xml_node& node, const char* pszName, xColor& value);
XWINDOW_EXPORT bool PopupAndroidDimension(xml_node& node, const char* pszName, int& value);
XWINDOW_EXPORT bool PopupAndroidGravity(xml_node& node, const char* pszName, int& value);
XWINDOW_EXPORT bool PopupAndroidVisibility(xml_node& node, const char* pszName, int& value);
XWINDOW_EXPORT bool PopupAndroidOrientation(xml_node& node, const char* pszName, int& value);
XWINDOW_EXPORT bool PopupAndroidPixmap(xml_node& node, const char* pszName, xPixmap& value);
XWINDOW_EXPORT bool PopupAndroidBrush(xml_node& node, const char* pszName, xBrush& value);
XWINDOW_EXPORT bool PopupAndroidCorner(xml_node& node, const char* pszName, int& value);
XWINDOW_EXPORT bool PopupAndroidOverLengthStyle(xml_node& node, const char* pszName, YLVector<int>& vecType);
XWINDOW_EXPORT bool PopupAndroidArray(xml_node& node, const char* pszName, YLVector<int>& vec);

// xView :      non-float/no-children/must have parent/non-active/non-focus
// xControl :   non-float/no-children/must have parent/non-active/focus-able

// xViewGroup : can float/can have children/must have parent/non-active/non-focus
// xWindow :    float/can have children/can have parent/active-able/non-focus
// xBar :       can float/can have children/can have parent/non-active/non-focus

// xLineEdit
// xButton
// xCheckBox
// xCombBox
// xListView
// xIconView
// xTreeView

// XML layout attributes named layout_something define layout parameters for the View
// that are appropriate for the ViewGroup in which it resides.
class XWINDOW_EXPORT LayoutParams
{
public:
    virtual void loadAttributes(xml_node& node);
public:
    LayoutParams();
    virtual ~LayoutParams();
public:
    enum
    {
        match_parent = -1,
        wrap_content = -2,
    };
public:
    int layout_width;                       // Specifies the basic height of the view.
    int layout_height;                      // MATCH_PARENT = -1,WRAP_CONTENT = -2

    /*
    ╱══:BoundRect
    ╱━━:FrameRect (paint and mouse effect on this rect)
    ╱──:ContentRect
    ╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱
    ╱╔═══════════╗╱
    ╱║╱╱╱╱╱╱╱╱╱╱╱║╱
    ╱║╱┏━━━━━━━┓╱║╱
    ╱║╱┃╲╲╲╲╲╲╲┃╱║╱
    ╱║╱┃╲┌───┐╲┃╱║╱
    ╱║╱┃╲│╲╲╲│╲┃╱║╱
    ╱║╱┃╲│╲╲╲│╲┃╱║╱
    ╱║╱┃╲│╲╲╲│╲┃╱║╱
    ╱║╱┃╲└───┘╲┃╱║╱
    ╱║╱┃╲╲╲╲╲╲╲┃╱║╱
    ╱║╱┗━━━━━━━┛╱║╱
    ╱║╱╱╱╱╱╱╱╱╱╱╱║╱
    ╱╚═══════════╝╱
    ╱╱╱╱╱╱╱╱╱╱╱╱╱╱╱
    */
    int layout_marginLeft;                  // space between frame and bound
    int layout_marginTop;
    int layout_marginRight;
    int layout_marginBottom;

    int paddingLeft;                        // space between frame and content
    int paddingRight;
    int paddingTop;
    int paddingBottom;
};

#define LAYOUT_PARAMS_IN_PARENT_TYPE_BASE 0

class XWINDOW_EXPORT LayoutParamsInParent
{
public:
    virtual void loadAttributes(xml_node& node);

public:
    explicit LayoutParamsInParent(int nType = LAYOUT_PARAMS_IN_PARENT_TYPE_BASE);
    virtual ~LayoutParamsInParent();

public:
    static LayoutParamsInParent* GetLayoutParams(LayoutParamsInParent* pLayout);

public:
    xView*  m_pHostView;
    int     m_nType;
};

#define DECLARE_LAYOUT_PARAMS_IN_PARENT( type ) \
private: \
  virtual LayoutParamsInParent* allocLayout()   { return new type; }\
  virtual bool checkLayout(LayoutParamsInParent* layout) { if (NULL != type::GetLayoutParams(layout)) return true; chASSERT(FALSE); return false; }

// margin : space outside
// padding : space inside
// frame : rect of visible
// bound : frame and margin(outside space)
// content : frame exclude padding(inside space)
class ExtraData
{
public:
    virtual ~ExtraData() {}
};
typedef YLSmartPtr<ExtraData>                       ViewDataFrame;
typedef YLHashMap<yl::string, ViewDataFrame>        ViewPropertys;

class xPainter;
class xViewGroup;
class dataWindowObject;
class xViewHandler;
class XWINDOW_EXPORT xView : public xXmlLoader
    , public xViewTree
    , public LayoutParams
{
    friend class xWorkStation;
    friend class xViewGroup;
    friend class xStyle;
    friend class xXmlLoader;
    friend class ViewOrWindow;
    DECALRE_GET_OBJECT_METHOD(xView);
public:
    virtual void showValue(const yl::string&);
#if IF_XWINDOW_DEBUG
    void dump(int iLevel = 0);
    yl::string viewNick();
#endif
public:
    template<typename T>
    T& viewAt(const yl::string& id)
    {
#if IF_XWINDOW_RTTI_CHECK
        T* pView = dynamic_cast<T*>(getViewById(id));
#else
        T* pView = static_cast<T*>(getViewById(id));
#endif
        chASSERT(pView != NULL);
        return *pView;
    }
    template<typename T>
    T& handlerAt(const yl::string& id)
    {
#if IF_XWINDOW_RTTI_CHECK
        T* pHandler = dynamic_cast<T*>(viewAt<xView>(id).m_pHandler);
#else
        T* pHandler = static_cast<T*>(viewAt<xView>(id).m_pHandler);
#endif
        chASSERT(pHandler != NULL);
        return *pHandler;
    }
    template<typename T>
    T* findView(const yl::string& id)
    {
#if IF_XWINDOW_RTTI_CHECK
        return dynamic_cast<T*>(getViewById(id));
#else
        return static_cast<T*>(getViewById(id));
#endif
    }
    template<typename T>
    T* findHandler(const yl::string& id)
    {
        return T::GetHandle(viewAt<xView>(id).m_pHandler);
    }
    template<typename T>
    T* findHandler()
    {
        for (xViewHandler* pHandler = this->m_pHandler; pHandler != NULL; pHandler = pHandler->m_pNextHandler)
        {
            T* pResult = T::GetHandle(pHandler);
            if (pResult != NULL)
            {
                return pResult;
            }
        }
        return NULL;
    }

    enum
    {
        FOCUS_REASON_NONE           = 0,
        FOCUS_REASON_DEFAULT        = 1,
        FOCUS_REASON_LEFT           = 2,
        FOCUS_REASON_RIGHT          = 3,
        FOCUS_REASON_UP             = 4,
        FOCUS_REASON_DOWN           = 5,
#if IF_PC_ENABLE
        FOCUS_REASON_PREVIOUS       = 6,
        FOCUS_REASON_NEXT           = 7,
#endif
    };

    enum
    {
        SHOW_CMD_NONE               = 0x00000000,
        SHOW_CMD_BRING_TO_TOP       = 0x00000001,           // 置顶
        SHOW_CMD_SET_ACTIVE         = 0x00000002,           // 活动窗口
        SHOW_CMD_DEFAULT            = 0x00000003,           // 默认值（置顶 + 活动窗口）
    };

    enum
    {
        RELAYOUT_ACTION_NONE            = 0,
        RELAYOUT_ACTION_CONTENT         = 0x00000001,
        RELAYOUT_ACTION_BOUND           = 0x00000002,
        RELAYOUT_ACTION_REDRAW_CONTENT  = 0x00000004,
        RELAYOUT_ACTION_RANGE_MASK      = 0x000000FF,
        RELAYOUT_ACTION_FORCE           = 0x00000100,
        RELAYOUT_ACTION_FORCE_MASK      = 0x0000FF00,
    };

    enum
    {
        VIEW_ASYNC_LIST_TYPE_NONE       = 0,
        VIEW_ASYNC_LIST_TYPE_RELAYOUT   = 0x01,
        VIEW_ASYNC_LIST_TYPE_REDRAW     = 0x02,
        VIEW_ASYNC_LIST_TYPE_DELETE     = 0x04,
    };

public:
    xView* getViewById(const yl::string& id);
    int getViewById(const yl::string& id, xView** outArray, int count = -1);
    dataWindowObject* getHostWindow();

    ViewDataFrame getProperty(const yl::string& strName);
    void setProperty(const yl::string& strName, ViewDataFrame pDataFrame);
    void releaseProperty(const yl::string& strName);
    bool hasProperty(const yl::string& strName);

    bool setStringProperty(const yl::string& strName, const yl::string& strValue);
    yl::string getStringProperty(const yl::string& strName, const yl::string& strDef);

    void setStyle(const yl::string& strStyle);

    void setGeometry(const chRect& rect);
    chRect geometry() const;

    chRect clientRect() const;
    chRect contentRect() const;

    chRect clientGeometry() const;
    chRect contentGeometry() const;

    inline bool checkVisible() const
    {
        return m_bAppear;
    }

    bool checkExposed(bool bInclueChildren) const;

    inline int height() const
    {
        return m_rectLayout.Height();
    }

    inline int width() const
    {
        return m_rectLayout.Width();
    }

    virtual void close();   // virtual for issue of save before close
    void show(int nShowCmd = SHOW_CMD_DEFAULT);
    void hide(bool bGone = false);
    void update(bool bUpdateChildren = false);
    void update(const chRect& rect, bool bUpdateChildren = false);

    virtual bool setFocus(int nFocusReason = FOCUS_REASON_DEFAULT);
    virtual xView* getDefaultFocusView();

    bool hasFocus() const;
    bool hasFocused() const;
    bool underFocus() const;
    bool underFocused() const;
    bool containFocus() const;
    bool containFocused() const;

    void setDisable(bool bDisable);
    inline bool checkDisable() const
    {
        return m_bDisable;
    }
    inline bool IsEnable() const
    {
        return !m_bDisable;
    }

#if IF_MULTI_TOUCH_ENABLE
    void setTouchable(bool bTouchable);
#endif

    void activateWindow();
    bool isActiveWindow();

    inline bool isVisible()
    {
        return (visibility == visible);
    }

#if IF_VIEW_MASK
    void setMask(bool bMask, bool bUpdate = true);
#endif

    void setBackground(const xColor& col);
    void setBackground(const xPixmap& pixmap);
    void setOverlapMode(bool bOverlap);
    void setViewHandler(xViewHandler* pHandler);

#if IF_GESTURE_ENABLE
    void grabGesture(int nGestureType);
    void ungrabGesture(int nGestureType);
    void clearGesture();
#endif
public: // xml interface
    virtual void loadXmlContent(xml_node& node);
    virtual void loadAttributes(xml_node& node);
    virtual void onLoadChildElement(xml_node& nodeChild);
public: // event interface
    virtual bool onEvent(xViewEvent* pEvent);
    virtual bool onKeyPressedEvent(int key);
    virtual bool onKeyReleasedEvent(int key);
    virtual bool onKeyLongPressedEvent(int key);
#if IF_MULTI_TOUCH_ENABLE
    virtual bool onTouchBeganEvent(xView* pAnchorView, LIST_RAW_TOUCH_POINT& lsTouchPoint);
    virtual bool onTouchMovedEvent(xView* pAnchorView, LIST_RAW_TOUCH_POINT& lsTouchPoint);
    virtual bool onTouchEndedEvent(xView* pAnchorView, LIST_RAW_TOUCH_POINT& lsTouchPoint);
    virtual bool onTouchCancelEvent(xView* pAnchorView, LIST_RAW_TOUCH_POINT& lsTouchPoint);
#endif
#if IF_GESTURE_ENABLE
    virtual bool onGestureEvent(xGestureEvent* pEvent);
#endif

    virtual bool onActiveEvent(bool bActive);
    virtual bool onFocusCustom(int nFocusReason);
    virtual bool onFocusEvent(bool bFocusIn);
    virtual bool onFocusMoveEvent(xView* pNewFocus, xView* pOldFoucs);
public: // layout interface
    virtual int wrapContentWidth();
    virtual int wrapContentHeight();
    virtual void LayoutContentInRect(const chRect&);

    int wrapFrameWidth();
    int wrapFrameHeight();

    int wrapBoundWidth();
    int wrapBoundHeight();

    inline bool IsToBeRelayout() const
    {
        return RELAYOUT_ACTION_NONE != m_nRelayoutAction;
    };

protected:
    inline void SetRedrawRelayoutContent()
    {
        m_nRelayoutAction |= RELAYOUT_ACTION_REDRAW_CONTENT;
    }

private:
    inline bool IsForceRelayout() const
    {
        return (RELAYOUT_ACTION_FORCE == (RELAYOUT_ACTION_FORCE & m_nRelayoutAction));
    }

    inline bool IsBoundRelayout() const
    {
        return (RELAYOUT_ACTION_BOUND == (RELAYOUT_ACTION_BOUND & m_nRelayoutAction));
    }

    inline bool IsRedrawRelayoutContent()
    {
        return (RELAYOUT_ACTION_REDRAW_CONTENT == (RELAYOUT_ACTION_REDRAW_CONTENT & m_nRelayoutAction));
    }

    inline void ResetRelayoutAction()
    {
        m_nRelayoutAction = RELAYOUT_ACTION_NONE;
    }

    inline void ResetForceRelayout()
    {
        m_nRelayoutAction &= (~RELAYOUT_ACTION_FORCE_MASK);
    }

    inline void ResetRangeRelayout()
    {
        m_nRelayoutAction &= (~RELAYOUT_ACTION_RANGE_MASK);
    }

    inline void ResetRedrawRelayoutContent()
    {
        m_nRelayoutAction &= (~RELAYOUT_ACTION_REDRAW_CONTENT);
    }

    inline bool IsInRelayoutList() const
    {
        return (VIEW_ASYNC_LIST_TYPE_RELAYOUT == (VIEW_ASYNC_LIST_TYPE_RELAYOUT & m_nAsyncListType));
    }

    inline void ResetInRelayoutList()
    {
        m_nAsyncListType &= (~VIEW_ASYNC_LIST_TYPE_RELAYOUT);
    }

public:// content changed or profile changed
    virtual void onContentChanged();
    bool relaoutWrapContentParent();
    int measureWidthWithBound(int widthBound);
    int measureHeightWithBound(int heightBound);

    void LayoutInBoundRect(const chRect& rectBound);
public:
    virtual bool onPaintBackground(xPainter& p);
    virtual bool onPaintContent(xPainter& p);
#if IF_VIEW_MASK
    virtual bool onPaintMask(xPainter& p);
#endif

    virtual bool onRectChanged(const chRect& rectOld);
    virtual void onClick();

    void PaintBackground(xPainter& p, const chRect& rcBG);

public:
    static xView* LoadView(const yl::string& strFielName);
    static xPixmap LoadPixmap(const yl::string& strFielName);

    static int GetFocusReasonByKey(int nKey);

public:
    void setLayoutInParent(LayoutParamsInParent* pLayoutInParent);
protected:
    LayoutParamsInParent* getLayoutInParent();

    bool releaseFocusInWindow();
    void freeLayoutInParent();
    void deleteChildrenViews();
#if IF_VIEW_RENDER
    void ReleaseRenderer();
#endif

private:
    inline bool IsInRedrawList() const
    {
        return (VIEW_ASYNC_LIST_TYPE_REDRAW == (VIEW_ASYNC_LIST_TYPE_REDRAW & m_nAsyncListType));
    }

    inline void ResetInRedrawList()
    {
        m_nAsyncListType &= (~VIEW_ASYNC_LIST_TYPE_REDRAW);
    }

    inline bool IsInDeleteList() const
    {
        return (VIEW_ASYNC_LIST_TYPE_DELETE == (VIEW_ASYNC_LIST_TYPE_DELETE & m_nAsyncListType));
    }

    inline void ResetInDeleteList()
    {
        m_nAsyncListType &= (~VIEW_ASYNC_LIST_TYPE_DELETE);
    }

public:
    xView();
    virtual ~xView();
public:
    enum
    {
        invisible = 0,
        visible = 1,
        gone = 2,

        bool_auto = -1,
        bool_false = 0,
        bool_true = 1,
    };
public: // profile
#if IF_XWINDOW_DUMP_EXPORT
    int                     m_nClassID;
#endif
    yl::string              m_strId;
    int                     focusable;                          // effect this view only
    bool                    m_bFocusPresent;                    // show the focus status for self or children
    bool                    clickable;
    bool                    overlap;
#if IF_MULTI_TOUCH_ENABLE
    bool                    touchable;
#endif
#if IF_VIEW_MASK
    bool                    mask;
#endif
    bool                    disable;                            // effect this view and it's children
    int                     visibility;                         // visible=0, invisible=1, gone=2
    xBrush                  background;
public:
    xSignal                 signalClicked;
    xSignal                 signalAppear;
    xSignal                 signalClosed;
public:
    chRect                  m_rectLayout;
    LayoutParamsInParent*   m_pLayoutInParent;
    dataWindowObject*       m_pWindow;
    ViewPropertys           m_mapProperty;
    ViewPropertys*          m_pExternAttributes;
    xStyle*                 m_pStyle;                   // 1 style object service for n-views
    xViewHandler*           m_pHandler;                 // 1 view handler service for 1-view
#if IF_VIEW_RENDER
    xViewRendererPtr        m_pRenderer;
#endif
    UINT                    m_bClosed: 1;
    UINT                    m_bAppear: 1;
    UINT                    m_bDisable: 1;              // m_bDisable:status disable:attribute
private:
    UINT                    m_nRelayoutAction;
    UINT                    m_nAsyncListType;
};

typedef xView       xRootView;      // view that m_pParent == NULL
typedef xView       xWindow;        // view that m_pParent == NULL && m_pWindow != NULL

class XWINDOW_EXPORT xViewGroup : public xView
{
    friend class xWorkStation;
public:
    void loadContent(const yl::string& strFileName);

    void replaceChildView(const yl::string& id, xView* newChild);
    void addChildView(xView* view);
    void insertChildView(int iIndex, xView* view);
    void popupChildView(xView* view);
    void closeAllChildren();

    void moveChildToTop(xView* pChild);
    void moveChildToBottom(xView* pChild);

public:
    virtual void onChildrenLayoutChanged();
    virtual void onLoadChildElement(xml_node& nodeChild);
public:
    virtual LayoutParamsInParent* allocLayout()
    {
        return NULL;
    }
    virtual bool checkLayout(LayoutParamsInParent* layout)
    {
        chASSERT(false);
        return false;
    }
    virtual void loadAttributesForContentView(xView* pChild, xml_node& node)        { }

protected:
    void OnChildAppend(xView* pView);

public:
    xViewGroup();
    virtual ~xViewGroup();
};

#if IF_IMAGE_VIEW_ENABLE
//////////////////////////////////////////////////////////////////////////
class XWINDOW_EXPORT xImageView : public xView
{
public:
#if IF_IMAGE_VIEW_KEEP_PIXMAP_RATIO
    bool setPixmap(const yl::string& file, bool bKeepRatio = false);
    bool setPixmap(const xPixmap& pixmap, bool bKeepRatio = false);
#else
    bool setPixmap(const yl::string& file);
    bool setPixmap(const xPixmap& pixmap);
#endif

#if IF_INVERT_EFFECT
    void setInvert(bool bInvert = true);
#endif

public:
    virtual void showValue(const yl::string&);
    virtual void loadAttributes(xml_node& node);
private:
    virtual int wrapContentWidth();
    virtual int wrapContentHeight();
protected:
    virtual bool onPaintContent(xPainter&);
#if IF_IMAGE_VIEW_KEEP_PIXMAP_RATIO
    bool GetRatioPixmap(const xPixmap& pmSrc,
                        const chRect& rcDst, xPixmap& pmResult);
#endif
public:
    xImageView();

#if IF_IMAGE_VIEW_BORDER
    inline void SetBorderWidth(int nWidth)
    {
        m_nBorderWidth = nWidth;
    }

    inline void SetBorderColor(const xColor& clrBorder)
    {
        m_clrBorder = clrBorder;
    }
#endif

public:
#if IF_INVERT_EFFECT
    int         invert;
#endif
#if IF_XWINDOW_DUMP_EXPORT
    yl::string  m_strSrc;
#endif
    xPixmap     src;
#if IF_IMAGE_VIEW_BORDER
    int         m_nBorderWidth;
    xColor      m_clrBorder;
#endif
#if IF_IMAGE_VIEW_SUPPORT_GRAVITY
    int gravity;                                // Specifies how an object should position its content, on both the X and Y axes, within its own bounds.
#endif
};
#endif

//////////////////////////////////////////////////////////////////////////

class CActiveFontText;
typedef YLVector<int> VEC_TEXTVIEW_INT;
class XWINDOW_EXPORT xTextView : public xView
    , public xTextTranslate
    , public xSlotHandler
{
    DECLARE_VIEW_DUMP_CLASS(xTextView)

public:
    virtual void SetInitText(const yl::string& strText);
    virtual void SetText(const yl::string& src);
    virtual void SetTranText(const yl::string& strText);
    virtual void setWText(const yl::wstring& wText);
    virtual void SetTextColor(const xColor& objColor);
    virtual void SetWrapLimited(int nWrapLimited);
    virtual void SetWrapLimitedHeight(int nWrapLimitedHeight);

    inline int GetWrapLimitedHeight()
    {
        return wrapLimitedHeight;
    }

    void SetFont(const xFont& font);
    void SetFontPointSize(int nPointSize);
    void SetFontBold(bool bBold);
    void SetFontFamily(const yl::string& strFamily);

    inline void SetAlign(int nAlign)
    {
        align = nAlign;
    }

    inline void SetGravity(int nGravity)
    {
        gravity = nGravity;
    }

    inline const yl::string& GetText() const
    {
        return m_strText;
    }

    inline const xFont& GetFont() const
    {
        return font;
    }

    inline const xColor& GetTextColor() const
    {
        return textColor;
    }

    inline int GetAlign() const
    {
        return align;
    }

    inline int GetGravity() const
    {
        return gravity;
    }

public:
    virtual void showValue(const yl::string&);
    virtual void loadAttributes(xml_node& node);
    virtual int wrapContentWidth();
    chSize& cacheTextSize();
    bool clearCacheTextSize();

    void DrawText(xPainter& p, const xColor& objColor);
    void DrawText(xPainter& p, const chRect& rcContent, const xColor& objColor);

public:
    xTextView();
    virtual ~xTextView();

    static void InitOverLengthEffect();
    virtual bool SetOverLengthStyle(int nType);

    bool EnableScroll(bool bEnabled);

    void SetScrollDirection(int nScrollDirection);

    virtual void SetMultiLineEnable(bool bMultiLine);

    inline bool GetMultiLineEnable()
    {
        return m_bMultiLineEnable;
    }

    inline void EnableNewLineByWord(bool bEnable)
    {
        m_bNewLineByWord = bEnable;
    }

    inline bool IsNewLineByWord()
    {
        return m_bNewLineByWord;
    }

    void SetTextRect(const chRect& rcText);

    inline const chRect& GetTextRect() const
    {
        return m_rcText;
    }

    inline bool IsOverLength()
    {
        return (m_rcText.Width() < cacheTextSize().cx);
    }

    inline bool IsOverHeight()
    {
        return (m_rcText.Height() < cacheTextSize().cy);
    }

#if IF_TEXTVIEW_SUPPORT_HIGH_LIGHT_TEXT
    inline void setHighlightKeyword(const yl::string& strKey)
    {
        m_strHighlightKeyword = strKey;
    }

    static xColor& GetTextHighLightColor()
    {
        return s_clrHighlightText;
    }
    static void SetTextHightLightColor(const xColor& objColor);
#endif
    int TextLength();

protected:
    virtual chRect GetViewPort();

    virtual bool onPaintContent(xPainter& p);

    virtual int wrapContentHeight();

    virtual bool onRectChanged(const chRect& rectOld);

public:
    yl::wstring GetAutoReturnText(const yl::wstring& wstr);
    virtual yl::string GetStringToPaint();

    inline bool IsContainRTLChar() const
    {
        return m_bContainRTLChar;
    }

protected:
    yl::wstring GetMultiLineString(const yl::wstring& wstr);

    yl::wstring GetEllipsisText(const yl::wstring& wstr);

    yl::wstring GetEllipsisTextByFont(const yl::wstring& wstr, const xFont& font);

    yl::wstring& CacheWText();

    void slotOnTextChange(xSignal* sender, WPARAM wParam, LPARAM lParam, int /*nDataBytes*/, LPVOID /*pData*/);

    int GetNumOfAutoReturnAdd(int iStPos, int iEnPos);

    void OnViewAppear(xSignal* sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);

    COverLengthEffectBase* FindEffectByType(int nType);
    COverLengthEffectBase* GetEffectByType(int nType);

    void OnStartOverLengthEffect();

    int GetOverLengthEffectAlignment(int nNormalAlignment, int nEffectAlignment);

    virtual void OnTextChange();
    virtual void OnDirectionChange();

#if IF_TEXTVIEW_SUPPORT_ACTIVE_FONT
    void SetFontFitSize();
#endif

    void DeleteOverLengthEffect();

public:
    chSize CachePaintTextSize();

    void SetScrollEffectInfoByType(int eType, int nScrollTickTime, int nScrollShift, int nScrollRoundDuration, int nScrollLineDuration);

    virtual void OnScrollFinish() {}

public:
    enum
    {
        WRAP_LIMITED_DEFAULT    = -1,
    };
protected:
    yl::string          m_strText;
    xColor              textColor;
    int                 gravity;
    int                 align;
    int                 wrapLimitedHeight;          //超长情况下的高度限制
    int                 wrapLimited;                // wrap content width limited
    xFont               font;
    bool                m_bMultiLineEnable; //是否多行文本
    bool                m_bNewLineByWord;
    bool                m_bContainRTLChar;
public:
    xSignal         signalTextChanged;
private:
    chSize              m_cacheTextSize;
protected:
    int    overlength_style;

    yl::wstring        m_strCachePaintString;
    yl::wstring        m_strCacheWText;         //缓存的宽字符
    chRect             m_rcText;
    chSize             m_cachePaintTextSize;

    VEC_TEXTVIEW_INT        m_vecAutoReturnLocation;    // 自动换行加'\n'位置（代码自动加的，不算在字符总数中）
    VEC_TEXTVIEW_INT        m_vecLineBreakLocation;     // 字符串中的'\n'位置
#if IF_TEXTVIEW_SUPPORT_HIGH_LIGHT_TEXT
    yl::string          m_strHighlightKeyword;
    static xColor       s_clrHighlightText;
#endif
#if IF_TEXTVIEW_SUPPORT_ACTIVE_FONT
    CActiveFontText* m_pActiveFont;
#endif

    COverLengthEffectBase* m_pOverLengthEffect;
    YLVector<COverLengthEffectBase*> m_vecOverLengthEffect;

};

//////////////////////////////////////////////////////////////////////////
class XWINDOW_EXPORT xButton : public xTextView
{
    DECLARE_VIEW_DUMP_CLASS(xButton)
public:
    void SetPixmap(const yl::string& strPixmap);

    inline const xPixmap& GetPixMap()
    {
        return src;
    }

#if IF_BUTTON_SUPPORT_DISABLE_STATE
    void SetDisableTextColor(const xColor& color);
    void SetDisablePixmap(const yl::string& strDisable);
#endif

#if IF_BUTTON_SUPPORT_FOCUS_STATE
    void SetFocusTextColor(const xColor& color);
    void SetFocusPixmap(const yl::string& strPixmap);
#endif

#if IF_INVERT_EFFECT
    void SetInvert(bool bInvert)
    {
        m_bInvert = bInvert;
    }
#endif

#if IF_BUTTON_SUPPORT_PRESS_STATE
    void SetPressTextColor(const xColor& color);
    void SetPressPixmap(const yl::string& strPixmap);

    inline bool IsPressed() const
    {
        return m_bPressed;
    }

    virtual bool onFocusEvent(bool bFocusIn);
    virtual bool onKeyPressedEvent(int key);
    virtual bool onKeyReleasedEvent(int key);
#endif

#if IF_MULTI_TOUCH_ENABLE
    virtual bool onTouchBeganEvent(xView* pAnchorView, LIST_RAW_TOUCH_POINT& lsTouchPoint);
    virtual bool onTouchMovedEvent(xView* pAnchorView, LIST_RAW_TOUCH_POINT& lsTouchPoint);
    virtual bool onTouchEndedEvent(xView* pAnchorView, LIST_RAW_TOUCH_POINT& lsTouchPoint);
    virtual bool onTouchCancelEvent(xView* pAnchorView, LIST_RAW_TOUCH_POINT& lsTouchPoint);
#endif

    void PaintPixmap(xPainter& p, const chRect& rcDst);
    void PaintBtnText(xPainter& p);

    virtual void Clear();

protected:
    virtual void loadAttributes(xml_node& node);
    virtual bool onPaintContent(xPainter& p);

private:
    virtual int wrapContentWidth();
    virtual int wrapContentHeight();

public:
    xButton();
    virtual ~xButton();

protected:
    xPixmap     src;                // 常态下图片
#if IF_XWINDOW_DUMP_EXPORT
    yl::string  m_strSrc;
#endif

#if IF_BUTTON_SUPPORT_DISABLE_STATE
    xColor      m_clrDisable;       // 不可用状态字体颜色
    xPixmap     m_pmDisable;        // 不可用状态图片
#endif

#if IF_BUTTON_SUPPORT_FOCUS_STATE
    xColor      m_clrFoucs;         // 焦点状态字体颜色
    xPixmap     m_pmFocus;          // 焦点状态图片
#endif

#if IF_BUTTON_SUPPORT_PRESS_STATE
    bool        m_bPressed;         // 是否被点击
    xColor      m_clrPress;         // 点击状态下字体颜色
    xPixmap     m_pmPress;          // 点击状态下图片
#endif

#if IF_INVERT_EFFECT
    bool        m_bInvert;
#endif

#if IF_MULTI_TOUCH_ENABLE
    int         m_nTouchPointID;
#endif
};

//////////////////////////////////////////////////////////////////////////
class XWINDOW_EXPORT xScrollBar : public xView
{
    DECLARE_VIEW_DUMP_CLASS(xScrollBar)

public:
    xScrollBar();
    virtual ~xScrollBar();

public:
    virtual bool onRectChanged(const chRect& rectOld);

    void SetRect(const chRect& rect);

    void SetScrollInfo(int nPageRange, int nRange, int nPos);
    void SetScrollPos(int nPos);

#if IF_SCROLL_BAR_SUPPORT_IMAGE
    void SetBlockPixmap(const yl::string& strBlock);

    bool GetBlockRect(chRect& rcBlock);

    const xPixmap& GetBlockPixmap() const
    {
        return m_pmBlock;
    }
#endif

private:
    virtual void loadAttributes(xml_node& node);

protected:
    virtual bool onPaintContent(xPainter& p);
#if IF_SCROLL_BAR_SUPPORT_IMAGE
    virtual bool onPaintBackground(xPainter& p);
#endif

    int GetMinBlockHeight();

    void AdjustBlock();

private:
#if IF_SCROLL_BAR_DENOTE_ITEM_POS
    int m_nPageCount;
    int m_nTotalCount;
#else
#if IF_SCROLL_BAR_DENOTE_CONTENT_POS
    int m_nScrollArea;
    int m_nScrollRange;
#endif
#endif
    int m_nPosition;
    int m_nBlockHeight;
    int m_nBlockPos;

#if IF_SCROLL_BAR_SUPPORT_IMAGE
    xPixmap m_pmBlock;
#endif
};

//////////////////////////////////////////////////////////////////////////
#if IF_PROGRESS_BAR_ENABLE
class XWINDOW_EXPORT xProgressBar : public xView
{
    DECLARE_VIEW_DUMP_CLASS(xProgressBar)

public:
    void SetRange(int nRange);
    void SetPosition(int nPosition, bool bNotify = false);

    inline int GetRange() const
    {
        return m_nRange;
    }

    inline int GetPosition() const
    {
        return m_nPosition;
    }

    //设置背景图片
    inline void SetBGPixmap(xPixmap& pmBG)
    {
        m_brushBG.setPixmap(pmBG);
    }

    //设置前景图片
    inline void SetBarPixmap(xPixmap& pmBar)
    {
        m_brushBar.setPixmap(pmBar);
    }

    //设置背景颜色
    inline void SetBGColor(xColor colBG)
    {
        m_brushBG.setColor(colBG);
    }

    //设置前景颜色
    inline void SetBarColor(xColor colBar)
    {
        m_brushBar.setColor(colBar);
    }

    //设置绘制的宽度
    inline void SetBarWidth(int nBarWidth)
    {
        m_nBarWidth = nBarWidth;
    }

    // 当前值改变
    void OnPositionChange(bool bAdd);

protected:
    //设置绘制起始位置
    void SetPaintPosition();
    //设置绘制时偏移量
    inline void SetPaintOffset(int iOffset)
    {
        m_nPaintOffset = iOffset;
    }

public:
    xProgressBar();
    ~xProgressBar();

protected:
    virtual void loadAttributes(xml_node& node);
    virtual bool onPaintContent(xPainter&);

public:
#if IF_PROGRESSBAR_WITH_VERTICAL
    void PaintVertical(xPainter& p, chRect& rcContent);
    void PaintVerticalBG(xPainter& p, chRect& rcContent);
    void PaintVerticalBar(xPainter& p, chRect& rcContent);
#endif
    void PaintHorizontal(xPainter& p, chRect& rcContent);

    void PaintHorizontalBG(xPainter& p, chRect& rcContent);
    void PaintHorizontalBar(xPainter& p, chRect& rcContent);

protected:
    int m_nRange;           // 进度条总值
    int m_nPosition;        // 进度条当前值
    int m_nPaintPosition;   //进度条开始绘制位置
    int m_nPaintOffset;     //绘制时的偏移量(图标的宽高,无图标时为0)
    int m_nBarWidth;
    xBrush m_brushBG;
    xBrush m_brushBar;

public:
    xSignal signalSliderValueChanged;

#if IF_PROGRESSBAR_WITH_VERTICAL
public:
    enum
    {
        PROGRESSBAR_VERTICAL = 0,
        PROGRESSBAR_HORIZONTAL = 1,
    };
public:
    int m_nOrientation;
#endif
};
#endif

//////////////////////////////////////////////////////////////////////////
#if IF_SLIDE_BAR_ENABLE
class XWINDOW_EXPORT xSlideBar : public xProgressBar
{
public:
    xSlideBar();
    ~xSlideBar();

#if IF_GESTURE_ENABLE
    virtual bool onGestureEvent(xGestureEvent* pEvent);
#endif

protected:
    virtual void loadAttributes(xml_node& node);
    virtual bool onPaintContent(xPainter&);

#if IF_GESTURE_ENABLE
    void onTouchPoint(const chPoint& ptTouch);
    void checkFocus();
#endif

public:
    void SetSlideBarIcon(xPixmap& pmIcon);
    void SetIconChange();
#if IF_GESTURE_ENABLE
    void EnableSlide(bool bEnable);
#endif

protected:
    xPixmap m_pmIcon;
};
#endif

//////////////////////////////////////////////////////////////////////////
#if IF_RADIO_BUTTON_ENABLE
class xRadioButton : public xView, public xTextTranslate
{
    DECLARE_VIEW_DUMP_CLASS(xRadioButton)

public:
    void SetChecked(bool bChecked);
    inline bool IsChecked()
    {
        return m_bChecked;
    }

    void SetIcon(const yl::string& strOn, const yl::string& strOff);
    void SetText(const yl::string& strText);
    inline const yl::string& GetText() const
    {
        return m_strText;
    }

    inline void SetTextColor(const xColor& objColor)
    {
        m_objTextColor = objColor;
    }

    inline void SetFont(const xFont& objFont)
    {
        m_objFont = objFont;
    }

    virtual void SetTranText(const yl::string& strText);

public:
    xRadioButton();
    virtual ~xRadioButton();

protected:
    virtual bool onPaintContent(xPainter&);
    virtual void loadAttributes(xml_node& node);

private:
    chRect m_rcText;
    xFont m_objFont;
    xColor m_objTextColor;
    yl::string m_strText;

    chRect m_rcIcon;
    yl::string m_strIconOn;
    yl::string m_strIconOff;

    bool m_bContainRTLChar;
    bool m_bChecked;
};
#endif

//////////////////////////////////////////////////////////////////////////
// Params for children in LinearLayout
class LinearLayoutParams : public LayoutParamsInParent
{
public:
    virtual void loadAttributes(xml_node& node);
public:
    LinearLayoutParams();

    static LinearLayoutParams* GetLayoutParams(LayoutParamsInParent* pLayout);
public:
    int layout_gravity;                     //  Defines how the child view should be positioned, on both the X and Y axes, within its enclosing layout.
    int layout_weight;

    enum
    {
        LINEAR_LAYOUT_PARAMS = LAYOUT_PARAMS_IN_PARENT_TYPE_BASE + 2,
    };
};

class XWINDOW_EXPORT xLinearLayout : public xViewGroup
{
    DECLARE_LAYOUT_PARAMS_IN_PARENT(LinearLayoutParams);
public:
    virtual void loadAttributes(xml_node& node);

protected:
    virtual int wrapContentWidth();
    virtual int wrapContentHeight();
    virtual void LayoutContentInRect(const chRect& rectContentBound);
public:
    xLinearLayout();

public:
    enum
    {
        orientation_vertical = 0,
        orientation_horizontal = 1,
    };
public:
    int gravity;                                // Specifies how an object should position its content, on both the X and Y axes, within its own bounds.
    int orientation;
};

//////////////////////////////////////////////////////////////////////////
class FrameLayoutParams : public LayoutParamsInParent
{
public:
    virtual void loadAttributes(xml_node& node);
public:
    FrameLayoutParams();

    static FrameLayoutParams* GetLayoutParams(LayoutParamsInParent* pLayout);
public:
    int layout_gravity;                     // Standard gravity constant that a child supplies to its parent.

    enum
    {
        FRAME_LAYOUT_PARAMS = LAYOUT_PARAMS_IN_PARENT_TYPE_BASE + 3,
    };
};

class XWINDOW_EXPORT xFrameLayout : public xViewGroup
{
    DECLARE_LAYOUT_PARAMS_IN_PARENT(FrameLayoutParams);
public:
    xFrameLayout();

protected:
    virtual int wrapContentWidth();
    virtual int wrapContentHeight();
    virtual void LayoutContentInRect(const chRect& rectContentBound);
};

//////////////////////////////////////////////////////////////////////////
#if IF_SCALE_LAYOUT_ENABLE
class ScaleLayoutParams : public LayoutParamsInParent
{
public:
    virtual void loadAttributes(xml_node& node);

public:
    ScaleLayoutParams();

    static ScaleLayoutParams* GetLayoutParams(LayoutParamsInParent* pLayout);
public:
    chRect      scale_rect;

    enum
    {
        SCALE_LAYOUT_PARAMS = (LAYOUT_PARAMS_IN_PARENT_TYPE_BASE + 4),
    };
};

class XWINDOW_EXPORT xScaleLayout : public xViewGroup
{
    DECLARE_LAYOUT_PARAMS_IN_PARENT(ScaleLayoutParams);
public:
    virtual void loadAttributes(xml_node& node);
protected:
    virtual void LayoutContentInRect(const chRect& rectContentBound);
public:
    xScaleLayout();
public:
    int scale_width;
    int scale_height;
};
#endif

//////////////////////////////////////////////////////////////////////////
#if IF_RECT_VIEW_ENABLE
class XWINDOW_EXPORT xRectView : public xViewGroup
{
    DECLARE_LAYOUT_PARAMS_IN_PARENT(LayoutParamsInParent);
public:
    enum corner_type
    {
        left_top         = 0x1,
        right_top        = 0x1 << 1,
        left_bottom      = 0x1 << 2,
        right_bottom     = 0x1 << 3,
        all              = 0xf,
    };
    int     rect_corner_size;
    int     rect_corner_type;
    xColor  rect_color;

public:
    xRectView();

    virtual void loadAttributes(xml_node& node);

protected:
    virtual bool onPaintContent(xPainter& painter);

    virtual void LayoutContentInRect(const chRect& rectFrame);
};
#endif

//////////////////////////////////////////////////////////////////////////
class XWINDOW_EXPORT RootLayoutParams : public LayoutParamsInParent
{
public:
    static void RelayoutRootView(xView* pRootView);
    static RootLayoutParams* GetLayoutParams(LayoutParamsInParent* pLayout);

    void RelayoutRootView();
public:
    virtual void loadAttributes(xml_node& node);
public:
    RootLayoutParams();
public:
    bool        activable;
    int         layout_gravity;
    chRect      logic_ltwh;

    enum
    {
        ROOT_LAYOUT_PARAMS = LAYOUT_PARAMS_IN_PARENT_TYPE_BASE + 1,
    };
};

#endif // __XWINDOW_H__
