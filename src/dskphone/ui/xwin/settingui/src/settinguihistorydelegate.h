#ifndef __SETTINGUI_HISTORY_DELEGATE_H__
#define __SETTINGUI_HISTORY_DELEGATE_H__

#include "basesettinguidelegate.h"

class CSettingUIHistoryDelegate : public CSettingUIDelegateBase
{
public:
    CSettingUIHistoryDelegate();
    virtual ~CSettingUIHistoryDelegate();

    static CSettingUIDelegateBase * CreateHistoryDelegate();
    static bool EnterHistoryDirectly();

public:
    // 加载配置数据
    virtual CSettingUIPageDataBase * LoadPageData(bool bReLoad = false);

public:
    virtual bool OnAction(int eActionType, int iDataIndex);

protected:
    bool AddItem(CSettingUIPageDataBase * pPageData, const yl::string & strID,
                 const yl::string & strName);
};

#endif
