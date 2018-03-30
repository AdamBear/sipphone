#include "model.h"
#include "cdlgcallcontrol.h"
#include "canywhereuilogic.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "actiondefinecommon.h"
#include "uikernel/languagehelper.h"
#include "qtcommon/qmisc.h"
#include "setting/include/common.h"
#include "setting/src/callforwardcontroller.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "settingui/include/modsettingui.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "talk/fwd_dnd/include/modfwddnd.h"
#include "setting/include/modmenu.h"
#include "account/include/modaccount.h"
#include "broadsoft/executiveassistant/include/modexecutiveassistant.h"
#include "account/include/modaccount.h"
#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
#include "broadsoft/callrecording/include/modcallrecording.h"
#endif


const yl::string strAnyWherePage             = "OpenSubPage(anywhere)";
const yl::string strRemoteOfficePage         = "OpenSubPage(remote_office)";
const yl::string strFWDNotReachablePage         = "OpenSubPage(Call_Forwarding_Not_Reachable)";
const yl::string strGroupNightForward               = "OpenSubPage(GroupNightForward)";
const yl::string strMobility                        = "OpenSubPage(Mobility)";
const yl::string strDND                      = "OpenSubPage(dnd)";
const yl::string strHideNumbersPage          = "OpenSubPage(hide_numbers)";
const yl::string strAnonymousRejectionPage   = "OpenSubPage(anonymous_rejection)";
const yl::string strSimultaneousRingPage     = "OpenSubPage(simultaneous_ring)";
const yl::string strACDInitialStatePage   = "OpenSubPage(initial_state)";
const yl::string strACDSelectStatePage        = "OpenSubPage(acd_status_select)";
const yl::string strCallControlLineSelect     = "OpenSubPage(callcontrol_lineselect)";
const yl::string strExecutiveDetailsPage      = "OpenSubPage(executive_details)";

const yl::string strCallRecordPage         = "OpenSubPage(call_record)";

const yl::string strBsftCallWaitingPage         = "OpenSubPage(bsft_callwaiting)";
const yl::string strForwardSelectivePage    = "OpenSubPage(forward_selective)";
const yl::string strSecurityClassification = "OpenSubPage(security_classification)";
const yl::string strSilentAlertingPage = "OpenSubPage(silent_alerting)";

#define CALLCONTROL_ACTION_ANYWHERE             "action_anywhere"
#define CALLCONTROL_ACTION_AW_LINESELECT        "action_anywhere_lineselect"
#define CALLCONTROL_ACTION_REMOTEOFFICE         "action_remoteoffice"
#define CALLCONTROL_ACTION_R0_LINESELECT        "action_remoteoffice_lineselect"
#define CALLCONTROL_ACTION_FWDNOTREACH          "action_CallForwardingNotReachable"
#define CALLCONTROL_ACTION_FWDNOTREACH_LINESELECT   "action_CallFWDNotReachableLineSelect"
#define CALLCONTROL_ACTION_GROUPNIGHTFORWARD            "action_GroupNightForward"
#define CALLCONTROL_ACTION_GROUPNIGHTFORWARD_LINESELECT "action_GroupNightForwardLineSelect"
#define CALLCONTROL_ACTION_MOBILITY                     "action_Mobility"
#define CALLCONTROL_ACTION_MOBILITY_LINESELECT          "action_MobilityLineSelect"
#define CALLCONTROL_ACTION_FWD                  "action_callforward"
#define CALLCONTROL_ACTION_DND                  "action_dnd"
#define CALLCONTROL_ACTION_HIDENUMBER           "action_hidenumber"
#define CALLCONTROL_ACTION_HN_LINESELECT        "action_hidenumber_lineselect"
#define CALLCONTROL_ACTION_REJECTION            "action_anonymousrejection"
#define CALLCONTROL_ACTION_AR_LINESELECT        "action_anonymousrejection_lineselect"
#define CALLCONTROL_ACTION_SIMULTANEOUS         "action_simultaneous"
#define CALLCONTROL_ACTION_SRP_LINESELECT       "action_simultaneous_lineselect"
#define CALLCONTROL_ACTION_ACDINIT              "action_acd_initial_state"
#define CALLCONTROL_ACTION_ACDSELSTATE          "action_acd_select_state"
#define CALLCONTROL_ACTION_EXECUTIVE            "action_executive"
#define CALLCONTROL_ACTION_EXECUTIVE_LINESELECT "action_executive_lineselect"
#define CALLCONTROL_ACTION_ASSISTANT            "action_assistant"
#define CALLCONTROL_ACTION_ASSISTANT_LINESELECT "action_assistant_lineselect"

