///////////////////////////////////////////////////////////
//  CCameraManager.cpp
//  Created on:      06-二月-2015 14:51:59
//  Original author: huangwz
///////////////////////////////////////////////////////////

#ifdef IF_SUPPORT_VIDEO
#include "cameramanager.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "dsklog/log.h"
#include "service_config.h"
#include "hotplugdev/camera_api.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "idle/idle_data_define.h"
#include "idlescreen/include/modidlescreen.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talk/talklogic/include/talkmsgdefine.h"
#include "service_ipvp.h"

IMPLEMENT_GETINSTANCE(CCameraManager)

CCameraManager::CCameraManager():
    m_iSensorTimer((UINT) & m_iSensorTimer)
{
    m_eStatus = CST_NONE;
}

CCameraManager::~CCameraManager()
{
    Uninit();
}

bool CCameraManager::Init()
{
    if (configParser_GetConfigInt(kszCameraFuncEnable) == 0
            || configParser_GetConfigInt(kszUsbPortEnable) == 0)
    {
        COMMONUNITS_INFO("Camera init fail, Usb port or camera func disabled");
        return false;
    }

    // 注册消息
    SingleMsgReg(HOTPLUG_CAMERA_STATUS_CHANGE, CCameraManager::OnMessage);
    SingleMsgReg(HOTPLUG_CAMERA_SENSOR_STATUS_CHANGE, CCameraManager::OnMessage);

    SingleMsgReg(CONFIG_MSG_BCAST_CHANGED, CCameraManager::OnMessage);
    SingleMsgReg(TM_TIMER, CCameraManager::OnMessage);

    m_iFd = Camera_helper_open();

    // 默认是要关闭摄像头,降低功耗
    CtrlCamera(false, false, true);
    return true;
}

CameraStatusType CCameraManager::GetStatus()
{
    return m_eStatus;
}

void CCameraManager::CtrlCamera(bool bOpen, bool bSpecStream, bool bForce/* = false*/)
{
    static bool s_bOpen = false;
    static bool s_bStream = false;

    // 怀疑存在调用接口卡住问题
    COMMONUNITS_INFO("Camera ctrl old open[%d] stream[%d] new open[%d] stream[%d] force[%d]",
                     s_bOpen, s_bStream, bOpen, bSpecStream, bForce);

//  if (bOpen)
//  {
//      // 镜像功能
//      Camera_helper_set_Mirrormode(m_iFd, MIRROR_FLIPH);
//  }

    if (bOpen != s_bOpen
            || bForce)
    {
        // 记录当前状态
        s_bOpen = bOpen;

        // 打开摄像头需要关闭待机模式,关闭黑屏帧
        if (m_eStatus != CST_NOT_CONNECTED)
        {
            // 摄像头插入的情况下设置摄像头功耗模式
            COMMONUNITS_INFO("Camera StandbyMode Open[%d]", bOpen);
            Camera_helper_set_StandbyMode(m_iFd, bOpen ? STANDBY_OFF : STANDBY_ON);
        }
    }

    if (bSpecStream != s_bStream
            || bForce)
    {
        s_bStream = bSpecStream;
        // 开关黑屏帧（每1s产生一帧黑屏）
        COMMONUNITS_INFO("Camera PGMode SpecStream[%d]", bOpen);
        Camera_helper_set_PGMode(m_iFd, bSpecStream ? PG_ON : PG_OFF);
    }

    COMMONUNITS_INFO("Camera mode PG[%d] standby[%d]", bSpecStream, !bOpen);
}

void CCameraManager::SetDetectSensorTimer(bool bEnable)
{
    COMMONUNITS_INFO("set detect sensor timer[%d]", bEnable);
    if (bEnable)
    {
        timerSetThreadTimer(m_iSensorTimer, 10 * 1000);
    }
    else
    {
        timerKillThreadTimer(m_iSensorTimer);
    }
}

