#ifdef IF_FEATURE_BROADSOFT_CALL_FORWARD_NOT_REACHABLE

#include "callforwardnotreachableprocess.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/xsiactionhelper/include/xsiaction.h"
#include "dsklog/log.h"

#define  CFNR_SERVICE_NAME "CallForwardingNotReachable"

CCFNRProcessor * CCFNRProcessor::GetInstance()
{
    static CCFNRProcessor s_Inst;
    return &s_Inst;
}

CCFNRProcessor::CCFNRProcessor(void)
{
    XsiAction::InsertMap(XSI_CALLFORWARD_NOTREACHABLE_GET, "services/CallForwardingNotReachable");
    XsiAction::InsertMap(XSI_CALLFORWARD_NOTREACHABLE_PUT, "services/CallForwardingNotReachable");
}

CCFNRProcessor::~CCFNRProcessor(void)
{

}

//设置Forward Not Reachable
bool CCFNRProcessor::SetCFNR(bool bState, const char * szForwardNumber, int nLineID)
{
    yl::string strFile = XsiAction::GetRandomFile();

    m_sCFNR.bActive = bState;
    m_sCFNR.strPhoneNumber = szForwardNumber;
    if (SaveRequestCmdToFile(strFile.c_str()))
    {
        return ExeXsiAction(nLineID, XSI_CALLFORWARD_NOTREACHABLE_PUT, "", strFile) != NULL;
    }
    return false;
}

//获取服务器上的Forward Not Reachable信息
bool CCFNRProcessor::QueryCFNR(int nLineID)
{
    return ExeXsiAction(nLineID, XSI_CALLFORWARD_NOTREACHABLE_GET) != NULL;
}

//获取上次从服务器请求的Forward Not Reachable信息
void CCFNRProcessor::GetCFNRInfo(SCallForwardNotReachable & cfnrInfo, int nLineID)
{
    YLHashMap<int, SCallForwardNotReachable>::iterator iter = m_mapCFNR.find(nLineID);
    if (m_mapCFNR.end() != iter)
    {
        cfnrInfo = iter->second;
    }
}

//保存服务器返回的结果
void CCFNRProcessor::SaveCFNR(const SCallForwardNotReachable & sCFNR, int nLineID)
{
    m_mapCFNR[nLineID] = sCFNR;
}

//override
bool CCFNRProcessor::ProcessByType(XsiAction * pAction)
{
    bool bSucc = true;
    if (NULL != pAction)
    {
        switch (pAction->GetXsiType())
        {
        case XSI_CALLFORWARD_NOTREACHABLE_GET:
        {
            bSucc = ParseReponseCmdFromFile(pAction->GetFilePath().c_str());
            SaveCFNR(m_sCFNR, pAction->GetLineId());
        }
        break;
        case XSI_CALLFORWARD_NOTREACHABLE_PUT:
        {
            SaveCFNR(m_sCFNR, pAction->GetLineId());
        }
        break;
        default:
            break;
        }
    }
    return bSucc;
}

//解析XML数据,返回操作结果
bool CCFNRProcessor::ParseReponseCmdFromFile(const yl::string & strFilePath)
{
    xml_document doc;
    xml_parse_result objResult = doc.load_file(strFilePath.c_str());
    if (objResult.status != status_ok)
    {
        return false;
    }

    //获取根结点
    xml_node nodeRoot = doc.child(CFNR_SERVICE_NAME);
    if (nodeRoot.empty())
    {
        CALLCONTROL_INFO("ParseReponseCmdFromFile ReponseCmd %s do not match %s", nodeRoot.value(),
                         CFNR_SERVICE_NAME);
        return false;
    }

    if (!GetXmlChildText(nodeRoot, "active", m_sCFNR.bActive))
    {
        return false;
    }

    if (!GetXmlChildText(nodeRoot, "forwardToPhoneNumber", m_sCFNR.strPhoneNumber))
    {
        m_sCFNR.strPhoneNumber.clear();
    }

    return true;
}

//保存要提交到服务器的数据到文件,如果命令类型不需要发送内容时返回true
bool CCFNRProcessor::SaveRequestCmdToFile(const yl::string & strFilePath)
{
    xml_document doc;

    // 添加声明结点
    xml_node nodeDecl = doc.append_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "ISO-8859-1");

    //增加根结点.
    xml_node nodeRoot  = doc.append_child(node_element);

    nodeRoot.set_name(CFNR_SERVICE_NAME);
    nodeRoot.append_attribute("xmlns", "http://schema.broadsoft.com/xsi");

    PutXmlChildBool(nodeRoot, "active", m_sCFNR.bActive);

    //增加forwardToPhoneNumber节点
    if (!m_sCFNR.strPhoneNumber.empty())
    {
        PutXmlChildText(nodeRoot, "forwardToPhoneNumber", m_sCFNR.strPhoneNumber);
    }
    else
    {
        xml_node nodeNumber = nodeRoot.append_child("forwardToPhoneNumber");
        nodeNumber.append_attribute("xs:nil", "true");
        nodeNumber.append_attribute("xmlns:xs", "http://www.w3.org/2001/XMLSchema-instance");
    }

    doc.save_file(strFilePath.c_str());
    return true;
}

#endif // IF_FEATURE_BROADSOFT_CALL_FORWARD_NOT_REACHABLE
