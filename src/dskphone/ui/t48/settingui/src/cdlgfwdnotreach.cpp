#include "model.h"
#include "cdlgfwdnotreach.h"
#include "canywhereuilogic.h"
#include "messagebox/modmessagebox.h"

#include "uikernel/languagehelper.h"
#include "qtcommon/qmisc.h"
#include "setting/include/common.h"
#include "setting/src/callforwardcontroller.h"
#include "inputmethod/inputmethod.h"
#include "baseui/fontcolor.h"
#include "qtcommon/qmisc.h"
#include "account/include/modaccount.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "cdlgcallcontrol.h"
#include "modsettingui.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/xsiactionhelper/include/xsicommon.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "uikernel/qcomboboxutility.h"
#include "settinguicommon.h"

#include "uikernel/qcheckbox.h"

//设定edit控件输入长度的最大值
#define EDIT_MAX_LENGTH  30


static LRESULT OnFWDNotReachMsg(msgObject & objMessage)
{
    SETTINGUI_INFO("OnFWDNotReachMsg 0x[%x] 0x[%x]",
                   objMessage.message, objMessage.wParam);

    CDlgFWDNotReach * pCFNRNew = qobject_cast<CDlgFWDNotReach *>(UIManager_GetPrivateDlg(
                                     DLG_CDlgFWDNotReach));
    if (NULL == pCFNRNew || !pCFNRNew->IsTopPage())
    {
        return 0;
    }

    pCFNRNew->OnReceiveMsg(objMessage);
    return 0;
}


CDlgFWDNotReach::CDlgFWDNotReach(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pEditNumber(NULL)
    , m_pCheckboxCfnrStatus(NULL)
    , m_iAccountId(-1)
{
    InitData();
}

void CDlgFWDNotReach::InitData()
{
    // 设置标题
    m_strTitle = LANG_TRANSLATE(TRID_CALL_FORWARD_NOTE_REACHABLE).toUtf8().data();

    etl_RegisterMsgHandle(XSI_MESSAGE_BEGIN,
                          XSI_MESSAGE_END, OnFWDNotReachMsg);

    //初始化 下面会改变父窗口，这里用this初始化是为了防止内存泄漏问题
    m_pEditNumber = new QLineEdit(this);
    m_pCheckboxCfnrStatus = new qCheckBox(this);

    //指针判空
    if (m_pEditNumber == NULL
            || m_pCheckboxCfnrStatus == NULL)
    {
        return;
    }

    //qComboBoxUtility::SetComboBoxStyle(m_pComCfnrStatus);
    SetLineEditDefaultStyle(m_pEditNumber);

    //设置输入框输入的最大长度
    m_pEditNumber->setMaxLength(EDIT_MAX_LENGTH);

    //为输入框安装输入法
    qInstallIME(m_pEditNumber, IME_DIAL);

    /*
    设置标签上文字的大小及标签名
    LANG_TRANSLATE是翻译宏，为了方便语言的转化
    */
    //设置Activation标签
    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_STATUS);
    qPair.first = strLab;
    qPair.second = m_pCheckboxCfnrStatus;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_PHONE_NUMBER);
    qPair.first = strLab;
    qPair.second = m_pEditNumber;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);

    //安装事件过滤器
    m_pEditNumber->installEventFilter(this);
    m_pCheckboxCfnrStatus->installEventFilter(this);

    //为界面自己安装事件过滤器
    this->installEventFilter(this);
}

//处理XSI密码
void CDlgFWDNotReach::OnSubmitXSIPassword(const yl::string & strPswd)
{
    BWCallControl_SetAccountXSIPassword(strPswd.c_str(), m_iAccountId);
#if IF_BUG_22686
    QTimer::singleShot(10, this, SLOT(showEvent(NULL)));
#endif
}

void CDlgFWDNotReach::OnCancelInputXSIPassword()
{
    //回到上一层界面
    emit BtnBackClick();
}

//收到logic层返回的消息
void CDlgFWDNotReach::OnReceiveMsg(msgObject & objMessage)
{
    switch (objMessage.message)
    {
    case XSI_MESSAGE_REQ_FINISH:
        {
            if (XSI_CALLFORWARD_NOTREACHABLE_GET == objMessage.wParam)
            {
                MessageBox_RemoveTopMessageBox(MessageBox_Destory);

                GetFWDNotReachData();
            }
            else if (XSI_CALLFORWARD_NOTREACHABLE_PUT == objMessage.wParam)
            {
                MessageBox_RemoveTopMessageBox(MessageBox_OK);
                emit BtnBackClick();
            }
        }
        break;
    case XSI_MESSAGE_REQ_FAIL:
        {
            if (XSI_CALLFORWARD_NOTREACHABLE_GET != objMessage.wParam
                    && XSI_CALLFORWARD_NOTREACHABLE_PUT != objMessage.wParam)
            {
                return ;
            }

            MessageBox_RemoveTopMessageBox(MessageBox_Destory);
#if IF_BUG_22977
            SXsiExtenInfo * pErrInfo = (SXsiExtenInfo *)objMessage.GetExtraData();

            if (NULL == pErrInfo)
            {
                return ;
            }

            if (pErrInfo->m_nError == 401)
            {
                ProcessMsgBoxPassword(m_iAccountId);
            }
            else
#endif
            {
#if IF_BUG_22686
                if (m_bIsInLoad)
                {
                    MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_LOAD_FAILED), MESSAGEBOX_NOTE, 1500);
                }
                else
#endif  //  IF_BUG_22686
                {
                    MessageBox_ShowCommonMsgBox(this,
                                                LANG_TRANSLATE(TRID_SAVE_FAILED),
                                                MESSAGEBOX_NOTE, 1500, "", "",
                                                MSG_ID_SHOW_NOTE_BEFORE_BACK);
                }
            }

        }
        break;
    default:
        break;
    }
}

