#include "fwdentity.h"

#include <dsklog/log.h>
#include <etlmsghandler/modetlmsghandler.h> // etl_NotifyApp
#include <configiddefine.h>
#include <modconfigparser.h>
#include <popupboxmsgdefine.h> // MULTIFWD_ENABLED_NOTICE

#include "fwddndmsgdefine.h"


CFwdEntity::CFwdEntity(int iAccount)
    : m_iAccount(iAccount)
{
    // 直接从配置中加载
    m_iRingDelay = GetRingDelayInConfig(); // 应该先加载RingDelay再调用NotifyChange
    FWD_TYPE arrTypes[] = { FWD_TYPE_ALWAYS, FWD_TYPE_BUSY, FWD_TYPE_NO_ANSWER };
    for (int i = sizeof(arrTypes) / sizeof(arrTypes[0]) - 1; i >= 0; --i)
    {
        SetEnable(arrTypes[i], IsEnableInConfig(arrTypes[i]));
        SetTarget(arrTypes[i], GetTargetInConfig(arrTypes[i]));
        if (IsEnable(arrTypes[i]))
        {
            NotifyChange(arrTypes[i], FWD_CFG_SRC_TYPE_STARTUP);
        }
    }
}

CFwdEntity::~CFwdEntity()
{
    // 切换模式（全局/账号）时会析构原有模式的实体，因此原有实体为启用状态时，需要发送禁用消息
    FWD_TYPE arrTypes[] = { FWD_TYPE_ALWAYS, FWD_TYPE_BUSY, FWD_TYPE_NO_ANSWER };
    for (int i = sizeof(arrTypes) / sizeof(arrTypes[0]) - 1; i >= 0; --i)
    {
        if (IsEnable(arrTypes[i]))
        {
            SetEnable(arrTypes[i], false);
            NotifyChange(arrTypes[i], FWD_CFG_SRC_TYPE_SHUTDOWN);
        }
    }
}

void CFwdEntity::ReloadConfig()
{
    FWD_TYPE arrTypes[] = { FWD_TYPE_ALWAYS, FWD_TYPE_BUSY, FWD_TYPE_NO_ANSWER };
    for (int i = sizeof(arrTypes) / sizeof(arrTypes[0]) - 1; i >= 0; --i)
    {
        bool bConfigChanged = false;
        // 从配置加载当前状态
        bConfigChanged |= (IsEnable(arrTypes[i]) != IsEnableInConfig(arrTypes[i]));
        SetEnable(arrTypes[i], IsEnableInConfig(arrTypes[i]));
        bConfigChanged |= (GetTarget(arrTypes[i]) != GetTargetInConfig(arrTypes[i]));
        SetTarget(arrTypes[i], GetTargetInConfig(arrTypes[i]));
        if (arrTypes[i] == FWD_TYPE_NO_ANSWER)
        {
            bConfigChanged |= (m_iRingDelay != GetRingDelayInConfig());
            m_iRingDelay = GetRingDelayInConfig();
        }
        // 如果当前状态改变，则发送消息
        if (bConfigChanged)
        {
            NotifyChange(arrTypes[i], FWD_CFG_SRC_TYPE_COMMON);
            BackupInfo(arrTypes[i]); // 外部修改配置也需要备份
        }
    }
    // 设置最后一次修改配置时启用的FWD类型
    SetLastSavedType();
}

int CFwdEntity::GetAccountId() const
{
    return m_iAccount;
}

bool CFwdEntity::GetInfo(FWD_TYPE eType, yl::string * pstrTarget/* = NULL */,
                         int * pRingDelay/* = NULL */) const
{
    if (eType == FWD_TYPE_MANUAL)
    {
        FWDDND_WARN("Don't get forward info of manual type!");
        return true;
    }
    if (pstrTarget)
    {
        if (FWD_TYPE_ALL == eType)
        {
            FWDDND_WARN("Don't get forward target of all type!");
        }
        else
        {
            *pstrTarget = GetTarget(eType);
        }
    }
    if (FWD_TYPE_NO_ANSWER == eType && pRingDelay)
    {
        *pRingDelay = m_iRingDelay;
    }
    return IsEnable(eType);
}

