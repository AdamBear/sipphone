#include "cdlgbsftcallwaiting.h"
#include "messagebox/modmessagebox.h"
#include "uikernel/languagehelper.h"
#include "qtcommon/qmisc.h"
#include "inputmethod/inputmethod.h"
#include "baseui/fontcolor.h"
#include "account/include/modaccount.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/xsiactionhelper/include/xsicommon.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "uimanager/moduimanager.h"
#include "modsettingui.h"
#include "uikernel/qcheckbox.h"
#include "settinguicommon.h"

#ifdef IF_FEATURE_BROADSOFT_CALL_WAITING
namespace
{
#define MSG_ID_LOADING 100
}

static LRESULT OnCallWaitingMsg(msgObject & objMessage)
{
    SETTINGUI_INFO("OnCallWaitingMsg [%d] [%d]",
                   objMessage.message, objMessage.wParam);

    CDlgBSFTCallWaiting * pARNew = qobject_cast<CDlgBSFTCallWaiting *>(UIManager_GetPrivateDlg(
                                       DLG_CDlgBSFTCallWaiting));
    if (NULL != pARNew || pARNew->IsTopPage())
    {
        pARNew->OnReceiveMsg(objMessage);
    }

    return 0;
}

CDlgBSFTCallWaiting::CDlgBSFTCallWaiting(QWidget * parent/* = 0*/)
    : CDlgBaseSubPage(parent)
    , m_pCheckboxARStatus(NULL)
    , m_iAccountId(-1)
{
    InitData();
}

CDlgBSFTCallWaiting::~CDlgBSFTCallWaiting()
{
}

void CDlgBSFTCallWaiting::Init()
{
    etl_RegisterMsgHandle(XSI_MESSAGE_BEGIN,
                          XSI_MESSAGE_END, OnCallWaitingMsg);
}

void CDlgBSFTCallWaiting::Uninit()
{
    etl_UnregisterMsgHandle(XSI_MESSAGE_BEGIN,
                            XSI_MESSAGE_END, OnCallWaitingMsg);
}

void CDlgBSFTCallWaiting::InitData()
{
    m_iAccountId = acc_GetDefaultAccount();
    // 设置标题
    m_strTitle = LANG_TRANSLATE(TRID_CALL_WAITING);

    //初始化 下面会改变父窗口，这里用this初始化是为了防止内存泄漏问题
    m_pCheckboxARStatus = new qCheckBox(this);

    //指针判空
    if (m_pCheckboxARStatus == NULL)
    {
        return ;
    }

    //安装事件过滤器
    m_pCheckboxARStatus->installEventFilter(this);
    m_pCheckboxARStatus->hide();

    //为界面自己安装事件过滤器
    this->installEventFilter(this);
}

//处理XSI密码
void CDlgBSFTCallWaiting::OnSubmitXSIPassword(const yl::string & strPswd)
{
    BWCallControl_SetAccountXSIPassword(strPswd.c_str(), m_iAccountId);
    ExtraDialogInitData();
}

void CDlgBSFTCallWaiting::OnCancelInputXSIPassword()
{
    //回到上一层界面
    emit BtnBackClick();
}

