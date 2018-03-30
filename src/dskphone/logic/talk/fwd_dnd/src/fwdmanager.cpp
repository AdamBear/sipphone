#include "fwdmanager.h"

#include <dsklog/log.h>
#include <configiddefine.h>
#include <modconfigparser.h>
#include <account/include/modaccount.h> // acc_GetAccountList
#include <etlmsghandler/modetlmsghandler.h> // SingleMsgReg SingleMsgUnReg

IMPLEMENT_GETINSTANCE(CFwdManager)

CFwdManager::CFwdManager()
    : m_eMode(FWDDND_MODE_GLOBAL)
{
    m_unionEntities.pVoid = NULL;
}

CFwdManager::~CFwdManager()
{
}

void CFwdManager::Init()
{
    // 订阅消息
    SingleMsgReg(CONFIG_MSG_BCAST_CHANGED, CFwdManager::OnMessage); // 订阅配置改变消息
    // 加载配置
    if (IsActive())
    {
        // 当前FWD功能开关启用，需要创建实体
        m_eMode = GetModeFromConfig();
        CreateEntities();
    }
}

void CFwdManager::Finalize()
{
    if (IsActive())
    {
        ReleaseEntities();
    }
    // 取消消息订阅
    SingleMsgUnReg(CONFIG_MSG_BCAST_CHANGED, CFwdManager::OnMessage); // 取消配置改变消息订阅
}

bool CFwdManager::IsActive() const
{
    // 【配置】kszForwardForbidden对应FWD功能开关
    return configParser_GetConfigInt(kszForwardForbidden) == 0;
}

FWDDND_MODE CFwdManager::GetMode() const
{
    return m_eMode;
}

void CFwdManager::SetMode(FWDDND_MODE eMode)
{
    // FWD功能未启用则直接返回
    if (!IsActive())
    {
        return;
    }
    // 写入配置：【配置】kszForwardMode对应FWD模式，0=全局模式，1=账号模式
    configParser_SetConfigInt(kszForwardMode, eMode);
    // 处理模式切换（从配置中更新m_eMode）
    OnModeChanged();
}

bool CFwdManager::GetInfo(int iAccount, FWD_TYPE eType,
                          yl::string * pstrTarget/* = NULL */, int * pRingDelay/* = NULL */) const
{
    // FWD功能未启用则直接返回
    if (!IsActive())
    {
        return false;
    }
    CFwdEntity * pEntity = GetEntity(iAccount);
    return pEntity && pEntity->GetInfo(eType, pstrTarget, pRingDelay);
}

bool CFwdManager::SetInfo(int iAccount, FWD_TYPE eType, bool bEnabled,
                          const yl::string * pstrTarget/* = NULL */, int * pRingDelay/* = NULL */,
                          FWD_CONFIG_SOURCE_TYPE eSource/* = FWD_CFG_SRC_TYPE_COMMON */)
{
    // FWD功能未启用则直接返回
    if (!IsActive())
    {
        return false;
    }
    CFwdEntity * pEntity = GetEntity(iAccount);
    return pEntity && pEntity->SetInfo(eType, bEnabled, pstrTarget,
                                       pRingDelay, eSource);
}

bool CFwdManager::SetMultiInfo(int iAccount, int eType, bool bEnabled,
                               FWD_CONFIG_SOURCE_TYPE eSource/* = FWD_CFG_SRC_TYPE_COMMON */)
{
    // FWD功能未启用则直接返回
    if (!IsActive())
    {
        return false;
    }
    CFwdEntity * pEntity = GetEntity(iAccount);
    return pEntity && pEntity->SetMultiInfo(eType, bEnabled, eSource);
}

bool CFwdManager::RestoreInfo(int iAccount, FWD_TYPE eType)
{
    // FWD功能未启用则直接返回
    if (!IsActive())
    {
        return false;
    }
    CFwdEntity * pEntity = GetEntity(iAccount);
    return pEntity && pEntity->RestoreInfo(eType);
}

