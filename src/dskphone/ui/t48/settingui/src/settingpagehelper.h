#ifndef _SETTINGPAGE_HELPER_H_
#define _SETTINGPAGE_HELPER_H_

#include "setting/include/common.h"
class CSettingWindow;

class CSettingPageHelper
{
public:
    CSettingPageHelper() {};
    CSettingPageHelper(CSettingWindow * pWnd);
    ~CSettingPageHelper();

    void SetSettingWnd(CSettingWindow * pWnd);
    //创建一个子界面放到当前窗口中, strContent为简单action
    bool CreateSubPage(const SubMenuData & pageInfo, const yl::string strContent = "");
    //穿件一个menu类子界面到窗口中， strOpenPageAction必须是fullaction
    bool CreatePageMenu(const yl::string & strOpenPageAction, int nDefaultIndex = -1);
private:
    CSettingWindow * m_pSettingWnd;
};

#endif  //_SETTINGPAGE_HELPER_H_
