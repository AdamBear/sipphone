#include "eventparser.h"
#include "xmlparser/xmlparser.hpp"
#include "commonapi/stringhelperapi.h"
#include "leddata.h"
#include "devicelib/phonedevice.h"
#include "ylstl/ylalgorithm.h"
#include "dsklog/log.h"
#if IF_FEATURE_37754
#include <configiddefine.h>
#include <configparser/modconfigparser.h>
#endif
//#include <string.h>

/* 字串转成数字 */
int _StrToInt(const char * pStr)
{
    int nre = -1;
    if (!pStr)
    {
        return nre;
    }
    if (0 == strlen(pStr))
    {
        return nre;
    }
    sscanf(pStr, "%d", &nre);
    return nre;
}
/*
解析xml文档
@param:
strFilePath: [in] xml文档路径
_tEventMap:  [out] 解析内容存放
*/
bool CEventParser::Parse(const yl::string & strFilePath, MAPEVENTUNIT & _tEventMap)
{
    /* 缓存map(字串, 事件类型) */
    _InitKmap();

    /* 加载xml文件 */
    xml_document doc;
    if (!doc.load_file(strFilePath.c_str()))
    {
        return false;
    }
    xml_node nodeRoot = doc.first_child();
    if (!nodeRoot)
    {
        return false;
    }

    xml_node nodeChild;// = nodeRoot.child("event");

    // V81 新格式
    xml_node nodeConfig = nodeRoot.child("configuration");
    if (!nodeConfig.empty())
    {
        printf("load new format xml\n");
        // 如果携带configuration节点说明是新格式，需要查找对应的机型
        yl::string strDev = nodeConfig.attribute("dev").value();
        while (!nodeConfig.empty()
                && strDev.find(devicelib_GetPhoneName()) == yl::string::npos)
        {
            nodeConfig = nodeConfig.next_sibling();
            strDev = nodeConfig.attribute("dev").value();
        }

        nodeChild = nodeConfig.child("event");
    }
    else
    {
        nodeChild = nodeRoot.child("event");
    }

    if (!nodeChild)
    {
        return false;
    }

    /* 循环查找event节点 */
    while (1)
    {
        if (!nodeChild)
        {
            break;
        }

        /* 优先级 */
        const char * pPri = nodeChild.attribute("priority").value();
        /* 事件名称 */
        const char * pevtName = nodeChild.attribute("eventname").value();
        /* 亮时间 */
        const char * plT = nodeChild.attribute("lightTime").value();
        /* 暗时间 */
        const char * pdT = nodeChild.attribute("darkTime").value();
        /* 灯号 */
        const char * pLampId = nodeChild.attribute("LampID").value();
        /* 灯状态 */
        const char * pStatus = nodeChild.attribute("lampstatus").value();

        tEventUnit teU;
        teU._npriority = _StrToInt(pPri);
        teU._nDarkTime = _StrToInt(pdT);
        teU._nLightTime = _StrToInt(plT);

        // 解析灯序号
        ParseLampId(teU.MapLed, NULL == pLampId ? "" : pLampId, NULL == pStatus ? "" : pStatus);

        teU._nLampNum = _StrToInt(pLampId);

        /* 由事件名称, 在缓存中取出相应事件类型 */
        LAMP_EVENTTYPE evtType = _GetEventTypeByStr(pevtName);
        if (LAMP_EVENTTYPE_NONE == evtType)
        {
            nodeChild = nodeChild.next_sibling();
            continue;
        }

        _tEventMap.put(evtType, teU);

        nodeChild = nodeChild.next_sibling();
    }

    return true;
}