//获取FWDNotReach数据
void CDlgFWDNotReach::GetFWDNotReachData()
{
    SCallForwardNotReachable cfnrInfo;
    CFNR_GetForwardNotReachableInfo(cfnrInfo, m_iAccountId);

    m_pCheckboxCfnrStatus->SetData(cfnrInfo.bActive);

    m_pEditNumber->setText(toQString(cfnrInfo.strPhoneNumber));
}

bool CDlgFWDNotReach::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    if (m_pCheckboxCfnrStatus == NULL
            || m_pEditNumber == NULL)
    {
        return false;
    }

    bool bStatus = m_pCheckboxCfnrStatus->GetData();
    QString strNumber = m_pEditNumber->text();

    //判断Number是否合法
    if (!BWCallControl_CheckPhoneNumberValid(strNumber.toUtf8().data())
            && !(!bStatus && strNumber.isEmpty()))
    {
        objErrorContent.strErrorContent = TRID_INVALID_NUMBER;
        objErrorContent.pLineEdit = m_pEditNumber;
        m_pEditNumber->setFocus();
        return false;
    }

    return true;
}


bool CDlgFWDNotReach::IsStatusChanged()
{
    return false;
}

bool CDlgFWDNotReach::LoadPageData()
{
    //GetFWDNotReachData();
    return true;
}

bool CDlgFWDNotReach::SavePageData()
{
#if IF_BUG_22686
    m_bIsInLoad = false;
#endif  //  IF_BUG_22686

    bool bStatus  = m_pCheckboxCfnrStatus->GetData();
    CFNR_SetForwardNotReachable(bStatus, m_pEditNumber->text().toUtf8().data(), m_iAccountId);
    MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_SAVING_PLEASE_WAIT), MESSAGEBOX_Cancel);
    return false;
}

CDlgFWDNotReach::~CDlgFWDNotReach()
{
    etl_UnregisterMsgHandle(XSI_MESSAGE_BEGIN,
                            XSI_MESSAGE_END, OnFWDNotReachMsg);
}

// 设置子页面前的操作
void CDlgFWDNotReach::BeforeSetSubPage(void * pData)
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

// 设置子页面后的操作
void CDlgFWDNotReach::AfterSetSubPage(void * pData)
{
//  if (NULL != pData
//      && NULL != m_pComCfnrStatus)
//  {
//      m_pComCfnrStatus->setCurrentIndex(m_pComCfnrStatus
//          ->findData(true, Qt::UserRole));
//  }
}


void CDlgFWDNotReach::showEvent(QShowEvent * e)
{
    OnShow();
}

void CDlgFWDNotReach::ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    if (pMessageBox->GetType() == MESSAGEBOX_Cancel
            && pMessageBox->GetResult() == MessageBox_Cancel)
    {
        CFNR_CancelRequest();
    }
}

void CDlgFWDNotReach::OnShow()
{
#if IF_BUG_22686
#else
    return;
#endif  //IF_BUG_22686

    //表示处于加载中
    m_bIsInLoad = true;

    if (BWCallControl_IsAccountXSIEnable(m_iAccountId))
    {
        if (ProcessMsgBoxPassword(m_iAccountId))
        {
            return ;
        }
        bool bSuc = CFNR_QueryForwardNotReachable(m_iAccountId);
    }
    else
    {
        m_bIsInLoad = false;
    }

    if (!m_bIsInLoad)
    {
        // 如果所有的账号都下载失败，则退出
        MessageBox_ShowCommonMsgBox(NULL,
                                    LANG_TRANSLATE(TRID_LOAD_FAILED),
                                    MESSAGEBOX_NOTE, 1500, "", "",
                                    MSG_ID_SHOW_NOTE_BEFORE_BACK);
    }
    else
    {
        MessageBox_ShowCommonMsgBox(this,
                                    LANG_TRANSLATE(TRID_LOADING_PLEASE_WAIT),
                                    MESSAGEBOX_Cancel);
    }
}

// 处理密码弹窗
bool CDlgFWDNotReach::ProcessMsgBoxPassword(int nLineId)
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
        SettingUI_PopPasswordDlg(this, nLineId);
        return true;
    }
    else
    {
        return false;
    }
}
