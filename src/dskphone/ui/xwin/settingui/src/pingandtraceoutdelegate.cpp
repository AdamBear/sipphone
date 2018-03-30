#include "pingandtraceoutdelegate.h"
#include <settinguilogic/include/settingdefine.h>
#include "uicallbackfun.h"
#include "setuidatadefine.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/pageiddefine.h"
#include "settingui/include/settinguimanager.h"
#include "settinguilogic/include/settinguicommondata.h"
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "baseui/include/baseuicommon.h"
#include "uimanager/moduimanager.h"
#include "diagnostics/include/moddiagnostics.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "messagebox/modmessagebox.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

namespace
{
//等待的提示ID
#define ID_NOTE_WAIT 1
//输入url地址的提示ID
#define ID_NOTE_INPUT_URL 2
//执行检测失败
#define ID_NOTE_CHECK_ERROR 3

//输入url地址的等待时间
#define MSCBOX_TIME_URL_ERROR 2000
//等待提示的等待时间
#define MSCBOX_TIME_WAIT_RESULT -1
//执行检测失败的等待时间
#define MSCBOX_TIME_CHECK_ERROR 2000
}

CPingAndTraceoutDelegate::CPingAndTraceoutDelegate()
    : m_eShowInfoType(NETWORK_CHECK_SHOWINFO_TYPE_IP)
{
    etl_RegisterMsgHandle(DIAGNOISTICMSG_FINISH, DIAGNOISTICMSG_FINISH,
                          &CPingAndTraceoutDelegate::OnMessage);
}

CPingAndTraceoutDelegate::~CPingAndTraceoutDelegate()
{
    etl_UnregisterMsgHandle(DIAGNOISTICMSG_FINISH, DIAGNOISTICMSG_FINISH,
                            &CPingAndTraceoutDelegate::OnMessage);
}

CSettingUIDelegateBase * CPingAndTraceoutDelegate::CreatePingAndTraceDelegate()
{
    CPingAndTraceoutDelegate * pDelegate = new CPingAndTraceoutDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

void CPingAndTraceoutDelegate::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return ;
    }

    int nResult = pMessageBox->GetResult();
    int nID = pMessageBox->GetID();

    //输入URL的提示   或者  检测失败
    if (ID_NOTE_INPUT_URL == nID || ID_NOTE_CHECK_ERROR == nID)
    {
        return;
    }

    //等待的提示
    if (ID_NOTE_WAIT == nID)
    {
        if (MessageBox_Cancel == nResult || MessageBox_Destory == nResult)
        {
            //取消执行检测命令
            Diagnostics_StopSysCmd();
        }
        else if (MessageBox_Close == nResult)
        {
            //进入结果界面,传入对应的界面类型
            yl::string strPage = (kszPageIDTraceOut == GetPageID()) ? kszPageIDTraceResult :
                                 kszPageIDPingResult;
            PageOpenData opPageData(strPage);
            g_SetUICallBackFun.EnterPage(opPageData);
        }

        m_eShowInfoType = NETWORK_CHECK_SHOWINFO_TYPE_IP;
        UIManager_LockReturnToIdle(false);
    }
}

LRESULT CPingAndTraceoutDelegate::OnMessage(msgObject & msg)
{
    if (DIAGNOISTICMSG_FINISH == msg.message)
    {
        CSettingUIDelegateBase * pDelegateBase = g_pSettingUIManager->GetPageDelegate(kszPageIDTraceOut);
        if (NULL == pDelegateBase)
        {
            pDelegateBase = g_pSettingUIManager->GetPageDelegate(kszPageIDPing);
        }

        CPingAndTraceoutDelegate * pDelegate = static_cast<CPingAndTraceoutDelegate *>(pDelegateBase);

        if (NULL != pDelegate)
        {
            pDelegate->OnDiagnosticsResultMessage();
            return TRUE;
        }
    }

    return FALSE;
}

