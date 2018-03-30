#ifdef IF_FEATURE_BROADSOFT_MUSIC_ON_HOLD

#include "musiconholdprocess.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/xsiactionhelper/include/xsiaction.h"
#include "xmlparser/xmlparser.hpp"

#define MOH_SERVICE_NAME "MusicOnHold"

CMusicOnHoldProcessor & CMusicOnHoldProcessor::GetInstance()
{
    static CMusicOnHoldProcessor pInst;
    return pInst;
}

//设置MusicOnHold
bool CMusicOnHoldProcessor::SetMOHState(bool bActive, int nLineID)
{
    yl::string strFile = XsiAction::GetRandomFile();

    m_bActive = bActive;
    if (SaveRequestCmdToFile(strFile.c_str()))
    {
        return ExeXsiAction(nLineID, XSI_MUSICONHOLD_PUT, "", strFile) != NULL;
    }
    return false;
}

//获取服务器上的MusicOnHold信息
bool CMusicOnHoldProcessor::QueryMOH(int nLineID)
{
    return ExeXsiAction(nLineID, XSI_MUSICONHOLD_GET) != NULL;
}

//获取上次从服务器请求的MusicOnHold信息
bool CMusicOnHoldProcessor::GetMOHState(int nLineID)
{
    YLHashMap<int, bool>::iterator iter = m_mapMOHInfo.find(nLineID);
    if (iter != m_mapMOHInfo.end())
    {
        return iter->second;
    }
}

CMusicOnHoldProcessor::CMusicOnHoldProcessor(void)
{
    XsiAction::InsertMap(XSI_MUSICONHOLD_GET, "services/MusicOnHold");
    XsiAction::InsertMap(XSI_MUSICONHOLD_PUT, "services/MusicOnHold");
}

CMusicOnHoldProcessor::~CMusicOnHoldProcessor(void)
{
}

//解析XML数据
bool CMusicOnHoldProcessor::ParseReponseCmdFromFile(const yl::string & strFilePath)
{
    if (strFilePath.empty())
    {
        return false;
    }
    xml_document doc;
    doc.load_file(strFilePath.c_str());

    xml_node nodeRoot = doc.child(MOH_SERVICE_NAME);
    if (!nodeRoot.empty())
    {
        return GetXmlChildText(nodeRoot, "active", m_bActive);
    }
    return false;
}

//保存要提交到服务器的数据到文件
bool CMusicOnHoldProcessor::SaveRequestCmdToFile(const yl::string & strFilePath)
{
    if (strFilePath.empty())
    {
        return false;
    }
    xml_document doc;

    xml_node nodeDel = doc.append_child(node_declaration);
    nodeDel.append_attribute("version", "1.0");
    nodeDel.append_attribute("encoding", "ISO-8859-1");

    xml_node nodeRoot = doc.append_child(node_element);
    nodeRoot.set_name(MOH_SERVICE_NAME);
    nodeRoot.append_attribute("xmlns", "http://schema.broadsoft.com/xsi");
    PutXmlChildBool(nodeRoot, "active", m_bActive);

    doc.save_file(strFilePath.c_str());
    return true;
}

//override.
bool CMusicOnHoldProcessor::ProcessByType(XsiAction * pAction)
{
    bool bSucc = true;
    if (pAction != NULL)
    {
        switch (pAction->GetXsiType())
        {
        case XSI_MUSICONHOLD_GET:
            {
                bSucc = ParseReponseCmdFromFile(pAction->GetFilePath());
                m_mapMOHInfo[pAction->GetLineId()] = m_bActive;//保存结果
            }
            break;
        case XSI_MUSICONHOLD_PUT:
            {
                m_mapMOHInfo[pAction->GetLineId()] = m_bActive;//保存结果
            }
            break;
        default:
            break;
        }
    }
    return bSucc;
}

#endif
