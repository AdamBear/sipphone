#include "cexecutiveassistantluilogic.h"
#include "cdlgexeassisuibase.h"
#include "uimanager/moduimanager.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "uikernel/languagehelper.h"
#include "settingui/include/modsettingui.h"
#include "uimanager/uimanager_common.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/xsiactionhelper/include/xsicommon.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "modsettingui.h"
#include "messagebox/modmessagebox.h"
#include "settinguicommon.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include <QtGui/QDialog>
#include "settinguicommon.h"
#include "uicommon/globalinputhook/modinputhook.h"

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT

static const int MSGBOX_ID_QUERY(100);   //查询
static const int MSGBOX_ID_SAVE(101);    //保存
static const int MSGBOX_ID_SAVE_FAILED(102);    //保存失败
static const int MSGBOX_ID_XSI_INFO(103);    //XSI信息不完整
static const int MSGBOX_ID_XSI_SIP_ERROR(104);    //SIP鉴权失败
static const int MSGBOX_ID_NOT_ALLOW_OPTOUT(105);    //不允许OPT OUT
static const int MSGBOX_ID_NO_CHANGED(106);    //没有改变

static const int ishow_note_timer = 1500;

#define LOG_TRACE \
    SETTINGUI_INFO("CExecutiveAssistantUILogic::Trace fun=[%s] no=[%d] account=[%d] user=[%d] ui=[%d]", \
    __FUNCTION__, __LINE__, m_iLineId, m_eUserType, m_eUIType);

static LRESULT OnMessage(msgObject & objMessage)
{
    SETTINGUI_INFO("CExecutiveAssistantUILogic::OnMessage msg=[%#x][%d] w=[%d] l=[%d]",
                   objMessage.message, objMessage.message, objMessage.wParam, objMessage.lParam);

    g_refExeAssisUILogic.OnReceiveMsg(objMessage);
    return 0;
}

static bool ExecutiveAssistant_KeypadHookCallbackFun(int nKeyCode, bool bPress)
{
    if (bPress)
    {
        return g_refExeAssisUILogic.OnKeyPress(nKeyCode, bPress);
    }

    return false;
}

CExecutiveAssistantUILogic & CExecutiveAssistantUILogic::GetInstance()
{
    static CExecutiveAssistantUILogic spIns;
    return spIns;
}


CExecutiveAssistantUILogic::CExecutiveAssistantUILogic()
    : QObject(),
      m_iLineId(0), m_eUserType(EET_None), m_eUIType(UI_NONE), m_bQuitPage(true)
{
    // 注册按键回调函数
    inputHook_RegisterKeyPadHookProc(ExecutiveAssistant_KeypadHookCallbackFun);

    etl_RegisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END, OnMessage);
}

CExecutiveAssistantUILogic::~CExecutiveAssistantUILogic()
{
    inputHook_UnRegisterKeyPadHookProc(ExecutiveAssistant_KeypadHookCallbackFun);

    etl_UnregisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END, OnMessage);
}

