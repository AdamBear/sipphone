#include "model.h"
#include "cdlgsimultaneousringnumbers.h"
#include "messagebox/modmessagebox.h"
#include "actiondefinecommon.h"

#include "uikernel/languagehelper.h"
#include "qtcommon/qmisc.h"
#include "setting/include/common.h"
#include "account/include/modaccount.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "settingui/include/modsettingui.h"
#include "uikernel/qcheckbox.h"
#include "settinguicommon.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/xsiactionhelper/include/xsicommon.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "baseui/t4xpicpath.h"

namespace
{
#define MAX_SIMULTANEOUS_RING_NUMBER_COUNT  10
//设定edit控件输入长度的最大值
const int EDIT_MAX_LENGTH = 99;
static const int MSGBOX_ID_COMFIRM_DELETE_ALL_NUMBERS(102);
static const int MSGBOX_ID_DELETING_ALL_NUMBERS(103);
static const int MSGBOX_ID_LOADING(104);
static const int MSGBOX_ID_TIP_ENTERING_NEW(105);

const yl::string strSimultaneousNewPage  = "OpenSubPage(simultaneous_ring_new)";
}
static LRESULT OnSimultaneousRingMsg(msgObject & objMessage)
{
    SETTINGUI_INFO("OnSimultaneousRingMsg [%d] [%d]",
                   objMessage.message, objMessage.wParam);

    CDlgSimultaneousRingNumbers * pSRNumbers = qobject_cast<CDlgSimultaneousRingNumbers *>
            (UIManager_GetPrivateDlg(DLG_CDlgSimultaneousRingNumbers));
    if (NULL == pSRNumbers || !pSRNumbers->IsTopPage())
    {
        return 0;
    }

    pSRNumbers->OnReceiveMsg(objMessage);
    return 0;
}

CDlgSimultaneousRingNumbers::CDlgSimultaneousRingNumbers(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_eMessageBoxType(EMBT_NONE)
    , m_pStatusCheckBox(NULL)
    , m_iAccountId(-1)
{
    InitData();
}

CDlgSimultaneousRingNumbers::~CDlgSimultaneousRingNumbers()
{
}

void CDlgSimultaneousRingNumbers::Init()
{
    etl_RegisterMsgHandle(XSI_MESSAGE_BEGIN,
                          XSI_MESSAGE_END, OnSimultaneousRingMsg);
}

void CDlgSimultaneousRingNumbers::Uninit()
{
    etl_UnregisterMsgHandle(XSI_MESSAGE_BEGIN,
                            XSI_MESSAGE_END, OnSimultaneousRingMsg);
}

void CDlgSimultaneousRingNumbers::InitData()
{
    m_iAccountId = acc_GetDefaultAccount();
    //设置标题
    m_strTitle = LANG_TRANSLATE(TRID_PHONE_NUMBERS).toUtf8().data();

    m_pStatusCheckBox = new qCheckBox(this);
    //指针判空
    if (m_pStatusCheckBox == NULL)
    {
        return;
    }

    //安装事件过滤器
    m_pStatusCheckBox->installEventFilter(this);
    this->installEventFilter(this);
    m_pStatusCheckBox->hide();
}

//获取SimultaneousRing号码列表
void CDlgSimultaneousRingNumbers::GetSimultaneousRingNumbers()
{
    m_RingNumbers.Reset();
    MakeReadyForRelayout();

    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_STATUS);
    qPair.first = strLab;
    qPair.second = m_pStatusCheckBox;
    qPair.bChangeStyleOnPress = false;
    m_vecWidgetPair.push_back(qPair);

    SRP_GetSimultaneousRingInfo(m_RingNumbers);
    m_pStatusCheckBox->SetData(m_RingNumbers.bActive);

    qPair.bChangeStyleOnPress = true;
    for (int i = 0; i < m_RingNumbers.simRingLocationArry.size(); ++i)
    {
        qPair.first = m_RingNumbers.simRingLocationArry.at(i).strAddress.c_str();
        qPair.second = NULL;
        qPair.eType = SETTING_ITEM_MENU;
        qPair.third = OPTION_ACTION_EDIT;
        qPair.bAddIndex = false;
        m_vecWidgetPair.push_back(qPair);
    }

    AddWidget(m_vecWidgetPair, false, false, true, false);
    UpdateWnd();
}

//处理XSI密码
void CDlgSimultaneousRingNumbers::OnSubmitXSIPassword(const yl::string & strPswd)
{
    BWCallControl_SetAccountXSIPassword(strPswd.c_str(), m_iAccountId);
    ExtraDialogInitData();
}

void CDlgSimultaneousRingNumbers::OnCancelInputXSIPassword()
{
    //回到上一层界面
    emit BtnBackClick();
}


