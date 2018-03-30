#ifndef __AUDIO_RECORD_H__
#define __AUDIO_RECORD_H__

#ifdef IF_USB_SUPPORT
#include "baserecord.h"

class CAudioRecord : public CBaseRecord
{
public:
    CAudioRecord();
    virtual ~CAudioRecord();

    // 操作执行
    virtual bool DoOperation(OPERATION_TYPE eOprType, const char * pFile = NULL,
                             void * pExtData = NULL, int iId = STORAGE_DEFAULT);

    // 消息处理
    virtual bool ProcessMsg(msgObject & msg);

    // 空间转换成时长
    virtual long GetAvailableTime(unsigned long long dSize);

    static void UpdateChannelInfo(CHANNEL_MODE & eMode);

private:
    // 文件夹名
    virtual yl::string GetFolder();

    // 额外的文件夹名
    virtual yl::string GetFolderEx();

    // 后缀名
    virtual yl::string GetSuffix();

    // 支持的格式
    virtual yl::string GetSupportFormat();

    // 设置状态
    void SetRecordState(RECORD_STATE eState);

    // 设置音频播放状态
    void SetRecordPlayState(RECORD_STATE eState);

    // 设置暂停的位置
    void SetPausePos(int iPausePos, const char * pFile);

    // 处理Idle录制
    void ProcessIdleRecord(OPERATION_TYPE eOprType);

	inline bool IsAACFormat()
	{
		return (PT_T49 == devicelib_GetPhoneType() || PT_CP920 == devicelib_GetPhoneType());
	}
private:
    int m_iWarningLevel;

};

#endif // IF_USB_RECORD
#endif // __AUDIO_RECORD_H__
