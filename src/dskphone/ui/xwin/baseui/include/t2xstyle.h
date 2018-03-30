#ifndef __T2X_STYLE_H__
#define __T2X_STYLE_H__

#include "xwindow/xViewHandler.h"
#include <baseuidefine.h>

////////////////////////////////////////////////////////////////////////////////////////////

void InitCommonStyle();
void UnInitCommonStyle();

// ComboBox风格
class xComboBoxStyle : public xStyle
{
public:
    xComboBoxStyle();
    virtual ~xComboBoxStyle();

    static void Init();
    static void UnInit();

public:
    virtual bool onViewPaintContent(xView * pView, xPainter & p);
    virtual bool onViewRectChanged(xView * pView, const chRect & rectOld);
protected:
    xPixmap m_pmArrow;
};

////////////////////////////////////////////////////////////////////////////////////////////
// 弹出框方形背景风格
class CRectFrameBGStyle : public xStyle
{
public:
    CRectFrameBGStyle();
    virtual ~CRectFrameBGStyle();

public:
    virtual bool onViewPaintBackground(xView * pView, xPainter & p);
};

#if !IS_COLOR
void InitT2xStyle();
void UnInitT2xStyle();
////////////////////////////////////////////////////////////////////////////////////////////
// 圆角矩形按钮
class xRoundBtnStyle : public xStyle
{
public:
    xRoundBtnStyle();
    virtual ~xRoundBtnStyle();

public:
    virtual bool onViewPaintBackground(xView * pView, xPainter & p);
};

////////////////////////////////////////////////////////////////////////////////////////////
// 圆角矩形边框按钮
class xRoundFrameBtnStyle : public xStyle
{
public:
    xRoundFrameBtnStyle();
    virtual ~xRoundFrameBtnStyle();

public:
    virtual bool onViewPaintBackground(xView * pView, xPainter & p);
};

////////////////////////////////////////////////////////////////////////////////////////////
// 横线Title风格
class xNormalTitleStyle : public xStyle
{
public:
    xNormalTitleStyle();
    virtual ~xNormalTitleStyle();

public:
    virtual bool onViewPaintBackground(xView * pView, xPainter & p);
};

////////////////////////////////////////////////////////////////////////////////////////////
// 弹出框标题栏风格
class CMsgBoxCommonTitleStyle : public xStyle
{
public:
    CMsgBoxCommonTitleStyle();
    virtual ~CMsgBoxCommonTitleStyle();

    static void Init();

public:
    virtual bool onViewPaintContent(xView * pView, xPainter & p);

protected:
    int m_nTildeWidth;
    int m_nTildeHeight;
};

////////////////////////////////////////////////////////////////////////////////////////////
#if IF_OPTION_BOX_SUPPORT_RECT_BG
// Option方形背景风格
class COptionRectFrameBGStyle : public xStyle
{
public:
    COptionRectFrameBGStyle();
    virtual ~COptionRectFrameBGStyle();

public:
    virtual bool onViewPaintBackground(xView * pView, xPainter & p);
};
#endif

////////////////////////////////////////////////////////////////////////////////////////////
// 弹出框圆形背景风格
class CMsgBoxRoundBGStyle : public xStyle
{
public:
    CMsgBoxRoundBGStyle();
    virtual ~CMsgBoxRoundBGStyle();

public:
    virtual bool onViewPaintBackground(xView * pView, xPainter & p);
};
#endif

#endif