bool CFwdEntity::SetInfo(FWD_TYPE eType, bool bEnabled,
                         const yl::string * pstrTarget/* = NULL */, int * pRingDelay/* = NULL */,
                         FWD_CONFIG_SOURCE_TYPE eSource/* = FWD_CFG_SRC_TYPE_COMMON */,
                         bool bSetLastSavedType/* = true */)
{
    bool bInfoChanged = false;
    // 更新变量
    bInfoChanged |= (IsEnable(eType) != bEnabled);
    SetEnable(eType, bEnabled);
    if (pstrTarget)
    {
        bInfoChanged |= (GetTarget(eType) != *pstrTarget);
        SetTarget(eType, *pstrTarget);
    }
    if (eType == FWD_TYPE_NO_ANSWER && pRingDelay)
    {
        bInfoChanged |= (m_iRingDelay != *pRingDelay);
        m_iRingDelay = *pRingDelay;
    }
    // 判断是否发生变化，未变化则不处理
    if (!bInfoChanged)
    {
        FWDDND_INFO("FWD info not changed.");
        return true;
    }
    SaveInfo(eType, eSource);
    // 判断eSource是否为DSSKEY，如果不是则需要存入备份配置
    if (eSource != FWD_CFG_SRC_TYPE_DSSKEY)
    {
        BackupInfo(eType);
    }
    if (bSetLastSavedType)
    {
        // 设置最后一次修改配置时启用的FWD类型
        SetLastSavedType();
    }
    // 发送状态改变消息
    NotifyChange(eType, eSource);
    return true;
}

bool CFwdEntity::SetMultiInfo(int eType, bool bEnabled,
                              FWD_CONFIG_SOURCE_TYPE eSource/* = FWD_CFG_SRC_TYPE_COMMON */)
{
    const FWD_TYPE arrTypes[] = { FWD_TYPE_ALWAYS, FWD_TYPE_BUSY, FWD_TYPE_NO_ANSWER };
    for (int i = 0; i < sizeof(arrTypes) / sizeof(arrTypes[0]); ++i)
    {
        if (arrTypes[i] & eType)
        {
            SetInfo(arrTypes[i], false, NULL, NULL, eSource, false);
        }
    }
    // 设置最后一次修改配置时启用的FWD类型
    SetLastSavedType();
    return true;
}

bool CFwdEntity::RestoreInfo(FWD_TYPE eType)
{
    const FWD_TYPE arrTypes[] = { FWD_TYPE_ALWAYS, FWD_TYPE_BUSY, FWD_TYPE_NO_ANSWER };
    for (int i = 0; i < sizeof(arrTypes) / sizeof(arrTypes[0]); ++i)
    {
        if (arrTypes[i] & eType)
        {
            if (arrTypes[i] == FWD_TYPE_ALWAYS && m_iAccount == FWDDND_ACCOUNT_GLOBAL)
            {
                // 从备份配置还原
                yl::string strTarget = configParser_GetConfigString(kszForwardAlwaysTargetBackup);
                SetInfo(arrTypes[i], false, &strTarget, NULL, FWD_CFG_SRC_TYPE_DSSKEY, false);
            }
            else
            {
                SetInfo(arrTypes[i], false, NULL, NULL, FWD_CFG_SRC_TYPE_DSSKEY, false);
            }
        }
    }
    // 设置最后一次修改配置时启用的FWD类型
    SetLastSavedType();
    return true;
}

int CFwdEntity::GetLastSavedType() const
{
    if (m_iAccount == FWDDND_ACCOUNT_GLOBAL) // 全局模式
    {
        return configParser_GetConfigInt(kszForwardTypeLastSaved);
    }
    else
    {
        return 0;
    }
}

bool CFwdEntity::IsEnable(FWD_TYPE eType) const
{
    switch (eType)
    {
    case FWD_TYPE_ALWAYS:
        return m_bAlwaysFwdEnable;
    case FWD_TYPE_BUSY:
        return m_bBusyFwdEnable;
    case FWD_TYPE_NO_ANSWER:
        return m_bNoAnswerFwdEnable;
    case FWD_TYPE_ALL:
        return m_bAlwaysFwdEnable || m_bBusyFwdEnable || m_bNoAnswerFwdEnable;
    default:
        return false;
    }
}

void CFwdEntity::SetEnable(FWD_TYPE eType, bool bEnable)
{
    switch (eType)
    {
    case FWD_TYPE_ALWAYS:
        m_bAlwaysFwdEnable = bEnable;
        break;
    case FWD_TYPE_BUSY:
        m_bBusyFwdEnable = bEnable;
        break;
    case FWD_TYPE_NO_ANSWER:
        m_bNoAnswerFwdEnable = bEnable;
        break;
    default:
        break;
    }
}

