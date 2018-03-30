#if IF_FEATURE_METASWITCH_ACD
#include "cdlgmtswacdlist.h"
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
#include "metaswitch/mtswcontrol/include/modmtsw.h"
#include "metaswitch/mtswcontrol/include/mtsw_messagedefine.h"
#include "baseui/t4xpicpath.h"
#include "threadtask/include/common.h"

#include "metaswitch_inc.h"

#define MSGBOX_ID_CANCEL_DOWNLOAD_MTSW_ACD  10000
#define MSGBOX_ID_LOGIN_LOGOUT_MTSW_ACD     10001
#define MSGBOX_ID_MULTILOGIN_MTSW_ACD       10002
#define MSGBOX_ID_CLICK_ITEM                10003
#define MSGBOX_ID_COMMPORTAL_LOGIN          10004
#define MSGBOX_ID_COMMPORTAL_LOGIN_FAIL     10005

static LRESULT OnMTSWAcdlistMsg(msgObject & objMessage)
{
    SETTINGUI_INFO("OnMTSWAcdlistMsg [%d] [%d]",
                   objMessage.message, objMessage.wParam);

    CDlgMTSWAcdList * pDlgMTSWAcdList = qobject_cast<CDlgMTSWAcdList *>(UIManager_GetPrivateDlg(
                                            DLG_CDlgMTSWAcdList));
    if (NULL == pDlgMTSWAcdList || !pDlgMTSWAcdList->IsTopPage())
    {
        return 0;
    }

    pDlgMTSWAcdList->OnReceiveMsg(objMessage);
    return 0;
}


CDlgMTSWAcdList::CDlgMTSWAcdList(QWidget * parent)
    : CDlgBaseSubPage(parent)
{
    m_iFocusIndex = 0;
    InitData();
}

CDlgMTSWAcdList::~CDlgMTSWAcdList()
{
    etl_UnregisterMsgHandle(MTSW_ACD_MESSAGE_BEGIN, MTSW_ACD_MESSAGE_END, OnMTSWAcdlistMsg);
    etl_UnregisterMsgHandle(MTSW_MESSAGE_AUTH_ERR, MTSW_MESSAGE_AUTH_ERR, OnMTSWAcdlistMsg);
}

void CDlgMTSWAcdList::InitData()
{
    //设置标题
    m_strTitle = GetAcdStatusToShow();
    etl_RegisterMsgHandle(MTSW_ACD_MESSAGE_BEGIN, MTSW_ACD_MESSAGE_END, OnMTSWAcdlistMsg);
    etl_RegisterMsgHandle(MTSW_MESSAGE_AUTH_ERR, MTSW_MESSAGE_AUTH_ERR, OnMTSWAcdlistMsg);

    //为界面自己安装事件过滤器
    this->installEventFilter(this);
}

//获取MLHG列表
void CDlgMTSWAcdList::GetMLHGList()
{
    MakeReadyForRelayout();

    for (int i = 0; i < m_vecMLGHData.size(); ++i)
    {
        QWidgetPair qPair;
        qPair.first = m_vecMLGHData[i].strName.c_str();

        QString strStatus = m_vecMLGHData[i].IsLogin() ? LANG_TRANSLATE(TRID_MTSW_ACD_LOGIN)
                            : LANG_TRANSLATE(TRID_MTSW_ACD_LOGOUT);
        QLabel * pLabel = new QLabel(this);
        if (pLabel == NULL)
        {
            return;
        }
        pLabel->setText(strStatus);
        qPair.second = pLabel;
        qPair.third = ACTION_ACD_ITEM_CLICK;

        qPair.eType = SETTING_ITEM_MTSWACDLIST;
        m_vecWidgetPair.push_back(qPair);
    }

    //设置标题
    m_strTitle = GetAcdStatusToShow();
    SETTINGUI_INFO("m_strTitle = %s, m_iFocusIndex = %d", m_strTitle.toUtf8().data(), m_iFocusIndex);

    AddWidget(m_vecWidgetPair, true, false);

    emit RefreshTitle();

    //SetCurrentIndex(m_iFocusIndex);
    UpdateWnd();
}