// #define CALLCONTROL_ACTION_CALLRECORD       "action_callrecord"
// #define CALLCONTROL_ACTION_CALLRECORD_LINESELECT        "action_callrecord_lineselect"

#define CALLCONTROL_ACTION_CALLWAITING                      "action_callwaiting"
#define CALLCONTROL_ACTION_CALLWAITING_LINESELECT           "action_callwaiting_lineselect"

#ifdef IF_FEATURE_BROADSOFT_CALL_FORWARD_SELECTIVE
#define CALLCONTROL_ACTION_FS_LINESELECT        "action_forward_selective_lineselect"
#define CALLCONTROL_ACTION_FS                   "action_forward_selective"
#endif

#define CALLCONTROL_ACTION_SECURITY_CLASSIFICATION          "action_security_classification"
#define CALLCONTROL_ACTION_SILENT_ALERTING                   "action_Silent_Alerting"
#define CALLCONTROL_ACTION_SILENT_ALERTING_LINESELECT        "action_Silent_Alerting_lineselect"

CDlgCallControl::CDlgCallControl(QWidget * parent)
    : CDlgBaseSubPage(parent)
{
    InitData();
}

void CDlgCallControl::InitData()
{
    //设置Activation标签
    QWidgetPair qPair;
    QString strLab;

    int iAccRegisterNum = acc_GetRegisteredAccountNum();

    if (iAccRegisterNum == 0)
    {
        if (fwd_IsActive())
        {
            AddItem(LANG_TRANSLATE(TRID_CALL_FORWARD), NULL, CALLCONTROL_ACTION_FWD);
        }
        if (dnd_IsActive())
        {
            AddItem(LANG_TRANSLATE(TRID_DO_NOT_DISTURB), NULL, CALLCONTROL_ACTION_DND);
        }
    }
    else if (iAccRegisterNum > 1)
    {
        AddItem(LANG_TRANSLATE(TRID_ANYWHERE), NULL, CALLCONTROL_ACTION_AW_LINESELECT);
#ifdef IF_FEATURE_BROADSOFT_MOBILITY
        AddItem(LANG_TRANSLATE(TRID_MOBILITY), NULL, CALLCONTROL_ACTION_MOBILITY_LINESELECT);
#endif
        AddItem(LANG_TRANSLATE(TRID_REMOTE_OFFICE), NULL, CALLCONTROL_ACTION_R0_LINESELECT);

        if (fwd_IsActive())
        {
            AddItem(LANG_TRANSLATE(TRID_CALL_FORWARD), NULL, CALLCONTROL_ACTION_FWD);
        }
#ifdef IF_FEATURE_BROADSOFT_CALL_FORWARD_NOT_REACHABLE
        AddItem(LANG_TRANSLATE(TRID_FORWARD_NOT_REACHABLE), NULL,
                CALLCONTROL_ACTION_FWDNOTREACH_LINESELECT);
#endif // IF_FEATURE_BROADSOFT_CALL_FORWARD_NOT_REACHABLE
        if (dnd_IsActive())
        {
            AddItem(LANG_TRANSLATE(TRID_DO_NOT_DISTURB), NULL, CALLCONTROL_ACTION_DND);
        }
        AddItem(LANG_TRANSLATE(TRID_HIDE_NUMBER), NULL, CALLCONTROL_ACTION_HN_LINESELECT);
        AddItem(LANG_TRANSLATE(TRID_ANONYMOUS_REJECTION), NULL, CALLCONTROL_ACTION_AR_LINESELECT);
        AddItem(LANG_TRANSLATE(TRID_SIMULTANEOUS_RING_PERSONAL), NULL, CALLCONTROL_ACTION_SRP_LINESELECT);
#ifdef IF_FEATURE_BROADSOFT_GROUP_NIGHT_FORWARD
        AddItem(LANG_TRANSLATE(TRID_GROUP_NIGHT_FORWARD), NULL,
                CALLCONTROL_ACTION_GROUPNIGHTFORWARD_LINESELECT);
#endif

#ifdef IF_FEATURE_BROADSOFT_CALL_WAITING
        if (configParser_GetConfigInt(kszBroadsoftActive) == 1
                && configParser_GetConfigInt(kszCallWaitingMode) == 1)
        {
            AddItem(LANG_TRANSLATE(TRID_CALL_WAITING), NULL, CALLCONTROL_ACTION_CALLWAITING_LINESELECT);
        }
#endif
#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
//      if (configParser_GetConfigInt(kszCallRecordModeEnable) == 1)
//      {
//          AddItem(LANG_TRANSLATE(TRID_CALL_RECORD), NULL, CALLCONTROL_ACTION_CALLRECORD_LINESELECT);
//      }
#endif
#ifdef IF_FEATURE_BROADSOFT_CALL_FORWARD_SELECTIVE
        AddItem(LANG_TRANSLATE(TRID_FORWARD_SELECTIVE), NULL, CALLCONTROL_ACTION_FS_LINESELECT);
#endif
        if (configParser_GetConfigInt(kszBroadsoftActive) == 1
                && configParser_GetConfigInt(kszXSISwitch) == 1)
        {
            AddItem(LANG_TRANSLATE(TRID_SILENTALERT), NULL, CALLCONTROL_ACTION_SILENT_ALERTING_LINESELECT);
        }
    }
    else
    {
        AddItem(LANG_TRANSLATE(TRID_ANYWHERE), NULL, CALLCONTROL_ACTION_ANYWHERE);
#ifdef IF_FEATURE_BROADSOFT_MOBILITY
        AddItem(LANG_TRANSLATE(TRID_MOBILITY), NULL, CALLCONTROL_ACTION_MOBILITY);
#endif
        AddItem(LANG_TRANSLATE(TRID_REMOTE_OFFICE), NULL, CALLCONTROL_ACTION_REMOTEOFFICE);

        if (fwd_IsActive())
        {
            AddItem(LANG_TRANSLATE(TRID_CALL_FORWARD), NULL, CALLCONTROL_ACTION_FWD);
        }
#ifdef IF_FEATURE_BROADSOFT_CALL_FORWARD_NOT_REACHABLE
        AddItem(LANG_TRANSLATE(TRID_FORWARD_NOT_REACHABLE), NULL, CALLCONTROL_ACTION_FWDNOTREACH);
#endif  //  IF_FEATURE_BROADSOFT_CALL_FORWARD_NOT_REACHABLE
        if (dnd_IsActive())
        {
            AddItem(LANG_TRANSLATE(TRID_DO_NOT_DISTURB), NULL, CALLCONTROL_ACTION_DND);
        }
        AddItem(LANG_TRANSLATE(TRID_HIDE_NUMBER), NULL, CALLCONTROL_ACTION_HIDENUMBER);
        AddItem(LANG_TRANSLATE(TRID_ANONYMOUS_REJECTION), NULL, CALLCONTROL_ACTION_REJECTION);
        AddItem(LANG_TRANSLATE(TRID_SIMULTANEOUS_RING_PERSONAL), NULL, CALLCONTROL_ACTION_SIMULTANEOUS);
#ifdef IF_FEATURE_BROADSOFT_GROUP_NIGHT_FORWARD
        AddItem(LANG_TRANSLATE(TRID_GROUP_NIGHT_FORWARD), NULL, CALLCONTROL_ACTION_GROUPNIGHTFORWARD);
#endif
#ifdef IF_FEATURE_BROADSOFT_CALL_WAITING
        if (configParser_GetConfigInt(kszBroadsoftActive) == 1
                && configParser_GetConfigInt(kszCallWaitingMode) == 1)
        {
            AddItem(LANG_TRANSLATE(TRID_CALL_WAITING), NULL, CALLCONTROL_ACTION_CALLWAITING);
        }
#endif
#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
//      if (configParser_GetConfigInt(kszCallRecordModeEnable) == 1)
//      {
//          AddItem(LANG_TRANSLATE(TRID_CALL_RECORD), NULL, CALLCONTROL_ACTION_CALLRECORD);
//      }
#endif
#ifdef IF_FEATURE_BROADSOFT_CALL_FORWARD_SELECTIVE
        AddItem(LANG_TRANSLATE(TRID_FORWARD_SELECTIVE), NULL, CALLCONTROL_ACTION_FS);
#endif
        if (configParser_GetConfigInt(kszBroadsoftActive) == 1
                && configParser_GetConfigInt(kszXSISwitch) == 1)
        {
            AddItem(LANG_TRANSLATE(TRID_SILENTALERT), NULL, CALLCONTROL_ACTION_SILENT_ALERTING);
        }
    }

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    //ACD开启才添加ACD State和ACD Init State两个选项
    if (CallCenter_IsACDOn()
            && ACD_BROADSOFT == Get_ACDType())
    {
        AddItem(LANG_TRANSLATE(TRID_INITAIL_ACD_STATE), NULL, CALLCONTROL_ACTION_ACDINIT);
        AddItem(LANG_TRANSLATE(TRID_ACD_STATE), NULL, CALLCONTROL_ACTION_ACDSELSTATE);
    }
#endif //IF_FEATURE_BROADSOFT_CALL_CENTER



    AddExecutiveItems();

    AddSecurityClassificationItem();

    AddWidget(m_vecWidgetPair, true, false, true, false);
    //为界面自己安装事件过滤器
    this->installEventFilter(this);
}

