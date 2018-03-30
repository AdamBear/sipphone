#ifndef __PROCESSBAR_UTILITY_H__
#define __PROCESSBAR_UTILITY_H__

//mod-stl
#include <ylstring.h>

//mod-qt
#include <QtGui>

enum ProcessBarType
{
    PBT_BTN = 0,
    PBT_ICON,
    PBT_MAX
};

// 前向
class qUIPushButton;

//音量条的进度栏
class CSliderBar : public QProgressBar
{
    Q_OBJECT
public:
    //默认构造时，设置的音量变化步长值为1
    explicit CSliderBar(QWidget * parent = 0, int iStep = 1);

    //音量提升
    int AddValue();

    //降低音量
    int MinusValue();

    //对外接口：设置音量调节步长
    int SetStep(int iStep);

    //取得音量调节步长
    int GetStep();

    //取得音量调节步长
    void SetImage(QString strEmptyPic, QString strFullPic, QString strCurPosPic = "");

    //设置是否只读
    void SetReadOnly(bool bReadOnly)
    {
        m_bReadOnly = bReadOnly;
    }

protected:
    //每次值变化进行重画
    void paintEvent(QPaintEvent * pPaintEvent);

    //鼠标点击获得当前值
    void mousePressEvent(QMouseEvent * pMouseEvent);

    //鼠标移动并刷新控制条
    void mouseMoveEvent(QMouseEvent * pMouseEvent);

    //鼠标抬起并刷新控制条
    void mouseReleaseEvent(QMouseEvent * pMouseEvent);

    //控件大小改变事件
    void resizeEvent(QResizeEvent * pResizeEvent);

signals:

    //鼠标操作的信号，包括移动和点击
    void mouseAction(int);

private:
    //控制量为零显示的图片
    QString m_strPicEmpty;

    //控制量满时显示的图片
    QString m_strPicFull;

    //显示在控制条上当前控制量的图片
    QString m_strPicCurPos;

    int m_iLenStep;

    //用于基本增加的单元值
    int m_iStep;

    // 滑动条水平边缘
    int m_nHMargin;

    int m_nLastValue;

    bool m_bReadOnly;
};


class CProcessBarUtility : public QFrame
{
    Q_OBJECT
public:
    //音量条控件元素
    enum PROCESSBAR_CTRL_ELEMENT
    {
        PCE_ADD_BUTTON = 0,
        PCE_MINUS_BUTTON,
        PCE_PROGRESSBAR,
        PCE_ICON_LEFT,
        PCE_ICON_RIGHT,
        PCE_ICON_L_AND_R
    };

private:
    enum PROCESSBAR_CTRL_ATTR
    {
        //Frame外边框的默认水平间距
        PCA_FRAME_HMARGIN_DEF       = 10,
        //按钮默认宽度
        PCA_BTN_WIDTH_DEF           = 32,
        //按钮默认高度
        PCA_BTN_HEIGHT_DEF          = 32,
        //图标默认宽度
        PCA_ICON_WIDTH_DEF           = 40,
        //图标默认高度
        PCA_ICON_HEIGHT_DEF          = 40,
        //进度条左部外边框的默认水平间距
        PCA_PROGESSBAR_LMARGIN_DEF  = 0,
        //进度条右部外边框的默认水平间距
        PCA_PROGESSBAR_RMARGIN_DEF  = 0,
        //进度条默认宽度
        PCA_PROGESSBAR_WIDTH_DEF    = 371,
        //进度条默认高度
        PCA_PROGESSBAR_HEIGHT_DEF   = 30,
    };
public:
    explicit CProcessBarUtility(QWidget * parent = 0, ProcessBarType ePBType = PBT_ICON);
    ~CProcessBarUtility();

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
    void SetImage(PROCESSBAR_CTRL_ELEMENT eCtrlElement, const QString & strFileNormal,
                  const QString & strFileActive, const QString & strFileDisabled);

