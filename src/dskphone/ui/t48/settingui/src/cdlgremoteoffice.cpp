#include "model.h"
#include "cdlgremoteoffice.h"
#include "messagebox/modmessagebox.h"

#include "uikernel/languagehelper.h"
#include "qtcommon/qmisc.h"
#include "setting/include/common.h"
#include "inputmethod/inputmethod.h"
#include "account/include/modaccount.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "uikernel/qlineeditutility.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/xsiactionhelper/include/xsicommon.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "modsettingui.h"
#include "uikernel/qcheckbox.h"
#include "settinguicommon.h"

namespace
{
//设定edit控件输入长度的最大值
#define EDIT_MAX_LENGTH  30
#define MSG_ID_LOADING  100
}

static LRESULT OnRemoteOfficeMsg(msgObject & objMessage)
{
    SETTINGUI_INFO("OnRemoteOfficeMsg [%d] [%d]",
                   objMessage.message, objMessage.wParam);

    CDlgRemoteOfficeNew * pRONew = qobject_cast<CDlgRemoteOfficeNew *>(UIManager_GetPrivateDlg(
                                       DLG_CDlgRemoteOfficeNew));
    if (NULL == pRONew || !pRONew->IsTopPage())
    {
        return 0;
    }

    pRONew->OnReceiveMsg(objMessage);
    return 0;
}


CDlgRemoteOfficeNew::CDlgRemoteOfficeNew(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pCheckStatus(NULL)
    , m_pEditNumber(NULL)
    , m_iAccountId(-1)
{
    InitData();
}

CDlgRemoteOfficeNew::~CDlgRemoteOfficeNew()
{
}

void CDlgRemoteOfficeNew::Init()
{
    etl_RegisterMsgHandle(XSI_MESSAGE_BEGIN,
                          XSI_MESSAGE_END, OnRemoteOfficeMsg);
}

void CDlgRemoteOfficeNew::Uninit()
{
    etl_UnregisterMsgHandle(XSI_MESSAGE_BEGIN,
                            XSI_MESSAGE_END, OnRemoteOfficeMsg);
}

void CDlgRemoteOfficeNew::InitData()
{
    m_iAccountId = acc_GetDefaultAccount();
    // 设置标题
    m_strTitle = LANG_TRANSLATE(TRID_REMOTE_OFFICE).toUtf8().data();

    //初始化 下面会改变父窗口，这里用this初始化是为了防止内存泄漏问题
    m_pCheckStatus = new qCheckBox(this);
    m_pEditNumber = new QLineEdit(this);

    //指针判空
    if (m_pCheckStatus == NULL
            || m_pEditNumber == NULL)
    {
        return;
    }

    qLineEditUtility::SetLineEditStyle(m_pEditNumber, Qt::black);

    m_pCheckStatus->SetData(false);

    //设置输入框输入的最大长度
    m_pEditNumber->setMaxLength(EDIT_MAX_LENGTH);

    //为输入框安装输入法
    qInstallIME(m_pEditNumber, IME_DIAL);

    //设置Activation标签
    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_STATUS);
    qPair.first = strLab;
    qPair.second = m_pCheckStatus;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_PHONE_NUMBER);
    qPair.first = strLab;
    qPair.second = m_pEditNumber;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);

    //安装事件过滤器
    m_pCheckStatus->installEventFilter(this);
    m_pEditNumber->installEventFilter(this);

    //为界面自己安装事件过滤器
    this->installEventFilter(this);


}


//处理XSI密码
void CDlgRemoteOfficeNew::OnSubmitXSIPassword(const yl::string & strPswd)
{
    SETTINGUI_INFO("CDlgRemoteOfficeNew::OnSubmitXSIPassword, account %d", m_iAccountId);
    BWCallControl_SetAccountXSIPassword(strPswd.c_str(), m_iAccountId);
    ExtraDialogInitData();
}

void CDlgRemoteOfficeNew::OnCancelInputXSIPassword()
{
    //回到上一层界面
    emit BtnBackClick();
}

//收到logic层返回的消息
void CDlgRemoteOfficeNew::OnReceiveMsg(msgObject & objMessage)
{
    switch (objMessage.message)
    {
    case XSI_MESSAGE_REQ_FINISH:
        {
            if (XSI_REMOTEOFFICE_GET == objMessage.wParam)
            {
                MessageBox_RemoveTopMessageBox(MessageBox_Destory);

                GetRemoteOfficeData();
            }
            else if (XSI_REMOTEOFFICE_PUT == objMessage.wParam)
            {
                MessageBox_RemoveTopMessageBox(MessageBox_OK);

                emit BtnBackClick();
            }
        }
        break;
    case XSI_MESSAGE_REQ_FAIL:
        {
            if (XSI_REMOTEOFFICE_GET != objMessage.wParam
                    && XSI_REMOTEOFFICE_PUT != objMessage.wParam)
            {
                return ;
            }

            MessageBox_RemoveTopMessageBox(MessageBox_Destory);

            SETTINGUI_INFO("RO_MESSAGE_REQUEST_FAILED wParam [%d], lParam [%d], m_iAccountId [%d]",
                           objMessage.wParam, objMessage.lParam, m_iAccountId);

            SXsiExtenInfo * pErrInfo = (SXsiExtenInfo *)objMessage.GetExtraData();

            if (NULL == pErrInfo)
            {
                return ;
            }

            if (pErrInfo->m_nError == 401)
            {
                if (m_iAccountId != objMessage.lParam)
                {
                    SETTING_INFO("RO Account do NOT match!!");
                }

                ProcessMsgBoxPassword(m_iAccountId);
            }
            else
            {
                if (m_bIsInLoad)
                {
                    MessageBox_ShowCommonMsgBox(this,
                                                LANG_TRANSLATE(TRID_LOAD_FAILED),
                                                MESSAGEBOX_NOTE, SHORT_TIME, "", "",
                                                MSG_ID_SHOW_NOTE_BEFORE_BACK);
                }
                else
                {
                    // 提示后回退界面
                    MessageBox_ShowCommonMsgBox(this,
                                                LANG_TRANSLATE(TRID_SAVE_FAILED),
                                                MESSAGEBOX_NOTE, SHORT_TIME, "", "",
                                                MSG_ID_SHOW_NOTE_BEFORE_BACK);
                }
            }
        }
        break;
    default:
        break;
    }
}

