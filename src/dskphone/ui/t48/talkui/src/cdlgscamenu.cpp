/************************************************************************
 * FileName  : CDlgSCAMenu.cpp (implementation file)
 * Purpose   :
 * Date      : 2013/01/15 20:41:23
 ************************************************************************/
#include "cdlgscamenu.h"
#include "qtcommon/qmisc.h"
#include "keymap.h"
#include "uikernel/languagehelper.h"
#include "uimanager/moduimanager.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "modtalkui.h"
// #include "DssKey/Dsskey.h"
#include "dsskey/include/moddsskey.h"
#include "account/include/modaccount.h"
#include "mainwnd/mainwnd.h"
// #include "CommonAPI/CommonAPI.h"
#include "talk/broadsoftsca/include/modbroadsoft_sca.h"
#include "baseui/t4xpicpath.h"
#include "uikernel/qwidgetutility.h"
#include "baseui/t4xpicpath.h"
#include "uikernel/qscrolltextlabel.h"
#include "uimanager/rectdefine.h"
#include "imagemanager/modimagemanager.h"
#include "broadsoft/executiveassistant/include/modexecutiveassistant.h"

#define ACTION_DLG_SCAMENU_CANCEL       "cancel"
#define ACTION_DLG_SCAMENU_ANSWER       "answer"
#define ACTION_DLG_SCAMENU_REJECT       "reject"
#define ACTION_DLG_SCAMENU_SWITCH       "switch"
#define ACTION_DLG_SCAMENU_NEWCALL      "newcall"
#define ACTION_DLG_SCAMENU_BARGEIN      "bargein"
#define ACTION_DLG_SCAMENU_SILENTBARGEIN    "silentbargein"
#define ACTION_DLG_SCAMENU_RESUME       "resume"
#define ACTION_DLG_SCAMENU_RETRIEVE     "retrieve"
#define ACTION_DLG_SCAMENU_CALLPULL     "callpull"

CDlgSCAMenu::CDlgSCAMenu(QWidget * parent /*= 0*/)
    : CDlgBaseSubPage(parent)
    , m_pQuitTimer(NULL)
    , m_pScaMenuData(NULL)
    , m_nReturnInterval(10000)
{
    SetWindowType(eWT_SubPage);

    //设置顶层窗口属性
    SetTopWnd();

    // 设置窗体位置
    SetDlgGeometry(RECT_SETTING);

    InitData();

    //设置标题
    SetWindowTitle(LANG_TRANSLATE(TRID_SHARED_CALLS));

    if (m_pGeneralFrame != NULL)
    {
        m_pGeneralFrame->SetItemSelectBG(PIC_WIDGET_SOME_PAGE_SELECT_BG);
        m_pGeneralFrame->SetItemClickBG(PIC_GENERAL_LIST_ITEM_CLICK_BG_TOP,
                                        PIC_GENERAL_LIST_ITEM_CLICK_BG,
                                        PIC_GENERAL_LIST_ITEM_CLICK_BG);
        m_pGeneralFrame->SetRect(132, 0, 536, 336);
    }

    connect(this, SIGNAL(RefreshSoftkey()), this, SLOT(OnRefreshSoftkey()));
    connect(this, SIGNAL(ProcessPageAction(const QString &)), this,
            SLOT(OnProcessPageAction(const QString &)));

    //为界面自己安装事件过滤器
    this->installEventFilter(this);

    // 自动退出
    m_pQuitTimer = new QTimer(this);
    if (m_pQuitTimer != NULL)
    {
        connect(m_pQuitTimer, SIGNAL(timeout()), this, SLOT(OnTimer()));
    }
}

CDlgSCAMenu::~CDlgSCAMenu()
{

}

void CDlgSCAMenu::Init()
{

}

// 释放数据，将窗口清空回复到初始化状态
void CDlgSCAMenu::Uninit()
{
}

