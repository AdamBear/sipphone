//mod-base
#include "volumebar.h"

//mod-ImageManager
#include "imagemanager/modimagemanager.h"
#include "picpath.h"

//mod-this(Platform Style)
#include "qwidgetutility.h"
#include "quipushbutton.h"

//提供单元像素值（20 + 10）
#define VOLUME_UNIT_PIXEL 14

//约定5为邻界值，用于比较鼠标点击的位置
#define VOLUME_MIN_POS 5
#define VOLUME_MAX_POS 185


//默认构造时，设置的音量变化步长值为1
CVolProgressBar::CVolProgressBar(QWidget * parent, int iStep /* = 1 */)
    : QProgressBar(parent)
{
    this->setObjectName("CVolProgressBar");

    //加载音量满图片
    m_bmpFull = THEME_GET_BMP(PIC_VOLUME_FULL);

    //加载音量零图片
    m_bmpEmpty = THEME_GET_BMP(PIC_VOLUME_EMPTY);

    //设置控制条步长值
    m_iStep = iStep;

    //设置控制条文本内容不可见
    setTextVisible(false);
}

int CVolProgressBar::SetStep(int iStep)
{
    //设置每点击一次增加的音量值,按设定步长增加
    if (iStep > 0)
    {
        int iOldStep = m_iStep;
        m_iStep = iStep;
        return iOldStep;
    }
    return -1;
}

int CVolProgressBar::GetStep()
{
    //获得音量值
    return m_iStep;
}

int CVolProgressBar::AddValue()
{
    int nValue = value();
    //增加音量值
    setValue(nValue + m_iStep);
    return nValue;
}

int CVolProgressBar::MinusValue()
{
    int nValue = value();
    //减少音量值
    setValue(nValue - m_iStep);
    return nValue;
}

//每次值变化进行重画
void CVolProgressBar::paintEvent(QPaintEvent * pPaintEvent)
{
    QPainter painter(this);
    QRect rcBar = m_bmpFull.rect();
    //首先画上音量为空的图片，总是画上
    //校正位置：使得音量图居中
    // 减8个像素是为了美观
    int nX = (this->width() - rcBar.width()) / 2;
    int nY = (this->height() - rcBar.height()) / 2;

    painter.drawPixmap(nX, nY, m_bmpEmpty);
    if (value() > 0 && maximum() > 0)
    {
        //按数值所占比例画上音量满图片的部分区域到控制条上
        painter.drawPixmap(nX, nY, m_bmpFull, 0, 0,
                           rcBar.width() * value() / maximum(), rcBar.height());
    }
}

//鼠标点击获得当前值
void CVolProgressBar::mousePressEvent(QMouseEvent * pMouseEvent)
{
    if (pMouseEvent == NULL)
    {
        //若鼠标事件为空，则返回
        return;
    }

    //取得相对父控件的坐标位置
    QPoint point = pMouseEvent->pos();

    //触击点小于设定最像素值时情况
    if (point.x() < VOLUME_MIN_POS)
    {
        //把控制条值清零
        this->setValue(0);
    }
    else if (point.x() > VOLUME_MAX_POS)
    {
        setValue(maximum());
    }
    else
    {
        //把鼠标点击的坐标水平值转换成控制条相应的值
        int iValue = (point.x() / VOLUME_UNIT_PIXEL + 1) * m_iStep;
        this->setValue(iValue);
    }
    //发出鼠标操作信号
    emit mouseAction();
}

//鼠标移动并刷新控制条
void CVolProgressBar::mouseMoveEvent(QMouseEvent * pMouseEvent)
{
    if (pMouseEvent == NULL)
    {
        //若鼠标事件为空，则返回
        return;
    }
    //取得相对父控件的坐标位置
    QPoint point = pMouseEvent->pos();
    if (point.x() < VOLUME_MIN_POS)
    {
        //把控制条值清零
        this->setValue(0);
    }
    else
    {
        //把鼠标点击的坐标水平值转换成控制条相应的值
        int iValue = (point.x() / VOLUME_UNIT_PIXEL + 1) * m_iStep;
        this->setValue(iValue);
    }
    //发出鼠标操作信号
    emit mouseAction();
}

////////////////////////////////////////////////////////////////////////////////////////////
CVolumeBar::CVolumeBar(QWidget * parent)
    : QFrame(parent)
    , m_pVolProgressBar(NULL)
    , m_pBtnAdd(NULL)
    , m_pBtnMinus(NULL)
    , m_eAlignment(Qt::AlignCenter)
    , m_nLFrameMargin(VCA_FRAME_HMARGIN_DEF)
    , m_nRFrameMargin(VCA_FRAME_HMARGIN_DEF)
{
    //!!!!! 是否会影响VPX视频的音量调节
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this->setObjectName(QString::fromUtf8("CVolumeBar"));

    //设置音量调节控件背景
    //T3X上的音量条不用背景
    qWidgetUtility::SetBackGround(this, "");

    //设置样式
    this->setFrameStyle(QFrame::NoFrame);

    //创建音量控制条
    CreateVolumeBar();
}

