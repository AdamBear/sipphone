#ifndef __X_WINDOW_MANAGER_H__
#define __X_WINDOW_MANAGER_H__

#include <yllist.h>
#include <ylstring.h>
#include <ylsmartptr.h>
#include "ETLLib/ETLLib.hpp"
#include "xWindow.h"
#include <xwindow/xscreendefine.h>
#include <xwindow/xwindowdefine.h>
#include "xwindow-kernel/include/xkernel.h"
#include "xwindow-kernel/include/xthreadtimer.h"
#if IF_MULTI_TOUCH_ENABLE
#include "touchdriver/include/touchdrivercommon.h"
#endif


USING_XKERNEL_NAMESPACE
// Station ： 同一套输入/输出设备为一个Station，可能是多进程共用一套输入/输出设备
// Window ：没有Parent位置可以在整个屏幕内自由移动的顶层窗口
// View ：位置大小受Parent限制的窗口

#define KEY_REPEAT_MASK 0x80000000
#define SET_KEY_REPEAT(key) (key | KEY_REPEAT_MASK)
#define GET_KEY_VALUE(key)  (key & 0x0FFFFFFF)

#define KEY_LONG_PRESS_TRIGGER_TIME 2000
#if IF_KEY_REPEAT
#define KEY_REPEAT_TRIGGER_TIME 1000
#define KEY_REPEAT_TIME 100
#endif

enum StationEventE
{
    STATION_EVENT_FIRST                     = TM_WINDOW_BEGIN,
    STATION_WINDOW_EVENT,

    TASK_FETCH_KEY_EVENT                    = TM_WINDOW_BEGIN + 0x100,          // wParam:key   lParam:bPressed
    STATION_RESIZE_EVENT,
    STATION_SYNC_HOVER_BY_MOUSE,
    STATION_RESET_MOUSE_BUTTONS,
    STATION_INPUT_METHOD_EVENT,

    WINDOW_MOUSE_BUTTON_DBCLICK,
    WINDOW_MOUSE_WHEEL,
};

enum STATION_WINDOW_EVENT_TYPE
{
    STATION_WINDOW_EVENT_NONE               = 0,
    STATION_WINDOW_EVENT_LAYOUT,                    // 窗口重新布局
    STATION_WINDOW_EVENT_PEND_TILL_LAYOUT,          // 异步等待Layout完成
    STATION_WINDOW_EVENT_ACTIVE,                    // 活动窗口变化
    STATION_WINDOW_EVENT_FOCUS,                     // 窗口焦点变化
    STATION_WINDOW_EVENT_PEND_TILL_FOCUS,           // 异步等待焦点变化
#if IF_VIEW_ASYNC_CLICK
    STATION_WINDOW_EVENT_CLICK,                     // 异步点击
#endif
    STATION_WINDOW_EVENT_FLUSH_REGION,              // 重绘区域
#if IF_KEY_REPEAT
    STATION_WINDOW_EVENT_REPEAT_KEY_PRESS,          // 重复发出按键按下事件
#endif
    STATION_WINDOW_EVENT_DELETE_VIEW,
};

typedef struct _ST_STATION_WINDOW_EVENT
{
    explicit _ST_STATION_WINDOW_EVENT(int nEventType = STATION_WINDOW_EVENT_NONE,
                                      WPARAM wParam = 0,
                                      LPARAM lParam = 0)
        : m_nEventType(nEventType)
        , m_wParam(wParam)
        , m_lParam(lParam)
    {
    }

    int         m_nEventType;
    WPARAM      m_wParam;
    LPARAM      m_lParam;
} ST_STATION_WINDOW_EVENT;

typedef YLList<ST_STATION_WINDOW_EVENT> LIST_STATION_WINDOW_EVENT;

class dataWindowObject
{
#if IF_XKERNEL_THREAD_CHECK
public:
    THREAD_ID getHostThreadId();
#endif

public:
    dataWindowObject();
public:
    UINT                magic;
#if IF_XKERNEL_THREAD_CHECK
    THREAD_ID           hostThreadID;
#endif
    xView*              hostView;
    xView*              focusView;
    chRect              rect;
    bool                visible;
    bool                overlap;
    bool                activable;
    bool                m_bWaitSetActive;
    BYTE                topLevel;       // 5:system topmost; 10:user topmost window 20:normal window
    BYTE                opacity;
};

