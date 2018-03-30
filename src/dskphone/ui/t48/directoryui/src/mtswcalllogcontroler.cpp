#if IF_FEATURE_METASWITCH_CALLLOG
#include "mtswcalllogcontroler.h"
//#include "directorystring.h"
#include "metaswitch/mtswcontrol/include/modmtsw.h"
//#include "settingui/modsettingui.h"
#include "talk/talklogic/include/modtalklogic.h"

#ifndef DIR_SUPPORT_SINGLE_UIHELPER
#include "mtswcallloguihelper.h"
#endif

#include "settingui/include/modsettingui.h"
#include "account/include/modaccount.h"
#include "moddirectoryui.h"

#include "transfernumselector.h"
#include "directorymgr.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "messagebox/selectmessagebox.h"

#include "threadtask/include/common.h"
#include "uicommon/messagebox/modmessagebox.h"
#if IF_FEATURE_METASWITCH
#include "metaswitch/mtswcontrol/include/mtsw_messagedefine.h"
#endif

CMTSWCallLogControler::CMTSWCallLogControler()
    : CCommonDirCtrl(CALL_LOG_TYPE_MTSW, MTSW_LOG)
    , m_bDownloading(false)
{
    m_bIsNeedDownLoad = true;
}

CMTSWCallLogControler::~CMTSWCallLogControler()
{
}

CMTSWCallLogUIHelper * CMTSWCallLogControler::GetMTSWCallLogUIHelper()
{
#ifdef DIR_SUPPORT_SINGLE_UIHELPER
    return m_pUIHelper;
#else
    return CMTSWCallLogUIHelper::GetUIHelper(m_pUIHelper);
#endif
}

CMTSWCallLogControler * CMTSWCallLogControler::GetControler(CDirectoryControlerPtr & pControler)
{
    if (pControler.IsEmpty() || CALL_LOG_TYPE_MTSW != pControler->GetType())
    {
        return NULL;
    }

    return (CMTSWCallLogControler *)pControler.Get();
}

MTSWCallLogRecord * CMTSWCallLogControler::GetMTSWCallLog(SmartPtr & pDetail)
{
    if (pDetail.IsEmpty() || DIR_TYPE_MTSW_CALLLOG != pDetail->GetType())
    {
        return NULL;
    }

    return (MTSWCallLogRecord *)pDetail.Get();
}

bool CMTSWCallLogControler::PreLoadContact(int nGroupId)
{
    if (!CCommonDirCtrl::PreLoadContact(nGroupId))
    {
        return false;
    }

    DIRUI_INFO("CMTSWCallLogControler::PreLoadContact %d", nGroupId);

    if (m_bDownloading || !m_bIsNeedDownLoad || talklogic_SessionExist())
    {
        DIRUI_INFO("-=-=-=-= do not download metaswitch call log =-=-=-=-");
        return true;
    }

    //若没有登录Commportal则跳转登录界面
    if (!MTSW_IsLogined())
    {
        DIRUI_INFO("-=-=-=-= MTSW_IsLogined fail =-=-=-=-");
        CDirUIHelper::EnterUI(CALL_LOG_TYPE_MTSW, DIR_UI_STATE_CHG_PWD, DIR_ACTION_MTSW_CHG_PWD,
                              m_nGroupId);
        return true;
    }

    //重新下载历史记录
    int nRetCode = MTSWLog_ReqLoadCallLog();

    if (DIR_RET_SUCC != nRetCode)
    {
        CDirUIHelper::ShowErrorMsg(nRetCode);
        return true;
    }

    m_bDownloading = true;
    CDirUIHelper::ShowWaiting(TRID_NOTE, TRID_LOADING_PLEASE_WAIT, DIR_ACTION_MTSW_DOWNLOAD);
    return true;
}

bool CMTSWCallLogControler::LoadMenuData(int nDataIndex, int nDataCount)
{
    return CCommonDirCtrl::LoadMenuData(nDataIndex, nDataCount);
}

bool CMTSWCallLogControler::DialCallLog(int nAccountId/* = AUTO_ACCOUNT_IDENTIFY*/)
{
    return DialContact(m_nContactId, nAccountId);
}