CVolumeBar::~CVolumeBar()
{
}

void CVolumeBar::UpdateLayout()
{
    Qt::Alignment eAlignment = this->alignment();

    QRect rcWidget = geometry();
    int iX = 0, iY = 0;

    //根据选择的布局位置类型来设置控件布局
    switch (eAlignment & Qt::AlignHorizontal_Mask)
    {
    case Qt::AlignLeft:
        {
            //左样式控件布局
            iX = m_nLFrameMargin;
            iY = 0;
            //设置减少按钮坐标
            if (m_pBtnMinus != NULL
                    && m_pBtnMinus->isVisible())
            {
                //按钮可见
                iY = (rcWidget.height() - VCA_BTN_HEIGHT_DEF) / 2;
                iY = iY < 0 ? 0 : iY;
                m_pBtnMinus->setGeometry(QRect(iX, iY, VCA_BTN_WIDTH_DEF, VCA_BTN_WIDTH_DEF));
                iX += VCA_BTN_WIDTH_DEF;
            }
            //设置进度条坐标
            if (GetProgressBar() != NULL)
            {
                iX += VCA_PROGESSBAR_HMARGIN_DEF;
                iY = (rcWidget.height() - VCA_PROGESSBAR_HEIGHT_DEF) / 2;
                iY = iY < 0 ? 0 : iY;
                GetProgressBar()->setGeometry(QRect(iX, iY, VCA_PROGESSBAR_WIDTH_DEF, VCA_PROGESSBAR_HEIGHT_DEF));
            }
            //设置添加按钮坐标
            if (m_pBtnAdd != NULL
                    && m_pBtnAdd->isVisible())
            {
                //按钮可见
                iX += (VCA_PROGESSBAR_WIDTH_DEF + VCA_PROGESSBAR_HMARGIN_DEF);
                iY = (rcWidget.height() - VCA_BTN_HEIGHT_DEF) / 2;
                iY = iY < 0 ? 0 : iY;
                m_pBtnMinus->setGeometry(QRect(iX, iY, VCA_BTN_WIDTH_DEF, VCA_BTN_WIDTH_DEF));
            }
        }
        break;
    case Qt::AlignRight:
        {
            //右样式控件布局
            iX = rcWidget.width() - m_nRFrameMargin - VCA_FRAME_HMARGIN_DEF;
            iY = 0;
            //设置添加按钮坐标
            if (m_pBtnAdd != NULL
                    && m_pBtnAdd->isVisible())
            {
                //按钮可见
                iX -= VCA_BTN_WIDTH_DEF;
                iY = (rcWidget.height() - VCA_BTN_HEIGHT_DEF) / 2;
                iY = iY < 0 ? 0 : iY;
                m_pBtnMinus->setGeometry(QRect(iX, iY, VCA_BTN_WIDTH_DEF, VCA_BTN_WIDTH_DEF));
            }
            //设置进度条坐标
            if (GetProgressBar() != NULL)
            {
                iX -= (VCA_PROGESSBAR_WIDTH_DEF + VCA_PROGESSBAR_HMARGIN_DEF);
                iY = (rcWidget.height() - VCA_PROGESSBAR_HEIGHT_DEF) / 2;
                iY = iY < 0 ? 0 : iY;
                GetProgressBar()->setGeometry(QRect(iX, iY, VCA_PROGESSBAR_WIDTH_DEF, VCA_PROGESSBAR_HEIGHT_DEF));
            }
            //设置减少按钮坐标
            if (m_pBtnAdd != NULL
                    && m_pBtnAdd->isVisible())
            {
                //按钮可见
                iX -= (VCA_BTN_WIDTH_DEF + VCA_PROGESSBAR_HMARGIN_DEF);
                iY = (rcWidget.height() - VCA_BTN_HEIGHT_DEF) / 2;
                iY = iY < 0 ? 0 : iY;
                m_pBtnMinus->setGeometry(QRect(iX, iY, VCA_BTN_WIDTH_DEF, VCA_BTN_WIDTH_DEF));
            }
        }
        break;
    case Qt::AlignHCenter:
    default:
        {
            //居中样式布局
            iX = m_nLFrameMargin;
            iY = 0;
            //设置进度条坐标
            if (GetProgressBar() != NULL)
            {
                iX += (rcWidget.width() - m_nLFrameMargin - m_nRFrameMargin - VCA_PROGESSBAR_WIDTH_DEF) / 2;
                iY = (rcWidget.height() - VCA_PROGESSBAR_HEIGHT_DEF) / 2;
                iY = iY < 0 ? 0 : iY;
                GetProgressBar()->setGeometry(QRect(iX, iY, VCA_PROGESSBAR_WIDTH_DEF, VCA_PROGESSBAR_HEIGHT_DEF));
            }
            //设置减少按钮坐标
            if (m_pBtnMinus != NULL
                    && m_pBtnMinus->isVisible())
            {
                //按钮可见
                int iMinusX = iX - (VCA_BTN_WIDTH_DEF + VCA_PROGESSBAR_HMARGIN_DEF);
                iY = (rcWidget.height() - VCA_BTN_HEIGHT_DEF) / 2;
                iY = iY < 0 ? 0 : iY;
                m_pBtnMinus->setGeometry(QRect(iMinusX, iY, VCA_BTN_WIDTH_DEF, VCA_BTN_WIDTH_DEF));
            }
            //设置添加按钮坐标
            if (m_pBtnAdd != NULL
                    && m_pBtnAdd->isVisible())
            {
                //按钮可见
                int iAddX = iX + (VCA_PROGESSBAR_WIDTH_DEF + VCA_PROGESSBAR_HMARGIN_DEF);
                iY = (rcWidget.height() - VCA_BTN_HEIGHT_DEF) / 2;
                iY = iY < 0 ? 0 : iY;
                m_pBtnMinus->setGeometry(QRect(iAddX, iY, VCA_BTN_WIDTH_DEF, VCA_BTN_WIDTH_DEF));
            }
        }
        break;
    }
}

