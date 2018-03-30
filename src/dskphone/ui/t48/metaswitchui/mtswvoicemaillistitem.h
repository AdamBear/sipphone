#ifndef VOICEMAILLISTITEM_H
#define VOICEMAILLISTITEM_H

#include "config.h"
#if IF_FEATURE_METASWITCH_VOICEMAIL
#include "settingui/src/settingitembase.h"
#include "uicommon/uikernel/qprocessbarutility.h"
#include "ylstring.h"

enum MTSW_VOICE_MAIL_BTN
{
    MTSW_VOICE_MAIL_MORE        = 29281,
    MTSW_VOICE_MAIL_STOP        = 29282,
    MTSW_VOICE_MAIL_FINISH      = 29283,
};

class CVoiceMailListItem : public CSettingItem
{
    Q_OBJECT
public:
    CVoiceMailListItem(QWidget * parent, QString strIcon, QString strLabel
                       , QString strAction = "", bool bAddIcon = false
                       , int iDataIndex = -1, bool bAddInex = true, bool bWithStatus = false);
    CVoiceMailListItem(QWidget * pParent, QWidgetPair & pair);
    ~CVoiceMailListItem();

    void Init();

    virtual void ClearContent();

    void SetReadStatus(bool bRead)
    {
        m_bRead = bRead;
    }
    void SetPlayStatus(bool bShow)
    {
        m_bPlayStatus = bShow;
    }
    void SetShowUrgent(bool bShow)
    {
        m_bShowUrgent = bShow;
    }
    void ResetTime()
    {
        m_nPlayTime = 0;
    }

    // 开启播放计时器
    void StartPlayTimer();
    // 关闭播放计时器
    void StopPlayTimer();

    void AddIcon(LPCSTR strIconPath);

    void SetDate(QString strDate);
    void SetTime(QString strTime);

    int Time2Second(QString strTime);
protected:
    virtual void mousePressEvent(QMouseEvent * pEvent);
    virtual void mouseReleaseEvent(QMouseEvent * pEvent);

    virtual void paintEvent(QPaintEvent * pEvent);
    virtual void Relayout();

#if IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR
    virtual void ApplyNomalStyle();
#endif

private slots:
    //播放计时器
    void OnPlayTimeOut();

private:
    QLabel * m_pLabDate; //日期
    QLabel * m_pLabTime; //时间
    QLabel * m_pLabUrgent; //紧急标志

    CSliderBar m_ProgressBar;
    CButtonWrapper m_btnMore;
    QPixmap     m_iconMap;
    bool        m_bAddIcon;
    bool        m_bWithStatus;//是否显示状态
    bool        m_bRead;
    bool        m_bPlayStatus;//是否处于播放状态
    bool        m_bShowUrgent;//是否标志紧急

    QTimer         *        m_pPlayTimer;
    int                     m_nPlayTime;        // 倒计时时间
};

#endif
#endif
