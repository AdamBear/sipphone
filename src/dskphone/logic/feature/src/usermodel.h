#ifndef _USER_MODEL_H__
#define _USER_MODEL_H__

#include "stringrelation.h"
#include "xmlparser/xmlparser.hpp"
#include <ylvector.h>
#include <ylstring.h>

// 权限信息
struct CPermissionInfo
{
    int m_iOpenShowLev;     // 显示直接权限等级
    int m_iOpenWReadLev;    // 读写直接权限等级
    int m_iMaxWReadLev;     // 读写最大权限等级

    CPermissionInfo()
    {
        m_iOpenShowLev = 0;
        m_iOpenWReadLev = 0;
        m_iMaxWReadLev = 0;
    }
};

// 权限信息列表（key == currAction）
typedef YLHashMap<yl::string, CPermissionInfo> MAP_PERMISSIONINFO;
typedef MAP_PERMISSIONINFO::iterator ITER_PERMISSIONINFO;


class CUserModel
{
public:
    CUserModel();
    virtual ~CUserModel();

public:
    // 初始化
    void Init();

    void UnInit();

    // 根据Action获取显示权限等级
    int GetShowLevByAction(const char * lpszAction);

    // 根据Action获取读写权限等级
    int GetWReadLevByAction(const char * lpszAction);

    // 用户配置是否为11：兼容网页的错
    bool IsDoubleLev1(const char * lpszAction);

    MAP_PERMISSIONINFO & GetMapPermissionInfo();
private:
    // 加载权限数据
    bool LoadPermissionData(const char * lpszFileName);

    bool LoadPermissionFromCfgserver(const char * lpszFileName);

    bool LoadPermissionFromIni(const char * lpszFileName);

    // 读取权限值 优化于网页规则一致
    void ReadPermissionValue(const yl::string & strPermissionInfo, int & iShowLevel, int & iWReadLevel);

    // 加载界面树形关系表
    bool LoadActionTreeRelation(const char * lpszFileName);

    // 递归遍历xml的所有节点
    void CallbackTraversal(xml_node node);

    yl::string GetActionTreeFilePath();
    yl::string GetItemMapFilePath();

    // 对特殊字符串进行处理
    yl::string HandleSpecialAtcion(const char * lpszAction);

    // 根据配置处理机制（cfgserver/ ini文件）区分读取三级权限文件名是WebItemslevel还是levelTranslate;
    yl::string GetInitThreeLevelFileName();

private:
    // 权限列表
    MAP_PERMISSIONINFO m_mapPermissionInfo;

    // 不同机型间ItemName映射表
    CStringRelation m_relationItemName;
};

#endif //_USER_MODEL_H__

