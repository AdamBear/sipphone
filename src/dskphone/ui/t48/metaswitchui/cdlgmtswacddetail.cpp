#if IF_FEATURE_METASWITCH_ACD
#include "cdlgmtswacddetail.h"
#include "messagebox/modmessagebox.h"

#include "uikernel/languagehelper.h"
#include "qtcommon/qmisc.h"
#include "setting/include/common.h"
#include "inputmethod/inputmethod.h"
#include "baseui/fontcolor.h"
#include "qtcommon/qmisc.h"

#include "account/include/modaccount.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "baseui/modbaseui.h"
#include "keymap.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "uikernel/qcomboboxutility.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "uimanager/moduimanager.h"
#include "baseui/t4xpicpath.h"

#include "metaswitch_inc.h"

#define MSGBOX_ID_CANCEL_DOWNLOAD_MTSW_ACD  10000

static LRESULT OnMTSWAcdDetailMsg(msgObject & objMessage)
{
    SETTINGUI_INFO("OnMTSWAcdlistMsg [%d] [%d]",
                   objMessage.message, objMessage.wParam);

    CDlgMTSWAcdDetail * pDlg = qobject_cast<CDlgMTSWAcdDetail *>(UIManager_GetPrivateDlg(
                                   DLG_CDlgMTSWAcdDetail));
    if (NULL == pDlg || !pDlg->IsTopPage())
    {
        return 0;
    }

    pDlg->OnReceiveMsg(objMessage);
    return 0;
}


CDlgMTSWAcdDetail::CDlgMTSWAcdDetail(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pLabelWaitingTime(NULL)
    , m_pLabelStatus(NULL)
    , m_pLabelCallers(NULL)
    , m_pLabelMembers(NULL)
{
    InitData();
}

CDlgMTSWAcdDetail::~CDlgMTSWAcdDetail()
{
    etl_UnregisterMsgHandle(MTSW_ACD_MESSAGE_BEGIN, MTSW_ACD_MESSAGE_END, OnMTSWAcdDetailMsg);
}

void CDlgMTSWAcdDetail::InitData()
{
    //设置标题
    m_strTitle = toQString(MTSWACD_GetCurrentMLHG().c_str());
    etl_RegisterMsgHandle(MTSW_ACD_MESSAGE_BEGIN, MTSW_ACD_MESSAGE_END, OnMTSWAcdDetailMsg);

    m_pLabelWaitingTime = new QLabel(this);
    m_pLabelCallers = new QLabel(this);
    m_pLabelMembers = new QLabel(this);
    m_pLabelStatus = new QLabel(this);

    QWidgetPair qPair;
    QString strLab;

    strLab = LANG_TRANSLATE(TRID_MTSW_QUEUE_AVG_WAITING_TIME);
    qPair.first = strLab;
    qPair.second = m_pLabelWaitingTime;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_MTSW_QUEUE_CALLERS_WAITING);
    qPair.first = strLab;
    qPair.second = m_pLabelCallers;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_MTSW_QUEUE_LOGGEDIN_MEMBERS);
    qPair.first = strLab;
    qPair.second = m_pLabelMembers;
    m_vecWidgetPair.push_back(qPair);

    strLab = LANG_TRANSLATE(TRID_MTSW_QUEUE_QUEUE_STATUS);
    qPair.first = strLab;
    qPair.second = m_pLabelStatus;
    m_vecWidgetPair.push_back(qPair);

    AddWidget(m_vecWidgetPair);

    //为界面自己安装事件过滤器
    this->installEventFilter(this);
}

void CDlgMTSWAcdDetail::GetMLHGDetail()
{
    //MakeReadyForRelayout();

    const SMLGHQueuesData * pData = MTSWACD_GetCurQueuesData();
    if (pData == NULL)
    {
        return;
    }

    // 获取当前焦点组名
    yl::string strMLHGName = MTSWACD_GetCurrentMLHG();
    m_strTitle = toQString(strMLHGName.c_str());

    char szBuff[16] = {0};

    m_pLabelWaitingTime->setText(toQString(pData->strAVGWaitingTime.c_str()));

    sprintf(szBuff, "%d", pData->nCountInQueue);
    m_pLabelCallers->setText(toQString(szBuff));

    sprintf(szBuff, "%d", pData->nLogged);
    m_pLabelMembers->setText(toQString(szBuff));

    yl::string strValue = pData->bLoggedIn ? TRID_MTSW_ACD_LOGIN : TRID_MTSW_ACD_LOGOUT;
    m_pLabelStatus->setText(toQString(strValue.c_str()));

    //AddWidget(m_vecWidgetPair, true, true);

    // 刷新标题
    emit RefreshTitle();

    UpdateWnd();
}

