#include "test.h"
#include "record_inc.h"
#include "../include/modrecord.h"
#include "hotplug/commondefine.h"

#ifdef _RECORD_TEST_

IMPLEMENT_GETINSTANCE(CRecordTest)

static LRESULT OnMessage(msgObject & msg)
{
    bool bHandled = true;
    switch (msg.message)
    {
    case TM_TIMER:
        {
            bHandled = _RecordTest.OnTimer(msg.wParam);
        }
        break;
    case RECORDTEST_MESSAGE_DEVICE_PLUS:
        {
            _RecordTest.ProcessDevicePlus(msg.wParam);
        }
        break;
    case RECORDTEST_MESSAGE_DEVICE_REMOVE:
        {
            _RecordTest.ProcessDeviceRemove();
        }
        break;
    case RECORDTEST_MESSAGE_DEVICE_GET_STORAGE_INFO:
        {
            _RecordTest.ProcessDeviceGetStorageInfo();
        }
        break;
    case RECORDTEST_MESSAGE_DEVICE_GET_STORAGE_STATE:
        {
            _RecordTest.ProcessDeviceGetStorageState();
        }
        break;
    case RECORDTEST_MESSAGE_DEVICE_GET_RESOURCE_LIST:
        {
            _RecordTest.ProcessDeviceGetResourceList(msg.wParam);
        }
        break;
    case RECORDTEST_MESSAGE_DEVICE_DELETE_RESOURCE:
        {
            _RecordTest.ProcessDeviceDeleteResource(msg.wParam, (char *)msg.GetExtraData());
        }
        break;
    case RECORDTEST_MESSAGE_DEVICE_SET_DEFAULT_WALLPAPER:
        {
            _RecordTest.ProcessDeviceSetDefaultWallpaper((char *)msg.GetExtraData());
        }
        break;
    case RECORDTEST_MESSAGE_DEVICE_VIDEO_SCREEN_SHOT:
        {
            _RecordTest.ProcessDeviceScreenShot((char *)msg.GetExtraData());
        }
        break;
    case RECORDTEST_MESSAGE_DEVICE_RECORD:
        {
            _RecordTest.ProcessDeviceRecord(msg.wParam, msg.lParam, (char *)msg.GetExtraData());
        }
        break;
    case RECORDTEST_MESSAGE_DEVICE_PLAY:
        {
            _RecordTest.ProcessDevicePlay(msg.wParam, msg.lParam, (char *)msg.GetExtraData());
        }
        break;
    case RECORDTEST_MESSAGE_DEVICE_GET_RECORD_STATE:
        {
            _RecordTest.ProcessDeviceGetRecordState();
        }
        break;
    default:
        bHandled = false;
        break;
    }

    return bHandled;
}

CRecordTest::CRecordTest()
{
    m_nRecognizeRet = 0;
    m_strDir = TEMP_PATH "udisk1";
    APP_INFO("yuh ------Please mount usbDir to %s", m_strDir.c_str());

    m_iDetectingTimerID = 4000;
    m_iRecortingTimerID = 3000;

    m_lstSize.clear();

    SingleMsgReg(TM_TIMER, OnMessage);
    RangeMsgReg(RECORDTEST_MESSAGE_BEGIN, RECORDTEST_MESSAGE_END, OnMessage);

    APP_INFO("yuh ------RecordTest All Support Message Begin------");

    int i = 0;
    yl::string strOutBuffer;
    for (unsigned int uMsg = RECORDTEST_MESSAGE_DEVICE_PLUS; uMsg <= RECORDTEST_MESSAGE_DEVICE_PLAY;
            ++uMsg)
    {
        TransMessageToString(uMsg, strOutBuffer);
        APP_INFO("Message%d [%s] [%0x]", ++i, strOutBuffer.c_str(), uMsg);
    }

    APP_INFO("yuh ------RecordTest All Support Message End------");
}

CRecordTest::~CRecordTest()
{
}

