#include "wifi_inc.h"

/************************************************************************/
/* 接口  ： WifiUI_EnterBaseView()                                       */
/* 功能  ： 进入wifi列表界面                                            */
/* 参数说明 ：无                                                        */
/* 返回值 ： 无                                                         */
/************************************************************************/
void WifiUI_EnterBaseView()
{
    SettingUI_OpenPage(SubMenuData("OpenSubPage(wifi)"));
}
