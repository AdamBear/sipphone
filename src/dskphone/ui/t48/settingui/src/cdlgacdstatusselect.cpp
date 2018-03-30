/************************************************************************
 * FileName  : CDlgACDStatusSelect.cpp (implementation file)
 * Purpose   :
 * Date      : 2012/12/22 11:56:36
 ************************************************************************/

#include "cdlgacdstatusselect.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "uimanager/rectdefine.h"
#include "settingui/include/modsettingui.h"
#include "uimanager/moduimanager.h"
#include "etlmsghandler/modetlmsghandler.h"

namespace
{
#define ACTION_SIGN_IN          "SignIn"
#define ACTION_AVAILABEL        "Available"
#define ACTION_UN_AVAILABEL     "Unavailable"
#define ACTION_WRAP_UP          "Wrap-Up"
#define ACTION_SIGN_OUT         "SignOut"

ACD_STATUS ACDActionToType(const QString & strAction)
{
    ACD_STATUS eACDStatus = AS_IDLE;
    if (ACTION_AVAILABEL == strAction)
    {
        eACDStatus = AS_AVAILABLE;
    }
    else if (ACTION_UN_AVAILABEL == strAction)
    {
        eACDStatus = AS_UNAVAILABLE;
    }
    else if (ACTION_WRAP_UP == strAction)
    {
        eACDStatus = AS_WRAPUP;
    }
    else if (ACTION_SIGN_OUT == strAction)
    {
        eACDStatus = AS_IDLE;
    }

    return eACDStatus;
}
}


QString ACDTypeToAction(ACD_STATUS eACDStatus)
{
    QString strAction;
    if (AS_AVAILABLE == eACDStatus)
    {
        strAction = ACTION_AVAILABEL;
    }
    else if (AS_UNAVAILABLE == eACDStatus)
    {
        strAction = ACTION_UN_AVAILABEL;
    }
    else if (AS_WRAPUP == eACDStatus)
    {
        // genesys wrapup状态被映射到了unavail上
        strAction = Get_ACDType() == ACD_GENESYS ? ACTION_UN_AVAILABEL : ACTION_WRAP_UP;
    }
    else if (AS_IDLE == eACDStatus)
    {
        strAction = ACTION_SIGN_OUT;
    }

    return strAction;
}

static LRESULT OnACDStatusSelectPageMsg(msgObject & objMessage)
{
    SETTINGUI_INFO("OnACDStatusSelectPageMsg objMessage [%d]", objMessage.message);
    CDlgACDStatusSelect * pDlg = qobject_cast<CDlgACDStatusSelect *>(UIManager_GetPrivateDlg(
                                     DLG_CDlgACDStatusSelect));
    if (NULL == pDlg || !pDlg->IsTopPage())
    {
        return 0;
    }

    pDlg->OnReceiveMsg(objMessage);

    return 1;
}

CDlgACDStatusSelect::CDlgACDStatusSelect(QWidget * parent /*= 0*/)
    : CDlgBaseSubPage(parent)
{
    InitData();
}

CDlgACDStatusSelect::~CDlgACDStatusSelect()
{
}

void CDlgACDStatusSelect::Init()
{
    etl_RegisterMsgHandle(ACD_MESSAGE_STATUS_CHANGE,
                          ACD_MESSAGE_STATUS_CHANGE, OnACDStatusSelectPageMsg);
}

void CDlgACDStatusSelect::Uninit()
{
    etl_UnregisterMsgHandle(ACD_MESSAGE_STATUS_CHANGE,
                            ACD_MESSAGE_STATUS_CHANGE, OnACDStatusSelectPageMsg);
}

// 读取页面数据
bool CDlgACDStatusSelect::LoadPageData()
{
    this->setFocus();
    if (m_vecWidgetPair.size() == 0)
    {
        return false;
    }

    if (m_pGeneralFrame != NULL)
    {
        //获取ACD状态
        SelectIndexByAction(ACDTypeToAction(ACD_GetAcdStatus()));
    }

    return true;
}

void CDlgACDStatusSelect::SelectIndexByAction(QString strAction)
{
    if (NULL != m_pListDelegate)
    {
        m_pListDelegate->SelectIndexByAction(strAction);
        SetConfigedIndex(m_pListDelegate->GetCurrentIndex());
    }
}

// 查看页面状态是否改变
bool CDlgACDStatusSelect::IsStatusChanged()
{
    if (GetCurrentItemAction() != ACDTypeToAction(ACD_GetAcdStatus()))
    {
        return true;
    }

    return false;
}