bool CRecordTest::OnTimer(unsigned int iTimerID)
{
    if ((UINT)&m_iDetectingTimerID == iTimerID)
    {
        timerKillThreadTimer((UINT)&m_iDetectingTimerID);

        if (m_nRecognizeRet == 0)
        {
            APP_INFO("yuh ------Usb Recognize Sucess");
            etl_NotifyAppEx(false, HOTPLUG_USB_STORAGE_STATE_CHANGING, 1, 0, m_strDir.length() + 1,
                            m_strDir.c_str());
        }

        return true;
    }
    else if ((UINT)&m_iRecortingTimerID == iTimerID)
    {
        if (0 < m_lstSize.size())
        {
            etl_NotifyAppEx(false, RECORD_DEVICE_VOLUME_CHANGE, m_lstSize.front(), 0, m_strDir.length() + 1,
                            m_strDir.c_str());
            m_lstSize.erase(m_lstSize.begin());
        }
    }

    return false;
}

void CRecordTest::ProcessDevicePlus(int nRet)
{
    APP_INFO("yuh ------Usb Plus");

    m_nRecognizeRet = nRet;
    timerSetThreadTimer((UINT)&m_iDetectingTimerID, m_iDetectingTimerID);

    etl_NotifyApp(false, HOTPLUG_USB_DEVICER_STATE_CHANGE, 1, 0);
}

void CRecordTest::ProcessDeviceRemove()
{
    APP_INFO("yuh ------Usb Remove");

    timerKillThreadTimer((UINT)&m_iDetectingTimerID);
    etl_NotifyApp(false, HOTPLUG_USB_DEVICER_STATE_CHANGE, 0, 0);
    etl_NotifyApp(false, HOTPLUG_USB_STORAGE_STATE_CHANGING, 0, 0);
}

void CRecordTest::ProcessDeviceGetStorageInfo()
{
    StorageInfo info;
    _RecordManager.GetStorageInfo(info);

    APP_INFO("yuh ------GetStorageInfo Begin------");

    yl::string strOutBuff;
    TransSizeToString(info.m_ullTotal, strOutBuff);
    APP_INFO("TotalSize = [%s]", strOutBuff.c_str());
    TransSizeToString(info.m_ullFree, strOutBuff);
    APP_INFO("FreeSize = [%s]", strOutBuff.c_str());
    TransSizeToString(info.m_ullUsed, strOutBuff);
    APP_INFO("UsedSize = [%s]", strOutBuff.c_str());
    TransTimeToString(info.m_ulAudioRecordTime, strOutBuff);
    APP_INFO("Audio Record Time = [%s]", strOutBuff.c_str());
    TransTimeToString(info.m_ulVideoRecordTime, strOutBuff);
    APP_INFO("Video Record Time = [%s]", strOutBuff.c_str());

    bool bRet1 = modRecord_IsStorageFull(RECORD_NONE);
    bool bRet2 = modRecord_IsStorageFull(RECORD_PICTURE);
    bool bRet3 = modRecord_IsStorageFull(RECORD_AUDIO);
    bool bRet4 = modRecord_IsStorageFull(RECORD_VIDEO);

    APP_INFO("IsStorageFull, NONE = [%d], VIDEO = [%d], AUDIO = [%d], PICTURE = [%d]", bRet1, bRet4,
             bRet3, bRet2);

    APP_INFO("yuh ------GetStorageInfo end-------");
}

void CRecordTest::ProcessDeviceGetStorageState()
{
    STORAGE_STATE eState = _RecordManager.GetStorageState();

    switch (eState)
    {
    case STATE_DISCONNECT:
        {
            APP_INFO("yuh ------GetUsbState = [%s]", "STATE_DISCONNECT");
        }
        break;
    case STATE_DETECTING:
        {
            APP_INFO("yuh ------GetUsbState = [%s]", "STATE_DETECTING");
        }
        break;
    case STATE_CONNECT:
        {
            APP_INFO("yuh ------GetUsbState = [%s]", "STATE_CONNECT");
        }
        break;
    default:
        {
            APP_INFO("yuh ------GetUsbState Failed");
        }
        break;
    }
}

