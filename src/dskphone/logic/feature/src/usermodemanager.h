#ifndef _USERMODE_CONTROLLER_H__
#define _USERMODE_CONTROLLER_H__

#include "usermodel.h"
#include "../include/featurecommon.h"
#include "common/common_data_define.h"
#include <singletonclassdefine.h>

class CUserModeManager
{
    DECLARE_SINGLETON_CLASS(CUserModeManager)

public:

    static LRESULT OnMessage(msgObject & msg);

    bool OnConfigChang();

    // 初始化
    void Init();

    // 是否启用三级权限
    inline bool IsEnable()
    {
        return m_bEnable;
    }

    inline int LockLevel()
    {
        return m_iLockLevel;
    }

    // 返回当前用户权限
    USER_TYPE GetUserType(int iUIID = DEF_UIIID);

    // 设置当前用户权限
    void SetUserType(USER_TYPE eType, int iUIID = DEF_UIIID);

    // 判断指定的项是否有可见的权限
    bool IsItemHasShowAuth(const yl::string & strItemAction, int iUIID = DEF_UIIID);

    // 判断指定的项是否只读
    bool IsItemReadOnly(const yl::string & strItemAction, int iUIID = DEF_UIIID);

    // 根据权限获取账号名：用于密码匹配 / 显示
    void GetUserNameByUserMode(USER_TYPE eType, yl::string & strUserName);

    //获取权限数据
    bool GetUserModeData(UserModeListData & refData, int iUIID = DEF_UIIID);

    //获取权限数量
    int GetUserModeCnt(int iUIID = DEF_UIIID);

    // 获取默认的显示名
    void GetDefaultUserName(USER_TYPE eType, yl::string & strUserName);

private:
    // 是否启用三级权限
    bool m_bEnable;

    int  m_iLockLevel;

    // 当前权限类型
    USER_TYPE m_arrayUserType[HANDSET_NUMBER];

    // 权限配置数据
    CUserModel * m_pUserModel;
};

// 获取实例
#define _UserModeManager GET_SINGLETON_INSTANCE(CUserModeManager)

#endif //_USERMODE_CONTROLLER_H__