//收到logic层返回的消息
void CDlgMTSWAcdDetail::OnReceiveMsg(const msgObject & objMessage)
{
    SETTINGUI_INFO("CDlgMTSWAcdDetail ReceiveMsg [%d]", objMessage.message);
    switch (objMessage.message)
    {
    case MTSW_ACD_GET_RESULT:
        {
            if (objMessage.wParam == EMTSWLOGDTYPE_Queues)
            {
                MessageBox_RemoveTopMessageBox(MessageBox_OK);
                GetMLHGDetail();
            }
        }
        break;
    case MTSW_ACD_GET_FAILED:
        {
            MessageBox_RemoveTopMessageBox(MessageBox_Destory);

            // 如果所有的账号都下载失败，则回退界面
            MessageBox_ShowCommonMsgBox(this,
                                        LANG_TRANSLATE(TRID_LOAD_FAILED),
                                        MESSAGEBOX_NOTE, 1500, "", "",
                                        MSG_ID_SHOW_NOTE_BEFORE_BACK);
        }
        break;
    default:
        break;
    }
}

// 设置页面的SoftKey
bool CDlgMTSWAcdDetail::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    objWndData[3].m_strSoftkeyAction = ACTION_ACD_MEMBERS;
    objWndData[3].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_SOFTKEY_MTSW_ACD_MEMBER);
    objWndData[3].m_strIconPath = PIC_SOFTKEY_MTSW_ACD_MEMBER;
    return true;
}

// 设置子页面不可编辑时的SoftKey
bool CDlgMTSWAcdDetail::GetPageDisableSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    if (NULL == m_pGeneralFrame)
    {
        return false;
    }

    return true;
}

bool CDlgMTSWAcdDetail::LoadPageData()
{
    return false;
}

bool CDlgMTSWAcdDetail::SavePageData()
{
    return false;
}

void CDlgMTSWAcdDetail::showEvent(QShowEvent * e)
{
    OnShow();
}

void CDlgMTSWAcdDetail::OnShow()
{
    SETTINGUI_INFO("CDlgMTSWAcdDetail::OnShow()");
    DownloadMLGHDetail();
}

bool CDlgMTSWAcdDetail::OnAction(const QString & strAction)
{
    bool bHandled = true;

    if (strAction == ACTION_ACD_MEMBERS)
    {
        //进入组成员列表界面
        emit openpage(SubMenuData("mtswacdmemberlist"));
    }
    else
    {
        bHandled = false;
    }

    return bHandled;
}

void CDlgMTSWAcdDetail::ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox)
{
}

// 设置子页面前的操作
void CDlgMTSWAcdDetail::BeforeSetSubPage(void * pData)
{
}

// 从服务器下载数据
void CDlgMTSWAcdDetail::DownloadMLGHDetail()
{
    // 下载数据
    MTSWACD_DownloadData(EMTSWLOGDTYPE_Queues);

    MessageBox_ShowCommonMsgBox(this,
                                LANG_TRANSLATE(TRID_LOADING_PLEASE_WAIT),
                                MESSAGEBOX_Cancel, -1, "", "",
                                MSGBOX_ID_CANCEL_DOWNLOAD_MTSW_ACD);
}

bool CDlgMTSWAcdDetail::eventFilter(QObject * pObject, QEvent * pEvent)
{
    // 判断事件类型
    switch (pEvent->type())
    {
    case QEvent::KeyPress:
        {
            QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;
            switch (pKeyEvent->key())
            {
            case PHONE_KEY_LEFT:
                {
                    OnPaging(false);
                    return true;
                }
                break;
            case PHONE_KEY_RIGHT:
                {
                    OnPaging(true);
                    return true;
                }
                break;
            }
        }
        break;
    }

    return CDlgBaseSubPage::eventFilter(pObject, pEvent);
}

bool CDlgMTSWAcdDetail::IsStatusChanged()
{
    return false;
}

#if IF_SETTING_WND_PRE_NEXT_BTN
// 向左翻页
void CDlgMTSWAcdDetail::OnBtnPreClick()
{
    OnPaging(false);
}

// 向右翻页
void CDlgMTSWAcdDetail::OnBtnNextClick()
{
    OnPaging(true);
}
#endif

// 按左右键翻页
void CDlgMTSWAcdDetail::OnPaging(bool bNext)
{
    YLVector<SMLHGData> vecMLGHData;
    MTSWACD_GetMLHGList(vecMLGHData);
    yl::string strCurMLHGName = MTSWACD_GetCurrentMLHG();

    for (int i = 0; i < vecMLGHData.size(); i++)
    {
        if (vecMLGHData[i].strName == strCurMLHGName)
        {
            if (bNext)
            {
                if (i == vecMLGHData.size() - 1)
                {
                    i = 0;
                }
                else
                {
                    i++;
                }
            }
            else
            {
                if (i == 0)
                {
                    i = vecMLGHData.size() - 1;
                }
                else
                {
                    i--;
                }
            }
            // 重新设置当前mlhg组
            MTSWACD_SetCurrentMLHG(vecMLGHData[i].strName);

            GetMLHGDetail();
            break;
        }
    }
}
#endif