void CRecordTest::ProcessDeviceGetRecordState()
{
    yl::string strOutBuff;
    for (int i = STATE_RECORDING; i <= STATE_PLAY_PAUSE; ++i)
    {
        if (_RecordManager.IsInRecordState((RECORD_STATE)i, RECORD_NONE))
        {
            switch (i)
            {
            case STATE_RECORDING:
                {
                    APP_INFO("yuh ------GetRecordState = [%s]", "STATE_RECORDING");
                }
                break;
            case STATE_PLAYING:
                {
                    APP_INFO("yuh ------GetRecordState = [%s]", "STATE_PLAYING");
                }
                break;
            case STATE_PLAY_PAUSE:
                {
                    APP_INFO("yuh ------GetRecordState = [%s]", "STATE_PLAY_PAUSE");
                }
                break;
            default:
                break;
            }
            return ;
        }
    }

    APP_INFO("yuh ------GetRecordState = [%s]", "STATE_IDLE");
}

void CRecordTest::ProcessDeviceGetResourceList(int nRecordType)
{
    YLList<RecordFileInfo> lstFileInfo;
    _RecordManager.GetRecordList(lstFileInfo, (RECORD_TYPE)nRecordType);

    yl::string strOutBuffer;
    TransRecordTypeToString((RECORD_TYPE)nRecordType, strOutBuffer);

    APP_INFO("yuh ------GetResoucrceList Begin------");

    if (0 == lstFileInfo.size())
    {
        APP_INFO("");
        APP_INFO("yuh -----%s List is empty", strOutBuffer.c_str());
    }
    else
    {
        yl::string strTmp;
        YLList<RecordFileInfo>::iterator itr = lstFileInfo.begin();
        for (int i = 1; itr != lstFileInfo.end(); itr++, ++i)
        {
            APP_INFO("");
            APP_INFO("yuh ------%s%d Info------", strOutBuffer.c_str(), i);
            APP_INFO("FileName = [%s]", (*itr).strName.c_str());
            APP_INFO("FilePath = [%s]", (*itr).strPath.c_str());
            TransSizeToString((*itr).lSize, strTmp);
            APP_INFO("FileSize = [%s]", strTmp.c_str());
            TransTimeToString((*itr).lTime, strTmp);
            APP_INFO("FileTime = [%s]", strTmp.c_str());
            TransTimeToString((*itr).modifyTime, strTmp);
            APP_INFO("FileModTime = [%s]", strTmp.c_str());
        }
    }

    APP_INFO("");
    APP_INFO("yuh ------GetResoucrceList end------");
}

void CRecordTest::ProcessDeviceDeleteResource(int nRecordType, const char * pFileName)
{
    if (NULL == pFileName
            || strcmp(pFileName, "") == 0)
    {
        APP_INFO("yuh ------Please Input FileName!!");
        return ;
    }

    yl::string strOutBuffer;
    TransRecordTypeToString((RECORD_TYPE)nRecordType, strOutBuffer);

    if (modRecord_DeleteResoucrce((RECORD_TYPE)nRecordType, pFileName))
    {
        APP_INFO("yuh ------Delete %s(%s) Success", strOutBuffer.c_str(), pFileName);
    }
    else
    {
        APP_INFO("yuh ------Delete %s(%s) Failed", strOutBuffer.c_str(), pFileName);
    }
}

void CRecordTest::ProcessDeviceSetDefaultWallpaper(const char * pFileName)
{
    if (NULL == pFileName
            || strcmp(pFileName, "") == 0)
    {
        APP_INFO("yuh ------Please Input FileName!!");
        return ;
    }

    if (modRecord_SetDefaultWallPaper(pFileName))
    {
        APP_INFO("yuh ------Set %s Wallpaper Success", pFileName);
    }
    else
    {
        APP_INFO("yuh ------Set %s Wallpaper Failed", pFileName);
    }
}

void CRecordTest::ProcessDeviceScreenShot(const char * pFileName)
{
    if (modRecord_Screenshot(pFileName))
    {
        APP_INFO("yuh ------ScreenShot Success");
    }
    else
    {
        APP_INFO("yuh ------ScreenShot Failed");
    }
}