yl::string CDlgCallControl::GetClickActionByItemAction(const QString & strItemAction)
{
    if (CALLCONTROL_ACTION_FWD == strItemAction)
    {
        //进入Call Forwarding界面
        if (FWDDND_MODE_ACCOUNT == fwd_GetMode())
        {
            return "OpenSubPage(call_forward_for_custom)";
        }
        else
        {
            return "OpenPage(call_forward_list.xml)";
        }
    }
    else if (CALLCONTROL_ACTION_DND == strItemAction)
    {
        //进入DND界面
        if (FWDDND_MODE_ACCOUNT == dnd_GetMode())
        {
            return "OpenSubPage(dnd_custom)";
        }
        else
        {
            return "OpenSubPage(dnd)";
        }
    }
    else if (CALLCONTROL_ACTION_ANYWHERE == strItemAction
             || CALLCONTROL_ACTION_AW_LINESELECT == strItemAction)
    {
        return strAnyWherePage;
    }
    else if (CALLCONTROL_ACTION_REMOTEOFFICE == strItemAction
             || CALLCONTROL_ACTION_R0_LINESELECT == strItemAction)
    {
        return strRemoteOfficePage;
    }
    else if (CALLCONTROL_ACTION_FWDNOTREACH == strItemAction
             || CALLCONTROL_ACTION_FWDNOTREACH_LINESELECT == strItemAction)
    {
        return strFWDNotReachablePage;
    }
    else if (CALLCONTROL_ACTION_HIDENUMBER == strItemAction
             || CALLCONTROL_ACTION_HN_LINESELECT == strItemAction)
    {
        return strHideNumbersPage;
    }
    else if (CALLCONTROL_ACTION_REJECTION == strItemAction
             || CALLCONTROL_ACTION_AR_LINESELECT == strItemAction)
    {
        return strAnonymousRejectionPage;
    }
    else if (CALLCONTROL_ACTION_SIMULTANEOUS == strItemAction
             || CALLCONTROL_ACTION_SRP_LINESELECT == strItemAction)
    {
        return strSimultaneousRingPage;
    }
    else if (CALLCONTROL_ACTION_ACDINIT == strItemAction)
    {
        return strACDInitialStatePage;
    }
    else if (CALLCONTROL_ACTION_ACDSELSTATE == strItemAction)
    {
        return strACDSelectStatePage;
    }
    else if (CALLCONTROL_ACTION_SECURITY_CLASSIFICATION == strItemAction)
    {
        return strSecurityClassification;
    }
    else if (CALLCONTROL_ACTION_SILENT_ALERTING == strItemAction
             || CALLCONTROL_ACTION_SILENT_ALERTING_LINESELECT == strItemAction)
    {
        return strSilentAlertingPage;
    }
#ifdef IF_FEATURE_BROADSOFT_GROUP_NIGHT_FORWARD
    else if (CALLCONTROL_ACTION_GROUPNIGHTFORWARD == strItemAction
             || CALLCONTROL_ACTION_GROUPNIGHTFORWARD_LINESELECT == strItemAction)
    {
        return strGroupNightForward;
    }
#endif
#ifdef IF_FEATURE_BROADSOFT_MOBILITY
    else if (CALLCONTROL_ACTION_MOBILITY == strItemAction
             || CALLCONTROL_ACTION_MOBILITY_LINESELECT == strItemAction)
    {
        return strMobility;
    }
#endif
#ifdef IF_FEATURE_BROADSOFT_CALL_WAITING
    else if (CALLCONTROL_ACTION_CALLWAITING == strItemAction
             || CALLCONTROL_ACTION_CALLWAITING_LINESELECT == strItemAction)
    {
        return strBsftCallWaitingPage;
    }
#endif
#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
//  else if (CALLCONTROL_ACTION_CALLRECORD == strItemAction
//      || CALLCONTROL_ACTION_CALLRECORD_LINESELECT == strItemAction)
//  {
//      return strCallRecordPage;
//  }
#endif
#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    else if (CALLCONTROL_ACTION_ASSISTANT == strItemAction
             || CALLCONTROL_ACTION_ASSISTANT_LINESELECT == strItemAction
             || CALLCONTROL_ACTION_EXECUTIVE == strItemAction
             || CALLCONTROL_ACTION_EXECUTIVE_LINESELECT == strItemAction)
    {
        return strExecutiveDetailsPage;
    }