struct ST_DATA_PROFILE
{
public:
    ST_DATA_PROFILE()
        : m_bOverlapMode(true)
#if IF_XWINDOW_STATION_ALPHA
        , m_nGlobalAlpha(0xff)
#endif
    {
    }

public:
    // section : profile
    chSize              m_sizeLogic;
    xBrush              m_brushBackground;
    xColor              m_clrViewDefaultBG;
    bool                m_bOverlapMode;
#if IF_XWINDOW_STATION_ALPHA
    int                 m_nGlobalAlpha;
#endif
};
// sort by z-order, first is bottom
typedef YLList<dataWindowObject*> LIST_WINDOW_OBJECT;

class ViewOrWindow
{
public:
    xView* getViewObject() const
    {
        return m_bWindowObject ? NULL : static_cast<xView*>(m_pViewOrWindow);
    }
    dataWindowObject* getWindowObject() const
    {
        return m_bWindowObject ? (dataWindowObject*)m_pViewOrWindow : NULL;
    }
    bool hasData() const
    {
        return m_pViewOrWindow != NULL;
    }
    bool isWindowObject() const
    {
        return m_bWindowObject;
    }
    bool isViewObject() const
    {
        return !m_bWindowObject;
    }
    bool operator == (const ViewOrWindow& other) const;
public:
    ViewOrWindow() : m_bWindowObject(false), m_pViewOrWindow(NULL) {}
    explicit ViewOrWindow(xView* pView) : m_bWindowObject(false), m_pViewOrWindow(pView) {}
    explicit ViewOrWindow(dataWindowObject* pWindow);
private:
    bool        m_bWindowObject;
    xView*      m_pViewOrWindow;
};

// 翻译回调定义
typedef bool (*FUN_SETUP_TEXT_TRAN)(xTextTranslate* pTextObject, const yl::string& strText);
typedef void (*FUN_RELEASE_TEXT_TRAN)(xTextTranslate* pTextObject);
typedef yl::string(*FUN_LANG)(const yl::string& strText);
typedef yl::string(*FUN_TRAN)(const yl::string& strText);

// RTL接口回调
typedef bool(*FUN_IS_RTL_TEXT)(const yl::string& strRTL);
typedef bool(*FUN_IS_RTL_WTEXT)(const yl::wstring& strRTL);
typedef bool(*FUN_IS_RTL_CHAR)(WCHAR wChar);
typedef bool(*FUN_GET_RTL_SHOW_TEXT)(const yl::string& strText, yl::string& strShowText);
typedef void(*FUN_GET_RTL_SHOW_WTEXT)(yl::wstring& strShowText);

typedef xPixmap(*FunGetImage)(const yl::string&);

#if IF_MULTI_TOUCH_ENABLE
typedef bool(*TOUCH_HOOK_PROC)(CTouchPoints* pTouchPoints);
#endif

//////////////////////////////////////////////////////////////////////////
typedef struct
{
    ViewOrWindow vw;
    xRegion region;
} ST_REDRAW_REGION;
typedef YLSmartPtr<ST_REDRAW_REGION> ST_REDRAW_REGION_PTR;

typedef YLList<ST_REDRAW_REGION_PTR> typeRedrawRegionList;
class XWINDOW_EXPORT xWorkStation : public xThreadLocal, public xTimerHandler
{
    friend class xXmlLoader;
    friend class xView;
    friend class xViewGroup;

    typedef YLList<xView*>                          typeViewList;
#if IF_VIEW_MASK
    typedef YLList<ST_REDRAW_REGION_PTR>            typeMaskRegionStack;
#endif

public:
    static xWorkStation& getCurrentStation();
    static bool dispatchDirectionalViewEvent(xView* pTargetView, xViewEvent* pEvent);
    static bool dispatchNonDirectionalViewKeyEvent(xView* pTargetView, xViewEvent* pEvent);
    static bool IsViewInList(typeViewList& listView, xView* pView);
    static void resetChildrenLayout(xView* parent);
    static void resetChildrenRelayout(xView* parent);

    //////////////////////////////////////////////////////////////////////////
    // section : station open/set/close/debug
public:
    // 设置字体库根目录（需要在OpenStation接口前调用）
    void SetFontPath(const yl::string& strFontPath);
    bool OpenStation(const yl::string& strStackName, int nStackBytes);
    void CloseStation();

    bool CreateStationScreen();
    void SetScreen(xScreen* pScreen);
    bool InitScreen(const char* pszConfigFile);
    bool UnInitScreen();

