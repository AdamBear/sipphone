//mod-base
#include "qprocessbarutility.h"

//mod-ImageManager
#include "uicommon/imagemanager/modimagemanager.h"
#include "picpath.h"

//mod-this(Platform Style)
#include "qwidgetutility.h"
#include "quipushbutton.h"

#define SAFE_DELETE_POINTER(p)      if ((p) != NULL) \
                                    { \
                                    delete (p); \
                                    (p) = NULL; \
                                    }


//默认构造时，设置的音量变化步长值为1
CSliderBar::CSliderBar(QWidget * parent, int iStep /* = 1 */)
    : QProgressBar(parent)
    , m_nLastValue(0)
{
    this->setObjectName("CSliderBar");

    //设置控制条步长值
    m_iStep = iStep;

    m_nHMargin = 0;

    // 设置允许触控
    setAttribute(Qt::WA_AcceptTouchEvents);
    //设置控制条文本内容不可见
    setTextVisible(false);

    m_bReadOnly = false;
}

int CSliderBar::SetStep(int iStep)
{
    //设置每点击一次增加的音量值,按设定步长增加
    if (iStep > 0)
    {
        int iOldStep = m_iStep;
        m_iStep = iStep;

        QRect rcArea = rect();
        int nWidth = rcArea.width() - (m_nHMargin * 2);
        int isteps = maximum() / m_iStep;
        m_iLenStep = nWidth / isteps;

        return iOldStep;
    }
    return -1;
}

int CSliderBar::GetStep()
{
    //获得音量值
    return m_iStep;
}

int CSliderBar::AddValue()
{
    int nValue = value();
    //增加音量值
    setValue(nValue + m_iStep);
    return nValue;
}

int CSliderBar::MinusValue()
{
    int nValue = value();
    //减少音量值
    setValue(nValue - m_iStep);
    return nValue;
}

//取得音量调节步长
void CSliderBar::SetImage(QString strEmptyPic, QString strFullPic, QString strCurPosPic)
{
    m_strPicEmpty = strEmptyPic;
    m_strPicFull = strFullPic;
    m_strPicCurPos = strCurPosPic;

    if (!strCurPosPic.isEmpty())
    {
        //加载当前控制量图片
        QPixmap pmCurPos = THEME_GET_BMP(strCurPosPic.toUtf8().data());

        if (!pmCurPos.isNull())
        {
            m_nHMargin = pmCurPos.width() / 2;
        }
    }
}

//控件大小改变事件
void CSliderBar::resizeEvent(QResizeEvent * pResizeEvent)
{
    QRect rcArea = rect();
    int nWidth = rcArea.width() - (m_nHMargin * 2);
    int isteps = maximum() / m_iStep;
    m_iLenStep = nWidth / isteps;
    QProgressBar::resizeEvent(pResizeEvent);
}

