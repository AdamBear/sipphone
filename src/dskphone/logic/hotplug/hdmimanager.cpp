///////////////////////////////////////////////////////////
//  CHdmiManager.cpp
//  Created on:      06-二月-2015 14:51:59
//  Original author: huangwz
///////////////////////////////////////////////////////////
#include "hdmimanager.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "dsklog/log.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "idle/idle_data_define.h"
#include "idlescreen/include/modidlescreen.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "media/modmedia.h"

#ifdef IF_SUPPORT_HDMI
IMPLEMENT_GETINSTANCE(CHdmiManager)

CHdmiManager::CHdmiManager()
{
    m_eStatus = HST_NONE;
    memset(&m_stDevInfo, 0, sizeof(DskDisplayDevInfo));
}

CHdmiManager::~CHdmiManager()
{
    Uninit();
}

bool CHdmiManager::Init()
{
    // 注册消息
    SingleMsgReg(HOTPLUG_HDMI_STATUS_CHANGE, CHdmiManager::OnMessage);
    SingleMsgReg(CONFIG_MSG_BCAST_CHANGED, CHdmiManager::OnMessage);
    SingleMsgReg(DSK_MSG_GET_DISPLAY_INFO, CHdmiManager::OnMessage);

    return true;
}

void CHdmiManager::Uninit()
{
    SingleMsgUnReg(HOTPLUG_HDMI_STATUS_CHANGE, CHdmiManager::OnMessage);
    SingleMsgUnReg(CONFIG_MSG_BCAST_CHANGED, CHdmiManager::OnMessage);
    SingleMsgUnReg(DSK_MSG_GET_DISPLAY_INFO, CHdmiManager::OnMessage);
}

LRESULT CHdmiManager::OnMessage(msgObject & objMsg)
{
    COMMONUNITS_INFO("Receive hdmi msg[%d] w[%d] l[%d]", objMsg.message, objMsg.wParam, objMsg.lParam);
    switch (objMsg.message)
    {
    case HOTPLUG_HDMI_STATUS_CHANGE:
    {
        if (NULL == objMsg.GetExtraData())
        {
            break;
        }

        _HdmiManager.SetConnectState(objMsg.wParam == 1, (const DskDisplayDevInfo *)objMsg.GetExtraData());
    }
    break;
    case CONFIG_MSG_BCAST_CHANGED:
    {
        if (objMsg.wParam != ST_HDMI_CONFIG)
        {
            break;
        }

        etl_NotifyApp(FALSE, HDMI_MSG_SWITCH_CHANGE, _HdmiManager.GetHdmiSwitch(), 0);

        // 配置变化，才设置
        _HdmiManager.SetDisplayDevice(false);
    }
    break;
    case DSK_MSG_GET_DISPLAY_INFO:
    {
        DskDisplayDevInfo info;
        memset(&info, 0, sizeof(DskDisplayDevInfo));
        _HdmiManager.GetDisplayDeviceInfo(info);
        objMsg.ReplyMessageEx(1, sizeof(DskDisplayDevInfo), (void *)&info);
    }
    break;
    default:
        break;
    }

    return 1;
}

void CHdmiManager::SetConnectState(bool bConnected, const DskDisplayDevInfo * devResInfo)
{
    HDMIStatusType eOldStatus = m_eStatus;

    m_eStatus = bConnected ? HST_CONNECTED : HST_DISCONNECTED;

    //连接上需要设置一下分辨率信息
    if (NULL != devResInfo)
    {
        m_stDevInfo = *devResInfo;
        SetDisplayDevice(true);
    }

    // 通知状态变更
    etl_NotifyApp(false, HDMI_MSG_STATUS_CHANGE, m_eStatus, eOldStatus);

    //通话中开启需要打开开关
    if (talklogic_IsVideoSessionExist()
            && IsHdmiEnable())
    {
        SetHdmiSwitch(true);
    }
}

int CHdmiManager::GetScreenInfo(DskScreenInfo & info)
{
    if (m_stDevInfo.nCount == 0)
    {
        info.nFramerate = 60;
        info.nWidth = 1920;
        info.nHeight = 1080;

        return 0;
    }

    yl::string strInfo = configParser_GetConfigString(kszExtDisplayResolution);
    COMMONUNITS_INFO("hdmi config reslution[%s]", strInfo.c_str());
    int width = 0;
    int height = 0;
    int frame = 0;
    if (sscanf(strInfo.c_str(), "%dx%df%d", &width, &height, &frame) != 3)
    {
        width = 0;
        height = 0;
        frame = 0;
    }

    int nIndex = -1;
    for (int i = 0; i < m_stDevInfo.nCount; i++)
    {
        COMMONUNITS_INFO("hdmi [%d][%d][%d]", m_stDevInfo.stInfoArr[i].nWidth,
                         m_stDevInfo.stInfoArr[i].nHeight, m_stDevInfo.stInfoArr[i].nFramerate);

        if (width == m_stDevInfo.stInfoArr[i].nWidth
                && height == m_stDevInfo.stInfoArr[i].nHeight
                && frame == m_stDevInfo.stInfoArr[i].nFramerate)
        {
            nIndex = i;
            break;
        }
    }

    if (nIndex < 0
            || nIndex >= DEVICE_SCREEN_INFO_MAX)
    {
        // 没找到对应的配置值，一定要重新写入
        nIndex = 0;
    }

    if (m_stDevInfo.stInfoArr[nIndex].nWidth == 0
            || m_stDevInfo.stInfoArr[nIndex].nHeight == 0
            || m_stDevInfo.stInfoArr[nIndex].nFramerate == 0)
    {
        info.nFramerate = 60;
        info.nWidth = 1920;
        info.nHeight = 1080;
    }
    else
    {
        info.nFramerate = m_stDevInfo.stInfoArr[nIndex].nFramerate;
        info.nWidth = m_stDevInfo.stInfoArr[nIndex].nWidth;
        info.nHeight = m_stDevInfo.stInfoArr[nIndex].nHeight;
    }

    return nIndex;
}

void CHdmiManager::SetDisplayDevice(bool bForce)
{
    if (m_eStatus != HST_CONNECTED)
    {
        COMMONUNITS_WARN("hdmi not connect");
        return;
    }

    DskScreenInfo info;
    int nIndex = GetScreenInfo(info);

    if (bForce
            || m_stDevInfo.nFocus != nIndex)
    {
        m_stDevInfo.nFocus = nIndex;
        modmedia_SetDisplay(info.nWidth, info.nHeight, info.nFramerate);
    }

    return;
}

bool CHdmiManager::SetHdmiSwitch(bool bEnable)
{
    bool bChanged = configParser_SetConfigInt(kszExtDisplaySwitch, bEnable ? 1 : 0);
    if (bChanged)
    {
        etl_NotifyApp(false, HDMI_MSG_SWITCH_CHANGE, bEnable ? 1 : 0, 0);
    }
    return true;
}

bool CHdmiManager::GetHdmiSwitch()
{
    if (!IsHdmiEnable())
    {
        return false;
    }

    return (configParser_GetConfigInt(kszExtDisplaySwitch) != 0);
}

bool CHdmiManager::IsHdmiEnable()
{
    return (configParser_GetConfigInt(kszExtDisplayEnable) == 1);
}

#endif

