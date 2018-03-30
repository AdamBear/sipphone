#include "model.h"
#include "cdlggroupnightfwd.h"
#include "canywhereuilogic.h"
#include "uicommon/messagebox/modmessagebox.h"

#include "uikernel/languagehelper.h"
#include "qtcommon/qmisc.h"
#include "setting/include/common.h"
#include "setting/src/callforwardcontroller.h"
#include "inputmethod/inputmethod.h"
#include "baseui/fontcolor.h"
#include "qtcommon/qmisc.h"
//#include "bwcallcontrol/callforwardnotreachablemanager.h"
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

namespace
{
#define MSG_ID_LOADING 100

//设定edit控件输入长度的最大值
// #define EDIT_MAX_LENGTH  30
}

static LRESULT OnGroupNightForward(msgObject & objMessage)
{
    SETTINGUI_INFO("OnGroupNightForward 0x[%x] 0x[%x]",
                   objMessage.message, objMessage.wParam);

    CDlgGroupNightFWD * pGNFNew = qobject_cast<CDlgGroupNightFWD *>(UIManager_GetPrivateDlg(
                                      DLG_CDlgGroupNightFWD));
    if (NULL == pGNFNew || !pGNFNew->IsTopPage())
    {
        return 0;
    }

    pGNFNew->OnReceiveMsg(objMessage);
    return 0;
}


CDlgGroupNightFWD::CDlgGroupNightFWD(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pLabGroupNightForward(NULL)
    , m_pLabBusinessHour(NULL)
    , m_pLabHolidaySchedule(NULL)
    , m_iAccountId(-1)
{
    InitData();
    connect(this, SIGNAL(ProcessPageAction(const QString &)), this,
            SLOT(OnProcessPageAction(const QString &)));
}

void CDlgGroupNightFWD::InitData()
{
    etl_RegisterMsgHandle(XSI_MESSAGE_BEGIN,
                          XSI_MESSAGE_END, OnGroupNightForward);

    // 设置标题
    m_strTitle = LANG_TRANSLATE(TRID_GROUP_NIGHT_FORWARD).toUtf8().data();

    //初始化 下面会改变父窗口，这里用this初始化是为了防止内存泄漏问题
    m_pLabGroupNightForward = new QLabel(this);
    m_pLabBusinessHour = new QLabel(this);
    m_pLabHolidaySchedule = new QLabel(this);

    //指针判空
    if (m_pLabGroupNightForward == NULL
            || m_pLabBusinessHour == NULL
            || m_pLabHolidaySchedule == NULL)
    {
        return;
    }

    /*
    设置标签上文字的大小及标签名
    LANG_TRANSLATE是翻译宏，为了方便语言的转化
    */
    //设置Activation标签

    //安装事件过滤器
    m_pLabGroupNightForward->installEventFilter(this);
    m_pLabBusinessHour->installEventFilter(this);
    m_pLabHolidaySchedule->installEventFilter(this);

    m_pLabGroupNightForward->setDisabled(true);
    m_pLabBusinessHour->setDisabled(true);
    m_pLabHolidaySchedule->setDisabled(true);

    m_pLabBusinessHour->hide();
    m_pLabHolidaySchedule->hide();

    //为界面自己安装事件过滤器
    this->installEventFilter(this);
}

//处理XSI密码
void CDlgGroupNightFWD::OnSubmitXSIPassword(const yl::string & strPswd)
{
    BWCallControl_SetAccountXSIPassword(strPswd.c_str(), m_iAccountId);
    ExtraDialogInitData();
}

void CDlgGroupNightFWD::OnCancelInputXSIPassword()
{
    //回到上一层界面
    emit BtnBackClick();
}

//收到logic层返回的消息
void CDlgGroupNightFWD::OnReceiveMsg(msgObject & objMessage)
{
    switch (objMessage.message)
    {
    case XSI_MESSAGE_REQ_FINISH:
        {
            if (XSI_GROUPNIGHT_FORWARD_GET == objMessage.wParam)
            {
                MessageBox_RemoveTopMessageBox(MessageBox_Destory);

                GetGroupNightForwardData();
            }
        }
        break;
    case XSI_MESSAGE_REQ_FAIL:
        {
            if (XSI_GROUPNIGHT_FORWARD_GET != objMessage.wParam)
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
                if (m_bIsInLoad)
                {
                    MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_LOAD_FAILED), MESSAGEBOX_NOTE, 1500);
                }
                else
                {
                    // 如果所有的账号都下载失败，则退出
                    MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_SAVE_FAILED),
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

bool CDlgGroupNightFWD::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    // 因为该界面不允许更改，softkey只需设置回退键

    objWndData[3].m_strSoftkeyAction = "";
    objWndData[3].m_strSoftkeyTitle = "";
    return true;
}

//获取FWDNotReach数据
void CDlgGroupNightFWD::GetGroupNightForwardData()
{
    MakeReadyForRelayout();
    SGroupNightForward gnfInfo;
    GNF_GetGnfInfo(gnfInfo, m_iAccountId);
    //设置Activation标签
    QWidgetPair qPair;
    QString strLab;

    m_vecWidgetPair.clear();
    strLab = LANG_TRANSLATE(TRID_GROUP_NIGHT_FORWARD);
    qPair.first = strLab;
    qPair.second = m_pLabGroupNightForward;
    m_vecWidgetPair.push_back(qPair);

    // 根据获取内容确定是否显示办公时间
    if (!gnfInfo.strBusinessHoursName.empty())
    {
        strLab = LANG_TRANSLATE(TRID_BUSINESS_HOURS);
        qPair.first = strLab;
        qPair.second = m_pLabBusinessHour;
        m_vecWidgetPair.push_back(qPair);
        m_pLabBusinessHour->setText(toQString(gnfInfo.strBusinessHoursName));
    }
    // 根据获取内容确定是否显示假期时间
    if (!gnfInfo.strHolidayScheduleName.empty())
    {
        strLab = LANG_TRANSLATE(TRID_HOLIDAY_SCHEDULE);
        qPair.first = strLab;
        qPair.second = m_pLabHolidaySchedule;
        m_vecWidgetPair.push_back(qPair);
        m_pLabHolidaySchedule->setText(toQString(gnfInfo.strHolidayScheduleName));
    }

    AddWidget(m_vecWidgetPair);
    UpdateWnd();
    SETTINGUI_DEBUG("CDlgGroupNightFWD :: gnfInfo.strUserNightFwdMode is %s.\n",
                    gnfInfo.strUserNightFwdMode.c_str());
    SETTINGUI_DEBUG("CDlgGroupNightFWD :: gnfInfo.strForwarTarget is %s.\n",
                    gnfInfo.strForwarTarget.c_str());
    m_pLabGroupNightForward->setText(toQString(gnfInfo.strUserNightFwdMode));
}

bool CDlgGroupNightFWD::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
    return false;
}