    void setStationLogicSize(int xPoints, int yPoints);
    void setStationOverlapMode(bool bOverlap);
    void setStationBackground(const xBrush& brush);
    void setStationViewBGColor(const xColor& col);
#if IF_XWINDOW_STATION_ALPHA
    int& getStationAlpha();
    void setStationAlpha(int nAlpha);
#endif

    void setDrawableRootPath(const yl::string& strRootPath);
    void setLayoutRootPath(const yl::string& strRootPath);

    const yl::string& getDrawableRootPath() const;

    const ST_DATA_PROFILE& profile();
#if IF_DEBUG_VIEW_MGR
    void dump();
#endif

    const chRect& getScreenRect();
#if IF_SCREEN_SUPPORT_DEV_PIXEL_SCALE
    int getScreenDevPixelX(int nX);
    int getScreenDevPixelY(int nY);
#endif

    int width();
    int height();

#if IF_XWINDOW_DUMP_EXPORT
    bool DumpAllVisibleWindow(xml_document& doc);
    bool ScreenCapture(xPixmap& pmScreen);
    bool InputText(yl::string& strText);
#endif
private:
    ST_DATA_PROFILE     m_stProfile;

public:
    //////////////////////////////////////////////////////////////////////////
    // section : station message
    virtual void dispatchKeypadEvent(int nKey, bool bPress);

    void PostPendTillLayout(WPARAM wParam = 0, LPARAM lParam = 0);
    void PostPendTillFocus(WPARAM wParam = 0, LPARAM lParam = 0);

private:
    void PostWindowEvent(int nEventType, WPARAM wParam, LPARAM lParam);

    //////////////////////////////////////////////////////////////////////////
    // section : resource loader
public:
    xView* LoadView(const yl::string& strFileName);
    xView* LoadView(xml_node& node);
    bool LoadLayout(const yl::string& strFileName, xml_document& doc);
    xPixmap LoadPixmap(const yl::string& strFielName);

    yl::string getDrawableFile(const yl::string& strFileName);
    yl::string getLayoutFile(const yl::string& strFileName);

protected:
    bool LoadLayoutDirect(const yl::string& strFullPath, xml_document& doc);
#if IF_SUPPORT_MACRO_LAYOUT_FILE
    bool LoadLayoutWithMacros(const yl::string& strFullPath, MAP_STRING& mapMacro, xml_document& doc);
#endif
    bool ExpandNode(xml_node& node);
    bool ExpandEmbedNode(xml_node& node);

public:
    // set create screen function
    void SetFunCreateScreen(FUN_CREATE_SCREEN pFunCreateScreen);
    void SetFunReleaseScreen(FUN_RELEASE_SCREEN pFunReleaseScreen);
    // 设置获取图片回调函数
    void SetGetImageFunction(FunGetImage pGetImage);
    // 获取图片
    bool GetImageByPath(const yl::string& strFile, xPixmap& value);

    // 翻译回调
    void SetFunSetupTextTran(FUN_SETUP_TEXT_TRAN pFunSetupTextTran);
    void SetFunReleaseTextTran(FUN_RELEASE_TEXT_TRAN pFunReleaseTextTran);
    void SetFunLang(FUN_LANG pFunLang);
    void SetFunTran(FUN_TRAN pFunTran);
    void SetFunIsRTLText(FUN_IS_RTL_TEXT pFunIsRTLText);
    void SetFunIsRTLWText(FUN_IS_RTL_WTEXT pFunIsRTLWText);
    void SetFunIsRTLChar(FUN_IS_RTL_CHAR pFunIsRTLChar);
    void SetFunGetRTLShowText(FUN_GET_RTL_SHOW_TEXT pFunGetRTLShowText);
    void SetFunGetRTLShowWText(FUN_GET_RTL_SHOW_WTEXT pFunGetRTLShowWText);

    bool SetupTextTran(xTextTranslate* pTextObject, const yl::string& strText);
    void ReleaseTextTran(xTextTranslate* pTextObject);
    yl::string Lang(const yl::string& strText);
    yl::string Tran(const yl::string& strText);
    bool IsRTLText(const yl::string& strText);
    bool IsRTLWText(const yl::wstring& strText);
    bool IsRTLChar(WCHAR wChar);
    bool GetRTLShowText(const yl::string& strText, yl::string& strShowText);
    bool GetRTLShowWText(yl::wstring& strShowText);

