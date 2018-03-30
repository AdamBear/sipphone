#include "hotplugdev/vcsport.h"
#include "ETLLib.hpp"
#include "etlmsghandler/modetlmsghandler.h"
#include "cameramanager.h"
#include "hdmimanager.h"
#include "storagemanager.h"
#include <hpdev/hpdev_api.h>
#include "dsklog/log.h"
#include "dskthread/moddskthread.h"
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "taskaction/modtaskaction.h"
#include "commonunits/commonunits_def.h"

static int OpenPort(DRV_DevManage_HotplugState & portState)
{
    int iFd;

    iFd = open("/dev/port", O_RDWR);
    if (iFd < 0)
    {
        return -1;
    }

    int iRet = read(iFd, &portState, sizeof(DRV_DevManage_HotplugState));
    if (iRet == sizeof(DRV_DevManage_HotplugState))
    {
        return iFd;
    }

    return -1;
}

static int WaitPortEvent(int iFd, DRV_DevManage_HotplugState & portState)
{
    if (iFd < 0)
    {
        return -1;
    }

    fd_set readFds;
    int iRet;

    FD_ZERO(&readFds);
    FD_SET(iFd, &readFds);

    iRet = select(iFd + 1, &readFds, NULL, NULL, NULL);
    if (iRet > 0)
    {
        if (FD_ISSET(iFd, &readFds))
        {
            iRet = read(iFd, &portState, sizeof(DRV_DevManage_HotplugState));
        }
    }

    return iRet;
}

static inline void Port_helper_close(int portfd)
{
    close(portfd);
}

#ifdef IF_SUPPORT_HDMI
void MapHDMIInfo(const DRV_DevManage_HotplugState * drvState, DskDisplayDevInfo & disResInfo)
{
    if (NULL == drvState)
    {
        return ;
    }

    printf("uiHoriSize[%d] uiVertSize[%d] iResValid[%d]\n", drvState->stVidOut0ResInfo.uiHoriSize,
           drvState->stVidOut0ResInfo.uiVertSize, drvState->stVidOut0ResInfo.iResValid);

    disResInfo.nCount = 0;
    for (int i = 0; i < drvState->stVidOut0ResInfo.iResValid && i < DEVICE_SCREEN_INFO_MAX; i++)
    {
        disResInfo.stInfoArr[i].nFramerate = drvState->stVidOut0ResInfo.stResPtn[i].uiFrmRate;
        disResInfo.stInfoArr[i].nHeight = drvState->stVidOut0ResInfo.stResPtn[i].uiResHei;
        disResInfo.stInfoArr[i].nWidth = drvState->stVidOut0ResInfo.stResPtn[i].uiResWid;
        disResInfo.nCount++;
    }

    disResInfo.nScreen_x = drvState->stVidOut0ResInfo.uiHoriSize;
    disResInfo.nScreen_y = drvState->stVidOut0ResInfo.uiVertSize;
}
#endif //#ifdef IF_SUPPORT_HDMI

#if defined(IF_SUPPORT_HDMI) || defined(IF_SUPPORT_VIDEO)
void HotplugStateNotify(DRV_DevManage_HotplugState & newState,
                        DRV_DevManage_HotplugState & oldState)
{
    // Test
    printf("Hotplug video[In0:%d] PC[In1:%d] light[%d] sensor[%d] HDMI[Out0:%d] Decoder[Out1:%d]\n",
           newState.videoInput0, newState.videoInput1,
           newState.lightSensorInput, newState.sensorInput, newState.videoOutput0, newState.videoOutput1);

    if (newState.sensorInput != oldState.sensorInput)
    {
        etl_NotifyApp(false, HOTPLUG_CAMERA_STATUS_CHANGE, newState.sensorInput == 1 ? 1 : 0, 0);
    }

    if (newState.lightSensorInput != oldState.lightSensorInput)
    {
        etl_NotifyApp(false, HOTPLUG_CAMERA_SENSOR_STATUS_CHANGE, newState.lightSensorInput == 1 ? 1 : 0,
                      0);
    }

    // 频繁拔插下，会出现拔出时，挡板是开的，并且插入时，挡板不会先关在开，此处容错
    if (newState.sensorInput == 0)
    {
        // 如果摄像头拔出，就强制写挡板为关
        newState.lightSensorInput = 0;
    }

#ifdef IF_SUPPORT_HDMI
    if (newState.videoOutput0 != oldState.videoOutput0)
    {
        // 底层上报液晶、HDMI可支持的分辨率，然后可通过web配置
        DskDisplayDevInfo disResInfo;
        MapHDMIInfo(&newState, disResInfo);

        etl_NotifyAppEx(false, HOTPLUG_HDMI_STATUS_CHANGE, newState.videoOutput0 == 1 ? 1 : 0, 0,
                        sizeof(DskDisplayDevInfo), &disResInfo);
    }
#endif //#ifdef IF_SUPPORT_HDMI

    oldState = newState;
}
#endif