void CRecordTest::ProcessDeviceRecord(int nRecordType, int nOperation, const char * pFileName)
{
    yl::string strOutBuffer;
    TransRecordTypeToString((RECORD_TYPE)nRecordType, strOutBuffer);

    switch (nOperation)
    {
    case 0:
        {
            if (modRecord_RecordStart((RECORD_TYPE)nRecordType, pFileName))
            {
                m_lstSize.clear();
                m_lstSize.push_back(245760000);
                m_lstSize.push_back(76800000);
                m_lstSize.push_back(61440000);
                m_lstSize.push_back(23040000);

                timerSetThreadTimer((UINT)&m_iRecortingTimerID, m_iRecortingTimerID);

                APP_INFO("yuh ------%s Record Start Sucess", strOutBuffer.c_str());
            }
            else
            {
                APP_INFO("yuh ------%s Record Start Failed", strOutBuffer.c_str());
            }
        }
        break;
    default:
        {
            if (modRecord_RecordStop((RECORD_TYPE)nRecordType))
            {
                m_lstSize.clear();
                timerKillThreadTimer((UINT)&m_iRecortingTimerID);

                APP_INFO("yuh ------%s Record end Sucess", strOutBuffer.c_str());
            }
            else
            {
                APP_INFO("yuh ------%s Record end Failed", strOutBuffer.c_str());
            }
        }
        break;
    }
}

void CRecordTest::ProcessDevicePlay(int nRecordType, int nOperation, const char * pFileName)
{
    if (NULL == pFileName
            || strcmp(pFileName, "") == 0)
    {
        APP_INFO("yuh ------Please Input FileName!!");
        return ;
    }

    yl::string strOutBuffer;
    TransRecordTypeToString((RECORD_TYPE)nRecordType, strOutBuffer);

    switch (nOperation)
    {
    case 0:
        {
            if (modRecord_PlayStart((RECORD_TYPE)nRecordType, pFileName))
            {
                APP_INFO("yuh ------Playing %s Success", pFileName);
            }
            else
            {
                APP_INFO("yuh ------Playing %s Failed", pFileName);
            }
        }
        break;
    case 2:
        {
            if (modRecord_PlayPause((RECORD_TYPE)nRecordType, pFileName, 0))
            {
                APP_INFO("yuh ------PlayPause %s Success", pFileName);
            }
            else
            {
                APP_INFO("yuh ------PlayPause %s Failed", pFileName);
            }
        }
        break;
    case 3:
        {
            if (modRecord_PlayResume((RECORD_TYPE)nRecordType, pFileName))
            {
                APP_INFO("yuh ------PlayResume %s Success", pFileName);
            }
            else
            {
                APP_INFO("yuh ------PlayResume %s Failed", pFileName);
            }
        }
        break;
    case 4:
        {
            if (modRecord_PlaySeek((RECORD_TYPE)nRecordType, pFileName, 5))
            {
                APP_INFO("yuh ------PlaySeek to %d(s) Success", 5);
            }
            else
            {
                APP_INFO("yuh ------PlaySeek to %d(s) Failed", pFileName);
            }
        }
        break;
    default:
        {
            if (modRecord_PlayStop((RECORD_TYPE)nRecordType, pFileName))
            {
                APP_INFO("yuh ------PlayStop %s Success", pFileName);
            }
            else
            {
                APP_INFO("yuh ------PlayStop %s Failed", pFileName);
            }
        }
        break;
    }
}