void CVolumeBar::CreateVolumeBar()
{
    //创建进度条
    GetProgressBar();
    //默认Button不可见
    //  SetControlButtonVisible(false);
}

CVolProgressBar * CVolumeBar::GetProgressBar()
{
    if (m_pVolProgressBar == NULL)
    {
        m_pVolProgressBar = new CVolProgressBar(this);
        if (m_pVolProgressBar != NULL)
        {
            m_pVolProgressBar->setObjectName(QString::fromUtf8("VolProgressBar"));

            //设置焦点代理
            m_pVolProgressBar->setFocusProxy(this);

            m_pVolProgressBar->setValue(0);

            //默认情况设置有效值0到10
            m_pVolProgressBar->setRange(0, 10);

            //设置步长
            m_pVolProgressBar->SetStep(1);

            //当控制条值改变时，对父窗体发送信号
            connect(m_pVolProgressBar, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged(int)));

            //控件条的鼠标操作信号与音量条鼠标操作信号相连
            connect(m_pVolProgressBar, SIGNAL(mouseAction()), this, SIGNAL(mouseAction()));
        }
    }

    return m_pVolProgressBar;
}

qUIPushButton * CVolumeBar::GetMinusButton()
{
    if (m_pBtnMinus == NULL)
    {
        //创建音量减的按钮
        m_pBtnMinus = new qUIPushButton(this);
        if (m_pBtnMinus != NULL)
        {
            m_pBtnMinus->setObjectName(QString::fromUtf8("BtnMinus"));
            m_pBtnMinus->setText("");

            //设置焦点策略为：不获取焦点
            m_pBtnMinus->setFocusPolicy(Qt::NoFocus);

            //设置三态图标!!!
            /*
            m_pBtnMinus->set3StatePic(QString(BMP_BTN_MINUS_BG_01),
            QString(BMP_BTN_MINUS_BG_02),
            QString(BMP_BTN_MINUS_BG_03));
            */

            connect(m_pBtnMinus, SIGNAL(clicked()), this, SLOT(on_btn_Minus_clicked()));
        }
    }

    return m_pBtnMinus;
}

qUIPushButton * CVolumeBar::GetAddButton()
{
    if (m_pBtnMinus == NULL)
    {
        //创建音量增加的按钮
        m_pBtnAdd = new qUIPushButton(this);

        if (m_pBtnAdd != NULL)
        {
            m_pBtnAdd->setObjectName(QString::fromUtf8("BtnAdd"));
            m_pBtnAdd->setText("");

            //设置焦点策略为：不获取焦点
            m_pBtnAdd->setFocusPolicy(Qt::NoFocus);

            //设置三态图标!!!
            /*
            m_pBtnAdd->set3StatePic(QString(BMP_BTN_ADD_BG_01),
            QString(BMP_BTN_ADD_BG_02),
            QString(BMP_BTN_ADD_BG_03));
            */

            connect(m_pBtnAdd, SIGNAL(clicked()), this, SLOT(on_btn_Add_clicked()));
        }
    }

    return m_pBtnAdd;
}

//更新焦点外观
void CVolumeBar::RefreshFocusAppearance()
{
    return;
}

