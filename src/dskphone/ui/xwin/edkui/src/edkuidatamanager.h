#ifndef _EDKUI_DATA_MANAGER_H_
#define _EDKUI_DATA_MANAGER_H_

#include <singletonclassdefine.h>
#include <ETLLib.hpp>
#include "edkui/include/edkui_def.h"

class CEdkUIDataManager
{
    DECLARE_SINGLETON_CLASS(CEdkUIDataManager)
public:
    bool LoadMapConfig();
    //---------------------- softkey ----------------------
    // 通过softkey type获取softkey id
    bool GetSoftkeyIDByType(SoftKey_TYPE eType, yl::string & strSoftkeyID);
    // 通过softkey id获取softkey type
    SoftKey_TYPE GetSoftkeyTypeByID(const yl::string & strSoftkeyID);

    //---------------------- setting ----------------------
    // 通过menu id获取page name
    bool GetPageNameByMenuID(const yl::string & strMenuID, yl::string & strPageName);
    // 通过page name获取menu id
    bool GetMenuIDByPageName(const yl::string & strPageName, yl::string & strMenuID);

    //---------------------- directory ----------------------
    // 通过menu id获取dir type
    int GetDirTypeByMenuID(const yl::string & strMenuID);
    // 通过dir type获取menu id
    bool GetMenuIDByDirType(int iDirType, yl::string & strMenuID);

    //---------------------- program key ----------------------
    bool GetProgramkeyIDByType(DssKey_Type eType, yl::string & strKeyID);
    DssKey_Type GetProgramkeyTypeByID(const yl::string & strKeyID);

    bool FindSoftkeyItemByID(Iter_SoftkeyID_Type_Map & iterDest, const yl::string & strID);

    //---------------------- Allow enter ui func --------------
    // 设置menu id对应的条件判断函数指针
    bool SetAllowEnterUIFunc(FuncIsAllowEnterMenu pAllowFunc,
                             const yl::string & strPageName = "",
                             int iDirType = 0);
    // 获取menu id对应的条件判断函数指针
    bool GetAllowEnterUIFunc(const yl::string & strMenuID, FuncIsAllowEnterMenu & pAllowFunc);

private:
    // 清空数据
    void Clear();

    bool IsPhoneSupport(const yl::string& strDev);

    // softkey id和softkey type映射
    typeSoftkeyIDToTypeMap  m_mapSoftkeyIDToType;
    // 保存menu id和联系人ui类型
    typeMenuIDToDirTypeMap  m_mapMenuIDToDirType;
    // menu id和page name的映射表
    typeMenuIDToPageMap     m_mapMenuIDPage;
    // 存放menu id和页面是否允许进入的判断的函数指针
    typeMenuIDToAllowFunc   m_mapAllowEnterUIFunc;
};
// 实例
#define _EDKUIDataMgr GET_SINGLETON_INSTANCE(CEdkUIDataManager)
#endif // _EDKUI_DATA_MANAGER_H_