bool CDlgGroupNightFWD::IsStatusChanged()
{
    return false;
}

bool CDlgGroupNightFWD::LoadPageData()
{
    //GetGroupNightForwardData();
    return true;
}

bool CDlgGroupNightFWD::SavePageData()
{
    return false;
}


CDlgGroupNightFWD::~CDlgGroupNightFWD()
{
    etl_UnregisterMsgHandle(XSI_MESSAGE_BEGIN,
                            XSI_MESSAGE_END, OnGroupNightForward);
}

// 设置子页面前的操作
void CDlgGroupNightFWD::BeforeSetSubPage(void * pData)
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

void CDlgGroupNightFWD::ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    int iMsgID = pMessageBox->GetID();
    int iRet = pMessageBox->GetResult();
    int iMsgType = pMessageBox->GetType();

    switch (iMsgID)
    {
    case MSG_ID_LOADING:
        {
            if (iRet == MessageBox_Cancel)
            {
                GNF_CancelRequest();
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
            return;
        }
        break;
    default:
        break;
    }

    if (iMsgType == MESSAGEBOX_Cancel
            && iRet == MessageBox_Cancel)
    {
        GNF_CancelRequest();
    }
}

//事件过滤器
bool CDlgGroupNightFWD::eventFilter(QObject * pObject, QEvent * pEvent)
{
    switch (pEvent->type())
    {

    case QEvent::KeyPress:
        {
            QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;
            switch (pKeyEvent->key())
            {
            case PHONE_KEY_OK:
                {
                    return true;
                }
            default:
                break;
            }
        }
        break;
    default:
        break;
    }

    return CDlgBaseSubPage::eventFilter(pObject, pEvent);
}

void CDlgGroupNightFWD::ExtraDialogInitData()
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
            return ;
        }
#endif
        GNF_QueryGnf(m_iAccountId);
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
        MessageBox_ShowCommonMsgBox(this, LANG_TRANSLATE(TRID_LOADING_PLEASE_WAIT),
                                    MESSAGEBOX_Cancel, -1, "", "", MSG_ID_LOADING);
    }
}

// 处理密码弹窗
bool CDlgGroupNightFWD::ProcessMsgBoxPassword(int nLineId)
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
void CDlgGroupNightFWD::Init()
{

}

// 反初始化
void CDlgGroupNightFWD::Uninit()
{

}
