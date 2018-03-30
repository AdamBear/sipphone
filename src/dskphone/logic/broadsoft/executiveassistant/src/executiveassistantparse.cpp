#include "exedata_inc.h"

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
bool CExecutiveAssistantParse::ParseAssistantList(const xml_node & node, ExeAssisData * pData)
{
    if (node.empty() || pData == NULL || pData->IsInvalid())
    {
        return false;
    }

    CExecutive_Data & exeData = pData->tExeData;
    GetXmlChildText(node, "allowOptInOut", exeData.tCommonData.bAllowOpt);

    exeData.tCommonData.listData.clear();
    xml_node userRootNode = node.child("assignedUsers");
    if (!userRootNode.empty())
    {
        xml_node childNode = userRootNode.child("userDetails");
        int iPriority = 0;
        while (!childNode.empty())
        {
            CUserItem AssistantItem;
            GetXmlChildText(childNode, "userId", AssistantItem.ItemName.strUserId);
            GetXmlChildText(childNode, "optIn", AssistantItem.bOptIn);
            // 权值递增
            AssistantItem.iPriority = iPriority++;

            if (AssistantItem)
            {
                exeData.tCommonData.listData.push_back(AssistantItem);
            }

            childNode = childNode.next_sibling("userDetails");
        }
    }

    XSI_INFO("ParseAssistantList size=[%d]", exeData.tCommonData.listData.size());
    return true;
}

bool CExecutiveAssistantParse::ParseFiltering(const xml_node & node, ExeAssisData * pData)
{
    if (node.empty() || pData == NULL || pData->IsInvalid())
    {
        return false;
    }

    CExeFilterInfo & exeData = pData->tExeData.tFilter;
    if (!GetXmlChildText(node, "allowOptInOut", exeData.bSwitchOn))
    {
        GetXmlChildText(node, "enableFiltering", exeData.bSwitchOn);
    }

    yl::string strParseResult;
    GetXmlChildText(node, "filteringMode", strParseResult);
    exeData.SetFilterMode(strParseResult);

    GetXmlChildText(node, "simpleFilterType", strParseResult);
    exeData.SetFilterType(strParseResult);

    exeData.tCriteriaList.clear();
    xml_node nodeCriteriaRoot = node.child("criteriaActivations");
    if (!nodeCriteriaRoot.empty())
    {
        xml_node childNode = nodeCriteriaRoot.child("criteriaActivation");
        while (!childNode.empty())
        {
            CExeFilterInfo::CCriteria CriteriaItem;

            GetXmlChildText(childNode, "criteriaName", CriteriaItem.strName);
            GetXmlChildText(childNode, "uri", CriteriaItem.strUri);
            GetXmlChildText(childNode, "active", CriteriaItem.bActive);

            if (!CriteriaItem.strUri.empty())
            {
                exeData.tCriteriaList.push_back(CriteriaItem);
            }

            childNode = childNode.next_sibling("criteriaActivation");
        }
    }

    return true;
}

bool CExecutiveAssistantParse::ParseScreening(const xml_node & node, ExeAssisData * pData)
{
    if (node.empty() || pData == NULL || pData->IsInvalid())
    {
        return false;
    }

    CExeScreenInfo & exeData = pData->tExeData.tScreen;
    GetXmlChildText(node, "enableScreening", exeData.bScreeningOn);

    yl::string strParseResult;
    GetXmlChildText(node, "screeningAlertType", strParseResult);
    exeData.SetAlterType(strParseResult);

    GetXmlChildText(node, "alertBroadWorksMobilityLocation", exeData.bAlertMobility);
    GetXmlChildText(node, "alertBroadWorksAnywhereLocations", exeData.bAlertAnywhere);
    GetXmlChildText(node, "alertSharedCallAppearanceLocations", exeData.bAlertSCA);

    return true;
}