//每次值变化进行重画
void CSliderBar::paintEvent(QPaintEvent * pPaintEvent)
{
    QPainter painter(this);

    QPixmap pmEmpty;
    QPixmap pmFull;
    QPixmap pmCurPos;

    //加载音量零图片
    if (!m_strPicEmpty.isEmpty())
    {
        pmEmpty = THEME_GET_BMP(m_strPicEmpty.toUtf8().data());
    }
    else
    {
        pmEmpty = THEME_GET_BMP(PIC_VOLUME_EMPTY);
    }

    //加载音量满图片
    if (!m_strPicFull.isEmpty())
    {
        pmFull = THEME_GET_BMP(m_strPicFull.toUtf8().data());
    }
    else
    {
        pmFull = THEME_GET_BMP(PIC_VOLUME_FULL);
    }

    if (!m_strPicCurPos.isEmpty())
    {
        //加载当前控制量图片
        pmCurPos = THEME_GET_BMP(m_strPicCurPos.toUtf8().data());
    }

    QRect rcBar = pmFull.rect();
    //首先画上音量为空的图片，总是画上
    //校正位置：使得音量图居中

    int nX = 0;
    int nY = 0;

    QRect rcPos = pmCurPos.rect();
    int nWidth = this->width() - (m_nHMargin * 2);
    nX = 0 + m_nHMargin;
    nY = (this->height() - rcBar.height()) / 2;

    painter.drawPixmap(nX, nY, nWidth, rcBar.height(), pmEmpty);

    int curPos = value();
    int maxPos = maximum();

    if (curPos >= 0 && maxPos > 0)
    {
        //按数值所占比例画上音量满图片的部分区域到控制条上
        if (curPos != 0)
        {
            painter.drawPixmap(nX, nY, pmFull, 0, 0,
                               nWidth * curPos / maxPos, rcBar.height());
        }

        if (!pmCurPos.isNull())
        {
            // 有控制量图标时,绘制当前控制量位置
            nY = (this->height() - rcPos.height()) / 2;
            nX = nWidth * curPos / maxPos;
            painter.drawPixmap(nX, nY, pmCurPos, 0, 0,
                               rcPos.width(), rcPos.height());
        }
    }

}

//鼠标点击获得当前值
void CSliderBar::mousePressEvent(QMouseEvent * pMouseEvent)
{
    if (pMouseEvent == NULL || m_bReadOnly)
    {
        //若鼠标事件为空，则返回
        return;
    }

    //取得相对父控件的坐标位置
    QPoint point = pMouseEvent->pos();

    int pos = point.x() - m_nHMargin;

    {
        //把鼠标点击的坐标水平值转换成控制条相应的值
        int iValue = (pos / m_iLenStep) * m_iStep;
        this->setValue(iValue);
    }
    //发出鼠标操作信号
    emit mouseAction(QEvent::MouseButtonPress);
}

//鼠标移动并刷新控制条
void CSliderBar::mouseMoveEvent(QMouseEvent * pMouseEvent)
{
    if (pMouseEvent == NULL || m_bReadOnly)
    {
        //若鼠标事件为空，则返回
        return;
    }
    //取得相对父控件的坐标位置
    QPoint point = pMouseEvent->pos();
    int npos = point.x() - m_nHMargin;
    {
        //把鼠标点击的坐标水平值转换成控制条相应的值
        int iValue = (npos / m_iLenStep) * m_iStep;
        if (m_nLastValue != iValue)
        {
            this->setValue(iValue);
        }

        m_nLastValue = iValue;
    }
    //发出鼠标操作信号
    emit mouseAction(QEvent::MouseMove);
}

//鼠标移动并刷新控制条
void CSliderBar::mouseReleaseEvent(QMouseEvent * pMouseEvent)
{
    if (pMouseEvent == NULL || m_bReadOnly)
    {
        //若鼠标事件为空，则返回
        return;
    }
    //取得相对父控件的坐标位置
    QPoint point = pMouseEvent->pos();
    int npos = point.x() - m_nHMargin;
    {
        //把鼠标点击的坐标水平值转换成控制条相应的值
        int iValue = (npos / m_iLenStep) * m_iStep;
        this->setValue(iValue);
    }
    //发出鼠标操作信号
    emit mouseAction(QEvent::MouseButtonRelease);
}


