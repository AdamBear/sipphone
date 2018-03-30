#include "cdlgtestmode.h"
#include "interfacedefine.h"
#include "testphonet4x.h"
#include "ETLLib/ETLLib.hpp"
#include "keymap.h"
#include "devicelib/phonedevice.h"
#include "globalinputhook/modinputhook.h"
#include "uimanager/uimanager_common.h"
#include "uimanager/moduimanager.h"
#include "uimanager/rectdefine.h"
#include <QtCore/QTimerEvent>
#include <QtGui/QLinearGradient>
#include <QtGui/QVBoxLayout>
#include <QtGui/QKeyEvent>
#include "testexp.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "modtestmode.h"
#include "dsklog/log.h"
#include "service_ipvp.h"
#include "mainwnd/autovolumebar.h"
#include "exp/include/modexp.h"


bool isInEXPTest = false;
static bool TestModeHookCallbackFun(int nKeyCode, bool bPress);//按键回调函数

static LRESULT OnTestModeMessage(msgObject & objMsg);

CDlgTestMode::CDlgTestMode(QWidget * parent)
    : CBaseDialog(parent)
{
    m_pVolume = NULL;//音量条
    m_pLabelTitle  = NULL;
    m_USBText = NULL;
    m_pLine = NULL;
    SetWindowType(eWT_TestMode);
    //设置为顶层窗体属性
    SetTopWnd();
    //不会自动return to idle
    SetRejectReturnToIdle();
    m_Label.setParent(this);
    m_Label.setAutoFillBackground(true);

    m_LabelInRect.setParent(this);
    m_LabelInRect.setAutoFillBackground(true);
    m_LabelInRect.hide();
    m_LabelInRect.setWordWrap(true);

    m_LabelPoint.setParent(this);
    m_LabelPoint.setAutoFillBackground(true);
    m_LabelPoint.hide();

    installEventFilter(&m_LabelPoint);
    m_bTouched = false;

    inputHook_RegisterKeyPadHookProc(TestModeHookCallbackFun,
                                     HOOK_MODULE_TESTMODE);//设置测试模式按键钩子
    SetSize(PHONE_WIDTH, PHONE_HEIGHT);
    GetVolumeBar();
    //  connect(GetVolumeBar(), SIGNAL(OnValueChanged(int)), this, SLOT(OnVolumeBarChange(int)));
    InitData();
}

CDlgTestMode::~CDlgTestMode()
{
    if (m_pVolume != NULL)
    {
        m_pVolume->setParent(NULL);
        m_pVolume->deleteLater();
        m_pVolume = NULL;
    }
}

// 初始化
void CDlgTestMode::Init()
{
    etl_RegisterMsgHandle(DSK_MSG_EXT_CHANGE, DSK_MSG_EXT_CHANGE, OnTestModeMessage);
}

// 释放数据，将窗口清空回复到初始化状态
void CDlgTestMode::Uninit()
{
    etl_UnregisterMsgHandle(DSK_MSG_EXT_CHANGE, DSK_MSG_EXT_CHANGE, OnTestModeMessage);

    inputHook_UnRegisterKeyPadHookProc(TestModeHookCallbackFun);
    CTestPhoneT4X::DestoryInstance();

}

// 黑白渐变
void CDlgTestMode::FullScreenByGradient()
{
    QPalette pal = m_Label.palette();
    QLinearGradient linearGradient(0, 0, 0, height());
    linearGradient.setColorAt(0.0, Qt::black);
    linearGradient.setColorAt(1.0, Qt::white);
    pal.setBrush(QPalette::Window, linearGradient);
    m_Label.setPalette(pal);
}

void CDlgTestMode::FullScreenByColor(const QColor & color)
{
    QPalette pal = m_Label.palette();
    pal.setBrush(QPalette::Background, QColor(color));
    m_Label.setPalette(pal);
}

void CDlgTestMode::FullScreenByPic(const QString & strPic)
{
    QPalette pal = m_Label.palette();
    QPixmap pixPic;
    pixPic.load(strPic);
    pal.setBrush(QPalette::Background, pixPic);
    m_Label.setPalette(pal);
}

