#include "cdlgcustomforwardoption.h"
#include "uikernel/languagehelper.h"
#include "baseui/fontcolor.h"
#include "setting/include/modmenu.h"
#include "qtcommon/qmisc.h"
#include "uimanager/moduimanager.h"
#include "settingui/include/modsettingui.h"
#include "setting/src/callforwardcontroller.h"
#include <configiddefine.h>
#include "setting/include/modsetting.h"
#include "configparser/modconfigparser.h"
#include "setting/include/authorityiddefine.h"
#include "account/include/modaccount.h"
#include "uicommon.h"
#include "actiondefinecommon.h"
#include "baseui//t4xpicpath.h"
#include "talk/fwd_dnd/include/modfwddnd.h"
#include "messagebox/modmessagebox.h"

CDlgCustomForwardOption::CDlgCustomForwardOption(QWidget * parent)
    : CDlgBaseFwdPageUI(FWD_TYPE_ALL, parent)
    , m_pLabAlwaysStatus(NULL)
    , m_pLabBusyStatus(NULL)
    , m_pLabNoAnswerStatus(NULL)
{
    InitPageData();
}


void CDlgCustomForwardOption::InitPageData()
{
    m_pLabAlwaysStatus = new QFwdWidget(this);
    m_pLabBusyStatus = new QFwdWidget(this);
    m_pLabNoAnswerStatus = new QFwdWidget(this);

    if (m_pLabAlwaysStatus != NULL
            && m_pLabBusyStatus != NULL
            && m_pLabNoAnswerStatus != NULL)
    {
        SetWgtFontColor(m_pLabAlwaysStatus, FONT_TYPE_TEXT);
        SetWgtFontColor(m_pLabBusyStatus, FONT_TYPE_TEXT);
        SetWgtFontColor(m_pLabNoAnswerStatus, FONT_TYPE_TEXT);

        QWidgetPair qPair;
        qPair.eType = SETTING_ITEM_MENU;
        qPair.bWithStatus = true;
        QString strLab;

        strLab = LANG_TRANSLATE(TRID_ALWAYS_FORWARD);
        qPair.first = strLab;
        qPair.second = m_pLabAlwaysStatus;
        qPair.third = "enter";
        qPair.strAuthorId = kszAllwaysForward;
        qPair.iFlag = FWD_TYPE_ALWAYS;
        m_vecWidgetPair.push_back(qPair);

        strLab = LANG_TRANSLATE(TRID_BUSY_FORWARD);
        qPair.first = strLab;
        qPair.second = m_pLabBusyStatus;
        qPair.third = "enter";
        qPair.strAuthorId = kszBusyForward;
        qPair.iFlag = FWD_TYPE_BUSY;
        m_vecWidgetPair.push_back(qPair);

        strLab = LANG_TRANSLATE(TRID_NO_ANSWER_FORWARD);
        qPair.first = strLab;
        qPair.second = m_pLabNoAnswerStatus;
        qPair.third = "enter";
        qPair.strAuthorId = kszNoAnswerForward;
        qPair.iFlag = FWD_TYPE_NO_ANSWER;
        m_vecWidgetPair.push_back(qPair);

        AddWidget(m_vecWidgetPair, true, false, true, false);
    }
}


CDlgCustomForwardOption::~CDlgCustomForwardOption()
{
}

//加载数据
bool CDlgCustomForwardOption::LoadPageData()
{
    InitFwdData(m_pLabAlwaysStatus, FWD_TYPE_ALWAYS);
    InitFwdData(m_pLabBusyStatus, FWD_TYPE_BUSY);
    InitFwdData(m_pLabNoAnswerStatus, FWD_TYPE_NO_ANSWER);

    return true;
}

