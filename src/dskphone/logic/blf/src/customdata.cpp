#include "customdata.h"
#include "blf_inc.h"

#define NODE_ITEM_DATA          "itemdata"
#define ATTR_ENABLE             "enable"
#define ATTR_INDEX              "index"
#define ATTR_PRIORITY           "priority"
#define ATTR_LED                "led"
#define ATTR_IDLE               "idle"
#define ATTR_CALLIN             "callin"
#define ATTR_CALLOUT            "callout"
#define ATTR_TALKING            "talking"
#define ATTR_PARKED             "parked"

CBLFCustomData::CBLFCustomData()
{
    m_bEnable = false;
    m_nPriority = 0;
}

CBLFCustomData::~CBLFCustomData()
{

}

bool CBLFCustomData::ParseFromXmlNode(const xml_node & xmlNode)
{
    m_strStatus = xmlNode.name();

    yl::string strTmp = xmlNode.attribute(ATTR_ENABLE).value();
    m_bEnable = (strTmp.compare("1") == 0);

    xml_node dataNode = xmlNode.child(NODE_ITEM_DATA);

    strTmp = dataNode.attribute(ATTR_PRIORITY).value();
    m_nPriority = atoi(strTmp.c_str());

    m_strLed = dataNode.attribute(ATTR_LED).value();

    strTmp = dataNode.attribute(ATTR_IDLE).value();
    m_mapDsskeyAction.insert(ATTR_IDLE, strTmp);
    strTmp = dataNode.attribute(ATTR_CALLIN).value();
    m_mapDsskeyAction.insert(ATTR_CALLIN, strTmp);
    strTmp = dataNode.attribute(ATTR_CALLOUT).value();
    m_mapDsskeyAction.insert(ATTR_CALLOUT, strTmp);
    strTmp = dataNode.attribute(ATTR_TALKING).value();
    m_mapDsskeyAction.insert(ATTR_TALKING, strTmp);
    strTmp = dataNode.attribute(ATTR_PARKED).value();
    m_mapDsskeyAction.insert(ATTR_PARKED, strTmp);

    return true;
}

eBLFOperateType CBLFCustomData::GetOperationType(const yl::string & strSessState)
{
    if (strSessState.empty())
    {
        return OPERATE_BLF_TYPE_NONE;
    }

    if (m_mapDsskeyAction.find(strSessState) == m_mapDsskeyAction.end())
    {
        return OPERATE_BLF_TYPE_NONE;
    }

    return MapDsskeyAction(m_mapDsskeyAction[strSessState]);
}

yl::string CBLFCustomData::GetOperationValue(const yl::string & strSessState)
{
    if (strSessState.empty())
    {
        return "";
    }

    if (m_mapDsskeyAction.find(strSessState) == m_mapDsskeyAction.end())
    {
        return "";
    }

    return m_mapDsskeyAction[strSessState];
}

eBLFOperateType CBLFCustomData::MapDsskeyAction(const yl::string & strDsskeyAction)
{
    if (strcasecmp(strDsskeyAction.c_str(), "newcall") == 0)
    {
        return OPERATE_BLF_TYPE_NEWCALL;
    }
    else if (strcasecmp(strDsskeyAction.c_str(), "btransfer") == 0)
    {
        return OPERATE_BLF_TYPE_BTRANSFER;
    }
    else if (strcasecmp(strDsskeyAction.c_str(), "ctransfer") == 0)
    {
        return OPERATE_BLF_TYPE_CTRANSFER;
    }
    else if (strcasecmp(strDsskeyAction.c_str(), "pickup") == 0)
    {
        return OPERATE_BLF_TYPE_PICKUP;
    }
    else if (strcasecmp(strDsskeyAction.c_str(), "bargein") == 0)
    {
        return OPERATE_BLF_TYPE_BARGEIN;
    }
    else if (!strDsskeyAction.empty())
    {
        return OPERATE_BLF_TYPE_EDK;
    }

    return OPERATE_BLF_TYPE_NONE;
}