//收到logic层返回的消息
void CDlgMTSWAcdList::OnReceiveMsg(const msgObject & objMessage)
{
    switch (objMessage.message)
    {
    case MTSW_ACD_GET_RESULT:
        {
            SETTINGUI_INFO("CDlgMTSWAcdList MTSW_ACD_GET_RESULT w = %d", objMessage.wParam);
            if (objMessage.wParam == EMTSWLOGDTYPE_MemberList
                    || objMessage.wParam == EMTSWLOGDTYPE_MLHG
                    || objMessage.wParam == EMTSWLOGDTYPE_All)
            {
                m_vecMLGHData.clear();
                MTSWACD_GetMLHGList(m_vecMLGHData);

                // 只有当下载完成memberlist时才刷新界面
                if (objMessage.wParam == EMTSWLOGDTYPE_All
                        && m_vecMLGHData.size() > 0)
                {
                    break;
                }

                MessageBox_RemoveTopMessageBox(MessageBox_OK);
                GetMLHGList();
            }
        }
        break;
    case MTSW_ACD_GET_FAILED:
        {
            SETTINGUI_INFO("CDlgMTSWAcdList MTSW_ACD_GET_FAILED");
            if (MessageBox_GetMessageBoxCount() > 0)
            {
                MessageBox_RemoveTopMessageBox(MessageBox_Destory);

                // 如果所有的账号都下载失败，则回退界面
                MessageBox_ShowCommonMsgBox(this,
                                            LANG_TRANSLATE(TRID_LOAD_FAILED),
                                            MESSAGEBOX_NOTE, 1500, "", "",
                                            MSG_ID_SHOW_NOTE_BEFORE_BACK);
            }
        }
        break;
    case MTSW_MESSAGE_AUTH_ERR:
        {
            NS_TA::TaskActionErrorType eErrCode = (NS_TA::TaskActionErrorType)objMessage.lParam;
            if ((eErrCode == NS_TA::TA_HTTP_DOWNLOAD_UNAUTHORIZED)
                    || eErrCode == NS_TA::TA_HTTP_ERR403)
            {
                MessageBox_ShowCommonMsgBox(this,
                                            LANG_TRANSLATE(TRID_COMMPORTAL_LOGIN_FAILED),
                                            MESSAGEBOX_OkCancel, -1, "", "",
                                            MSGBOX_ID_COMMPORTAL_LOGIN_FAIL);
            }
            else
            {
                MessageBox_ShowCommonMsgBox(this,
                                            LANG_TRANSLATE(TRID_LOGIN_FAILED),
                                            MESSAGEBOX_OkCancel, -1, "", "",
                                            MSGBOX_ID_COMMPORTAL_LOGIN_FAIL);
            }
        }
        break;
    default:
        break;
    }
}

// 设置页面的SoftKey
bool CDlgMTSWAcdList::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    if (m_vecMLGHData.size() > 0)
    {
        bool bShowPreminumAcd = MTSWACD_BeHasPreminumAcd() && !MTSWACD_IsAllMLHGLogout();
        if (bShowPreminumAcd)
        {
            objWndData[0].m_strSoftkeyAction = ACTION_ACD_STATE;
            objWndData[0].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_SOFTKEY_MTSW_ACD_STATE);
            objWndData[0].m_strIconPath = PIC_SOFTKEY_MTSW_ACD_MYSTATE;
        }
    }

    objWndData[3].m_strSoftkeyAction = "";
    objWndData[3].m_strSoftkeyTitle = "";
    objWndData[3].m_strIconPath = "";

    return true;
}

// 设置子页面不可编辑时的SoftKey
bool CDlgMTSWAcdList::GetPageDisableSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    if (NULL == m_pGeneralFrame)
    {
        return false;
    }

    return true;
}

bool CDlgMTSWAcdList::LoadPageData()
{
    return false;
}

bool CDlgMTSWAcdList::SavePageData()
{
    return false;
}

void CDlgMTSWAcdList::showEvent(QShowEvent * e)
{
    OnShow();
}

void CDlgMTSWAcdList::OnShow()
{

}