/*
解析xml文档
@param:
strFilePath: [in] xml文档路径
_tEventMap:  [out] 解析内容存放
*/
bool CEventParser::ParseColor(const yl::string & strFilePath, MAPLEDCOLORVALUE & _tColorMap)
{
    /* 加载xml文件 */
    xml_document doc;
    if (!doc.load_file(strFilePath.c_str()))
    {
        return false;
    }

    xml_node nodeRoot = doc.first_child();
    if (!nodeRoot)
    {
        return false;
    }

    xml_node nodeChild;/* = nodeRoot.child("ledlampcolor")*/;
    xml_node nodeConfig = nodeRoot.child("configuration");
    if (!nodeConfig.empty())
    {
        // 如果携带configuration节点说明是新格式，需要查找对应的机型
        yl::string strDev = nodeConfig.attribute("dev").value();
        while (!nodeConfig.empty()
                && strDev.find(devicelib_GetPhoneName()) == yl::string::npos)
        {
            nodeConfig = nodeConfig.next_sibling();
            strDev = nodeConfig.attribute("dev").value();
        }

        nodeChild = nodeConfig.child("ledlampcolor");
    }
    else
    {
        nodeChild = nodeRoot.child("ledlampcolor");
    }

    if (!nodeChild)
    {
        return false;
    }

    /* 循环查找event节点 */
    while (1)
    {
        if (!nodeChild)
        {
            break;
        }

        /* LedID */
        int iLampID = nodeChild.attribute("LampID").as_int();

        xml_node nodeColor = nodeChild.child("color");

        if (nodeColor)
        {
            MAPCOLORVALUE MapValue;

            /* 黄 */
            const char * pY = nodeColor.attribute("y").value();
            /* 红 */
            const char * pR = nodeColor.attribute("r").value();
            /* 绿 */
            const char * pG = nodeColor.attribute("g").value();
            /* 橙 */
            const char * pO = nodeColor.attribute("o").value();

            if (pY != NULL && strtoul(pY, 0, 16) > 0)
            {
                MapValue[LED_COLOR_YELLOW] = strtoul(pY, 0, 16);
            }
            if (pR != NULL && strtoul(pR, 0, 16) > 0)
            {
                MapValue[LED_COLOR_RED] = strtoul(pR, 0, 16);
            }
            if (pG != NULL && strtoul(pG, 0, 16) > 0)
            {
                MapValue[LED_COLOR_GREEN] = strtoul(pG, 0, 16);
            }

            if (pO != NULL && strtoul(pO, 0, 16) > 0)
            {
                MapValue[LED_COLOR_ORANGE] = strtoul(pO, 0, 16);
            }
            else if (pY != NULL && pR != NULL)
            {
                MapValue[LED_COLOR_ORANGE] = MapValue[LED_COLOR_RED] + MapValue[LED_COLOR_GREEN];
            }

            _tColorMap[iLampID] = MapValue;
        }

        nodeChild = nodeChild.next_sibling();
    }

    return true;
}

// 解析灯序号
void CEventParser::ParseLampId(MPA_LED_COLOR & MapLedColor, const yl::string & strLampId,
                               const yl::string & strStatus)
{
//  LEDLAMP_INFO("strLampId[%s] .strStatus[%s]", strLampId.c_str(), strStatus.c_str());

    if (strLampId.empty())
    {
        return;
    }

    YLList<yl::string> ListLampIDGroup;
    YLList<yl::string> ListStatus;

    commonAPI_splitStringBySeparator(strLampId, ';', ListLampIDGroup);
    commonAPI_splitStringBySeparator(strStatus, ';', ListStatus);

    YLList<yl::string>::ListIterator itrLampIDs = ListLampIDGroup.begin();
    YLList<yl::string>::ListIterator itrStatus = ListStatus.begin();

    for (; itrLampIDs != ListLampIDGroup.end(); itrLampIDs++)
    {
        LED_COLOR eColor = LED_COLOR_YELLOW;

        if (itrStatus != ListStatus.end())
        {
            eColor =  GetColor(*itrStatus);
        }

        YLList<yl::string> ListLampIDs;
        commonAPI_splitStringBySeparator(*itrLampIDs, ',', ListLampIDs);

        for (YLList<yl::string>::ListIterator itr = ListLampIDs.begin(); itr != ListLampIDs.end(); itr++)
        {
            int iLampNum = _StrToInt((*itr).c_str());
            MapLedColor[iLampNum] = eColor;//_LedData.GetColorValue(iLampNum, eColor);
        }

        if (itrStatus != ListStatus.end())
        {
            itrStatus++;
        }
    }
}

LED_COLOR CEventParser::GetColor(const yl::string & strColor)
{
    if (strColor.compare("y") == 0)
    {
        return LED_COLOR_YELLOW;
    }
    if (strColor.compare("r") == 0)
    {
        return LED_COLOR_RED;
    }
    if (strColor.compare("g") == 0)
    {
        return LED_COLOR_GREEN;
    }
    if (strColor.compare("o") == 0)
    {
        return LED_COLOR_ORANGE;
    }

    return LED_COLOR_YELLOW;
}

