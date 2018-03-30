#ifndef __CONFIG_FILTER_MANAGER_H__
#define __CONFIG_FILTER_MANAGER_H__

#include "xmlparser/pugixml.hpp"
#include "xmlparser/xmlparser.hpp"
#include "ylhashmap.h"
#include "yllist.h"
#include "ylstring.h"
#include "settinguicommondata.h"


typedef YLHashMap<yl::string, SettingStaticConfigFilter *> StaticFilterMap;
typedef YLHashMap<yl::string, SettingDynamicConfigFilter *> DynamicFilterMap;

class CConfigFilterMgr
{
public:
    static CConfigFilterMgr * GetInstance();

    void Init();
    bool IsAllowShowStatic(const yl::string & strAuthorityID);
    SettingDynamicConfigFilter * GetAllowShowDynamic(const yl::string & strAuthorityID);
    bool DynamicFilter(SettingDynamicConfigFilter * pDynamicFilter, const yl::string & strConfig,
                       const yl::string & strAuthority
                       , const yl::string & strValue, bool & bShow, bool & bEditable);

    bool LoadConfigFilterByFileName(const char * pFileName);

private:
    explicit CConfigFilterMgr();

    void ParseStaticConfigFilter(const xml_node & node);

    void ParseDynamicConfigFilter(const xml_node & node);

    bool ParseConfigItem(const xml_node & node, int eType);
    void ParseConfigItem(const xml_node & node, SettingDynamicConfigFilter * pDynamic);

    // 数据转换
    SETTING_FILTER_TYPE StringFilterType2Enum(const yl::string & strFilterType);
    // 处理
    bool DealWithDynamicItemFilter(SettingDynamicItem * pDynamicItem, const yl::string & strConfig,
                                   const yl::string & strAuthority
                                   , const yl::string & strValue, bool & bShow, bool & bEditable, SETTING_FILTER_TYPE eType);

private:
    StaticFilterMap             m_mapStaticFilter;
    DynamicFilterMap            m_mapDynamicFilter;
    static CConfigFilterMgr  *  m_pInstance;
};

#define g_pConfigFilterMgr (CConfigFilterMgr::GetInstance())


#endif //__CONFIG_FILTER_MANAGER_H__

