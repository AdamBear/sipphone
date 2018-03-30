#if IF_FEATURE_GENBAND_E911
#include "e911locationcontroller.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "uikernel/languagehelper.h"
#include "include/translateiddefine.h"
#include "qtcommon/qmisc.h"
#include "baseui/t4xpicpath.h"
#include "messagebox/modmessagebox.h"

#include "settingui/include/modsettingui.h"
#include "setting/include/modsetting.h"

#include "include/uicommon.h"

#ifdef _T48
#include "settingui/src/settinguicommon.h"
#include "uicommon/messagebox/selectmessagebox.h"
#endif

#include "dlge911location.h"
#include "dlge911setlocation.h"
#include "e911address/include/e911address_def.h"
#include "e911address/include/mode911address.h"

#include "baseui/modbaseui.h"

namespace
{
#define E911_LIST_LOADING_TIP 2
#define E911_SET_LOCATION_TIP 3
}

CE911LocationController& CE911LocationController::GetInstance()
{
    static CE911LocationController E911LocationController;
    return E911LocationController;
}


static LRESULT OnE911LocationMailMsg(msgObject& objMessage)
{
    return g_E911Controller.OnE911Message(objMessage);
}

CE911LocationController::CE911LocationController(QObject* pParent/* = NULL*/)
    : CMsgBoxCallBackAction()
{
    m_bPopNotify = false;
    etl_RegisterMsgHandle(E911_MESSAGE_BEGIN, E911_MESSAGE_END, OnE911LocationMailMsg);
}

CE911LocationController::~CE911LocationController()
{
    etl_UnregisterMsgHandle(E911_MESSAGE_BEGIN, E911_MESSAGE_END, OnE911LocationMailMsg);
}

LRESULT CE911LocationController::OnE911Message(msgObject& objMessage)
{
    SETTINGUI_INFO("CE911LocationController::OnE911Message, %d, %d, %d",
                   objMessage.message, objMessage.wParam, objMessage.lParam);

    switch (objMessage.message)
    {
    case E911_MESSAGE_DOWNLOAD_RESULT:
        {
            MessageBox_RemoveMessageBox(this, MessageBox_Destory, E911_LIST_LOADING_TIP);

            if (objMessage.wParam == 1)
            {
                SettingUI_EnterE911Setting(kszE911RootID);
            }
            else
            {
                MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_LOAD_FAILED),
                                            MESSAGEBOX_Cancel, 1500);
            }
        }
        break;
    case E911_MESSAGE_DOWNLOAD_ADDRESS:
        {
            if (UIManager_IsInIdle())
            {
                MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_SET_YOU_LOCATION),
                                            MESSAGEBOX_OkCancel, 10000, "", "", E911_SET_LOCATION_TIP);
            }
            else
            {
                m_bPopNotify = true;
            }
        }
        break;
    default:
        break;
    }

    return 0;
}

void CE911LocationController::ProcessIdleEvent()
{
    if (m_bPopNotify
            && e911_IsGenbandE911On()
            && UIManager_IsInIdle())
    {
        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_SET_YOU_LOCATION),
                                    MESSAGEBOX_OkCancel, 10000, "", "", E911_SET_LOCATION_TIP);
    }
    m_bPopNotify = false;
}

void CE911LocationController::MessageBoxCallBack(CMessageBoxBase* pMessageBox)
{
    SETTINGUI_INFO("CE911LocationController::MessageBoxCallBack");
    if (pMessageBox == NULL)
    {
        return;
    }
    switch (pMessageBox->GetID())
    {
    case E911_LIST_LOADING_TIP:
        {
            if (pMessageBox->GetResult() == MessageBox_Cancel)
            {
                e911_CancelDownload();
            }
        }
        break;
    case E911_SET_LOCATION_TIP://hoteling 登出功能
        {
            if (pMessageBox->GetResult() == MessageBox_OK)
            {
                if (e911_GetAddressListCount(kszE911RootID) > 0)
                {
                    SettingUI_EnterE911Setting(kszE911RootID);
                }
                else
                {
                    if (e911_ReqAddress())
                    {
                        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_LOADING_PLEASE_WAIT),
                                                    MESSAGEBOX_Cancel, -1, "", "", E911_LIST_LOADING_TIP);
                    }
                    else
                    {
                        MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_LOAD_FAILED),
                                                    MESSAGEBOX_Cancel, 1500);
                    }
                }
            }
            else if (pMessageBox->GetResult() == MessageBox_Destory
                     || pMessageBox->GetResult() == MessageBox_Cancel)
            {
                e911_ClearListData();
            }

        }
        break;
    default:
        break;
    }
}

void CE911LocationController::ResetE911Location()
{
    bool bSuc = e911_ReqAddress();
    if (bSuc)
    {
        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_LOADING_PLEASE_WAIT),
                                    MESSAGEBOX_Cancel, -1, "", "", E911_LIST_LOADING_TIP);
    }
    else
    {
        MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_LOAD_FAILED),
                                    MESSAGEBOX_Cancel, 1500);
    }
}

void CE911LocationController::ExitE911SetUI(int nCurrentID)
{
    CBaseDialog* pDlgPage = qobject_cast<CBaseDialog*>(UIManager_GetPrivateDlg(DLG_CDlgE911SetLocation));
    if (pDlgPage == NULL)
    {
        SETTINGUI_INFO("CE911LocationController::ExitE911SetUI Dlg Not Exist");
        return;
    }


    CBaseDialog* pDlgTop = UIManager_GetTopWindow();
    UIManager_PutPrivateDlg(pDlgTop);
    if (nCurrentID > 0)
    {
        E911AddressData* pData = e911_GetAddressDetailByID(nCurrentID);
        if (pData != NULL)
        {
            //需要退出上级界面
            ExitE911SetUI(pData->m_nParentID);
        }
    }
}

void CE911LocationController::QuitSetLoactionPage(int nCurrentID)
{
    ExitE911SetUI(nCurrentID);
    //全部退出刷新顶层界面
    CDlgE911Location* pDlg = qobject_cast<CDlgE911Location*>(UIManager_GetPrivateDlg(DLG_CDlgE911Location));
    if (pDlg != NULL)
    {
        pDlg->UpdateText();

        UIManager_SetTopWindow(pDlg);
        UIManager_UpdateWnd(pDlg);
    }
}
#endif