    void SetKeyLongPressTime(int nTime = KEY_LONG_PRESS_TRIGGER_TIME);
    void SetLongPressTimeByKey(int nKey, int nTime = KEY_LONG_PRESS_TRIGGER_TIME);

protected:
    BOOL onProcessFetchKeyEvent(msgObject&);
    virtual void onTaskKeyEvent(int key, bool bPressed);
#if IF_KEY_REPEAT
    void ProcessKeyPress(int nKey, bool bRepeat);
#else
    void ProcessKeyPress(int nKey);
#endif
private:
    void OnKeyLongPressed(xThreadTimer* timer, LPVOID pExtraData);
#if IF_KEY_REPEAT
    void OnKeyRepeat(xThreadTimer* timer, LPVOID pExtraData);
#endif
public:
    xSignal             signalKeyEvent;
    xSignal             signalLongKeyEvent;

    //////////////////////////////////////////////////////////////////////////
    // section : active and focus
protected:
    BOOL onSyncActiveWindow(msgObject& msg);
    BOOL onSyncFocusView(msgObject& msg);
    virtual void onWindowActive(xWindow* pWindow, bool bActive);
    virtual void onViewFocusEvent(xView* pView, bool bFocusIn);

public:
    inline bool isFocusView_helper(const xView* pView)
    {
        return pView == getFocusView();
    }
    inline bool isFocusedView_helper(const xView* pView)
    {
        return pView == m_pCacheFocusView;
    }

    static xView* getStrongFocusChild(xView* pView, bool bForward = true);
    static xView* getFocusableChildView(xView* pView);
    static xView* getFocusPresentView(xView* pView);
    static bool checkViewFocusble(const xView* pView);

    void setActiveWindowObject(dataWindowObject* pWindowObject);
    dataWindowObject* getActiveWindowObject();
    dataWindowObject* getNextActivableWindow(dataWindowObject* pWindow);
    dataWindowObject* getNextWindowObject(dataWindowObject* pWindow);
    dataWindowObject* getPreWindowObject(dataWindowObject* pWindow, bool bOnlyVisible);
    dataWindowObject* getTopWindowByLevel(int nTopLevel);

    xWindow* getActiveWindow();
    xView* getFocusView();
    void setFocus_helper(xView* pView);
#if IF_MOVE_FOCUS_BY_KEY
    bool MoveWindowFocus(xWindow* pWindow, xView* pFocusView, int key);
    bool SwitchWindowFocus(xWindow* pWindow, xView* pFocusView, int nFocusReason);
    void SetAutoSwitchFocus(bool bAutoSwitch);
    void MoveFocusByKey(int key);
#endif

    void releaseFocusOnViewDisappear(xView* pView);

    void UpdateFocusWidget(xView* pView);

private:
    bool focusNextView(xView* pView, int nFocusReason);
    bool focusPrevView(xView* pView, int nFocusReason);
public:
    xSignal             signalActiveEvent;
    xSignal             signalFocusEvent;
    xSignal             signalPendTillLayout;
    xSignal             signalPendTillFocus;
private:
    // 当前有效的活动窗口
    dataWindowObject*       m_pActiveWindowObject;
    // 缓存的活动窗口，设置活动窗口异步处理后，会同步为m_pActiveWindowObject的hostView
    xWindow*                m_pCacheActiveWindow;
    // 缓存的焦点控件，设置焦点控件异步处理后，会同步为m_pActiveWindowObject的focusView
    xView*                  m_pCacheFocusView;
#if IF_MOVE_FOCUS_BY_KEY
    bool                    m_bAutoSwitchFocus;
#endif

    //////////////////////////////////////////////////////////////////////////
    // section : cache view list
public:
    void recursiveViewClosed(xView* pView);
    void recursiveViewAppear(xView* pView, bool bAppear);
    void recursiveViewDisable(xView* pView, bool bDisable);

    virtual void onViewAppear(xView* pView, bool bAppear);
    virtual void onViewClosed(xView* pView);
    void deleteRedrawRegion(xView* pView);

    //////////////////////////////////////////////////////////////////////////
    // section : async layout
public:
    void LayoutViewInBoundRect(xView* pView, const chRect& rect);
    bool asyncRelayoutViewContent(xView* pView);
    bool asyncRelayoutViewBound(xView* pView);

