#if (defined(IF_SUPPORT_BLUETOOTH ) || defined(IF_SUPPORT_WIFI))
#include "../include/viewmodectrler.h"
#include "dskphone/ui/xwin/settingui/src/viewmodeuimanager.h"
#include "../include/uicommon.h"

bool CViewModeControler::OnSoftKeyEvent(const yl::string & strSoftkeyAction,
                                        int iDataID, const yl::string & strValue, void * pData /*= NULL*/)
{
    if (strSoftkeyAction.empty())
    {
        return false;
    }

    // 将softeky action转换为softkey type
    int iKey = UICommonAPI_MapSoftkeyAction2Type(strSoftkeyAction);

    return OnSoftKeyEvent(iKey, iDataID, strValue, pData);
}

bool CViewModeControler::OnOptionEvent(const yl::string & strSoftkeyAction,
                                       int iDataID, const yl::string & strValue, void * pData /*= NULL*/)
{
    if (strSoftkeyAction.empty())
    {
        return false;
    }

    // 将softeky action转换为softkey type
    int iKey = UICommonAPI_MapSoftkeyAction2Type(strSoftkeyAction);

    return OnOptionEvent(iKey, iDataID, strValue, pData);
}

// 进入页面
bool CViewModeControler::EnterPage(int iUIType, int iLastDataID /*= DATA_ID_INVALID*/)
{
    if (iUIType < VIEW_MODE_UI_TYPE_BEGIN
            || iUIType > VIEW_MODE_UI_TYPE_END)
    {
        return false;
    }

    // 记录非invalid的data id
    if (DATA_ID_INVALID != iLastDataID)
    {
        // 记录当前页焦点的data id
        m_iLastFocusDataID = iLastDataID;
        APP_INFO("Record last data id[%d]", m_iLastFocusDataID);
    }

    return _ViewModeUIMgr.EnterPage(iUIType);
}


bool CViewModeControler::ExitPage()
{
    return _ViewModeUIMgr.ExitPage();
}

#endif // IF_SUPPORT_BLUETOOTH || IF_SUPPORT_WIFI
