#include "textstatusrender.h"
#include "devicelib/phonedevice.h"

#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR

namespace
{
// 默认颜色
#define DEFAULT_NORMAL_COLOR        (xColor(0x0, 0x0, 0x0))
#define DEFAULT_NORMAL_COLOR_T29     (xColor(0xff, 0xff, 0xff))
#define DEFAULT_FOCUS_COLOR         (xColor(0x0, 0x0, 0x0))
#define DEFAULT_DISABLE_COLOR_T2946         (xColor(0xB7, 0xCD, 0xE6))
#define DEFAULT_DISABLE_COLOR_T5254         (xColor(0xff, 0xff, 0xff))
}

CTextStatusRender::CTextStatusRender()
{
    ResetTextColor();
}

CTextStatusRender::~CTextStatusRender()
{
}

void CTextStatusRender::SetTextColor(const xColor& clrNormal, const xColor& clrFocus, const xColor& clrDisale)
{
    m_colorNormal = clrNormal;
    m_colorFocus = clrFocus;
    m_colorDisable = clrDisale;
}

void CTextStatusRender::ResetTextColor()
{
    PHONE_TYPE_DEF ePhoneType = devicelib_GetPhoneType();
    m_colorNormal = (PT_T29 == ePhoneType) ? DEFAULT_NORMAL_COLOR_T29 : DEFAULT_NORMAL_COLOR;
    m_colorFocus = DEFAULT_FOCUS_COLOR;
    if (PT_T29 == ePhoneType || DEVICELIB_IS_T46(ePhoneType))
    {
        m_colorDisable = DEFAULT_DISABLE_COLOR_T2946;
    }
    else
    {
        m_colorDisable = DEFAULT_DISABLE_COLOR_T5254;
    }
}

#endif
