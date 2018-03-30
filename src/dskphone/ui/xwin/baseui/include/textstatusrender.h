#ifndef __TEXT_STATUS_RENDER_H__
#define __TEXT_STATUS_RENDER_H__

#include "xwindow/xGraphic.h"
#include <baseui/include/baseuidefine.h>

#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR

class CTextStatusRender
{
public:
    CTextStatusRender();
    ~CTextStatusRender();

public:
    virtual void SetTextColor(const xColor& clrNormal, const xColor& clrFocus, const xColor& clrDisale);

    void ResetTextColor();

protected:
    xColor              m_colorNormal;
    xColor              m_colorFocus;
    xColor              m_colorDisable;
};

#endif //IF_ITEM_SUPPORT_MUTI_TEXT_COLOR
#endif // __TEXT_STATUS_RENDER_H__