bool CPingAndTraceoutDelegate::OnAction(int eActionType, int iDataIndex)
{
    if (NULL == m_pPageData)
    {
        return false;
    }

    if (SET_ACTION_TYPE_CLICK == eActionType || SET_ACTION_TYPE_START == eActionType)
    {
        StartNetworkCheck();
        return true;
    }

    return CSettingUIDelegateBase::OnAction(eActionType, iDataIndex);
}

void CPingAndTraceoutDelegate::StartNetworkCheck()
{
    //不在输入IP的界面 不用处理
    if (NETWORK_CHECK_SHOWINFO_TYPE_IP != m_eShowInfoType)
    {
        return ;
    }

    yl::string strURL = "";
    bool bInPageTrace = (kszPageIDTraceOut == GetPageID()) ? true : false;
    if (bInPageTrace)
    {
        strURL = GetValue(DIAGNOISTICS_URL_TRACEOUT);
    }
    else
    {
        strURL = GetValue(DIAGNOISTICS_URL_PING);
    }

    //编辑框中的字符串为空
    if (strURL.empty())
    {
        ShowErrorMsgBox(_LANG(TRID_PLEASE_ENTER_AN_IP_ADDRESS_OR_DOMAIN_NAME), ID_NOTE_INPUT_URL,
                        MSCBOX_TIME_URL_ERROR);
        return ;
    }

    DIAGNOSTICSTYPE eSysCMD = DIAGNOSTICSTYPE_NONE;
    if (bInPageTrace)
    {
        eSysCMD = DIAGNOSTICSTYPE_TRACEROUTE;
    }
    else
    {
        eSysCMD = DIAGNOSTICSTYPE_PING;
    }

    //执行检测成功
    if (Diagnostics_RunSysCmd(eSysCMD, strURL))
    {
        //调用成功才执行以下语句
        m_eShowInfoType = NETWORK_CHECK_SHOWINFO_TYPE_WAIT; //设置为等待状态
        ShowWaitingMsgBox();
    }
    else        //执行检测失败
    {
        yl::string strErrorInfo = bInPageTrace ? _LANG(TRID_TRACE_ROUTE_ERROR) : _LANG(TRID_PING_ERROR);
        ShowErrorMsgBox(strErrorInfo, ID_NOTE_CHECK_ERROR, MSCBOX_TIME_CHECK_ERROR);
    }
}

void CPingAndTraceoutDelegate::OnDiagnosticsResultMessage()
{
    //如果当前不处于等待状态 那么直接返回
    if (NETWORK_CHECK_SHOWINFO_TYPE_WAIT != m_eShowInfoType)
    {
        return ;
    }
    //处理相关操作
    //当前状态设置为是显示结果状态
    m_eShowInfoType = NETWORK_CHECK_SHOWINFO_TYPE_RESULT;

    //关闭弹出框
    MessageBox_RemoveMessageBox(this, MessageBox_Close, ID_NOTE_WAIT);
}

void CPingAndTraceoutDelegate::ShowWaitingMsgBox()
{
    MSGBOX_INFO infoMsgBox;

    infoMsgBox.pAction = this;
    infoMsgBox.eType = MESSAGEBOX_Cancel;
    infoMsgBox.strNote = _LANG(TRID_PLEASE_WAIT);
    infoMsgBox.iID = ID_NOTE_WAIT;
    infoMsgBox.nMsTime = MSCBOX_TIME_WAIT_RESULT;

    MessageBox_ShowCommonMsgBox(infoMsgBox);

    UIManager_LockReturnToIdle(true);
}

void CPingAndTraceoutDelegate::ShowErrorMsgBox(const yl::string & strNote, int nID, int nTime)
{
    MSGBOX_INFO infoMsgBox;

    infoMsgBox.pAction = this;
    infoMsgBox.eType = MESSAGEBOX_NOTE;
    infoMsgBox.strNote = strNote;
    infoMsgBox.iID = nID;
    infoMsgBox.nMsTime = nTime;

    MessageBox_ShowCommonMsgBox(infoMsgBox);
}