void CDlgSCAMenu::SetData(void * pData)
{

    TALKUI_INFO("CSCAMenuUI::SetData");

    if (NULL == pData)
    {
        TALKUI_INFO("CSCAMenuUI: Unexpected null pointer !!");
        return;
    }

    // 赋值
    ScaMenuData * pMenuData = (ScaMenuData *)pData;
    if (pMenuData->pScaInfoList == NULL)
    {
        return;
    }

    // 从数据中获取显示菜单内容
    if (!SetSCAMenuData(*pMenuData->pScaInfoList, pMenuData->iAccount))
    {
        TALKUI_WARN("Failed to get SCA menu data !");
        return;
    }

    // 保存数据指针
    m_pScaMenuData = pMenuData;

    //刷新softkey
    UpdateSoftKey();

    //不自动退出到idle
    SetRejectReturnToIdle();

    if (NULL != m_pGeneralFrame)
    {
        m_pGeneralFrame->SelectItemByDataIndex(0);
    }

    m_nReturnInterval = pMenuData->nReturnTime;
    if (NULL != m_pQuitTimer)
    {
        m_pQuitTimer->start(m_nReturnInterval);
    }
}

bool CDlgSCAMenu::LoadPageData()
{
    return true;
}

bool CDlgSCAMenu::SavePageData()
{
    return false;
}

//http://10.3.5.199/Bug.php?BugID=65420
//实际上  数据是不可能为空的
//获取Softkey数据
void CDlgSCAMenu::GetSoftkey(CArraySoftKeyBarData & objSoftkeyData)
{
    UpdateSoftKey();

    objSoftkeyData = m_SoftkeyData;
}

bool CDlgSCAMenu::OnAction(const QString & strAction)
{
    bool bHandled = true;
    if (strAction == ACTION_DLG_SCAMENU_CANCEL)
    {
        bHandled = true;
    }
    else if (strAction == ACTION_DLG_SCAMENU_ANSWER)
    {
        OnAnswerPress();
    }
    else if (strAction == ACTION_DLG_SCAMENU_REJECT)
    {
        //如果不为拒接模式
        if (1 == configParser_GetConfigInt(kszRejectIncomingCallSwitch)
           )
        {
            OnRejectPress();
        }
    }
    else if (strAction == ACTION_DLG_SCAMENU_SWITCH)
    {
        OnSwitchPress();
    }
    else if (strAction == ACTION_DLG_SCAMENU_NEWCALL)
    {
        OnNewCallPress();
    }
    else if (strAction == ACTION_DLG_SCAMENU_BARGEIN)
    {
        OnBargeInPress();
    }
    else if (strAction == ACTION_DLG_SCAMENU_RESUME)
    {
        OnResumePress();
    }
    else if (strAction == ACTION_DLG_SCAMENU_CALLPULL)
    {
        OnCallPullPress();
    }
    else if (strAction == ACTION_DLG_SCAMENU_RETRIEVE)
    {
        OnRetrievePress();
    }
    else if (strAction == ACTION_DLG_SCAMENU_SILENTBARGEIN)
    {
        OnSilentBargeInPress();
    }
    else
    {
        bHandled = false;
    }

    // 如果处理了，要取消界面
    if (bHandled)
    {
        OnCancel();
    }

    return bHandled;
}

//处理new call事件
void CDlgSCAMenu::OnNewCallPress()
{
    // 转逻辑处理
    talklogic_EnterDialUI("", GetBindAccountID());
}

//处理cancle事件
void CDlgSCAMenu::OnCancel()
{
    talkui_ExitSCAMenu();
}

//处理Answer事件
void CDlgSCAMenu::OnAnswerPress()
{
    if (NULL == m_pGeneralFrame)
    {
        return;
    }

    SCA_Info * pInfo = GetInfoByID(m_pGeneralFrame->GetFocusItemDataIndex());
    if (NULL == pInfo)
    {
        TALKUI_WARN("CSCAMenuUI:: Can't get SCA Info !");
        return;
    }

    // 转逻辑处理
    DataLogic2UI objData;
    talklogic_GetDataByAccountBlaIndex(GetBindAccountID(), pInfo->iIndex, objData);

    talklogic_UIEvent(objData.iSessionID, TALK_ACTION_ANSWER);
}

//处理Reject事件
void CDlgSCAMenu::OnRejectPress()
{
    if (NULL == m_pGeneralFrame)
    {
        return;
    }

    SCA_Info * pInfo = GetInfoByID(m_pGeneralFrame->GetFocusItemDataIndex());
    if (NULL == pInfo)
    {
        TALKUI_WARN("CSCAMenuUI:: Can't get SCA Info !");
        return;
    }

    // 转逻辑处理
    DataLogic2UI objData;
    talklogic_GetDataByAccountBlaIndex(GetBindAccountID(), pInfo->iIndex, objData);

    talklogic_UIEvent(objData.iSessionID, TALK_ACTION_REJECT);
}

