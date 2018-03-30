#include "model.h"
#include "cdlgbusyforward.h"
#include "uicommon.h"
#include "qtcommon/qmisc.h"
#include "setting/include/common.h"
#include "setting/src/callforwardcontroller.h"
#include "uikernel/languagehelper.h"
#include "inputmethod/inputmethod.h"
#include "baseui/fontcolor.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "messagebox/modmessagebox.h"
#include "talk/fwd_dnd/include/modfwddnd.h"
#include "account/include/modaccount.h"
#include "uikernel/qcomboboxutility.h"
#include "uikernel/qlineeditutility.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "cforworduilogic.h"
#include "settingpage.h"
#include "baseui/t4xpicpath.h"


LRESULT CDlgBaseFwdPageUI::OnMessage(msgObject & msg)
{
    SETTINGUI_INFO("OnForwardMsg [%#x] [%#x] [%#x]",
                   msg.message, msg.wParam, msg.lParam);

    CBaseDialog * pBaseDlg = UIManager_GetTopWindow();
    if (pBaseDlg != NULL && pBaseDlg->inherits("CSettingWindow"))
    {
        CSettingWindow * pSetWnd = qobject_cast<CSettingWindow *>(pBaseDlg);
        if (NULL != pSetWnd)
        {
            CDlgBaseFwdPageUI * pSubFwdPageUI =
                qobject_cast<CDlgBaseFwdPageUI *>(pSetWnd->GetCurrentSubPage());

            if (pSubFwdPageUI != NULL)
            {
                return pSubFwdPageUI->OnSyncResult(msg);
            }
        }
    }

    return 0;
}

int CDlgBaseFwdPageUI::GetMsgBoxTimer()
{
#ifdef IF_FEATURE_FORWARD_OPT
    return m_iLineID == GLOAD_ID ? 2000 : FWD_PAGE_SAVE_POP_TIMER;
#else
    return FWD_PAGE_SAVE_POP_TIMER;
#endif
};

CDlgBaseFwdPageUI::CDlgBaseFwdPageUI(FWD_TYPE iType, QWidget * parent/* = 0*/)
    : CDlgBaseSubPage(parent)
    , m_iLineID(-1), m_eFwdType(iType)
{
}

void CDlgBaseFwdPageUI::OnBtnBackClick()
{
    emit BtnBackClick();
}

void CDlgBaseFwdPageUI::BeforeSetSubPage(void * pData)
{
    if (NULL != pData)
    {
        m_iLineID = *((int *)pData);
    }

    SETTINGUI_INFO("CDlgBaseFwdPageUI::BeforeSetSubPage data=[%p] account=[%d]", pData, m_iLineID);
}

bool CDlgBaseFwdPageUI::LoadData()
{
    return (g_pCallForwardController->GetForwardDataByType(&m_FwdData, (FWD_TYPE)m_eFwdType,
            m_iLineID));
}

void CDlgBaseFwdPageUI::ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox)
{
#ifdef IF_FEATURE_FORWARD_OPT
    if (pMessageBox != NULL && FWD_PAGE_SAVE_POP_ID == pMessageBox->GetID())
    {
        if (MessageBox_Close != pMessageBox->GetResult())
        {
            OnBtnBackClick();
        }
    }
#endif
}

bool CDlgBaseFwdPageUI::OnAction(const QString & strAction)
{
#ifdef IF_FEATURE_FORWARD_OPT
    if (strAction == ACTION_SAVE_DATA)
    {
        return OnBtnSaveClick();
    }
#endif

    return CDlgBaseSubPage::OnAction(strAction);
}

bool CDlgBaseFwdPageUI::SavePageData()
{
    if (IsStatusChanged())
    {
        /*
        将界面显示值存入pData
        利用Controller将pData值存入BusyForward界面的配置中
        */
        ForwardData objData;
        GetPageData(&objData, GetUIType());

        if (g_pCallForwardController->SetForwardDataByType(FWD_TYPE(m_eFwdType), objData, m_iLineID))
        {
            return true;
        }
        else
        {
            //保存失败
            MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_SAVE_CONFIG_FAILED_PLEASE_RETRY),
                                        MESSAGEBOX_NOTE, SHORT_TIME);
        }
    }
    else
    {
        return true;
    }

    return false;
}