bool CDlgMTSWAcdList::OnAction(const QString & strAction)
{
    bool bHandled = true;

    if (strAction == ACTION_ACD_ITEM_CLICK)
    {
        if (NULL == m_pGeneralFrame
                || m_vecMLGHData.size() <= m_pGeneralFrame->GetFocusItemDataIndex())
        {
            return false;
        }
        int iIndex  =  m_pGeneralFrame->GetFocusItemDataIndex();
        if (iIndex < 0)
        {
            return false;
        }

        SetCurrentMLHG();

        EMTSWAcdListShowLoggedMode eMode;
        if (!m_vecMLGHData[iIndex].bLoginLogoutSupport)
        {
            eMode = EMTSWACDLIST_SHOW_NONE;
        }
        else if (m_vecMLGHData[iIndex].IsLogin())
        {
            eMode = EMTSWACDLIST_SHOW_LOGOUT;
        }
        else
        {
            eMode = EMTSWACDLIST_SHOW_LOGIN;
        }

        MessageBox_ShowMTSWAcdListBox(this, toQString(m_vecMLGHData[iIndex].strName.c_str()),
                                      eMode, MSGBOX_ID_CLICK_ITEM);

        return true;
    }
    else if (strAction == ACTION_ACD_STATE)
    {
        emit openpage(SubMenuData("mtswacdmystate"));
    }
    else
    {
        bHandled = false;
    }

    return bHandled;
}

void CDlgMTSWAcdList::ProcessMsgBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    SETTING_INFO("pMessageBox->GetID() = %d, pMessageBox->GetResult() = %d ", pMessageBox->GetID(),
                 pMessageBox->GetResult());

    int iRet = pMessageBox->GetResult();
    switch (pMessageBox->GetID())
    {
    case MSGBOX_ID_MULTILOGIN_MTSW_ACD:
        {
            if (iRet == MessageBox_OK)
            {
                ACD_ReqLogin();
            }
        }
        break;
    case MSGBOX_ID_CANCEL_DOWNLOAD_MTSW_ACD:
        {
            if (iRet == MessageBox_Cancel)
            {
                emit BtnBackClick();
            }
        }
        break;
    case MSGBOX_ID_CLICK_ITEM:
        {
            if (iRet == MessageBox_MTSWACDList_Login)
            {
                if (MTSWACD_IsAllowMultipleLogin()
                        || IsAllLogout())
                {
                    MessageBox_ShowCommonMsgBox(this,
                                                LANG_TRANSLATE(TRID_MTSW_ACD_LOGINING),
                                                MESSAGEBOX_Cancel, -1, "", "",
                                                MSGBOX_ID_LOGIN_LOGOUT_MTSW_ACD);

                    // 请求登录
                    ACD_ReqLogin();
                }
                else
                {
                    // 提示是否允许登录
                    MessageBox_ShowCommonMsgBox(this,
                                                LANG_TRANSLATE(TRID_MTSW_ACD_LOGIN_TIPS),
                                                MESSAGEBOX_OkCancel, -1, "", "",
                                                MSGBOX_ID_MULTILOGIN_MTSW_ACD);
                }
            }
            else if (iRet == MessageBox_MTSWACDList_Logout)
            {
                MessageBox_ShowCommonMsgBox(this,
                                            LANG_TRANSLATE(TRID_MTSW_ACD_LOGOUTING),
                                            MESSAGEBOX_Cancel, -1, "", "",
                                            MSGBOX_ID_LOGIN_LOGOUT_MTSW_ACD);

                CallCenter_Logout();
            }
            else if (iRet == MessageBox_MTSWACDList_Detail)
            {
                //进入详情界面
                emit openpage(SubMenuData("mtswacdqueuedetail"));
            }
            else if (iRet == MessageBox_MTSWACDList_Member)
            {
                //进入组成员列表界面
                emit openpage(SubMenuData("mtswacdmemberlist"));
            }
        }
        break;
    case MSGBOX_ID_COMMPORTAL_LOGIN:
        {
            if (iRet == MessageBox_OK)
            {
                MessageBox_ShowCommonMsgBox(this,
                                            LANG_TRANSLATE(TRID_LOADING_PLEASE_WAIT),
                                            MESSAGEBOX_Cancel, -1, "", "",
                                            MSGBOX_ID_CANCEL_DOWNLOAD_MTSW_ACD);
            }
            else if (iRet == MessageBox_Cancel
                     || iRet == MessageBox_Destory)
            {
                //回到上一层界面
                emit BtnBackClick();
            }
        }
        break;
    case MSGBOX_ID_COMMPORTAL_LOGIN_FAIL:
        {
            if (iRet == MessageBox_OK)
            {
                MessageBox_ShowMTSWCommportalBox(this, MSGBOX_ID_COMMPORTAL_LOGIN);
            }
            else
            {
                //回到上一层界面
                emit BtnBackClick();
            }
        }
        break;
    default:
        {

        }
        break;
    }
}

