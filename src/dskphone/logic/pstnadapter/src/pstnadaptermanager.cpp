#include "pstnadaptermanager.h"
#include "pstnadapterdatadefine.h"
#include "pstnadaptermessagedefine.h"
#include "pstnline.h"
#include "include/common/common_data_define.h"

#ifdef IF_FEATURE_PSTN

#if !WIN32
#include <configparser/modconfigparser.h>
#include <configiddefine.h>
#include <dsklog/log.h>
#include <etlmsghandler/modetlmsghandler.h>
#include <hotplug/commondefine.h>
#include <service_config.h>
#endif // !WIN32


IMPLEMENT_GETINSTANCE(CPstnAdapterManager)

CPstnAdapterManager::CPstnAdapterManager()
    : CMKThread(PSTN_MGR_THREAD_NAME)
{
}

CPstnAdapterManager::~CPstnAdapterManager()
{
}

BOOL CPstnAdapterManager::AfterThreadLaunched()
{
    COMMONUNITS_INFO("CPstnAdapterManager::AfterThreadLaunched");
    Init();
    return TRUE;
}

void CPstnAdapterManager::BeforeThreadShutdown()
{
    COMMONUNITS_INFO("CPstnAdapterManager::BeforeThreadShutdown");
    Uninit();
}

BOOL CPstnAdapterManager::TryThreadMessage(msgObject & msg)
{
    COMMONUNITS_DEBUG("CPstnAdapterManager::TryThreadMessage(msg=%d, l=%d, w=%d)",
                      msg.message, msg.lParam, msg.wParam);
    switch (msg.message)
    {
    case MKIT_MSG_TIMER:
        {
            // 处理定时器
            const UINT nTimerID = msg.wParam;
            for (DeviceMap::iterator it = m_mapDevice.begin(); it != m_mapDevice.end(); ++it)
            {
                if (it->second->OnTimer(nTimerID))
                {
                    return TRUE;
                }
            }
            break;
        }
    case PSTN_MSG_SET_TIMER:
        {
            // 设置定时器
            timerSetThreadTimer(msg.wParam, msg.lParam);
            return TRUE;
        }
    case PSTN_MSG_KILL_TIMER:
        {
            // 停止定时器
            timerKillThreadTimer(msg.wParam);
            return TRUE;
        }
    default:
        break;
    }
    return FALSE;
}

int CPstnAdapterManager::GetDeviceCount()
{
    COMMONUNITS_INFO("%s()=%d", __FUNCTION__, m_mapDevice.size());
    return m_mapDevice.size();
}

int CPstnAdapterManager::GetLineCount()
{
    int iCount = 0;
    for (DeviceMap::iterator it = m_mapDevice.begin(); it != m_mapDevice.end(); ++it)
    {
        if (it->second->IsConnected())
        {
            ++iCount;
        }
    }
    COMMONUNITS_DEBUG("%s()=%d", __FUNCTION__, iCount);
    return iCount;
}

void CPstnAdapterManager::SetEnable(int iDeviceId, bool bEnable)
{
    COMMONUNITS_INFO("%s(%d, %s)", __FUNCTION__, iDeviceId, bEnable ? "true" : "false");
    DeviceMap::iterator it = m_mapDevice.find(iDeviceId);
    if (it == m_mapDevice.end())
    {
        COMMONUNITS_WARN("Unknown device ID #%d!", iDeviceId);
    }
    else
    {
        it->second->SetEnable(bEnable);
    }
}

bool CPstnAdapterManager::SetHook(int iDeviceId, bool bOffHook)
{
    COMMONUNITS_INFO("%s(%d, %s)", __FUNCTION__, iDeviceId, bOffHook ? "true" : "false");
    DeviceMap::iterator it = m_mapDevice.find(iDeviceId);
    if (it == m_mapDevice.end())
    {
        COMMONUNITS_WARN("Unknown device ID #%d!", iDeviceId);
        return false;
    }
    return it->second->SetHook(bOffHook);
}

bool CPstnAdapterManager::Flash(int iDeviceId)
{
    COMMONUNITS_INFO("%s(%d)", __FUNCTION__, iDeviceId);
    DeviceMap::iterator it = m_mapDevice.find(iDeviceId);
    if (it == m_mapDevice.end())
    {
        COMMONUNITS_WARN("Unknown device ID #%d!", iDeviceId);
        return false;
    }
    return it->second->Flash();
}
bool CPstnAdapterManager::IsFlashing(int iDeviceId)
{
    COMMONUNITS_INFO("%s(%d)", __FUNCTION__, iDeviceId);
    DeviceMap::iterator it = m_mapDevice.find(iDeviceId);
    if (it == m_mapDevice.end())
    {
        COMMONUNITS_WARN("Unknown device ID #%d!", iDeviceId);
        return false;
    }
    return it->second->IsFlashing();
}