//处理switch事件
void CDlgSCAMenu::OnSwitchPress()
{
    if (NULL == m_pGeneralFrame)
    {
        return;
    }

    SCA_Info * pInfo = GetInfoByID(m_pGeneralFrame->GetFocusItemDataIndex());
    if (NULL == pInfo)
    {
        TALKUI_WARN("CSCAMenuUI:: Can't get SCA Info !");
        return;
    }

    // 转逻辑处理
    DataLogic2UI objData;
    talklogic_GetDataByAccountBlaIndex(GetBindAccountID(), pInfo->iIndex, objData);
    // 设置为焦点
    talklogic_SetFocusSessionByID(objData.iSessionID);
}

//处理bargein事件
void CDlgSCAMenu::OnBargeInPress()
{
    OnResumePress();
}

//处理resume事件
void CDlgSCAMenu::OnResumePress()
{
    if (NULL == m_pGeneralFrame)
    {
        return;
    }

    SCA_Info * pInfo = GetInfoByID(m_pGeneralFrame->GetFocusItemDataIndex());
    if (NULL == pInfo)
    {
        TALKUI_WARN("CSCAMenuUI:: Can't get SCA Info !");
        return;
    }

    talklogic_CallOutBySCALine(GetBindAccountID(), pInfo->iIndex);
}

void CDlgSCAMenu::OnCallPullPress()
{
    yl::string strCallPullCode = "";
    if (m_pScaMenuData != NULL)
    {
        strCallPullCode = configParser_GetCfgItemStringValue(kszAccountCallPullFeatureCode,
                          m_pScaMenuData->iAccount);
    }

    if (strCallPullCode.empty())
    {
        talklogic_EnterDialUI(strCallPullCode, GetBindAccountID());
    }
    else
    {
        talklogic_CallOutBySCALine(GetBindAccountID(), 0, strCallPullCode);
    }
}

void CDlgSCAMenu::OnRetrievePress()
{
    OnResumePress();
}