    void relayoutAllWindow();
private:
    bool asyncRelayoutView(xView* pView, bool bForceRelayout);
    void RelayoutViewGroupContent(xViewGroup* pViewGroup);
private:
    BOOL onRelayoutView(msgObject&);
private:
    typeViewList            m_listRelayoutViews;
    bool                    m_bExposedRegionChanged;            // 因布局调整导致的屏幕可见View的区域发生变化
    xView*                  m_pLayoutingView;
    xRegion                 m_objRegionOfLayoutting;
    bool                    m_bLoopInRelayoutViews;
public:
    xSignal             signalLayoutChanged;

    //////////////////////////////////////////////////////////////////////////
    // section : async redraw
public:
    void updateBackground();

    void update_helper(xView* pView, const chRect& rect);
    void update_helper(xView* pView);
    void viewRectChanged(xView* pView, const chRect& oldRect, const chRect& newRect);
    void exposeView(xView* pView, const chRect& rect, bool bRedrawViewOnly);
    void exposeView(xView* pView, bool bRedrawViewOnly);
    void unexposeView(xView* pView);

private:
    void updateRegionOverView(xView* pView, xRegion& region);
    bool upwardUpdateFrom(ViewOrWindow vw, xRegion& region, bool calcuRegionOnly);
    bool downwardUpdateFrom(ViewOrWindow vw, xRegion& region);

    void pushRedrawRegion(const ViewOrWindow& vw, const xRegion& region, const chRect& rcClip);
    void queuedRedrwRegion(const ViewOrWindow& vw, const xRegion& region, const chRect& rcClip);
#if IF_VIEW_MASK
private:
    void flushMaskRegionUnderView(xView* pView);
    bool pushMaskRegionOverView(ST_REDRAW_REGION_PTR pRedrawRegion, xRegion& region);
    void flushMaskRegions();
#endif
private:
    BOOL onFlushRedrawRegion(msgObject&);
    void paintStationBackground(const xRegion& region);
    bool paintView(xView* pView, const xRegion& region);
#if IF_VIEW_MASK
    bool paintMaskView(xView* pView, const xRegion& region);
#endif
private:
    UINT                    m_uRedrawMagic;
    typeRedrawRegionList    m_listRedrawRegions;
#if IF_VIEW_MASK
    typeMaskRegionStack     m_stackMaskRegions;
#endif

    //////////////////////////////////////////////////////////////////////////
    // section : view tree helper
public:
    bool bringWindowToTop_helper(xWindow* pWindow);
public:
    static xView* getPrevVisibleView(xView* pView);
    static xView* getTopMostVisibleChild(xView* pView);
    static bool containChildView(const xView* pParent, const xView* pChild);
    ViewOrWindow getNextViewOrWindow(const ViewOrWindow& vw, bool bSkipChildern);
    ViewOrWindow getPrevVisibleViewOrWindow(const ViewOrWindow& vw);
private:
    bool AOnTopOfB(const ViewOrWindow& A, const ViewOrWindow& B);

    //////////////////////////////////////////////////////////////////////////
    // section : view helper
public:
    void show_helper(xView* pView, int nShowCmd = xView::SHOW_CMD_DEFAULT);
    void hide_helper(xView* pView, bool bGone);
    void close_helper(xView* pView, bool bDeleteLater);

    dataWindowObject* createWindow(xView* pRootView, BYTE nTopMostLevel, bool bWaitSetActive = false);

    void pushWindowObject(dataWindowObject* pWindowObject);
    void popWindowObject(dataWindowObject* pWindowObject);

protected:
    dataWindowObject* allocWindowObject();
    void freeWindowObject(dataWindowObject* pWindowObject);

private:
    bool isLocalWindow(dataWindowObject* pWindow);
    chRect getVisibleRect(const xView* pView);

    void closeAllWindow();
public:
    BOOL onStationEvent(msgObject& msg);

private:
    void clearReDrawRegionByWindow(xWindow *pWindow);
    void onStationResized(int nOldWidth, int nOldHeight);
    BOOL onWindowEvent(msgObject& msg);
    BOOL onDeleteViews(msgObject& msg);
    //////////////////////////////////////////////////////////////////////////

#if IF_VIEW_ASYNC_CLICK
    //////////////////////////////////////////////////////////////////////////
    // section : async click
public:
    void AsyncClick(xView* pView);

protected:
    BOOL onSyncClick(msgObject& msg);
    //////////////////////////////////////////////////////////////////////////
#endif

    //////////////////////////////////////////////////////////////////////////
    // section : multi-touch
#if IF_MULTI_TOUCH_ENABLE
public:
    void SetTouchHookProc(TOUCH_HOOK_PROC pTouchHookProc);

