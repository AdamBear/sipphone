#ifdef IF_FEATURE_BROADSOFT_CALL_WAITING

#include "callwaitingprocess.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/xsiactionhelper/include/xsiaction.h"
#include "xmlparser/xmlparser.hpp"

#define CALLWAITING_SERVICE_NAME "CallWaiting"

CCallWatingProcessor & CCallWatingProcessor::GetInstance()
{
    static CCallWatingProcessor s_Inst;
    return s_Inst;
}

//设置CallWating
bool CCallWatingProcessor::SetCallWatingState(bool bActive, int nLineID)
{
    yl::string strFile = XsiAction::GetRandomFile();

    m_bActive = bActive;
    if (SaveRequestCmdToFile(strFile.c_str()))
    {
        return ExeXsiAction(nLineID, XSI_CALL_WAITING_PUT, "", strFile) != NULL;
    }
    return false;
}

//获取服务器上的CallWating信息
bool CCallWatingProcessor::QueryCallWating(int nLineID)
{
    return ExeXsiAction(nLineID, XSI_CALL_WAITING_GET) != NULL;
}

//获取上次从服务器请求的CallWating信息
bool CCallWatingProcessor::GetCallWatingState(int nLineID)
{
    YLHashMap<int, bool>::iterator iter = m_mapCallWaitingInfo.find(nLineID);
    if (iter != m_mapCallWaitingInfo.end())
    {
        return iter->second;
    }
    return false;
}

CCallWatingProcessor::CCallWatingProcessor(void)
{
    XsiAction::InsertMap(XSI_CALL_WAITING_GET, "services/CallWaiting");
    XsiAction::InsertMap(XSI_CALL_WAITING_PUT, "services/CallWaiting");
}

CCallWatingProcessor::~CCallWatingProcessor(void)
{
}

//解析XML数据
bool CCallWatingProcessor::ParseReponseCmdFromFile(const yl::string & strFilePath)
{
    if (strFilePath.empty())
    {
        return false;
    }
    xml_document doc;
    doc.load_file(strFilePath.c_str());

    xml_node nodeRoot = doc.child(CALLWAITING_SERVICE_NAME);
    if (!nodeRoot.empty())
    {
        return GetXmlChildText(nodeRoot, "active", m_bActive);
    }
    return false;
}

//保存要提交到服务器的数据到文件
bool CCallWatingProcessor::SaveRequestCmdToFile(const yl::string & strFilePath)
{
    if (strFilePath.empty())
    {
        return false;
    }
    xml_document doc;

    xml_node nodeDecl = doc.append_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "ISO-8859-1");

    xml_node nodeRoot = doc.append_child(node_element);
    nodeRoot.set_name(CALLWAITING_SERVICE_NAME);
    nodeRoot.append_attribute("xmlns", "http://schema.broadsoft.com/xsi");

    PutXmlChildBool(nodeRoot, "active", m_bActive);

    doc.save_file(strFilePath.c_str());
    return true;
}

//override.
bool CCallWatingProcessor::ProcessByType(XsiAction * pAction)
{
    bool bSucc = true;
    if (pAction != NULL)
    {
        switch (pAction->GetXsiType())
        {
        case XSI_CALL_WAITING_GET:
            {
                bSucc = ParseReponseCmdFromFile(pAction->GetFilePath());
                m_mapCallWaitingInfo[pAction->GetLineId()] = m_bActive;//保存结果
            }
            break;
        case XSI_CALL_WAITING_PUT:
            {
                m_mapCallWaitingInfo[pAction->GetLineId()] = m_bActive;//保存结果
            }
            break;
        default:
            break;
        }
    }
    return bSucc;
}

#endif // IF_FEATURE_BROADSOFT_CALL_WAITING
