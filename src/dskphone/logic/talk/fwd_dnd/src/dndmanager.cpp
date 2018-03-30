#include "dndmanager.h"

#include <dsklog/log.h>
#include <configiddefine.h>
#include <modconfigparser.h>
#include <account/include/modaccount.h> // acc_GetAccountList
#include <etlmsghandler/modetlmsghandler.h> // SingleMsgReg SingleMsgUnReg


IMPLEMENT_GETINSTANCE(CDndManager)

CDndManager::CDndManager()
    : m_eMode(FWDDND_MODE_GLOBAL)
{
    m_unionEntities.pVoid = NULL;
}

CDndManager::~CDndManager()
{
}

void CDndManager::Init()
{
    // 订阅消息
    SingleMsgReg(CONFIG_MSG_BCAST_CHANGED, CDndManager::OnMessage); // 订阅配置改变消息
    // 加载配置
    if (IsActive())
    {
        // 当前DND功能开关启用，需要创建实体
        m_eMode = GetModeFromConfig();
        CreateEntities();
    }
}

void CDndManager::Finalize()
{
    if (IsActive())
    {
        ReleaseEntities();
    }
    // 取消消息订阅
    SingleMsgUnReg(CONFIG_MSG_BCAST_CHANGED, CDndManager::OnMessage); // 取消配置改变消息订阅
}

bool CDndManager::IsActive() const
{
    // 【配置】kszDndForbidden对应DND功能开关
    return configParser_GetConfigInt(kszDndForbidden) == 0;
}

FWDDND_MODE CDndManager::GetMode() const
{
    return m_eMode;
}

void CDndManager::SetMode(FWDDND_MODE eMode)
{
    // DND功能未启用则直接返回
    if (!IsActive())
    {
        return;
    }
    // 写入配置：【配置】kszDndMode对应DND模式，0=全局模式，1=账号模式
    configParser_SetConfigInt(kszDndMode, eMode);
    // 处理模式切换（从配置中更新m_eMode）
    OnModeChanged();
}

bool CDndManager::IsEnable(int iAccount) const
{
    // DND功能未启用则直接返回
    if (!IsActive())
    {
        return false;
    }
    CDndEntity * pEntity = GetEntity(iAccount);
    return pEntity && pEntity->IsEnable();
}

bool CDndManager::SetEnable(int iAccount, bool bEnable, DND_CONFIG_SOURCE_TYPE eSource)
{
    // DND功能未启用则直接返回
    if (!IsActive())
    {
        return false;
    }
    CDndEntity * pEntity = GetEntity(iAccount);
    return pEntity && pEntity->SetEnable(bEnable, eSource);
}

void CDndManager::Backup()
{
    FWDDND_INFO("CDndManager::Backup");
    // 当前没有实体则直接返回
    if (m_unionEntities.pVoid == NULL)
    {
        return;
    }
    // 判断当前模式
    if (FWDDND_MODE_GLOBAL == m_eMode)
    {
        m_unionEntities.pGlobalEntity->Backup();
    }
    else // FWDDND_MODE_ACCOUNT
    {
        for (YLVector<CDndEntity *>::iterator it = m_unionEntities.pAccountEntityList->begin();
                it != m_unionEntities.pAccountEntityList->end(); ++it)
        {
            (*it)->Backup();
        }
    }
}

void CDndManager::Restore()
{
    FWDDND_INFO("CDndManager::Restore");
    // 当前没有实体则直接返回
    if (m_unionEntities.pVoid == NULL)
    {
        return;
    }
    // 判断当前模式
    if (FWDDND_MODE_GLOBAL == m_eMode)
    {
        m_unionEntities.pGlobalEntity->Restore();
    }
    else // FWDDND_MODE_ACCOUNT
    {
        for (YLVector<CDndEntity *>::iterator it = m_unionEntities.pAccountEntityList->begin();
                it != m_unionEntities.pAccountEntityList->end(); ++it)
        {
            (*it)->Restore();
        }
    }
}

FWDDND_MODE CDndManager::GetModeFromConfig() const
{
    // 【配置】kszDndMode对应DND模式，0=全局模式，1=账号模式
    return (FWDDND_MODE)configParser_GetConfigInt(kszDndMode);
}