void CDlgTestMode::SetFrontColor(const QColor & color)
{
    QPalette pal = m_Label.palette();
    pal.setBrush(QPalette::WindowText, QColor(color));
    m_Label.setPalette(pal);
}

void CDlgTestMode::SetDisplayText(const QString & strContent, bool bCenter, int fontsize)
{
    m_LabelInRect.hide();

    QFont font = m_Label.font();
    if (fontsize > 0)
    {
        font.setPointSize(fontsize);
        if (bCenter)
        {
            m_Label.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }
        else
        {
            m_Label.setAlignment(Qt::AlignTop | Qt::AlignLeft);
        }
    }
    else
    {
        if (bCenter)
        {
            font.setPointSize(50);
            m_Label.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        }
        else
        {
            font.setPointSize(30);
            m_Label.setAlignment(Qt::AlignTop | Qt::AlignLeft);
        }

    }

    m_Label.setWordWrap(true);
    m_Label.setFont(font);
    m_Label.setText(strContent);
}
void CDlgTestMode::SetSize(int width, int height)
{
    setGeometry(0, 0, width, height);
    m_Label.setGeometry(0, 0, width, height);
}

void CDlgTestMode::SetDisplayTextInRect(const QString & strText, QRect rectText, QColor clrTx)
{
    m_LabelInRect.show();

    QPalette pal = m_LabelInRect.palette();
    pal.setBrush(QPalette::Background, QColor(255, 255, 255));
    pal.setBrush(QPalette::WindowText, clrTx);
    m_LabelInRect.setPalette(pal);

    QFont font = m_LabelInRect.font();
    font.setPointSize(20);
    m_LabelInRect.setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    m_LabelInRect.setWordWrap(true);
    m_LabelInRect.setFont(font);
    m_LabelInRect.setText(strText);
    m_LabelInRect.setGeometry(rectText);
}

//初始化之后所有的都是设置为不可见，后面设置可见，单独设置
void CDlgTestMode::InitData()
{
    //设置公共标题栏文字显示
    SetLabTitle(false);
    //画线，并设置位置，粗细，颜色
    m_pLine = new DrawLine(QRect(0, 80, 800, 80), this, 5, Qt::black);
    if (m_pLine == NULL)
    {
        return;
    }
    m_pLine->hide();        //设置线条不可见
    m_USBText = new QLabel(this);
    if (m_USBText == NULL)
    {
        TESTMODE_INFO("NULL ");
        return ;
    }
    m_USBText->setGeometry(150, 120, 500, 60);
    m_USBText->setAlignment(Qt::AlignCenter);
    m_USBText->hide();
    QFont font;
    font.setFamily(QString::fromUtf8("Tahoma"));
    font.setPointSize(30);
    m_USBText->setFont(font);
    //设置主背景色为白色
    this->setAutoFillBackground(true);
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Window, QBrush(Qt::white));      //设置画刷颜色为白色
    this->setPalette(palette);

    //设置各子界面的位置大小
    //frmKeyBoard = new KeyboradTestFrame(QRect(0, 0, 800, 480), this);
    //frmTouchScreen = new TouchScreenTestFrame(QRect(0, 0, 800, 480), this);

    //if (frmKeyBoard == NULL
    //  || frmTouchScreen == NULL)
    //{
    //  //确保成功申请空间
    //  return;
    //}

    //设置check box要用到的12个位置和大小
    vector<QRect> posChkBox;
    posChkBox.push_back(QRect(75, 65, 55, 30));
    posChkBox.push_back(QRect(275, 65, 55, 30));
    posChkBox.push_back(QRect(475, 65, 55, 30));
    posChkBox.push_back(QRect(675, 65, 55, 30));
    posChkBox.push_back(QRect(75, 225, 55, 30));
    posChkBox.push_back(QRect(275, 225, 55, 30));
    posChkBox.push_back(QRect(475, 225, 55, 30));
    posChkBox.push_back(QRect(675, 225, 55, 30));
    posChkBox.push_back(QRect(75, 385, 55, 30));
    posChkBox.push_back(QRect(275, 385, 55, 30));
    posChkBox.push_back(QRect(475, 385, 55, 30));
    posChkBox.push_back(QRect(675, 385, 55, 30));



    QString strTempNum;     //用于int型序号到标签显示文字的转化
    //设置各个CheckBox的属性，以及信号的连接
    for (int i = 0; i <= 11; i++)
    {
        /*m_pLabClickCheck[i] = new CLabClickCheck(this, i, posChkBox[i]);*/
        m_pLabClickCheck.push_back(new CLabClickCheck(this, i, posChkBox[i]));
        if (m_pLabClickCheck[i] == NULL)
        {
            return;
        }
        m_pLabClickCheck[i]->setFocusPolicy(Qt::NoFocus);       //CheckBox不占有焦点
        m_pLabClickCheck[i]->hide();
        connect(m_pLabClickCheck[i], SIGNAL(CheckBoxCheckOut()), this, SLOT(OnCheckBox()));
    }
}
void CDlgTestMode::setUSBText(QString content, QColor col)
{
    QPalette pal = m_USBText->palette();
    pal.setBrush(QPalette::WindowText, QColor(col));
    m_USBText->setPalette(pal);
    m_USBText->setText(content);

}
void CDlgTestMode::OnCheckBox()
{
    ++m_iNumOfCheckBox;
    if (12 == m_iNumOfCheckBox)
    {
        emit CheckBoxAllChecked();//全部按键都按完
        //响铃
    }
}
// 重置CheckBox到初始状态
void CDlgTestMode::ResetCheckBox()
{
    for (int i = 0; i <= 11; ++i)
    {
        if (NULL != m_pLabClickCheck[i])
        {
            m_pLabClickCheck[i]->Reset();
        }
    }
    m_iNumOfCheckBox = 0;
}