    bool CheckViewActiveAndFocus(xView* pView, xView* pFocusView);
    BOOL onTouchEvent(msgObject&);

protected:
    typedef YLHashMap<int, xView*> MAP_TOUCH_CACHE_VIEW;
    typedef YLList<xTouchEvent*> LIST_TOUCH_EVENT;
    typedef YLList<int> LIST_TOUCH_POINT_ID;

    void dispatchTouchBeganEvent(const LIST_TOUCH_POINT& lsTouchPoint);
    void dispatchTouchEvent(int nPointState, int nEventType, const LIST_TOUCH_POINT& lsTouchPoint, LIST_TOUCH_POINT_ID& lsPointID);
    void dispatchTouchEvent(LIST_TOUCH_EVENT& lsTouchEvent);

    xView* getTouchAnchorViewByPos(int nX, int nY);
    xView* getTouchAnchorViewByID(int nPointID);
    xView* getTouchAnchorView(xView* pView, const chPoint& ptTouch);

    bool IsTouchPointOnView(int nX, int nY);

    xTouchEvent* allocTouchEvent(int nEventType, xView* pAnchorView);
    void recycleTouchEvent(xTouchEvent* pTouchEvent);
    xTouchEvent* getAnchorViewEvent(xView* pAnchorView, LIST_TOUCH_EVENT& lsTouchEvent);
    void clearTouchEventList(LIST_TOUCH_EVENT& lsTouchEvent);

    void ResetTouchCacheView(const LIST_TOUCH_POINT_ID& lsPointID);
    void ResetTouchCacheView(xView* pAnchorView);

    virtual bool OnDisengagedPoint(ST_TOUCH_POINT* pTouchPoint);

protected:
    TOUCH_HOOK_PROC         m_pFunTouchHookProc;
    MAP_TOUCH_CACHE_VIEW    m_mapCacheTouchView;
    LIST_TOUCH_EVENT        m_lsCacheTouchEvent;
#endif

private:
    LIST_WINDOW_OBJECT  m_lsWindowObject;
#if IF_XWINDOW_CACHE_WINDOW_OBJECT
    LIST_WINDOW_OBJECT  m_lsCacheWindowObject;
#endif
    LIST_STATION_WINDOW_EVENT m_listWindowEvent;

public:
    explicit xWorkStation(LPCSTR lpszThreadName);
    ~xWorkStation();

#if IF_USE_DOB_FONT && IF_USE_FREETYPE_LIB
    void SetUseDotLib(bool bUseDotLib);
    bool IsUseDotLib() const;
#endif

protected:
    xScreen*                m_pScreen;
    FUN_CREATE_SCREEN       m_pFunCreateScreen;
    FUN_RELEASE_SCREEN      m_pFunReleaseScreen;
    typeViewList            m_listDeleteViews;
    xThreadTimer            m_timerLongPressed;
#if IF_KEY_REPEAT
    xThreadTimer            m_timerKeyRepeat;
#endif
    yl::string              m_strDrawablePath;
    yl::string              m_strLayoutPath;
    FunGetImage             m_pFunGetImage;
    FUN_SETUP_TEXT_TRAN     m_pFunSetupTextTran;
    FUN_RELEASE_TEXT_TRAN   m_pFunReleaseTextTran;
    FUN_LANG                m_pFunLang;
    FUN_TRAN                m_pFunTran;
    FUN_IS_RTL_TEXT         m_pFunIsRTLText;
    FUN_IS_RTL_WTEXT        m_pFunIsRTLWText;
    FUN_IS_RTL_CHAR         m_pFunIsRTLChar;
    FUN_GET_RTL_SHOW_TEXT   m_pFunGetRTLShowText;
    FUN_GET_RTL_SHOW_WTEXT  m_pFunGetRTLShowWText;

#if IF_USE_DOB_FONT && IF_USE_FREETYPE_LIB
    bool                    m_bUseDotLib;
#endif

#if !IF_MULTI_XKERNEL_ENABLE
    static xWorkStation*    s_pWorkStation;
#endif

    int                     m_nKeyLongPressTimer;

    chRect                  m_rcScreen;

public:
    xSignal             signalCustomElement;
    xSignal             signalViewClosed;
public:
    xStyle*                 m_pStationStyle;
};

#define g_WorkStation       xWorkStation::getCurrentStation()

#endif // __X_WINDOW_MANAGER_H__
