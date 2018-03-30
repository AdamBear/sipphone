#ifndef __DSSKEY_STYLE_H__
#define __DSSKEY_STYLE_H__

#include "xwindow/xViewHandler.h"
#include "baselibs/include/devicelib/phonedevice.h"

////////////////////////////////////////////////////////////////////////////////////////////


#if IS_COLOR
class CDsskeyFrameBGStyle : public xStyle
{
public:
    CDsskeyFrameBGStyle();
    virtual ~CDsskeyFrameBGStyle();
protected:
    PHONE_TYPE_DEF m_ePhoneType;
};

// T5x dsskey left frame 背景图
class CDsskeyFrameLeftBGStyle : public CDsskeyFrameBGStyle
{
public:
    CDsskeyFrameLeftBGStyle();
    virtual ~CDsskeyFrameLeftBGStyle();
public:
    virtual bool onViewPaintBackground(xView* pView, xPainter& p);
};

// T5x dsskey right frame 背景图
class CDsskeyFrameRightBGStyle : public CDsskeyFrameBGStyle
{
public:
    CDsskeyFrameRightBGStyle();
    virtual ~CDsskeyFrameRightBGStyle();
public:
    virtual bool onViewPaintBackground(xView* pView, xPainter& p);
    void SetPageBtn(bool bHavePageBtn);
protected:
    bool m_bHavePageBtn;
};
#endif

#endif