bool CPstnAdapterManager::SetMute(int iDeviceId, bool bMute)
{
    COMMONUNITS_INFO("%s(%d, %s)", __FUNCTION__, iDeviceId, bMute ? "true" : "false");
    DeviceMap::iterator it = m_mapDevice.find(iDeviceId);
    if (it == m_mapDevice.end())
    {
        COMMONUNITS_WARN("Unknown device ID #%d!", iDeviceId);
        return false;
    }
    return it->second->SetMute(bMute);
}

bool CPstnAdapterManager::SetHold(int iDeviceId, bool bHold)
{
    COMMONUNITS_INFO("%s(%d, %s)", __FUNCTION__, iDeviceId, bHold ? "true" : "false");
    DeviceMap::iterator it = m_mapDevice.find(iDeviceId);
    if (it == m_mapDevice.end())
    {
        COMMONUNITS_WARN("Unknown device ID #%d!", iDeviceId);
        return false;
    }
    return it->second->SetHold(bHold);
}

bool CPstnAdapterManager::SetDND(int iDeviceId, bool bDND)
{
    COMMONUNITS_INFO("%s(%d, %s)", __FUNCTION__, iDeviceId, bDND ? "true" : "false");
    DeviceMap::iterator it = m_mapDevice.find(iDeviceId);
    if (it == m_mapDevice.end())
    {
        COMMONUNITS_WARN("Unknown device ID #%d!", iDeviceId);
        return false;
    }
    return it->second->SetDND(bDND);
}

bool CPstnAdapterManager::Dial(int iDeviceId, const yl::string & strNumber, int iCallId)
{
    COMMONUNITS_INFO("%s(%d, %s, %d)", __FUNCTION__, iDeviceId, strNumber.c_str(), iCallId);
    DeviceMap::iterator it = m_mapDevice.find(iDeviceId);
    if (it == m_mapDevice.end())
    {
        COMMONUNITS_WARN("Unknown device ID #%d!", iDeviceId);
        return false;
    }
    return it->second->Dial(strNumber, iCallId);
}

int CPstnAdapterManager::GetLineId(int iDeviceId)
{
    COMMONUNITS_INFO("%s(%d)", __FUNCTION__, iDeviceId);
    DeviceMap::iterator it = m_mapDevice.find(iDeviceId);
    if (it == m_mapDevice.end())
    {
        COMMONUNITS_WARN("Unknown device ID #%d!", iDeviceId);
        return -1;
    }
    return it->second->GetLineId();
}

bool CPstnAdapterManager::IsAvailable(int iDeviceId)
{
    COMMONUNITS_INFO("%s(%d)", __FUNCTION__, iDeviceId);
    DeviceMap::iterator it = m_mapDevice.find(iDeviceId);
    if (it == m_mapDevice.end())
    {
        COMMONUNITS_WARN("Unknown device ID #%d!", iDeviceId);
        return false;
    }
    bool bRet = it->second->IsAvailable();
    COMMONUNITS_INFO("%s(%d)=%d", __FUNCTION__, iDeviceId, bRet);
    return bRet;
}

yl::string CPstnAdapterManager::GetVersion(int iDeviceId)
{
    COMMONUNITS_INFO("%s(%d)", __FUNCTION__, iDeviceId);

    DeviceMap::iterator it = m_mapDevice.find(iDeviceId);
    if (it == m_mapDevice.end())
    {
        COMMONUNITS_WARN("Unknown device ID #%d!", iDeviceId);
        return yl::string();
    }
    return it->second->GetVersion();
}

yl::string CPstnAdapterManager::GetFxoId(int iDeviceId)
{
    COMMONUNITS_INFO("%s(%d)", __FUNCTION__, iDeviceId);

    DeviceMap::iterator it = m_mapDevice.find(iDeviceId);
    if (it == m_mapDevice.end())
    {
        COMMONUNITS_WARN("Unknown device ID #%d!", iDeviceId);
        return yl::string();
    }
    return it->second->GetFxoId();
}

bool CPstnAdapterManager::Upgrade(int iDeviceId, const yl::string & strRomPath,
                                  UpgradeCallback pCallback, void * pParam)
{
    COMMONUNITS_INFO("%s(%d, %s, %p, %p)", __FUNCTION__, iDeviceId, strRomPath.c_str(), pCallback,
                     pParam);

    DeviceMap::iterator it = m_mapDevice.find(iDeviceId);
    if (it == m_mapDevice.end())
    {
        COMMONUNITS_WARN("Unknown device ID #%d!", iDeviceId);
        return false;
    }
    return it->second->Upgrade(strRomPath, pCallback, pParam);
}