void CDlgTestMode::SetLabTitle(bool visible)
{
    if (m_pLabelTitle == NULL)
    {
        m_pLabelTitle = new QLabel(this);
    }
    //设置标题背景颜色，标题文字字体
    if (m_pLabelTitle != NULL)
    {
        //设置文字显示格式
        m_pLabelTitle->setAlignment(Qt::AlignCenter | Qt::AlignHCenter | Qt::AlignLeading | Qt::AlignLeft |
                                    Qt::AlignVCenter);

        //设置标题栏样式
        m_pLabelTitle->setAutoFillBackground(true);
        QPalette palette = m_pLabelTitle->palette();
        QBrush brushBlue(QColor(30, 144, 255, 255));            //设置画刷颜色

        palette.setBrush(QPalette::Window, brushBlue);
        m_pLabelTitle->setPalette(palette);
        m_pLabelTitle->setGeometry(0, 0, 800, 80);          //设置位置大小
        //设置字体
        QFont font;
        font.setFamily(QString::fromUtf8("Tahoma"));
        font.setPointSize(38);
        m_pLabelTitle->setFont(font);
        m_pLabelTitle->setVisible(visible);
    }
}

void CDlgTestMode::SetEasyTouch(bool bShow, QPoint point, int nSize)
{
    m_TouchPoint = point;
    m_nTouchSize = nSize;

    EasyTouchChange(bShow, false);
}

void CDlgTestMode::EasyTouchChange(bool bShow, bool bTouched)
{
    if (bShow)
    {
        m_LabelPoint.show();
    }
    else
    {
        m_LabelPoint.hide();
    }

    if (m_bTouched)
    {
        return;
    }

    QPixmap pix(m_nTouchSize, m_nTouchSize);

    pix.fill(QColor(255, 255, 255));

    QPainter painter(&pix);

    if (bTouched)
    {
        painter.setBrush(QBrush(Qt::red));
    }
    else
    {
        painter.setBrush(QBrush(Qt::green));
    }
    painter.drawEllipse(m_nTouchSize / 4, m_nTouchSize / 4, m_nTouchSize / 2, m_nTouchSize / 2);
    painter.end();

    m_LabelPoint.setPixmap(pix);
    m_LabelPoint.setGeometry(m_TouchPoint.x() - m_nTouchSize / 2, m_TouchPoint.y() - m_nTouchSize / 2,
                             m_nTouchSize, m_nTouchSize);

    m_bTouched = bTouched;
}

