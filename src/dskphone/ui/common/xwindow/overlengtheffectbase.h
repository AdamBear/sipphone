#ifndef _OVER_LENGTH_STYLE_BASE_H_
#define _OVER_LENGTH_STYLE_BASE_H_

#include "ylstringalgorithm.h"
#include "ylalgorithm.h"
#include "commonapi/stringhelperapi.h"
#include "ETLLib/ETLLib.hpp"
#include "xmlparser/pugixml.hpp"
#include "xmlparser/xmlparser.hpp"
#include "xwindow/xwindowcommon.h"
#include "xwindow-kernel/include/xthreadtimer.h"

enum OVERLENGTH_TYPE
{
    OVERLENGTH_TYPE_DIRECT_CUT = 0,     //不做处理，直接截断
    OVERLENGTH_TYPE_CUT_WITH_ELLIPSIS,  //使用省略号
    OVERLENGTH_TYPE_SCROLL,             //(default)滚动
#if IF_TEXTVIEW_SUPPORT_VERTICAL_SCROLL
    OVERLENGTH_TYPE_VSCROLL,            //垂直滚动(附加自动换行)
#endif
    OVERLENGTH_TYPE_CUSTOM,
};

enum SCROLL_DIRECTION
{
    SCROLL_DIRECTION_NONE       = 0,
    SCROLL_DIRECTION_LEFT       = 1,        // 向左滚动
    SCROLL_DIRECTION_RIGHT      = 2,        // 向右滚动
    SCROLL_DIRECTION_BOTH       = 3,        // 先向左，再向右滚动
};

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
class xTextView;
class xPainter;

class COverLengthEffectBase
{
public:
    explicit COverLengthEffectBase(int nType);
    virtual ~COverLengthEffectBase();

public:
    virtual bool OnStartEffect()
    {
        return false;
    }
    virtual void onEndEffect()
    {
        return;
    }
    virtual int GetGravity();
    virtual int GetAlign();
    virtual yl::wstring GetStringToPaint(const chRect& rcText, const yl::wstring& strText);
    void SetTextView(xTextView* pTextView)
    {
        m_pTextView = pTextView;
    }
    int GetType()
    {
        return m_nOverLengthEffectType;
    }
    virtual chSize GetOffset()
    {
        return chSize(0, 0);
    }
    virtual void loadAttributes(xml_node& node)
    {
        return;
    }
    virtual bool GetMultiLineString(const chRect& rcText, const yl::wstring& strText, yl::wstring& strResult)
    {
        return false;
    }
#if IF_TEXTVIEW_SUPPORT_OVERLENGTH_WRAPCONTENT
    virtual bool wrapContentHeight(int& nHeight)
    {
        return false;
    }
    virtual bool wrapContentWidth(int& nWidth)
    {
        return false;
    }
    virtual bool onPaintContent(xPainter& p)
    {
        return false;
    }
#endif
protected:
    xTextView* m_pTextView;
    int m_nOverLengthEffectType;
};

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
class xFont;