const yl::string CFwdEntity::GetTarget(FWD_TYPE eType) const
{
    switch (eType)
    {
    case FWD_TYPE_ALWAYS:
        return m_strAlwaysFwdTarget;
    case FWD_TYPE_BUSY:
        return m_strBusyFwdTarget;
    case FWD_TYPE_NO_ANSWER:
        return m_strNoAnswerFwdTarget;
    default:
        return yl::string();
    }
}

void CFwdEntity::SetTarget(FWD_TYPE eType, const yl::string & strTarget)
{
    switch (eType)
    {
    case FWD_TYPE_ALWAYS:
        m_strAlwaysFwdTarget = strTarget;
        break;
    case FWD_TYPE_BUSY:
        m_strBusyFwdTarget = strTarget;
        break;
    case FWD_TYPE_NO_ANSWER:
        m_strNoAnswerFwdTarget = strTarget;
        break;
    default:
        break;
    }
}

bool CFwdEntity::IsEnableInConfig(FWD_TYPE eType) const
{
    if (m_iAccount == FWDDND_ACCOUNT_GLOBAL)
    {
        // 全局模式
        switch (eType)
        {
        case FWD_TYPE_ALWAYS:
            return configParser_GetConfigInt(kszForwardAlwaysSwitch);
            break;
        case FWD_TYPE_BUSY:
            return configParser_GetConfigInt(kszForwardBusySwitch);
            break;
        case FWD_TYPE_NO_ANSWER:
            return configParser_GetConfigInt(kszForwardNoAnswerSwitch);
            break;
        default:
            break;
        }
    }
    else
    {
        // 账号模式
        switch (eType)
        {
        case FWD_TYPE_ALWAYS:
            return configParser_GetCfgItemIntValue(kszAccountAlwaysFWDStatus, m_iAccount);
            break;
        case FWD_TYPE_BUSY:
            return configParser_GetCfgItemIntValue(kszAccountBusyFWDStatus, m_iAccount);
            break;
        case FWD_TYPE_NO_ANSWER:
            return configParser_GetCfgItemIntValue(kszAccountNoASWFWDStatus, m_iAccount);
            break;
        default:
            break;
        }
    }
    return false;
}

yl::string CFwdEntity::GetTargetInConfig(FWD_TYPE eType) const
{
    if (m_iAccount == FWDDND_ACCOUNT_GLOBAL) // 全局模式
    {
        switch (eType)
        {
        case FWD_TYPE_ALWAYS:
            return configParser_GetConfigString(kszForwardAlwaysTarget);
            break;
        case FWD_TYPE_BUSY:
            return configParser_GetConfigString(kszForwardBusyTarget);
            break;
        case FWD_TYPE_NO_ANSWER:
            return configParser_GetConfigString(kszForwardNoAnswerTarget);
            break;
        default:
            break;
        }
    }
    else // 账号模式
    {
        switch (eType)
        {
        case FWD_TYPE_ALWAYS:
            return configParser_GetCfgItemStringValue(kszAccountAlwaysFWDTarget, m_iAccount);
            break;
        case FWD_TYPE_BUSY:
            return configParser_GetCfgItemStringValue(kszAccountBusyFWDTarget, m_iAccount);
            break;
        case FWD_TYPE_NO_ANSWER:
            return configParser_GetCfgItemStringValue(kszAccountNoASWFWDTarget, m_iAccount);
            break;
        default:
            break;
        }
    }
    return yl::string();
}

int CFwdEntity::GetRingDelayInConfig() const
{
    if (m_iAccount == FWDDND_ACCOUNT_GLOBAL)
    {
        return configParser_GetConfigInt(kszForwardNoAnswerDelayTime);
    }
    else
    {
        return configParser_GetCfgItemIntValue(kszAccountNoASWFWDTimeout, m_iAccount);
    }
}

