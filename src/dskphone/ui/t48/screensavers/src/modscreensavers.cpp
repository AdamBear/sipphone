#include "modscreensavers.h"
#include "screensavers.h"
#include "uimanager/moduimanager.h"
#include "idlescreen/include/modidlescreen.h"
#if IF_FEATURE_XMLB_SCREENSAVER
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "extended/xmlbrowser/include/modxmlbrowser.h"
#endif

namespace
{
#define SCREENSAVER_STATUS_ICON_AMOUNT              5
}

void ScreenSavers_Init()
{
    REGISTER_DLG(CDlgScreenSavers);

    g_pScreenSaverController->LoadDefault();

    idleScreen_SetNotifyFunByCount(&CDlgScreenSavers::StatusNotifyFunction,
                                   SCREENSAVER_STATUS_ICON_AMOUNT);
}

void ScreenSavers_Enter()
{
#if IF_FEATURE_XMLB_SCREENSAVER
    SCREENSAVER_MODE_CHANGE_TYPE eSSType = (SCREENSAVER_MODE_CHANGE_TYPE)configParser_GetConfigInt(
            kszScreenSaverType);
    if (eSSType == SCREENSAVER_MODE_CHANGE_TYPE_XMLB)
    {
        XMLLogicToUIData objData;
        if (xmlbrowser_GetSSData(objData)
                && objData.pElementData != NULL)
        {
            XBScreenSaverData * pSSData = static_cast<XBScreenSaverData *>(objData.pElementData);
            ScreenSavers_XMBEnter(pSSData);
            return;
        }

        yl::string strURL = configParser_GetConfigString(kszXmlBrowserScreenSaverURL);
        if (!strURL.empty())
        {
            xmlbrowser_ScreenSaverEnter(strURL.c_str());
            return;
        }
        else
        {
            //将屏保模式设置为system模式
            configParser_SetConfigInt(kszScreenType, 0, CONFIG_LEVEL_PHONE);
            etl_NotifyApp(FALSE, CONFIG_MSG_BCAST_CHANGED, ST_SCREEN_SAVER_MODE, 0);
        }
    }
#endif

    CBaseDialog * pScreenSavers = UIManager_GetPrivateDlg(DLG_CDlgScreenSavers);
    if (NULL != pScreenSavers)
    {
        return ; // 屏保界面只允许存在一个
    }

    pScreenSavers = UIManager_CreatePrivateDlg(DLG_CDlgScreenSavers);
    if (NULL == pScreenSavers)
    {
        return ;
    }

    // 刷新窗口
    UIManager_SetTopWindow(pScreenSavers);
    UIManager_UpdateWnd(pScreenSavers);

    return ;
}

void ScreenSavers_ExitScreensaver()
{
    CBaseDialog * pDlgScreensaver = UIManager_GetPrivateDlg(DLG_CDlgScreenSavers);

    if (NULL == pDlgScreensaver)
    {
        return;
    }

    UIManager_PutPrivateDlg(pDlgScreensaver);
}

#if IF_FEATURE_XMLB_SCREENSAVER
//xmlbrowser方式屏保入口
bool ScreenSavers_XMBEnter(void * pData)
{
    CBaseDialog * pBaseDialog = UIManager_GetPrivateDlg(DLG_CDlgScreenSavers);

    if (NULL == pBaseDialog)
    {
        pBaseDialog = UIManager_CreatePrivateDlg(DLG_CDlgScreenSavers);

        if (NULL == pBaseDialog)
        {
            return false;
        }

    }

    CDlgScreenSavers * pScreenSavers = static_cast<CDlgScreenSavers *>(pBaseDialog);
    if (pScreenSavers == NULL)
    {
        return false;
    }

    bool bRet = pScreenSavers->InitXmlWigdet(pData);
    if (!bRet)
    {
        return false;
    }

    // 刷新窗口
    UIManager_SetTopWindow(pScreenSavers);

    UIManager_UpdateWnd(pScreenSavers);

    return true;

}
#endif

bool ScreenSavers_IsExist()
{
    CBaseDialog * pScreenSavers = UIManager_GetPrivateDlg(DLG_CDlgScreenSavers);
    if (NULL != pScreenSavers)
    {
        return true;
    }

    return false;
}
