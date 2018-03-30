#include "remoteofficeprocess.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/xsiactionhelper/include/xsiaction.h"


#define REMOTEOFFICE_SERVICE "RemoteOffice"

CRemoteOfficeProcessor & CRemoteOfficeProcessor::GetInstance()
{
    static CRemoteOfficeProcessor s_Inst;
    return s_Inst;
}

//设置Remote Office
bool CRemoteOfficeProcessor::SetRemoteOffice(bool bState, const char * szRemoteNumber, int nLineID)
{
    yl::string strFile = XsiAction::GetRandomFile();

    m_sRO.bActive = bState;
    m_sRO.strPhoneNumber = szRemoteNumber;

    if (SaveRequestCmdToFile(strFile.c_str()))
    {
        return ExeXsiAction(nLineID, XSI_REMOTEOFFICE_PUT, "", strFile) != NULL;
    }
    return false;
}

//获取服务器上的Remote Office信息，返回的结果是通过发送RO_MESSAGE_GET_RESULT消息来传送的
bool CRemoteOfficeProcessor::QueryRemoteOffice(int nLineID)
{
    return ExeXsiAction(nLineID, XSI_REMOTEOFFICE_GET) != NULL;
}

//获取上次从服务器请求的Remote office信息
void CRemoteOfficeProcessor::GetRemoteOfficeInfo(SRemoteOffice & roInfo, int nLineID)
{
    YLHashMap<int, SRemoteOffice>::iterator iter = m_mapRemoteOffice.find(nLineID);
    if (m_mapRemoteOffice.end() != iter)
    {
        roInfo = iter->second;
    }
}

CRemoteOfficeProcessor::CRemoteOfficeProcessor(void)
{
    XsiAction::InsertMap(XSI_REMOTEOFFICE_GET, "services/RemoteOffice");
    XsiAction::InsertMap(XSI_REMOTEOFFICE_PUT, "services/RemoteOffice");
}

CRemoteOfficeProcessor::~CRemoteOfficeProcessor(void)
{

}

//override,处理cmd执行之后服务器返回的数据
bool CRemoteOfficeProcessor::ProcessByType(XsiAction * pAction)
{
    bool bSucc = true;
    if (NULL != pAction)
    {
        switch (pAction->GetXsiType())
        {
        case XSI_REMOTEOFFICE_GET:
            {
                bSucc = ParseReponseCmdFromFile(pAction->GetFilePath().c_str());
                SaveRemoteOffice(m_sRO, pAction->GetLineId());
            }
            break;
        case XSI_REMOTEOFFICE_PUT:
            {
                SaveRemoteOffice(m_sRO, pAction->GetLineId());
            }
            break;
        default:
            break;
        }
    }
    return bSucc;
}

//保存服务器返回的结果
void CRemoteOfficeProcessor::SaveRemoteOffice(const SRemoteOffice & sRO, int nLineID)
{
    m_mapRemoteOffice[nLineID] = sRO;
}

//解析XML数据
bool CRemoteOfficeProcessor::ParseReponseCmdFromFile(const yl::string & strFilePath)
{
    xml_document doc;
    xml_parse_result objResult = doc.load_file(strFilePath.c_str());
    if (objResult.status != status_ok)
    {
        return false;
    }

    //获取根结点
    xml_node nodeRoot = doc.child(REMOTEOFFICE_SERVICE);
    if (nodeRoot.empty())
    {
        return false;
    }

    if (!GetXmlChildText(nodeRoot, "active", m_sRO.bActive))
    {
        return false;
    }

    if (!GetXmlChildText(nodeRoot, "remoteOfficeNumber", m_sRO.strPhoneNumber))
    {
        m_sRO.strPhoneNumber.clear();
    }

    return true;
}

//保存要提交到服务器的数据到文件,如果命令类型不需要发送内容时返回true
bool CRemoteOfficeProcessor::SaveRequestCmdToFile(const yl::string & strFilePath)
{
    //增加Xml头
    xml_document doc;
    xml_node nodeDecl = doc.append_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "ISO-8859-1");

    //增加根结点
    xml_node nodeRoot = doc.append_child(REMOTEOFFICE_SERVICE);

    // 添加结点属性
    nodeRoot.append_attribute("xmlns", "http://schema.broadsoft.com/xsi");

    PutXmlChildBool(nodeRoot, "active", m_sRO.bActive);
    //增加remoteOfficeNumber节点
    if (!m_sRO.strPhoneNumber.empty())
    {
        PutXmlChildText(nodeRoot, "remoteOfficeNumber", m_sRO.strPhoneNumber.c_str());
    }
    else
    {
        //增加remoteOfficeNumber节点
        xml_node nodeNumber = nodeRoot.append_child("remoteOfficeNumber");
        nodeNumber.append_attribute("xs:nil", "true");
        nodeNumber.append_attribute("xmlns:xs", "http://www.w3.org/2001/XMLSchema-instance");
    }

    doc.save_file(strFilePath.c_str());
    return true;
}