bool CMTSWCallLogControler::EditCallLog()
{
    SmartPtr pDetail = Dir_GetDetailById(m_nContactId, m_eLogicType);
    MTSWCallLogRecord * pRecord = GetMTSWCallLog(pDetail);

    if (NULL == pRecord)
    {
        DIRUI_ERR("GetMTSWCallLog(ContactId:%d) fail EditCallLog", m_nContactId);
        return false;
    }

    talklogic_EnterDialUI(pRecord->GetNumber(), acc_GetDefaultAccount());

    return true;
}

void CMTSWCallLogControler::OnCopyContact(int nAction, int nId)
{
    SmartPtr pDetail = Dir_GetDetailById(nId, m_eLogicType);
    MTSWCallLogRecord * pMTSWCallLog = GetMTSWCallLog(pDetail);

    if (NULL == pMTSWCallLog)
    {
        return;
    }

    LIST_ATTRIBUTE lsAttr;
    ST_CONTACT_ATTRIBUTE stAttr;

    if (nAction == DIR_ACTION_LOCAL_ENTER_COPY_TO_CONTACTS)
    {
        stAttr.m_strName = ATTR_NAME;
        stAttr.m_strValue = pMTSWCallLog->GetDisplayName();
        lsAttr.push_back(stAttr);

        stAttr.m_strName = ATTR_NUMBER;
        stAttr.m_strValue = pMTSWCallLog->GetNumber();
        lsAttr.push_back(stAttr);

        CDirUIHelper::SetAttributeList(m_nType, lsAttr);
        CDirUIHelper::EnterUI(DIRECTORY_TYPE_LOCAL, DIR_UI_STATE_ADDITION, nAction, knInvalidRId);
    }
    else if (nAction == DIR_ACTION_MTSW_ADD_CONTACTLIST)
    {
        stAttr.m_strName = ATTR_MTSW_FIRST_NAME;
        stAttr.m_strValue = pMTSWCallLog->GetDisplayName();
        lsAttr.push_back(stAttr);
        stAttr.m_strName = ATTR_MTSW_HOMENUMBER;
        stAttr.m_strValue = pMTSWCallLog->GetNumber();
        lsAttr.push_back(stAttr);
        stAttr.m_strName = ATTR_MTSW_WORKNUMBER;
        lsAttr.push_back(stAttr);
        stAttr.m_strName = ATTR_MTSW_MOBILENUMBER;
        lsAttr.push_back(stAttr);
        CDirUIHelper::SetAttributeList(m_nType, lsAttr);
        //DM_AddDirectory(pMTSWCallLog->GetDisplayName(), pMTSWCallLog->GetNumber(),nAction, DIRECTORY_TYPE_METASWICTH );
        CDirUIHelper::EnterUI(DIRECTORY_TYPE_METASWICTH, DIR_UI_STATE_ADDITION, nAction, kszMtswAllContact);
    }

}

bool CMTSWCallLogControler::OnAction(int nAction, int nDataIndex, int nId)
{
    DIRUI_INFO("-=-=-=  CMTSWCallLogControler::OnAction(Action:%d Index:%d Id:%d) =-=-=-=",
               nAction, nDataIndex, nId);

    if (CCommonDirCtrl::OnAction(nAction, nDataIndex, nId))
    {
        return true;
    }
    switch (nAction)
    {
    case DIR_ACTION_LCLOG_EDIT:
        {
            EditCallLog();
        }
        break;
    case DIR_ACTION_LOCAL_ENTER_COPY_TO_CONTACTS:
    case DIR_ACTION_MTSW_ADD_CONTACTLIST:
        {
            OnCopyContact(nAction, nId);
        }
        break;
    case DIR_ACTION_MTSW_CHG_PWD:
        {
            m_bDownloading = true;
            CDirUIHelper::ShowWaiting(TRID_NOTE, TRID_LOADING_PLEASE_WAIT,
                                      DIR_ACTION_MTSW_DOWNLOAD);
        }
        break;
    default:
        {
            return false;
        }
        break;
    }
    return true;
}