void CDndManager::CreateEntities()
{
    if (FWDDND_MODE_GLOBAL == m_eMode)
    {
        // 全局模式：创建全局DND实体
        m_unionEntities.pGlobalEntity = new CDndEntity(FWDDND_ACCOUNT_GLOBAL);
    }
    else // FWDDND_MODE_ACCOUNT
    {
        // 账号模式：创建DND实体列表
        m_unionEntities.pAccountEntityList = new YLVector<CDndEntity *>();
        if (m_unionEntities.pAccountEntityList)
        {
            ListAccounts listAccounts = acc_GetAccountList(ACCOUNT_TYPE_SIP);
            for (ListAccounts::iterator it = listAccounts.begin(); it != listAccounts.end(); ++it)
            {
                m_unionEntities.pAccountEntityList->push_back(new CDndEntity(*it));
            }
        }
    }
}

void CDndManager::ReleaseEntities()
{
    if (m_unionEntities.pVoid)
    {
        if (FWDDND_MODE_GLOBAL == m_eMode)
        {
            delete m_unionEntities.pGlobalEntity;
        }
        else
        {
            for (YLVector<CDndEntity *>::iterator it = m_unionEntities.pAccountEntityList->begin();
                    it != m_unionEntities.pAccountEntityList->end(); ++it)
            {
                delete *it;
            }
            delete m_unionEntities.pAccountEntityList;
        }
    }
    m_unionEntities.pVoid = NULL;
}

CDndEntity * CDndManager::GetEntity(int iAccount) const
{
    FWDDND_INFO("CDndManager::GetEntity(%d) mode=%d", iAccount, m_eMode);
    // 当前没有实体则直接返回NULL
    if (m_unionEntities.pVoid == NULL)
    {
        return NULL;
    }
    // 判断当前模式
    if (FWDDND_MODE_GLOBAL == m_eMode)
    {
        if (FWDDND_ACCOUNT_GLOBAL != iAccount)
        {
            FWDDND_WARN("DND is global mode currently, account #%d should be FWDDND_ACCOUNT_GLOBAL.", iAccount);
        }
        return m_unionEntities.pGlobalEntity;
    }
    else // FWDDND_MODE_ACCOUNT
    {
        for (YLVector<CDndEntity *>::iterator it = m_unionEntities.pAccountEntityList->begin();
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

LRESULT CDndManager::OnMessage(msgObject & msg)
{
    switch (msg.message)
    {
    case CONFIG_MSG_BCAST_CHANGED:
        if (msg.wParam == ST_DND)
        {
            // kszDndMode配置变更：判断是否需要切换模式
            _DndManager.OnModeChanged();
            // kszDndStatus/kszAccountDNDStatus配置变更：由实体处理
            _DndManager.NotifyEntitiesConfigChanged();
            return TRUE;
        }
        else if (msg.wParam == ST_ADV_SETTING)
        {
            // kszDndForbidden配置变更
            _DndManager.OnActiveStatusChanged();
            return TRUE;
        }
        break;
    default:
        break;
    }
    return FALSE;
}

void CDndManager::OnActiveStatusChanged()
{
    // 原先DND功能开关是否启用：有没有分配实体
    bool bOldActiveStatus = m_unionEntities.pVoid != NULL;
    // 当前DND功能开关是否启用：读取配置
    bool bNewActiveStatus = IsActive();
    // DND功能开关没有变化，则不进行任何处理
    if (bOldActiveStatus == bNewActiveStatus)
    {
        return;
    }
    if (bOldActiveStatus)
    {
        // 原先DND功能开关启用，需要释放实体
        ReleaseEntities();
    }
    if (bNewActiveStatus)
    {
        // 当前DND功能开关启用，需要创建实体
        m_eMode = GetModeFromConfig();
        CreateEntities();
    }
}

void CDndManager::OnModeChanged()
{
    // DND功能开关禁用则忽略
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

void CDndManager::NotifyEntitiesConfigChanged()
{
    // DND功能开关禁用则忽略
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
            for (YLVector<CDndEntity *>::iterator it = m_unionEntities.pAccountEntityList->begin();
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
