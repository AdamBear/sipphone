#ifndef _SCREENSAVERS_H_
#define _SCREENSAVERS_H_

#include <QtGui>
#include "screensavercommon.h"
#include "screensavercontroller.h"

#include "uimanager/basedialog.h"

#include "mainwndhelp/statusiconsbar.h"
#include "uikernel/qidleclock.h"
#if IF_FEATURE_XMLB_SCREENSAVER
#include "extended/xmlbrowser/include/logicanduicommon.h"
#endif


class qIdleClock;

class CDlgScreenSavers : public CBaseDialog
{
    Q_OBJECT

public:
    CDlgScreenSavers(QWidget * parent = 0);
    ~CDlgScreenSavers();

    static LRESULT OnMessage(msgObject & refMessage);

    // 返回该窗口指针
    static CBaseDialog * CreateInstance();

    // 状态改变通知响应函数
    static void StatusNotifyFunction(const YLList<IdleStatusItem> & ylVecStatusList);

    static void StatusChangeCallback(STATUS_ACTION eAction);

    // 初始化
    virtual void Init();

    // 释放数据，将窗.口清空回复到初始化状态
    virtual void Uninit();

    virtual QPixmap GetCustomBackGround();

    //事件过滤器
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

    virtual void showEvent(QShowEvent * e);
    virtual void hideEvent(QHideEvent * e);

    virtual bool IsShowHomeButton()
    {
        return false;
    }
    virtual bool IsShowBackButton()
    {
        return false;
    }
    virtual bool IsShowSoftkey()
    {
        return false;
    }
    virtual bool IsShowTitle(TitleLayoutInfo & objInfo)
    {
        return false;
    }
    virtual bool IsShowDsskey(bool & bExpend)
    {
        bExpend = false;
        return false;
    }

    void OnStatusChange(YLList<IdleStatusItem> listStatusList);

    // 是否允许鼠标事件通过
    virtual bool AllowMouseEventOnKeyLock(QObject * pObject, QMouseEvent * pEvent)
    {
        return true;
    }

    // 是否允许键盘事件通过
    virtual bool AllowKeyPadEventOnKeyLock(int nKeyCode, bool bPress)
    {
        return true;
    }

    virtual bool IsAllowKeyguardCount()
    {
        return true;
    }

    // 设置显示背景
    void SetShowBackGround(bool bShow);

#if IF_FEATURE_XMLB_SCREENSAVER
    //初始化xmlbrowser模式相关控件
    bool InitXmlWigdet(void * pData);
#endif

protected:
    virtual void paintEvent(QPaintEvent * p);

private:
    void InitWigdet();

    //背景变换计时器
    void StartBGChangeTimer();
    void StopBGChangeTimer();

    //控制时钟移动计时器
    void StartMoveTimer();
    void StopMoveTimer();

    //开始变化
    void StartChanging();

    void ClockChangeToRandomRect();

    bool IsShowClock();
    bool IsStartChangePic();

    void ResetClockTimeFmt();

#if IF_FEATURE_XMLB_SCREENSAVER
    QRect GetXMLTimeRect(IMAGE_VERTICAL_ALIGN eVertical, IMAGE_HORIZONTAL_ALIGN eHorizontal);

    QRect GetXMLStatusIconRect(IMAGE_VERTICAL_ALIGN eVertical, IMAGE_HORIZONTAL_ALIGN eHorizontal);

    void SetXMLTimeAlignment(IMAGE_HORIZONTAL_ALIGN eHorizontal);

    void SetXMLStatusIconAlignment(IMAGE_HORIZONTAL_ALIGN eHorizontal);
    //插入图片时返回图片纵坐标
    int InitLineAlone(XBScreenSaverData * pData, int iImageInsertLine = 0, int iImageHeight = 0);

    bool InitTimeIcon(XBScreenSaverData * pSSData);
    //初始化xmlbrowser下载的图片
    bool InitXmlImage(XBScreenSaverData * pData);
    //清空m_listLine内容
    void ClearLineList();

    QColor GetChangeToRGB(yl::string strRGB);
#endif

private slots:
    //当时钟移动计时器时间到，移动时钟位置
    void OnMoveTimeOut();
    //当时钟显示格式配置改变
    void OnClockFormatChange();
    //当背景变化时间到，开始改变背景（先改变透明度，到最不透明时更换背景，再减少透明度）
    void OnChangeBGTimeOut();
    //当透明度计时器时间到，透明度进行step的变化
    void OnChange();

    //当收到配置改变消息时，需要强制改变背景
    void OnScreenSaverPicChange(bool bStopTimer);

    //当收到配置改变消息时，根据条件判断是否改变背景
    void OnScreenSaverPicChangeByFirst();

    //当收到配置改变消息时，根据条件判断是否移动时钟
    void OnScreenSaverClockMoveByFirst();

    //当透明度变化到最不透明时，实际上的更换背景的操作
    void OnReallyChangeBG();

    void OnBgIntervalChange();
    void OnMoveTimeIntervalChange();

    //停止变化
    void StopChanging();

    void CheckShowClock();

    void RegisterMsg();
    void UnRegisterMsg();

    void RefreshBackGroundPic();

    void OnScreenSaverModeOrURLChange();

private:
    //时钟+状态图标
    QFrame          m_frmBG;
    //时钟
    qIdleClock      m_ClockLab;
    //状态图标
    CStatusIconsBar     m_TitleIcons;

    QLabel              m_labelLoadError;

    QTimer              m_timerMove;
    QTimer              m_timerChangeBG;
    QTimer              m_timerBGOpacity;

    //当前正在使用的背景路径
    yl::string          m_strBGPic;
    yl::string          m_strOldBGPic;
    QPixmap             m_pmBG;

#if SCREEN_SAVE_FEATRUE_CLOCK_MOVING
    //时钟移动方向
    //是否向上
    bool                    m_bRightwards;
    //是否向右
    bool                    m_bUpwards;
#endif

    bool                m_bShowBackGround;

    bool                    m_bFirstSetBGChangeTime;
    bool                    m_bFirstSetClockMoveTime;

#if IF_FEATURE_XMLB_SCREENSAVER
    bool                    m_bEnterXMLSS; //是否进入过xmlbrowser屏保
    QFrame                  m_frmXML;
    qDateTimeLabel          m_XMLDateTime;  // 时间控件
    CStatusIconsBar         m_XMLTitleIcons;
    QLabel                  m_labelImage; //显示图片label
    YLList<QLabel *>         m_listLine;  //显示文字列表
#endif

};

#endif // _SCREENSAVERS_H_