bool CExecutiveAssistantParse::ParseAlter(const xml_node & node, ExeAssisData * pData)
{
    if (node.empty() || pData == NULL || pData->IsInvalid())
    {
        return false;
    }

    CExeAlterInfo & exeData = pData->tExeData.tAlter;
    yl::string strParseResult;
    GetXmlChildText(node, "alertingMode", strParseResult);
    exeData.SetAlterMode(strParseResult);

    GetXmlChildText(node, "alertingCallingLineIdNameMode", strParseResult);
    exeData.SetCallInfoName(strParseResult);
    exeData.strCustomCallName.clear();
    GetXmlChildText(node, "alertingCustomCallingLineIdName", exeData.strCustomCallName);
    exeData.strUnicodeCustomCallName.clear();
    GetXmlChildText(node, "unicodeAlertingCustomCallingLineIdName", exeData.strUnicodeCustomCallName);

    GetXmlChildText(node, "alertingCallingLineIdPhoneNumberMode", strParseResult);
    exeData.SetCallInfoNumber(strParseResult);
    exeData.strCustomCallNumber.clear();
    GetXmlChildText(node, "alertingCustomCallingLineIdPhoneNumber", exeData.strCustomCallNumber);

    GetXmlChildText(node, "callPushRecallNumberOfRings", strParseResult);
    exeData.iCallPushRecallAfter = atoi(strParseResult.c_str());
    GetXmlChildText(node, "nextAssistantNumberOfRings", strParseResult);
    exeData.iAdvanceToNextAssistantAfter = atoi(strParseResult.c_str());

    GetXmlChildText(node, "rolloverWaitTimeSeconds", strParseResult);
    exeData.iRolloverAfter = atoi(strParseResult.c_str());

    GetXmlChildText(node, "rolloverAction", strParseResult);
    exeData.SetRolloverAction(strParseResult);

    exeData.strRolloverTo.clear();
    GetXmlChildText(node, "rolloverForwardToPhoneNumber", exeData.strRolloverTo);

    return true;
}

bool CExecutiveAssistantParse::ParseExecutiveList(const xml_node & node, ExeAssisData * pData)
{
    if (node.empty() || pData == NULL || pData->IsInvalid())
    {
        return false;
    }

    CAssistant_Data & exeData = pData->tAssData;

    GetXmlChildText(node, "enableDivert", exeData.tCommonData.bDivertOn);
    exeData.tCommonData.strDivertNumber.clear();
    GetXmlChildText(node, "divertToPhoneNumber", exeData.tCommonData.strDivertNumber);

    xml_node userRootNode = node.child("executives");
    if (!userRootNode.empty())
    {
        xml_node childNode = userRootNode.child("executiveDetails");
        while (!childNode.empty())
        {
            CUserItem AssistantItem;
            GetXmlChildText(childNode, "executiveUserId", AssistantItem.ItemName.strUserId);
            GetXmlChildText(childNode, "executiveLastName", AssistantItem.ItemName.strLastName);
            GetXmlChildText(childNode, "executiveFirstName", AssistantItem.ItemName.strFirstName);
            GetXmlChildText(childNode, "executiveUri", AssistantItem.ItemName.strUri);

            GetXmlChildText(childNode, "optIn", AssistantItem.bOptIn);

            // warning:: 不能清空列表，因Put的时候，相对顺序不对的话也会失败。只能修改列表，在功能键同步的时候才允许清空列表
            if (AssistantItem)
            {
                ExeAssisListIt it = yl::find(exeData.tCommonData.listData.begin(),
                                             exeData.tCommonData.listData.end(), AssistantItem);
                if (it != exeData.tCommonData.listData.end())
                {
                    it->bOptIn = AssistantItem.bOptIn;
                }
            }

            childNode = childNode.next_sibling("executiveDetails");
        }
    }

    XSI_INFO("ParseExecutiveList size=[%d]", exeData.tCommonData.listData.size());
    return true;
}

bool CExecutiveAssistantParse::FillAssistantList(xml_node & node, ExeAssisData * pData)
{
    if (node.empty() || pData == NULL || pData->IsInvalid())
    {
        return false;
    }

    CExecutive_Data & exeData = pData->tExeData;
    PutXmlChildBool(node, "allowOptInOut", exeData.tCommonData.bAllowOpt);

    xml_node userRootNode = node.append_child("assignedUsers");
    ExeAssisList & targetList = exeData.tCommonData.listData;
    for (ExeAssisListIt it = targetList.begin(); it != targetList.end(); ++it)
    {
        xml_node childNode = userRootNode.append_child("userDetails");
        PutXmlChildText(childNode, "userId", it->ItemName.strUserId.c_str());
    }

    return true;
}

