#ifndef _SETTINGUI_SCROLL_BY_PAGE_DELEGATE_BASE_H_
#define _SETTINGUI_SCROLL_BY_PAGE_DELEGATE_BASE_H_

#include "basesettinguidelegate.h"
#include "ylvector.h"

typedef YLVector<yl::string> VEC_TEXT;

class CSettinguiScrollByPageDelegateBase : public CSettingUIDelegateBase
{
public:
    CSettinguiScrollByPageDelegateBase();
    virtual ~CSettinguiScrollByPageDelegateBase();
    static CSettingUIDelegateBase * CreatScrollByPageDelegateBase();
public:
    // 处理按键事件
    virtual bool OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex);
public:
    yl::string GetValueByIndex(int nIndex);
    int GetTotalPage();
    int getCurrentPage();
    int GetTotalItemCount();
protected:
    VEC_TEXT m_vecText;
    int m_nCurrentPage;
};

#endif //_SETTINGUI_SCROLL_BY_PAGE_DELEGATE_BASE_H_
