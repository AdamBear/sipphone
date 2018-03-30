#ifndef _IF_EXECUTIVE_ASSISTANT_PARSE_H_
#define _IF_EXECUTIVE_ASSISTANT_PARSE_H_

#include "execommon.h"
#include <xmlparser/pugixml.hpp>

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
// 该类用于解析和组合XML文件

struct CExecutiveAssistantParse
{
    // 根据类型解析
    static bool ParseAssistantList(const xml_node & node, ExeAssisData * pData);
    static bool ParseFiltering(const xml_node & node, ExeAssisData * pData);
    static bool ParseScreening(const xml_node & node, ExeAssisData * pData);
    static bool ParseAlter(const xml_node & node, ExeAssisData * pData);
    static bool ParseExecutiveList(const xml_node & node, ExeAssisData * pData);

    // 依据类型生成xml, 基本原则, 保证xml节点的字符串类型不能为空
    static bool FillAssistantList(xml_node & node, ExeAssisData * pData);
    static bool FillFiltering(xml_node & node, ExeAssisData * pData);
    static bool FillScreening(xml_node & node, ExeAssisData * pData);
    static bool FillAltering(xml_node & node, ExeAssisData * pData);
    static bool FillExecutiveList(xml_node & node, ExeAssisData * pData, ExeAssisData * pDataRefer);

    // 填充单个Executive
    static bool FillExecutiveList(xml_node & node, ExeAssisData * pData, ExeAssisData * pDataRefer,
                                  int iNode);
};

#endif
#endif //_IF_EXECUTIVE_ASSISTANT_PARSE_H_
