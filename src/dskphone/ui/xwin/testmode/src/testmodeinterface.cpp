#include "testmodeinterface.h"
#include "uimanager/moduimanager.h"
#include "dsklog/log.h"
#include "cdlgtestmode.h"
#include "testphonet19.h"
#include "testphonet21.h"
#include "testphonet23.h"
#include "testphonet27.h"
#include "testphonet40.h"
#include "testphonet42.h"
#include "testphonecp.h"
#include "testphonet54.h"
#include "testphonet52.h"
#include "testphonet46.h"
#ifdef _CP920
#include "testphonecp.h"
#endif

CDlgTestMode * GetTestModeDlg()
{
    CDlgTestMode * pTestDlg = static_cast<CDlgTestMode *>(UIManager_GetPrivateDlg(DLG_CDlgTestMode));
    if (NULL != pTestDlg && UIManager_GetTopWindow() == pTestDlg)
    {
        return pTestDlg;
    }
    else
    {
        TESTMODE_WARN("DlgTestMode No running.");
    }
    return NULL;
}

void testmode_DrawText(yl::string strText, int nTextType/* = TT_CONTENT_CENTER*/,
                       int nFontSize/* = TEST_FONT_20*/)
{
    CDlgTestMode * pTestDlg = GetTestModeDlg();
    if (NULL != pTestDlg)
    {
        // 刷新显示文本
        pTestDlg->SetDisplayText(strText, nTextType, nFontSize);
    }
}

void testmode_SetTextColor(int nTextType, const xColor & col)
{
    CDlgTestMode * pTestDlg = GetTestModeDlg();
    if (NULL != pTestDlg)
    {
        // 刷区域颜色
        pTestDlg->SetTextColor(nTextType, col);
    }
}

void testmode_FillRect(int nAreaType, const xColor & col)
{
    CDlgTestMode * pTestDlg = GetTestModeDlg();
    if (NULL != pTestDlg)
    {
        // 刷区域颜色
        pTestDlg->FillRectByColor(nAreaType, col);
    }
}

void testmode_FillRect(const yl::string& strPic)
{
    CDlgTestMode * pTestDlg = GetTestModeDlg();
    if (NULL != pTestDlg)
    {
        pTestDlg->FillFullByPicture(strPic);
    }
}


#ifdef _CP920
void testmode_ShowErrorKeys(bool bShow)
{
    CDlgTestMode * pTestDlg = GetTestModeDlg();

    if (NULL != pTestDlg)
    {
        pTestDlg->ShowErrorKeys(bShow);
    }
}
#endif

CTestPhone * testmode_GetCurrentTestPhone()
{
    CDlgTestMode * pTestDlg = GetTestModeDlg();
    if (NULL != pTestDlg)
    {
        // 刷区域颜色
        return pTestDlg->GetTestPhone();
    }
    return NULL;
}

CTestPhone* testmode_CreateTestPhone()
{
    switch (devicelib_GetPhoneType())
    {
    case PT_T19:
        return new CTestPhoneT19;
        break;
    case PT_T21:
        return new CTestPhoneT21;
        break;
    case PT_T23:
        return new CTestPhoneT23;
        break;
    case PT_T66:
        return new CTestPhoneT66;
        break;
    case PT_T27:
    case PT_T27S:
        return new CTestPhoneT27;
        break;
    case PT_T41:
    case PT_T42:
    case PT_T43S:
    case PT_T43P:
        return new CTestPhoneT42;
        break;
#ifdef _CP
    case PT_CP860:
        return new CTestPhoneCP;
        break;
#endif
    case PT_T40:
    case PT_T40G:
        return new CTestPhoneT40;
        break;
#if IS_COLOR
    case PT_T54S:
        return new CTestPhoneT54;
        break;
    case PT_T52:
        return new CTestPhoneT52;
        break;
    case PT_T46:
    case PT_T46S:
        return new CTestPhoneT46;
        break;
#endif //IS_COLOR
#ifdef _CP920
    case PT_CP920:
        return new CTestPhoneCP;
        break;
#endif
    default:
        break;
    }

    return NULL;
}