bool CDlgSCAMenu::eventFilter(QObject * pObject, QEvent * pEvent)
{
    // 不处理非自身的消息
    if (!CheckEventValid(pObject, pEvent))
    {
        return false;
    }

    switch (pEvent->type())
    {
    case QEvent::MouseButtonPress:
        {
            if (NULL != m_pQuitTimer)
            {
                m_pQuitTimer->start(m_nReturnInterval);
            }
        }
        break;
    case QEvent::KeyPress:
        {
            if (NULL != m_pQuitTimer)
            {
                m_pQuitTimer->start(m_nReturnInterval);
            }

            QKeyEvent * pKeyEvent = static_cast<QKeyEvent *>(pEvent);
            int nKey = pKeyEvent->key();
            switch (nKey)
            {
            case PHONE_KEY_OK:
            case PHONE_KEY_CANCEL:
                {
                    // 设置返回值
                    BOOL bOk = (PHONE_KEY_OK == nKey);

                    // 确认是否是账号绑定来电
                    BOOL bRing = FALSE;
                    SCA_Info * pInfo = GetInfoByID(m_pGeneralFrame->GetFocusItemDataIndex());
                    if (pInfo != NULL
                            && CN_BLA_STATUS_ALERTING == pInfo->eStatus)
                    {
                        DataLogic2UI objData;
                        talklogic_GetDataByAccountBlaIndex(GetBindAccountID(), pInfo->iIndex, objData);
                        if (talklogic_GetSessionStateBySessionID(objData.iSessionID) == SESSION_RINGING)
                        {
                            bRing = TRUE;
                        }
                    }

                    // 按键处理
                    if (bRing)
                    {
                        OnAction(bOk ? ACTION_DLG_SCAMENU_ANSWER : ACTION_DLG_SCAMENU_REJECT);
                    }
                    else
                    {
                        OnAction(bOk ? m_SoftkeyData[3].m_strSoftkeyAction.c_str() : ACTION_DLG_SCAMENU_CANCEL);
                    }

                    return true;
                }
                break;
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

//子界面可单独设置其背景图片，返回图片路径，如果图片为空，则按模板设置
QPixmap CDlgSCAMenu::GetCustomBackGround()
{
    return THEME_GET_BMP(PIC_GENERAL_DLG_NORMAL_BACKGROUND);
}

void CDlgSCAMenu::InitData()
{
    m_SoftkeyData[0].m_strSoftkeyAction = "";
    m_SoftkeyData[0].m_strSoftkeyTitle  = "";

    m_SoftkeyData[1].m_strSoftkeyAction = "";
    m_SoftkeyData[1].m_strSoftkeyTitle  = "";

    m_SoftkeyData[2].m_strSoftkeyAction = "";
    m_SoftkeyData[2].m_strSoftkeyTitle  = "";

    m_SoftkeyData[3].m_strSoftkeyAction = "";
    m_SoftkeyData[3].m_strSoftkeyTitle  = "";
}

// 根据SCA数据，获取SCA显示内容
BOOL CDlgSCAMenu::SetSCAMenuData(ListSCAInfo & refSCAList, int iAccountID)
{
    if (m_pGeneralFrame == NULL)
    {
        return false;
    }

    if (refSCAList.size() <= 0)
    {
        return FALSE;
    }

    // clear old data
    int iOldFocus = m_pGeneralFrame->GetFocusItemDataIndex();
    m_pGeneralFrame->ClearContent(true);
    m_vecWidgetPair.clear();

    yl::string strNameByStatus = "";
    yl::string strDisplayName = "";
    QWidgetPair qPair;
    ListSCAInfo::iterator it = refSCAList.begin();
    ListSCAInfo::iterator itEnd = refSCAList.end();
    for (; it != itEnd; ++it)
    {
        // Private Hold状态无法接起,故不显示在界面
        SCA_Info * pInfo = (SCA_Info *)(*it);
        if (NULL == pInfo
                || CN_BLA_STATUS_HELD_PRIVATE == pInfo->eStatus)
        {

            continue;
        }

        BLF_STATUS_CODE eGetStatus = bsSCA_GetStatus2UIShow(iAccountID,
                                     pInfo->iIndex);// 该账号分线的状态

        strNameByStatus = GetNameByStatus(eGetStatus);
        strDisplayName = GetDisplayName(pInfo);

        qScrollTextLabel * pLabel = new qScrollTextLabel(this);
        if (NULL == pLabel)
        {
            continue;
        }
        pLabel->setText(QString::fromUtf8(strDisplayName.c_str()));

        qPair.first = QString::fromUtf8(strNameByStatus.c_str());
        qPair.second = pLabel;
        qPair.bChangeStyleOnPress = true;
        m_vecWidgetPair.push_back(qPair);
    }

    AddWidget(m_vecWidgetPair, true, false, true, true);

    // set focus item
    iOldFocus = qMax(iOldFocus, 0);
    iOldFocus = qMin(iOldFocus, m_pGeneralFrame->GetTotalDataCount() - 1);
    SetDefaultIndex(iOldFocus);
    return TRUE;
}

// 根据SCA状态获取状态显示名
yl::string CDlgSCAMenu::GetNameByStatus(BLF_STATUS_CODE eStatus)
{
    switch (eStatus)
    {
    case CN_BLA_STATUS_ALERTING:
        return LANG_TRANSLATE(TRID_SCA_ALERTING).toUtf8().data();
    case CN_BLA_STATUS_ACTIVE:
        return LANG_TRANSLATE(TRID_SCA_ACTIVE).toUtf8().data();
    case CN_BLA_STATUS_HELD:
        return LANG_TRANSLATE(TRID_SCA_HELD).toUtf8().data();
    case CN_BLA_STATUS_HELD_PRIVATE:
        return LANG_TRANSLATE(TRID_SCA_HELD_PRIVATE).toUtf8().data();
    case CN_BLA_STATUS_BRIDGE_ACITVE:
        return LANG_TRANSLATE(TRID_SCA_BRIDGE_ACTIVE).toUtf8().data();
    case CN_BLA_STATUS_BRIDGE_HELD:
        return LANG_TRANSLATE(TRID_SCA_BRIDGE_HELD).toUtf8().data();
    default:
        return LANG_TRANSLATE(TRID_SCA_UNKNOWN).toUtf8().data();
    }
}

// 获取SCA账号分线的显示名
yl::string CDlgSCAMenu::GetDisplayName(SCA_Info * pInfo)
{
    yl::string strRet = LANG_TRANSLATE(TRID_UNKNOWN).toUtf8().data();
    if (pInfo == NULL)
    {
        return strRet;
    }

    strRet = pInfo->strDisplay;
    if (strRet.empty()
            ||  LANG_TRANSLATE(TRID_UNKNOWN).toUtf8().data() == strRet)
    {
        strRet = pInfo->strURI;
    }

    if ((strRet.empty() ||  LANG_TRANSLATE(TRID_UNKNOWN).toUtf8().data() == strRet)
            && m_pScaMenuData != NULL)
    {
        SCA_Info * pCallIn = bsSCA_GetCallInData(m_pScaMenuData->iAccount, pInfo->iIndex);
        if (pCallIn != NULL)
        {
            strRet = pCallIn->strDisplay;
            if (strRet.empty()
                    ||  LANG_TRANSLATE(TRID_UNKNOWN).toUtf8().data() == strRet)
            {
                strRet = pCallIn->strURI;
            }
        }
    }

    if (strRet.empty())
    {
        strRet =  LANG_TRANSLATE(TRID_UNKNOWN).toUtf8().data();
    }

    return strRet;
}

// 获取当前焦点对应的账号分线信息
SCA_Info * CDlgSCAMenu::GetInfoByID(UINT uIndex)
{
    if (NULL == m_pScaMenuData)
    {
        TALKUI_INFO("Warning! SCA Menu Data Pointer is NULl!");
        return NULL;
    }

    ListSCAInfo * pScaInfoList = m_pScaMenuData->pScaInfoList;
    if (pScaInfoList != NULL
            && uIndex < (UINT)pScaInfoList->size())
    {
        ListSCAInfo::iterator it = pScaInfoList->begin();
        ListSCAInfo::iterator itEnd = pScaInfoList->end();
        for (UINT uTemp = 0; it != itEnd; uTemp++, ++it)
        {
            // Private Hold状态无法接起,故不显示在界面
            SCA_Info * pInfo = (SCA_Info *)(*it);
            if (NULL == pInfo
                    || CN_BLA_STATUS_HELD_PRIVATE == pInfo->eStatus)
            {
                uTemp--;
                continue;
            }

            if (uTemp == uIndex)
            {
                return (SCA_Info *)(*it);
            }
        }
    }

    return NULL;
}

// 更新Softkey
VOID CDlgSCAMenu::UpdateSoftKey()
{
    if (NULL == m_pGeneralFrame)
    {
        return;
    }

    // 这里可以由logic提供一个接口
    SCA_Info * pInfo = GetInfoByID(m_pGeneralFrame->GetFocusItemDataIndex());
    if (NULL == pInfo)
    {
        return;
    }

    m_SoftkeyData[0].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_CANCEL);
    m_SoftkeyData[0].m_strSoftkeyAction = ACTION_DLG_SCAMENU_CANCEL;
    m_SoftkeyData[0].m_strIconPath = PIC_SOFTKEY_CANCEL;

    //
    DataLogic2UI objData;
    talklogic_GetDataByAccountBlaIndex(GetBindAccountID(), pInfo->iIndex, objData);
    SESSION_STATE eSessionState = talklogic_GetSessionStateBySessionID(objData.iSessionID);

    if (SESSION_UNKNOWN != eSessionState)
    {
        m_SoftkeyData[1].m_strSoftkeyTitle  = "";
        m_SoftkeyData[1].m_strSoftkeyAction = "";
        if (SESSION_RINGING == eSessionState)
        {
            m_SoftkeyData[2].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_ANSWER);
            m_SoftkeyData[2].m_strSoftkeyAction = ACTION_DLG_SCAMENU_ANSWER;
            m_SoftkeyData[2].m_strIconPath = PIC_SOFTKEY_ANSWER;

            //如果不为拒接模式
            if (1 == configParser_GetConfigInt(kszRejectIncomingCallSwitch)
               )
            {
                m_SoftkeyData[3].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_REJECT);
                m_SoftkeyData[3].m_strSoftkeyAction = ACTION_DLG_SCAMENU_REJECT;
                m_SoftkeyData[3].m_strIconPath = PIC_SOFTKEY_REJECT;
            }
            else
            {
                m_SoftkeyData[3].m_strSoftkeyTitle = "";
                m_SoftkeyData[3].m_strSoftkeyAction = "";
            }
        }
        else if (objData.iSessionID != talklogic_GetFocusedSessionID())
        {
            m_SoftkeyData[2].m_strSoftkeyTitle  = "";
            m_SoftkeyData[2].m_strSoftkeyAction = "";

            m_SoftkeyData[3].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_SWITCH);
            m_SoftkeyData[3].m_strSoftkeyAction = ACTION_DLG_SCAMENU_SWITCH;
            m_SoftkeyData[3].m_strIconPath = PIC_SOFTKEY_SWITCH;
        }
        else
        {
            m_SoftkeyData[2].m_strSoftkeyTitle  = "";
            m_SoftkeyData[2].m_strSoftkeyAction = "";

            m_SoftkeyData[3].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_NEW_CALL);
            m_SoftkeyData[3].m_strSoftkeyAction = ACTION_DLG_SCAMENU_NEWCALL;
            m_SoftkeyData[3].m_strIconPath = PIC_SOFTKEY_NEWCALL;
        }
    }
    else
    {
        if (configParser_GetCfgItemIntValue(kszAccountCallPullEnable, GetBindAccountID()) != 1)
        {
            m_SoftkeyData[1].m_strSoftkeyTitle  = "";
            m_SoftkeyData[1].m_strSoftkeyAction = "";
            m_SoftkeyData[1].m_strIconPath = "";
        }
        else
        {
            m_SoftkeyData[1].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_CALL_PULL);
            m_SoftkeyData[1].m_strSoftkeyAction = ACTION_DLG_SCAMENU_CALLPULL;
            m_SoftkeyData[1].m_strIconPath = PIC_SOFTKEY_CALLPULL;
        }

        bool bEnabelBarginIn = configParser_GetCfgItemIntValue(kszAccountBargeInEnable,
                               GetBindAccountID()) != 0;

        switch (pInfo->eStatus)
        {
        case CN_BLA_STATUS_ALERTING:
            {
                m_SoftkeyData[1].m_strSoftkeyTitle  = "";
                m_SoftkeyData[1].m_strSoftkeyAction = "";
                m_SoftkeyData[1].m_strIconPath = "";

                m_SoftkeyData[2].m_strSoftkeyTitle  = "";
                m_SoftkeyData[2].m_strSoftkeyAction = "";

                m_SoftkeyData[3].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_NEW_CALL);
                m_SoftkeyData[3].m_strSoftkeyAction = ACTION_DLG_SCAMENU_NEWCALL;
                m_SoftkeyData[3].m_strIconPath = PIC_SOFTKEY_NEWCALL;
            }
            break;
        case CN_BLA_STATUS_BRIDGE_ACITVE:
        case CN_BLA_STATUS_ACTIVE:
            {
#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
                bool bExecutiveEnable = BWExeAssis_IsSilentMonitorEnable(GetBindAccountID());
                if (bExecutiveEnable)
                {
                    m_SoftkeyData[2].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_DSSKEY_SILENT_MONITOR);
                    m_SoftkeyData[2].m_strSoftkeyAction = ACTION_DLG_SCAMENU_SILENTBARGEIN;
                    m_SoftkeyData[2].m_strIconPath = PIC_SOFTKEY_SILENT_BARGR_IN;
                }
                else