#endif
#ifdef IF_FEATURE_BROADSOFT_CALL_FORWARD_SELECTIVE
    else if (CALLCONTROL_ACTION_FS == strItemAction
             || CALLCONTROL_ACTION_FS_LINESELECT == strItemAction)
    {
        return strForwardSelectivePage;
    }
#endif

    return "";
}

// 添加项
void  CDlgCallControl::AddItem(const QString & strDisplayText, QWidget * pWidget,
                               const QString & strAction/*, bool isMenu = true*/)
{
    QWidgetPair qPair;
    qPair.first = strDisplayText;
    qPair.second = pWidget;
    qPair.eType = SETTING_ITEM_MENU;
    qPair.bAddIndex = true;
    qPair.third = strAction;
    qPair.strAuthorId = GetClickActionByItemAction(strAction);
    qPair.strAuthorId = Menu_GetWebItemKeyByClickAction(qPair.strAuthorId);

    m_vecWidgetPair.push_back(qPair);
}

// 设置页面的SoftKey
bool CDlgCallControl::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    if (m_pGeneralFrame == NULL)
    {
        return false;
    }

    // Call Control 目前不下载，不存在保存键
    objWndData[3].m_strSoftkeyAction = "";
    objWndData[3].m_strSoftkeyTitle = "";


    return true;
}