//根据当前的状态，决定什么控件显示或者不显示，以及显示的内容
void CDlgTestMode::SetFrameAndTitleVisible(TEST_STATE eType)
{
    //先隐藏所有
    SetAllHide();
    if (m_pLabelTitle == NULL || m_pLine == NULL)
    {
        return;
    }
    switch (eType)
    {
    case TS_EASY_TOUCH:
    {
        m_pLabelTitle->hide();
        m_pLine->hide();
        m_Label.setGeometry(0, 0, 800, 480);
    }
    break;
    //按键测试界面,和usb界面相比，labelTitle和line的位置有改变
    case TS_KEY:
    {
        //设置显示标题栏和键盘测试窗口
        m_pLabelTitle->setText("KeyBoard Test");
        m_pLabelTitle->resize(800, 80);
        m_pLabelTitle->show();
        m_pLine->move(0, 80);
        m_pLine->show();
        m_Label.setGeometry(0, 80, 800, 400);//显示文字的
    }
    break;
    //触屏,USB和SD测试界面
    case TS_USB:
    {
        //设置显示标题栏和触屏测试窗口
        ResetCheckBox();
        m_pLabelTitle->setText("TouchScreen Test");
        m_pLabelTitle->resize(800, 60);
        m_pLine->move(0, 60);
        m_pLine->show();
        m_pLabelTitle->show();
        m_USBText->show();
        showAllCheckBox(true);
    }
    break;
    //LCD测试界面
    case TS_LCD:
    {
        //设置隐藏标题栏
        m_pLabelTitle->hide();
        m_pLine->hide();
        m_Label.setGeometry(0, 0, 800, 480);
    }
    break;
    case TS_RANDNUM:
    {
        m_pLabelTitle->setText("RandNum Test");
        m_pLabelTitle->resize(800, 80);
        m_pLabelTitle->show();
        m_pLine->move(0, 80);
        m_pLine->show();
        m_Label.setGeometry(0, 80, 800, 400);//显示文字的

    }
    break;
    case TS_LED:
    {
        m_pLabelTitle->setText("LED Test");
        m_pLabelTitle->resize(800, 80);
        m_pLabelTitle->show();
        m_pLine->move(0, 80);
        m_pLine->show();
        m_Label.setGeometry(0, 80, 800, 400);//显示文字的
    }
    break;
    //回音测试界面
    case TS_ECHO:
    {
        m_Label.setGeometry(0, 0, PHONE_WIDTH, PHONE_HEIGHT);//显示文字的
    }
    break;
    case TS_EXP_LIGHT:
    {
        m_pLabelTitle->setText(" EXP Light Test");
        m_pLabelTitle->resize(800, 80);
        m_pLabelTitle->show();
        m_pLine->move(0, 80);
        m_pLine->show();
        m_Label.setGeometry(0, 80, 800, 400);//显示文字的

    }
    break;
    case TS_EXP_LCD:
    {
        m_pLabelTitle->setText(" EXP LCD Test");
        m_pLabelTitle->resize(800, 80);
        m_pLabelTitle->show();
        m_pLine->move(0, 80);
        m_pLine->show();
        m_Label.setGeometry(0, 80, 800, 400);//显示文字的
    }
    break;
    case TS_EXP_REAL_KEY:
    {
        m_pLabelTitle->setText(" EXPKey Test");
        m_pLabelTitle->resize(800, 80);
        m_pLabelTitle->show();
        m_pLine->move(0, 80);
        m_pLine->show();
        m_Label.setGeometry(0, 80, 800, 400);//显示文字的
    }
    break;
    case TS_EXP_EXIT:
    {
        m_pLabelTitle->setText(" EXP Exit");
        m_pLabelTitle->resize(800, 80);
        m_pLabelTitle->show();
        m_pLine->move(0, 80);
        m_pLine->show();
        m_Label.setGeometry(0, 0, PHONE_WIDTH, PHONE_HEIGHT);//恢复初始原来大小
    }
    break;
    case TS_EXP_ABORT:
    {
        m_Label.setGeometry(0, 0, PHONE_WIDTH, PHONE_HEIGHT);//恢复初始原来大小
    }
    break;
    default:
        break;
    }
}

