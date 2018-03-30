#ifndef __AUTO_VOLUME_BAR__
#define __AUTO_VOLUME_BAR__


#include "ETLLib/ETLLib.hpp"

//mod-VoiceChannel
#include "xwindow/xWindow.h"
#include "xwindow-kernel/include/xthreadtimer.h"

#include "baseui/include/xsliderprogressbar.h"

#include "voice/include/modvoice.h"


class CAutoVolumeBar : public xLinearLayout, public xSlotHandler, public xTimerHandler
{
public:
    CAutoVolumeBar();
    ~CAutoVolumeBar();

#if IS_COLOR
    static LRESULT OnVolumeBarMessage(msgObject & refMessage);
#endif

    //////////////////////////////////////////////////////
public:
    void ConnectVisibleChangeSlot(xSlotHandler * pHandler, slotMethod pMethod);
    void ConnectVolumeChangeSlot(xSlotHandler * pHandler, slotMethod pMethod);

    void DisconnectVisibleChangeSlot(xSlotHandler * pHandler, slotMethod pMethod);
    void DisconnectVolumeChangeSlot(xSlotHandler * pHandler, slotMethod pMethod);

    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress);

    virtual bool onPaintContent(xPainter & p);
    //////////////////////////////////////////////////////

    virtual void loadChildrenElements(xml_node & node);

    //////////////////////音量条业务////////////////////
public:
    //获取音量类型
    int GetVolumeType();

    //设置音量类型
    void SetVolumeType(int nVolType);

    //设置音量条可见性
    void SetVolumeBarVisible(bool bVisible);

    int GetVolumeValue()
    {
        return m_nVolume;
    }

    //刷新音量条
    void UpdateVolume(int nVolume);

    void ChangeVolume(bool bAdd);

    void SetAutoHideEnable(bool bAutoHide)
    {
        m_bAutoHide = bAutoHide;
    }

#if IS_COLOR
    //设置音量条左侧图标
    void SetVolumeBarIconByChannelType(CHANNEL_MODE eChannelType = CHANNEL_AUTO);
    void SetVolumeBarIcon(VOLUME_TYPE eVolType = VT_AUTO);
    void SetVolumeBarIcon(const yl::string & strIcon);
#endif

protected:
    virtual void loadAttributes(xml_node& node);

    //////////////////////////////////////////////////////

private:
    //初始化控件
    void InitWidget();

    //激活/开始 自动隐藏的计时器
    void SetHideVolumeBarTimer();

    //音量条自动消失计时器超时处理槽
    void OnHideVolumeBarTimer(xThreadTimer* timer, LPVOID pExtraData);

    //设置基本数据
    void SetVolumeData();

    void slotKeyEvent(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);
    void slotOnViewAppear(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);

    void adjustVolumeRange();

private:
    //音量类型
    int                 m_nVolumeType;

    //允许的最大最小音量
    int                 m_nMinVolume;
    int                 m_nMaxVolume;
    int                 m_nVolume;

    //音量条自动消失计时器
    xThreadTimer        m_timerHideVolumeBar;

private:
    //音量条显示状态改变信号
    xSignal             signalVisibleChange;
    //音量条音量改变信号
    xSignal             signalVolumeChange;
    bool                m_bAutoHide;

    int m_nRange;           // 进度条总值
    int m_nPosition;        // 进度条当前值
    int m_nPaintPosition;   //进度条开始绘制位置

    xImageView    *   m_pChannelIcon;
    xSliderProgressBar    *   m_pProgressBar;

    int m_nVolumeBlockLeft;
    int m_nVolumeBlockTop;

    int m_nVolumeBlockWidth;
    int m_nVolumeBlockHeight;
    int m_nVolumeBlockInterval;

};

#endif //__AUTO_VOLUME_BAR__
