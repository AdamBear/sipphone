#include "simultaneousringprocess.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/xsiactionhelper/include/xsiaction.h"
#include "dsklog/log.h"

#define SRP_SERVICE_NAME "SimultaneousRingPersonal"

CSimultaneousRingProcessor & CSimultaneousRingProcessor::GetInstance()
{
    static CSimultaneousRingProcessor s_Inst;
    return s_Inst;
}

//设置Simultaneous Ring Personal
bool CSimultaneousRingProcessor::SetSRP(const SSimultaneousRing & simRingInfo)
{
    yl::string strFile = XsiAction::GetRandomFile();

    m_simRingInfo = simRingInfo;

    if (SaveRequestCmdToFile(strFile.c_str()))
    {
        return ExeXsiAction(m_nLineID, XSI_SIMULTANEOUSRING_PUT, "", strFile) != NULL;
    }
    return false;
}

//获取服务器上的Simultaneous Ring Personal信息，返回的结果是通过发送
bool CSimultaneousRingProcessor::QuerySRP(int nLineID)
{
    if (SetLineID(nLineID))
    {
        return ExeXsiAction(nLineID, XSI_SIMULTANEOUSRING_GET) != NULL;
    }
    return false;
}

//获取上次从服务器得到的Simultaneous Ring Personal信息
bool CSimultaneousRingProcessor::GetSRPInfo(SSimultaneousRing & simRingInfo)
{
    YLHashMap<int, SSimultaneousRing>::iterator iter = m_mapSimRing.find(m_nLineID);
    if (iter != m_mapSimRing.end())
    {
        simRingInfo = iter->second;
        return true;
    }
    return false;
}

//检查号码在Simultaneous Ring Personal里是否已经存在
bool CSimultaneousRingProcessor::CheckNumberExit(const char * szNumber)
{
    if (NULL == szNumber || 0 == strlen(szNumber))
    {
        return false;
    }

    if (m_mapSimRing.find(m_nLineID) != m_mapSimRing.end())
    {
        for (int i = 0; i < m_mapSimRing[m_nLineID].simRingLocationArry.size(); i++)
        {
            if (0 == strcmp(szNumber, m_mapSimRing[m_nLineID].simRingLocationArry.at(i).strAddress.c_str()))
            {
                return true;
            }
        }
    }

    return false;
}

CSimultaneousRingProcessor::CSimultaneousRingProcessor(void)
{
    m_simRingInfo.Reset();
    XsiAction::InsertMap(XSI_SIMULTANEOUSRING_GET, "services/SimultaneousRingPersonal");
    XsiAction::InsertMap(XSI_SIMULTANEOUSRING_PUT, "services/SimultaneousRingPersonal");
}

CSimultaneousRingProcessor::~CSimultaneousRingProcessor(void)
{
}

//override
bool CSimultaneousRingProcessor::ProcessByType(XsiAction * pAction)
{
    bool bSucc = true;
    if (NULL != pAction)
    {
        switch (pAction->GetXsiType())
        {
        case XSI_SIMULTANEOUSRING_GET:
        {
            //服务器返回号码列表时，先清除本地保存的数据
            m_simRingInfo.Reset();
            bSucc = ParseReponseCmdFromFile(pAction->GetFilePath().c_str());
            SaveSRPInfo(m_simRingInfo, pAction->GetLineId());
        }
        break;
        case XSI_SIMULTANEOUSRING_PUT:
        {
            SaveSRPInfo(m_simRingInfo, pAction->GetLineId());
        }
        break;
        default:
            break;
        }
    }

    return bSucc;
}

