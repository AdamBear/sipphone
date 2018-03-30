#include "model.h"
#include "cdlgcallrecord.h"
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

#include "broadsoft/callrecording/include/modcallrecording.h"
#include "broadsoft/callrecording/include/callrecord_def.h"
#include "uicommon/uikernel/qcomboboxutility.h"

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
namespace
{
//设定edit控件输入长度的最大值
#define EDIT_MAX_LENGTH  10
#define MSG_ID_LOADING  100
}

static LRESULT OnCallRecordMsg(msgObject & objMessage)
{
    SETTINGUI_INFO("OnCallRecordMsg [%d] [%d]",
                   objMessage.message, objMessage.wParam);

    CDlgCallRecord * pRONew = qobject_cast<CDlgCallRecord *>(UIManager_GetPrivateDlg(
                                  DLG_CDlgCallRecord));
    if (NULL == pRONew || !pRONew->IsTopPage())
    {
        return 0;
    }

    pRONew->OnReceiveMsg(objMessage);
    return 0;
}


CDlgCallRecord::CDlgCallRecord(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pComBoMode(NULL)
    , m_pComBoPauseResume(NULL)
    , m_pCheckPlayStartStop(NULL)
    , m_pCheckRecordVM(NULL)
    , m_pCheckPlayTone(NULL)
    , m_pEditPlayToneTime(NULL)
    , m_iAccountId(-1)
{
    InitData();
}

CDlgCallRecord::~CDlgCallRecord()
{
}

void CDlgCallRecord::Init()
{
    etl_RegisterMsgHandle(XSI_MESSAGE_BEGIN,
                          XSI_MESSAGE_END, OnCallRecordMsg);
}

void CDlgCallRecord::Uninit()
{
    etl_UnregisterMsgHandle(XSI_MESSAGE_BEGIN,
                            XSI_MESSAGE_END, OnCallRecordMsg);
}

void CDlgCallRecord::InitData()
{
    m_iAccountId = acc_GetDefaultAccount();
    // 设置标题
    m_strTitle = LANG_TRANSLATE(TRID_CALL_RECORD).toUtf8().data();

    //初始化 下面会改变父窗口，这里用this初始化是为了防止内存泄漏问题
    m_pComBoMode = new QComboBox(this);
    m_pComBoPauseResume = new QComboBox(this);
    m_pCheckPlayStartStop = new qCheckBox(this);
    m_pCheckRecordVM = new qCheckBox(this);
    m_pCheckPlayTone = new qCheckBox(this);
    m_pEditPlayToneTime = new QLineEdit(this);

    //指针判空
    if (m_pComBoMode == NULL
            || m_pComBoPauseResume == NULL
            || m_pCheckPlayStartStop == NULL
            || m_pCheckRecordVM == NULL
            || m_pCheckPlayTone == NULL
            || m_pEditPlayToneTime == NULL)
    {
        return;
    }

    qLineEditUtility::SetLineEditStyle(m_pEditPlayToneTime, Qt::black);

    //设置输入框输入的最大长度
    m_pEditPlayToneTime->setMaxLength(EDIT_MAX_LENGTH);

    qComboBoxUtility::SetComboBoxStyle(m_pComBoMode, Qt::black);
    qComboBoxUtility::SetComboBoxStyle(m_pComBoPauseResume, Qt::black);

    m_pComBoMode->addItem(LANG_TRANSLATE(TRID_CALLRECORD_NEVER), (int)CRM_NEVER);
    m_pComBoMode->addItem(LANG_TRANSLATE(TRID_CALLRECORD_ALLWAY), (int)CRM_ALWAYS);
    m_pComBoMode->addItem(LANG_TRANSLATE(TRID_CALLRECORD_ALLWAY_PR), (int)CRM_ALWAYS_PAUSE_RESUME);
    m_pComBoMode->addItem(LANG_TRANSLATE(TRID_CALLRECORD_DEMAND), (int)CRM_DEMAND);
    m_pComBoMode->addItem(LANG_TRANSLATE(TRID_CALLRECORD_DEMAND_USER), (int)CRM_DEMAND_USER_START);

    m_pComBoPauseResume->addItem(LANG_TRANSLATE("None"), 0);
    m_pComBoPauseResume->addItem(LANG_TRANSLATE("Beep"), 1);
    m_pComBoPauseResume->addItem(LANG_TRANSLATE("Announcement"), 2);

    m_pCheckPlayStartStop->SetData(false);
    m_pCheckRecordVM->SetData(false);
    m_pCheckPlayTone->SetData(false);

    //为输入框安装输入法
    qInstallIME(m_pEditPlayToneTime, IME_DIAL);

    //设置Activation标签
    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_CALL_RECORD_MODE);
    qPair.first = strLab;
    qPair.second = m_pComBoMode;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_CALL_RECORD_PR_NOTE);
    qPair.first = strLab;
    qPair.second = m_pComBoPauseResume;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_CALL_RECORD_PLAY_SS);
    qPair.first = strLab;
    qPair.second = m_pCheckPlayStartStop;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_CALL_RECORD_RECORD_VM);
    qPair.first = strLab;
    qPair.second = m_pCheckRecordVM;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_CALL_RECORD_PLAY_TONE);
    qPair.first = strLab;
    qPair.second = m_pCheckPlayTone;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_CALL_RECORD_PLAY_TONETIME);
    qPair.first = strLab;
    qPair.second = m_pEditPlayToneTime;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);

    //安装事件过滤器
    m_pComBoMode->installEventFilter(this);
    m_pComBoPauseResume->installEventFilter(this);
    m_pCheckPlayStartStop->installEventFilter(this);
    m_pCheckRecordVM->installEventFilter(this);
    m_pCheckPlayTone->installEventFilter(this);
    m_pEditPlayToneTime->installEventFilter(this);

    //为界面自己安装事件过滤器
    this->installEventFilter(this);


}