////////////////////////////////////////////////////////////////////////////////////////////
CProcessBarUtility::CProcessBarUtility(QWidget * parent, ProcessBarType ePBType)
    : QFrame(parent)
    , m_pVolProgressBar(NULL)
    , m_pBtnAdd(NULL)
    , m_pBtnMinus(NULL)
    , m_pLabLeftIcon(NULL)
    , m_pLabRightIcon(NULL)
    , m_eAlignment(Qt::AlignCenter)
    , m_nFrameMargin(PCA_FRAME_HMARGIN_DEF)
    , m_BtnHeight(PCA_BTN_HEIGHT_DEF)
    , m_BtnWidth(PCA_BTN_WIDTH_DEF)
    , m_IconHeight(PCA_ICON_HEIGHT_DEF)
    , m_IconWidth(PCA_ICON_WIDTH_DEF)
    , m_nSliderHMarginLeft(PCA_PROGESSBAR_LMARGIN_DEF)
    , m_nSliderHMarginRight(PCA_PROGESSBAR_RMARGIN_DEF)
    , m_nSliderHeight(PCA_PROGESSBAR_HEIGHT_DEF)
    , m_nSliderWidth(PCA_PROGESSBAR_WIDTH_DEF)
    , m_ePBType(ePBType)
{
    //!!!!! 是否会影响VPX视频的音量调节
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this->setObjectName(QString::fromUtf8("CProcessBarUtility"));

    // 设置允许触控
    setAttribute(Qt::WA_AcceptTouchEvents);

    //设置音量调节控件背景
    //T3X上的音量条不用背景
    qWidgetUtility::SetBackGround(this, "");

    //设置样式
    this->setFrameStyle(QFrame::NoFrame);

    //创建音量控制条
    CreateVolumeBar();
}

CProcessBarUtility::~CProcessBarUtility()
{
    SAFE_DELETE_POINTER(m_pBtnAdd);
    SAFE_DELETE_POINTER(m_pBtnMinus);
    SAFE_DELETE_POINTER(m_pVolProgressBar);
    SAFE_DELETE_POINTER(m_pLabLeftIcon);
    SAFE_DELETE_POINTER(m_pLabRightIcon);

}