bool CExecutiveAssistantUILogic::OnReceiveMsg(msgObject & objMessage)
{
    CDlgExeAssisUIBase * pPageUI = GetUI();
    if (pPageUI == NULL || !pPageUI->IsTopPage())
    {
        return false;
    }

    LOG_TRACE;

    switch (objMessage.message)
    {
    case XSI_MESSAGE_REQ_FINISH:
        {
            if (objMessage.wParam >= XSI_EXECUTIVE_ASSISTANTLIST_GET
                    && objMessage.wParam <= XSI_ASSISTANT_EXECUTIVELIST_GET)
            {
                MessageBox_RemoveTopMessageBox(MessageBox_Destory);

                pPageUI->SetCurrentIndex(0);
                pPageUI->setFocus();
                pPageUI->RefreshData();
                pPageUI->UpdateWnd();
            }
            else if (objMessage.wParam >= XSI_EXECUTIVE_ASSISTANTLIST_PUT
                     && objMessage.wParam <= XSI_ASSISTANT_EXECUTIVELIST_PUT)
            {
                if (objMessage.wParam == XSI_ASSISTANT_EXECUTIVELIST_PUT && m_eUIType == UI_ListView)
                {
                    // Push Executive List 页面比较特殊，因为分片的原因，不能退出页面，但是需要刷新数据，确保焦点不丢失
                    int iIndex = pPageUI->GetCurrentIndex();
                    pPageUI->RefreshData();
                    pPageUI->SetCurrentIndex(iIndex);
                }
                else
                {
                    pPageUI->QuitPage();
                }

                MessageBox_RemoveTopMessageBox(MessageBox_OK);
            }
        }
        break;
    case XSI_MESSAGE_REQ_FAIL:
        {
            m_bQuitPage = false;

            MessageBox_RemoveTopMessageBox(MessageBox_Destory);

            SXsiExtenInfo * pErrInfo = (SXsiExtenInfo *)objMessage.GetExtraData();
            if (NULL != pErrInfo)
            {
                SETTINGUI_INFO("CExecutiveAssistantUILogic::ResponseActionError line=[%d] code=[%d] reason=[%s] index=[%d]",
                               objMessage.lParam, pErrInfo->m_nError, pErrInfo->m_strErrInfo.c_str(), pErrInfo->m_nEntenParm);

                if (ResponseActionError(objMessage.lParam, pErrInfo->m_nError, pPageUI->IsInLoading(),
                                        pErrInfo->m_nEntenParm))
                {
                    return true;
                }
            }

            if (pPageUI->IsInLoading())
            {
                pPageUI->QuitPage();

                MessageBox_ShowCommonMsgBox(NULL,
                                            LANG_TRANSLATE(TRID_LOAD_FAILED), MESSAGEBOX_NOTE, ishow_note_timer);
            }
            else
            {
                MessageBox_ShowCommonMsgBox(this,
                                            LANG_TRANSLATE(TRID_FAILED_PLEASE_RETRY), MESSAGEBOX_NOTE, ishow_note_timer, "", "",
                                            MSGBOX_ID_SAVE_FAILED);
            }
        }
        break;
    case XSI_MESSAGE_FINISH_ALL:
        {
            if (m_bQuitPage && !pPageUI->IsInLoading())
            {
                MessageBox_RemoveTopMessageBox(MessageBox_Destory);

                pPageUI->QuitPage();
            }
        }
        break;
    default:
        return false;
        break;
    }
    return true;
}

CDlgExeAssisUIBase * CExecutiveAssistantUILogic::GetUI()
{
    CDlgExeAssisUIBase * pExeAssiaPageUI = NULL;
    switch (m_eUIType)
    {
    case UI_Common:
        {
            pExeAssiaPageUI = qobject_cast<CDlgExeAssisUIBase *>(UIManager_GetPrivateDlg(
                                  DLG_CDlgExeAssisCommon));
        }
        break;
    case UI_Filter:
        {
            pExeAssiaPageUI = qobject_cast<CDlgExeAssisUIBase *>(UIManager_GetPrivateDlg(
                                  DLG_CDlgExeAssisFilter));
        }
        break;
    case UI_Screen:
        {
            pExeAssiaPageUI = qobject_cast<CDlgExeAssisUIBase *>(UIManager_GetPrivateDlg(
                                  DLG_CDlgExeAssisScreen));
        }
        break;
    case UI_Alter:
        {
            pExeAssiaPageUI = qobject_cast<CDlgExeAssisUIBase *>(UIManager_GetPrivateDlg(
                                  DLG_CDlgExeAssisAlter));
        }
        break;
    case UI_ListView:
        {
            pExeAssiaPageUI = qobject_cast<CDlgExeAssisUIBase *>(UIManager_GetPrivateDlg(
                                  DLG_CDlgExeAssisListView));
        }
        break;
    case UI_Details:
        {
            //pExeAssiaPageUI = qobject_cast<CDlgExeAssisUIBase*>(UIManager_GetPrivateDlg(DLG_CDlgExeAssisDetails));
        }
        break;
    default:
        break;
    }

    return pExeAssiaPageUI;
}