//隐藏所有界面
void CDlgTestMode::SetAllHide()
{
    showUSBText(false);
    showAllCheckBox(false);
    SetTitleHide();
}

void CDlgTestMode::showAllCheckBox(bool visible)
{
    for (int i = 0; i <= 11 ; i++)
    {
        m_pLabClickCheck[i]->setVisible(visible);
    }
}
void CDlgTestMode::SetTitleHide()
{
    m_pLine->hide();
    m_pLabelTitle->hide();
}

///////////////////////////////////////////////////////////////////////////////////////
bool TestModeHookCallbackFun(int nKeyCode, bool bPress)//lcm回调函数的调用
{
    TESTMODE_INFO("TestModeHookCallbackFun  nKeyCode %d  bPress %d ", nKeyCode, bPress);
    CDlgTestMode * pTestDlg = qobject_cast<CDlgTestMode *>(UIManager_GetPrivateDlg(DLG_CDlgTestMode));
    if (NULL != pTestDlg
            && !pTestDlg->isHidden())
    {
        return g_pTestPhone->ProcessKeyEvent(nKeyCode, bPress);
    }
    else
    {
        return false;
    }
}

LRESULT OnTestModeMessage(msgObject & objMsg)
{
    TESTMODE_INFO("MsgType: %d, lParam: %d, lParam: %d", objMsg.message, objMsg.lParam, objMsg.wParam);
    switch (objMsg.message)
    {
    case DSK_MSG_EXT_CHANGE:
    {
        //objMsg.lParam EXP number
        int expnum = exp_GetEXPNum();
        if (expnum > 1//两个以上
                && g_pTestPhone != NULL)
        {
            //等待逻辑层提交代码
            //  exp_SetExpTestMode(true);
            g_pTestPhone->StartEXPTest();
            isInEXPTest = true;
        }
        else if (expnum <  1) //一个都没有
        {
            if (isInEXPTest)
            {
                g_pTestPhone->AbortEXPTest();
                isInEXPTest = false;
            }

        }
        else
        {
            //
        }
    }
    break;
    default:
        break;
    }

    return TRUE;
}
CAutoVolumeBar * CDlgTestMode::GetVolumeBar()
{
    if (m_pVolume == NULL)
    {
        if (NULL != this)
        {
            // 初始化音量控制条，并设置相关属性
            m_pVolume = new CAutoVolumeBar(this);
            m_pVolume->setGeometry((this->width() - 290) / 2, (this->height() - 100), 290, 50);
            m_pVolume->SetVolumeBarVisible(false);
            m_pVolume->SetVolumeValue();
            m_pVolume->SetVolumeBarIcon();
            m_pVolume->SetRange(1, 15);
            m_pVolume->SetStep(1);
            m_pVolume->SetValue(8);
        }
    }
    return m_pVolume;
}
// 音量变化信号处理
void CDlgTestMode::OnVolumeBarChange(int iValue)
{
    //voice_SetVolume(iValue, VT_TONE_HANDFREE, false);
    ipvp_set_volume(mkGetHandle(), iValue);
}

bool CDlgTestMode::eventFilter(QObject * obj, QEvent * ev)
{
    if (NULL != obj && NULL != ev)
    {
        if (ev->type() == QEvent::MouseButtonPress
                && obj == &m_LabelPoint)
        {
            EasyTouchChange(true, true);
        }
    }

    return CBaseDialog::eventFilter(obj, ev);
}