bool CExecutiveAssistantParse::FillFiltering(xml_node & node, ExeAssisData * pData)
{
    if (node.empty() || pData == NULL || pData->IsInvalid())
    {
        return false;
    }

    CExecutive_Data & exeData = pData->tExeData;
    PutXmlChildBool(node, "enableFiltering", exeData.tFilter.bSwitchOn);

    PutXmlChildText(node, "filteringMode", exeData.tFilter.GetFilterMode());
    PutXmlChildText(node, "simpleFilterType", exeData.tFilter.GetFilterType());

    xml_node nodeCriteriaRoot = node.append_child("criteriaActivations");
    if (!nodeCriteriaRoot.empty())
    {
        CExeFilterInfo::CriteriaList & targetList = exeData.tFilter.tCriteriaList;
        for (CExeFilterInfo::CriteriaListIt it = targetList.begin(); it != targetList.end(); ++it)
        {
            // warring:: Criteria只需要Put Active的节点
            if (it->bActive)
            {
                xml_node childNode = nodeCriteriaRoot.append_child("criteriaActivation");
                PutXmlChildText(childNode, "criteriaName", it->strName);
                PutXmlChildBool(childNode, "active", it->bActive);
            }
        }
    }

    return true;
}

bool CExecutiveAssistantParse::FillScreening(xml_node & node, ExeAssisData * pData)
{
    if (node.empty() || pData == NULL || pData->IsInvalid())
    {
        return false;
    }

    CExecutive_Data & exeData = pData->tExeData;
    PutXmlChildBool(node, "enableScreening", exeData.tScreen.bScreeningOn);
    PutXmlChildText(node, "screeningAlertType", exeData.tScreen.GetAlterType());
    PutXmlChildBool(node, "alertBroadWorksMobilityLocation", exeData.tScreen.bAlertMobility);
    PutXmlChildBool(node, "alertBroadWorksAnywhereLocations", exeData.tScreen.bAlertAnywhere);
    PutXmlChildBool(node, "alertSharedCallAppearanceLocations", exeData.tScreen.bAlertSCA);

    return true;
}

bool CExecutiveAssistantParse::FillAltering(xml_node & node, ExeAssisData * pData)
{
    if (node.empty() || pData == NULL || pData->IsInvalid())
    {
        return false;
    }

    CExecutive_Data & exeData = pData->tExeData;
    PutXmlChildText(node, "alertingMode", exeData.tAlter.GetAlterMode());
    PutXmlChildText(node, "alertingCallingLineIdNameMode", exeData.tAlter.GetCallInfoName());

    // warring:: string类型不能为空，所以添加一个类型判断，只有custom类型才会去填写
    if (exeData.tAlter.eCallInfoNameSource == ECIS_Custom)
    {
        PutXmlChildText(node, "alertingCustomCallingLineIdName", exeData.tAlter.strCustomCallName);
        if (!exeData.tAlter.strUnicodeCustomCallName.empty())
        {
            PutXmlChildText(node, "unicodeAlertingCustomCallingLineIdName",
                            exeData.tAlter.strUnicodeCustomCallName);
        }
    }

    PutXmlChildText(node, "alertingCallingLineIdPhoneNumberMode", exeData.tAlter.GetCallInfoNumber());

    // warring:: string类型不能为空，所以添加一个类型判断，只有custom类型才会去填写
    if (exeData.tAlter.eCallInfoNumberSource == ECIS_Custom)
    {
        PutXmlChildText(node, "alertingCustomCallingLineIdPhoneNumber", exeData.tAlter.strCustomCallNumber);
    }

    PutXmlChildText(node, "callPushRecallNumberOfRings", commonAPI_FormatString("%d",
                    exeData.tAlter.iCallPushRecallAfter));
    PutXmlChildText(node, "nextAssistantNumberOfRings", commonAPI_FormatString("%d",
                    exeData.tAlter.iAdvanceToNextAssistantAfter));

    PutXmlChildText(node, "rolloverWaitTimeSeconds", commonAPI_FormatString("%d",
                    exeData.tAlter.iRolloverAfter));
    PutXmlChildText(node, "rolloverAction", exeData.tAlter.GetRolloverAction());

    // warring:: string类型不能为空，所以添加一个类型判断，只有sip类型才会去填写
    if (exeData.tAlter.eRolloverAction == ERA_FWD_SIPURI)
    {
        PutXmlChildText(node, "rolloverForwardToPhoneNumber", exeData.tAlter.strRolloverTo);
    }

    return true;
}

