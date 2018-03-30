#include "mobilityprocess.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/xsiactionhelper/include/xsiaction.h"


CMobilityProcessor & CMobilityProcessor::GetInstance()
{
    static CMobilityProcessor s_Inst;
    return s_Inst;
}

//设置mobility
bool CMobilityProcessor::SetMobility(const SMobility & mobilityInfo, int nLineID)
{
    if (SetLineID(nLineID))
    {
        yl::string strFile = XsiAction::GetRandomFile();

        m_mobilityInfo = mobilityInfo;
        if (SaveRequestCmdToFile(strFile.c_str()))
        {
            return ExeXsiAction(nLineID, XSI_MOBILITY_PUT, "", strFile) != NULL;//
        }
    }
    return false;
}

//获取服务器上的Moblity信息
bool  CMobilityProcessor::QueryMobility(int nLineID)
{
    if (SetLineID(nLineID))
    {
        return ExeXsiAction(nLineID, XSI_MOBILITY_GET) != NULL;
    }
    return false;
}

//获取上次从服务器请求的Mobility信息
void  CMobilityProcessor::GetMobilityInfo(SMobility & mobilityInfo, int nLineID)
{
    YLHashMap<int, SMobility>::iterator iter = m_mapMobilityInfo.find(nLineID);
    if (iter != m_mapMobilityInfo.end())
    {
        mobilityInfo = iter->second;
    }
}

CMobilityProcessor::CMobilityProcessor(void)
{
    XsiAction::InsertMap(XSI_MOBILITY_PUT, "services/BroadworksMobility");
    XsiAction::InsertMap(XSI_MOBILITY_GET, "services/BroadworksMobility");
}

CMobilityProcessor::~CMobilityProcessor(void)
{

}

//override, 处理执行cmd之后从服务器返回的结果
bool CMobilityProcessor::ProcessByType(XsiAction * pAction)
{
    bool bSucc = true;
    if (NULL != pAction)
    {
        switch (pAction->GetXsiType())
        {
        case XSI_MOBILITY_GET:
            {
                bSucc = ParseReponseCmdFromFile(pAction->GetFilePath().c_str());
                SaveMobilityInfo(m_mobilityInfo, pAction->GetLineId());
            }
            break;
        case XSI_MOBILITY_PUT:
            {
                SaveMobilityInfo(m_mobilityInfo, pAction->GetLineId());
            }
            break;
        default:
            break;
        }
    }

    return bSucc;
}

//解析XML数据
bool CMobilityProcessor::ParseReponseCmdFromFile(const yl::string & strFilePath)
{
    // 解析前先清除数据
    m_mobilityInfo.Reset();

    xml_document doc;
    xml_parse_result objResult = doc.load_file(strFilePath.c_str());
    if (objResult.status != status_ok)
    {
        return false;
    }

    xml_node nodeRoot = doc.child("BroadWorksMobility");
    if (nodeRoot.empty())
    {
        return false;
    }

    yl::string strPhoneToRingType;
    //获取active结点
    GetXmlChildText(nodeRoot, "active", m_mobilityInfo.bActive);
    GetXmlChildText(nodeRoot, "phonesToRing", strPhoneToRingType);
    GetXmlChildText(nodeRoot, "mobilePhoneNumber", m_mobilityInfo.strMobilePhoneNumber);
    GetXmlChildText(nodeRoot, "alertClickToDialCalls", m_mobilityInfo.bAlertClickToDialCalls);
    GetXmlChildText(nodeRoot, "alertGroupPagingCalls", m_mobilityInfo.bAlertGroupPagingCalls);
    GetXmlChildText(nodeRoot, "enableDiversionInhibitor", m_mobilityInfo.bEnableDiversionInhibitor);
    GetXmlChildText(nodeRoot, "requireAnswerConfirmation", m_mobilityInfo.bRequireAnswerConfirmation);
    GetXmlChildText(nodeRoot, "broadworksCallControl", m_mobilityInfo.bBroadworksCallControl);
    //找到对应PhoneToRing的Index
    m_mobilityInfo.nPhoneToRing = 0;
    for (int i = 0; i < sizeof(MobilityPhoneToRingType) / sizeof(MobilityPhoneToRingType[0]); ++i)
    {
        if (strPhoneToRingType == MobilityPhoneToRingType[i])
        {
            m_mobilityInfo.nPhoneToRing = i;
            break;
        }
    }
    return true;
}

//保存要提交到服务器的数据到文件
bool CMobilityProcessor::SaveRequestCmdToFile(const yl::string & strFilePath)
{
    //增加Xml头
    xml_document doc;
    xml_node nodeDecl = doc.append_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "UTF-8");

    //增加根结点
    xml_node nodeRoot = doc.append_child("BroadWorksMobility");

    // 设置属性
    nodeRoot.append_attribute("xmlns", "http://schema.broadsoft.com/xsi");

    PutXmlChildBool(nodeRoot, "active", m_mobilityInfo.bActive);
    PutXmlChildText(nodeRoot, "phonesToRing", MobilityPhoneToRingType[m_mobilityInfo.nPhoneToRing]);
    PutXmlChildText(nodeRoot, "mobilePhoneNumber", m_mobilityInfo.strMobilePhoneNumber);
    PutXmlChildBool(nodeRoot, "alertClickToDialCalls", m_mobilityInfo.bAlertClickToDialCalls);
    PutXmlChildBool(nodeRoot, "alertGroupPagingCalls", m_mobilityInfo.bAlertGroupPagingCalls);
    PutXmlChildBool(nodeRoot, "enableDiversionInhibitor", m_mobilityInfo.bEnableDiversionInhibitor);
    PutXmlChildBool(nodeRoot, "requireAnswerConfirmation", m_mobilityInfo.bRequireAnswerConfirmation);
    PutXmlChildBool(nodeRoot, "broadworksCallControl", m_mobilityInfo.bBroadworksCallControl);

    doc.save_file(strFilePath.c_str());
    return true;
}

//保存服务器返回的结果
void CMobilityProcessor::SaveMobilityInfo(const SMobility & mobilityInfo, int nLineID)
{
    m_mapMobilityInfo[nLineID] = mobilityInfo;
}
