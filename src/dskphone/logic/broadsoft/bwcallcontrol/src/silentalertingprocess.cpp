#include "bwcallcontrol_inc.h"

#define SILENT_ALERTING_KEY_WORD "SilentAlerting"

using namespace pugi;
CSilentAlertingProcessor & CSilentAlertingProcessor::GetInstance()
{
    static CSilentAlertingProcessor s_Inst;
    return s_Inst;
}

CSilentAlertingProcessor::CSilentAlertingProcessor(void)
    : m_pArrBValue(NULL), m_bEnabled(false)
{
    int iSize = acc_AccountNum();
    m_pArrBValue = new bool[iSize];
    memset(m_pArrBValue, 0, sizeof(bool)*iSize);

    XsiAction::InsertMap(XSI_SILENT_ALERTING_GET, "services/" SILENT_ALERTING_KEY_WORD);
    XsiAction::InsertMap(XSI_SILENT_ALERTING_PUT, "services/" SILENT_ALERTING_KEY_WORD);
}

CSilentAlertingProcessor::~CSilentAlertingProcessor(void)
{
    delete [] m_pArrBValue;
}

bool  CSilentAlertingProcessor::Query(int nLineID)
{
    return SetLineID(nLineID) && ExeXsiAction(nLineID, XSI_SILENT_ALERTING_GET) != NULL;
}

bool CSilentAlertingProcessor::GetValue(int nLineID, bool & bValue)
{
    if (nLineID >= 0 && nLineID < acc_AccountNum())
    {
        bValue = m_pArrBValue[nLineID];
        return true;
    }

    return false;
}

bool CSilentAlertingProcessor::Save(int nLineID, bool bEnabled)
{
    if (SetLineID(nLineID))
    {
        m_bEnabled = bEnabled;
        yl::string strFile = XsiAction::GetRandomFile();

        return SaveRequestCmdToFile(strFile)
               && ExeXsiAction(nLineID, XSI_SILENT_ALERTING_PUT, "", strFile) != NULL;
    }

    return false;
}

bool CSilentAlertingProcessor::Store(int nLineID)
{
    if (nLineID >= 0 && nLineID < acc_AccountNum())
    {
        m_pArrBValue[nLineID] = m_bEnabled;
        return true;
    }

    return false;
}

//override
bool CSilentAlertingProcessor::ProcessByType(XsiAction * pAction)
{
    if (NULL == pAction)
    {
        return false;
    }

    switch (pAction->GetXsiType())
    {
    case XSI_SILENT_ALERTING_GET:
        {
            return ParseReponseCmdFromFile(pAction->GetFilePath()) && Store(pAction->GetLineId());
        }
        break;
    case XSI_SILENT_ALERTING_PUT:
        {
            return Store(pAction->GetLineId());
        }
        break;
    default:
        break;
    }

    return false;
}

//解析XML数据
bool CSilentAlertingProcessor::ParseReponseCmdFromFile(const yl::string & strFilePath)
{
    xml_document doc;
    xml_parse_result objResult = doc.load_file(strFilePath.c_str());
    if (objResult.status != status_ok)
    {
        return false;
    }

    //获取根结点
    xml_node nodeRoot = doc.child(SILENT_ALERTING_KEY_WORD);
    return GetXmlChildText(nodeRoot, "active", m_bEnabled);
}

bool CSilentAlertingProcessor::SaveRequestCmdToFile(const yl::string & strFilePath)
{
    //增加Xml头
    xml_document doc;
    xml_node nodeDecl = doc.append_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "ISO-8859-1");

    //增加根结点
    xml_node nodeRoot = doc.append_child(SILENT_ALERTING_KEY_WORD);
    // 添加结点属性
    nodeRoot.append_attribute("xmlns", "http://schema.broadsoft.com/xsi");
    PutXmlChildBool(nodeRoot, "active", m_bEnabled);

    return doc.save_file(strFilePath.c_str());
}