void CProcessBarUtility::UpdateLayout()
{
    Qt::Alignment eAlignment = this->alignment();

    QRect rcWidget = geometry();
    int iX = 0, iY = 0;

    switch (m_ePBType)
    {
    case PBT_BTN:
        {
            //根据选择的布局位置类型来设置控件布局
            switch (eAlignment & Qt::AlignHorizontal_Mask)
            {
            case Qt::AlignLeft:
                {
                    //左样式控件布局
                    iX = m_nFrameMargin;
                    iY = 0;
                    //设置减少按钮坐标
                    if (m_pBtnMinus != NULL
                            && m_pBtnMinus->isVisible())
                    {
                        //按钮可见
                        iY = (rcWidget.height() - m_BtnHeight) / 2;
                        iY = iY < 0 ? 0 : iY;
                        m_pBtnMinus->setGeometry(QRect(iX, iY, m_BtnWidth, m_BtnHeight));
                        iX += m_BtnWidth;
                    }
                    //设置进度条坐标
                    if (GetProgressBar() != NULL)
                    {
                        iX += m_nSliderHMarginLeft;
                        iY = (rcWidget.height() - m_nSliderHeight) / 2;
                        iY = iY < 0 ? 0 : iY;
                        GetProgressBar()->setGeometry(QRect(iX, iY, m_nSliderWidth, m_nSliderHeight));
                    }
                    //设置添加按钮坐标
                    if (m_pBtnAdd != NULL
                            && m_pBtnAdd->isVisible())
                    {
                        //按钮可见
                        iX += (m_nSliderWidth + m_nSliderHMarginRight);
                        iY = (rcWidget.height() - m_BtnHeight) / 2;
                        iY = iY < 0 ? 0 : iY;
                        m_pBtnAdd->setGeometry(QRect(iX, iY, m_BtnWidth, m_BtnHeight));
                    }
                }
                break;
            case Qt::AlignRight:
                {
                    //右样式控件布局
                    iX = rcWidget.width() - m_nFrameMargin;
                    iY = 0;
                    //设置添加按钮坐标
                    if (m_pBtnAdd != NULL
                            && m_pBtnAdd->isVisible())
                    {
                        //按钮可见
                        iX -= m_BtnWidth;
                        iY = (rcWidget.height() - m_BtnHeight) / 2;
                        iY = iY < 0 ? 0 : iY;
                        m_pBtnMinus->setGeometry(QRect(iX, iY, m_BtnWidth, m_BtnHeight));
                    }
                    //设置进度条坐标
                    if (GetProgressBar() != NULL)
                    {
                        iX -= (m_nSliderWidth + m_nSliderHMarginRight);
                        iY = (rcWidget.height() - m_nSliderHeight) / 2;
                        iY = iY < 0 ? 0 : iY;
                        GetProgressBar()->setGeometry(QRect(iX, iY, m_nSliderWidth, m_nSliderHeight));
                    }
                    //设置减少按钮坐标
                    if (m_pBtnAdd != NULL
                            && m_pBtnAdd->isVisible())
                    {
                        //按钮可见
                        iX -= (m_BtnWidth + m_nSliderHMarginLeft);
                        iY = (rcWidget.height() - m_BtnHeight) / 2;
                        iY = iY < 0 ? 0 : iY;
                        m_pBtnAdd->setGeometry(QRect(iX, iY, m_BtnWidth, m_BtnHeight));
                    }
                }
                break;
            case Qt::AlignHCenter:
            default:
                {
                    //居中样式布局
                    iX = m_nFrameMargin;
                    iY = 0;
                    //设置进度条坐标
                    if (GetProgressBar() != NULL)
                    {
                        iX += (rcWidget.width() - m_nFrameMargin - m_nFrameMargin - m_nSliderWidth) / 2;
                        iY = (rcWidget.height() - m_nSliderHeight) / 2;
                        iY = iY < 0 ? 0 : iY;
                        GetProgressBar()->setGeometry(QRect(iX, iY, m_nSliderWidth, m_nSliderHeight));
                    }
                    //设置减少按钮坐标
                    if (m_pBtnMinus != NULL
                            && m_pBtnMinus->isVisible())
                    {
                        //按钮可见
                        int iMinusX = iX - (m_BtnWidth + m_nSliderHMarginLeft);
                        iY = (rcWidget.height() - m_BtnHeight) / 2;
                        iY = iY < 0 ? 0 : iY;
                        m_pBtnMinus->setGeometry(QRect(iMinusX, iY, m_BtnWidth, m_BtnHeight));
                    }
                    //设置添加按钮坐标
                    if (m_pBtnAdd != NULL
                            && m_pBtnAdd->isVisible())
                    {
                        //按钮可见
                        int iAddX = iX + (m_nSliderWidth + m_nSliderHMarginRight);
                        iY = (rcWidget.height() - m_BtnHeight) / 2;
                        iY = iY < 0 ? 0 : iY;
                        m_pBtnAdd->setGeometry(QRect(iAddX, iY, m_BtnWidth, m_BtnHeight));
                    }
                }
                break;
            }
        }
        break;
    case PBT_ICON:
        {
            //根据选择的布局位置类型来设置控件布局
            switch (eAlignment & Qt::AlignHorizontal_Mask)
            {
            case Qt::AlignLeft:
                {
                    //左样式控件布局
                    iX = m_nFrameMargin;
                    iY = 0;
                    //设置减少按钮坐标
                    if (m_pLabLeftIcon != NULL
                            && m_pLabLeftIcon->isVisible())
                    {
                        //按钮可见
                        iY = (rcWidget.height() - m_IconHeight) / 2;
                        iY = iY < 0 ? 0 : iY;
                        m_rctLeftIcon = QRect(0, 0, m_IconWidth + m_nFrameMargin + m_nSliderHMarginLeft, rcWidget.height());
                        m_pLabLeftIcon->setGeometry(QRect(iX, iY, m_IconWidth, m_IconHeight));
                        iX += m_IconWidth;
                    }
                    //设置进度条坐标
                    if (GetProgressBar() != NULL)
                    {
                        iX += m_nSliderHMarginLeft;
                        iY = (rcWidget.height() - m_nSliderHeight) / 2;
                        iY = iY < 0 ? 0 : iY;
                        GetProgressBar()->setGeometry(QRect(iX, iY, m_nSliderWidth, m_nSliderHeight));
                    }
                    //设置添加按钮坐标
                    if (m_pLabRightIcon != NULL
                            && m_pLabRightIcon->isVisible())
                    {
                        //按钮可见
                        iX += (m_nSliderWidth + m_nSliderHMarginRight);
                        iY = (rcWidget.height() - m_IconHeight) / 2;
                        iY = iY < 0 ? 0 : iY;
                        m_rctRightIcon = QRect(iX - m_nSliderHMarginRight, 0,
                                               m_IconWidth + m_nFrameMargin + m_nSliderHMarginRight, rcWidget.height());
                        m_pLabRightIcon->setGeometry(QRect(iX, iY, m_IconWidth, m_IconHeight));
                    }
                }
                break;
            case Qt::AlignRight:
                {
                    //右样式控件布局
                    iX = rcWidget.width() - m_nFrameMargin;
                    iY = 0;
                    //设置添加按钮坐标
                    if (m_pLabRightIcon != NULL
                            && m_pLabRightIcon->isVisible())
                    {
                        //按钮可见
                        iX -= m_IconWidth;
                        iY = (rcWidget.height() - m_IconHeight) / 2;
                        iY = iY < 0 ? 0 : iY;
                        m_rctRightIcon = QRect(iX - m_nSliderHMarginRight, 0,
                                               m_IconWidth + m_nFrameMargin + m_nSliderHMarginRight, rcWidget.height());
                        m_pLabRightIcon->setGeometry(QRect(iX, iY, m_IconWidth, m_IconHeight));
                    }
                    //设置进度条坐标
                    if (GetProgressBar() != NULL)
                    {
                        iX -= (m_nSliderWidth + m_nSliderHMarginRight);
                        iY = (rcWidget.height() - m_nSliderHeight) / 2;
                        iY = iY < 0 ? 0 : iY;
                        GetProgressBar()->setGeometry(QRect(iX, iY, m_nSliderWidth, m_nSliderHeight));
                    }
                    //设置减少按钮坐标
                    if (m_pLabLeftIcon != NULL
                            && m_pLabLeftIcon->isVisible())
                    {
                        //按钮可见
                        iX -= (m_IconWidth + m_nSliderHMarginLeft);
                        iY = (rcWidget.height() - m_IconHeight) / 2;
                        iY = iY < 0 ? 0 : iY;
                        m_rctLeftIcon = QRect(iX - m_nFrameMargin, 0, m_IconWidth + m_nFrameMargin + m_nSliderHMarginLeft,
                                              rcWidget.height());
                        m_pLabLeftIcon->setGeometry(QRect(iX, iY, m_IconWidth, m_IconHeight));
                    }
                }
                break;
            case Qt::AlignHCenter:
            default:
                {
                    //居中样式布局
                    iX = m_nFrameMargin;
                    iY = 0;
                    //设置进度条坐标
                    if (GetProgressBar() != NULL)
                    {
                        iX += (rcWidget.width() - m_nFrameMargin - m_nFrameMargin - m_nSliderWidth) / 2;
                        iY = (rcWidget.height() - m_nSliderHeight) / 2;
                        iY = iY < 0 ? 0 : iY;
                        GetProgressBar()->setGeometry(QRect(iX, iY, m_nSliderWidth, m_nSliderHeight));
                    }
                    //设置减少按钮坐标
                    if (m_pLabLeftIcon != NULL
                            /*&& m_pLabLeftIcon->isVisible()*/)
                    {
                        //按钮可见
                        int iMinusX = iX - (m_IconWidth + m_nSliderHMarginLeft);
                        iY = (rcWidget.height() - m_IconHeight) / 2;
                        iY = iY < 0 ? 0 : iY;
                        m_rctLeftIcon = QRect(iMinusX - m_nFrameMargin, 0,
                                              m_IconWidth + m_nFrameMargin + m_nSliderHMarginLeft, rcWidget.height());
                        m_pLabLeftIcon->setGeometry(QRect(iMinusX, iY, m_IconWidth, m_IconHeight));
                    }
                    //设置添加按钮坐标
                    if (m_pLabRightIcon != NULL
                            /*&& m_pLabRightIcon->isVisible()*/)
                    {
                        //按钮可见
                        int iAddX = iX + (m_nSliderWidth + m_nSliderHMarginRight);
                        iY = (rcWidget.height() - m_IconHeight) / 2;
                        iY = iY < 0 ? 0 : iY;
                        m_rctRightIcon = QRect(iAddX - m_nSliderHMarginRight, 0,
                                               m_IconWidth + m_nFrameMargin + m_nSliderHMarginRight, rcWidget.height());
                        m_pLabRightIcon->setGeometry(QRect(iAddX, iY, m_IconWidth, m_IconHeight));
                    }
                }
                break;
            }
        }
        break;
    default:
        break;
    }

}