static int Compare(int iLeft, int iRight)
{
    return iLeft < iRight;
}

#if defined(IF_SUPPORT_HDMI) || defined(IF_SUPPORT_VIDEO)
static void ProcessMediaDevPlugIn(int iFd,
                                  DRV_DevManage_HotplugState& portState,
                                  DRV_DevManage_HotplugState& oldState)
{
    if (read(iFd, &portState, sizeof(DRV_DevManage_HotplugState)) > 0)
    {
        // 摄像头插入，发消息通知
        HotplugStateNotify(portState, oldState);
    }
}
#endif

#ifdef IF_FEATURE_PSTN
static void ProcessPsntEvent(struct hpd_act* pAction)
{
    if (!pAction)
    {
        return;
    }

    switch (pAction->action)
    {
    case HPD_ACT_PLUGIN:
        {
            COMMONUNITS_INFO("PSTN Box plug in!");
            // 发送PSTN设备编号（由pstnadapter模块处理）
            etl_NotifyApp(false, HOTPLUG_USB_PSTN_BOX_STATE_CHANGE, HOTPLUG_USB_ADD, pAction->minor);
            // 取消未知类型USB设备的识别过程（由record模块处理）
            //etl_NotifyApp(false, HOTPLUG_USB_DEVICER_STATE_CHANGE, USBDEVICE_PSTNBOX, 0);
            etl_NotifyApp(false, HOTPLUG_USB_DEVICER_STATE_CHANGE, HOTPLUG_USB_ADD, pAction->type);
        }
        break;
    case HPD_ACT_DELETE:
        {
            COMMONUNITS_INFO("PSTN Box pull out!");
            // 发送PSTN设备断开消息（由pstnadapter模块处理）
            etl_NotifyApp(false, HOTPLUG_USB_PSTN_BOX_STATE_CHANGE, HOTPLUG_USB_REMOVE, pAction->minor);
            // 发送PSTN设备断开消息（由record模块处理）
            etl_NotifyApp(false, HOTPLUG_USB_DEVICER_STATE_CHANGE, HOTPLUG_USB_REMOVE, pAction->type);
        }
        break;
    default:
        break;
    }
}
#endif

