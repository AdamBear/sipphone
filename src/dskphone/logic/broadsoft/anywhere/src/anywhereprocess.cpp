#include "anywhereprocess.h"
#include "broadsoft/xsiactionhelper/include/xsiaction.h"
#include "xmlparser/xmlparser.hpp"
#include "dsklog/log.h"

CAnywherePrcocesser * CAnywherePrcocesser::GetInstance()
{
    static CAnywherePrcocesser s_Inst;

    return &s_Inst;
}

CAnywherePrcocesser::CAnywherePrcocesser()
{
    XsiAction::InsertMap(XSI_ANYWHERE_GET_LOCATION_LIST, "services/BroadWorksAnywhere");
    XsiAction::InsertMap(XSI_ANYWHERE_DELETE_LOCATION, "services/BroadWorksAnywhere/Location/");
    XsiAction::InsertMap(XSI_ANYWHERE_ADD_LOCATION, "services/BroadWorksAnywhere/Location/");
    XsiAction::InsertMap(XSI_ANYWHERE_MODIFY_LOCATION, "services/BroadWorksAnywhere/Location/");
    XsiAction::InsertMap(XSI_ANYWHERE_MODIFY_LOCATIONS, "services/BroadWorksAnywhere/Location/");
    XsiAction::InsertMap(XSI_ANYWHERE_GET_LOCATION_DETAIL, "services/BroadWorksAnywhere/Location/");
}

CAnywherePrcocesser::~CAnywherePrcocesser()
{
}

bool CAnywherePrcocesser::RequestLocationList()
{
    m_loactArry.clear();

    return ExeXsiAction(m_nLineID, XSI_ANYWHERE_GET_LOCATION_LIST) != NULL;
}

bool CAnywherePrcocesser::RequestDeleteLocation(const yl::string & strPhoneNumber)
{
    return ExeXsiAction(m_nLineID, XSI_ANYWHERE_DELETE_LOCATION, strPhoneNumber) != NULL;
}

bool CAnywherePrcocesser::RequestAddLocation(const SAYWLocation & sLoct, bool bCreatorbyModify /*= false*/)
{
    yl::string strFile = XsiAction::GetRandomFile();
    if (SaveDataToFile(sLoct, XSI_ANYWHERE_ADD_LOCATION, strFile, bCreatorbyModify))
    {
        return ExeXsiAction(m_nLineID, XSI_ANYWHERE_ADD_LOCATION, sLoct.m_strPhoneNum, strFile) != NULL;
    }
    return false;
}

bool CAnywherePrcocesser::RequestModifyLocation(const yl::string & strOriginalPhoneNumber,
        const SAYWLocation & sLoct, eXsiActionType eType/* = XSI_ANYWHERE_MODIFY_LOCATION*/)
{
    yl::string strFile = XsiAction::GetRandomFile();
    if (SaveDataToFile(sLoct, eType, strFile))
    {
        XsiAction * pAction = ExeXsiAction(m_nLineID, eType, strOriginalPhoneNumber, strFile);
        if (pAction != NULL)
        {
            pAction->SetChangeData(sLoct.m_bActive);
            return true;
        }
    }
    return false;
}

bool CAnywherePrcocesser::RequestLocationDetail(const yl::string & strPhoneNumber)
{
    m_locatDetail.clear();

    return ExeXsiAction(m_nLineID,
                        XSI_ANYWHERE_GET_LOCATION_DETAIL, strPhoneNumber) != NULL;
}

bool CAnywherePrcocesser::RequestModifyMutilLocations(CStringVector & strOriginalPhoneNumberList,
        SAYWLocationArray & arry)
{
    bool suc = false;
    for (int i = 0, j = 0;
            i < arry.size() && j < strOriginalPhoneNumberList.size(); ++i, ++j)
    {
        //不一样才进行更改
        if (arry.m_locatArr.at(i).m_bActive != m_loactArry.m_locatArr.at(i).m_bActive)
        {
            suc |= RequestModifyLocation(strOriginalPhoneNumberList[j], arry.m_locatArr.at(i),
                                         XSI_ANYWHERE_MODIFY_LOCATIONS);

            etlSleep(2);
        }
    }

    return suc;
}

bool CAnywherePrcocesser::RequestDeleteAllLocation()
{
    bool suc = false;
    for (int i = 0; i < m_loactArry.size(); ++i)
    {
        suc |= RequestDeleteLocation(m_loactArry.m_locatArr.at(i).m_strPhoneNum);

        etlSleep(20);
    }

    return suc;
}