bool CDlgBaseFwdPageUI::OnBtnSaveClick()
{
    if (GetSubPageEnabled())
    {
        //设置处于预输入状态的字符有效
        ReturnAllEdit();

        // 进行数据有效性验证
        InputDataErrorInfo objErrorInfo;

        if (IsInputDataValid(objErrorInfo))
        {
            //保存成功时才弹出相应提示并进行后续操作
            //子页面return false表示要自己处理保存后的操作
            if (SavePageData())
            {
                if (SaveCompleted())
                {
                    //弹出保存成功的提示框
                    MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_SAVING_CONFIG_FILE),
                                                MESSAGEBOX_NOTE, GetMsgBoxTimer(), "", "", FWD_PAGE_SAVE_POP_ID);
                }
                else
                {
                    //回到上一层界面
                    OnBtnBackClick();
                }
            }
        }
        else
        {
            //先设置焦点，再弹出提示框
            if (NULL != objErrorInfo.pLineEdit)
            {
                SetErrorFocus(objErrorInfo.pLineEdit);
            }

            //弹出错误提示
            MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(objErrorInfo.strErrorContent), MESSAGEBOX_NOTE,
                                        1000);
        }

        return true; // 数据错误需要重新填写
    }

    return false;
}

bool CDlgBaseFwdPageUI::OnSyncResult(msgObject & msg)
{
#ifdef IF_FEATURE_FORWARD_OPT
    int iType = msg.wParam;
    int iLine = msg.lParam;

    SETTINGUI_INFO("OnSyncResult type=[%d][%d] acc=[%d][%d]", m_eFwdType, iType, iLine, m_iLineID);

    if (iLine != m_iLineID && m_iLineID != GLOAD_ID)
    {
        return FALSE;
    }

    if (iType & m_eFwdType)
    {
        ForwardData objData;
        if (GetPageData(&objData, (FWD_TYPE)iType)
                && IsForwardSyncFailed(objData.toLogicElem(), iLine, (FWD_TYPE)iType))
        {
            MessageBox_RemoveTopMessageBox(MessageBox_Close);
            //弹出保存成功的提示框
            MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_SYNC_FAILED),
                                        MESSAGEBOX_NOTE, 2000, "", "", FWD_PAGE_SAVE_POP_ID);
        }
        else if (m_iLineID != GLOAD_ID)
        {
            MessageBox_RemoveTopMessageBox();
        }

        return true;
    }
#endif

    return false;
}

void CDlgBaseFwdPageUI::Init()
{
#ifdef IF_FEATURE_FORWARD_OPT
    SingleMsgReg(FWD_DND_SYNC_RESULT, CDlgBaseFwdPageUI::OnMessage);
#endif
}

void CDlgBaseFwdPageUI::Uninit()
{
#ifdef IF_FEATURE_FORWARD_OPT
    etl_UnregisterMsgHandle(FWD_DND_SYNC_RESULT, FWD_DND_SYNC_RESULT, CDlgBaseFwdPageUI::OnMessage);
#endif
}

bool CDlgBaseFwdPageUI::IsStatusChanged()
{
#ifdef IF_FEATURE_FORWARD_OPT
    return GLOAD_ID == m_iLineID;
#else
    return false;
#endif
}

bool CDlgBaseFwdPageUI::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
#ifdef IF_FEATURE_FORWARD_OPT
    return true;
#else
    if (m_pGeneralFrame == NULL)
    {
        return false;
    }

    if (FWDDND_MODE_ACCOUNT == fwd_GetMode())
    {
        objWndData[0].m_strSoftkeyAction = ACTION_COPY_TO_ALL_LINES;
        objWndData[0].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_ALL_LINES);
        objWndData[0].m_strIconPath = PIC_SOFTKEY_FWD_ALLLINES;
    }

    return true;
#endif
}