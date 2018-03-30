#include "itemdataparserfactory.h"


IMPLEMENT_GETINSTANCE(CItemDataParserFactory)


CItemDataParserFactory::CItemDataParserFactory()
{
}

CItemDataParserFactory::~CItemDataParserFactory()
{
}

void CItemDataParserFactory::RegisterItemDataParser(const yl::string & strItemType,
        FUN_CREATE_ITEM_DATA_PARSER pFunCreateItemDataParser)
{
    if (NULL == pFunCreateItemDataParser)
    {
        return;
    }

    m_mapCreateItemDataParser[strItemType] = pFunCreateItemDataParser;
}

bool CItemDataParserFactory::ParserItemData(const yl::string & strItemType, const xml_node & node,
        CSettingUIItemDataBase * pItemInfo)
{
    MAP_CREATE_ITEM_DATA_PARSER_FUN::iterator it = m_mapCreateItemDataParser.find(strItemType);
    FUN_CREATE_ITEM_DATA_PARSER pFun = NULL;
    if (it != m_mapCreateItemDataParser.end())
    {
        pFun = it->second;
    }

    if (NULL != pFun)
    {
        return (*pFun)(node, pItemInfo);
    }

    // 默认处理
    return CSettingUIItemDataBase::ParserItemData(node, pItemInfo);
}

