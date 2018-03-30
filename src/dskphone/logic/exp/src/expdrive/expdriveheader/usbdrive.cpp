#include "usbdrive.h"
#ifdef _EXP_COLOR_SUPPORT
#include "expusb_api.h"
#include "hpdev_api.h"
#include "expscannerdefine.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "taskaction/modtaskaction.h"
#include "commonunits/commonunits_def.h"

#define DEVICELIB_EXPUSB_MODE_NONE    -1
#define EXP_POWER_LACK_KEY_CODE       128
#define EXP_POWER_LACK_KEY_STATE      1



static IMPLEMENT_THREAD_METHOD(InsertScanThread)
{
    g_USBDrive.USBEXPInsertScanFunc();
    return NULL;
}

LRESULT CUSBDrive::USBEXPInsertScanFunc()
{
    struct hpd_event event;
    struct expusb_event key_scan_event;
    int retval;

    fd_set rfdset;
    FD_ZERO(&rfdset);

    //按键扫描端口加入集合
    int thread_key_scan_fd = m_KeyScanHandle;
    if (thread_key_scan_fd >= 0)
    {
        FD_SET(thread_key_scan_fd, &rfdset);
    }

    //拔插端口
    int thread_insert_scan_fd = hpdev_helper_open();
    if (thread_insert_scan_fd >= 0)
    {
        FD_SET(thread_insert_scan_fd, &rfdset);
    }

    int iMaxFd = thread_insert_scan_fd > thread_key_scan_fd ? thread_insert_scan_fd : thread_key_scan_fd;

    for (int i = 0; i < MAX_EXP_NUM; ++i)
    {
        m_nExpStatus[i] = DEVICELIB_EXPUSB_MODE_NONE;
    }

    while (1)
    {
        fd_set rfdsetTemp = rfdset; //每处理完一次，需要重新设置监控端口

        retval = select(iMaxFd + 1, &rfdsetTemp, NULL, NULL, NULL);
        if (retval > 0)
        {
            if (thread_insert_scan_fd >= 0 && FD_ISSET(thread_insert_scan_fd, &rfdsetTemp))//拔插
            {
                if (read(thread_insert_scan_fd, &event, sizeof(hpd_event)) == sizeof(hpd_event))
                {
                    if (event.action.type != HPD_TYP_USB_EXP) //只处理事件类型是EXP50
                    {
                        continue;
                    }

                    //EXP 50 只处理0,0 的case
                    if (event.action.major != 0
                            || event.action.minor != 0)
                    {
                        continue;
                    }

                    if (event.action.action == HPD_ACT_PLUGIN)  //事件是插入
                    {
                    }
                    else if (event.action.action == HPD_ACT_READY)
                    {
                        static expusb_val mod;
                        mod.port = event.action.port;

                        if (event.action.port >= MAX_EXP_NUM)
                        {
                            continue;
                        }

                        if (event.action.etype != HPD_EXTRA_TYPE_3
                                || sizeof(struct hpd_extra3) != event.esize
                                || (event.e3.ver & 0x3) != 0x2)
                        {
                            DEVICELIB_INFO("[regnize failed!!!]event.action.action = %d event.action.port = %d,event.action.type = %d event.e3.ver=[%d]",
                                           event.action.action, event.action.port, event.action.type, event.e3.ver);
                            PostMsgToApp(EXP_MSG_EXP_REGNIZE_FAIL, 0, 0);
                            continue;
                        }

                        //先获取当前状态
                        int nRet = expusb_helper_getrunmod(m_InsertScanHandle, &mod);

                        EXP_INFO("Exp Device: hpdev event[HPD_ACT_READY], expusb_helper_getrunmod result[%d].", nRet);

                        if (nRet < 0)
                        {
                            continue;
                        }

                        EXP_INFO("Exp Device:HPD_ACT_READY mode [%d], port [%d]!!!!!!!!!!!!!!!!!!!!!!!!!!!!", mod.val,
                                 event.action.port);

                        //如果已经是执行模式，则向ui上报插入成功
                        if (EXPUSB_MODE_APP == mod.val)
                        {
                            m_nExpStatus[event.action.port] = EXPUSB_MODE_APP;
                            PostMsgWithRetry(EXP_IN, event.action.port, DT_EXP_50, 0, NULL);
                        }
                        //如果是引导模式，则执行下载
                        else
                        {
                            if (event.action.port >= MAX_EXP_NUM)
                            {
                                continue;
                            }

                            m_nExpStatus[event.action.port] = EXPUSB_MODE_LOADER;

                            //由于引导模式下是不能对扩展台进行操作的，所以不进行通知，而是等进入app模式才通知
                            OnUSBExpDownloadApp(DT_EXP_50, event.action.port);
                        }

                        EXP_INFO("Exp Device:HPD_ACT_READY over!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
                    }
                    else if (event.action.action == HPD_ACT_DELETE) //事件是拔出
                    {
                        if (event.action.port >= MAX_EXP_NUM)
                        {
                            continue;
                        }

                        int nStatus = m_nExpStatus[event.action.port];
                        m_nExpStatus[event.action.port] = DEVICELIB_EXPUSB_MODE_NONE;

                        //如果是app模式下拔出，才进行通知拔出
                        if (EXPUSB_MODE_APP == nStatus)
                        {
                            EXP_INFO("Exp Device: hpdev event mode[HPD_ACT_DELETE][APP]. port [%d]", event.action.port);
                            PostMsgWithRetry(EXP_OUT, event.action.port, DT_EXP_50);
                        }
                        else
                        {
                            EXP_INFO("Exp Device: hpdev event mode[HPD_ACT_DELETE][NOT APP]. port [%d]", event.action.port);
                        }
                    }
                }
            }
            else if (thread_key_scan_fd >= 0 && FD_ISSET(thread_key_scan_fd, &rfdsetTemp))//按键
            {
                if (read(thread_key_scan_fd, &key_scan_event, sizeof(key_scan_event)) == sizeof(key_scan_event) && key_scan_event.code != 0)
                {
                    if (EXP_POWER_LACK_KEY_CODE == key_scan_event.code
                            && EXP_POWER_LACK_KEY_STATE == key_scan_event.value)
                    {
                        PostMsgToApp(EXP_MSG_EXP_POWER_LACK, 0, 0);
                        continue;
                    }
                    //为了统一进行位移运算，需要改成从0开始记按键
                    DsskeyID dssId = KeyID2DsskeyID(key_scan_event.code - 1, key_scan_event.port);

                    //event.value为0是抬起，1是按下
                    EXP_SCANNER_MESSAGE msg = KeyStatus2Msg(key_scan_event.value);
                    //KeyProcess(dssId, event.value);
                    PostMsgToApp(msg, dssId, DT_EXP_50);
                    EXP_INFO("EXP USBdrive expindex[%d] keycode[%d] iKeyStatus[%d] dssId[%d]", key_scan_event.port,
                             key_scan_event.code - 1, key_scan_event.value, dssId);
                }
            }
        }
    }

    close(thread_insert_scan_fd);
    close(thread_key_scan_fd);
    return 1;
}

CUSBDrive & CUSBDrive::GetInstance()
{
    static CUSBDrive instance;
    return instance;
}

void CUSBDrive::InitInsertScan()
{
    if (m_InsertScanHandle == USB_INVALID_HANDLE)
    {
        m_InsertScanHandle = expusb_helper_open(EXP_USB_DEV_NAME);
        EXP_INFO("EXP Usbdrive InsertScanHandle[%d]", m_InsertScanHandle);
    }
}

void CUSBDrive::InitKeyScan()
{
    if (m_KeyScanHandle == USB_INVALID_HANDLE)
    {
        m_KeyScanHandle = expusb_helper_open(EXP_USB_DEV_NAME);
        EXP_INFO("EXP Usbdrive KeyScanHandle[%d]", m_KeyScanHandle);
    }
}

DsskeyID CUSBDrive::KeyID2DsskeyID(KeyID keyId, ExpID expId)
{
    int iKeyAfterMap = -1;
    switch (keyId)
    {
    case EXP50_KEY_1:
        iKeyAfterMap = 0;
        break;
    case EXP50_KEY_2:
        iKeyAfterMap = 2;
        break;
    case EXP50_KEY_3:
        iKeyAfterMap = 4;
        break;
    case EXP50_KEY_4:
        iKeyAfterMap = 6;
        break;
    case EXP50_KEY_5:
        iKeyAfterMap = 8;
        break;
    case EXP50_KEY_6:
        iKeyAfterMap = 10;
        break;
    case EXP50_KEY_7:
        iKeyAfterMap = 12;
        break;
    case EXP50_KEY_8:
        iKeyAfterMap = 14;
        break;
    case EXP50_KEY_9:
        iKeyAfterMap = 16;
        break;
    case EXP50_KEY_10:
        iKeyAfterMap = 18;
        break;
    case EXP50_KEY_11:
        iKeyAfterMap = 1;
        break;
    case EXP50_KEY_12:
        iKeyAfterMap = 3;
        break;
    case EXP50_KEY_13:
        iKeyAfterMap = 5;
        break;
    case EXP50_KEY_14:
        iKeyAfterMap = 7;
        break;
    case EXP50_KEY_15:
        iKeyAfterMap = 9;
        break;
    case EXP50_KEY_16:
        iKeyAfterMap = 11;
        break;
    case EXP50_KEY_17:
        iKeyAfterMap = 13;
        break;
    case EXP50_KEY_18:
        iKeyAfterMap = 15;
        break;
    case EXP50_KEY_19:
        iKeyAfterMap = 17;
        break;
    case EXP50_KEY_20:
        iKeyAfterMap = 19;
        break;
    //底层给的翻页键值是反过来的
    case EXP50_KEY_PAGE1:
        iKeyAfterMap = EXP50_KEY_PAGE3;
        break;
    case EXP50_KEY_PAGE2:
        iKeyAfterMap = EXP50_KEY_PAGE2;
        break;
    case EXP50_KEY_PAGE3:
        iKeyAfterMap = EXP50_KEY_PAGE1;
        break;
    default:
        iKeyAfterMap = keyId;
        break;
    }
    if (iKeyAfterMap != EXP_KEY_NONE)
    {
        // EXP类型左移24位+EXP序号左移6位+EXP按键编号
        return ((DT_EXP_50 << 24) + (expId << 6) + iKeyAfterMap);
    }
    return EXP_KEY_NONE;
}

bool CUSBDrive::PostMsgWithRetry(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return PostMsgWithRetry(uMsg, wParam, lParam, 0, NULL);
}

bool CUSBDrive::PostMsgWithRetry(UINT uMsg, WPARAM wParam, LPARAM lParam, int nExtraSize,
                                 LPCVOID lpExtraData)
{
    bool bRet = PostMsgToApp(uMsg, wParam, lParam, nExtraSize, lpExtraData);

    if (!bRet)
    {
        int iInterval = configParser_GetConfigInt(kszExpInsertRequestInterval);
        EXP_WARN("msg post failed,send again [%d]~~~~", iInterval);

        using namespace yl;
        iInterval = max(iInterval, 50);
        iInterval = min(iInterval, 500);
        etlSleep(iInterval);

        bRet = PostMsgToApp(uMsg, wParam, lParam, nExtraSize, lpExtraData);

        EXP_WARN("msg resend [%s],ignore~~~~", bRet ? "sucess" : "failed");
    }
    return bRet;
}

void CUSBDrive::StartKeyScan()
{
    return;
}

void CUSBDrive::StartInsertScan()
{
    TaskAction_CreateSpecialThreadByMethod(InsertScanThread, TA_THREAD_USBEXP_INSERT_SCAN);
}

void CUSBDrive::InitDrive()
{
    InitKeyScan();
    InitInsertScan();

}

void CUSBDrive::InitExpByIndex(ExpID expId)
{
    return;
}

void CUSBDrive::OnUSBExpDownloadApp(DEVICE_TYPE_DEF devType, ExpID expId)
{
    EXP_INFO("Exp Device:onUSBExpDownloadApp start");

    //如果还没有加载好文件，则进行加载
    if (!m_bFileLoaded)
    {
        LoadUSBExpAppFile();
        if (!m_bFileLoaded)
        {
            return;
        }
    }

    EXP_INFO("Exp Device: expusb_helper_memory_write, ExpIndex[%d], AppBuffer[%p].",
             expId, m_ExpAppBuffer);

    //将文件下载到第nExpIndex台
    int nRet = expusb_helper_memory_write(m_KeyScanHandle, expId, 4, 0x00, m_ExpAppBuffer, m_lFileSize);
    if (nRet < 0)
    {
        return;
    }

    //操作第nExpIndex台进行引导
    nRet = expusb_helper_bootm(m_KeyScanHandle, expId, 0, 0, NULL);
    if (nRet < 0)
    {
        return;
    }

    EXP_INFO("Exp Device:onUSBExpDownloadApp end.");
}

int CUSBDrive::GetExpStatus(ExpID expId)
{
    if (expId < MAX_EXP_NUM && expId >= 0)
    {
        return m_nExpStatus[expId];
    }
    return DEVICELIB_EXPUSB_MODE_NONE;
}

CUSBDrive::CUSBDrive()
{
    m_KeyScanHandle = USB_INVALID_HANDLE;
    m_InsertScanHandle = USB_INVALID_HANDLE;
    m_bFileLoaded = false;
    for (int i = 0; i < MAX_EXP_NUM; i++)
    {
        m_nExpStatus[i] = 0;
    }
    m_lFileSize = 0;
    m_ExpAppBuffer = NULL;
    m_eDriveType = EXP_DRIVE_TYPE_USB;
}

CUSBDrive::CUSBDrive(const CUSBDrive &)
{

}

CUSBDrive::~CUSBDrive()
{

}

void CUSBDrive::LoadUSBExpAppFile()
{
    EXP_INFO("Exp Device:LoadUSBExpAppFile start");

    FILE * pFile;
    size_t result;

    pFile = fopen(EXP_USB_APP_FILE, "rb");
    if (pFile == NULL)
    {
        return;
    }

    fseek(pFile, 0, SEEK_END);
    m_lFileSize = ftell(pFile);
    rewind(pFile);
    fseek(pFile, 0, SEEK_SET);

    m_ExpAppBuffer = (char *)malloc(sizeof(char) * m_lFileSize);
    if (m_ExpAppBuffer == NULL)
    {
        fclose(pFile);
        return;
    }

    result = fread(m_ExpAppBuffer, 1, m_lFileSize, pFile);
    if (result != m_lFileSize)
    {
        fclose(pFile);
        free(m_ExpAppBuffer);
        return;
    }

    fclose(pFile);
    m_bFileLoaded = true;

    EXP_INFO("Exp Device:LoadUSBExpAppFile end");
}

ExpHandle CUSBDrive::GetExpHandle()
{
	return m_KeyScanHandle;
}

#endif