void CProcessBarUtility::CreateVolumeBar()
{
    //创建进度条
    GetProgressBar();
    //默认Button不可见
    //  SetControlButtonVisible(false);
}

CSliderBar * CProcessBarUtility::GetProgressBar()
{
    if (m_pVolProgressBar == NULL)
    {
        m_pVolProgressBar = new CSliderBar(this);
        if (m_pVolProgressBar != NULL)
        {
            m_pVolProgressBar->setObjectName(QString::fromUtf8("VolProgressBar"));

            //设置焦点代理
            m_pVolProgressBar->setFocusProxy(this);

            m_pVolProgressBar->setValue(0);

            //默认情况设置有效值0到10
            m_pVolProgressBar->setRange(0, 36);

            //设置步长
            m_pVolProgressBar->SetStep(1);

            //当控制条值改变时，对父窗体发送信号
            connect(m_pVolProgressBar, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged(int)));

            //控件条的鼠标操作信号与音量条鼠标操作信号相连
            connect(m_pVolProgressBar, SIGNAL(mouseAction(int)), this, SIGNAL(mouseAction(int)));
        }
    }

    return m_pVolProgressBar;
}

qUIPushButton * CProcessBarUtility::GetMinusButton()
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

            connect(m_pBtnMinus, SIGNAL(clicked()), this, SLOT(OnBtnMinusClicked()));
        }
    }

    return m_pBtnMinus;
}

