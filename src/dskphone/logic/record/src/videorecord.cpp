#include "record_inc.h"


#ifdef IF_SUPPORT_VIDEO
CVideoRecord::CVideoRecord()
{
    m_eType = RECORD_VIDEO;

    CreateFolder(CVideoRecord::GetFolder());
}

CVideoRecord::~CVideoRecord()
{
}

// 文件夹名
yl::string CVideoRecord::GetFolder()
{
    return "Video Record";
}

// 额外的文件夹名
yl::string CVideoRecord::GetFolderEx()
{
    return yl::string("/");
}

// 后缀名
yl::string CVideoRecord::GetSuffix()
{
    return ".mkv";
}

// 支持的格式
yl::string CVideoRecord::GetSupportFormat()
{
    return yl::string("mkv") + yl::string(",") + yl::string("avi");
}

// 操作执行
bool CVideoRecord::DoOperation(OPERATION_TYPE eOprType, const char * pFile/* = NULL*/,
                               void * pExtData/* = NULL*/, int iId /*= STORAGE_DEFAULT*/)
{
    bool bSuccess = false;

    switch (eOprType)
    {
    case OPERATION_SCREENSHOT:
        {
            yl::string strFile = GenerateFullPath(iId, "Screenshot", ".jpg", pFile);
#ifdef _T49
            bSuccess = RecordInterface_Screenshot(strFile.c_str());
#endif
        }
        break;
    default:
        {
            bSuccess = CAudioRecord::DoOperation(eOprType, pFile, pExtData, iId);
        }
        break;
    }

    return bSuccess;
}

// 空间转换成时长
long CVideoRecord::GetAvailableTime(unsigned long long dSize)
{
    long lRet = dSize / KB(133) - 120;
    return (lRet < 0 ? 0 : lRet);
}

// 消息处理
bool CVideoRecord::ProcessMsg(msgObject & msg)
{
    switch (msg.message)
    {
    case HDMI_MSG_STATUS_CHANGE:
    case HDMI_MSG_SWITCH_CHANGE:
        {
            // 未播放视频, 则不处理
            if (STATE_PLAYING != GetRecordState()
                    && STATE_PLAY_PAUSE != GetRecordState())
            {
                break;
            }

            if (1 == msg.wParam)
            {
                // 扩展屏连接上, 或开关打开
                RecordInterface_SetVideoHDMILayout();
            }
            else
            {
                RecordInterface_ClearVideoHDMILayout();
            }
        }
        break;
    case IPVP_MSG_VIDEO_SNAPSHOT_DONE:
        {
            // http://bugfree.yealink.com/Bug.php?BugID=74632
            // 等待VPM返回后，才通知UI给出截图成功的提示
            etl_NotifyAppEx(false, RECORD_STORAGE_RECORD_RESULT, 2, 
                            (0 == msg.wParam), msg.GetExtraSize(), msg.GetExtraData());
        }
        break;
    default:
        break;
    }

    return CAudioRecord::ProcessMsg(msg);
}
#endif
