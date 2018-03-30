#include "callpark_include.h"

// 消息处理
static LRESULT OnCallParkProcess(msgObject & msg)
{
    switch (msg.message)
    {
    case SIP_SUB_BSFT_CALLPARK_UPDATE:
        {
            g_objCallParkProcesser.ProcessCallParkNotifMSG(msg);
        }
        break;
    default:
        break;
    }

    return 1;
}


CParkProcesser & CParkProcesser::GetInstance()
{
    static CParkProcesser s_Inst;

    return s_Inst;
}

CParkProcesser::CParkProcesser()
{
    XsiAction::InsertMap(XSI_PARK_RETRIVE, "calls/parkretrieve/");

    XsiAction::InsertMap(XSI_CALLPARK, "calls/");

    XsiAction::InsertMap(XSI_GROUPPARK, "calls/");

    etl_RegisterMsgHandle(SIP_SUB_BSFT_CALLPARK_UPDATE, SIP_SUB_BSFT_CALLPARK_UPDATE,
                          OnCallParkProcess);
}

CParkProcesser::~CParkProcesser()
{
    etl_UnregisterMsgHandle(SIP_SUB_BSFT_CALLPARK_UPDATE, SIP_SUB_BSFT_CALLPARK_UPDATE,
                            OnCallParkProcess);
}

bool CParkProcesser::CallPark(int iCallId, const yl::string & strTarget,
                              const yl::string & strXsiCallId, int iAccountId)
{
    if (!SetLineID(iAccountId))
    {
        return false;
    }

    m_xsiExtenInfo.m_nEntenParm = iCallId;

    yl::string strExtenUrl = strXsiCallId;

    strExtenUrl.append("/Park");

    if (!strTarget.empty())
    {
        strExtenUrl.append("?address=").append(strTarget);
    }

    XsiAction * pAction = ExeXsiAction(iAccountId, XSI_CALLPARK, strExtenUrl);

    if (pAction != NULL)
    {
        pAction->SetExtraParam(iCallId);
        return true;
    }

    return false;
}

bool CParkProcesser::GroupPark(int iCallId, const yl::string & strXsiCallId, int iAccountId)
{
    if (!SetLineID(iAccountId))
    {
        return false;
    }

    m_xsiExtenInfo.m_nEntenParm = iCallId;

    yl::string strExtenUrl = strXsiCallId;

    strExtenUrl.append("/GroupCallPark");

    XsiAction * pAction = ExeXsiAction(iAccountId, XSI_GROUPPARK, strExtenUrl);

    if (pAction != NULL)
    {
        pAction->SetExtraParam(iCallId);
        return true;
    }

    return false;
}

bool CParkProcesser::RetrieveCallPark(const yl::string & strTarget, int iAccountId)
{
    if (!SetLineID(iAccountId))
    {
        return false;
    }

    yl::string strExtenUrl;

    if (!strTarget.empty())
    {
        strExtenUrl.append("?address=").append(strTarget);
    }

    return ExeXsiAction(iAccountId, XSI_PARK_RETRIVE, strExtenUrl) != NULL;
}


void CParkProcesser::ProcessCallParkNotifMSG(msgObject & msg)
{
    if (configParser_GetConfigInt(kszCallParkVisualNotifySwitch) == 0
            || configParser_GetConfigInt(kszBroadsoftActive) == 0)
    {
        return;
    }
    // parse xml
    xml_document docXmlDoc;
    docXmlDoc.load_buffer((LPCSTR)msg.GetExtraData(), msg.GetExtraSize());

    m_objNotifyData.Reset();

    bool bEnable = false;
    xml_node nodeIdentity = docXmlDoc.first_child().child("callpark").child("parked").child("identity");
    if (!nodeIdentity.empty())
    {
        m_objNotifyData.strDiaplayName = nodeIdentity.attribute("display").value();
        m_objNotifyData.strNumber      = nodeIdentity.text().get();
        size_t ibegin, iend;
        ibegin = m_objNotifyData.strNumber.find("sip:");
        iend = m_objNotifyData.strNumber.find("@");
        if (ibegin != yl::string::npos && iend != yl::string::npos)
        {
            m_objNotifyData.strNumber = m_objNotifyData.strNumber.substr(ibegin + 4, iend - ibegin - 4);
            CALLPARK_INFO("ProcessParkSIPMsg strNumber = %s strDisplay = %s", m_objNotifyData.strNumber.c_str(),
                          m_objNotifyData.strDiaplayName.c_str());
        }

        if (!m_objNotifyData.strNumber.empty())
        {
            bEnable = true;
        }
    }

    CALLPARK_INFO("msg.wParam = %d,bEnable = %d", msg.wParam, bEnable);

    DataParkNotifyEx dataParkNotify;
    strncpy(dataParkNotify.szNumber, m_objNotifyData.strNumber.c_str(), MAX_CALL_PARK_NAME_LEN);
    dataParkNotify.szNumber[MAX_CALL_PARK_NAME_LEN - 1] = '\0';

    strncpy(dataParkNotify.szDisplayName, m_objNotifyData.strDiaplayName.c_str(),
            MAX_CALL_PARK_NAME_LEN);
    dataParkNotify.szDisplayName[MAX_CALL_PARK_NAME_LEN - 1] = '\0';

    etl_NotifyAppEx(false,
                    CALL_PARK_NOTIFY,
                    msg.wParam,
                    bEnable,
                    sizeof(DataParkNotifyEx),
                    &dataParkNotify);
}
