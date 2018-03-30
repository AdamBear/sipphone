#include "cdlgexeassisuibase.h"
#include "uikernel/languagehelper.h"
#include "qtcommon/qmisc.h"
#include "keymap.h"
#include "inputmethod/inputmethod.h"
#include "account/include/modaccount.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/xsiactionhelper/include/xsicommon.h"
#include "cdlgexeassisuibase.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "uicommon.h"

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT

CDlgExeAssisUIBase::CDlgExeAssisUIBase(QWidget * parent /*= 0*/)
    : CDlgBaseSubPage(parent), m_bIsInLoad(false)
{
}

CDlgExeAssisUIBase::~CDlgExeAssisUIBase()
{
    g_refExeAssisUILogic.SetUIType(UI_NONE);
}

void CDlgExeAssisUIBase::OnShow()
{
    g_refExeAssisUILogic.SetUIType(m_eUIType);

    g_refExeAssisUILogic.OnUIShow(this);
}

void CDlgExeAssisUIBase::showEvent(QShowEvent * e)
{
    OnShow();
}

bool CDlgExeAssisUIBase::SavePageData()
{
    g_refExeAssisUILogic.SaveToLogic(this);
    // 屏蔽SettingWindow的自动保存机制，返回false
    return false;
}

bool CDlgExeAssisUIBase::GetPageDisableSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    objWndData[3].m_strSoftkeyAction = "";
    objWndData[3].m_strSoftkeyTitle = "";

    return true;
}

void CDlgExeAssisUIBase::BeforeSetSubPage(void * pData)
{
    if (pData != NULL)
    {
        int iLineId = *(int *)pData;
        g_refExeAssisUILogic.SetLineId(iLineId);
    }
}

void CDlgExeAssisUIBase::AfterSetSubPage(void * pData)
{
    if (pData != NULL)
    {
        g_refExeAssisUILogic.SetUserType(*(EEA_TYPE *)pData);
    }
}
#endif