bool CFwdManager::IsAllowForwardTo(const yl::string & strTarget)
{
    // 此接口与FWD功能开关是否开启无关
    // 【配置】kszForwardInternationalEnable对应Forward International功能开关
#ifdef IF_FEATURE_FWD_INTERNATIONAL
    bool bFwdIntEnable = configParser_GetConfigInt(kszForwardInternationalEnable);
#else
    bool bFwdIntEnable = false;
#endif
    if (bFwdIntEnable)
    {
        // 如果启用Forward International功能，则允许任何号码转移
        return true;
    }
    // 【配置】kszForwardInternationalNumber对应国际长途前缀，默认值为“00”
    yl::string strFwdIntPrefix = configParser_GetConfigString(kszForwardInternationalNumber);
    if (strFwdIntPrefix.empty())
    {
        // 如果国际长途前缀为空，则允许任何号码转移
        return true;
    }
    if (strTarget.size() <= strFwdIntPrefix.size())
    {
        // 目标号码长度小于或等于国际长途前缀，则目标号码不是国际长途号码
        return true;
    }
    if (strTarget.compare(0, strFwdIntPrefix.size(), strFwdIntPrefix) == 0)
    {
        // 目标号码前缀符合国际长途前缀，则禁止转移
        FWDDND_WARN("Not allowed forwarding to international number: %s", strTarget.c_str());
        return false;
    }
    return true;
}

int CFwdManager::GetLastSavedType() const
{

    if (IsActive() // FWD功能已启用
            && GetMode() == FWDDND_MODE_GLOBAL) // 当前为全局模式
    {
        CFwdEntity * pEntity = GetEntity(FWDDND_ACCOUNT_GLOBAL);
        if (pEntity)
        {
            return pEntity->GetLastSavedType();
        }
    }
    return 0;
}

FWDDND_MODE CFwdManager::GetModeFromConfig() const
{
    // 【配置】kszForwardMode对应FWD模式，0=全局模式，1=账号模式
    return (FWDDND_MODE)configParser_GetConfigInt(kszForwardMode);
}

void CFwdManager::CreateEntities()
{
    if (FWDDND_MODE_GLOBAL == m_eMode)
    {
        // 全局模式：创建全局FWD实体
        m_unionEntities.pGlobalEntity = new CFwdEntity(FWDDND_ACCOUNT_GLOBAL);
    }
    else // FWDDND_MODE_ACCOUNT
    {
        // 账号模式：创建FWD实体列表
        m_unionEntities.pAccountEntityList = new YLVector<CFwdEntity *>();
        if (m_unionEntities.pAccountEntityList)
        {
            ListAccounts listAccounts = acc_GetAccountList(ACCOUNT_TYPE_SIP);
            for (ListAccounts::iterator it = listAccounts.begin(); it != listAccounts.end(); ++it)
            {
                m_unionEntities.pAccountEntityList->push_back(new CFwdEntity(*it));
            }
        }
    }
}

void CFwdManager::ReleaseEntities()
{
    if (m_unionEntities.pVoid)
    {
        if (FWDDND_MODE_GLOBAL == m_eMode)
        {
            delete m_unionEntities.pGlobalEntity;
        }
        else
        {
            for (YLVector<CFwdEntity *>::iterator it = m_unionEntities.pAccountEntityList->begin();
                    it != m_unionEntities.pAccountEntityList->end(); ++it)
            {
                delete *it;
            }
            delete m_unionEntities.pAccountEntityList;
        }
    }
    m_unionEntities.pVoid = NULL;
}