#endif
                {
                    m_SoftkeyData[2].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_NEW_CALL);
                    m_SoftkeyData[2].m_strSoftkeyAction = ACTION_DLG_SCAMENU_NEWCALL;
                    m_SoftkeyData[2].m_strIconPath = PIC_SOFTKEY_NEWCALL;
                }

                if (bEnabelBarginIn)
                {
                    m_SoftkeyData[3].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_BARGE_IN);
                    m_SoftkeyData[3].m_strSoftkeyAction = ACTION_DLG_SCAMENU_BARGEIN;
                    m_SoftkeyData[3].m_strIconPath = PIC_SOFTKEY_BARGR_IN;
                }
                else
                {
                    m_SoftkeyData[3].m_strSoftkeyTitle  = "";
                    m_SoftkeyData[3].m_strSoftkeyAction = "";
                    m_SoftkeyData[3].m_strIconPath = "";
                }
            }
            break;
        case CN_BLA_STATUS_HELD:
            {
                m_SoftkeyData[2].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_NEW_CALL);
                m_SoftkeyData[2].m_strSoftkeyAction = ACTION_DLG_SCAMENU_NEWCALL;
                m_SoftkeyData[2].m_strIconPath = PIC_SOFTKEY_NEWCALL;

                if (!bsSCA_GetDsskeySessionByStatus(GetBindAccountID(), CN_BLA_STATUS_HELD))
                {
                    m_SoftkeyData[3].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_RETRIEVE);
                }
                else
                {
                    m_SoftkeyData[3].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_RESUME);
                }
                m_SoftkeyData[3].m_strSoftkeyAction = ACTION_DLG_SCAMENU_RESUME;
                m_SoftkeyData[3].m_strIconPath = PIC_SOFTKEY_RESUME;
            }
            break;
        case CN_BLA_STATUS_BRIDGE_HELD:
            {
                m_SoftkeyData[2].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_NEW_CALL);
                m_SoftkeyData[2].m_strSoftkeyAction = ACTION_DLG_SCAMENU_NEWCALL;
                m_SoftkeyData[2].m_strIconPath = PIC_SOFTKEY_NEWCALL;

                if (!bsSCA_GetDsskeySessionByStatus(GetBindAccountID(), CN_BLA_STATUS_BRIDGE_HELD))
                {
                    if (bEnabelBarginIn)
                    {
                        m_SoftkeyData[3].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_BARGE_IN);
                        m_SoftkeyData[3].m_strSoftkeyAction = ACTION_DLG_SCAMENU_BARGEIN;
                        m_SoftkeyData[3].m_strIconPath = PIC_SOFTKEY_BARGR_IN;
                    }
                    else
                    {
                        m_SoftkeyData[3].m_strSoftkeyTitle  = "";
                        m_SoftkeyData[3].m_strSoftkeyAction = "";
                        m_SoftkeyData[3].m_strIconPath = "";
                    }
                }
                else
                {
                    m_SoftkeyData[3].m_strSoftkeyTitle  = LANG_TRANSLATE(TRID_RESUME);
                    m_SoftkeyData[3].m_strSoftkeyAction = ACTION_DLG_SCAMENU_RESUME;
                    m_SoftkeyData[3].m_strIconPath = PIC_SOFTKEY_RESUME;
                }
            }
            break;
        default:
            break;
        }
    }
}

