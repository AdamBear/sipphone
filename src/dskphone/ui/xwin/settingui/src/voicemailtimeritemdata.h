#ifndef __SETTINGUI_VOICEMAIL_TIMER_ITEM_DATA_H__
#define __SETTINGUI_VOICEMAIL_TIMER_ITEM_DATA_H__

#include "settinguilogic/src/settinguibaseitemdata.h"

#if IF_FEATURE_METASWITCH_VOICEMAIL || IF_FEATURE_BROADSOFT_VOICEMAIL
enum
{
    LIST_ITEM_TYPE_VOICE_MAIL_TIMER = 0x14000000,
};

enum
{
    VMPS_STOP = 0,
    VMPS_PLAY,
    VMPS_PAUSE,
    VMPS_RESUME,
};

class CVoiceMailTimerItemData : public CSettingUIItemDataBase
{
public:
    explicit CVoiceMailTimerItemData();
    virtual ~CVoiceMailTimerItemData();

    static int GetVoiceMailTimerDataItemType();
    static CVoiceMailTimerItemData* GetVoiceMailTimerDataItem(CSettingUIItemDataBase* pDataItem);
    static CSettingUIItemDataBase* CreateVoiceMailTimerDataItem();

public:
    // 状态图标
    const yl::string& GetStatusIcon();
    void SetStatusIcon(const yl::string& strIcon);

    // 状态文本
    const yl::string& GetStatusText();
    void SetStatusText(const yl::string& strStatusText);

    // 最大时间计数
    long GetTimerMaxTicks();
    void SetTimerMaxTicks(long lTickTime);

    // 总时长
    const yl::string& GetTotalTimeString();
    void SetTotalTimeString(const yl::string& strTotal);

    // 是否正在播放
    int GetPlayStatus();
    void SetPlayStatus(int eStatus);

    // 是否正在播放
    bool IsIconShow();
    void SetIconShow(bool bShow);

    // 是否正在播放
    bool IsStatusTextShow();
    void SetStatusTextShow(bool bShow);

protected:
    yl::string      m_strIcon;; // 图标
    yl::string      m_strTotal;; // 时间总长度
    yl::string      m_strStatusText; // 时间总长度
    int             m_nPlayStatus;
    long            m_lTickTime;
    bool            m_bShowIcon;
    bool            m_bShowStatusText;
};

typedef     CVoiceMailTimerItemData*   CVoiceMailTimerItemDataPtr;
#endif

#endif //__SETTINGUI_VOICEMAIL_TIMER_ITEM_DATA_H__
