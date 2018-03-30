#include "bwcallcontrol_inc.h"

//匿名呼叫服务名
const char * kszBroadsoftACServiceName = "CallingLineIDDeliveryBlocking";
//匿名呼叫拒绝服务名
const char * kszBroadsoftACRServiceName = "AnonymousCallRejection";

CLIDProcesser & CLIDProcesser::GetInstance()
{
    static CLIDProcesser s_Inst;

    return s_Inst;
}

CLIDProcesser::CLIDProcesser()
{
    XsiAction::InsertMap(XSI_ANONYOUSCALL_GET_STATE, "services/CallingLineIDDeliveryBlocking");
    XsiAction::InsertMap(XSI_ANONYMOUNSCALL_REJECT_GET_STATE, "services/AnonymousCallRejection");

    XsiAction::InsertMap(XSI_ANONYOUSCALL_PUT_STATE, "services/CallingLineIDDeliveryBlocking");
    XsiAction::InsertMap(XSI_ANONYMOUNSCALL_REJECT_PUT_STATE, "services/AnonymousCallRejection");
}

CLIDProcesser::~CLIDProcesser()
{
}

bool CLIDProcesser::SetAnonymousCallState(bool bAnonymousCallState, int nLineID)
{
    return SetCallState(XSI_ANONYOUSCALL_PUT_STATE, bAnonymousCallState, nLineID);
}

bool CLIDProcesser::SetAnonymousCallRejectionState(bool bAnonymousCallRejectionState, int nLineID)
{
    return SetCallState(XSI_ANONYMOUNSCALL_REJECT_PUT_STATE, bAnonymousCallRejectionState, nLineID);
}

bool CLIDProcesser::QueryAnonymousCallState(int nLineID)
{
    if (!SetLineID(nLineID))
    {
        return false;
    }

    return ExeXsiAction(nLineID, XSI_ANONYOUSCALL_GET_STATE) != NULL;
}

bool CLIDProcesser::QueryAnonymousCallRejectionState(int nLineID)
{
    if (!SetLineID(nLineID))
    {
        return false;
    }

    return ExeXsiAction(nLineID, XSI_ANONYMOUNSCALL_REJECT_GET_STATE) != NULL;
}

bool CLIDProcesser::GetAnonymousCallState(int nLineID)
{
    return m_mapAnonymousCall[nLineID];
}

bool CLIDProcesser::GetAnonymousCallRejectionState(int nLineID)
{
    return m_mapAnonymousCallRejection[nLineID];
}

bool CLIDProcesser::SetCallState(eXsiActionType eType, bool bActive, int nLineID)
{
    if (!SetLineID(nLineID))
    {
        return false;
    }
    yl::string strFile = XsiAction::GetRandomFile();
    if (SaveDataToFile(bActive, strFile, eType))
    {
        XsiAction * pAction = ExeXsiAction(m_nLineID, eType, "", strFile);
        if (pAction != NULL)
        {
            pAction->SetChangeData(bActive);
            return true;
        }
    }
    return false;
}

bool CLIDProcesser::ProcessByType(XsiAction * pAction)
{
    if (pAction == NULL)
    {
        return false;
    }
    bool suc = true;
    switch (pAction->GetXsiType())
    {
    case XSI_ANONYOUSCALL_PUT_STATE:
        {
            m_mapAnonymousCall[pAction->GetLineId()] = pAction->GetChangeData();
        }
        break;

    case XSI_ANONYMOUNSCALL_REJECT_PUT_STATE:
        {
            m_mapAnonymousCallRejection[pAction->GetLineId()] = pAction->GetChangeData();
        }
        break;

    case XSI_ANONYMOUNSCALL_REJECT_GET_STATE:
    case XSI_ANONYOUSCALL_GET_STATE:
        {
            suc = ParseReponseCmdFromFile(pAction->GetFilePath());

            if (suc)
            {
                if (pAction->GetXsiType() == XSI_ANONYOUSCALL_GET_STATE)
                {
                    m_mapAnonymousCall[pAction->GetLineId()] = m_bTempState;
                }
                else
                {
                    m_mapAnonymousCallRejection[pAction->GetLineId()] = m_bTempState;
                }
            }
        }
        break;
    default:
        break;
    }

    return suc;
}

bool CLIDProcesser::SaveDataToFile(bool bActive, const yl::string & strFilePath,
                                   eXsiActionType eType)
{
    if (eType != XSI_ANONYMOUNSCALL_REJECT_PUT_STATE && eType != XSI_ANONYOUSCALL_PUT_STATE)
    {
        return false;
    }

    xml_document doc;

    // 添加声明结点
    xml_node nodeDecl = doc.append_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "ISO-8859-1");

    //增加根结点.
    xml_node nodeRoot  = doc.append_child(node_element);

    nodeRoot.set_name(XSI_ANONYMOUNSCALL_REJECT_PUT_STATE == eType ? kszBroadsoftACRServiceName :
                      kszBroadsoftACServiceName);
    nodeRoot.append_attribute("xmlns", "http://schema.broadsoft.com/xsi");

    //增加Active节点
    PutXmlChildBool(nodeRoot, "active", bActive);

    doc.save_file(strFilePath.c_str());

    return true;
}

bool CLIDProcesser::ParseReponseCmdFromFile(const yl::string & strFilePath)
{
    xml_document doc;
    xml_parse_result objResult = doc.load_file(strFilePath.c_str());
    if (objResult.status == status_ok)
    {
        //获取根结点
        xml_node nodeRoot = doc.first_child();
        if (nodeRoot.empty())
        {
            return false;
        }

        return GetXmlChildText(nodeRoot, "active", m_bTempState);
    }

    return false;
}

