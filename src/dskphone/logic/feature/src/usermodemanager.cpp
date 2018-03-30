#include "feature_inc.h"
#include "service_config.h"

#define DEFAULTNAME_USER    "user"
#define DEFAULTNAME_VAR     "var"
#define DEFAULTNAME_ADMIN   "admin"

IMPLEMENT_GETINSTANCE(CUserModeManager)

LRESULT CUserModeManager::OnMessage(msgObject & msg)
{
    if (msg.message == CONFIG_MSG_BCAST_CHANGED
            && msg.wParam == ST_USER_MODE)
    {
        return _UserModeManager.OnConfigChang();
    }

    return 0;
}

CUserModeManager::CUserModeManager()
    : m_pUserModel(NULL)
    , m_bEnable(false)
    , m_iLockLevel(0)
{
    for (int i = 0; i < HANDSET_NUMBER; i++)
    {
        m_arrayUserType[i] = UT_ADMIN;
    }

    SingleMsgReg(CONFIG_MSG_BCAST_CHANGED, CUserModeManager::OnMessage);
}

bool CUserModeManager::OnConfigChang()
{
    COMMONUNITS_INFO("CUserModeManager::OnConfigChang");

    if (m_pUserModel)
    {
        m_pUserModel->UnInit();
    }

    Init();
    return true;
}

CUserModeManager::~CUserModeManager()
{
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                            CUserModeManager::OnMessage);
    DEL_OBJ(m_pUserModel);
}

// 初始化
void CUserModeManager::Init()
{
    m_bEnable = (configParser_GetConfigInt(kszAdvancedVarEnabled) == 1);
    m_iLockLevel = configParser_GetConfigInt(kszLockLevel);
    if (m_iLockLevel < 0)
    {
        m_iLockLevel = 0;
    }

    COMMONUNITS_INFO("CUserModeManager::Init  locklevel=[%d] varenable=[%d]", m_iLockLevel, m_bEnable);
    if (m_bEnable || m_iLockLevel)
    {
        USER_TYPE eUserType;

        // 默认是user用户,如果user没开,则使用var
        eUserType = (USER_TYPE)configParser_GetConfigInt(kszDefaultUserLevel);

        if (UT_USER == eUserType
                && configParser_GetConfigInt(kszUserEnable) == 0)
        {
            eUserType = UT_VAR;
        }

        if (configParser_GetConfigInt(kszVarEnable) == 0
                && eUserType != UT_ADMIN)
        {
            // var被屏蔽则开启user，user也被屏蔽只能开admin
            eUserType = UT_USER;
            if (configParser_GetConfigInt(kszUserEnable) == 0)
            {
                eUserType = UT_ADMIN;
            }
        }

        if (!m_bEnable && m_iLockLevel && eUserType < UT_ADMIN)
        {
            // 开启lock，未开启三级权限
            eUserType = UT_ADMIN;
        }

        for (int i = 0; i < HANDSET_NUMBER; i++)
        {
            m_arrayUserType[i] = eUserType;
        }

        if (m_pUserModel == NULL)
        {
            // 实例化Model
            m_pUserModel = new CUserModel();
        }

        m_pUserModel->Init();
    }
}

// 返回当前用户权限
USER_TYPE CUserModeManager::GetUserType(int iUIID/* = DEF_UIIID*/)
{
    if (iUIID > DEF_UIIID)
    {
        iUIID--;
    }

    if (iUIID >= 0 && iUIID < HANDSET_NUMBER)
    {
        return m_arrayUserType[iUIID];
    }

    return UT_NONE;
}

// 设置当前用户权限
void CUserModeManager::SetUserType(USER_TYPE eType, int iUIID/* = DEF_UIIID*/)
{
    if (iUIID > DEF_UIIID)
    {
        iUIID--;
    }

    if (iUIID >= 0 && iUIID < HANDSET_NUMBER)
    {
        USER_TYPE eOldUser = m_arrayUserType[iUIID];
        m_arrayUserType[iUIID] = eType;
        etl_NotifyApp(FALSE, CONFIG_MSG_BCAST_CHANGED, ST_USER_INFO_CHANGED, 1);
    }
}

// 判断指定的项是否有可见的权限
bool CUserModeManager::IsItemHasShowAuth(const yl::string & strItemAction,
        int iUIID/* = DEF_UIIID*/)
{
    // 未开启三级权限
    if (!m_bEnable && !m_iLockLevel)
    {
        return true;
    }

    if (strItemAction.empty())
    {
        return true;
    }

    if (NULL == m_pUserModel)
    {
        return true;
    }

    if (iUIID > DEF_UIIID)
    {
        iUIID--;
    }

    if (iUIID < 0 || iUIID >= HANDSET_NUMBER)
    {
        return true;
    }

    int iShowLev = m_pUserModel->GetShowLevByAction(strItemAction.c_str());

    // 当前权限大于等于时才显示
    return (!m_bEnable && m_iLockLevel) ? UT_NONE >= iShowLev : m_arrayUserType[iUIID] >= iShowLev;
}