void CRecordTest::TransSizeToString(const unsigned long long & ullSize, yl::string & strOutBuff)
{
#define BSize (1024)
#define KBSize (1024l*1024l)
#define MBSize (1024l*1024l*1024l)

    if (ullSize <= 0)
    {
        strOutBuff = "0B";
        return ;
    }

    char szBuff[64] = {0};
    // GB
    if (ullSize > MBSize)
    {
        sprintf(szBuff, "%.1f", (double)ullSize / MBSize);
        strcat(szBuff, "GB");
    }
    // MB
    else if (ullSize > KBSize)
    {
        sprintf(szBuff, "%.1f", (double)ullSize / KBSize);
        strcat(szBuff, "MB");
    }
    // KB
    else if (ullSize > BSize)
    {
        sprintf(szBuff, "%.1f", (double)ullSize / BSize);
        strcat(szBuff, "KB");
    }
    // B
    else
    {
        sprintf(szBuff, "%.1d", ullSize);
        strcat(szBuff, "B");
    }

    strOutBuff = szBuff;
}

void CRecordTest::TransTimeToString(const unsigned long & lTime, yl::string & strOutBuff)
{
    UINT uHour, uMinute, uSecond;
    uHour = lTime / 3600;
    uMinute = (lTime / 60) % 60;
    uSecond = lTime % 60;

    strOutBuff = commonAPI_FormatString("%.2d:%.2d:%.2d", uHour, uMinute, uSecond);
}

void CRecordTest::TransRecordTypeToString(RECORD_TYPE eType, yl::string & strOutBuff)
{
    switch (eType)
    {
    case RECORD_PICTURE:
        {
            strOutBuff = "Picture";
        }
        break;
    case RECORD_AUDIO:
        {
            strOutBuff = "Audio";
        }
        break;
    case RECORD_VIDEO:
        {
            strOutBuff = "Video";
        }
        break;
    default:
        break;
    }
}

void CRecordTest::TransMessageToString(unsigned int uMsg, yl::string & strOutBuff)
{
    switch (uMsg)
    {
    case RECORDTEST_MESSAGE_DEVICE_PLUS:
        {
            strOutBuff = "RECORDTEST_MESSAGE_DEVICE_PLUS";
        }
        break;
    case RECORDTEST_MESSAGE_DEVICE_REMOVE:
        {
            strOutBuff = "RECORDTEST_MESSAGE_DEVICE_REMOVE";
        }
        break;
    case RECORDTEST_MESSAGE_DEVICE_GET_STORAGE_INFO:
        {
            strOutBuff = "RECORDTEST_MESSAGE_DEVICE_GET_STORAGE_INFO";
        }
        break;
    case RECORDTEST_MESSAGE_DEVICE_GET_STORAGE_STATE:
        {
            strOutBuff = "RECORDTEST_MESSAGE_DEVICE_GET_STORAGE_STATE";
        }
        break;
    case RECORDTEST_MESSAGE_DEVICE_GET_RECORD_STATE:
        {
            strOutBuff = "RECORDTEST_MESSAGE_DEVICE_GET_RECORD_STATE";
        }
        break;
    case RECORDTEST_MESSAGE_DEVICE_GET_RESOURCE_LIST:
        {
            strOutBuff = "RECORDTEST_MESSAGE_DEVICE_GET_RESOURCE_LIST";
        }
        break;
    case RECORDTEST_MESSAGE_DEVICE_DELETE_RESOURCE:
        {
            strOutBuff = "RECORDTEST_MESSAGE_DEVICE_DELETE_RESOURCE";
        }
        break;
    case RECORDTEST_MESSAGE_DEVICE_SET_DEFAULT_WALLPAPER:
        {
            strOutBuff = "RECORDTEST_MESSAGE_DEVICE_SET_DEFAULT_WALLPAPER";
        }
        break;
    case RECORDTEST_MESSAGE_DEVICE_VIDEO_SCREEN_SHOT:
        {
            strOutBuff = "RECORDTEST_MESSAGE_DEVICE_VIDEO_SCREEN_SHOT";
        }
        break;
    case RECORDTEST_MESSAGE_DEVICE_RECORD:
        {
            strOutBuff = "RECORDTEST_MESSAGE_DEVICE_RECORD";
        }
        break;
    case RECORDTEST_MESSAGE_DEVICE_PLAY:
        {
            strOutBuff = "RECORDTEST_MESSAGE_DEVICE_PLAY";
        }
        break;
    default:
        break;
    }
}

#endif