class COverLengthEllipsisEffect : public COverLengthEffectBase
{
public:
    COverLengthEllipsisEffect();
    virtual ~COverLengthEllipsisEffect();

public:
    static int GetOverLengthEllipsisEffectType();
    static COverLengthEllipsisEffect* GetOverLengthEllipsisEffect(COverLengthEffectBase* pOverLengthEffectBase);
    static COverLengthEffectBase* CreateOverLengthEllipsisEffect();

public:
    virtual bool OnStartEffect();
    virtual void onEndEffect();
    virtual yl::wstring GetStringToPaint(const chRect& rcText, const yl::wstring& strText);
    yl::wstring GetEllipsisEffectText(const chRect& rcText, const yl::wstring& wstr, const xFont& font);
    yl::wstring GetMultiEllipsosEffectText(const chRect& rcText, const yl::wstring& wstr);

protected:
    yl::wstring GetLineEllipsisText(const chRect& rcText, const yl::wstring& wstrEllipsis);
};

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
class COverLengthScrollEffectBase : public COverLengthEffectBase, public xTimerHandler
{
public:
    explicit COverLengthScrollEffectBase(OVERLENGTH_TYPE enType);
    virtual ~COverLengthScrollEffectBase();

public:
    void SetScrollEnable(bool bEnable);
    void SetScrollDirection(int nScrollDirection)
    {
        m_nScrollDirection = nScrollDirection;
    }
    void SetScrollTickTime(int nTickTime);
    void SetScrollShift(int nShift);
    void SetScrollRoundDuration(int nRoundDuration);
    void SetScrollLineDuration(int nLineDuration);
    void SetScrollCount(int nScrollCount);
    void OnScrollEndTimer(xThreadTimer* timer, LPVOID pExtraData);
    virtual void loadAttributes(xml_node& node);
protected:
    virtual void ResetScrollInfo();
    virtual bool RestartScroll();
    virtual void onEndEffect();

protected:
    bool               m_bScrollEnabled;        //滚动是否启用
    int                m_nScrollDirection;      //滚动的方向（水平滚动）
    int                m_nScrollTickTime;       //每次滚动的时间
    int                m_nScrollShift;          //每次滚动的长度(垂直滚动)
    int                m_nScrollRoundDuration;  //滚动开始的结束的停留时间
    int                m_nScrollLineDuration;   //纵向滚动时每行的停留时间
    int                m_nScrollOffset;         //绘图用的滚动位移
    int                m_nTotalScrollCount;     //滚动次数
    xThreadTimer      m_tmScroll;              //滚动定时器
    xThreadTimer      m_tmScrollEnd;           //滚动结束停滞用定时器
};

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
class CHorizontalScrollEffect : public COverLengthScrollEffectBase
{
public:
    explicit CHorizontalScrollEffect(OVERLENGTH_TYPE enType);
    virtual ~CHorizontalScrollEffect();
public:
    static int GetHorizontalScrollEffectType();
    static CHorizontalScrollEffect* GetHorizontalScrollEffect(COverLengthEffectBase* pOverLengthEffectBase);
    static COverLengthEffectBase* CreateHorizontalScrollEffect();
public:
    virtual bool OnStartEffect();
    virtual yl::wstring GetStringToPaint(const chRect& rcText, const yl::wstring& strText);
    virtual int GetGravity();
    virtual int GetAlign();
    virtual chSize GetOffset()
    {
        return chSize(m_nScrollOffset, 0);
    }
protected:
    virtual void ResetScrollInfo();
    virtual bool RestartScroll();
    void OnScrollTimer(xThreadTimer* timer, LPVOID pExtraData);
    bool IsHorizontalScrollEnd();
    bool UpdateScrollOffset();
#if IF_TEXTVIEW_SCROLL_BY_CHAR
private:
    int GetCharWidthByIndex(int nIndex);
private:
    int m_nScrollIndex;          //滚到第几个像素（水平滚动）
#endif
protected:
    bool m_bScrollLeft;
};

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#if IF_TEXTVIEW_SUPPORT_VERTICAL_SCROLL
class CVerticalScrollEffect : public COverLengthScrollEffectBase
{
public:
    explicit CVerticalScrollEffect(OVERLENGTH_TYPE enType);
    virtual ~CVerticalScrollEffect();

public:

    static int GetVerticalScrollEffectType();
    static CVerticalScrollEffect* GetVerticalScrollEffect(COverLengthEffectBase* pOverLengthEffectBase);
    static COverLengthEffectBase* CreateVerticalScrollEffect();

public:
    virtual bool OnStartEffect();
    virtual yl::wstring GetStringToPaint(const chRect& rcText, const yl::wstring& strText);
    virtual int GetGravity();
    virtual int GetAlign();
    virtual chSize GetOffset()
    {
        return chSize(0, m_nScrollOffset);
    }

protected:
    virtual void ResetScrollInfo();
    virtual bool RestartScroll();
    BOOL OnScrollTimer(msgObject& msg);

public:
    void PrepareLineSize(const yl::wstring& strCachePaintString, int nGravity);

protected:
    YLVector<int>      m_vecLineVPos;           //每行的垂直位置，用于垂直滚动
    int                m_nScrollTargetLine;     //当前滚动的目标行号
};
#endif

#endif //_OVER_LENGTH_STYLE_BASE_H_
