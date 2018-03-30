
#ifndef __I_POPUP_MENU_DELEGATE_H__
#define __I_POPUP_MENU_DELEGATE_H__

#include "uikernel/iframelistdelegate.h"

class IPopupMenuDelegate : public IFrameListDelegate
{
public:
    IPopupMenuDelegate() {};
    virtual ~IPopupMenuDelegate() {};

public:
    // 获得Menu总数量
    virtual int GetTotalMenuCount()                             = 0;
};

#endif