void CFwdEntity::SaveInfo(FWD_TYPE eType, FWD_CONFIG_SOURCE_TYPE eSource) const
{
    CONFIG_LEVEL_TYPE eLevel = CONFIG_LEVEL_IGNORE;

    if (FWD_CFG_SRC_TYPE_DSSKEY == eSource
            || FWD_CFG_SRC_TYPE_COMMON == eSource)
    {
        eLevel = CONFIG_LEVEL_PHONE;
    }

    // 存入配置文件
    if (m_iAccount == FWDDND_ACCOUNT_GLOBAL) // 全局模式
    {
        switch (eType)
        {
        case FWD_TYPE_ALWAYS:
            configParser_SetConfigInt(kszForwardAlwaysSwitch, IsEnable(eType), eLevel);
            configParser_SetConfigString(kszForwardAlwaysTarget, GetTarget(eType).c_str(), eLevel);
            break;
        case FWD_TYPE_BUSY:
            configParser_SetConfigInt(kszForwardBusySwitch, IsEnable(eType), eLevel);
            configParser_SetConfigString(kszForwardBusyTarget, GetTarget(eType).c_str(), eLevel);
            break;
        case FWD_TYPE_NO_ANSWER:
            configParser_SetConfigInt(kszForwardNoAnswerSwitch, IsEnable(eType), eLevel);
            configParser_SetConfigString(kszForwardNoAnswerTarget, GetTarget(eType).c_str(), eLevel);
            configParser_SetConfigInt(kszForwardNoAnswerDelayTime, m_iRingDelay, eLevel);
            break;
        default:
            break;
        }
    }
    else // 账号模式
    {
        switch (eType)
        {
        case FWD_TYPE_ALWAYS:
            configParser_SetCfgItemIntValue(kszAccountAlwaysFWDStatus,
                                            m_iAccount, IsEnable(eType), eLevel);
            configParser_SetCfgItemStringValue(kszAccountAlwaysFWDTarget,
                                               m_iAccount, GetTarget(eType).c_str(), eLevel);
            break;
        case FWD_TYPE_BUSY:
            configParser_SetCfgItemIntValue(kszAccountBusyFWDStatus,
                                            m_iAccount, IsEnable(eType), eLevel);
            configParser_SetCfgItemStringValue(kszAccountBusyFWDTarget,
                                               m_iAccount, GetTarget(eType).c_str(), eLevel);
            break;
        case FWD_TYPE_NO_ANSWER:
            configParser_SetCfgItemIntValue(kszAccountNoASWFWDStatus,
                                            m_iAccount, IsEnable(eType), eLevel);
            configParser_SetCfgItemStringValue(kszAccountNoASWFWDTarget,
                                               m_iAccount, GetTarget(eType).c_str(), eLevel);
            configParser_SetCfgItemIntValue(kszAccountNoASWFWDTimeout,
                                            m_iAccount, m_iRingDelay, eLevel);
            break;
        default:
            break;
        }
    }
}

void CFwdEntity::BackupInfo(FWD_TYPE eType) const
{
    if (m_iAccount == FWDDND_ACCOUNT_GLOBAL) // 全局模式
    {
        switch (eType)
        {
        case FWD_TYPE_ALWAYS:
            configParser_SetConfigString(kszForwardAlwaysTargetBackup,
                                         GetTarget(eType).c_str());
            break;
        default:
            break;
        }
    }
}

void CFwdEntity::NotifyChange(FWD_TYPE eType, FWD_CONFIG_SOURCE_TYPE eSource)
{
    FwdMessageStatusData data(eType);
    yl::string strTarget;
    data.bEnabled = GetInfo(eType, &strTarget, &data.iRingDelay);
    strncpy(data.szTarget, strTarget.c_str(), FwdMessageStatusData::TARGET_BUFFER_SIZE - 1);
    data.szTarget[FwdMessageStatusData::TARGET_BUFFER_SIZE - 1] = '\0';
    FWDDND_INFO("Notify forward status changed: "
                "account[%d] type[%d], enable[%d], target[%s], ringDelay[%d]",
                m_iAccount, eType, data.bEnabled, data.szTarget, data.iRingDelay);
    // 发送FWD状态改变消息
    etl_NotifyAppEx(false, FWD_MESSAGE_STATUS_CHANGED, m_iAccount, eSource,
                    sizeof(data), &data);
    // 通过DSSKey修改并启用Always FWD时，发消息给UI弹窗提示
    if (eSource == FWD_CFG_SRC_TYPE_DSSKEY && eType == FWD_TYPE_ALWAYS && data.bEnabled)
    {
        etl_NotifyAppEx(false, MULTIFWD_ENABLED_NOTICE, 0, 0,
                        strlen(strTarget.c_str()) + 1, strTarget.c_str());
    }
}

void CFwdEntity::SetLastSavedType() const
{
    if (m_iAccount == FWDDND_ACCOUNT_GLOBAL) // 只有全局模式才需要保存
    {
        int iType = 0;
        if (IsEnable(FWD_TYPE_ALWAYS))
        {
            iType |= FWD_TYPE_ALWAYS;
        }
        if (IsEnable(FWD_TYPE_BUSY))
        {
            iType |= FWD_TYPE_BUSY;
        }
        if (IsEnable(FWD_TYPE_NO_ANSWER))
        {
            iType |= FWD_TYPE_NO_ANSWER;
        }
        if (iType)
        {
            configParser_SetConfigInt(kszForwardTypeLastSaved, iType);
        }
    }
}
