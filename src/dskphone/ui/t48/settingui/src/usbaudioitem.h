#ifndef _USB_AUDIO_ITEM_H_
#define _USB_AUDIO_ITEM_H_

#include "cusbitembase.h"
#include "usbuicommon.h"
#include "usbtimelabel.h"
#include "cusbtimecounter.h"
#include "usbprocessbar.h"
#include "record/include/modrecord.h"

class CUSBAudioItem : public CUSBItemBase, public CUSBTimeCounter
{
    Q_OBJECT

public:
    CUSBAudioItem(QWidget * pParent, QWidgetPair & pair);
    virtual ~CUSBAudioItem();

    void SetPlayStatus(USB_PLAY_STATE PlayState);
    inline USB_PLAY_STATE GetPlayStatus()
    {
        return m_PlayState;
    }

    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

    virtual void LoadPlayData(long nTotalSeconds, long nCurrentSeconds);

    virtual bool ProcessKeyEvent(QObject * pObject, QEvent * pEvent);

    virtual void ResetItemInfo(const QWidgetPair & itemData);

    void SetPlayFileInfo(RecordFileInfo RecordInfo)
    {
        m_fileInfo = RecordInfo;
    }
    RecordFileInfo GetPlayFileInfo()
    {
        return m_fileInfo;
    }

public:
    void AudioItemActionPlay();
    void AudioItemActionPause();
    void AudioItemActionWait();
    void AudioItemActionResume();
    void AudioItemActionStop(bool bActivedStopped = true);
    void AudioItemActionWaitToStop();
    void AudioItemActionComplete();

    void AudioItemSelect();
signals:
    //当item被点击play按钮时
    void PlayPress(CUSBAudioItem *);
    //当item被点击stop按钮时
    void StopPress(CUSBAudioItem *);
    //当item状态改变
    void PlayStatusChange(CUSBAudioItem *);
    //当item播放失败
    void PlayFail(CUSBAudioItem *);

private Q_SLOTS:
    //鼠标操作的信号
    void mouseAction(int nMouseEvent);

    void OnOtherItemPlay(CUSBAudioItem * pItem);
    void OnOtherItemStop(CUSBAudioItem * pItem);

    void OnTimerCountOut();

    void OnProcessBarSwift(bool bIsLeftKeyPress, bool bPlay);

    void HideHintLabel();

protected:
    virtual void mousePressEvent(QMouseEvent * pEvent);
    virtual void mouseReleaseEvent(QMouseEvent * pEvent);

protected:
    virtual void paintEvent(QPaintEvent * pEvent);
    virtual void Relayout();

    virtual bool UILogicDoAction(int nAction);

private:
    void AudioItemDoAction(USB_CLICK_BUTTON_SIDE PressedButton);

    void USBAudioItemInit();
    void InitProgressBar();

private:
    //播放的进度条
    CUSBProcessBar   *  m_pProgressBar;
    //暂停小图标
    QLabel              m_PicShowPause;
    //播放结束（主动或被动）时显示的提示语：stopped或finished
    QLabel              m_strHint;
    //记录当前播放状态
    USB_PLAY_STATE      m_PlayState;
    //记录当前处理的文件信息
    RecordFileInfo      m_fileInfo;
};

#endif //_USB_AUDIO_ITEM_H_
