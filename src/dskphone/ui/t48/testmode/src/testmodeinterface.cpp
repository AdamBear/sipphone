#include "testmodeinterface.h"
#include "uimanager/moduimanager.h"
#include "dsklog/log.h"

// 初始化
void testmode_DrawText(QString strText, bool bCenter /*= true*/, int fontsize,
                       QColor clrTx /*= Qt::black*/, QColor clrBk /*= Qt::white*/)
{
    CDlgTestMode * pTestDlg = qobject_cast<CDlgTestMode *>(UIManager_GetPrivateDlg(DLG_CDlgTestMode));
    if (NULL != pTestDlg)
    {
        //刷屏
        pTestDlg->FullScreenByColor(clrBk);
        // 设置字体颜色
        pTestDlg->SetFrontColor(clrTx);
        //显示随机数
        pTestDlg->SetDisplayText(strText, bCenter, fontsize);
    }
    else
    {
        TESTMODE_WARN("DlgTestMode No running.");
    }
}

void testmode_DrawTextInRect(QString strText, QRect rectText, QColor clrTx /*= Qt::black*/)
{
    CDlgTestMode * pTestDlg = qobject_cast<CDlgTestMode *>(UIManager_GetPrivateDlg(DLG_CDlgTestMode));
    if (NULL != pTestDlg)
    {
        pTestDlg->SetDisplayTextInRect(strText, rectText, clrTx);
    }
}

void testmode_FullScreen(QColor clr)
{
    CDlgTestMode * pTestDlg = qobject_cast<CDlgTestMode *>(UIManager_GetPrivateDlg(DLG_CDlgTestMode));
    if (NULL != pTestDlg)
    {
        QColor clrGray = Qt::lightGray;
        if (clrGray == clr)
        {
            // 灰阶渐变
            pTestDlg->FullScreenByGradient();
        }
        else
        {
            // 刷屏
            pTestDlg->FullScreenByColor(clr);
        }
    }
}

void testmode_FullScreen(QString strPic)
{
    CDlgTestMode * pTestDlg = qobject_cast<CDlgTestMode *>(UIManager_GetPrivateDlg(DLG_CDlgTestMode));
    if (NULL != pTestDlg)
    {

        if ("" != strPic)
        {
            pTestDlg->FullScreenByPic(strPic);
        }
    }
}

void testmode_SetFrontColor(QColor clr)
{
    CDlgTestMode * pTestDlg = qobject_cast<CDlgTestMode *>(UIManager_GetPrivateDlg(DLG_CDlgTestMode));
    if (NULL != pTestDlg)
    {
        pTestDlg->SetFrontColor(clr);
    }

}
void testmode_SetAllHide()
{
    CDlgTestMode * pTestDlg = qobject_cast<CDlgTestMode *>(UIManager_GetPrivateDlg(DLG_CDlgTestMode));
    if (NULL != pTestDlg)
    {
        pTestDlg->SetAllHide();
    }
}
void testmode_SetFrameAndTitleVisible(TEST_STATE tstate)
{
    CDlgTestMode * pTestDlg = qobject_cast<CDlgTestMode *>(UIManager_GetPrivateDlg(DLG_CDlgTestMode));
    pTestDlg->SetFrameAndTitleVisible(tstate);
}

void testmode_SetEasyTouch(bool bShow, QPoint point, int nSize)
{
    CDlgTestMode * pTestDlg = qobject_cast<CDlgTestMode *>(UIManager_GetPrivateDlg(DLG_CDlgTestMode));
    pTestDlg->SetEasyTouch(bShow, point, nSize);
}

bool testmode_getBeTouched()
{
    CDlgTestMode * pTestDlg = qobject_cast<CDlgTestMode *>(UIManager_GetPrivateDlg(DLG_CDlgTestMode));
    return pTestDlg->getBeTouched();
}