//获取RemoteOffice数据
void CDlgRemoteOfficeNew::GetRemoteOfficeData()
{
    SRemoteOffice roInfo;
    RO_GetRemoteOfficeInfo(roInfo, m_iAccountId);

    //指针判空
    if (m_pCheckStatus == NULL
            || m_pEditNumber == NULL)
    {
        return;
    }

    m_pCheckStatus->SetData(roInfo.bActive);
    m_pEditNumber->setText(toQString(roInfo.strPhoneNumber));
}

bool CDlgRemoteOfficeNew::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    //指针判空
    if (m_pCheckStatus == NULL
            || m_pEditNumber == NULL)
    {
        return false;
    }

    bool bStatus = m_pCheckStatus->GetData();
    QString strNumber = m_pEditNumber->text();

    //判断Number是否合法
    if (!BWCallControl_CheckPhoneNumberValid(strNumber.toUtf8().data())
            && !(!bStatus && strNumber.isEmpty()))
    {
        objErrorContent.strErrorContent = LANG_TRANSLATE(TRID_INVALID_NUMBER);
        objErrorContent.pLineEdit = m_pEditNumber;
        return false;
    }

    return true;
}


bool CDlgRemoteOfficeNew::IsStatusChanged()
{
    SRemoteOffice roInfo;
    RO_GetRemoteOfficeInfo(roInfo, m_iAccountId);

    //指针判空
    if (m_pCheckStatus == NULL
            || m_pEditNumber == NULL)
    {
        return false;
    }

    if (m_pCheckStatus->GetData() != roInfo.bActive
            || m_pEditNumber->text() != toQString(roInfo.strPhoneNumber))
    {
        return true;
    }

    return false;
}

bool CDlgRemoteOfficeNew::LoadPageData()
{
    return true;
}

bool CDlgRemoteOfficeNew::SavePageData()
{
    m_bIsInLoad = false;

    bool bStatus  = m_pCheckStatus->GetData();
    RO_SetRemoteOffice(bStatus, m_pEditNumber->text().toUtf8().data(), m_iAccountId);
    MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_SAVING_PLEASE_WAIT), MESSAGEBOX_Cancel);
    return false;
}

// 设置子页面前的操作
void CDlgRemoteOfficeNew::BeforeSetSubPage(void * pData)
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

void CDlgRemoteOfficeNew::ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    int iMsgID = pMessageBox->GetID();
    int iRet = pMessageBox->GetResult();

    switch (iMsgID)
    {
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

    if (pMessageBox->GetType() == MESSAGEBOX_Cancel
            && pMessageBox->GetResult() == MessageBox_Cancel)
    {
        RO_CancelRequest();
    }
}

void CDlgRemoteOfficeNew::ExtraDialogInitData()
{
    //表示处于加载中
    m_bIsInLoad = true;
    if (BWCallControl_IsAccountXSIEnable(m_iAccountId))
    {
        if (ProcessMsgBoxPassword(m_iAccountId))
        {
            return;
        }

        RO_QueryRemoteOffice(m_iAccountId);
    }
    else
    {
        m_bIsInLoad = false;
    }

    if (!m_bIsInLoad)
    {
        // 如果信息不正确，则退出
        MessageBox_ShowCommonMsgBox(this,
                                    LANG_TRANSLATE(TRID_XSI_INFORMATION_IS_INCOMPLETE),
                                    MESSAGEBOX_NOTE, SHORT_TIME, "", "",
                                    MSG_ID_SHOW_NOTE_BEFORE_BACK);
    }
    else
    {
        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_LOADING_PLEASE_WAIT),
                                    MESSAGEBOX_Cancel, -1, "", "", MSG_ID_LOADING);
    }
}

// 处理密码弹窗
bool CDlgRemoteOfficeNew::ProcessMsgBoxPassword(int nLineId)
{
    if (XSI_SIPACCOUNT_NOT_AVAILABLE == CBaseHelper::GetXSIStatus(nLineId))
    {
        MessageBox_ShowCommonMsgBox(this,
                                    LANG_TRANSLATE(TRID_SETTUI_TIP_SIP_XSI_AUTHENTICATION_FAILED),
                                    MESSAGEBOX_NOTE, 1500, "", "",
                                    MSG_ID_SHOW_NOTE_BEFORE_BACK);

        return true;
    }
    else if (XSI_AUTH_ERROR == CBaseHelper::GetXSIStatus(nLineId))
    {
        SETTINGUI_INFO("CDlgRemoteOfficeNew::ProcessMsgBoxPassword, XSI_AUTH_ERROR");
        SettingUI_PopPasswordDlg(this, nLineId);
        return true;
    }
    else
    {
        //SETTINGUI_INFO("CDlgRemoteOfficeNew::ProcessMsgBoxPassword, xsi status [%d]", CBaseHelper::GetXSIStatus(nLineId));
        return false;
    }
}