CFwdEntity * CFwdManager::GetEntity(int iAccount) const
{
    FWDDND_INFO("CFwdManager::GetEntity(%d) mode=%d", iAccount, m_eMode);
    // 当前没有实体则直接返回NULL
    if (m_unionEntities.pVoid == NULL)
    {
        return NULL;
    }

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    // flexible seating禁用来电时的Forward
    if (FWDDND_ACCOUNT_GLOBAL != iAccount
            && acc_GetFlexibleSeatingSwitch(iAccount))
    {
        return NULL;
    }
#endif

    // 判断当前模式
    if (FWDDND_MODE_GLOBAL == m_eMode)
    {
        if (FWDDND_ACCOUNT_GLOBAL != iAccount)
        {
            FWDDND_WARN("FWD is global mode currently, account #%d should be FWDDND_ACCOUNT_GLOBAL.", iAccount);
        }
        return m_unionEntities.pGlobalEntity;
    }
    else // FWDDND_MODE_ACCOUNT
    {
        for (YLVector<CFwdEntity *>::iterator it = m_unionEntities.pAccountEntityList->begin();
                it != m_unionEntities.pAccountEntityList->end(); ++it)
        {
            if ((*it)->GetAccountId() == iAccount)
            {
                return *it;
            }
        }
    }
    return NULL;
}

LRESULT CFwdManager::OnMessage(msgObject & msg)
{
    switch (msg.message)
    {
    case CONFIG_MSG_BCAST_CHANGED:
        if (msg.wParam == ST_FORWARD)
        {
            // kszForwardMode配置变更：判断是否需要切换模式
            _FwdManager.OnModeChanged();
            // 其他配置变更：由实体处理
            _FwdManager.NotifyEntitiesConfigChanged();
            return TRUE;
        }
        else if (msg.wParam == ST_ADV_SETTING)
        {
            // kszForwardForbidden配置变更：判断是否需要启用/禁用FWD功能
            _FwdManager.OnActiveStatusChanged();
            return TRUE;
        }
        break;
    default:
        break;
    }
    return FALSE;
}

void CFwdManager::OnActiveStatusChanged()
{
    // 原先FWD功能开关是否启用：有没有分配实体
    bool bOldActiveStatus = m_unionEntities.pVoid != NULL;
    // 当前FWD功能开关是否启用：读取配置
    bool bNewActiveStatus = IsActive();
    // FWD功能开关没有变化，则不进行任何处理
    if (bOldActiveStatus == bNewActiveStatus)
    {
        return;
    }
    if (bOldActiveStatus)
    {
        // 原先FWD功能开关启用，需要释放实体
        ReleaseEntities();
    }
    if (bNewActiveStatus)
    {
        // 当前FWD功能开关启用，需要创建实体
        m_eMode = GetModeFromConfig();
        CreateEntities();
    }
}

void CFwdManager::OnModeChanged()
{
    // FWD功能开关禁用则忽略
    if (!IsActive())
    {
        return;
    }
    FWDDND_MODE eNewMode = GetModeFromConfig();
    // 模式没有变化则不处理
    if (eNewMode == m_eMode)
    {
        return;
    }
    // 模式改变，则释放原有模式，重新加载新模式
    ReleaseEntities();
    m_eMode = eNewMode;
    CreateEntities();
}

void CFwdManager::NotifyEntitiesConfigChanged()
{
    // FWD功能开关禁用则忽略
    if (!IsActive())
    {
        return;
    }
    if (m_unionEntities.pVoid == NULL)
    {
        FWDDND_WARN("Pointer to entities is NULL!");
        return;
    }
    // 根据模式通知不同实体
    if (FWDDND_MODE_GLOBAL == m_eMode)
    {
        m_unionEntities.pGlobalEntity->ReloadConfig();
    }
    else // FWDDND_MODE_ACCOUNT
    {
        if (m_unionEntities.pAccountEntityList)
        {
            for (YLVector<CFwdEntity *>::iterator it = m_unionEntities.pAccountEntityList->begin();
                    it != m_unionEntities.pAccountEntityList->end(); ++it)
            {
                if (*it)
                {
                    (*it)->ReloadConfig();
                }
                else
                {
                    FWDDND_WARN("Pointer to account entity is NULL!");
                }
            }
        }
    }
}
