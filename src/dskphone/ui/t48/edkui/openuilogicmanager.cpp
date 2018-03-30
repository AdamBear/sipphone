#include "openuilogicmanager.h"
#include "edkuimanager.h"
#include "path_define.h"
#include "baselibs/include/dsklog/log.h"
#include "xmlparser/xmlparser.hpp"
#include "resource/modresource.h"
#include "commonunits/t2xpath.h"

EDKUIOpenLogic::EDKUIOpenLogic()
{
    init();
}

void EDKUIOpenLogic::init()
{
    m_mapOpenLogic.clear();
    LoadXMLMapByNodeName(res_getPathByName(RESOURCE_PATH_EDKCONFIG), EDK_MAP_UI_NODE);
}

//从文件加载映射
void EDKUIOpenLogic::LoadXMLMapByNodeName(const yl::string & strFile, const yl::string & strNode)
{
    yl::string strFullPath =  strFile;

    EDK_INFO("CEdkSoftkeyAction::LoadXmlFiles, strFile = %s", strFile.c_str());

    if (!pathFileExist(strFullPath.c_str()))
    {
        return;
    }

    xml_document docXmlFile;
    if (!docXmlFile.load_file(strFullPath.c_str()))
    {
        EDK_INFO("CEdkSoftkeyAction::LoadXmlFiles, Load File Fail, strFile = %s", strFile.c_str());
        return;
    }

    xml_node xmlRoot = docXmlFile.first_child();
    if (xmlRoot.empty())
    {
        EDK_INFO("CEdkSoftkeyAction::LoadXmlFiles, First Child is Empty, strFile = %s", strFile.c_str());
        return;
    }

    //判断名字
    yl::string strBaseName = xmlRoot.name();
    if (0 != strBaseName.compare(EDK_MAP_ROOT_NODE))
    {
        EDK_INFO("CEdkSoftkeyAction::LoadXmlFiles, First Child Name Error, strFile = %s, strBaseName = %s",
                 strFile.c_str(), strBaseName.c_str());
        return;
    }

    xml_node xmlChildData = xmlRoot.child(strNode.c_str());
    yl::string strChildName = xmlChildData.name();
    if (0 != strChildName.compare(strNode))
    {
        EDK_INFO("CEdkSoftkeyAction::LoadXmlFiles, Child Name Error, strFile = %s, strChildName = %s",
                 strFile.c_str(), strChildName.c_str());
        return;
    }

    xml_node xmlItemData = xmlChildData.child(EDK_MAP_ITEM_NODE);
    while (!xmlItemData.empty())
    {
        if (strcmp(EDK_MAP_ITEM_NODE, xmlItemData.name()) != 0)
        {
            xmlItemData = xmlItemData.next_sibling();
            continue;
        }

        if (strNode == EDK_MAP_UI_NODE)
        {
            yl::string strTempHint = xmlItemData.attribute(EDK_MAP_HINT_ATTR).value();
            m_mapOpenLogic.insert(std::pair<yl::string, FunIsAllowEnterUI>(strTempHint,
                                  (FunIsAllowEnterUI)NULL));
        }
        else
        {
            break;
        }

        xmlItemData = xmlItemData.next_sibling();
    }
}

EDKUIOpenLogic * EDKUIOpenLogic::GetInstance()
{
    static EDKUIOpenLogic * pUILogic;
    if (pUILogic == NULL)
    {
        pUILogic = new EDKUIOpenLogic();
    }

    return pUILogic;
}

//设置对应UI的IsAllowEnter函数
bool EDKUIOpenLogic::SetFuncAllowEnterUI(yl::string strHint, FunIsAllowEnterUI pFunc)
{
    std::map<yl::string, FunIsAllowEnterUI>::iterator it = m_mapOpenLogic.find(strHint);
    if (it == m_mapOpenLogic.end())
    {
        EDK_INFO("SetFuncAllowEnterUI fail, %s is not find in map", strHint.c_str());
        return false;
    }

    m_mapOpenLogic[strHint] = pFunc;
    return true;
}

//是否允许进入UI
bool EDKUIOpenLogic::IsAllowEnterUI(yl::string strHint)
{
    std::map<yl::string, FunIsAllowEnterUI>::iterator it = m_mapOpenLogic.find(strHint);
    if (it == m_mapOpenLogic.end())
    {
        EDK_INFO("IsAllowEnterUI fail, %s is not find in map", strHint.c_str());
        return false;
    }

    if (m_mapOpenLogic[strHint] == NULL)
    {
        return true;
    }

    return m_mapOpenLogic[strHint]();
}