//收到logic层返回的消息
void CDlgSimultaneousRingNumbers::OnReceiveMsg(msgObject & objMessage)
{
    SETTINGUI_INFO("SimultaneousRingNumbers ReceiveMsg [%d]", objMessage.message);
    switch (objMessage.message)
    {
    case XSI_MESSAGE_REQ_FINISH:
        {
            if (XSI_SIMULTANEOUSRING_GET == objMessage.wParam)
            {
                MessageBox_RemoveTopMessageBox(MessageBox_OK);

                GetSimultaneousRingNumbers();
            }
            else if (XSI_SIMULTANEOUSRING_PUT == objMessage.wParam)
            {
                MessageBox_RemoveTopMessageBox(MessageBox_OK);
            }
        }
        break;
    case XSI_MESSAGE_REQ_FAIL:
        {
            if (XSI_SIMULTANEOUSRING_GET != objMessage.wParam
                    && XSI_SIMULTANEOUSRING_PUT != objMessage.wParam)
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
                ProcessMsgBoxPassword(m_iAccountId);
            }
            else
            {
                QString strErr;
                if (EMBT_DELETE == m_eMessageBoxType)
                {
                    strErr = LANG_TRANSLATE(TRID_DELETE_FIALED);
                }
                else if (EMBT_SAVE == m_eMessageBoxType)
                {
                    strErr = LANG_TRANSLATE(TRID_SAVE_FAILED);
                }
                else if (EMBT_LOAD == m_eMessageBoxType)
                {
                    strErr = LANG_TRANSLATE(TRID_LOAD_FAILED);
                }
                else
                {
                    break;
                }
                m_eMessageBoxType = EMBT_NONE;

                // 如果失败，则回退界面
                MessageBox_ShowCommonMsgBox(this,
                                            strErr,
                                            MESSAGEBOX_NOTE, SHORT_TIME, "", "",
                                            MSG_ID_SHOW_NOTE_BEFORE_BACK);
            }
        }
        break;
    default:
        break;
    }
}

// 设置页面的SoftKey
bool CDlgSimultaneousRingNumbers::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    objWndData[0].m_strSoftkeyAction = ACTION_ADD;
    objWndData[0].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_ADD);
    objWndData[0].m_strIconPath = PIC_SOFTKEY_ADD;


    if (m_RingNumbers.simRingLocationArry.size() > 0)
    {
        objWndData[1].m_strSoftkeyAction = ACTION_DELETEALL;
        objWndData[1].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_DELETE_ALL);
        objWndData[1].m_strIconPath = PIC_SOFTKEY_REMOVE;
    }
    else
    {
        objWndData[1].m_strSoftkeyAction = "";
        objWndData[1].m_strSoftkeyTitle = "";
    }

    return true;
}

//事件过滤器
bool CDlgSimultaneousRingNumbers::eventFilter(QObject * pObject, QEvent * pEvent)
{
    return CDlgBaseSubPage::eventFilter(pObject, pEvent);
}

bool CDlgSimultaneousRingNumbers::IsStatusChanged()
{
    return m_pStatusCheckBox->GetData() != m_RingNumbers.bActive;
}

bool CDlgSimultaneousRingNumbers::LoadPageData()
{
    return false;
}

bool CDlgSimultaneousRingNumbers::SavePageData()
{
    bool bStatus  = m_pStatusCheckBox->GetData();

    if (bStatus && m_RingNumbers.simRingLocationArry.size() == 0)
    {
        MessageBox_ShowCommonMsgBox(this,
                                    LANG_TRANSLATE(TRID_NEED_A_NUMBER),
                                    MESSAGEBOX_NOTE, SHORT_TIME, "", "",
                                    MSGBOX_ID_TIP_ENTERING_NEW);
    }
    else
    {
        m_RingNumbers.bActive = bStatus;

        m_eMessageBoxType = EMBT_SAVE;
        SRP_SetSimultaneousRing(m_RingNumbers);

        MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_SAVING_PLEASE_WAIT), MESSAGEBOX_NOTE);
    }

    return false;
}

void CDlgSimultaneousRingNumbers::showEvent(QShowEvent * e)
{

}

bool CDlgSimultaneousRingNumbers::OnAction(const QString & strAction)
{
    bool bHandled = true;
    if (strAction == ACTION_ADD)
    {
        if (m_RingNumbers.simRingLocationArry.size() >= MAX_SIMULTANEOUS_RING_NUMBER_COUNT)
        {
            MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_MEMORY_FULL), MESSAGEBOX_NOTE, SHORT_TIME);
            return true;
        }
        //进入Add界面
        emit openpage(SubMenuData(strSimultaneousNewPage));
    }
    else if (strAction == ACTION_DELETEALL)
    {
        OnDeleteAllSimultaneousRingNumber();
    }
    else if (strAction == OPTION_ACTION_EDIT)
    {
        OnEditSimultaneousRingNumber();                 //SimultaneousRingNumber界面Option(Edit)
    }
    else
    {
        bHandled = false;
    }

    return bHandled;
}

