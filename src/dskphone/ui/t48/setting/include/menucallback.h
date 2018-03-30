#ifndef _MENU_CALLBACK_H_
#define _MENU_CALLBACK_H_

#include <ylstring.h>


// Dsskey设置界面回调的数据类型
struct DsskeySettingCallbackData
{
    yl::string strLab;
    yl::string strValue;
};

class CMenuCallback
{
public:
    CMenuCallback()
    {

    }

    virtual ~CMenuCallback()
    {

    }

    // 回调函数.
    virtual void OnMenuCallback(void * pData) = 0;
};

#endif