bool CAnywherePrcocesser::ProcessByType(XsiAction * pAction)
{
    if (pAction == NULL)
    {
        return false;
    }
    bool suc = true;
    switch (pAction->GetXsiType())
    {
    case XSI_ANYWHERE_GET_LOCATION_LIST:
    {
        suc = PraseLocationList(pAction->GetFilePath());
    }
    break;

    case XSI_ANYWHERE_GET_LOCATION_DETAIL:
    {
        suc = PraseLocationDetail(pAction->GetFilePath());
    }
    break;

    case XSI_ANYWHERE_MODIFY_LOCATIONS:
    case XSI_ANYWHERE_MODIFY_LOCATION:
    {
        SAYWLocation * pLocation = GetLocationByNumber(pAction->GetExtenUrl());

        if (pLocation != NULL)
        {
            pLocation->m_bActive = pAction->GetChangeData();
        }
        else
        {
            suc = false;
        }
    }
    break;
    default:
        break;
    }

    return suc;
}

void CAnywherePrcocesser::GetAllLocationsQueryResultList(SAYWLocationArray & arry)
{
    arry = m_loactArry;
}

bool CAnywherePrcocesser::GetLocationDetailQueryResult(SAYWLocation & sLoct) const
{
    sLoct = m_locatDetail;
    return !sLoct.m_strPhoneNum.empty();
}

int CAnywherePrcocesser::GetCachedLocationCount()
{
    return m_loactArry.m_locatArr.size();
}

//判断号码是否已存在
bool CAnywherePrcocesser::CheckNumberExit(const yl::string & szNumber)
{
    return GetLocationByNumber(szNumber) != NULL;
}

SAYWLocation * CAnywherePrcocesser::GetLocationByNumber(const yl::string & strNumber)
{
    ANYWHERE_INFO("CAnywherePrcocesser::GetLocationByNumber number[%s],m_loactArry size[%d]",
                  strNumber.c_str(), m_loactArry.size());
    for (int i = 0; i < m_loactArry.size(); i++)
    {
        if (m_loactArry.m_locatArr.at(i).m_strPhoneNum == strNumber)
        {
            ANYWHERE_INFO("----------------------------------------");
            return &(m_loactArry.m_locatArr.at(i));
        }
    }

    return NULL;
}


bool CAnywherePrcocesser::PraseLocationList(const yl::string & strFile)
{
    xml_document doc;

    if (!doc.load_file(strFile.c_str()))
    {
        return false;
    }

    yl::string tmpVal;

    m_loactArry.clear();

    xml_node nodeRoot = doc.child(Elem_Name_BroadWorksAnywhere);
    if (nodeRoot.empty())
    {
        return false;
    }

    xml_node aalfctdcElem = nodeRoot.child(Elem_Name_AlertAllLocationsForClickToDialCalls);
    if (!aalfctdcElem.empty())
    {
        tmpVal = aalfctdcElem.text().get();
        m_loactArry.m_bAlertAllLocatForClickToDialCalls = (tmpVal == "true");
    }

    //无节点表示无Location信息
    xml_node loctionsElem = nodeRoot.child(Elem_Name_Locations);
    if (loctionsElem.empty())
    {
        return true;
    }

    xml_node loctElem = loctionsElem.child(Elem_Name_Location);

    SAYWLocation tmpLoct;
    while (!loctElem.empty())
    {
        tmpLoct.clear();

        //locationUri
        tmpLoct.m_strLocationUri = loctElem.child(Elem_Name_LocationUri).text().get();

        //phoneNumber
        tmpLoct.m_strPhoneNum = loctElem.child(Elem_Name_PhoneNumber).text().get();

        //description
        tmpLoct.m_strDescription = loctElem.child(Elem_Name_Description).text().get();

        //action
        tmpVal = loctElem.child(Elem_Name_Active).text().get();
        tmpLoct.m_bActive = (tmpVal == "true");

        m_loactArry.m_locatArr.push_back(tmpLoct);

        loctElem = loctElem.next_sibling();
    }
    ANYWHERE_INFO("m_arry.m_locatArr Size[%d]", m_loactArry.m_locatArr.size());

    return true;
}

