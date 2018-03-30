#include "groupnightforwardprocess.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/xsiactionhelper/include/xsiaction.h"
#include "dsklog/log.h"

#define GNF_SERVICE_NAME "GroupNightForwarding"


CGNFProcessor * CGNFProcessor::GetInstance()
{
    static CGNFProcessor s_Inst;
    return &s_Inst;
}

CGNFProcessor::CGNFProcessor(void)
{
    XsiAction::InsertMap(XSI_GROUPNIGHT_FORWARD_GET, "services/GroupNightForwarding");
}

CGNFProcessor::~CGNFProcessor(void)
{

}

//获取服务器上的Group Night Forward信息
bool  CGNFProcessor::QueryGNF(int nLineID)
{
    if (SetLineID(nLineID))
    {
        return ExeXsiAction(nLineID, XSI_GROUPNIGHT_FORWARD_GET) != NULL;
    }
    return false;
}

//获取上次从服务器请求的Group Night Forward信息
void  CGNFProcessor::GetGNFInfo(SGroupNightForward & gnfInfo, int nLineID)
{
    YLHashMap<int, SGroupNightForward>::iterator iter = m_mapGnfInfo.find(nLineID);
    if (m_mapGnfInfo.end() != iter)
    {
        gnfInfo = iter->second;
    }
}

//保存服务器返回的结果
void CGNFProcessor::SaveGNFInfo(const SGroupNightForward & gnfInfo, int nLineID)
{
    m_mapGnfInfo[nLineID] = gnfInfo;
}

//override
bool CGNFProcessor::ProcessByType(XsiAction * pAction)
{
    bool bSucc = true;
    if (NULL != pAction)
    {
        switch (pAction->GetXsiType())
        {
        case XSI_GROUPNIGHT_FORWARD_GET:
        {
            bSucc = ParseReponseCmdFromFile(pAction->GetFilePath().c_str());
            SaveGNFInfo(m_gnfInfo, pAction->GetLineId());
        }
        break;
        default:
            break;
        }
    }
    return bSucc;
}

//解析XML数据
bool CGNFProcessor::ParseReponseCmdFromFile(const yl::string & strFilePath)
{
    xml_document doc;
    xml_parse_result objResult = doc.load_file(strFilePath.c_str());
    if (objResult.status != status_ok)
    {
        return false;
    }

    m_gnfInfo.Reset();
    //获取根结点
    xml_node nodeRoot = doc.child(GNF_SERVICE_NAME);
    if (nodeRoot.empty())
    {
        CALLCONTROL_INFO("ParseReponseCmdFromFile ReponseCmd %s do not match %s", nodeRoot.value(),
                         GNF_SERVICE_NAME);
        return false;
    }
    GetXmlChildText(nodeRoot, "userNightForwardingMode", m_gnfInfo.strUserNightFwdMode);
    //获取需要的子节点
    xml_node nodeHoliday = nodeRoot.child("holidaySchedule");
    if (!nodeHoliday.empty())
    {
        GetXmlChildText(nodeHoliday, "level", m_gnfInfo.strHolidayScheduleLevel);
        GetXmlChildText(nodeHoliday, "name", m_gnfInfo.strHolidayScheduleName);
    }
    xml_node nodeBusiness = nodeRoot.child("businessHours");
    if (!nodeBusiness.empty())
    {
        GetXmlChildText(nodeBusiness, "level", m_gnfInfo.strBusinessHoursLevel);
        GetXmlChildText(nodeBusiness, "name", m_gnfInfo.strBusinessHoursName);
    }
    return true;
}