//处理XSI密码
void CDlgCallRecord::OnSubmitXSIPassword(const yl::string & strPswd)
{
    SETTINGUI_INFO("CDlgCallRecord::OnSubmitXSIPassword, account %d", m_iAccountId);
    BWCallControl_SetAccountXSIPassword(strPswd.c_str(), m_iAccountId);
    ExtraDialogInitData();
}

void CDlgCallRecord::OnCancelInputXSIPassword()
{
    //回到上一层界面
    emit BtnBackClick();
}

//收到logic层返回的消息
void CDlgCallRecord::OnReceiveMsg(msgObject & objMessage)
{
    switch (objMessage.message)
    {
    case XSI_MESSAGE_REQ_FINISH:
        {
            if (XSI_CALL_RECORD_MODE_GET == objMessage.wParam)
            {
                MessageBox_RemoveTopMessageBox(MessageBox_Destory);

                GetCallRecordData();
            }
            else if (XSI_CALL_RECORD_MODE_PUT == objMessage.wParam)
            {
                MessageBox_RemoveTopMessageBox(MessageBox_OK);

                emit BtnBackClick();
            }
        }
        break;
    case XSI_MESSAGE_REQ_FAIL:
        {
            if (XSI_CALL_RECORD_MODE_GET != objMessage.wParam
                    && XSI_CALL_RECORD_MODE_PUT != objMessage.wParam)
            {
                return ;
            }

            MessageBox_RemoveTopMessageBox(MessageBox_Destory);

            SETTINGUI_INFO("CDlgCallRecord::objMessage wParam [%d], lParam [%d], m_iAccountId [%d]",
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
                    SETTING_INFO("Call Record Account do NOT match!!");
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
void CDlgCallRecord::GetCallRecordData()
{
    SCallRecord sRecordInfo;
    callRecord_GetServerRecordMode(sRecordInfo, m_iAccountId);

    //指针判空
    if (m_pComBoMode == NULL
            || m_pComBoPauseResume == NULL
            || m_pCheckPlayStartStop == NULL
            || m_pCheckRecordVM == NULL
            || m_pCheckPlayTone == NULL
            || m_pEditPlayToneTime == NULL)
    {
        return;
    }

    m_pComBoMode->setCurrentIndex(m_pComBoMode->findData(sRecordInfo.nRecordMode, Qt::UserRole));
    m_pComBoPauseResume->setCurrentIndex(m_pComBoPauseResume->findData(sRecordInfo.nPauseResumeMethod,
                                         Qt::UserRole));
    m_pCheckPlayStartStop->SetData(sRecordInfo.bPlayStartStopAnnouncement);
    m_pCheckRecordVM->SetData(sRecordInfo.bRecordVM);
    m_pCheckPlayTone->SetData(sRecordInfo.bPlayTone);

    m_pEditPlayToneTime->setText(toQString(sRecordInfo.strPlayToneTime));
}

bool CDlgCallRecord::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    if (m_pEditPlayToneTime == NULL)
    {
        return false;
    }

    QString strToneTime = m_pEditPlayToneTime->text();

    int iToneTime = atoi(strToneTime.toUtf8().data());
    // 判断Number是否合法
    if (!BWCallControl_CheckPhoneNumberValid(strToneTime.toUtf8().data(), 10)
            || !(iToneTime >= 10 && iToneTime <= 1800))
    {
        objErrorContent.strErrorContent = TRID_INVALID_CALLRECORD_TIME;
        objErrorContent.pLineEdit = m_pEditPlayToneTime;
        m_pEditPlayToneTime->setFocus();
        return false;
    }

    return true;
}


bool CDlgCallRecord::IsStatusChanged()
{
    SCallRecord sRecordInfo;
    callRecord_GetServerRecordMode(sRecordInfo, m_iAccountId);

    //指针判空
    if (m_pComBoMode == NULL
            || m_pComBoPauseResume == NULL
            || m_pCheckPlayStartStop == NULL
            || m_pCheckRecordVM == NULL
            || m_pCheckPlayTone == NULL
            || m_pEditPlayToneTime == NULL)
    {
        return false;
    }

    int currentRecordMode = m_pComBoMode
                            ->itemData(m_pComBoMode->currentIndex(), Qt::UserRole).value<int>();

    int currentPauseResumeMethod = m_pComBoPauseResume
                                   ->itemData(m_pComBoPauseResume->currentIndex(), Qt::UserRole).value<int>();

    if (currentRecordMode != sRecordInfo.nRecordMode
            || currentPauseResumeMethod != sRecordInfo.nPauseResumeMethod
            || m_pCheckPlayStartStop->GetData() != sRecordInfo.bPlayStartStopAnnouncement
            || m_pCheckRecordVM->GetData() != sRecordInfo.bRecordVM
            || m_pCheckPlayTone->GetData() != sRecordInfo.bPlayTone
            || m_pEditPlayToneTime->text() != toQString(sRecordInfo.strPlayToneTime))
    {
        return true;
    }

    return false;
}

bool CDlgCallRecord::LoadPageData()
{
    return true;
}

bool CDlgCallRecord::SavePageData()
{
    m_bIsInLoad = false;

    SCallRecord sRecordInfo;

    sRecordInfo.nRecordMode = m_pComBoMode
                              ->itemData(m_pComBoMode->currentIndex(), Qt::UserRole).value<int>();

    sRecordInfo.nPauseResumeMethod = m_pComBoPauseResume
                                     ->itemData(m_pComBoPauseResume->currentIndex(), Qt::UserRole).value<int>();

    sRecordInfo.bPlayStartStopAnnouncement = m_pCheckPlayStartStop->GetData();

    sRecordInfo.bRecordVM = m_pCheckRecordVM->GetData();

    sRecordInfo.bPlayTone = m_pCheckPlayTone->GetData();

    sRecordInfo.strPlayToneTime = m_pEditPlayToneTime->text().toUtf8().data();

    callRecord_SetServerRecordMode(sRecordInfo, m_iAccountId);
    MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_SAVING_PLEASE_WAIT), MESSAGEBOX_Cancel);
    return false;
}

// 设置子页面前的操作
void CDlgCallRecord::BeforeSetSubPage(void * pData)
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

void CDlgCallRecord::ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox)
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
        callRecord_CancelRequest();
    }
}

void CDlgCallRecord::ExtraDialogInitData()
{
    //表示处于加载中
    m_bIsInLoad = true;
    if (BWCallControl_IsAccountXSIEnable(m_iAccountId))
    {
        if (ProcessMsgBoxPassword(m_iAccountId))
        {
            return;
        }

        callRecord_QueryServerRecordMode(m_iAccountId);
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
bool CDlgCallRecord::ProcessMsgBoxPassword(int nLineId)
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
        SETTINGUI_INFO("CDlgCallRecord::ProcessMsgBoxPassword, XSI_AUTH_ERROR");
        SettingUI_PopPasswordDlg(this, nLineId);
        return true;
    }
    else
    {
        SETTINGUI_INFO("CDlgCallRecord::ProcessMsgBoxPassword, xsi status [%d]",
                       CBaseHelper::GetXSIStatus(nLineId));
        return false;
    }
}
#endif