bool CAnywherePrcocesser::PraseLocationDetail(const yl::string & strFile)
{
    xml_document doc;

    if (!doc.load_file(strFile.c_str()))
    {
        return false;
    }

    m_locatDetail.clear();
    xml_node nodeRoot = doc.child(Elem_Name_BroadWorksAnywhereLocation);

    if (nodeRoot.empty())
    {
        return false;
    }

    yl::string tmpVal;

    //phoneNumber
    m_locatDetail.m_strPhoneNum = nodeRoot.child(Elem_Name_PhoneNumber).text().get();

    //description
    m_locatDetail.m_strDescription = nodeRoot.child(Elem_Name_Description).text().get();

    //active
    tmpVal = nodeRoot.child(Elem_Name_Active).text().get();
    m_locatDetail.m_bActive = (tmpVal == "true");

    //outbound number
    m_locatDetail.m_strOutBoundPhoneNum = nodeRoot.child(
            Elem_Name_OutboundAlternateNumber).text().get();

    //broadworksCallControl
    tmpVal = nodeRoot.child(Elem_Name_BroadworksCallControl).text().get();
    m_locatDetail.m_bUseBbasedCallControlServices = (tmpVal == "true");

    //useDiversionInhibitor
    tmpVal = nodeRoot.child(Elem_Name_UseDiversionInhibitor).text().get();
    m_locatDetail.m_bEnableDiversionInhibitor = (tmpVal == "true");

    //answerConfirmationRequired
    tmpVal = nodeRoot.child(Elem_Name_AnswerConfirmationRequired).text().get();
    m_locatDetail.m_bRequireAnswerConfirmation = (tmpVal == "true");

    return true;
}

bool CAnywherePrcocesser::SaveDataToFile(const SAYWLocation & sLoct, eXsiActionType eType,
        const yl::string & strFileName, bool bCreatorbyModify /*= false*/)
{
    xml_document doc;

    // 添加声明结点
    xml_node nodeDecl = doc.append_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "UTF-8");

    // 根节点.
    xml_node nodeRoot  = doc.append_child(node_element);
    nodeRoot.set_name(Elem_Name_BroadWorksAnywhereLocation);
    nodeRoot.append_attribute(Attrib_Name_xmlns, Attrib_Value_xmlns);

    //phonenumber
    xml_node nodeElem = nodeRoot.append_child(node_element);
    nodeElem.set_name(Elem_Name_PhoneNumber);
    nodeElem.append_child(node_pcdata).set_value(sLoct.m_strPhoneNum.c_str());

    //description
    if (!sLoct.m_strDescription.empty())
    {
        nodeElem = nodeRoot.append_child(node_element);
        nodeElem.set_name(Elem_Name_Description);
        nodeElem.append_child(node_pcdata).set_value(sLoct.m_strDescription.c_str());
    }
    else if (XSI_ANYWHERE_MODIFY_LOCATION == eType || XSI_ANYWHERE_MODIFY_LOCATIONS == eType)
    {
        nodeElem = nodeRoot.append_child(node_element);
        nodeElem.set_name(Elem_Name_Description);
        nodeElem.append_attribute("xs:nil", "true");
        nodeElem.append_attribute("xmlns:xs", "http://www.w3.org/2001/XMLSchema-instance");
    }

    //active
    nodeElem = nodeRoot.append_child(node_element);
    nodeElem.set_name(Elem_Name_Active);
    nodeElem.append_child(node_pcdata).set_value(sLoct.m_bActive ? "true" : "false");

    if (XSI_ANYWHERE_MODIFY_LOCATION == eType || bCreatorbyModify)
    {
        //OutboundAlternateNumber
        if (!sLoct.m_strOutBoundPhoneNum.empty())
        {
            nodeElem = nodeRoot.append_child(node_element);
            nodeElem.set_name(Elem_Name_OutboundAlternateNumber);
            nodeElem.append_child(node_pcdata).set_value(sLoct.m_strOutBoundPhoneNum.c_str());
        }
        else if (!bCreatorbyModify)
        {
            nodeElem = nodeRoot.append_child(node_element);
            nodeElem.set_name(Elem_Name_OutboundAlternateNumber);
            nodeElem.append_attribute("xs:nil", "true");
            nodeElem.append_attribute("xmlns:xs", "http://www.w3.org/2001/XMLSchema-instance");
        }

        //broadworksCallControl
        nodeElem = nodeRoot.append_child(node_element);
        nodeElem.set_name(Elem_Name_BroadworksCallControl);
        nodeElem.append_child(node_pcdata).set_value(sLoct.m_bUseBbasedCallControlServices ? "true" :
                "false");

        //useDiversionInhibitor
        nodeElem = nodeRoot.append_child(node_element);
        nodeElem.set_name(Elem_Name_UseDiversionInhibitor);
        nodeElem.append_child(node_pcdata).set_value(sLoct.m_bEnableDiversionInhibitor ? "true" : "false");

        //answerConfirmationRequired
        nodeElem = nodeRoot.append_child(node_element);
        nodeElem.set_name(Elem_Name_AnswerConfirmationRequired);
        nodeElem.append_child(node_pcdata).set_value(sLoct.m_bRequireAnswerConfirmation ? "true" : "false");
    }

    return doc.save_file(strFileName.c_str());
}

