///////////////////////////////////////////////////////////
//  CCameraManager.h
//  Implementation of the Class CCameraManager
//  Created on:      06-二月-2015 14:51:58
//  Original author: huangwz
///////////////////////////////////////////////////////////
#ifndef _CAMERA_MANAGER_H_
#define _CAMERA_MANAGER_H_

#include "hotplug/commondefine.h"
#include "singletonclassdefine.h"
#include <ETLLib.hpp>

#ifdef IF_SUPPORT_VIDEO
struct CameraParamConfig
{
    // 白平衡
    int m_iWBMode;
    // 红增益
    int m_iRedGain;
    // 蓝增益
    int m_iBlueGain;
    // 锐度
    int m_iSharpness;
    // 亮度
    int m_iBrighness;
    // 对比度
    int m_iContrast;
    // 饱和度
    int m_iSaturation;
    // 2D降噪
    int m_iNR2D;
    // 3D降噪
    int m_iNR3D;
};

class CCameraManager
{
    DECLARE_SINGLETON_CLASS(CCameraManager)
public:
    bool Init();
    void Uninit();

    static LRESULT OnMessage(msgObject & objMsg);

    CameraStatusType GetStatus();

    // 开关摄像头/控制是否产生特殊数据流
    void CtrlCamera(bool bOpen, bool bSpecStream, bool bForce = false);

protected:

    void SetDetectSensorTimer(bool bEnable);

    bool DetectSensor();

private:

    void OnConfigChanged();

    void GetConfig(CameraParamConfig & config);

    void OnDeviceStatusChanged(msgObject & objMsg);

    bool OnTimer(UINT uTimerID);

    // 将配置写到摄像头设备
    void SetCamera();

    // 抗闪烁
    bool SetFlicker(int iType);

    // 曝光补偿
    bool SetExposureCompensation(bool bOn, int val);

    // 白平衡
    int SetWhiteBalance(int iType);

    // 饱和度
    bool SetSaturation(int iLevel);

    //  设置红蓝增益
    bool SetRBGain(int iColor, int iLevel);

    // 亮度
    bool SetBrightness(int iLevel);

    // 对比度
    int SetContrast(int iLevel);

    // 锐度
    int SetSharpness(int iLevel);

    // NR2D
    bool SetNR2D(int iLevel);

    // NR3D
    bool SetNR3D(int iLevel);
private:
    // 设备
    int m_iFd;

    // 连接状态
    CameraStatusType m_eStatus;

    const int m_iSensorTimer;
};

#define _CameraManager GET_SINGLETON_INSTANCE(CCameraManager)
#endif // #ifdef IF_SUPPORT_VIDEO

#endif // _CAMERA_MANAGER_H_