void CCameraManager::SetCamera()
{
    // 未连接，不用写到设备
    if (m_eStatus == CST_NOT_CONNECTED)
    {
        return;
    }

    int iExposure = configParser_GetConfigInt(kszCameraExposureCompensation);
    SetExposureCompensation(iExposure > 0, iExposure);

    // 0：关/1-10：50Hz/大于50：60Hz
    int iFlicker = configParser_GetConfigInt(kszCameraExposureFlicker);
    if (iFlicker > 50)
    {
        iFlicker = 2;
    }
    else if (iFlicker > 0)
    {
        iFlicker = 1;
    }
    else
    {
        iFlicker = 0;
    }
    SetFlicker(iFlicker);

    CameraParamConfig confgInfo;
    GetConfig(confgInfo);
    SetWhiteBalance(confgInfo.m_iWBMode);

    // 用户定义白平衡模式
    if (confgInfo.m_iWBMode == 5)
    {
        SetRBGain(0, confgInfo.m_iRedGain);

        SetRBGain(1, confgInfo.m_iBlueGain);
    }

    SetSaturation(confgInfo.m_iSaturation);

    SetSharpness(confgInfo.m_iSharpness);

    SetBrightness(confgInfo.m_iBrighness);

    SetContrast(confgInfo.m_iContrast);

    SetNR2D(confgInfo.m_iNR2D);

    SetNR3D(confgInfo.m_iNR3D);
}

void CCameraManager::GetConfig(CameraParamConfig & config)
{
    // 读取
    int iMode = configParser_GetConfigInt(kszCameraSceneMode); // 0手动 1标准 2强光 3柔光
    if (iMode == 1)
    {
        // 将模式值写入
        config.m_iWBMode = configParser_GetConfigInt(kszCameraStandardWhiteBalanceMode); // 0 自动 5手动
        config.m_iRedGain = configParser_GetConfigInt(kszCameraStandardRedGain);
        config.m_iBlueGain = configParser_GetConfigInt(kszCameraStandardBlueGain);
        config.m_iSharpness = configParser_GetConfigInt(kszCameraStandardSharpness);
        config.m_iBrighness = configParser_GetConfigInt(kszCameraStandardBrightness);
        config.m_iContrast = configParser_GetConfigInt(kszCameraStandardContrast);
        config.m_iSaturation = configParser_GetConfigInt(kszCameraStandardSaturation);
        config.m_iNR2D = configParser_GetConfigInt(kszCameraStandardNR2DLevel);
        config.m_iNR3D = configParser_GetConfigInt(kszCameraStandardNR3DLevel);
    }
    else if (iMode == 2)
    {
        config.m_iWBMode = configParser_GetConfigInt(kszCameraWarmWhiteBalanceMode);
        config.m_iRedGain = configParser_GetConfigInt(kszCameraWarmRedGain);
        config.m_iBlueGain = configParser_GetConfigInt(kszCameraWarmBlueGain);
        config.m_iSharpness = configParser_GetConfigInt(kszCameraWarmSharpness);
        config.m_iBrighness = configParser_GetConfigInt(kszCameraWarmBrightness);
        config.m_iContrast = configParser_GetConfigInt(kszCameraWarmContrast);
        config.m_iSaturation = configParser_GetConfigInt(kszCameraWarmSaturation);
        config.m_iNR2D = configParser_GetConfigInt(kszCameraWarmNR2DLevel);
        config.m_iNR3D = configParser_GetConfigInt(kszCameraWarmNR3DLevel);
    }
    else if (iMode == 3)
    {
        config.m_iWBMode = configParser_GetConfigInt(kszCameraCoolWhiteBalanceMode);
        config.m_iRedGain = configParser_GetConfigInt(kszCameraCoolRedGain);
        config.m_iBlueGain = configParser_GetConfigInt(kszCameraCoolBlueGain);
        config.m_iSharpness = configParser_GetConfigInt(kszCameraCoolSharpness);
        config.m_iBrighness = configParser_GetConfigInt(kszCameraCoolBrightness);
        config.m_iContrast = configParser_GetConfigInt(kszCameraCoolContrast);
        config.m_iSaturation = configParser_GetConfigInt(kszCameraCoolSaturation);
        config.m_iNR2D = configParser_GetConfigInt(kszCameraCoolNR2DLevel);
        config.m_iNR3D = configParser_GetConfigInt(kszCameraCoolNR3DLevel);
    }
    else
    {
        // 其他情况默认是自动
        config.m_iWBMode = configParser_GetConfigInt(kszCameraWhiteBalanceMode);
        config.m_iRedGain = configParser_GetConfigInt(kszCameraRedGain);
        config.m_iBlueGain = configParser_GetConfigInt(kszCameraBlueGain);
        config.m_iSharpness = configParser_GetConfigInt(kszCameraSharpness);
        config.m_iBrighness = configParser_GetConfigInt(kszCameraBrightness);
        config.m_iContrast = configParser_GetConfigInt(kszCameraContrast);
        config.m_iSaturation = configParser_GetConfigInt(kszCameraSaturation);
        config.m_iNR2D = configParser_GetConfigInt(kszCameraNR2DLevel);
        config.m_iNR3D = configParser_GetConfigInt(kszCameraNR3DLevel);
    }
}