void CExecutiveAssistantUILogic::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    SETTINGUI_INFO("CExecutiveAssistantUILogic::MessageBoxCallBack id[%d] ret=[%d]",
                   pMessageBox->GetID(), pMessageBox->GetResult());

    int iRet = pMessageBox->GetResult();
    switch (pMessageBox->GetID())
    {
    case MSGBOX_ID_QUERY:
    case MSGBOX_ID_SAVE:
        {
            if (iRet == MessageBox_Cancel)
            {
                BWExeAssis_CancelAllRequest();
                CDlgExeAssisUIBase * pPageUI = GetUI();
                if (NULL != pPageUI)
                {
                    pPageUI->QuitPage();
                }
            }
        }
        break;
    case MSGBOX_ID_XSI_INFO:
    case MSGBOX_ID_XSI_SIP_ERROR:
    case MSGBOX_ID_NO_CHANGED:
        {
            CDlgExeAssisUIBase * pPageUI = GetUI();
            if (NULL != pPageUI)
            {
                pPageUI->QuitPage();
            }
        }
        break;
    case MSGBOX_ID_POP_XSI_PWD:
        {
            if (iRet == MessageBox_OK)
            {
                yl::string strPwd = pMessageBox->GetExtraData().toString().toUtf8().data();
                OnSubmitXSIPassword(strPwd);
            }
            else if (iRet == MessageBox_Cancel)
            {
                OnCancelInputXSIPassword();
            }
        }
        break;
    default:
        break;
    }
}

bool CExecutiveAssistantUILogic::SaveToLogic(CDlgExeAssisUIBase * pPageUi)
{
    if (pPageUi == NULL)
    {
        return false;
    }

    pPageUi->SetLoading(false);
    m_bQuitPage = true;

    CExeAssisInspector * pData = pPageUi->GetPageData();
    if (pData == NULL)
    {
        return false;
    }

    //http://10.2.1.199/Bug.php?BugID=92215 无论信息是否变化都可以保存
    ExeAssis_Action eAction = (m_eUIType == UI_ListView
                               && IsAssistant()) ? EAA_AssisCommon : EAA_AutoSelect;
    bool bSaved = BWExeAssis_SetData(pData, m_iLineId, eAction);

    MessageBox_ShowCommonMsgBox(this,
                                LANG_TRANSLATE(TRID_SAVING_PLEASE_WAIT), MESSAGEBOX_Cancel,
                                bSaved ?  -1 : ishow_note_timer, "", "", bSaved ? MSGBOX_ID_SAVE : MSGBOX_ID_NO_CHANGED);

    return true;
}

bool CExecutiveAssistantUILogic::GetLogicData(CExeAssisInspector * pInfo)
{
    if (pInfo == NULL)
    {
        return false;
    }

    return BWExeAssis_GetData(pInfo, m_iLineId);
}

void CExecutiveAssistantUILogic::OnUIShow(CDlgExeAssisUIBase * pPageUi)
{
    LOG_TRACE;

    if (pPageUi == NULL)
    {
        return;
    }

    if (UI_Details == m_eUIType)
    {
        return;
    }

    if (BWExeAssis_GetAccountCountByType(m_eUserType) <= 0)
    {
        return;
    }

    if (ProcessMsgBoxPassword(m_iLineId))
    {
        return;
    }

    bool bXSIInfo = BWCallControl_IsAccountXSIEnable(m_iLineId);
    if (bXSIInfo)
    {
        bXSIInfo &= BWExeAssis_QueryAction(pPageUi->GetAction(), m_iLineId);

        if (bXSIInfo)
        {
            MessageBox_ShowCommonMsgBox(this,
                                        LANG_TRANSLATE(TRID_LOADING_PLEASE_WAIT), MESSAGEBOX_Cancel, -1, "", "", MSGBOX_ID_QUERY);
        }
        else
        {
            MessageBox_ShowCommonMsgBox(this,
                                        LANG_TRANSLATE(TRID_LOAD_FAILED), MESSAGEBOX_Cancel, ishow_note_timer, "", "", MSGBOX_ID_XSI_INFO);
        }
    }
    else
    {
        // 如果所有的账号都下载失败，则退出
        MessageBox_ShowCommonMsgBox(this,
                                    LANG_TRANSLATE(TRID_XSI_INFORMATION_IS_INCOMPLETE), MESSAGEBOX_NOTE, ishow_note_timer, "", "",
                                    MSGBOX_ID_XSI_INFO);
    }

    pPageUi->SetLoading(bXSIInfo);
}

