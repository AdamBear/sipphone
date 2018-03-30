#ifndef __ITEM_DATA_PARSER_FACTORY_H__
#define __ITEM_DATA_PARSER_FACTORY_H__

#include <ylhashmap.h>
#include <singletonclassdefine.h>
#include "ylstring.h"
#include "../src/settinguibaseitemdata.h"
#include "xmlparser/pugixml.hpp"
#include "xmlparser/xmlparser.hpp"

typedef bool (*FUN_CREATE_ITEM_DATA_PARSER)(const xml_node & node,
        CSettingUIItemDataBase * pItemInfo);
typedef YLHashMap<yl::string, FUN_CREATE_ITEM_DATA_PARSER> MAP_CREATE_ITEM_DATA_PARSER_FUN;

class CItemDataParserFactory
{
    DECLARE_SINGLETON_CLASS(CItemDataParserFactory);

public:
    void RegisterItemDataParser(const yl::string & strItemType,
                                FUN_CREATE_ITEM_DATA_PARSER pFunCreateItemDataParser);

    bool ParserItemData(const yl::string & strItemType, const xml_node & node,
                        CSettingUIItemDataBase * pItemInfo);

protected:
    MAP_CREATE_ITEM_DATA_PARSER_FUN m_mapCreateItemDataParser;
};

#define g_ItemDataParserFactory GET_SINGLETON_INSTANCE(CItemDataParserFactory)

#endif // __ITEM_DATA_PARSER_FACTORY_H__