void CCameraManager::OnConfigChanged()
{
    COMMONUNITS_INFO("Camera process config changed");
    SetCamera();
}

bool CCameraManager::OnTimer(UINT uTimerID)
{
    bool bHandle = true;
    if (m_iSensorTimer == uTimerID)
    {
        bHandle = DetectSensor();
    }
    else
    {
        bHandle = false;
    }

    return bHandle;
}

bool CCameraManager::DetectSensor()
{
#define CAMERA_SENSOR_ID         0x2710

    if (GetStatus() == CST_NOT_CONNECTED
            || GetStatus() == CST_NONE)
    {
        return true;
    }

    // 注意：GUI调用摄像头的接口，在底层挂掉时，接口会阻塞
    int iSensorID = -1;
    Camera_helper_get_SensorId(m_iFd, &iSensorID);
    if (iSensorID == CAMERA_SENSOR_ID)
    {
        return true;
    }

    COMMONUNITS_INFO("reset cmaera sensor[%x]", iSensorID);

    // set power off
    Camera_helper_set_PowMode(m_iFd, POWER_OFF);

    // sleep 200ms,再power on
    etlSleep(200);
    Camera_helper_set_PowMode(m_iFd, POWER_ON);

    //sleep 10ms,重新设置sensor
    etlSleep(10);
    Camera_helper_set_CamMode(m_iFd, CAM_ON);

    //视频通话中重新设置mute状态
    if (talklogic_IsVideoSessionExist(true)
            && talklogic_GetMuteStatus(true))
    {
        COMMONUNITS_INFO("reset video mute status");
        etl_NotifyApp(false, TALK_MSG_VIDEO_MUTE, 0, 0);
    }

    return true;
}

void CCameraManager::OnDeviceStatusChanged(msgObject & objMsg)
{
    COMMONUNITS_INFO("CCameraManager::OnDeviceStatusChanged[%d][%d][%d]", objMsg.message, objMsg.wParam,
                     objMsg.lParam);
    CameraStatusType eOld = m_eStatus;
    if (HOTPLUG_CAMERA_STATUS_CHANGE == objMsg.message)
    {
        CameraStatusType eNew = (objMsg.wParam == 1 ? CST_CONNECTED : CST_NOT_CONNECTED);
        int iRet = Camera_helper_set_CamMode(m_iFd, eNew == CST_CONNECTED ? CAM_ON : CAM_OFF);
        if (eNew == CST_CONNECTED
                && iRet != 0)
        {
            // 上报连接状态又初始化失败，当作未连接
            COMMONUNITS_ERR("Init camera fail");
            eNew = CST_NOT_CONNECTED;
        }

        // 摄像头连接正常
        m_eStatus = eNew;
    }
    else if (HOTPLUG_CAMERA_SENSOR_STATUS_CHANGE == objMsg.message)
    {
        // 摄像头挡板拔插
        if (objMsg.wParam == 1
                && m_eStatus == CST_CONNECTED)
        {
            m_eStatus = CST_READY;
        }

        if (objMsg.wParam == 0
                && m_eStatus == CST_READY)
        {
            m_eStatus = CST_CONNECTED;
        }
    }

    if (eOld == m_eStatus)
    {
        return;
    }

    // 通知状态变更
    etl_NotifyApp(false, CAMERA_MSG_STATUS_CHANGE, m_eStatus, eOld);

    COMMONUNITS_INFO("Camera status Change [%d]", m_eStatus);

    // 从未插入状态到插入状态，要设置摄像头
    if ((eOld == CST_NONE
            || eOld == CST_NOT_CONNECTED)
            && m_eStatus != CST_NOT_CONNECTED)
    {
        // 插入Camera要将配置写到摄像头
        SetCamera();

        // 插入摄像头先关闭摄像头，及停止特殊数据流
        CtrlCamera(false, false, true);

        // 启动定时检测定时器
        SetDetectSensorTimer(true);
#if 0
        // 读取摄像头参数，如果要支持多种分辨率的摄像头，需要保存参数
        int iWidth = 0;
        int iHeight = 0;
        int iFps = 0;
        ipvp_get_capture(mkGetHandle(), IPVP_VIDEO_SRC_CAM, &iWidth, &iHeight, &iFps);
        COMMONUNITS_INFO("New camera width[%d] height[%d], fps[%d]", iWidth, iHeight, iFps);
#endif
    }
    else if (m_eStatus == CST_NOT_CONNECTED)
    {
        // 未连接摄像头，停止定时器
        SetDetectSensorTimer(false);
    }
}

