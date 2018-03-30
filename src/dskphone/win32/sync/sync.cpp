#include <xmlparser/pugixml.hpp>
#include <winimp/winimp.h>
#include "devicelib/phonedevice.h"
#include "service_cfg.h"

using namespace std;
using namespace pugi;

#define ATT_CFG_ROOTNODE "configMap"
#define ATT_CFG_OBJNODE  "object"
#define ATT_CFG_TRANSFER "transfer"
#define ATT_CFG_REBOOT   "reboot"
#define ATT_CFG_ITEM     "item"
#define ATT_CFG_DEVNAME  "dev"
#define ATT_CFG_OBJNAME  "name"
#define ATT_CFG_PATH     "path"
#define ATT_CFG_SET     "set"
#define ATT_CFG_CONDITION "condition"
#define ATT_CFG_VALUE  "value"
#define ATT_CFG_MODE    "mode"

#define INFO(fmt,...)   fprintf(stdout, fmt"\n", ##__VA_ARGS__)
#define ERR(fmt,...)    do { INFO("[err][%s][%d]" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__); exit(0); }while (0)

void parseTransferNode(xml_node pNode)
{
    if (pNode.empty())
    {
        return;
    }
    xml_node child_Node = pNode.first_child();
    while (child_Node)
    {
        xml_node instance_Node = child_Node.first_child();
        if (instance_Node.empty())
        {
            child_Node = child_Node.next_sibling();
            continue;
        }
        while (instance_Node)
        {
            std::string set = instance_Node.attribute(ATT_CFG_SET).as_string();
            std::string value = instance_Node.attribute(ATT_CFG_VALUE).as_string();
            if (!set.empty())
            {
                char strBuffer[1024] = "";
                int ret = 0;
                mkit_handle_t caller  = mkGetHandle();
                if (cfg_get_string(caller, set.c_str(), strBuffer, 1024, "") != 0)
                {
                    if (value.c_str()[0] == '?')
                    {
                        memcpy(strBuffer, &value.c_str()[1], value.size() - 1);
                        ret = cfg_set_string(caller, set.c_str(), strBuffer);
                    }
                    else
                    {
                        ret = cfg_set_string(caller, set.c_str(), (char *)value.c_str());
                    }
                }
                else
                {
                    if (!value.empty())
                    {
                        if (value.c_str()[0] == '?')
                        {
                            if (strcmp(strBuffer, &value.c_str()[1]) != 0)
                            {
                                memcpy(strBuffer, &value.c_str()[1], value.size() - 1);
                                ret = cfg_set_string(caller, set.c_str(), strBuffer);
                            }
                        }
                        else
                        {
                            if (strcmp(strBuffer, value.c_str()) != 0)
                            {
                                ret = cfg_set_string(caller, set.c_str(), (char *)value.c_str());
                            }
                        }
                    }
                }
                if (0 != ret)
                {
                    INFO("cfg_set_string fail key=%s ,value = %s ", set.c_str(), value.c_str());
                }
            }
            instance_Node = instance_Node.next_sibling();
        }
        child_Node = child_Node.next_sibling();
    }
}
bool getInitDataFromDeviceType()
{
    std::string factorFilepath = CUSTOM_FACTORY_PATH;
    xml_document pDoc;
    xml_parse_result ret = pDoc.load_file(factorFilepath.c_str());
    if (ret.status != status_ok)
    {
        return false;
    }
    xml_node pRootNode = pDoc.child(ATT_CFG_ROOTNODE);
    if (pRootNode.empty())
    {
        return false;
    }
    const char * currentdevName = devicelib_GetPhoneName();
    xml_node CFG_Node = pRootNode.first_child();

    while (CFG_Node)
    {
        std::string devName = CFG_Node.attribute(ATT_CFG_DEVNAME).as_string();
        printf("current devName  = %s;  cfgDevNAme = %s \n", currentdevName, devName.c_str());
        if (devName.empty())
        {
            CFG_Node = CFG_Node.next_sibling();
            continue;
        }
        if (strcmp(currentdevName, devName.c_str()) !=
                0) //判断当前运行的设备型号与读取配置节点不一致
        {
            CFG_Node = CFG_Node.next_sibling();
            continue;
        }
        else//当前节点为配置节点时
        {
            //transfer 节点
            xml_node Transfer_Node = CFG_Node.child(ATT_CFG_TRANSFER);
            parseTransferNode(Transfer_Node);
            break;
        }
        CFG_Node = CFG_Node.next_sibling();
    }
}

int main(int argc, char * argv[])
{
    if (mkservice_init(NULL, 0) < 0)
    {
        ERR("mkservice_init");
    }

    if (mkGetHandle() == 0)
    {
        ERR("mkGetHandle");
    }

    if (mktask_do_register(mkGetHandle()) != 0)
    {
        ERR("mktask_do_register");
    }

    if (cfg_init())
    {
        ERR("cfg_init");
    }

    return getInitDataFromDeviceType();
}
