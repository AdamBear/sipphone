#include "acdmodelstart2start.h"
#include "configiddefine.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "configparser/modconfigparser.h"
#include "extended/xmlbrowser/include/modxmlbrowser.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "broadsoft/callcenter/include/ccmessagedefine.h"

#if IF_FEATURE_START2START_ACD

#define ACD_REASON_CODE_SECTION "ReasonCode"

CACDModelStart2Start::CACDModelStart2Start() : m_iTimer(0)
{
    Set_ACDType(ACD_START2START);
    SetStatus(AS_IDLE);

    //ReSetTimer(0);
}

CACDModelStart2Start::~CACDModelStart2Start()
{
}

bool CACDModelStart2Start::ReqLogin(acd_accout * pAcd)
{
    return ReqChangeACDStatus(AA_LOGIN);
}

bool CACDModelStart2Start::IsUnavailReasonCodeOn()
{
    if (GetAccountId() == -1)
    {
        return false;
    }

    return configParser_GetCfgItemIntValue(kszAccountACDUnavailReasonSwitch, GetAccountId()) == 1;
}

void CACDModelStart2Start::SetStatus(ACD_STATUS eStatus)
{
    if (eStatus >= AS_IDLE && eStatus < AS_WRAPUP)
    {
        // 不处理 AS_WRAPUP
        CACDModelBase::SetStatus(eStatus);

        etl_NotifyApp(false,
                      ACD_MESSAGE_STATUS_CHANGE, GetAccountId(), eStatus);
    }
}

bool CACDModelStart2Start::ReSetTimer(unsigned long iTimer)
{
    CALLCENTER_INFO("ACD S2S ReSetTimer [%ld] !", iTimer);

    if (iTimer < 10 || iTimer > (3600 * 24 * 3))
    {
        //有效区间 [10s, 3天], 默认30分钟
        iTimer = 60 * 10;
    }

    m_iTimer = iTimer;

    return m_objTimer.SetTimer(m_iTimer * 1000, NULL, MK_MSG_ENTRY(this,
                               CACDModelStart2Start::OnTimeoutMessage));
}

BOOL CACDModelStart2Start::OnTimeoutMessage(msgObject & refObj)
{
    CALLCENTER_INFO("ACD S2S auto refresh!");
    //发送消息通知UI请求超时
    etl_NotifyApp(false,
                  ACD_MESSAGE_ACTION_AUTO_REFRESH, 0, 0);

    return TRUE;
}

bool CACDModelStart2Start::ReqChangeACDStatus(ACD_ACTION eACDAction)
{
    bool bHalded = false;
    switch (eACDAction)
    {
    case AA_LOGIN:
        {
            bHalded = OnAction(ACTION_LOGIN);
        }
        break;
    case AA_LOGOUT:
        {
            bHalded = OnAction(ACTION_LOGOUT);
        }
        break;
    case AA_REFRESH:
        {
            bHalded = OnAction(ACTION_SYNC);
        }
        break;
    case AA_AVAIL:
        {
            bHalded = OnAction(ACTION_SET_AVAIL);
        }
        break;
    case AA_UNAVAIL:
        {
            bHalded = OnAction(ACTION_SET_UNAVAIL);
        }
        break;
    default:
        break;
    }

    return bHalded;
}

bool CACDModelStart2Start::InitCalloutParam()
{

    yl::string strLine = configParser_GetCfgItemStringValue(kszAccountCallInfo, GetAccountId());

    // 形如: 806<1>1005<2>1234    第一个固定为呼出号码，之后是时间、号码对  如：时间间隔1秒之后发送DTMF:1005表示为： <1>1005

    m_CallOutParam.clear();

    ParseCallOutParam(strLine, "<>[]");

    return m_CallOutParam.size() > 0;
}

yl::string CACDModelStart2Start::GetNextCallOutParam()
{
    // 获取下一个参数，只能获取一次，返回副本，原参数被清除
    yl::string strNextParam;
    if (m_CallOutParam.size() > 0)
    {
        strNextParam = m_CallOutParam.front();
        m_CallOutParam.erase(m_CallOutParam.begin());
    }

    return strNextParam;
}

void CACDModelStart2Start::ParseCallOutParam(const yl::string & strSrc,
        const yl::string & filterStr)
{
    if (!strSrc.empty())
    {
        size_t begpos = 0;
        size_t found = strSrc.find_first_of(filterStr, begpos);
        while (found != yl::string::npos)
        {
            if (begpos < found)
            {
                m_CallOutParam.push_back(strSrc.substr(begpos, found - begpos));
            }

            begpos = found + 1;
            found = strSrc.find_first_of(filterStr, begpos);
        }

        if (begpos < strSrc.length())
        {
            // 剩余字符串
            m_CallOutParam.push_back(strSrc.substr(begpos));
        }
    }
}

bool CACDModelStart2Start::OnAction(ACTION_TYPE eType)
{
    CALLCENTER_INFO("CACDModelStart2Start::OnAction [%d] ", eType);
    switch (eType)
    {
    case ACTION_LOGIN:
    case ACTION_LOGOUT:
        {
            if (!InitCalloutParam())
            {
                return false;
            }

            if (CallOut(GetNextCallOutParam(), "", GetAccountId()))
            {
                return talkLogic_SetCalloutType(TCT_START2START_ACD_AUTO_LOGIN);
            }
        }
        break;
    case ACTION_SYNC:
    case ACTION_SET_AVAIL:
    case ACTION_SET_UNAVAIL:
        {
            // 重设定时器
            ReSetTimer(m_iTimer);
            return ProcessAction(eType);
        }
        break;
    default:
        break;
    }

    return false;
}

bool CACDModelStart2Start::ProcessAction(ACTION_TYPE eType)
{
    if (!talklogic_SessionExist())
    {
        xmlbrowser_EnableTips(false);
        xmlbrowser_Enter(GetURlByType(eType).c_str(), true, false);

        return true;
    }

    return false;
}

yl::string CACDModelStart2Start::GetURlByType(ACTION_TYPE eType)
{
#define REASON_VAR  "#REASONCODE"
#define REASON_STR  "reason_code=" REASON_VAR

    yl::string strLine;
    switch (eType)
    {
    case ACTION_SYNC:
        {

            strLine = configParser_GetCfgItemStringValue(kszAccountLoginOut, GetAccountId());

        }
        break;
    case ACTION_SET_AVAIL:
        {
            strLine = configParser_GetCfgItemStringValue(kszAccountSetAvail, GetAccountId());
        }
        break;
    case ACTION_SET_UNAVAIL:
        {

            strLine = configParser_GetCfgItemStringValue(kszAccountSetUnAvail, GetAccountId());

            yl::string::size_type pos = strLine.find(REASON_STR);
            if (pos != yl::string::npos)
            {
                pos += strlen(REASON_STR) - strlen(REASON_VAR); // 替换 REASON_VAR
                if (pos >= 0 && pos < strLine.length())
                {
                    strLine.replace(pos, strlen(REASON_VAR), m_strCurReason);
                }
            }
        }
        break;
    default:
        break;
    }

    CALLCENTER_INFO("CACDModelStart2Start::GetURlByType [%d][%s] ", eType, strLine.c_str());
    return strLine;
}

#endif // IF_FEATURE_START2START_ACD
