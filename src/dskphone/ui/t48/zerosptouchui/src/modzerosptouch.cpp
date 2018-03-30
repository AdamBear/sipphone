#include "modzerosptouch.h"
#include "cdlgzerosptouch.h"

#include "uimanager/uimanager_inc.h"
#include "mainwnd/mainwnd.h"
#include "setting/include/modzero.h"
#include "baseui/modbaseui.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "dsklog/log.h"
#if IF_FEATURE_METASWITCH
#include "autopcontrol/include/autop_common.h"
#include "autopcontrol/include/modautopcontrol.h"
#endif

void ZeroSPTouch_RegisterDlgClass()
{
    REGISTER_DLG(CDlgZeroSPTouch);
}

bool ZeroSPTouch_OpenDlg(const yl::string & strPageAction, bool bRepaint/* = false*/)
{
    // 有话路时，不进行Zero相关界面操作 http://10.3.5.199/Bug.php?BugID=62617
    if (talklogic_SessionExist())
    {
        return true;
    }

    // 获取窗口的指针
    CBaseDialog * pDlgBase = UIManager_GetPrivateDlg(DLG_CDlgZeroSPTouch);
    if (NULL == pDlgBase)
    {
        pDlgBase = UIManager_CreatePrivateDlg(DLG_CDlgZeroSPTouch);
    }

    CDlgZeroSPTouch * pDlgZeroSPTouch = qobject_cast<CDlgZeroSPTouch *>(pDlgBase);

    if (NULL != pDlgZeroSPTouch)
    {
        //pDlgZeroSPTouch->SetData();// 设置窗口数据
        pDlgZeroSPTouch->SetPageType(strPageAction);
        // 若窗口不在最顶层则置顶窗口
        if (pDlgZeroSPTouch->isHidden())
        {
            UIManager_SetTopWindow(pDlgZeroSPTouch);
        }

        UIManager_UpdateWnd(pDlgZeroSPTouch);// 刷新到界面上
        _MainWND.DisplayIMESoftkey(pDlgZeroSPTouch->focusWidget());

        if (bRepaint)
        {
            _MainWND.repaint();

            //处理所有其他事件，主要避免还有IME事件未处理!!!!
            QApplication::processEvents();
        }

        return true;
    }
    else
    {
        ZEROUI_INFO("Open DlgZeroSPTouch Error!");
        return false;
    }
}

bool ZeroSPTouch_Destroy()
{
    // 获取窗口的指针
    CBaseDialog * pDlgBase = UIManager_GetPrivateDlg(DLG_CDlgZeroSPTouch);
    if (NULL == pDlgBase)
    {
        pDlgBase = UIManager_CreatePrivateDlg(DLG_CDlgZeroSPTouch);
    }

    CDlgZeroSPTouch * pDlgZeroSPTouch = qobject_cast<CDlgZeroSPTouch *>(pDlgBase);

    if (NULL != pDlgZeroSPTouch)
    {
        pDlgZeroSPTouch->SetRejectReturnToIdle(false);
    }

    UIManager_ReturnToIdle();
    return true;
}


#if IF_FEATURE_METASWITCH
bool ZeroSPTouch_EnterAutopDirect()
{
    ZEROUI_INFO("ZeroSPTouch_EnterAutopDirect()");
    Autop_Start(AUT_MTSW_HTTP_DIGEST);

    return ZeroSPTouch_OpenDlg(ZERO_AUTOP_PAGE_DIRECT);
}
#endif

bool ZeroSPTouch_IsAllowExit()
{
    // 获取窗口的指针
    CBaseDialog* pDlgBase = UIManager_GetPrivateDlg(DLG_CDlgZeroSPTouch);
    if (pDlgBase == NULL)
    {
        return false;
    }

    CDlgZeroSPTouch* pDlgZeroSPTouch = qobject_cast<CDlgZeroSPTouch*>(pDlgBase);
    if (NULL != pDlgZeroSPTouch)
    {
        if (pDlgZeroSPTouch->IsAuthPage() || pDlgZeroSPTouch->IsResultPage())
        {
            return true;
        }
    }

    return false;
}