LRESULT CCameraManager::OnMessage(msgObject & objMsg)
{
    bool bHandle = true;

    switch (objMsg.message)
    {
    case CONFIG_MSG_BCAST_CHANGED:
    {
        if (objMsg.wParam == ST_CAMERA)
        {
            _CameraManager.OnConfigChanged();
        }
    }
    break;
    case HOTPLUG_CAMERA_STATUS_CHANGE:
    case HOTPLUG_CAMERA_SENSOR_STATUS_CHANGE:
    {
        _CameraManager.OnDeviceStatusChanged(objMsg);
    }
    break;
    case TM_TIMER:
    {
        bHandle = _CameraManager.OnTimer(objMsg.wParam);
    }
    break;
    default:
        break;
    }
    return bHandle;
}


void CCameraManager::Uninit()
{
    SingleMsgUnReg(HOTPLUG_CAMERA_STATUS_CHANGE, CCameraManager::OnMessage);
    SingleMsgUnReg(HOTPLUG_CAMERA_SENSOR_STATUS_CHANGE, CCameraManager::OnMessage);
    SingleMsgUnReg(CONFIG_MSG_BCAST_CHANGED, CCameraManager::OnMessage);
    SingleMsgUnReg(TM_TIMER, CCameraManager::OnMessage);
    Camera_helper_close(m_iFd);
}


/************************************************************************/
/*
     摄像头控制部分
*/
/************************************************************************/

// 抗闪烁 ExposureFlicker
bool CCameraManager::SetFlicker(int iType)
{
    if (iType < 0 || iType > 2)
    {
        return false;
    }

    // 0: OFF, 1: 50Hz, 2: 60Hz
    FLK_MODE eMapedValue = FLK_OFF;
    if (iType == 2)
    {
        eMapedValue = FLK_60HZ;
    }
    else if (iType == 1)
    {
        eMapedValue = FLK_50HZ;
    }

    return Camera_helper_set_Flicker(m_iFd, eMapedValue) == 0;
}

// 曝光补偿
bool CCameraManager::SetExposureCompensation(bool bOn, int val)
{
    int iMapedValue = EXPOSURE_POSI_0;
    AE_MODE eMode;

    // 自动曝光默认是开的
    eMode = AE_AUTO;
    Camera_helper_set_AEMode(m_iFd, eMode);

    // 映射关系
    if (val >= -4 && val <= 4)
    {
        iMapedValue += val;
    }

    return Camera_helper_set_Exposure(m_iFd, (EXPOSURE_MODE)iMapedValue) == 0;
}

// 白平衡
/* 0: Auto, 1: D6500K, 2: D5500K, 3: florescent, 4: incandescent 5: Manual,成功返回1，失败或参数不对返回0 */
int CCameraManager::SetWhiteBalance(int iType)
{
    AWB_MODE eAWBMode = AWB_AUTO;
    switch (iType)
    {
    case 0:
        eAWBMode = AWB_AUTO;
        break;
    case 1:
        eAWBMode = AWB_D6500K;
        break;
    case 2:
        eAWBMode = AWB_D5500K;
        break;
    case 3:
        eAWBMode = AWB_FLORESCENT;
        break;
    case 4:
        eAWBMode = AWB_INCANDESCENT;
        break;
    case 5:
        eAWBMode = AWB_USER;
        break;
    default:
        eAWBMode = AWB_USER;
    }

    COMMONUNITS_INFO("Set WB mode[%d]", eAWBMode);
    return Camera_helper_set_AWBMode(m_iFd, eAWBMode) == 0;
}