void CPstnAdapterManager::Init()
{
#if !WIN32
    if (configParser_GetConfigInt(kszPSTNEnable) == 1)
#endif // !WIN32
    {
        COMMONUNITS_INFO("PSTN adapter module init.");
        SingleMsgReg(HOTPLUG_USB_PSTN_BOX_STATE_CHANGE, CPstnAdapterManager::ProcessHotplugMsg);
        SingleMsgReg(CONFIG_MSG_BCAST_CHANGED, CPstnAdapterManager::ProcessConfigChangedMsg);
    }
}

void CPstnAdapterManager::Uninit()
{
    etl_UnregisterMsgHandle(HOTPLUG_USB_PSTN_BOX_STATE_CHANGE, HOTPLUG_USB_PSTN_BOX_STATE_CHANGE,
                            CPstnAdapterManager::ProcessHotplugMsg);

    for (DeviceMap::iterator it = m_mapDevice.begin(); it != m_mapDevice.end(); it++)
    {
        delete it->second;
    }
    m_mapDevice.clear();
}

LRESULT CPstnAdapterManager::ProcessHotplugMsg(msgObject & msg)
{
    CPstnAdapterManager * self = &(_PstnAdapterManager);
    if (HOTPLUG_USB_PSTN_BOX_STATE_CHANGE != msg.message)
    {
        COMMONUNITS_WARN("Unknown message[%d]!", msg.message);
        return FALSE;
    }
    COMMONUNITS_INFO("PSTN Box state changed[%d: %d, %d]!", msg.message, msg.wParam, msg.lParam);

#if WIN32
    const int iDev = msg.lParam;
#else
    const int iDev = msg.lParam -
                     CPstnLine::HPDEV_DEVICE_ID_OFFSET; // hpdev上报的子设备号与串口编号偏移192
    if (iDev < 0 || iDev >= MAX_PSTN_LINE_COUNT)
    {
        COMMONUNITS_ERR("Invalid device ID [%d] for PSTN Box device!", iDev);
        return FALSE;
    }
#endif // WIN32
    if (HOTPLUG_USB_ADD == msg.wParam) // PSTN Box插入
    {
        DeviceMap::iterator it = self->m_mapDevice.find(iDev);
        if (it != self->m_mapDevice.end())
        {
            COMMONUNITS_WARN("Device #%d already exist!", iDev);
        }
        CPstnLine * pDevice = new CPstnLine(iDev);
        self->m_mapDevice[iDev] = pDevice;
        pDevice->OpenSerial();
    }
    else if (HOTPLUG_USB_REMOVE == msg.wParam) // PSTN Box拔出
    {
        // 停止读取线程
        DeviceMap::iterator it = self->m_mapDevice.find(iDev);
        if (it == self->m_mapDevice.end())
        {
            COMMONUNITS_ERR("Unknown ID [%d] for PSTN Box device!", iDev);
            return FALSE;
        }
        COMMONUNITS_INFO("Going to shutdown device #%d.", iDev);
        // 关闭串口
        it->second->CloseSerial();
        delete it->second;
        self->m_mapDevice.erase(it);
    }
    else
    {
        COMMONUNITS_WARN("Unknown wParam[%d]!", msg.wParam);
        return FALSE;
    }
    return TRUE;
}

LRESULT CPstnAdapterManager::ProcessConfigChangedMsg(msgObject & msg)
{
    CPstnAdapterManager *self = &(_PstnAdapterManager);
    if (CONFIG_MSG_BCAST_CHANGED != msg.message)
    {
        COMMONUNITS_WARN("Unknown message[%d : wParam=%d, lParam=%d]!",
                         msg.message, msg.wParam, msg.lParam);
        return FALSE;
    }
    switch (msg.wParam)
    {
    case ST_COUNTRY:
        {
            COMMONUNITS_INFO("Country config changed.");
            // ÐÞ¸ÄËùÓÐPSTN BoxµÄ¹ú¼ÒÅäÖÃ
            yl::string strCountryCode = configParser_GetConfigString(kszCurrentCountry);
            for (DeviceMap::iterator it = self->m_mapDevice.begin(); it != self->m_mapDevice.end(); ++it)
            {
                it->second->SetCountryCode(strCountryCode);
            }
            break;
        }
    case ST_FLASH_DURATION:
        {
            COMMONUNITS_INFO("Flash duration config changed.");
            for (DeviceMap::iterator it = self->m_mapDevice.begin(); it != self->m_mapDevice.end(); ++it)
            {
                it->second->SetFlashDuration(true);
            }
            break;
        }
    default:
        break;
    }

    return TRUE;
}

#endif // IF_FEATURE_PSTN