bool CMTSWCallLogControler::ExecAction(int nAction, int nDataIndex, int nId)
{
    DIRUI_INFO("CMTSWCallLogControler::ExecAction(Action:%d Index:%d Id:%d)",
               nAction, nDataIndex, nId);

    switch (nAction)
    {
    case DIR_ACTION_MTSW_ERROR_PWD:
        {
            CDirUIHelper::EnterUI(CALL_LOG_TYPE_MTSW, DIR_UI_STATE_CHG_PWD, DIR_ACTION_MTSW_CHG_PWD,
                                  m_nGroupId);
        }
        break;
    default:
        {
            return false;
        }
        break;
    }
    return true;
}

bool CMTSWCallLogControler::CancelAction(int nAction, int nDataIndex, int nId)
{
    if (DIR_ACTION_MTSW_DOWNLOAD == nAction)
    {
        m_bDownloading = false;
        m_bIsNeedDownLoad = true;
        CDirUIHelper::RefreshUI(false, false);
        //CDirUIHelper::ExitCurrentUI(UI_CALLLOG);
        return true;
    }
    return false;
}

bool CMTSWCallLogControler::DialContact(int nContactId, int nAccountId/* = AUTO_ACCOUNT_IDENTIFY*/)
{
    SmartPtr pDetail = Dir_GetDetailById(nContactId, m_eLogicType);
    MTSWCallLogRecord * pRecord = GetMTSWCallLog(pDetail);

    if (NULL == pRecord)
    {
        DIRUI_ERR("GetMTSWCallLog(ContactId:%d) fail DialContact", nContactId);
        return false;
    }

    return CDirUIHelper::DialCalllog(nAccountId, pRecord->GetNumber(), this, pRecord);
}

LRESULT CMTSWCallLogControler::OnMessage(msgObject & refMessage)
{
//  if (refMessage.message < METASWITCH_CALLLOG_MESSAGE_BEGIN || refMessage.message > METASWITCH_CALLLOG_MESSAGE_END)
//  {
//      return FALSE;
//  }

    DIRUI_INFO("-=-=-= CMTSWCallLogControler::OnMessage(Msg:%d wp:%d lp:%d) =-=-=-",
               refMessage.message, refMessage.wParam, refMessage.lParam);

    if (METASWITCH_CALLLOG_DOWNLOADED_RESULT == refMessage.message)
    {
        CMTSWCallLogUIHelper * pUIHelper = GetMTSWCallLogUIHelper();

        if (NULL == pUIHelper)
        {
            return FALSE;
        }

        if (m_bDownloading && !MTSWLog_IsDownLoading())
        {
            CDirUIHelper::EndWaiting();
            m_bDownloading = false;
        }

        if (refMessage.lParam == 1)
        {
            CDirUIHelper::RefreshUI(true, true);
            m_bIsNeedDownLoad = false;
        }
        else if (refMessage.lParam == NS_TA::TA_FILE_EXCEED_MAX_SIZE)
        {
            CDirUIHelper::ShowMsgBox(TRID_DOWNLOADFILE_OUT_OF_MEMORY);
            DIRUI_INFO("-=-=-=-= DOWNLOAD FAIL ERROR:%d  =-=-=-=-", refMessage.lParam);
        }
        else
        {
            CDirUIHelper::ShowMsgBox(TRID_LOAD_FAILED);
            DIRUI_INFO("-=-=-=-= DOWNLOAD FAIL ERROR:%d  =-=-=-=-", refMessage.lParam);
        }

        return TRUE;
    }
    else if (MTSW_MESSAGE_AUTH_ERR == refMessage.message)
    {
        MessageBox_RemoveAllMessageBox();

        m_bDownloading = false;
        NS_TA::TaskActionErrorType eErrCode = (NS_TA::TaskActionErrorType)refMessage.lParam;
        if ((eErrCode == NS_TA::TA_HTTP_DOWNLOAD_UNAUTHORIZED)
                || eErrCode == NS_TA::TA_HTTP_ERR403)
        {
            CDirUIHelper::PopSureMsgBox(TRID_WARNING, TRID_COMMPORTAL_LOGIN_FAILED,
                                        DIR_ACTION_MTSW_ERROR_PWD, knInvalidRId);
        }
        else
        {
            CDirUIHelper::PopSureMsgBox(TRID_WARNING, TRID_LOGIN_FAILED,
                                        DIR_ACTION_MTSW_ERROR_PWD, knInvalidRId);
        }
    }

    return FALSE;
}

#endif //IF_FEATURE_METASWITCH_CALLLOG