// 设置饱和度
/* 入参iLevel:0-100 , 成功返回1，失败返回0 */
bool CCameraManager::SetSaturation(int iLevel)
{
    if (iLevel >= 100)
    {
        iLevel = 99;
    }
    else if (iLevel < 0)
    {
        iLevel = 0;
    }

    int iMapedValue = chMAX((iLevel) * 256 / 100, 1);

    COMMONUNITS_INFO("SetSaturation[%d]", iMapedValue);
    return Camera_helper_set_Saturation(m_iFd, iMapedValue) == 0;
}

//  设置红蓝增益
/* 入参iLevel:0-100 color 0:red 1:blue, 成功返回1，失败或参数不对返回0 */
bool CCameraManager::SetRBGain(int iColor, int iLevel)
{
    COMMONUNITS_INFO("SetRBGain [%d], [%d]", iColor, iLevel);

    if (iLevel >= 100)
    {
        iLevel = 99;
    }
    else if (iLevel < 0)
    {
        iLevel = 0;
    }

    GAIN_MODE mode = (iColor == 0 ? GAIN_RED : GAIN_BLUE);

    return Camera_helper_set_Gain(m_iFd, mode, iLevel) == 0;
}

/* 入参iLevel:0-100 , 成功返回1，失败返回0 */
bool CCameraManager::SetBrightness(int iLevel)
{
    if (iLevel >= 100)
    {
        iLevel = 99;
    }
    else if (iLevel < 0)
    {
        iLevel = 0;
    }

    int iMapedValue = chMAX((iLevel) * 256 / 100, 1);
    COMMONUNITS_INFO("SetBrightness[%d]", iMapedValue);

    return Camera_helper_set_Brightness(m_iFd, iMapedValue) == 0;
}

/* 入参iLevel:0-100 , 成功返回1，失败返回0 */
int CCameraManager::SetContrast(int iLevel)
{
    if (iLevel >= 100)
    {
        iLevel = 99;
    }
    else if (iLevel < 0)
    {
        iLevel = 0;
    }
    int iMapedValue = chMAX((iLevel) * 256 / 100, 1);
    COMMONUNITS_INFO("SetContrast[%d]", iMapedValue);

    return Camera_helper_set_Contrast(m_iFd, iMapedValue) == 0;
}

// 锐度
/* 入参iLevel:0-100 , 成功返回1，失败返回0 */
int CCameraManager::SetSharpness(int iLevel)
{
    if (iLevel >= 100)
    {
        iLevel = 99;
    }
    else if (iLevel < 0)
    {
        iLevel = 0;
    }

    int iMapedValue = chMAX((iLevel) * 256 / 100, 1);
    COMMONUNITS_INFO("SetSharpness[%d]", iMapedValue);

    return Camera_helper_set_Sharpness(m_iFd, iMapedValue) == 0;
}

// NR2D
/* 入参iLevel:0-100 , 成功返回1，失败返回0 */
bool CCameraManager::SetNR2D(int iLevel)
{
    if (iLevel >= 100)
    {
        iLevel = 99;
    }
    else if (iLevel < 0)
    {
        iLevel = 0;
    }

    NF_MODE iMapedValue = NF_HIGH;
    if (iLevel == 0)
    {
        iMapedValue = NF_OFF;
    }
    else if (iLevel < 33)
    {
        iMapedValue = NF_LOW;
    }
    else if (iLevel < 66)
    {
        iMapedValue = NF_MID;
    }

    COMMONUNITS_INFO("SetNR2D[%d] mode[%d]", iLevel, iMapedValue);
    return Camera_helper_set_NoiseFilter(m_iFd, iMapedValue) == 0;
}

bool CCameraManager::SetNR3D(int iLevel)
{
    //TODO
    return false;
}
#endif //#ifdef IF_SUPPORT_VIDEO
