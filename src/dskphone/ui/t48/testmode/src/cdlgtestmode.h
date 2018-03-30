#ifndef CTESTDIALOG_H
#define CTESTDIALOG_H

#include "uimanager/basedialog.h"
#include <QtGui/QLabel>
#include "hpdev.h"
#include "testmode_common.h"
#include "service_ipvp.h"
class CAutoVolumeBar;

/*
自定义画线，四个参数分别表示:
控件位置和大小
父控件
线条宽度
线条颜色
*/
class DrawLine : public QWidget
{
    Q_OBJECT
public:
    DrawLine(QRect rect, QWidget * parent, int nLineWidth, QColor color) ;

    ~DrawLine();

    void SetPoint(int x, int y, int x2, int y2);
protected:
    void paintEvent(QPaintEvent * pEvent);          //在paintEvent中实现画线

private:
    //存入画笔使用到的参数
    QColor m_verColor;
    QRect  m_rect;
    int m_nWidth;
    bool m_bUsed;
};

/*
自定义CheckBox的各种效果，四个参数分别表示:
父控件
控件序号
控件位置大小
控件标签文字
*/
class TestCheckBox : public QCheckBox
{
    Q_OBJECT
public:
    //CheckBox的重置，包括可选，未选中状态，标签字体颜色
    QPixmap m_unchecked;
    QPixmap m_disabled;
    virtual void paintEvent(QPaintEvent * event);
    void Reset();
    TestCheckBox(QWidget * parent);
    ~TestCheckBox();
};

//封装的控件，点击CheckBox时，显示点击完成的图片及标签文字变色
class CLabClickCheck : public QLabel
{
    Q_OBJECT

public:
    CLabClickCheck(QWidget * parent, int nIndex, QRect rect);
    void Reset();
signals:
    void CheckBoxCheckOut();

private:
    void SetLabTextColor(QColor color);
protected slots:
    void OnClickCheckBox(int nIndex);
private:
    TestCheckBox  * m_pCheckBox;
    QLabel     *    m_pLab;

};
class CDlgTestMode : public CBaseDialog
{
    Q_OBJECT

public:
    static CBaseDialog * CreateInstance()
    {
        return new CDlgTestMode();
    }

public:
    CDlgTestMode(QWidget * parent = 0);
    ~CDlgTestMode();

    // 初始化
    virtual void Init();
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();
    virtual bool eventFilter(QObject * obj, QEvent * ev);
    virtual bool IsShowSoftkey()
    {
        return false;
    }
    virtual bool IsShowTitle(TitleLayoutInfo & objInfo)
    {
        return false;
    }
    virtual bool IsShowBackButton()
    {
        return false;
    }
    virtual bool IsShowHomeButton()
    {
        return false;
    }
    void SetFrontColor(const QColor & color);
    void SetDisplayText(const QString & strContent, bool bCenter = false, int fontsize = -1);
    void SetDisplayTextInRect(const QString & strText, QRect rectText, QColor clrTx);
    void FullScreenByColor(const QColor & color);
    void FullScreenByPic(const QString & strPic);
    // 黑白渐变
    void FullScreenByGradient();
    CAutoVolumeBar * GetVolumeBar();
    //void SetSize(int width, int height);
    void InitData();
    void SetSize(int width, int height);
    void showUSBText(bool visible)
    {
        m_USBText->setVisible(visible);
    }
    void setUSBText(QString content, QColor col);
    void showAllCheckBox(bool visible);

    void showTitle(bool visible)
    {
        m_pLabelTitle->setVisible(visible);
    }
    void SetAllHide();
    void SetFrameAndTitleVisible(TEST_STATE eType);
    void  SetLabTitle(bool visible);
    void ResetCheckBox();
    void SetTitleHide();

    void SetEasyTouch(bool bShow, QPoint point, int nSize);
    void EasyTouchChange(bool bShow, bool bTouched);
    bool getBeTouched()
    {
        return m_bTouched;
    }

public slots:
    void OnCheckBox();
signals:
    void CheckBoxAllChecked();
public Q_SLOTS:
    void    OnVolumeBarChange(int iValue);

private:
    QLabel m_Label;
    QLabel m_LabelInRect;

    QLabel m_LabelPoint;
    QPoint      m_TouchPoint;
    int         m_nTouchSize;
    bool            m_bTouched;

    CAutoVolumeBar * m_pVolume;
    QLabel * m_pLabelTitle;                     //标题栏，在各界面需要的时候进行设置               //全局字体
    QLabel * m_USBText;
    vector<CLabClickCheck *> m_pLabClickCheck;      //存放12个CheckBox
    DrawLine * m_pLine;                         //标题栏的分隔线
    //主窗体上的2个窗体，轮流切换显示
    //KeyboradTestFrame* frmKeyBoard;
    //TouchScreenTestFrame* frmTouchScreen;
    QPixmap m_picColorBar;

    int m_iNumOfCheckBox;
};

#endif // TESTDIALOG_H
