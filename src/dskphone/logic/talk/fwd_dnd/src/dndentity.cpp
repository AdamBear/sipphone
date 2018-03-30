#include "dndentity.h"

#include <dsklog/log.h>
#include <etlmsghandler/modetlmsghandler.h> // etl_NotifyApp
#include <configiddefine.h>
#include <modconfigparser.h>

#include "fwddndmsgdefine.h"


CDndEntity::CDndEntity(int iAccount)
    : m_iAccount(iAccount)
{
    m_bEnableBackup = m_bEnable = IsEnableInConfig();
    if (m_bEnable)
    {
        NotifyChange(DND_CFG_SRC_TYPE_STARTUP);
    }
}

CDndEntity::~CDndEntity()
{
    if (m_bEnable)
    {
        m_bEnable = false;
        NotifyChange(DND_CFG_SRC_TYPE_SHUTDOWN);
    }
}

void CDndEntity::ReloadConfig()
{
    bool bEnable = IsEnableInConfig();
    if (m_bEnable != bEnable)
    {
        m_bEnable = bEnable;
        NotifyChange(DND_CFG_SRC_TYPE_COMMON);
    }
}

int CDndEntity::GetAccountId() const
{
    return m_iAccount;
}

bool CDndEntity::IsEnable() const
{
    return m_bEnable;
}

bool CDndEntity::SetEnable(bool bEnable, DND_CONFIG_SOURCE_TYPE eSource)
{
    if (bEnable == m_bEnable)
    {
        return true;
    }
    m_bEnable = bEnable;
    // 写入配置
    bool bResult = false;
    CONFIG_LEVEL_TYPE eLevel = CONFIG_LEVEL_IGNORE;

    if (DND_CFG_SRC_TYPE_COMMON == eSource)
    {
        eLevel = CONFIG_LEVEL_PHONE;
    }

    if (m_iAccount == FWDDND_ACCOUNT_GLOBAL)
    {
        // 【配置】kszDndStatus代表全局模式DND是否启用
        bResult = configParser_SetConfigInt(kszDndStatus, m_bEnable, eLevel);
    }
    else
    {
        // 【配置】kszDndStatus代表账号模式DND是否启用
        bResult = configParser_SetCfgItemIntValue(kszAccountDNDStatus, m_iAccount, m_bEnable, eLevel);
    }
    // 通知状态改变
    NotifyChange(eSource);
    return bResult;
}

void CDndEntity::Backup()
{
    m_bEnableBackup = m_bEnable;
}

void CDndEntity::Restore()
{
    SetEnable(m_bEnableBackup, DND_CFG_SRC_TYPE_COMMON);
}

bool CDndEntity::IsEnableInConfig() const
{
    if (m_iAccount == FWDDND_ACCOUNT_GLOBAL)
    {
        // 【配置】kszDndStatus代表全局模式DND是否启用
        return configParser_GetConfigInt(kszDndStatus);
    }
    else
    {
        // 【配置】kszDndStatus代表账号模式DND是否启用
        return configParser_GetCfgItemIntValue(kszAccountDNDStatus, m_iAccount);
    }
}

void CDndEntity::NotifyChange(DND_CONFIG_SOURCE_TYPE eSource)
{
    FWDDND_INFO("Notify status change of DND #%d: %d, source=%d", m_iAccount, m_bEnable, eSource);
    etl_NotifyAppEx(false, DND_MESSAGE_STATUS_CHANGED, m_iAccount, eSource,
                    sizeof(m_bEnable), &m_bEnable);
}
