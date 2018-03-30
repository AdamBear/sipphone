#include "cdlgexeassisdetails.h"
#include "uikernel/languagehelper.h"
#include <configiddefine.h>
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/xsiactionhelper/include/xsicommon.h"
#include "settinguicommon.h"
#include "setting/include/common.h"
#include "inputmethod/inputmethod.h"
#include "baseui/fontcolor.h"
#include "qtcommon/qmisc.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "setting/include/modmenu.h"

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT

static const char * strOpenCommonPage = "OpenSubPage(exe_common_setting)";
static const char * strOpenFilterPage = "OpenSubPage(exe_filter_setting)";
static const char * strOpenScreenPage = "OpenSubPage(exe_screen_setting)";
static const char * strOpenAlterPage = "OpenSubPage(exe_alter_setting)";
static const char * strOpenListviewPage = "OpenSubPage(exe_listview_setting)";

#define DETAILS_ACTION_COMMON               "action_common"
#define DETAILS_ACTION_FILTER               "action_filter"
#define DETAILS_ACTION_SCREEN               "action_screen"
#define DETAILS_ACTION_ALTER                "action_alter"
#define DETAILS_ACTION_LISTVIEW             "action_listview"

CDlgExeAssisDetails::CDlgExeAssisDetails(QWidget * parent /*= 0*/)
    : CDlgExeAssisUIBase(parent)
{
    m_eUIType = UI_Details;

    InitData();
}

bool CDlgExeAssisDetails::OnAction(const QString & strAction)
{
    QString strActionEx = strAction;

    if (strAction == ACTION_ENTER)
    {
        strActionEx = GetCurrentItemAction();
    }

    yl::string strActionPage = GetClickActionByItemAction(strActionEx);

    SETTINGUI_INFO("CDlgExeAssisDetails::OnAction action=[%s] focus=[%s] page=[%s]",
                   strAction.toLatin1().data(), strActionEx.toLatin1().data(), strActionPage.c_str());

    if (!strActionPage.empty())
    {
        emit openpage(SubMenuData(strActionPage));
        return true;
    }

    return CDlgExeAssisUIBase::OnAction(strAction);
}

void CDlgExeAssisDetails::AddItem(const QString & strDisplayText, QWidget * pWidget,
                                  const QString & strAction)
{
    QWidgetPair qPair;
    qPair.first = strDisplayText;
    qPair.second = pWidget;
    qPair.third = strAction;
    qPair.eType = SETTING_ITEM_MENU;
    qPair.bAddIndex = true;
    qPair.strAuthorId = GetClickActionByItemAction(strAction);
    qPair.strAuthorId = Menu_GetWebItemKeyByClickAction(qPair.strAuthorId);

    m_vecWidgetPair.push_back(qPair);
}

yl::string CDlgExeAssisDetails::GetClickActionByItemAction(const QString & strItemAction)
{
    if (DETAILS_ACTION_COMMON == strItemAction)
    {
        return strOpenCommonPage;
    }
    else if (DETAILS_ACTION_FILTER == strItemAction)
    {
        return strOpenFilterPage;
    }
    else if (DETAILS_ACTION_SCREEN == strItemAction)
    {
        return strOpenScreenPage;
    }
    else if (DETAILS_ACTION_ALTER == strItemAction)
    {
        return strOpenAlterPage;
    }
    else if (DETAILS_ACTION_LISTVIEW == strItemAction)
    {
        return strOpenListviewPage;
    }

    return yl::string();
}

bool CDlgExeAssisDetails::LoadPageData()
{
    m_strTitle = LANG_TRANSLATE(IsAssistant() ? TRID_ASSISTANT : TRID_EXECUTIVE);

    AddItem(LANG_TRANSLATE(TRID_COMMON_SETTINGS), 0, DETAILS_ACTION_COMMON);
    if (IsExecutive() || BWExeAssis_IsNeedShowAssistantDetailsSetting())
    {
        AddItem(LANG_TRANSLATE(TRID_EXECUTIVE_CALL_FILTER), 0, DETAILS_ACTION_FILTER);
        AddItem(LANG_TRANSLATE(TRID_EXECUTIVE_CALL_SCREEN), 0, DETAILS_ACTION_SCREEN);
        AddItem(LANG_TRANSLATE(TRID_EXECUTIVE_CALL_ALTER), 0, DETAILS_ACTION_ALTER);
    }
    AddItem(LANG_TRANSLATE(IsAssistant() ? TRID_ASSISTANT_VIEW_LIST : TRID_EXECUTIVE_VIEW_LIST), 0,
            DETAILS_ACTION_LISTVIEW);

    this->installEventFilter(this);

    AddWidget(m_vecWidgetPair, true, false);
    return true;
}

bool CDlgExeAssisDetails::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    objWndData[3].m_strSoftkeyAction = "";
    objWndData[3].m_strSoftkeyTitle = "";

    return true;
}

#endif


