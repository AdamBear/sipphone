#include "forwardselectiveprocess.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/xsiactionhelper/include/xsiaction.h"

#ifdef IF_FEATURE_BROADSOFT_CALL_FORWARD_SELECTIVE

CForwardSelectiveProcesser & CForwardSelectiveProcesser::GetInstance()
{
    static CForwardSelectiveProcesser s_ForwardSelective;
    return s_ForwardSelective;
}

CForwardSelectiveProcesser::CForwardSelectiveProcesser(void)
{
    XsiAction::InsertMap(XSI_FORWARD_SELECTIVE_GET, "services/CallForwardingSelective");
    XsiAction::InsertMap(XSI_FORWARD_SELECTIVE_PUT, "services/CallForwardingSelective");
}

CForwardSelectiveProcesser::~CForwardSelectiveProcesser(void)
{
}

bool CForwardSelectiveProcesser::ParseReponseCmdFromFile(const yl::string & strFilePath)
{
    xml_document doc;
    xml_parse_result objResult = doc.load_file(strFilePath.c_str());
    if (objResult.status == status_ok)
    {
        //获取根结点
        xml_node nodeRoot = doc.child("CallForwardingSelective");
        if (nodeRoot.empty())
        {
            return false;
        }

        m_FSItem.clear();

        GetXmlChildText(nodeRoot, "active", m_FSItem.bActive);
        GetXmlChildText(nodeRoot, "defaultForwardToPhoneNumber", m_FSItem.strForwardToPhoneNumber);
        GetXmlChildText(nodeRoot, "playRingReminder", m_FSItem.bPlayRingReminder);

        xml_node nodeCriterias = nodeRoot.child("criteriaActivations");
        if (nodeCriterias.empty())
        {
            return false;
        }

        xml_node nodeItemCriteria = nodeCriterias.child("criteriaActivation");
        while (!nodeItemCriteria.empty())
        {
            SForwardSelectiveDataCriteriaItem sItem;
            GetXmlChildText(nodeItemCriteria, "criteriaName", sItem.strName);
            GetXmlChildText(nodeItemCriteria, "active", sItem.bActive);
            GetXmlChildText(nodeItemCriteria, "uri", sItem.strURI);
            m_FSItem.vecCriteria.push_back(sItem);

            nodeItemCriteria = nodeItemCriteria.next_sibling("criteriaActivation");
        }

        return true;
    }

    return false;
}

bool CForwardSelectiveProcesser::SaveRequestCmdToFile(const yl::string & strFilePath)
{
    xml_document doc;
    // 添加声明结点
    xml_node nodeDecl = doc.append_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "ISO-8859-1");

    //增加根结点.
    xml_node nodeRoot  = doc.append_child(node_element);

    nodeRoot.set_name("CallForwardingSelective");
    nodeRoot.append_attribute("xmlns", "http://schema.broadsoft.com/xsi");

    //增加Active节点
    PutXmlChildBool(nodeRoot, "active", m_FSItem.bActive);
    PutXmlChildText(nodeRoot, "defaultForwardToPhoneNumber", m_FSItem.strForwardToPhoneNumber);
    PutXmlChildBool(nodeRoot, "playRingReminder", m_FSItem.bPlayRingReminder);

    xml_node nodeCriterias = nodeRoot.append_child("criteriaActivations");

    typedef SForwardSelectiveData::VecCriteriaItem VecItem;
    VecItem & vecItem = m_FSItem.vecCriteria;
    for (VecItem::iterator it = vecItem.begin(); it != vecItem.end(); ++it)
    {
        xml_node nodeItemCriteria = nodeCriterias.append_child("criteriaActivation");
        const SForwardSelectiveDataCriteriaItem & sItem = *it;

        PutXmlChildText(nodeItemCriteria, "criteriaName", sItem.strName);
        PutXmlChildBool(nodeItemCriteria, "active", sItem.bActive);
        PutXmlChildText(nodeItemCriteria, "uri", sItem.strURI);
    }

    return doc.save_file(strFilePath.c_str());
}

bool CForwardSelectiveProcesser::ProcessByType(XsiAction * pAction)
{
    if (pAction == NULL)
    {
        return false;
    }

    bool suc = true;
    int iLineId = pAction->GetLineId();

    switch (pAction->GetXsiType())
    {
    case XSI_FORWARD_SELECTIVE_GET:
        {
            if (ParseReponseCmdFromFile(pAction->GetFilePath()))
            {
                m_mapForwardSelective[iLineId] = m_FSItem;
            }
        }
        break;
    case XSI_FORWARD_SELECTIVE_PUT:
        {
            m_mapForwardSelective[iLineId] = m_FSItem;
        }
        break;
    default:
        suc = false;
        break;
    }

    return suc;
}

bool CForwardSelectiveProcesser::SetForwardSelective(const SForwardSelectiveData & sData,
        int nLineID)
{
    if (SetLineID(nLineID))
    {
        m_FSItem = sData;
        yl::string strFile = XsiAction::GetRandomFile();
        if (SaveRequestCmdToFile(strFile))
        {
            return ExeXsiAction(nLineID, XSI_FORWARD_SELECTIVE_PUT, "", strFile) != NULL;
        }
    }

    return false;
}

bool CForwardSelectiveProcesser::GetForwardSelective(SForwardSelectiveData & sData, int nLineID)
{
    mapFSData::iterator iter = m_mapForwardSelective.find(nLineID);
    if (iter != m_mapForwardSelective.end())
    {
        sData = iter->second;
        return true;
    }

    return false;
}

bool CForwardSelectiveProcesser::QueryForwardSelective(int nLineID)
{
    return SetLineID(nLineID) && ExeXsiAction(nLineID, XSI_FORWARD_SELECTIVE_GET) != NULL;
}
#endif