void CDlgSimultaneousRingNumbers::OnEditSimultaneousRingNumber()
{
    if (m_pGeneralFrame == NULL)
    {
        return;
    }

    int iIndex = m_pGeneralFrame->GetFocusItemDataIndex() - 1;
    if (iIndex < 0)
    {
        return;
    }

    //QString strNumber = GetSelectedNumber();

    //进入编辑页面
    SubMenuData subData(strSimultaneousNewPage);
    subData.pBeforeSetSubpageData = &iIndex;
    subData.pAfterSetSubpageData = &m_iAccountId;
    emit openpage(subData);
}

void CDlgSimultaneousRingNumbers::OnDeleteAllSimultaneousRingNumber()
{
    MessageBox_ShowCommonMsgBox(this,
                                LANG_TRANSLATE(TRID_IF_DELETE_ALL_NUMBERS),
                                MESSAGEBOX_OkCancel, -1, "", "",
                                MSGBOX_ID_COMFIRM_DELETE_ALL_NUMBERS);
}

// 设置子页面前的操作
void CDlgSimultaneousRingNumbers::BeforeSetSubPage(void * pData)
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

// 从服务器下载数据
void CDlgSimultaneousRingNumbers::DownloadSimData()
{
    m_eMessageBoxType = EMBT_LOAD;
    SRP_QuerySimultaneousRing(m_iAccountId);
    MessageBox_ShowCommonMsgBox(this,
                                LANG_TRANSLATE(TRID_LOADING_PLEASE_WAIT),
                                MESSAGEBOX_Cancel, -1, "", "",
                                MSGBOX_ID_LOADING);
}
void CDlgSimultaneousRingNumbers::ExtraDialogInitData()
{
    if (BWCallControl_IsAccountXSIEnable(m_iAccountId))
    {
        if (ProcessMsgBoxPassword(m_iAccountId))
        {
            return;
        }
        DownloadSimData();
    }
    else
    {
        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_XSI_INFORMATION_IS_INCOMPLETE),
                                    MESSAGEBOX_NOTE, SHORT_TIME, "", "", MSG_ID_SHOW_NOTE_BEFORE_BACK);
    }
}
bool CDlgSimultaneousRingNumbers::ProcessMsgBoxPassword(int nLineId)
{
    if (XSI_SIPACCOUNT_NOT_AVAILABLE == CBaseHelper::GetXSIStatus(nLineId))
    {
        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_SETTUI_TIP_SIP_XSI_AUTHENTICATION_FAILED),
                                    MESSAGEBOX_NOTE, SHORT_TIME, "", "", MSG_ID_SHOW_NOTE_BEFORE_BACK);
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
void CDlgSimultaneousRingNumbers::ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }
    int iRet = pMessageBox->GetResult();
    switch (pMessageBox->GetID())
    {
    case MSGBOX_ID_COMFIRM_DELETE_ALL_NUMBERS:
        {
            if (iRet == MessageBox_OK)
            {
                m_RingNumbers.Reset();
                m_RingNumbers.bActive = false;
                m_eMessageBoxType = EMBT_DELETE;
                SRP_SetSimultaneousRing(m_RingNumbers);
                MessageBox_ShowCommonMsgBox(this,
                                            LANG_TRANSLATE(TRID_DELETING_PLEASE_WAIT),
                                            MESSAGEBOX_Cancel, -1, "", "",
                                            MSGBOX_ID_DELETING_ALL_NUMBERS);
            }
        }
        break;
    case MSGBOX_ID_DELETING_ALL_NUMBERS:
        {
            if (iRet == MessageBox_Cancel)
            {
                SRP_CancelRequest();
            }
            else if (iRet == MessageBox_OK)
            {
                DownloadSimData();
            }
        }
        break;
    case MSGBOX_ID_LOADING:
        {
            if (iRet == MessageBox_Cancel)
            {
                SRP_CancelRequest();
            }
        }
        break;
    case MSGBOX_ID_TIP_ENTERING_NEW:
        {
            bool bStatus = m_pStatusCheckBox->GetData();
            SubMenuData subData(strSimultaneousNewPage);
            subData.pAfterSetSubpageData = &bStatus;
            emit openpage(subData);
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
        {
        }
        break;
    }
}
