#ifndef _SCAMENU_UI_BASE_H
#define _SCAMENU_UI_BASE_H

#include "scamenuprocessor.h"


class CSCAMenuUIBase
{
public:
    CSCAMenuUIBase();
    virtual ~CSCAMenuUIBase();

public:
    //获取焦点 SCAmenuitem 的序号
    virtual int GetFocusSCAMenuItemIndex() = 0;


protected:
    //SCAmenu 逻辑处理实例
    CSCAMenuProcessor m_objSCAMenuProcessor;



};


#endif