void CDlgSCAMenu::OnProcessPageAction(const QString & strAction)
{
    OnAction(strAction);
}

void CDlgSCAMenu::OnRefreshSoftkey()
{
    // 刷新softkey
    _MainWND.DrawSoftkey(this);

}

// 子界面响应Back按钮的操作
void CDlgSCAMenu::OnBackButtonClick()
{
    OnAction(ACTION_DLG_SCAMENU_CANCEL);
}

// 子界面响应Home按钮的操作
void CDlgSCAMenu::OnHomeButtonClick()
{
    OnAction(ACTION_DLG_SCAMENU_CANCEL);
    CBaseDialog::OnHomeButtonClick();
}

int CDlgSCAMenu::GetBindAccountID()
{
    return (m_pScaMenuData == NULL) ? -1 : m_pScaMenuData->iAccount;
}

void CDlgSCAMenu::OnTimer()
{
    //时间到退出
    OnAction(ACTION_DLG_SCAMENU_CANCEL);
}

// 查看页面状态是否改变
bool CDlgSCAMenu::IsStatusChanged()
{
    return false;
}

void CDlgSCAMenu::OnSilentBargeInPress()
{
#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    if (NULL == m_pGeneralFrame)
    {
        return;
    }

    SCA_Info * pInfo = GetInfoByID(m_pGeneralFrame->GetFocusItemDataIndex());
    if (NULL == pInfo)
    {
        TALKUI_WARN("CSCAMenuUI:: Can't get SCA Info !");
        return;
    }

    talklogic_CallOutBySCALine(GetBindAccountID(), pInfo->iIndex, "",  true);
#endif
}