    void SetProcessBarType(ProcessBarType eType)
    {
        m_ePBType = eType;
    }

    ProcessBarType GetProcessBarType()
    {
        return m_ePBType;
    }

    // 设置滑动条边缘
    void SetSliderBarMarginLeft(int nMargin);

    // 设置滑动条边缘
    void SetSliderBarMarginRight(int nMargin);

    // 设置滑动条高度
    void SetSliderHeight(int nHeight);

    // 设置滑动条宽度
    void SetSliderWidth(int nWidth);

    // 设置滚动条外框边缘
    void SetHMargin(int nMargin);

    // 设置按键高度
    void SetBtnHeight(int nHeight);

    // 设置按键宽度
    void SetBtnWidth(int nWidth);

    // 设置图标高度
    void SetIconHeight(int nHeight);

    // 设置图标宽度
    void SetIconWidth(int nWidth);

    // 获取滑动条边缘
    int GetSliderBarMarginLeft()
    {
        return m_nSliderHMarginLeft;
    }

    // 获取滑动条边缘
    int GetSliderBarMarginRight()
    {
        return m_nSliderHMarginRight;
    }

    // 获取滑动条高度
    int GetSliderHeight()
    {
        return m_nSliderHeight;
    }

    // 获取滑动条宽度度
    int GetSliderWidth()
    {
        return m_nSliderWidth;
    }

    // 获取滚动条外框边缘
    int GetHMargin()
    {
        return m_nFrameMargin;
    }

    // 获取按键高度
    int GetBtnHeight()
    {
        return m_BtnHeight;
    }

    // 获取按键宽度
    int GetBtnWidth()
    {
        return m_BtnWidth;
    }

    // 获取图标高度
    int GetIconHeight()
    {
        return m_IconHeight;
    }

    // 获取图标宽度度
    int GetIconWidth()
    {
        return m_IconWidth;
    }

protected:
    //创建音量控制条
    void CreateVolumeBar();

    //获取具体控制条
    CSliderBar * GetProgressBar();

    //获取音量减少按钮
    qUIPushButton * GetMinusButton();

    //获取音量增加按钮
    qUIPushButton * GetAddButton();

    // 获取左部图标
    QLabel * GetLeftIcon();

    // 获取右部图标
    QLabel * GetRightIcon();

    //更新焦点外观
    void RefreshFocusAppearance();

    //刷新布局
    void UpdateLayout();

    //控件大小改变事件
    void resizeEvent(QResizeEvent * pResizeEvent);

    //鼠标点击左右按钮
    void mousePressEvent(QMouseEvent * pMouseEvent);

signals:
    //音量值变化信号
    void valueChanged(int);

    //鼠标操作的信号
    void mouseAction(int);

    void SlideBarIconClicked(bool bIsLeftIconClicked);

private slots:
    //音量值增加插槽
    void OnBtnMinusClicked();

    //音量值减少插槽
    void OnBtnAddClicked();

private:
    //升降控制条
    CSliderBar * m_pVolProgressBar;

    //音量调节减少按钮
    qUIPushButton * m_pBtnMinus;

    //音量调节增加按钮
    qUIPushButton * m_pBtnAdd;

    //对齐标志
    Qt::Alignment   m_eAlignment;
    ProcessBarType  m_ePBType;

    int             m_nSliderHMarginLeft;
    int             m_nSliderHMarginRight;

    int             m_nSliderHeight;
    int             m_nSliderWidth;

    //左侧图标
    QLabel     *    m_pLabLeftIcon;
    //右侧图标
    QLabel     *    m_pLabRightIcon;

    QRect               m_rctLeftIcon;
    QRect               m_rctRightIcon;

    // 按键宽高
    int             m_BtnHeight;
    int             m_BtnWidth;

    // 按键宽高
    int             m_IconHeight;
    int             m_IconWidth;

    // 左右间距
    int             m_nFrameMargin;
};

#endif // __PROCESSBAR_UTILITY_H__
