#ifndef __VOLUME_BAR_H__
#define __VOLUME_BAR_H__

//mod-stl
#include <ylstring.h>

//mod-qt
#include <QtGui>

//音量条的进度栏
class CVolProgressBar : public QProgressBar
{
    Q_OBJECT
public:
    //默认构造时，设置的音量变化步长值为1
    explicit CVolProgressBar(QWidget * parent = 0, int iStep = 1);

    //音量提升
    int AddValue();

    //降低音量
    int MinusValue();

    //对外接口：设置音量调节步长
    int SetStep(int iStep);

    //取得音量调节步长
    int GetStep();

protected:
    //每次值变化进行重画
    void paintEvent(QPaintEvent * pPaintEvent);

    //鼠标点击获得当前值
    void mousePressEvent(QMouseEvent * pMouseEvent);

    //鼠标移动并刷新控制条
    void mouseMoveEvent(QMouseEvent * pMouseEvent);

signals:

    //鼠标操作的信号，包括移动和点击
    void mouseAction();

private:
    //音量为零显示的图片
    QPixmap m_bmpEmpty;

    //音量满时显示的图片
    QPixmap m_bmpFull;

    //用于基本增加的单元值
    int m_iStep;
};

class qUIPushButton;
class CVolumeBar : public QFrame
{
    Q_OBJECT
public:
    //音量条控件元素
    enum VOLUMEBAR_CTRL_ELEMENT
    {
        VCE_ADD_BUTTON = 0,
        VCE_MINUS_BUTTON,
        VCE_PROGRESSBAR,
    };

private:
    enum VOLUMEBAR_CTRL_ATTR
    {
        //Frame外边框的默认水平间距
        VCA_FRAME_HMARGIN_DEF       = 10,
        //按钮默认宽度
        VCA_BTN_WIDTH_DEF           = 32,
        //按钮默认高度
        VCA_BTN_HEIGHT_DEF          = 32,
        //进度条外边框的默认水平间距
        VCA_PROGESSBAR_HMARGIN_DEF  = 0,
        //进度条默认宽度
        VCA_PROGESSBAR_WIDTH_DEF    = 200,
        //进度条默认高度
        VCA_PROGESSBAR_HEIGHT_DEF   = 30,
    };
public:
    explicit CVolumeBar(QWidget * parent = 0);
    ~CVolumeBar();

public:
    //获取对齐标志
    Qt::Alignment alignment() const;

    //设置对齐标志
    void setAlignment(Qt::Alignment eAlignment);

    //对外接口：设置音量值
    int SetValue(int nValue);

    //对外接口：取得音量值
    int GetValue();

    //音量提升
    int AddValue();

    //降低音量
    int MinusValue();

    //对外接口：设置音量调节步长
    int SetStep(int iStep);

    //取得音量调节步长
    int GetStep();

    //对外接口：设置调节范围值
    void SetRange(int iMin, int iMax);

    //取得调节范围值
    void GetRange(int & iMin, int & iMax);

    //设置控制按钮可见性
    void SetControlButtonVisible(bool bVisible);

    //设置图片
    void SetImage(VOLUMEBAR_CTRL_ELEMENT eCtrlElement, const QString & strFileNormal,
                  const QString & strFileActive, const QString & strFileDisabled);

    //设置音量条间距
    void SetVolumeBarMargin(int nLMargin = -1, int nRMargin = -1);

protected:
    //创建音量控制条
    void CreateVolumeBar();

    //获取具体控制条
    CVolProgressBar * GetProgressBar();

    //获取音量减少按钮
    qUIPushButton * GetMinusButton();

    //获取音量增加按钮
    qUIPushButton * GetAddButton();

    //更新焦点外观
    void RefreshFocusAppearance();

    //刷新布局
    void UpdateLayout();

    //控件大小改变事件
    void resizeEvent(QResizeEvent * pResizeEvent);

signals:
    //音量值变化信号
    void valueChanged(int);

    //鼠标操作的信号
    void mouseAction();

private slots:
    //音量值增加插槽
    void OnBtnMinusClicked();

    //音量值减少插槽
    void OnBtnAddClicked();

private:
    //升降控制条
    CVolProgressBar * m_pVolProgressBar;

    //音量调节减少按钮
    qUIPushButton * m_pBtnMinus;

    //音量调节增加按钮
    qUIPushButton * m_pBtnAdd;

    //对齐标志
    Qt::Alignment   m_eAlignment;

    //左右间距
    int             m_nLFrameMargin;
    int             m_nRFrameMargin;
};

#endif // __VOLUME_BAR_H__