//事件过滤器
bool CDlgCallControl::eventFilter(QObject * pObject, QEvent * pEvent)
{
    return CDlgBaseSubPage::eventFilter(pObject, pEvent);

}

bool CDlgCallControl::OnAction(const QString & strAction)
{
    if (m_pGeneralFrame == NULL)
    {
        return false;
    }

    int iLine = acc_GetDefaultAccount();

    if (CALLCONTROL_ACTION_FWD == strAction)
    {
        //进入Call Forwarding界面
        if (FWDDND_MODE_ACCOUNT == fwd_GetMode())
        {
            emit openpage(SubMenuData("OpenSubPage(call_forward_for_custom)"));
        }
        else
        {
            emit openpage(SubMenuData("OpenSubPage(call_forward_option)"));
        }
    }
    else if (CALLCONTROL_ACTION_DND == strAction)
    {
        //进入DND界面
        if (FWDDND_MODE_ACCOUNT == dnd_GetMode())
        {
            emit openpage(SubMenuData("OpenSubPage(dnd_custom)"));
        }
        else
        {
            emit openpage(SubMenuData("OpenSubPage(dnd)"));
        }
    }
    else if (CALLCONTROL_ACTION_ANYWHERE == strAction)
    {
        //进入Anywhere界面
        SubMenuData subData;
        subData.strMenuAction = strAnyWherePage;
        subData.pBeforeSetSubpageData = &iLine;
        emit openpage(subData);
    }
    else if (CALLCONTROL_ACTION_AW_LINESELECT == strAction)
    {
        //进入Anywhere帐号选择界面
        SubMenuData subData;
        subData.strMenuAction = strCallControlLineSelect;
        subData.pBeforeSetSubpageData = (void *)&strAnyWherePage;
        emit openpage(subData);
    }
    else if (CALLCONTROL_ACTION_REMOTEOFFICE == strAction)
    {
        //进入Remote Office界面
        SubMenuData subData;
        subData.strMenuAction = strRemoteOfficePage;
        subData.pBeforeSetSubpageData = &iLine;
        emit openpage(subData);
    }
    else if (CALLCONTROL_ACTION_R0_LINESELECT == strAction)
    {
        //进入Remote Office帐号选择界面
        SubMenuData subData;
        subData.strMenuAction = strCallControlLineSelect;
        subData.pBeforeSetSubpageData = (void *)&strRemoteOfficePage;
        emit openpage(subData);
    }
    else if (CALLCONTROL_ACTION_SECURITY_CLASSIFICATION == strAction)
    {
        //进入Security Classification界面
        emit openpage(SubMenuData(strSecurityClassification));
    }
    else if (CALLCONTROL_ACTION_HIDENUMBER == strAction)
    {
        //进入Hide Numbers界面
        SubMenuData subData;
        subData.strMenuAction = strHideNumbersPage;
        subData.pBeforeSetSubpageData = &iLine;
        emit openpage(subData);
    }
    else if (CALLCONTROL_ACTION_HN_LINESELECT == strAction)
    {
        //进入Hide Numbers 帐号选择界面
        SubMenuData subData;
        subData.strMenuAction = strCallControlLineSelect;
        subData.pBeforeSetSubpageData = (void *)&strHideNumbersPage;
        emit openpage(subData);
    }
    else if (CALLCONTROL_ACTION_REJECTION == strAction)
    {
        //进入Anonymous Rejections界面
        SubMenuData subData;
        subData.strMenuAction = strAnonymousRejectionPage;
        subData.pBeforeSetSubpageData = &iLine;
        emit openpage(subData);
    }
    else if (CALLCONTROL_ACTION_AR_LINESELECT == strAction)
    {
        //进入Anonymous Rejection帐号选择界面
        SubMenuData subData;
        subData.strMenuAction = strCallControlLineSelect;
        subData.pBeforeSetSubpageData = (void *)&strAnonymousRejectionPage;
        emit openpage(subData);
    }
    else if (CALLCONTROL_ACTION_SIMULTANEOUS == strAction)
    {
        //进入Simultaneous Ring Personal界面
        SubMenuData subData;
        subData.strMenuAction = strSimultaneousRingPage;
        subData.pBeforeSetSubpageData = &iLine;
        emit openpage(subData);
    }
    else if (CALLCONTROL_ACTION_SRP_LINESELECT == strAction)
    {
        //进入Simultaneous Ring Personal帐号选择界面
        SubMenuData subData;
        subData.strMenuAction = strCallControlLineSelect;
        subData.pBeforeSetSubpageData = (void *)&strSimultaneousRingPage;
        emit openpage(subData);
    }
    else if (CALLCONTROL_ACTION_SILENT_ALERTING_LINESELECT == strAction)
    {
        //进入Silent Alerting账号选择界面
        SubMenuData subData;
        subData.strMenuAction = strCallControlLineSelect;
        subData.pBeforeSetSubpageData = (void *)&strSilentAlertingPage;

        emit openpage(subData);
    }
    else if (CALLCONTROL_ACTION_ACDINIT == strAction)
    {
        //进入ACD初始状态设置界面
        emit openpage(SubMenuData(strACDInitialStatePage));
    }
    else if (CALLCONTROL_ACTION_ACDSELSTATE == strAction)
    {
        //进入ACD状态改变界面
        emit openpage(SubMenuData(strACDSelectStatePage));
    }
    else if (CALLCONTROL_ACTION_SILENT_ALERTING == strAction)
    {
        emit openpage(SubMenuData(strSilentAlertingPage));
    }
#ifdef IF_FEATURE_BROADSOFT_CALL_WAITING
    else if (CALLCONTROL_ACTION_CALLWAITING == strAction)
    {
        SubMenuData subData;
        subData.strMenuAction = strBsftCallWaitingPage;
        subData.pBeforeSetSubpageData = &iLine;
        emit openpage(subData);
    }
    else if (CALLCONTROL_ACTION_CALLWAITING_LINESELECT == strAction)
    {
        SubMenuData subData;
        subData.strMenuAction = strCallControlLineSelect;
        subData.pBeforeSetSubpageData = (void *)&strBsftCallWaitingPage;
        emit openpage(subData);
    }
#endif
#ifdef IF_FEATURE_BROADSOFT_GROUP_NIGHT_FORWARD
    else if (CALLCONTROL_ACTION_GROUPNIGHTFORWARD == strAction)
    {
        SubMenuData subData;
        subData.strMenuAction = strGroupNightForward;
        subData.pBeforeSetSubpageData = &iLine;
        emit openpage(subData);
    }
    else if (CALLCONTROL_ACTION_GROUPNIGHTFORWARD_LINESELECT == strAction)
    {
        SubMenuData subData;
        subData.strMenuAction = strCallControlLineSelect;
        subData.pBeforeSetSubpageData = (void *)&strGroupNightForward;
        emit openpage(subData);
    }
#endif
#ifdef IF_FEATURE_BROADSOFT_MOBILITY
    else if (CALLCONTROL_ACTION_MOBILITY == strAction)
    {
        SubMenuData subData;
        subData.strMenuAction = strMobility;
        subData.pBeforeSetSubpageData = &iLine;
        emit openpage(subData);
    }
    else if (CALLCONTROL_ACTION_MOBILITY_LINESELECT == strAction)
    {
        SubMenuData subData;
        subData.strMenuAction = strCallControlLineSelect;
        subData.pBeforeSetSubpageData = (void *)&strMobility;
        emit openpage(subData);
    }
#endif
#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
//  else if (CALLCONTROL_ACTION_CALLRECORD_LINESELECT == strAction)
//  {
//      SubMenuData subData;
//      subData.strMenuAction = strCallControlLineSelect;
//      subData.pBeforeSetSubpageData = (void *)&strCallRecordPage;
//
//      emit openpage(subData);
//  }
//  else if (CALLCONTROL_ACTION_CALLRECORD == strAction)
//  {
//      emit openpage(SubMenuData(strCallRecordPage));
//  }
#endif
#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    else if (CALLCONTROL_ACTION_EXECUTIVE == strAction)
    {
        SubMenuData subData(strExecutiveDetailsPage);
        EEA_TYPE eType = EET_Executive;
        int iLine = BWExeAssis_GetFirstUsableAccountByType(eType);
        subData.pBeforeSetSubpageData = (void *)&iLine;
        subData.pAfterSetSubpageData = (void *)&eType;

        emit openpage(subData);
    }
    else if (CALLCONTROL_ACTION_EXECUTIVE_LINESELECT == strAction)
    {
        SubMenuData subData;
        subData.strMenuAction = strCallControlLineSelect;
        subData.pBeforeSetSubpageData = (void *)&strExecutiveDetailsPage;
        EEA_TYPE eType = EET_Executive;
        subData.pAfterSetSubpageData = (void *)&eType;

        emit openpage(subData);
    }
    else if (CALLCONTROL_ACTION_ASSISTANT == strAction)
    {
        SubMenuData subData(strExecutiveDetailsPage);
        EEA_TYPE eType = EET_Assistant;
        int iLine = BWExeAssis_GetFirstUsableAccountByType(eType);
        subData.pBeforeSetSubpageData = (void *)&iLine;
        subData.pAfterSetSubpageData = (void *)&eType;

        emit openpage(subData);
    }
    else if (CALLCONTROL_ACTION_ASSISTANT_LINESELECT == strAction)
    {
        SubMenuData subData;
        subData.strMenuAction = strCallControlLineSelect;
        subData.pBeforeSetSubpageData = (void *)&strExecutiveDetailsPage;
        EEA_TYPE eType = EET_Assistant;
        subData.pAfterSetSubpageData = (void *)&eType;

        emit openpage(subData);
    }
#endif
#ifdef IF_FEATURE_BROADSOFT_CALL_FORWARD_SELECTIVE
    else if (CALLCONTROL_ACTION_FS == strAction)
    {
        emit openpage(SubMenuData(strForwardSelectivePage));
    }
    else if (CALLCONTROL_ACTION_FS_LINESELECT == strAction)
    {
        SubMenuData subData;
        subData.strMenuAction = strCallControlLineSelect;
        subData.pBeforeSetSubpageData = (void *)&strForwardSelectivePage;

        emit openpage(subData);
    }
#endif
#ifdef IF_FEATURE_BROADSOFT_CALL_FORWARD_NOT_REACHABLE
    else if (CALLCONTROL_ACTION_FWDNOTREACH == strAction)
    {
        SubMenuData subData;
        subData.strMenuAction = strFWDNotReachablePage;
        subData.pBeforeSetSubpageData = &iLine;
        emit openpage(subData);
    }
    else if (CALLCONTROL_ACTION_FWDNOTREACH_LINESELECT == strAction)
    {
        SubMenuData subData;
        subData.strMenuAction = strCallControlLineSelect;
        subData.pBeforeSetSubpageData = (void *)&strFWDNotReachablePage;
        emit openpage(subData);
    }
#endif
    else
    {
        return false;
    }
    return true;
}

