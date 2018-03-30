#ifndef __DATA_FACTORY_H__
#define __DATA_FACTORY_H__

#include "xmlparser/pugixml.hpp"
#include "xmlparser/xmlparser.hpp"
#include "ylhashmap.h"
#include "yllist.h"
#include "ylstring.h"
#include "settinguibasepagedata.h"
#include "settinguisoftkeydata.h"
#include "settinguimessagedata.h"

class CSettingUIItemDataBase;
class CSettingUICheckBoxItemData;
class CSettingUIComboBoxItemData;
class CSettingUILineEditItemData;
class CSettingUIMenuItemData;
class CSettingUISoftkeyData;
class CSettingUIErrorInfo;

typedef YLHashMap<yl::string, CSettingUIPageDataBase *> PageInfoMap;
class CDataFactory
{
public:
    static CDataFactory * GetInstance();

    void init();
    bool GetPageInfo(const yl::string & strPagename, CSettingUIPageDataBase * pPageData);
    bool IsPageConfiged(const yl::string & strPagename);
    // 根据全路径，从指定文件中读取配置
    bool LoadPageDataByFileName(const yl::string & strFile, CSettingUIPageDataBase * pPageData);

    void ParseMessageInfo(const xml_node& node, LIST_SETTING_MSG_DATA* pMsgInfo);

    void ParseErrorInfo(const yl::string & strErrorString, CSettingUIItemDataBase * pItemInfo);

    void ParsePageRoot(const xml_node & node, CSettingUIPageDataBase * pPageData);

    void ParseSoftkeyInfo(const xml_node & node, CSettingUIPageDataBase * pPageData);

    void ParseItemNode(const xml_node & node, CSettingUIPageDataBase * pPageData);

    void ParseItemList(const xml_node & node, CSettingUIPageDataBase * pPageData);

    void PushPageInfo(const yl::string & strPageName, CSettingUIPageDataBase * pPageData);

private:
    explicit CDataFactory();

private:
    PageInfoMap m_mapInfo;
    static CDataFactory * m_pInstance;
};

#define g_DataFactory (CDataFactory::GetInstance())


#endif //__DATA_FACTORY_H__