// 保存页面数据
bool CDlgACDStatusSelect::SavePageData()
{
    if (m_pGeneralFrame != NULL)
    {
        //获取ACD状态
        ACD_STATUS eAcdState = ACD_GetAcdStatus();
        QString strCurrentAction = GetCurrentItemAction();

        if (strCurrentAction.isEmpty())
        {
            return false;
        }

        if (Get_ACDType() == ACD_BROADSOFT)
        {
            if (strCurrentAction == ACTION_SIGN_IN)
            {
                if (AS_IDLE == eAcdState)
                {
                    //如果是处于登出状态，则登录
                    ACD_ReqChangeACDStatus(AA_LOGIN);
                }
            }
            else if (strCurrentAction == ACTION_AVAILABEL)
            {
                if (AS_AVAILABLE != eAcdState)
                {
                    //设置ACD状态为Available
                    ACD_ReqChangeACDStatus(AA_AVAIL);
                }
            }
            else if (strCurrentAction == ACTION_UN_AVAILABEL)
            {
                if (AS_UNAVAILABLE != eAcdState)
                {
                    if (CallCenter_IsUnavailReasonCodeOn())
                    {
                        emit openpage("unavailable_reason");
                        return false;
                    }
                    else
                    {
                        //设置ACD为Unavailable
                        ACD_ReqChangeACDStatus(AA_UNAVAIL);
                    }
                }
            }
            else if (strCurrentAction == ACTION_WRAP_UP)
            {
                if (AS_WRAPUP != eAcdState)
                {
                    //设置ACD状态为Wrap-up
                    ACD_ReqChangeACDStatus(AA_WRAPUP);
                }
            }
            else if (strCurrentAction == ACTION_SIGN_OUT)
            {
                if (AS_IDLE != eAcdState)
                {
                    //登出ACD
                    ACD_ReqChangeACDStatus(AA_LOGOUT);
                }
            }
        }
        else
        {
            if (strCurrentAction == ACTION_AVAILABEL)
            {
                if (AS_AVAILABLE != eAcdState)
                {
                    //设置ACD状态为Available
                    ACD_ReqChangeACDStatus(AA_AVAIL);
                }
            }
            else if (strCurrentAction == ACTION_UN_AVAILABEL)
            {
                if (AS_UNAVAILABLE != eAcdState)
                {
#if IF_FEATURE_START2START_ACD
                    if (CallCenter_IsUnavailReasonCodeOn())
                    {
                        emit openpage("unavailable_reason");
                        return false;
                    }
                    else
#endif
                    {
                        //设置ACD为Unavailable
                        ACD_ReqChangeACDStatus(AA_UNAVAIL);
                    }
                }
            }
            else if (strCurrentAction == ACTION_SIGN_OUT)
            {
                if (AS_IDLE != eAcdState)
                {
                    //登出ACD
                    ACD_ReqChangeACDStatus(AA_LOGOUT);
                }
            }
        }
    }

    return true;
}

void CDlgACDStatusSelect::SetData(void * pData /* = NULL */)
{
//
}

void CDlgACDStatusSelect::InitData()
{
    //设置标题
    m_strTitle = LANG_TRANSLATE(TRID_ACD_STATE).toUtf8().data();

    if (m_pGeneralFrame != NULL)
    {
        this->setFocus();

        m_vecWidgetPair.clear();
        QWidgetPair qPair;
        qPair.eType = SETTING_ITEM_RIDIO;

        if (Get_ACDType() == ACD_BROADSOFT)
        {
            qPair.first = LANG_TRANSLATE(TRID_SIGN_IN);
            qPair.third = ACTION_SIGN_IN;
            m_vecWidgetPair.push_back(qPair);

            qPair.first = LANG_TRANSLATE(TRID_AVAILABLE);
            qPair.third = ACTION_AVAILABEL;
            m_vecWidgetPair.push_back(qPair);

            qPair.first = LANG_TRANSLATE(TRID_UNAVAILABLE);
            qPair.third = ACTION_UN_AVAILABEL;
            m_vecWidgetPair.push_back(qPair);

            qPair.first = LANG_TRANSLATE(TRID_WRAP_UP);
            qPair.third = ACTION_WRAP_UP;
            m_vecWidgetPair.push_back(qPair);

            qPair.first = LANG_TRANSLATE(TRID_SIGN_OUT);
            qPair.third = ACTION_SIGN_OUT;
            m_vecWidgetPair.push_back(qPair);
        }
        else
        {
            qPair.first = LANG_TRANSLATE(TRID_AVAILABLE);
            qPair.third = ACTION_AVAILABEL;
            m_vecWidgetPair.push_back(qPair);

            qPair.first = LANG_TRANSLATE(TRID_UNAVAILABLE);
            qPair.third = ACTION_UN_AVAILABEL;
            m_vecWidgetPair.push_back(qPair);

            qPair.first = LANG_TRANSLATE(TRID_SIGN_OUT);
            qPair.third = ACTION_SIGN_OUT;
            m_vecWidgetPair.push_back(qPair);
        }

        AddWidget(m_vecWidgetPair, false, false, true, false);
    }

    this->installEventFilter(this);
}

//收到logic层返回的消息
void CDlgACDStatusSelect::OnReceiveMsg(const msgObject & objMessage)
{
    if (m_pGeneralFrame == NULL)
    {
        return;
    }

    ACD_STATUS eACDStatus = (ACD_STATUS)objMessage.lParam;
    SETTINGUI_INFO("CDlgACDStatusSelect::OnReceiveMsg status [%d]", eACDStatus);

    SelectIndexByAction(ACDTypeToAction(eACDStatus));
}