qUIPushButton * CProcessBarUtility::GetAddButton()
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

            connect(m_pBtnAdd, SIGNAL(clicked()), this, SLOT(OnBtnAddClicked()));
        }
    }

    return m_pBtnAdd;
}

QLabel * CProcessBarUtility::GetLeftIcon()
{
    if (m_pLabLeftIcon == NULL)
    {
        //创建音量增加的按钮
        m_pLabLeftIcon = new QLabel(this);

        if (m_pLabLeftIcon != NULL)
        {
            m_pLabLeftIcon->setObjectName(QString::fromUtf8("LabLeftIcon"));
            m_pLabLeftIcon->setGeometry(0, 0,
                                        PCA_ICON_WIDTH_DEF, PCA_ICON_HEIGHT_DEF);
            //设置焦点策略为：不获取焦点
            m_pLabLeftIcon->setFocusPolicy(Qt::NoFocus);
        }
    }

    return m_pLabLeftIcon;
}

QLabel * CProcessBarUtility::GetRightIcon()
{
    if (m_pLabRightIcon == NULL)
    {
        //创建音量增加的按钮
        m_pLabRightIcon = new QLabel(this);

        if (m_pLabRightIcon != NULL)
        {
            m_pLabRightIcon->setObjectName(QString::fromUtf8("LabRightIcon"));
            m_pLabRightIcon->setGeometry(0, 0,
                                         PCA_ICON_WIDTH_DEF, PCA_ICON_HEIGHT_DEF);
            //设置焦点策略为：不获取焦点
            m_pLabRightIcon->setFocusPolicy(Qt::NoFocus);
        }
    }

    return m_pLabRightIcon;
}

