#include "bsftpwdmgr.h"
#include "directorycommon.h"
#include "translateiddefine.h"
#include "diruihelper.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"


IMPLEMENT_GETINSTANCE(CBSFTPwdMgr)

CBSFTPwdMgr::CBSFTPwdMgr()
    : m_nDirType(DIRECTORY_TYPE_NONE)
    , m_nGroupId(kszAllContactId)
{
}

CBSFTPwdMgr::~CBSFTPwdMgr()
{
}

void CBSFTPwdMgr::SetDirType(int nDirType)
{
    m_nDirType = nDirType;
}

void CBSFTPwdMgr::SetPassword(const yl::string & strPwd)
{
    m_strPwd = strPwd;
}

int CBSFTPwdMgr::GetDirType()
{
    return m_nDirType;
}

int CBSFTPwdMgr::GetGroupId()
{
    return m_nGroupId;
}

int CBSFTPwdMgr::CheckBSFTPassword(CDirUIHelper * pUIHelper, int nGroupId)
{
    int nAccountID = 0;
#if IF_FEATURE_MUTI_CALLLOG
    if (BsftLog_IsMutilLogEnable()
            && m_nDirType == CALL_LOG_TYPE_BROADSOFT)
    {
        //如果当前是账号组直接进入
        if (BsftLog_IsAccountGroup(nGroupId))
        {
            return true;
        }
        nAccountID = BsftLog_GetAccountIDByGroupID(nGroupId);
    }
#endif

    int nXSIStatus = CBaseHelper::GetXSIStatus(nAccountID);

    if (XSI_SUCCESS == nXSIStatus)
    {
        m_nGroupId = kszAllContactId;
        return XSI_SUCCESS;
    }

    if (XSI_SIPACCOUNT_NOT_AVAILABLE == nXSIStatus)
    {
        m_nGroupId = kszAllContactId;
        CDirUIHelper::ShowMsgBox(TRID_SETTUI_TIP_SIP_XSI_AUTHENTICATION_FAILED);
    }
    else
    {
        m_nGroupId = nGroupId;

        yl::string strTip = (IsBsftAllowSipAuth() ? TRID_SIP_CREDENTIAL_PASSWORD_IS_WRONG :
                             TRID_WEB_ACCESS_PASSWORD_IS_WRONG);

        CDirUIHelper::ShowMsgBox(strTip.c_str(), NULL, DIR_ACTION_BSFT_ENTER_CHG_PWD);
    }

    return nXSIStatus;
}

void CBSFTPwdMgr::EnterBSFTPwdUI()
{
    CDirUIHelper::EnterUI(m_nDirType, DIR_UI_STATE_CHG_PWD, DIR_ACTION_BSFT_CHG_PWD, m_nGroupId);
}

bool CBSFTPwdMgr::RetryBSFTPwd(CDirUIHelper * pUIHelper)
{
    if (NULL == pUIHelper)
    {
        return false;
    }

    if (m_strPwd.empty())
    {
        CDirUIHelper::ShowMsgBox(TRID_PASSWORD_CANNOT_BE_EMPTY);
        return false;
    }

    DIRECTORY_RET_CODE retCode = DIR_RET_FAIL;

    if (DIRECTORY_TYPE_BROADSOFT == m_nDirType)
    {
        retCode = BsftDir_OnSubmitXSIPassword(m_strPwd, m_nGroupId);
    }
    else if (CALL_LOG_TYPE_BROADSOFT == m_nDirType)
    {
        retCode = BsftLog_OnSubmitXSIPassword(m_strPwd, m_nGroupId);
    }

    m_nGroupId = kszAllContactId;
    m_nDirType = DIRECTORY_TYPE_NONE;

    CDirUIHelper::ExitCurrentUI(DIR_UI_STATE_CHG_PWD);

    if (DIR_RET_SUCC != retCode)
    {
        CDirUIHelper::ShowErrorMsg(retCode);
        return false;
    }

    return true;
}