bool CExecutiveAssistantUILogic::ProcessMsgBoxPassword(int nLineId)
{
    if (XSI_SIPACCOUNT_NOT_AVAILABLE == CBaseHelper::GetXSIStatus(nLineId))
    {
        MessageBox_ShowCommonMsgBox(this,
                                    LANG_TRANSLATE(TRID_SETTUI_TIP_SIP_XSI_AUTHENTICATION_FAILED), MESSAGEBOX_NOTE, ishow_note_timer,
                                    "", "", MSGBOX_ID_XSI_SIP_ERROR);

        return true;
    }
    else if (XSI_AUTH_ERROR == CBaseHelper::GetXSIStatus(nLineId))
    {
        SettingUI_PopPasswordDlg(this, nLineId);
        return true;
    }

    LOG_TRACE;

    return false;
}

void CExecutiveAssistantUILogic::OnSubmitXSIPassword(const yl::string & strPswd)
{
    BWCallControl_SetAccountXSIPassword(strPswd.c_str(), m_iLineId);

    CDlgExeAssisUIBase * pPageUI = GetUI();
    if (NULL != pPageUI)
    {
        QTimer::singleShot(10, this, SLOT(OnRefreshAfterSubmitXSI()));
    }
}

void CExecutiveAssistantUILogic::OnRefreshAfterSubmitXSI()
{
    OnUIShow(GetUI());
}

void CExecutiveAssistantUILogic::OnCancelInputXSIPassword()
{
    CDlgExeAssisUIBase * pPageUI = GetUI();
    if (NULL != pPageUI)
    {
        SETTINGUI_INFO("OnCancelInputXSIPassword action=[%d]", pPageUI->GetAction());
        pPageUI->QuitPage();
    }
}

bool CExecutiveAssistantUILogic::ResponseActionError(int iLineId, int errCode, bool bInLoading,
        int iIndex)
{
    switch (errCode)
    {
    case 401://验证失败
        {
            return ProcessMsgBoxPassword(iLineId);
        }
        break;
    case 400:
        {
            if (m_eUIType == UI_ListView && IsAssistant() && !bInLoading)
            {
                MessageBox_ShowCommonMsgBox(this,
                                            LANG_TRANSLATE(TRID_NOT_ALLOW_TO_OPTIN_OUT), MESSAGEBOX_NOTE, ishow_note_timer, "", "",
                                            MSGBOX_ID_NOT_ALLOW_OPTOUT);

                CDlgExeAssisUIBase * pPageUI = GetUI();
                if (NULL != pPageUI)
                {
                    pPageUI->RefreshData();
                    pPageUI->SetCurrentIndex(iIndex);
                    pPageUI->UpdateWnd();
                }

                return true;
            }
        }
        break;
    default:
        break;
    }

    return false;
}

bool CExecutiveAssistantUILogic::OnKeyPress(int nKeyCode, bool bPress)
{
    CDlgExeAssisUIBase * pPageUI = GetUI();
    if (pPageUI == NULL || !pPageUI->IsTopPage())
    {
        return false;
    }

    return pPageUI->OnKeyPress(nKeyCode, bPress);
}

void CExecutiveAssistantUILogic::SetLineId(int iLineId)
{
    m_iLineId = iLineId;
    BWExeAssis_SetLineId(iLineId);
}
#endif

