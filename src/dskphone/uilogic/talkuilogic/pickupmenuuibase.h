#ifndef _PICKUPMENU_UI_BASE_H
#define _PICKUPMENU_UI_BASE_H

#include "pickupmenuprocessor.h"


class CPickUPMenuUIBase
{
public:
    CPickUPMenuUIBase();
    virtual ~CPickUPMenuUIBase();

public:
    //获取焦点 pickupmenuitem 的序号
    virtual int GetFocusPickUPMenuItemIndex() = 0;


protected:
    //pickupmenu 逻辑处理实例
    CPickUPMenuProcessor m_objPickUPMenuProcessor;



};


#endif
