#ifndef _DSSKEY_UI_DATA_PARSE_HEADER_
#define _DSSKEY_UI_DATA_PARSE_HEADER_

#include <xmlparser/pugixml.hpp>
#include "dsskeyuilogiccommon.h"
#include "dsskeyfunctionmanager.h"
#include <ylstring.h>
#include <ylvector.h>
#include <ylhashmap.h>
#include "dsskey_define.h"

// Dsskey解析出来的数据结构
struct CDsskeyInrInfo
{
    char        chAuth[MAX_MODE_LENGTH];   // 三级权限配置
    int         iClassType;                // 类型属性
    int         iRedirectType;

    yl::string  strURI;                    // URI标志
    yl::string  strDisplayName;            // 显示名
    yl::string  strFunID;                  // 校验函数标志

    VecAttrItem   vecInrAttr;

public:
    // 查找
    CDsskeyItemAttr * Find(int iAttrType);
    // 新建
    CDsskeyItemAttr * CreateItem(int iAttrType);

public:
    ~CDsskeyInrInfo();
    CDsskeyInrInfo();

private:
    CDsskeyInrInfo(const CDsskeyInrInfo & other);
};

enum DSSKEY_RELOAD_MODE
{
    DSSKEY_ROAD_MOD_FACTORY = 0,
    DSSKEY_ROAD_MOD_CUSTOM,
    DSSKEY_ROAD_MOD_RECOVER,
};

typedef YLHashMap<int, CDsskeyInrInfo *> mapDsskeyIncAttr;

struct DsskeyMoudleSet
{
    vecDsskeySet vecDsskeyList;
    vecDsskeySet vecDsskeyEvent;
};

typedef YLHashMap<DSSKEY_UI_TYPE, DsskeyMoudleSet *> mapDsskeyList;

// 自定义校验函数
typedef bool (*dsskey_data_check)(int dsskeyID, const DssKeyUIData & dsskeyData,
                                  CDsskeyPair & errorTips);

using namespace pugi;

// 解析和数据缓存
class CDsskeyUIDataParse
{
public:
    // 初始化，解析入口
    void Init();

    // 释放
    void UnInit();

    // 获取函数操作结果
    bool GetFunctionResult(yl::string & strID);

    // 重新加载
    void Reload();

    // 获取默认数据
    const CDsskeyInrInfo & GetDefaultData()
    {
        return m_DefaultData;
    }

protected:

    // 获取Dsskey类型数据
    CDsskeyInrInfo & GetDsskeyXmlData(int iDsskeyType);

    // 是否默认信息
    bool IsDefaultData(const CDsskeyInrInfo & dsskeyInfo);

private:

    // 解析函数节点
    bool ParseFunction(const xml_node & nodeRoot);

    // 解析三级权限节点
    bool ParseAuthority(const xml_node & nodeRoot);

    // 解析场景节点
    bool PasrseSequence(const xml_node & nodeRoot, DSSKEY_RELOAD_MODE mode);

    // 解析单个Dsskey入口
    bool ParseDsskey(const xml_node & nodeRoot);

    // 解析默认值节点
    bool ParseDefaultDsskey(const xml_node & nodeRoot);

    // 解析提示语节点
    bool ParseTips(const xml_node & nodeRoot);

private:

    // 解析Dsskey通用属性
    bool ParseDsskeyAttr(const xml_node & nodeRoot, CDsskeyInrInfo & dsskeyIncAttrData);

    // 解析Dsskey差异累加属性
    bool ParseDsskeySingleAttr(const xml_node & nodeRoot, CDsskeyInrInfo & dsskeyIncAttrData);

    // 添加到列表
    bool FillListData(LPCSTR lpListData, vecDsskeySet & vecSet, vecDsskeySet & vecSetRef,
                      int iSkipType);

    // 解析单个文件
    bool ParseFile(LPCSTR lpFileName, DSSKEY_RELOAD_MODE mode);

    // 加入缓存中
    yl::string * CreatPool(const yl::string & strValue);

    // trace
    void LogTrace();

    static bool ParseString(YLVector<int> & veclist, const yl::string & strSrc,
        const yl::string & filterStr);

private:

    // 获取模块类型
    static DSSKEY_UI_TYPE GetMoudleIndex(LPCSTR lpName);

    // 获取联系人类型类型
    static DIR_TYPE GetDirType(LPCSTR lpName);

    // 获取xml string 属性值
    static bool GetXmlAttrValue(const xml_node & nodeRoot, LPCSTR lpAttr, yl::string & strValue);

    // 获取xml属性类型
    static DSSKEY_ATTR_ITEM GetAttrType(LPCSTR lpName);

protected:

    // lockdown level值
    static int GetLockDownLevel();

    // 查找重复项
    static bool Find(vecDsskeySet & vecSet, int iValue);

protected:
    CFunctionManager    m_FunLogic;         // 函数管理

    CDsskeyInrInfo      m_DefaultData;      // 默认的dsskey属性
    mapDsskeyIncAttr    m_mapIncAttr;       // dsskey个体属性
    mapDsskeyList       m_mapSet;           // dsskey集合属性
    vecDsskeySet        m_vecMsgHook;       // 接收的msg id回调消息值
    YLList<yl::string>  m_listString;       // 用来存储冗余的数据池
};

#endif