//获取数据状态是否改变
bool CDlgCustomForwardOption::IsStatusChanged()
{
#ifdef IF_FEATURE_FORWARD_OPT
    if (CDlgBaseFwdPageUI::IsStatusChanged())
    {
        return true;
    }

    FWD_TYPE fwd_arr[] = {FWD_TYPE_ALWAYS, FWD_TYPE_BUSY, FWD_TYPE_NO_ANSWER};
    for (int i = 0; i < sizeof(fwd_arr) / sizeof(fwd_arr[0]); ++i)
    {
        if (!GetPageDataByType(fwd_arr[i])
                || m_fwdLogicData != m_fwdPageData)
        {
            return true;
        }
    }
#endif

    return false;
}

//保存数据
bool CDlgCustomForwardOption::SavePageData()
{
#ifdef IF_FEATURE_FORWARD_OPT
    bool bChanged = false;
    bool bSendMsg = true;

    FWD_TYPE fwd_arr[] = {FWD_TYPE_ALWAYS, FWD_TYPE_BUSY, FWD_TYPE_NO_ANSWER};
    for (int i = 0; i < sizeof(fwd_arr) / sizeof(fwd_arr[0]); ++i)
    {
        FWD_TYPE eType = fwd_arr[i];

        if (GetPageDataByType(eType)
                && (m_fwdLogicData != m_fwdPageData || CDlgBaseFwdPageUI::IsStatusChanged()))
        {
            bChanged |= g_pCallForwardController->SetForwardDataByType(eType, m_fwdPageData, GetLineID(),
                        bSendMsg);

            if (bChanged)
            {
                bSendMsg = false;
            }
        }
    }

    if (!bChanged)
    {
        MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_NOTHING_CHANGED),
                                    MESSAGEBOX_NOTE, 1500);

        return false;
    }

    return true;
#else
    return false;
#endif
}

// 设置子页面前的操作
void CDlgCustomForwardOption::BeforeSetSubPage(void * pData)
{
    int iLineId = -1;
    if (NULL != pData)
    {
        iLineId = *((int *)pData);
        m_strTitle = LANG_TRANSLATE(TRID_FORWARD_OPTION);
    }
    else
    {
#ifdef IF_FEATURE_FORWARD_OPT
        if (FWDDND_MODE_GLOBAL == fwd_GetMode())
        {
            iLineId = -1;
            m_strTitle = LANG_TRANSLATE(TRID_CALL_FORWARD);
        }
        else
        {
            YLList<int> listAccount = acc_GetAvailableAccountList();
            if (listAccount.size() == 1)
            {
                iLineId = *(listAccount.begin());
                m_strTitle = LANG_TRANSLATE(TRID_FORWARD_OPTION);
            }
        }
#else
        m_strTitle = LANG_TRANSLATE(TRID_CALL_FORWARD);
#endif
    }

    SetLineID(iLineId);
    SETTINGUI_INFO("CDlgCustomForwardOption::BeforeSetSubPage data=[%p] account=[%d]", pData, iLineId);
}

// 设置子页面的SoftKey
bool CDlgCustomForwardOption::GetSoftkeyData(CArraySoftKeyBarData & objSoftkeyData)
{
#ifdef IF_FEATURE_FORWARD_OPT
    objSoftkeyData[3].m_strSoftkeyAction = ACTION_SAVE_DATA;
    objSoftkeyData[3].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_SAVE);
#else
    objSoftkeyData[3].m_strSoftkeyAction = "";
    objSoftkeyData[3].m_strSoftkeyTitle = "";
#endif

    return true;
}

bool CDlgCustomForwardOption::OnAction(const QString & strAction)
{
    if (strAction == "enter")
    {
        SubMenuData objData;
        int iLineID = GetLineID();
        objData.pBeforeSetSubpageData = (void *)&iLineID;

        // 使用序号判断不可靠，因为如果某项被隐藏了，会导致进入错误的页面
        int iFlag = GetFocusItemFlag();
        if (iFlag == FWD_TYPE_ALWAYS)
        {
            objData.strMenuAction = "OpenSubPage(call_forward_always_forward)";
        }
        else if (iFlag == FWD_TYPE_BUSY)
        {
            objData.strMenuAction = "OpenSubPage(call_forward_busy_forward)";
        }
        else if (iFlag == FWD_TYPE_NO_ANSWER)
        {
            objData.strMenuAction = "OpenSubPage(call_forward_no_answer_forward)";
        }

        emit openpage(objData);
        return true;
    }

    return CDlgBaseFwdPageUI::OnAction(strAction);
}

