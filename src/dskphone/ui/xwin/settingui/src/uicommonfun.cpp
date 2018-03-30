#include "uicommonfun.h"
#include "dlgsettingui.h"
#include "basesettingdialog.h"
#include "settinguidialogmanager.h"
#include "dsklog/log.h"

////////////////////////////////////////// 回调函数 begin ///////////////////////////////////////////
void RefreshUI(bool bLoadData /*= false*/, const yl::string & strPageId /*= ""*/)
{
    CSettingDialogBase * pDlg = g_pSettingUIDialogManager->GetTopActiveDialog(strPageId);
    if (NULL == pDlg)
    {
        return;
    }

    pDlg->RefreshUI(bLoadData);
}

void CacheDlgData(const yl::string & strPageId /*= ""*/)
{
    CSettingDialogBase * pDlg = g_pSettingUIDialogManager->GetTopActiveDialog(strPageId);
    if (NULL == pDlg)
    {
        return;
    }

    pDlg->CachePageData();
}

void RefreshSoftkey(const yl::string & strPageId /*= ""*/)
{
    CSettingDialogBase * pDlg = g_pSettingUIDialogManager->GetTopActiveDialog(strPageId);
    if (NULL == pDlg)
    {
        return;
    }

    pDlg->RefreshSoftkey();
}

void RefreshDlgTitle(const yl::string & strTitle, const yl::string & strPageId /*= ""*/)
{
    CDlgSettingUI * pDlg = static_cast<CDlgSettingUI *>(g_pSettingUIDialogManager->GetTopActiveDialog(
                               strPageId));
    if (NULL == pDlg)
    {
        return;
    }

    pDlg->RefreshTitle(strTitle);
}

void FallBackToPreviousPage(const yl::string & strPageId /*= ""*/)
{
    CSettingDialogBase * pDlg = g_pSettingUIDialogManager->GetTopActiveDialog(strPageId);
    if (NULL == pDlg)
    {
        return;
    }

    pDlg->FallBackToThePreviousInterface();
}

void BackToIdle()
{
    UIManager_ReturnToIdle(true);
}

void DropOutCurrentPage(bool bDelDlg /*= true*/, const yl::string & strPageId /*= ""*/)
{
    CSettingDialogBase * pDlg = g_pSettingUIDialogManager->GetTopActiveDialog(strPageId);
    if (NULL == pDlg)
    {
        return;
    }

    pDlg->DropOutCurrentPage(bDelDlg);
}

void DropOutSettingUI(const yl::string & strPageId /*= ""*/)
{
    g_pSettingUIDialogManager->DropSettingUI();
}

void BindDelegate(CSettingUIDelegateBasePtr pDelegate, const yl::string & strPageId /*= ""*/)
{
    CSettingDialogBase * pDlg = g_pSettingUIDialogManager->GetTopActiveDialog(strPageId);
    if (NULL == pDlg)
    {
        return;
    }

    pDlg->BindDelegate(pDelegate);
}

void UnBindDelegate(const yl::string & strPageId /*= ""*/)
{
    CSettingDialogBase * pDlg = g_pSettingUIDialogManager->GetTopActiveDialog(strPageId);
    if (NULL == pDlg)
    {
        return;
    }

    pDlg->UnBindDelegate();
}

void PopCommonMessageBox(const yl::string & strPageId, const yl::string & strTitle,
                         const yl::string & strNote,
                         int eType, const yl::string & strCancelBtnTitle,
                         const yl::string & strConfirmBtnTitle, int nMsTime,
                         int iID, void * pExtraData,
                         CMsgBoxCallBackAction * pAction)
{
    CSettingDialogBase * pDlg = g_pSettingUIDialogManager->GetTopActiveDialog(strPageId);
    if (NULL == pDlg)
    {
        return;
    }

    pDlg->PopCommonMessageBox(strTitle, strNote, eType, strCancelBtnTitle, strConfirmBtnTitle
                              , nMsTime, iID, pExtraData, pAction);
}

void DealWithValidData(CSettingUIDataValidInfo & dataValid, const yl::string & strPageId /*= ""*/)
{
    CSettingDialogBase * pDlg = g_pSettingUIDialogManager->GetTopActiveDialog(strPageId);
    if (NULL == pDlg)
    {
        return;
    }

    pDlg->DealWithDataValid(dataValid);
}

bool EnterPage(const PageOpenData & openPageData)
{
    return g_pSettingUIDialogManager->OpenPage(openPageData);
}