//解析
bool CSimultaneousRingProcessor::ParseReponseCmdFromFile(const yl::string & strFilePath)
{
    xml_document doc;
    xml_parse_result objResult = doc.load_file(strFilePath.c_str());
    if (objResult.status == status_ok)
    {
        //获取根结点
        xml_node nodeRoot = doc.child(SRP_SERVICE_NAME);
        if (nodeRoot.empty())
        {
            CALLCONTROL_INFO("ParseReponseCmdFromFile ReponseCmd %s do not match %s", nodeRoot.value(),
                             SRP_SERVICE_NAME);
            return false;
        }

        LPCSTR lpTemp = NULL;

        //获取active结点
        if (!GetXmlChildText(nodeRoot, "active", m_simRingInfo.bActive))
        {
            return false;
        }

        yl::string strTemp;

        //获取incomingCalls结点
        GetXmlChildText(nodeRoot, "incomingCalls", strTemp);
        m_simRingInfo.bDoNotRingOnACall = (strcmp(strTemp.c_str(), "Do not Ring if on a Call") == 0);

        //获取simRingLocations，并解析Simultaneous Ring Personal列表
        xml_node nodeElem = nodeRoot.child("simRingLocations");

        ParseSimRingLocation(nodeElem);

        return true;
    }

    return false;
}

//解析Location list
bool CSimultaneousRingProcessor::ParseSimRingLocation(const xml_node & nodeLocations)
{
    // 服务器不推送列表的情况下 清空
    m_simRingInfo.simRingLocationArry.clear();

    if (nodeLocations.empty())
    {
        return false;
    }

    for (xml_node nodeLocation = nodeLocations.child("simRingLocation"); !nodeLocation.empty();
            nodeLocation = nodeLocation.next_sibling("simRingLocation"))
    {
        SSimRingLocation simRingLocation;

        GetXmlChildText(nodeLocation, "address", simRingLocation.strAddress);

        GetXmlChildText(nodeLocation, "answerConfirmationRequired",
                        simRingLocation.bAnswerConfirmationRequired);

        //保存到链表中
        m_simRingInfo.simRingLocationArry.push_back(simRingLocation);
    }

    return true;
}

//保存cmd到文件
bool CSimultaneousRingProcessor::SaveRequestCmdToFile(const yl::string & strFilePath)
{
    if (strFilePath.empty())
    {
        return false;
    }
    //增加Xml头
    xml_document doc;
    xml_node nodeDecl = doc.append_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "ISO-8859-1");

    //增加根结点
    xml_node nodeRoot = doc.append_child(SRP_SERVICE_NAME);

    // 设置属性
    nodeRoot.append_attribute("xmlns", "http://schema.broadsoft.com/xsi");


    //增加Active节点
    PutXmlChildBool(nodeRoot, "active", m_simRingInfo.bActive
                    && m_simRingInfo.simRingLocationArry.size() > 0);

    //增加incomingCalls节点
    PutXmlChildText(nodeRoot, "incomingCalls",
                    m_simRingInfo.bDoNotRingOnACall ? "Do not Ring if on a Call" : "Ring for all Incoming Calls");

    //增加simRingLocations节点
    xml_node nodeSimRingLocations = nodeRoot.append_child("simRingLocations");

    if (m_simRingInfo.simRingLocationArry.size() == 0)
    {
        //如果没有数据，删除所有地节点
        nodeSimRingLocations.append_attribute("xs:nil", "true");
        nodeSimRingLocations.append_attribute("xmlns:xs", "http://www.w3.org/2001/XMLSchema-instance");
    }
    else
    {
        //增加Location列表
        for (int i = 0; i < m_simRingInfo.simRingLocationArry.size(); i++)
        {
            if (m_simRingInfo.simRingLocationArry.at(i).strAddress.empty())
            {
                continue;
            }

            //增加simRingLocation节点
            xml_node nodeLocation = nodeSimRingLocations.append_child("simRingLocation");

            //增加address节点,设置address内容
            PutXmlChildText(nodeLocation, "address",
                            m_simRingInfo.simRingLocationArry.at(i).strAddress.c_str());

            //增加answerConfirmationRequired节点
            PutXmlChildBool(nodeLocation, "answerConfirmationRequired",
                            m_simRingInfo.simRingLocationArry.at(i).bAnswerConfirmationRequired);
        }
    }

    doc.save_file(strFilePath.c_str());
    return true;
}

//保存服务器返回的结果
void CSimultaneousRingProcessor::SaveSRPInfo(const SSimultaneousRing & simRingInfo, int nLineID)
{
    m_mapSimRing[nLineID] = simRingInfo;
}