bool CExecutiveAssistantParse::FillExecutiveList(xml_node & node, ExeAssisData * pData,
        ExeAssisData * pDataRefer)
{
    if (node.empty() || pData == NULL || pDataRefer == NULL
            || pData->IsInvalid() || pDataRefer->IsInvalid())
    {
        return false;
    }

    bool bChanged = false;
    CAssistant_Data & exeData = pData->tAssData;
    CAssistant_Data & exeDataRefer = pDataRefer->tAssData;

    PutXmlChildBool(node, "enableDivert", exeData.tCommonData.bDivertOn);
    if (exeData.tCommonData.bDivertOn)
    {
        PutXmlChildText(node, "divertToPhoneNumber", exeData.tCommonData.strDivertNumber);
    }

    bChanged |= exeData.tCommonData.bDivertOn != exeDataRefer.tCommonData.bDivertOn;
    bChanged |= exeData.tCommonData.strDivertNumber != exeDataRefer.tCommonData.strDivertNumber;

    xml_node userRootNode = node.append_child("executives");

    ExeAssisList & targetList = exeData.tCommonData.listData;
    ExeAssisListIt it = targetList.begin();

    ExeAssisList & targetListRefer = exeDataRefer.tCommonData.listData;
    ExeAssisListIt itRefer = targetListRefer.begin();

    // Executive必须至少带上一个，否则缺少executiveDetails字段，服务器会拒绝接受
    bool bAdd = false;
    for (; it != targetList.end() && itRefer != targetListRefer.end(); ++it, ++itRefer)
    {
        if (it->bOptIn != itRefer->bOptIn)
        {
            xml_node childNode = userRootNode.append_child("executiveDetails");
            PutXmlChildText(childNode, "executiveUserId", it->ItemName.strUserId.c_str());
            PutXmlChildBool(childNode, "optIn", it->bOptIn);
            bChanged |= it->bOptIn != itRefer->bOptIn;
            bAdd = true;
            break;
        }
    }

    // 如果没有添加，则取第一个
    if (!bAdd && targetList.size() > 0)
    {
        it = targetList.begin();
        xml_node childNode = userRootNode.append_child("executiveDetails");
        PutXmlChildText(childNode, "executiveUserId", it->ItemName.strUserId.c_str());
        PutXmlChildBool(childNode, "optIn", it->bOptIn);
    }

    XSI_INFO("FillExecutiveList changed=[%d]", bChanged);
    // warring:: 该页面较特殊，如果没有改动，不需要更新
    return bChanged;
}

bool CExecutiveAssistantParse::FillExecutiveList(xml_node & node, ExeAssisData * pData,
        ExeAssisData * pDataRefer, int iNode)
{
    if (node.empty() || pData == NULL || pDataRefer == NULL
            || pData->IsInvalid() || pDataRefer->IsInvalid() || iNode < 0
            || iNode >= pData->tAssData.tCommonData.listData.size()
            || iNode >= pDataRefer->tAssData.tCommonData.listData.size())
    {
        return false;
    }

    CAssistant_Data & exeData = pData->tAssData;
    CAssistant_Data & exeDataRefer = pDataRefer->tAssData;

    PutXmlChildBool(node, "enableDivert", exeData.tCommonData.bDivertOn);
    if (exeData.tCommonData.bDivertOn)
    {
        PutXmlChildText(node, "divertToPhoneNumber", exeData.tCommonData.strDivertNumber);
    }
    xml_node userRootNode = node.append_child("executives");

    CUserItem & targetItem = exeData.tCommonData.listData[iNode];
    CUserItem & targetItemRefer = exeDataRefer.tCommonData.listData[iNode];

    // 如果OptIn状态没有改变，则不构造Xml文件
    if (targetItem.bOptIn != targetItemRefer.bOptIn)
    {
        xml_node childNode = userRootNode.append_child("executiveDetails");
        PutXmlChildText(childNode, "executiveUserId", targetItem.ItemName.strUserId.c_str());
        PutXmlChildBool(childNode, "optIn", targetItem.bOptIn);

        return true;
    }

    return false;
}

#endif

