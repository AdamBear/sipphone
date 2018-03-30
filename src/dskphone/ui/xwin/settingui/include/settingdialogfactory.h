#ifndef __SETTING_DIALOG_FACTORY_H__
#define __SETTING_DIALOG_FACTORY_H__

#include <ylhashmap.h>
#include <singletonclassdefine.h>
#include "ylstring.h"
#include "dlgsettingui.h"

typedef YLHashMap<yl::string, yl::string> MAP_DIALOG_NAME;

class CSettingDialogFactory
{
    DECLARE_SINGLETON_CLASS(CSettingDialogFactory);

public:
    void RegisterSettingDialog(const yl::string & strPageID, const yl::string & strDialogName);

    // 根据ID获取界面
    CDlgSettingUI * GetSettingDialog(const yl::string & strPageID);
    // 获取默认Delegate
    CDlgSettingUI * GetDefaultSettingDialog();

protected:
    MAP_DIALOG_NAME m_mapDialogName;
};

#define g_SettingDialogFactory GET_SINGLETON_INSTANCE(CSettingDialogFactory)

#endif // __SETTING_DIALOG_FACTORY_H__