void TestCheckBox::paintEvent(QPaintEvent * event)
{
    //解决会得到焦点绘制焦点矩形的问题
    setFocusPolicy(Qt::NoFocus);
    QCheckBox::paintEvent(event);
}
void TestCheckBox::Reset()
{
    this->setEnabled(true);     //设置可勾选
    this->setChecked(false);    //设置为未勾选状态
}
TestCheckBox::TestCheckBox(QWidget * parent):
    QCheckBox(parent)
{
    //重写CheckBox样式
    //  QString strStyle;
    //strStyle += "QCheckBox::indicator{ width: 27px;height: 27px;}";
    //设置未勾选时的图片
    //  strStyle += "QCheckBox::indicator:unchecked {image: url(" CHECKBOX_PATH "01.png)}";
    //设置无法选中时的图片
    //strStyle += "QCheckBox::indicator:disabled {image: url(" CHECKBOX_PATH "05.png)}";
    //this->setStyleSheet();
    this->setAutoFillBackground(true);
    m_unchecked.load(CHECKBOX_PATH_01);
    m_disabled.load(CHECKBOX_PATH_05);
    QPalette pal = palette();
    pal.setBrush(QPalette::Normal, QPalette::Button, QBrush(m_unchecked));
    pal.setBrush(QPalette::Disabled, QPalette::Button, QBrush(m_disabled));
    setPalette(pal);
    Reset();
}
TestCheckBox::~TestCheckBox() {}



DrawLine::DrawLine(QRect rect, QWidget * parent, int nLineWidth, QColor color) : QWidget(parent)
{
    if (!rect.isEmpty())
    {
        this->setGeometry(rect);        // 控件的位置和大小
    }
    //线的宽度和颜色
    m_verColor = color;
    m_nWidth = nLineWidth;
    m_bUsed = false;
}
DrawLine::~DrawLine() {}

void DrawLine::SetPoint(int x, int y, int x2, int y2)
{
    m_rect.setX(x);
    m_rect.setY(y);
    m_rect.setWidth(x2);
    m_rect.setHeight(y2);
    m_bUsed = true;
}


void DrawLine::paintEvent(QPaintEvent * pEvent)         //在paintEvent中实现画线
{
    QPainter paint(this);
    //设置画笔参数
    QPen pen;
    pen.setWidth(m_nWidth);
    pen.setColor(m_verColor);
    paint.setPen(pen);

    //画线
    if (m_bUsed)
    {
        paint.drawLine(m_rect.x(), m_rect.y(), m_rect.width(), m_rect.height());
    }
    else
    {
        paint.drawLine(0, 0, 800, 0);
    }
}


CLabClickCheck::CLabClickCheck(QWidget * parent, int nIndex, QRect rect)
    : QLabel(parent)
    , m_pCheckBox(NULL)
    , m_pLab(NULL)
{
    this->setGeometry(rect);

    m_pCheckBox = new TestCheckBox(this);

    if (m_pCheckBox != NULL)
    {
        m_pCheckBox->setGeometry(1, 1, 28, 28);
        connect(m_pCheckBox, SIGNAL(stateChanged(int)), this,
                SLOT(OnClickCheckBox(int)));  //连接槽函数
    }

    m_pLab = new QLabel(this);
    if (m_pLab != NULL)
    {
        m_pLab->setGeometry(30, 0, 25, 30);
        m_pLab->setAlignment(Qt::AlignCenter);

        QFont font;
        font.setFamily(QString::fromUtf8("Tahoma"));
        font.setPointSize(18);
        m_pLab->setFont(font);

        QString strLab;
        m_pLab->setText(strLab.setNum(nIndex + 1));
    }
}

void CLabClickCheck::Reset()
{
    if (m_pCheckBox == NULL || m_pLab == NULL)
    {
        return;
    }

    SetLabTextColor(Qt::black);

    m_pCheckBox->Reset();
}
void CLabClickCheck::SetLabTextColor(QColor color)
{
    if (m_pLab == NULL)
    {
        return;
    }
    m_pLab->setAutoFillBackground(true);
    QPalette palette = m_pLab->palette();
    palette.setBrush(QPalette::WindowText, QBrush(color));
    m_pLab->setPalette(palette);
}


void CLabClickCheck::OnClickCheckBox(int nIndex)
{
    if (m_pCheckBox != NULL && m_pLab != NULL)
    {
        if (m_pCheckBox->checkState() == Qt::Checked)
        {
            m_pCheckBox->setEnabled(false);     //设置checkbox不可操作
            //设置图片顺序文字颜色变化
            SetLabTextColor(Qt::red);
            ipvp_dtmf_play(mkGetHandle(), 'A', 0);
            this->repaint();
            emit CheckBoxCheckOut();
        }
    }
}