void CVolumeBar::OnBtnAddClicked()
{
    //插槽函数：实现音量值增加
    AddValue();
}

void CVolumeBar::OnBtnMinusClicked()
{
    //插槽函数：实现音量值减少
    MinusValue();
}

//获取对齐标志
Qt::Alignment CVolumeBar::alignment() const
{
    return m_eAlignment;
}

//设置对齐标志
void CVolumeBar::setAlignment(Qt::Alignment eAlignment)
{
    if (eAlignment != m_eAlignment)
    {
        m_eAlignment = eAlignment;
        UpdateLayout();
    }
}

int CVolumeBar::SetValue(int iValue)
{
    if (GetProgressBar() != NULL)
    {
        int nValue = GetProgressBar()->value();
        GetProgressBar()->setValue(iValue);
        return nValue;
    }
    return -1;
}

int CVolumeBar::GetValue()
{
    if (GetProgressBar() != NULL)
    {
        //取得控制条当前数值
        return GetProgressBar()->value();
    }
    return -1;
}

int CVolumeBar::AddValue()
{
    if (GetProgressBar() != NULL)
    {
        //增加音量值
        return GetProgressBar()->AddValue();
    }
    return -1;
}

int CVolumeBar::MinusValue()
{
    if (GetProgressBar() != NULL)
    {
        //减少音量值
        return GetProgressBar()->MinusValue();
    }
    return -1;
}

int CVolumeBar::SetStep(int iStep)
{
    if (GetProgressBar() != NULL)
    {
        //设置步长值
        return GetProgressBar()->SetStep(iStep);
    }
    return -1;
}

int CVolumeBar::GetStep()
{
    if (GetProgressBar() != NULL)
    {
        //获取步长值
        return GetProgressBar()->GetStep();
    }
    return -1;
}

void CVolumeBar::SetRange(int iMin, int iMax)
{
    if (GetProgressBar() != NULL)
    {
        //设置控制条有效值范围
        GetProgressBar()->setRange(iMin, iMax);
    }
}

void CVolumeBar::GetRange(int & iMin, int & iMax)
{
    if (GetProgressBar() != NULL)
    {
        //取得控制条有效值范围
        iMin = GetProgressBar()->minimum();
        iMax = GetProgressBar()->maximum();
    }
}

void CVolumeBar::SetImage(VOLUMEBAR_CTRL_ELEMENT eCtrlElement, const QString & strFileNormal,
                          const QString & strFileActive, const QString & strFileDisabled)
{
    switch (eCtrlElement)
    {
    case VCE_ADD_BUTTON:
        {
            //增加按钮
            if (GetAddButton() != NULL)
            {
                GetAddButton()->set3StatePic(strFileNormal, strFileActive, strFileDisabled);
            }
        }
        break;
    case VCE_MINUS_BUTTON:
        {
            //减少按钮
            if (GetMinusButton() != NULL)
            {
                GetMinusButton()->set3StatePic(strFileNormal, strFileActive, strFileDisabled);
            }
        }
        break;
    case VCE_PROGRESSBAR:
        {
            //进度条
            /*
            if (GetMinusButton() != NULL)
            {
            GetMinusButton()->set3StatePic(strFileNormal, strFileActive, strFileDisabled);
            }
            */
        }
        break;
    default:
        break;
    }
}

//设置控制按钮可见性
void CVolumeBar::SetControlButtonVisible(bool bVisible)
{
    if (bVisible)
    {
        //设置按钮可见
        if (GetAddButton() != NULL)
        {
            GetAddButton()->show();
        }

        if (GetMinusButton() != NULL)
        {
            GetMinusButton()->hide();
        }
    }
    else
    {
        //设置按钮不可见
        if (m_pBtnAdd != NULL)
        {
            //只有已初始化过，才设置隐藏
            m_pBtnAdd->hide();
        }

        if (m_pBtnMinus != NULL)
        {
            //只有已初始化过，才设置隐藏
            m_pBtnMinus->hide();
        }
    }

    //刷新布局
    UpdateLayout();
}

//设置音量条间距
void CVolumeBar::SetVolumeBarMargin(int nLMargin, int nRMargin)
{
    bool bChanged = false;
    if (nLMargin >= 0)
    {
        m_nLFrameMargin = nLMargin;
        bChanged = true;
    }
    if (nRMargin >= 0)
    {
        m_nRFrameMargin = nRMargin;
        bChanged = true;
    }
    if (bChanged)
    {
        UpdateLayout();
    }
}

//控件大小改变事件
void CVolumeBar::resizeEvent(QResizeEvent * pResizeEvent)
{
    //更新界面布局
    UpdateLayout();
    QFrame::resizeEvent(pResizeEvent);
}
