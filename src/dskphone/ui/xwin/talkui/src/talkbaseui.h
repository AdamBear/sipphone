#ifndef _TALK_BASEUI_H
#define _TALK_BASEUI_H

#include <ylstl/ylvector.h>
#include "talkuilogic/talkuilogiccommon.h"

#include "xwindow/xWindow.h"

namespace talkui
{

//softkey 数组
typedef YLVector<SoftKey_TYPE> CVectorSoftkeyType;

class CTalkBaseUI : public xFrameLayout
{
public:
    explicit CTalkBaseUI(const yl::string & strTalkUIClassName);
    virtual ~CTalkBaseUI();

public:
    //处理softkey
    virtual bool OnSoftkeyPress(SoftKey_TYPE eSoftType, bool bLongPress) = 0;

public:
    yl::string GetTalkUIClassName();        //获取通话子界面类名称


protected:
    yl::string m_strTalkUIClassName;        //通话子界面类名称

};



}       //namespace talkui


#endif