// 判断指定的项是否只读
bool CUserModeManager::IsItemReadOnly(const yl::string & strItemAction, int iUIID/* = DEF_UIIID*/)
{
    // 未开启三级权限
    if (!m_bEnable && !m_iLockLevel)
    {
        return false;
    }

    if (strItemAction.empty())
    {
        return false;
    }

    if (NULL == m_pUserModel)
    {
        return false;
    }

    if (iUIID > DEF_UIIID)
    {
        iUIID--;
    }

    if (iUIID < 0 || iUIID >= HANDSET_NUMBER)
    {
        return true;
    }

    int iWReadLev = m_pUserModel->GetWReadLevByAction(strItemAction.c_str());

    // 当前权限小于时只读
    bool bReadOnly = (!m_bEnable
                      && m_iLockLevel) ? UT_NONE < iWReadLev : m_arrayUserType[iUIID] < iWReadLev;

    // 特殊：11时var权限只读
    bReadOnly |= (m_bEnable && UT_VAR == m_arrayUserType[iUIID]
                  && m_pUserModel->IsDoubleLev1(strItemAction.c_str()));

    return bReadOnly;
}

// 根据权限获取账号名：用于密码匹配 / 显示
void CUserModeManager::GetUserNameByUserMode(USER_TYPE eType, yl::string & strUserName)
{
    switch (eType)
    {
    case UT_ADMIN:
        {
            strUserName = configParser_GetConfigString(kszUserNameAdmin);
            strUserName = strUserName.empty() ? "admin" : strUserName;
        }
        break;
    case UT_VAR:
        {
            strUserName = configParser_GetConfigString(kszUserNameVar);
            strUserName = strUserName.empty() ? "var" : strUserName;
        }
        break;
    case UT_USER:
        {
            if (configParser_GetConfigInt(kszUserEnable) == 1)
            {
                strUserName = configParser_GetConfigString(kszUserNameUser);
                strUserName = strUserName.empty() ? "user" : strUserName;
            }
        }
        break;
    default:
        break;
    }
}

//获取权限数据
bool CUserModeManager::GetUserModeData(UserModeListData & refData, int iUIID/* = DEF_UIIID*/)
{
    // 未开启三级权限
    if (!m_bEnable && !m_iLockLevel)
    {
        return false;
    }

    if (NULL == m_pUserModel)
    {
        return false;
    }

    if (iUIID > DEF_UIIID)
    {
        iUIID--;
    }

    if (iUIID < 0 || iUIID >= HANDSET_NUMBER)
    {
        return true;
    }

    MAP_PERMISSIONINFO & MapPermissionInfo =  m_pUserModel->GetMapPermissionInfo();

    refData.m_nTotalCount = MapPermissionInfo.size();

    int iCount = refData.m_nCountToGet;
    if (refData.m_nIndexFrom == 0 && refData.m_nCountToGet == 0)
    {
        iCount = refData.m_nTotalCount;
    }

    int iTmp = 0;

    for (ITER_PERMISSIONINFO itr = MapPermissionInfo.begin(); itr != MapPermissionInfo.end(); itr++)
    {
        if (iTmp++ < refData.m_nIndexFrom)
        {
            continue;
        }

        if (iCount-- <= 0)
        {
            break;
        }

        bool bShow = false;
        bool bReadOnly = false;

        // 当前权限大于等于时才显示
        bShow = (!m_bEnable
                 && m_iLockLevel) ? UT_NONE >= itr->second.m_iOpenShowLev : m_arrayUserType[iUIID] >=
                itr->second.m_iOpenShowLev;

        if (bShow)
        {
            // 当前权限小于时只读
            bReadOnly = (!m_bEnable
                         && m_iLockLevel) ? UT_NONE < itr->second.m_iMaxWReadLev : m_arrayUserType[iUIID] <
                        itr->second.m_iMaxWReadLev;

            // 特殊：11时var权限只读
            bReadOnly |= (m_bEnable && UT_VAR == m_arrayUserType[iUIID] && (1 == itr->second.m_iOpenShowLev
                          && 1 == itr->second.m_iOpenWReadLev));
        }

        UserModePermission sPermission;
        sPermission.SetData(itr->first, bShow, bReadOnly);

        refData.m_list.push_back(sPermission);
    }

    return true;
}

//获取权限数量
int CUserModeManager::GetUserModeCnt(int iUIID/* = DEF_UIIID*/)
{
    // 未开启三级权限
    if (!m_bEnable && !m_iLockLevel)
    {
        return 0;
    }

    if (NULL == m_pUserModel || iUIID < 1 || iUIID > HANDSET_NUMBER)
    {
        return 0;
    }

    return m_pUserModel->GetMapPermissionInfo().size();
}

// 获取默认的显示名
void CUserModeManager::GetDefaultUserName(USER_TYPE eType, yl::string & strUserName)
{
    switch (eType)
    {
    case UT_ADMIN:
        {
            strUserName = DEFAULTNAME_ADMIN;
        }
        break;
    case UT_VAR:
        {
            strUserName = DEFAULTNAME_VAR;
        }
        break;
    case UT_USER:
        {
            strUserName = DEFAULTNAME_USER;
        }
        break;
    default:
        break;
    }
}

