#include "feature_inc.h"

// 以下为Xml结点名定义.
// 根节点.
#define NODE_ROOT "root_rtp"
// 显示名属性.
#define ATTR_DISPLAY "display_name"
// 优先级属性.
#define ATTR_PRIORITY "priority"
// 开启属性.
#define ATTR_ENABLE "enable"

IMPLEMENT_GETINSTANCE(CRTPConfigManager)

CRTPConfigManager::CRTPConfigManager()
{
}

CRTPConfigManager::~CRTPConfigManager()
{
}

void CRTPConfigManager::Init()
{
    // 加载列表
    LoadRTPConfig();

    // 注册消息.
    SingleMsgReg(CONFIG_MSG_BCAST_CHANGED, CRTPConfigManager::OnMessage);
}

LRESULT CRTPConfigManager::OnMessage(msgObject & refObj)
{
    switch (refObj.message)
    {
    case CONFIG_MSG_BCAST_CHANGED:
        {
            if (ST_VQ_REPORT_OPTION == refObj.wParam)
            {
                // 重新加载配置文件
                _RTPConfigManager.LoadRTPConfig();
            }
        }
        break;
    default:
        break;
    }

    return 1;
}

// 辅助函数 比较大小.
typedef bool (*FunCompareCfgItem)(const RTPConfigureItem & lhs, const RTPConfigureItem & rhs);
static bool Compare(const RTPConfigureItem & lhs, const RTPConfigureItem & rhs)
{
    return lhs.m_nPriority <= rhs.m_nPriority;
}

bool CRTPConfigManager::LoadRTPConfig()
{
    m_listCfg.clear();

    // load xml file.
    xml_document doc;
    if (!doc.load_file(szRTPConfigureFile))
    {
        return false;
    }

    // Find root node.
    xml_node nodeRoot = doc.child(NODE_ROOT);
    if (!nodeRoot)
    {
        return false;
    }

    xml_node nodeChild = nodeRoot.first_child();
    if (!nodeChild)
    {
        return false;
    }

    // 读取各个配置结点.
    while (nodeChild)
    {
        RTPConfigureItem stCI;

        // 获取id名.
        stCI.m_strIdName = nodeChild.name();
        stCI.m_strDisplayName = nodeChild.attribute(ATTR_DISPLAY).value();
        stCI.m_isEnable = nodeChild.attribute(ATTR_ENABLE).as_int();
        // 如果没有开启, 则将权限设置为0, 否则读取配置.
        stCI.m_nPriority = nodeChild.attribute(ATTR_PRIORITY).as_int();

        if (stCI.m_isEnable)
        {
            m_listCfg.push_back(stCI);
        }
        nodeChild = nodeChild.next_sibling();
    } //while

    // 对配置文件进行排序.
    Sort<RTPConfigureItem, FunCompareCfgItem>(m_listCfg, Compare);

    return true;
}