//收到logic层返回的消息
void CDlgBSFTCallWaiting::OnReceiveMsg(msgObject & objMessage)
{
    switch (objMessage.message)
    {
    case XSI_MESSAGE_REQ_FINISH:
        {
            if (XSI_CALL_WAITING_GET == objMessage.wParam)
            {
                MessageBox_RemoveTopMessageBox(MessageBox_OK);

                m_pCheckboxARStatus->SetData(CW_GetCallWaitingState(m_iAccountId));
            }
            else if (XSI_CALL_WAITING_PUT == objMessage.wParam)
            {
                MessageBox_RemoveTopMessageBox(MessageBox_OK);

                emit BtnBackClick();
            }
        }
        break;
    case XSI_MESSAGE_REQ_FAIL:
        {
            if (XSI_CALL_WAITING_GET != objMessage.wParam
                    && XSI_CALL_WAITING_PUT != objMessage.wParam)
            {
                return ;
            }

            MessageBox_RemoveTopMessageBox(MessageBox_Destory);

            SXsiExtenInfo * pErrInfo = (SXsiExtenInfo *)objMessage.GetExtraData();

            if (NULL == pErrInfo)
            {
                return ;
            }

            if (pErrInfo->m_nError == 401)
            {
                ProcessMsgBoxPassword(objMessage.lParam);
            }
            else
            {
                if (m_bIsInLoad)
                {
                    MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_LOAD_FAILED),
                                                MESSAGEBOX_NOTE, SHORT_TIME, "", "", MSG_ID_SHOW_NOTE_BEFORE_BACK);
                }
                else
                {
                    //回到上一层界面
                    MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_SAVE_FAILED),
                                                MESSAGEBOX_NOTE, SHORT_TIME, "", "", MSG_ID_SHOW_NOTE_BEFORE_BACK);
                }
            }
        }
        break;
    default:
        break;
    }
}

bool CDlgBSFTCallWaiting::IsStatusChanged()
{
    return false;
}

bool CDlgBSFTCallWaiting::LoadPageData()
{
    m_vecWidgetPair.clear();

    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_STATUS);
    qPair.first = strLab;
    qPair.second = m_pCheckboxARStatus;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);

    return true;
}

bool CDlgBSFTCallWaiting::SavePageData()
{
    m_bIsInLoad = false;

    bool bStatus  = m_pCheckboxARStatus->GetData();

    CW_SetCallWaitingState(bStatus, m_iAccountId);

    MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_SAVING_PLEASE_WAIT), MESSAGEBOX_NOTE);
    return false;
}

// 设置子页面前的操作
void CDlgBSFTCallWaiting::BeforeSetSubPage(void * pData)
{
    if (NULL != pData)
    {
        m_iAccountId = *(int *)pData;
    }
    else
    {
        m_iAccountId = acc_GetDefaultAccount();
    }
}

void CDlgBSFTCallWaiting::ExtraDialogInitData()
{
    //表示处于加载中
    m_bIsInLoad = true;
    if (BWCallControl_IsAccountXSIEnable(m_iAccountId))
    {
        if (ProcessMsgBoxPassword(m_iAccountId))
        {
            return ;
        }

        CW_QueryCallWaiting(m_iAccountId);
    }
    else
    {
        m_bIsInLoad = false;
    }

    if (!m_bIsInLoad)
    {
        // 如果信息不正确，则退出
        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_XSI_INFORMATION_IS_INCOMPLETE),
                                    MESSAGEBOX_NOTE, 1500, "", "", MSG_ID_SHOW_NOTE_BEFORE_BACK);
    }
    else
    {
        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_LOADING_PLEASE_WAIT),
                                    MESSAGEBOX_Cancel, -1, "", "", MSG_ID_LOADING);
    }
}

void CDlgBSFTCallWaiting::ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    int iMsgID = pMessageBox->GetID();
    int iRet = pMessageBox->GetResult();

    switch (iMsgID)
    {
    case MSG_ID_LOADING:
        {
            if (iRet == MessageBox_Cancel)
            {
                CW_CancelRequest();
            }
        }
        break;;
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

// 处理密码弹窗
bool CDlgBSFTCallWaiting::ProcessMsgBoxPassword(int nLineId)
{
    if (XSI_SIPACCOUNT_NOT_AVAILABLE == CBaseHelper::GetXSIStatus(nLineId))
    {
        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_SETTUI_TIP_SIP_XSI_AUTHENTICATION_FAILED),
                                    MESSAGEBOX_NOTE, 1500, "", "", MSG_ID_SHOW_NOTE_BEFORE_BACK);

        return true;
    }
    else if (XSI_AUTH_ERROR == CBaseHelper::GetXSIStatus(nLineId))
    {
        SettingUI_PopPasswordDlg(this, nLineId);
        return true;
    }
    else
    {
        return false;
    }
}
#endif
