#ifndef __AUTO_VOLUME_BAR_H__
#define __AUTO_VOLUME_BAR_H__

//mod-ETLLib
#include <ETLLib/ETLLib.hpp>

//mod-UIKernel
#include "uikernel/volumebar.h"

#include "voice/include/modvoice.h"
//音量条自动隐藏时间
#define VOLUMEBAR_AUTO_HIDE_TIME        5000

class CAutoVolumeBar : public CVolumeBar
{
    Q_OBJECT
public:
    explicit CAutoVolumeBar(QWidget * parent = 0);
    ~CAutoVolumeBar();

    static LRESULT OnMessage(msgObject & refMessage);

public:
    //设置音量类型
    void SetVolumeValue(VOLUME_TYPE eVolType = VT_AUTO);

    //获取音量图标
    VOLUME_TYPE GetVolumeType();
    QLabel * GetVolumeBarIcon();

    //设置音量条左侧图标
    void SetVolumeBarIcon(VOLUME_TYPE eVolType = VT_AUTO);
    void SetVolumeBarIconByChannelType(CHANNEL_MODE eChannelType = CHANNEL_AUTO);

    //设置音量条可见性
    void SetVolumeBarVisible(bool bVisible);

    // 设置背景图片
    void SetVolumeBackGround(const yl::string & strPath);

    //事件过滤器
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

    // 处理案件事件
    bool ProcessKeyEvent(int nKey, bool bPress);

protected:
    // 重新启动定时器
    void StartAutoHideTimer();

    //设置音量条左侧图标
    void SetVolumeBarIcon(const QString & strIcon);
private:
    //初始化控件
    void InitWidget();

private slots:
    //音量值改变
    void OnValueChanged(int nValue);

    //自动隐藏定时器到达
    void OnAutoHideTimeOut();

    //鼠标操作的响应
    void OnMouseAction();

signals:
    //可见状态改变信号
    void visibleChanged(bool bVisible);

private:
    //自动隐藏定时器
    QTimer   *  m_pTimerAutoHide;
    //音量类型
    VOLUME_TYPE m_eVolumeType;

    //音量条左侧图标
    QLabel * m_pLabVolumeBarIcon;
};

#endif //__AUTO_VOLUME_BAR_H__