//更新焦点外观
void CProcessBarUtility::RefreshFocusAppearance()
{
    return;
}

void CProcessBarUtility::OnBtnAddClicked()
{
    //插槽函数：实现音量值增加
    AddValue();
}

void CProcessBarUtility::OnBtnMinusClicked()
{
    //插槽函数：实现音量值减少
    MinusValue();
}

//获取对齐标志
Qt::Alignment CProcessBarUtility::alignment() const
{
    return m_eAlignment;
}

//设置对齐标志
void CProcessBarUtility::setAlignment(Qt::Alignment eAlignment)
{
    if (eAlignment != m_eAlignment)
    {
        m_eAlignment = eAlignment;
        UpdateLayout();
    }
}

int CProcessBarUtility::SetValue(int iValue)
{
    if (GetProgressBar() != NULL)
    {
        int nValue = GetProgressBar()->value();
        GetProgressBar()->setValue(iValue);
        return nValue;
    }
    return -1;
}

int CProcessBarUtility::GetValue()
{
    if (GetProgressBar() != NULL)
    {
        //取得控制条当前数值
        return GetProgressBar()->value();
    }
    return -1;
}

int CProcessBarUtility::AddValue()
{
    if (GetProgressBar() != NULL)
    {
        //增加音量值
        return GetProgressBar()->AddValue();
    }
    return -1;
}

int CProcessBarUtility::MinusValue()
{
    if (GetProgressBar() != NULL)
    {
        //减少音量值
        return GetProgressBar()->MinusValue();
    }
    return -1;
}

int CProcessBarUtility::SetStep(int iStep)
{
    if (GetProgressBar() != NULL)
    {
        //设置步长值
        return GetProgressBar()->SetStep(iStep);
    }
    return -1;
}

int CProcessBarUtility::GetStep()
{
    if (GetProgressBar() != NULL)
    {
        //获取步长值
        return GetProgressBar()->GetStep();
    }
    return -1;
}

void CProcessBarUtility::SetRange(int iMin, int iMax)
{
    if (GetProgressBar() != NULL)
    {
        //设置控制条有效值范围
        GetProgressBar()->setRange(iMin, iMax);
    }
}

void CProcessBarUtility::GetRange(int & iMin, int & iMax)
{
    if (GetProgressBar() != NULL)
    {
        //取得控制条有效值范围
        iMin = GetProgressBar()->minimum();
        iMax = GetProgressBar()->maximum();
    }
}

void CProcessBarUtility::SetImage(PROCESSBAR_CTRL_ELEMENT eCtrlElement,
                                  const QString & strFileNormal,
                                  const QString & strFileActive, const QString & strFileDisabled)
{
    switch (eCtrlElement)
    {
    case PCE_ADD_BUTTON:
        {
            //增加按钮
            if (GetAddButton() != NULL)
            {
                GetAddButton()->set3StatePic(strFileNormal, strFileActive, strFileDisabled);
            }
        }
        break;
    case PCE_MINUS_BUTTON:
        {
            //减少按钮
            if (GetMinusButton() != NULL)
            {
                GetMinusButton()->set3StatePic(strFileNormal, strFileActive, strFileDisabled);
            }
        }
        break;
    case PCE_PROGRESSBAR:
        {
            //进度条
            if (GetProgressBar() != NULL)
            {
                GetProgressBar()->SetImage(strFileNormal, strFileActive, strFileDisabled);
            }
        }
        break;
    case PCE_ICON_LEFT:
        {
            //  左部图标
            if (GetLeftIcon() != NULL)
            {
                QString strBmp = strFileNormal;
                QPixmap pixmap = THEME_GET_BMP(strBmp.toUtf8().data());
                GetLeftIcon()->setPixmap(pixmap);
            }
        }
        break;
    case PCE_ICON_RIGHT:
        {
            // 右部图标
            if (GetRightIcon() != NULL)
            {
                QString strBmp = strFileNormal;
                QPixmap pixmap = THEME_GET_BMP(strBmp.toUtf8().data());
                GetRightIcon()->setPixmap(pixmap);
            }
        }
        break;
    case PCE_ICON_L_AND_R:
        {
            QPixmap pixmap;
            UIKERNEL_INFO("strFileNormal(%s), strFileActive(%s).", strFileNormal.toUtf8().data(), \
                          strFileActive.toUtf8().data());
            //  左部图标
            if (GetLeftIcon() != NULL)
            {
                pixmap = THEME_GET_BMP(strFileNormal.toUtf8().data());
                GetLeftIcon()->setPixmap(pixmap);
            }

            // 右部图标
            if (GetRightIcon() != NULL)
            {
                pixmap = THEME_GET_BMP(strFileActive.toUtf8().data());
                GetRightIcon()->setPixmap(pixmap);
            }
        }
        break;
    default:
        break;
    }
}