void CDlgCustomForwardOption::showEvent(QShowEvent * e)
{
    //从下面的子界面返回，需重新下载数据
    LoadPageData();
}

bool CDlgCustomForwardOption::InitFwdData(QFwdWidget * pWidget, FWD_TYPE eType)
{
    if (GetLogicDataByType(eType))
    {
#ifdef IF_FEATURE_FORWARD_OPT
        pWidget->SetData(m_fwdLogicData.m_isOn);
        pWidget->SetLabel(m_fwdLogicData.m_strTarget);
#else
        pWidget->setText(m_fwdLogicData.m_isOn ? LANG_TRANSLATE(TRID_ON) : LANG_TRANSLATE(TRID_OFF));
#endif

        return true;
    }

    return false;
}

bool CDlgCustomForwardOption::GetPageDataByType(FWD_TYPE eType)
{
#ifdef IF_FEATURE_FORWARD_OPT
    m_fwdPageData.Reset();
    QFwdWidget * pWidget = NULL;

    switch (eType)
    {
    case FWD_TYPE_ALWAYS:
        pWidget = m_pLabAlwaysStatus;
        break;
    case FWD_TYPE_BUSY:
        pWidget = m_pLabBusyStatus;
        break;
    case FWD_TYPE_NO_ANSWER:
        pWidget = m_pLabNoAnswerStatus;
        break;
    default:
        break;
    }

    if (pWidget != NULL && GetLogicDataByType(eType))
    {
        m_fwdPageData = m_fwdLogicData;
        m_fwdPageData.m_isOn = pWidget->GetData();

        return true;
    }
#endif

    return false;
}

bool CDlgCustomForwardOption::GetLogicDataByType(FWD_TYPE eType)
{
    m_fwdLogicData.Reset();
    return g_pCallForwardController->GetForwardDataByType(&m_fwdLogicData, eType, GetLineID());
}

bool CDlgCustomForwardOption::IsInputDataValid(InputDataErrorInfo & objErrorContent)
{
#ifdef IF_FEATURE_FORWARD_OPT
    FWD_TYPE fwd_arr[] = {FWD_TYPE_ALWAYS, FWD_TYPE_BUSY, FWD_TYPE_NO_ANSWER};
    for (int i = 0; i < sizeof(fwd_arr) / sizeof(fwd_arr[0]); ++i)
    {
        FWD_TYPE eType = fwd_arr[i];

        if (GetPageDataByType(eType))
        {
            QFwdWidget * pWidget = NULL;
            if (m_fwdPageData.m_isOn && m_fwdPageData.m_strTarget.empty())
            {
                switch (eType)
                {
                case FWD_TYPE_ALWAYS:
                    pWidget = m_pLabAlwaysStatus;
                    break;
                case FWD_TYPE_BUSY:
                    pWidget = m_pLabBusyStatus;
                    break;
                case FWD_TYPE_NO_ANSWER:
                    pWidget = m_pLabNoAnswerStatus;
                    break;
                default:
                    break;
                }

                if (pWidget != NULL)
                {
                    objErrorContent.strErrorContent = TRID_TARGET_CANNOT_BE_EMPTY;
                    objErrorContent.pLineEdit = NULL;
                    //objErrorContent.pLineEdit = pWidget;
                    return false;
                }
            }
        }
    }

    return true;
#else
    return true;
#endif
}

bool CDlgCustomForwardOption::GetPageData(ForwardData * pData, FWD_TYPE eFwdType)
{
    if (pData != NULL && GetPageDataByType(eFwdType))
    {
        *pData = m_fwdPageData;
        return true;
    }

    return false;
}

