#include "model.h"

#include "canywhereuilogic.h"
#include "uicommon/messagebox/modmessagebox.h"
#include "cdlgsilentalerting.h"
#include "uikernel/languagehelper.h"
#include "qtcommon/qmisc.h"
#include "setting/include/common.h"
#include "inputmethod/inputmethod.h"
#include "baseui/fontcolor.h"
#include "qtcommon/qmisc.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "account/include/modaccount.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "cdlgcallcontrol.h"
#include "keymap.h"
#include "modsettingui.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/xsiactionhelper/include/xsicommon.h"
#include "uimanager/moduimanager.h"
#include "settinguicommon.h"
#include "uikernel/qcomboboxutility.h"





static LRESULT OnSilentAlerting(msgObject & objMessage)
{
    SETTINGUI_INFO("OnSilentAlerting 0x[%x] 0x[%x]",
                   objMessage.message, objMessage.wParam);

    CDlgSilentAlerting * pSANew = qobject_cast<CDlgSilentAlerting *>(UIManager_GetPrivateDlg(
                                      DLG_CDlgSilentAlerting));
    if (NULL == pSANew || !pSANew->IsTopPage())
    {
        return 0;
    }

    pSANew->OnReceiveMsg(objMessage);
    return 0;
}


CDlgSilentAlerting::CDlgSilentAlerting(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pCmbBoxStatus(NULL)
    , m_iAccountId(-1)
    , m_bIsSilent(false)
{
    InitData();

}

void CDlgSilentAlerting::InitData()
{


    // 设置标题
    m_strTitle = LANG_TRANSLATE(TRID_SILENTALERT).toUtf8().data();

    //初始化 下面会改变父窗口，这里用this初始化是为了防止内存泄漏问题
    m_pCmbBoxStatus = new QComboBox(this);
    qComboBoxUtility::SetComboBoxStyle(m_pCmbBoxStatus, Qt::black);
    m_pCmbBoxStatus->addItem(LANG_TRANSLATE(TRID_OFF), QVariant(0));
    m_pCmbBoxStatus->addItem(LANG_TRANSLATE(TRID_ON), QVariant(1));

    //指针判空
    if (NULL == m_pCmbBoxStatus)
    {
        return;
    }


    QWidgetPair qPair;
    qPair.first = LANG_TRANSLATE(TRID_SILENTALERT);
    qPair.second = m_pCmbBoxStatus;
    m_vecWidgetPair.push_back(qPair);
    AddWidget(m_vecWidgetPair);




    //安装事件过滤器
    m_pCmbBoxStatus->installEventFilter(this);

    //为界面自己安装事件过滤器
    this->installEventFilter(this);
}

//处理XSI密码
void CDlgSilentAlerting::OnSubmitXSIPassword(const yl::string & strPswd)
{
    BWCallControl_SetAccountXSIPassword(strPswd.c_str(), m_iAccountId);
    OnShow();
}

void CDlgSilentAlerting::OnCancelInputXSIPassword()
{
    //回到上一层界面
    emit BtnBackClick();
}

//收到logic层返回的消息
void CDlgSilentAlerting::OnReceiveMsg(msgObject & objMessage)
{
    switch (objMessage.message)
    {
    case XSI_MESSAGE_REQ_FINISH:
        {
            if (XSI_SILENT_ALERTING_GET == objMessage.wParam)
            {
                MessageBox_RemoveTopMessageBox(MessageBox_Destory);

                GetSilentAlertingData();
            }
        }
        break;
    case XSI_MESSAGE_REQ_FAIL:
        {
            if (XSI_SILENT_ALERTING_GET != objMessage.wParam)
            {
                return;
            }

            MessageBox_RemoveTopMessageBox(MessageBox_Destory);

            SXsiExtenInfo * pErrInfo = (SXsiExtenInfo *)objMessage.GetExtraData();

            if (NULL == pErrInfo)
            {
                return;
            }

            if (pErrInfo->m_nError == 401)
            {
                ProcessMsgBoxPassword(m_iAccountId);
            }
            else

            {
                if (m_bIsInLoad)
                {
                    MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_LOAD_FAILED), MESSAGEBOX_NOTE, 1500);
                }
                else
                {
                    // 如果所有的账号都下载失败，则退出
                    MessageBox_ShowCommonMsgBox(this,   LANG_TRANSLATE(TRID_SAVE_FAILED),
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

bool CDlgSilentAlerting::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    // 因为该界面不允许更改，softkey只需设置回退键

    objWndData[3].m_strSoftkeyAction = "SaveData";
    objWndData[3].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_SAVE);
    return true;
}

//获取SilentAlerting数据
void CDlgSilentAlerting::GetSilentAlertingData()
{
    if (SA_GetSilentAlerting(m_iAccountId, m_bIsSilent) && NULL != m_pCmbBoxStatus)
    {
        m_pCmbBoxStatus->setCurrentIndex((int)m_bIsSilent);
    }

}

bool CDlgSilentAlerting::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    return true;
}


bool CDlgSilentAlerting::IsStatusChanged()
{
    return false;
}

bool CDlgSilentAlerting::LoadPageData()
{
    return true;
}

bool CDlgSilentAlerting::SavePageData()
{
    if (m_pCmbBoxStatus->currentIndex() != (int)m_bIsSilent)
    {
        SA_SetSilentAlerting(m_iAccountId, m_pCmbBoxStatus->currentIndex());
    }

    return true;
}


CDlgSilentAlerting::~CDlgSilentAlerting()
{

}

// 设置子页面前的操作
void CDlgSilentAlerting::BeforeSetSubPage(void * pData)
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

void CDlgSilentAlerting::ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox)
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
        SA_CancelRequest();
    }
}


void CDlgSilentAlerting::showEvent(QShowEvent * e)
{
    OnShow();
}

void CDlgSilentAlerting::OnShow()
{
#if IF_BUG_22977
#else
    if (_AccountManager.GetUsableAccountCount() > 1)
    {
        return;
    }
#endif
    //表示处于加载中
    m_bIsInLoad = true;

    if (BWCallControl_IsAccountXSIEnable(m_iAccountId))
    {
#if IF_BUG_22977
        if (ProcessMsgBoxPassword(m_iAccountId))
        {
            return;
        }
#endif
        SA_QuerySilentAlerting(m_iAccountId);
    }
    else
    {
        m_bIsInLoad = false;
    }

    if (!m_bIsInLoad)
    {
        // 如果所有的账号都下载失败，则退出
        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_XSI_INFORMATION_IS_INCOMPLETE),
                                    MESSAGEBOX_NOTE, 1500, "", "",
                                    MSG_ID_SHOW_NOTE_BEFORE_BACK);
    }
    else
    {
        MessageBox_ShowCommonMsgBox(this,   LANG_TRANSLATE(TRID_LOADING_PLEASE_WAIT),
                                    MESSAGEBOX_Cancel);
    }
}

// 处理密码弹窗
bool CDlgSilentAlerting::ProcessMsgBoxPassword(int nLineId)
{
    if (XSI_SIPACCOUNT_NOT_AVAILABLE == CBaseHelper::GetXSIStatus(nLineId))
    {
        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_SETTUI_TIP_SIP_XSI_AUTHENTICATION_FAILED),
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

// 初始化
void CDlgSilentAlerting::Init()
{
    etl_RegisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END, OnSilentAlerting);
}

// 反初始化
void CDlgSilentAlerting::Uninit()
{
    etl_UnregisterMsgHandle(XSI_MESSAGE_BEGIN,
                            XSI_MESSAGE_END, OnSilentAlerting);
}