//设置控制按钮可见性
void CProcessBarUtility::SetControlButtonVisible(bool bVisible)
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
            GetMinusButton()->show();
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

// 设置滑动条左边缘
void CProcessBarUtility::SetSliderBarMarginLeft(int nMargin)
{
    if (nMargin >= 0)
    {
        m_nSliderHMarginLeft = nMargin;
    }
}

// 设置滑动条右边缘
void CProcessBarUtility::SetSliderBarMarginRight(int nMargin)
{
    if (nMargin >= 0)
    {
        m_nSliderHMarginRight = nMargin;
    }
}

// 设置滑动条高度
void CProcessBarUtility::SetSliderHeight(int nHeight)
{
    if (nHeight >= 0)
    {
        m_nSliderHeight = nHeight;
    }
}

// 设置滑动条宽度
void CProcessBarUtility::SetSliderWidth(int nWidth)
{
    if (nWidth >= 0)
    {
        m_nSliderWidth = nWidth;
    }
}

// 设置按键高度
void CProcessBarUtility::SetBtnHeight(int nHeight)
{
    if (nHeight >= 0)
    {
        m_BtnHeight = nHeight;
    }
}

// 设置按键宽度
void CProcessBarUtility::SetBtnWidth(int nWidth)
{
    if (nWidth >= 0)
    {
        m_BtnWidth = nWidth;
    }
}

// 设置图标高度
void CProcessBarUtility::SetIconHeight(int nHeight)
{
    if (nHeight >= 0)
    {
        m_IconHeight = nHeight;
    }
}

// 设置图标宽度
void CProcessBarUtility::SetIconWidth(int nWidth)
{
    if (nWidth >= 0)
    {
        m_IconWidth = nWidth;
    }
}

// 设置滚动条外框边缘
void CProcessBarUtility::SetHMargin(int nMargin)
{
    bool bChanged = false;
    if (nMargin >= 0)
    {
        m_nFrameMargin = nMargin;
        bChanged = true;
    }

//  if (bChanged)
//  {
//      UpdateLayout();
//  }
}

//控件大小改变事件
void CProcessBarUtility::resizeEvent(QResizeEvent * pResizeEvent)
{
    //更新界面布局
    UpdateLayout();
    QFrame::resizeEvent(pResizeEvent);
}

void CProcessBarUtility::mousePressEvent(QMouseEvent * pMouseEvent)
{
    if (NULL == pMouseEvent)
    {
        return;
    }

    const QPoint & pt = pMouseEvent->pos();

    if (m_rctLeftIcon.contains(pt))
    {
        emit SlideBarIconClicked(true);
        pMouseEvent->accept();
    }
    else if (m_rctRightIcon.contains(pt))
    {
        emit SlideBarIconClicked(false);
        pMouseEvent->accept();
    }
}