bool CDlgCallControl::IsStatusChanged()
{
    return false;
}

bool CDlgCallControl::LoadPageData()
{
    return false;
}

bool CDlgCallControl::SavePageData()
{
    return false;
}

void CDlgCallControl::Uninit()
{
}

CDlgCallControl::~CDlgCallControl()
{

}

void CDlgCallControl::AddExecutiveItems()
{
#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    EEA_TYPE eType = EET_Executive;

    int iAvailAccount = BWExeAssis_GetAccountCountByType(eType);
    if (iAvailAccount == 1)
    {
        AddItem(LANG_TRANSLATE(TRID_EXECUTIVE), NULL, CALLCONTROL_ACTION_EXECUTIVE);
    }
    else if (iAvailAccount > 1)
    {
        AddItem(LANG_TRANSLATE(TRID_EXECUTIVE), NULL, CALLCONTROL_ACTION_EXECUTIVE_LINESELECT);
    }

    eType = EET_Assistant;
    iAvailAccount = BWExeAssis_GetAccountCountByType(eType);
    if (iAvailAccount == 1)
    {
        AddItem(LANG_TRANSLATE(TRID_EXECUTIVE_ASSISTANT), NULL, CALLCONTROL_ACTION_ASSISTANT);
    }
    else if (iAvailAccount > 1)
    {
        AddItem(LANG_TRANSLATE(TRID_EXECUTIVE_ASSISTANT), NULL, CALLCONTROL_ACTION_ASSISTANT_LINESELECT);
    }
#endif
}

void CDlgCallControl::AddSecurityClassificationItem()
{
    bool bEnableShow = false;

    //只要一个账号注册成功并且开启安全等级的功能就显示该菜单
    for (int i = 0; i < acc_AccountNum(); ++i)
    {
        if (!acc_IsAccountAvailable(i))
        {
            continue;
        }

        if (configParser_GetCfgItemIntValue(kszAccountEnableSecurityClassification, i) != 1)
        {
            continue;
        }

        bEnableShow = true;
        break;
    }

    if (bEnableShow)
    {
        AddItem(LANG_TRANSLATE(TRID_SECURITY_CLASSIFICATION), NULL,
                CALLCONTROL_ACTION_SECURITY_CLASSIFICATION);
    }
}