// 设置子页面前的操作
void CDlgMTSWAcdList::BeforeSetSubPage(void * pData)
{
}

// 从服务器下载数据
void CDlgMTSWAcdList::DownloadMLGHList()
{
    // 下载数据
    if (MTSWACD_DownloadData(EMTSWLOGDTYPE_All))
    {
        MessageBox_ShowCommonMsgBox(this,
                                    LANG_TRANSLATE(TRID_LOADING_PLEASE_WAIT),
                                    MESSAGEBOX_Cancel, -1, "", "",
                                    MSGBOX_ID_CANCEL_DOWNLOAD_MTSW_ACD);
    }
}

bool CDlgMTSWAcdList::eventFilter(QObject * pObject, QEvent * pEvent)
{
    // 判断事件类型
    switch (pEvent->type())
    {
    case  QEvent::KeyPress:
        {
            QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;
            switch (pKeyEvent->key())
            {
            case PHONE_KEY_OK:
                {
                }
                break;
            default:
                break;
            }
        }
        break;
    }

    return CDlgBaseSubPage::eventFilter(pObject, pEvent);
}

bool CDlgMTSWAcdList::IsStatusChanged()
{
    return false;
}

bool CDlgMTSWAcdList::SetCurrentMLHG(void)
{
    int nIndex = m_pGeneralFrame->GetFocusItemDataIndex();
    if (nIndex >= 0
            && nIndex < m_vecMLGHData.size())
    {
        m_iFocusIndex = nIndex;
        return MTSWACD_SetCurrentMLHG(m_vecMLGHData[nIndex].strName);
    }

    return false;
}


//获取ACD状态
QString CDlgMTSWAcdList::GetAcdStatusToShow()
{
    QString strTemp = LANG_TRANSLATE(TRID_ACD);
    //如果开启了ACD,优先显示ACD状态
    if (CallCenter_IsACDOn())
    {
        if (Get_ACDType() == ACD_METASWITCH
                && !MTSWACD_IsEnableShowState())
        {
            return strTemp;
        }

        switch (ACD_GetAcdStatus())
        {
        case AS_AVAILABLE:
            strTemp = LANG_TRANSLATE(TRID_AVAILABLE);
            break;
        case AS_UNAVAILABLE:
            {
                yl::string strUnavailCode;
                //获取Unavailable Code
                ACD_GetUnavailCode(strUnavailCode);
                //如果有code就显示Code，否则显示Unavailable
                if (strUnavailCode.empty())
                {
                    strTemp = LANG_TRANSLATE(TRID_UNAVAILABLE);
                }
                else
                {
                    strTemp = toQString(strUnavailCode.c_str());
                }
            }
            break;
        case AS_WRAPUP:
            if (Get_ACDType() == ACD_METASWITCH)
            {
                yl::string strWrapup;
                //获取Unavailable Code
                strWrapup = MTSWACD_GetLastAnseredMHLG();
                //如果有code就显示Code，否则显示Unavailable
                if (strWrapup.empty())
                {
                    strTemp = LANG_TRANSLATE(TRID_WRAP_UP);
                }
                else
                {
                    strTemp = LANG_TRANSLATE(TRID_WRAP_UP);
                    strTemp.append('(');
                    strTemp.append(strWrapup.c_str());
                    strTemp.append(')');
                }
            }
            else
            {
                strTemp = LANG_TRANSLATE(TRID_WRAP_UP);
            }
            break;
        default:
            break;
        }
    }
    return strTemp;
}



void CDlgMTSWAcdList::ExtraDialogInitData()
{
    if (!MTSW_IsLogined())
    {
        MessageBox_ShowMTSWCommportalBox(this, MSGBOX_ID_COMMPORTAL_LOGIN);
    }
    else
    {
        SETTINGUI_INFO("CDlgMTSWAcdList::OnShow()");
        DownloadMLGHList();
    }
}

bool CDlgMTSWAcdList::IsAllLogout()
{
    for (int i = 0 ; i < m_vecMLGHData.size(); i++)
    {
        if (m_vecMLGHData[i].IsLogin()
                && m_vecMLGHData[i].bLoginLogoutSupport)
        {
            return false;
        }
    }

    return true;
}
#endif

