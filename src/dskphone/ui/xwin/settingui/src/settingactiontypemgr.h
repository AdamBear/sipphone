#ifndef __SETTING_ACTION_TYPE_MANAGER_H__
#define __SETTING_ACTION_TYPE_MANAGER_H__

#include "setuidatadefine.h"


class CSettingActionTypeMgr
{
public:
    static CSettingActionTypeMgr * GetInstance();

public:
    int TransSoftkeyType2SettingActionType(int eSoftkeyType);

private:
    explicit CSettingActionTypeMgr();

private:
    static CSettingActionTypeMgr    *   m_pInstance;
};

#define g_pSettingActionTypeMgr (CSettingActionTypeMgr::GetInstance())


#endif //__SETTING_ACTION_TYPE_MANAGER_H__