static void ProcessHpdevPlugIn(int iFd)
{
    struct hpd_event evt;
    struct hpd_act* act = &(evt.action);

    if (read(iFd, &evt, sizeof(hpd_event)) != sizeof(hpd_event))
    {
        COMMONUNITS_WARN("Hpdev event, but size error[%d][%d]", sizeof(evt), sizeof(hpd_event));
        return;
    }

    COMMONUNITS_INFO("HPDEV event: dev_type=%d, dev_event=%d, dev_major=%d, dev_minor=%d", act->type, act->action, act->major, act->minor);

    // usb  过流检测
    if ((act->action == HPD_ACT_REPORT)
            && (act->etype == HPD_EXTRA_TYPE_5)
            && (sizeof(struct hpd_extra5) == evt.esize)
            && ((evt.e5).cls == HPD_E5_CLS_POWER))
    {
        USB_CURRENT_STATE eState = ((evt.e3).value) ? USB_CURRENT_STATE_NORMAL : USB_CURRENT_STATE_OVER;
        etl_NotifyApp(false, HOTPLUG_USB_STORAGE_CURRENT_OVER, eState, 0);
        COMMONUNITS_INFO("HPDEV even usb power[%s] ", eState == USB_CURRENT_STATE_NORMAL ? "NORMAL" : "OC");
    }

    switch (act->type)
    {
#ifdef IF_USB_SUPPORT
    case HPD_TYP_USB_STORAGE:
        _StorageManager.LoadUsbStorageMountEvent(*act);
        break;
#endif
#ifdef IF_FEATURE_PSTN
    case HPD_TYP_USB_PSTN:
        ProcessPsntEvent(act);
        break;
#endif
    case HPD_TYP_USB_INPUT:
        // 按键/触摸屏接入, 该事件属系统事件, 不需让用户感知到
        COMMONUNITS_INFO("Ignore this type.");
        break;
    default:
        if (HPD_ACT_PLUGIN == act->action)
        {
            etl_NotifyApp(false, HOTPLUG_USB_DEVICER_STATE_CHANGE,
                          HOTPLUG_USB_ADD, act->type);
        }
        else if (HPD_ACT_DELETE == act->action)
        {
            etl_NotifyApp(false, HOTPLUG_USB_DEVICER_STATE_CHANGE,
                          HOTPLUG_USB_REMOVE, act->type);
        }
        break;
    }
}

static IMPLEMENT_THREAD_METHOD(ScanHotplugDevice)
{
    COMMONUNITS_INFO("Scan hotplug device\n");

    fd_set readFds;
    FD_ZERO(&readFds);

    //YLVector<int> vecFd;
#if defined(IF_SUPPORT_HDMI) || defined(IF_SUPPORT_VIDEO)
    DRV_DevManage_HotplugState oldState;
    DRV_DevManage_HotplugState newState;
    // 打开Camera&HDMI端口
    int iMediaFd = OpenPort(newState);
    COMMONUNITS_INFO("Open hotplug media port [%s], fd[%d].",
                     iMediaFd < 0 ? "error" : "success", iMediaFd);
    if (iMediaFd >= 0)
    {
        //vecFd.push_back(iMediaFd);
        FD_SET(iMediaFd, &readFds);

        memset(&newState, 0, sizeof(DRV_DevManage_HotplugState));
        // 初始-1用于第一次都会发消息对外通知
        memset(&oldState, -1, sizeof(DRV_DevManage_HotplugState));
        HotplugStateNotify(newState, oldState);
    }
#else
    int iMediaFd = -1;
#endif

    // 打开U盘&PSTN端口
    int iHpdevFd = hpdev_helper_open();
    COMMONUNITS_INFO("Open hotplug USB port [%s], fd[%d].",
                     iHpdevFd < 0 ? "error" : "success", iHpdevFd);
    if (iHpdevFd >= 0)
    {
        FD_SET(iHpdevFd, &readFds);
    }
    int iMaxFd = iHpdevFd > iMediaFd ? iHpdevFd : iMediaFd;
    while (1)
    {
        if (select(iMaxFd + 1, &readFds, NULL, NULL, NULL) >= 0)
        {
#if defined(IF_SUPPORT_HDMI) || defined(IF_SUPPORT_VIDEO)
            if (iMediaFd >= 0 && FD_ISSET(iMediaFd, &readFds))
            {
                ProcessMediaDevPlugIn(iMediaFd, newState, oldState);
            }
            else
#endif
                if (iHpdevFd >= 0 && FD_ISSET(iHpdevFd, &readFds))
                {
                    ProcessHpdevPlugIn(iHpdevFd);
                }
        }
    }

    close(iMediaFd);
    close(iHpdevFd);
    return NULL;
}

bool CreateHotplugThread()
{
    TaskAction_CreateSpecialThreadByMethod(ScanHotplugDevice, TA_THREAD_SCAN_HOTPLUG);
    return true;
}
