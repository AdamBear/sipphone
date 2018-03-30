#include <QtGui>
#include "qsoftkeybar.h"
#include "qtcommon/qmisc.h"
#include "qwidgetutility.h"
#include "keymap.h"
#include "picpath.h"
#include "languagehelper.h"
#include "imagemanager/modimagemanager.h"
#include "baseui/t4xpicpath.h"
#include "inputmethod/inputmethod.h"
#include "include/uicommon.h"
#include "edk/include/modedk.h"
#include "customtheme/modcustomtheme.h"
//////////////////////////////////////////////////////////////////////////
#define     IME_MAXNUM      4
const QString strIME[IME_MAXNUM] = {"123", "abc", "ABC", "2aB"};
#define     QSOFTBUTTON_IME_FONT_SIZE       28
#define     QSOFTBUTTON_DEFAULT_FONT_SIZE   16
#define     QSOFTBUTTON_NORMAL_FONT_SIZE    18
//滚动间隔时间
#define _TIME_INTERVAL_SCROLL   300
//移动到端点停留次数
#define _COUNT_STOP             3
// class qSoftButton
qSoftButton::qSoftButton(const QString & strButtonName, QObject * receiver, const char * member)
{
    // 初始化数据
    m_strButtonTitle = strButtonName;
    m_pSignalReceiver = receiver;
    m_strSingalMember = QString::fromUtf8(member);
    m_pHostButton = NULL;
}

qSoftButton * qSoftButton::SetTitle(const QString & strTitle)
{
    // 设置Title
    if (m_pHostButton != NULL)
    {
        m_pHostButton->SetRealText(strTitle);
    }

    m_strButtonTitle = strTitle;
    return this;
}
qSoftButton * qSoftButton::ConnectTo(QObject * receiver, const char * member)
{
    if (m_pHostButton != NULL)
    {
        // 若已经连接其他槽，先断开旧的槽函数
        if (m_pSignalReceiver != NULL
                && m_strSingalMember != "")
        {
            QObject::disconnect(m_pHostButton, SIGNAL(clicked(qSoftButton *)), m_pSignalReceiver,
                                m_strSingalMember.toUtf8().data());
        }

        // 连接槽函数
        if (receiver != NULL
                && strcmp(member, "") != 0)
        {
            QObject::connect(m_pHostButton, SIGNAL(clicked(qSoftButton *)), receiver, member);
        }
    }

    m_pSignalReceiver = receiver;
    m_strSingalMember = QString::fromUtf8(member);
    return this;
}

qSoftButton * qSoftButton::EnableButton(bool bEnable)
{
    // to do
    return this;
}

qSoftButton * qSoftButton::ShowButton(bool bShow)
{
    if (bShow)
    {
        m_pHostButton->show();
    }
    else
    {
        m_pHostButton->hide();
    }

    return this;
}

// 用于管理所有插入softkey的Button
// 包括显示和不显示的
class dataSoftKey : public chObjArray< qSoftButton *, false >
{
public:
    dataSoftKey()
    {
        // 构造的时候初始化Btn的数量
        m_iFirstButtonIndex = 0;
    }

    ~dataSoftKey()
    {
        // 循环释放资源
        int nCount = GetSize();
        for (int i = 0; i < nCount; i++)
        {
            delete at(i);
        }
    }

    // 将Button添加到Index的位置
    qSoftButton * setButton(int iIndex, qSoftButton * pButton)
    {
        // Btn数量小于插入的Index，就新创建一个
        while (GetSize() <= iIndex)
        {
            Add(NULL);
        }

        at(iIndex) = pButton;
        return pButton;
    }

    // 添加一个新的Btn
    qSoftButton * addButton(qSoftButton * pButton)
    {
        Add(pButton);
        return pButton;
    }

    // 通过Index获得Button指针
    qSoftButton * findDataByButtonIndex(int iOffset)
    {
        int iIndex = m_iFirstButtonIndex + iOffset;
        return GetSize() > iIndex ? at(iIndex) : NULL;
    }

    // 通过按钮名字或者Button指针
    qSoftButton * findDataByButtonName(LPCSTR lpszName)
    {
        chConstTextA strBtnName = lpszName;
        int nCount = GetSize();
        for (int i = 0; i < nCount; i++)
        {
            qSoftButton * pBtn = at(i);
            if (pBtn != NULL && pBtn->m_strButtonName == strBtnName.c_str())
            {
                return pBtn;
            }
        }

        return NULL;
    }

    // 通过Button在数组中的Index，计算它在当前页面的Index
    int findButtonByDataIndex(int iIndex)
    {
        if (GetSize() > BUTTONS_PER_PAGE)
        {
            // softkey的页面大于一页
            // 减去该页面第一个按钮的Index获得偏移值
            int iOffset = iIndex - m_iFirstButtonIndex;
            return (iOffset >= BUTTONS_PER_PAGE - 1 || iOffset < 0) ? -1 : iOffset;
        }
        else
        {
            return (iIndex >= BUTTONS_PER_PAGE || iIndex < 0) ? -1 : iIndex;
        }
    }

    // 数据翻页
    void PageDown()
    {
        // 减1是因为每页的最后一个按钮会被设置为more，无数据
        m_iFirstButtonIndex += BUTTONS_PER_PAGE - 1;
        if (m_iFirstButtonIndex >= GetSize())
        {
            m_iFirstButtonIndex = 0;
        }
    }

    void Clear()
    {
        int nCount = GetSize();
        for (int i = 0; i < nCount; i++)
        {
            delete at(i);
        }
        chObjArray<qSoftButton *, false>::Clear();
        m_iFirstButtonIndex = 0;
    }
    // 获取软键栏当前页的软键个数（由于大于一页最大个数则以多页切换方式显示软键）
    int GetNumOfCurrentPage(void)
    {
        // Btn数量小于插入的Index，就新创建一个
        int iNum;
        int iNumofBtn = GetSize();
        // 如果按钮少于一页最大个数
        if (iNumofBtn < BUTTONS_PER_PAGE)
        {
            iNum = iNumofBtn;
        }
        else
        {
            // 如果按钮大于一页最大个数，则计算软键剩下个数
            iNum = iNumofBtn - m_iFirstButtonIndex;
            if (iNum > (BUTTONS_PER_PAGE - 2))
            {
                iNum = BUTTONS_PER_PAGE;
            }
            else
            {
                // 少于（BUTTONS_PER_PAGE-1个），都多最后一个要显示more的按钮
                iNum = iNum + 1;
            }
        }

        return iNum;
    }
private:
    // 通过Index获得Btn的指针
    qSoftButton *& at(int iIndex)
    {
        return operator [](iIndex);
    }

private:
    int         m_iFirstButtonIndex;
};

//Label滚动文字间隔时间
static int GetTimeIntervalSoftkey()
{
    int nValue = 500;
    return nValue < _TIME_INTERVAL_SCROLL ? _TIME_INTERVAL_SCROLL : nValue;
}

//////////////////////////////////////////////////////////////////////////
// class qButtonItem : public QPushButton
qButtonItem::qButtonItem(QWidget * parent) : qUIPushButton(parent),
    m_iIconYOffSet(2),
    m_iTextYOffSet(0)
{
    m_pGuestData = NULL;

    // 设置字体
    qWidgetUtility::setFontStyle(this,
                                 CustomTheme_GetCustomFontSizeByKey(CUSTOM_FONT_ID_SOFAKEY_NORMAL), QFont::Bold);

    // 设置该Button没有焦点
    QPushButton::setFocusPolicy(Qt::NoFocus);

    // 连接自己的点击信号和槽函数
    QObject::connect(this, SIGNAL(clicked()), this, SLOT(OnClicked()));

    // 设置三态图标, !!!!!
    //qUIPushButton::set3StatePic(PIC_BTN_SOFTKEY, PIC_BTN_SOFTKEY_PRESS, PIC_BTN_SOFTKEY);
    this->setFlat(true);

    // 设置Button不同状态下的文字颜色
    QPalette pal = this->palette();
    pal.setColor(QPalette::Active, QPalette::ButtonText, QColor(0, 0, 0));
    pal.setColor(QPalette::Inactive, QPalette::ButtonText, QColor(0, 0, 0));
    pal.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(0, 0, 0, 255));
    this->setPalette(pal);

    connect(&m_timerScrollText, SIGNAL(timeout()), this, SLOT(UpdateText()));
    m_nBegin = 0;
    m_nCount = 0;
    m_nWidth = 0;
    m_nDiffLength = 0;
    m_strShowText = "";
    m_strRealText = "";
}

void qButtonItem::setData(qSoftButton * pData)
{
    if (m_pGuestData != NULL)
    {
        // 若已经有数据，先断开旧数据的连接
        SetRealText(QString());
        if (m_pGuestData->m_pSignalReceiver != NULL
                && m_pGuestData->m_strSingalMember != "")
        {
            QObject::disconnect(SIGNAL(clicked(qSoftButton *)));
        }
    }
    if (pData != NULL)
    {
        //  设置图标路径和文本内容
        setIconPic(pData->m_strIconPath);
        SetRealText(pData->m_strButtonTitle);
        // 设置动作名称，IME
        setBtnAction(pData->m_strButtonName);
        // 连接新数据的槽函数
        if (pData->m_pSignalReceiver != NULL
                && pData->m_strSingalMember != "")
        {
            QObject::connect(this, SIGNAL(clicked(qSoftButton *)), pData->m_pSignalReceiver,
                             pData->m_strSingalMember.toUtf8().data());
        }
        pData->m_pHostButton = this;
    }

    m_pGuestData = pData;

    CheckStartTimer();
}

QString qButtonItem::GetAction() const
{
    if (NULL != m_pGuestData)
    {
        return m_pGuestData->m_strButtonName;
    }

    return "";
}

void qButtonItem::SetRealText(const QString & text)
{
    m_strRealText = LANG_PARSE(text);
    setText(m_strRealText);
}

QString qButtonItem::GetRealText() const
{
    return m_strRealText;
}

void qButtonItem::OnClicked()
{
    if (m_pGuestData != NULL)
    {
        emit clicked(m_pGuestData);
    }
}

// 设置软键图标显示类型
void qButtonItem::SetSoftkeyType(SoftkeyType eIconType)
{
    if (m_eSoftkeyType != eIconType)
    {
        m_eSoftkeyType = eIconType;
    }
}

// 获取软键图标显示类型
SoftkeyType qButtonItem::GetSoftkeySoftkeyType()
{
    return m_eSoftkeyType;
}

// 软键自定义绘制
void qButtonItem::paintEvent(QPaintEvent * pe)
{
    QStylePainter p(this);
    QPixmap pm;
    QStyleOptionButton option;
    initStyleOption(&option);
    // 画按钮上的ICON
    QPixmap pmIcon;
    bool bDrawnIcon = false;
    // 绘制idle类型更新状态图片的标志
    bool bDrawUpdated = false;
    QRect rcIcon;
    QRect rcUpdated;
    int xIcon = 0;
    int yIcon = 0;
    // 图标和文本偏移默认为0，根据类型不同偏移也不同
    m_iIconYOffSet = 5;
    m_iTextYOffSet = 0;
    // 画按钮上的文本, 如果已经画了ICON, 则需要进行相应的偏移
    QRect rcText = option.rect;
    QString tx = this->GetShowText();
    // 输入法字符串
    QString strFirst;
    QString strSecond;
    QPalette pal = option.palette;
    // 设置字体大小
    QFont fontTitle = font();
    fontTitle.setPointSize(QSOFTBUTTON_NORMAL_FONT_SIZE);
    p.setFont(fontTitle);
    if (!m_strIconPic.isEmpty())
    {
        pmIcon = THEME_GET_BMP(m_strIconPic.toUtf8().data());
        if (!pmIcon.isNull())
        {
            bDrawnIcon = true;
        }
    }
    else
    {
        if (STYLE_TYPE_DIVIDE == m_eSoftkeyType)
        {
            // 分割模式的"Delete"和"Save"没配置图标使用默认图标
            if (tx == LANG_TRANSLATE(TRID_DELETE))
            {
                pmIcon = THEME_GET_BMP(PIC_SOFTKEY_REMOVE);
            }
            else if (tx == LANG_TRANSLATE(TRID_SAVE))
            {
                pmIcon = THEME_GET_BMP(PIC_SOFTKEY_SAVE);
            }
            if (!pmIcon.isNull())
            {
                bDrawnIcon = true;
            }
        }
    }
    // 判断Icon的对齐方式.目前只有居左、居右、居中三种情况
    bool isIconAlignLeft = (m_eIconAlign & Qt::AlignLeft) == Qt::AlignLeft;
    bool isIconAlignRight = (m_eIconAlign & Qt::AlignRight) == Qt::AlignRight;
    bool isIconAlignCenter = (m_eIconAlign & Qt::AlignCenter) == Qt::AlignCenter;
    switch (m_eSoftkeyType)
    {
    case STYLE_TYPE_ICONTRAY:
        {
            // IDLE界面的软键样式，它们的点击效果通过图标显示不通过按钮背景
            // 通过Icon在文本内容上方居中的对齐方式，计算绘制的y坐标
            QPixmap pmBk;
            switch (m_iBtnID)
            {
            case 0:
                {
                    pmBk = THEME_GET_BMP(PIC_SOFTKEY_IDLE_KEY1);
                }
                break;
            case 1:
                {
                    pmBk = THEME_GET_BMP(PIC_SOFTKEY_IDLE_KEY2);
                }
                break;
            case 2:
                {
                    pmBk = THEME_GET_BMP(PIC_SOFTKEY_IDLE_KEY3);
                }
                break;
            case 3:
                {
                    pmBk = THEME_GET_BMP(PIC_SOFTKEY_IDLE_KEY4);
                }
                break;
            default:
                {
                    pmBk = THEME_GET_BMP(PIC_SOFTKEY_IDLE_KEY1);
                    IDLE_INFO("NoIdleBackGroud :: m_iBtnID is %d.\n", m_iBtnID);
                }
            }
            if (!pmBk.isNull())
            {

                yIcon = option.rect.y() + m_iIconYOffSet;
                xIcon = option.rect.x() + (option.rect.width() - pmBk.width()) / 2;
                // 判断Icon坐标是否为有效坐标
                if (xIcon >= 0 && xIcon < option.rect.x() + option.rect.width() &&
                        yIcon >= 0 && yIcon < option.rect.y() + option.rect.height())
                {
                    rcIcon.setRect(xIcon, yIcon, pmBk.width(), pmBk.height());
                    // 坐标有效绘制Icon背景图
                    p.drawPixmap(rcIcon, pmBk);
                    if (bDrawnIcon == true)
                    {
                        // 调节水平坐标，使中间的图标居中
                        int x = rcIcon.x() + (rcIcon.width() - pmIcon.width()) / 2 + 1;
                        // 背景高度需先减去图标下方的阴影部分的高度
                        int shadowheight = 6;
                        int y = rcIcon.y() + (rcIcon.height() - shadowheight - pmIcon.height()) / 2;
                        QRect rec;
                        rec.setRect(x, y, pmIcon.width(), pmIcon.height());
                        // 绘制Icon中心图标
                        p.drawPixmap(rec, pmIcon);
                    }
                }
                if ((this->isDown() || this->isChecked()) && (this->isEnabled()))
                {
                    // 如果是图标在文字上方的绘制，目前是IDLE界面的软键样式，它们的点击效果通过图标显示不通过按钮背景
                    // 且对应的图标点击效果图放在m_strEnablePic字段中
                    // 获取点击效果的图标图片
                    pm = THEME_GET_BMP(PIC_SOFTKEY_IDLE_SHADOW);
                    bDrawUpdated = true;
                    rcUpdated.setRect(xIcon, yIcon, pm.width(), pm.height());
                    // 绘制点击效果
                    p.drawPixmap(rcUpdated, pm);
                }
                // 通过Icon的位置大小，计算绘制文本的y坐标
                // +m_iTextYOffSet 在ICON和文字之间留一点空隙
                rcText.setY(rcText.y()  /*+ yIcon*/ + pmBk.height() + m_iTextYOffSet);
            }
            // 若是Text为空，就没必要绘制
            if (tx.isEmpty())
            {
                return ;
            }
            // 绘制Item的Text
            p.drawItemText(rcText, m_eAlign, pal, this->isEnabled(), tx);
        }
        break;
    case STYLE_TYPE_NORMAL:
        {
            // 判断button的状态，设置对应的图片
            if (!this->isEnabled())
            {
                pm = THEME_GET_BMP(m_strDisalbePic.toUtf8().data());
            }
            else if (this->isDown()
                     || this->isChecked())
            {
                pm = THEME_GET_BMP(m_strEnablePic.toUtf8().data());
            }
            else
            {
                pm = THEME_GET_BMP(m_strNormalPic.toUtf8().data());
            }

            // 画按钮背景图片
            if (!pm.isNull())
            {
                p.drawPixmap(option.rect.x() + (option.rect.width() - pm.width()) / 2,
                             option.rect.y() + option.rect.height() / 2 - pm.height() / 2, pm);
            }
            if (bDrawnIcon == true)
            {
                // icon的绘制
                // 通过Icon的对齐方式，计算绘制的x坐标
                if (isIconAlignLeft)
                {
                    xIcon = option.rect.x() + m_iIconXOffSet;
                }
                else if (isIconAlignRight)
                {
                    xIcon = option.rect.x() + option.rect.width() - pmIcon.width() - m_iIconXOffSet;
                }
                else
                {
                    xIcon = option.rect.x() + (option.rect.width() - pmIcon.width()) / 2;
                }
                yIcon = option.rect.y() + option.rect.height() / 2 - pmIcon.height() / 2;
                // 判断Icon坐标是否为有效坐标
                if (xIcon >= 0 && xIcon < option.rect.x() + option.rect.width() &&
                        yIcon >= 0 && yIcon < option.rect.y() + option.rect.height())
                {
                    rcIcon.setRect(xIcon, yIcon, pmIcon.width(), pmIcon.height());
                    // 绘制点击效果
                    p.drawPixmap(rcUpdated, pm);
                }
            }
            // 文本内容的位置计算
            if (isIconAlignLeft)
            {
                // +m_iTextXOffSet 在ICON和文字之间留一点空隙
                rcText.setX(rcText.x()  + rcIcon.right() + m_iTextXOffSet);
            }
            else if (isIconAlignRight)
            {
                // -m_iTextXOffSet 在ICON和文字之间留一点空隙
                rcText.setWidth(rcText.width() - rcIcon.right() - m_iTextXOffSet);
            }
            // 若是Text为空，就没必要绘制
            if (tx.isEmpty())
            {
                return ;
            }
            // 绘制Item的Text
            p.drawItemText(rcText, m_eAlign, pal, this->isEnabled(), tx);
        }
        break;
    case STYLE_TYPE_DIVIDE:
    default:
        {
            // IDLE界面的软键样式，它们的点击效果通过图标显示不通过按钮背景
            // 通过Icon在文本内容上方居中的对齐方式，计算绘制的y坐标
            m_iIconYOffSet = 2;
            int iCurIndex = -1;
            if (m_strBtnAction == IME_ACTION)
            {
                iCurIndex = 1;
            }

            if (iCurIndex != -1)
            {
                // 输入法按钮数据重组,组合输入法按钮第二行的文字
                // 获取剩余输入法信息
                QStringList list = tx.split(",");// 例如tx内容为"123,abc ABC 2aB"
                QString strTemp;
                if (list.size() > 1)
                {
                    // 保存当前标题，先用大号字体绘制
                    strFirst = list[0];         // 则strFirst内容为"123"
                    strSecond = list[1];        // 则strSecond内容为"abc ABC 2aB"
                    if (strSecond == "")
                    {
                        // 只有一行文字，居中显示
                        tx = strFirst;
                        rcText = option.rect;
                    }
                    else
                    {
                        // 让正常字体的绘制在最后完成，保持文字绘制的统一
                        tx = strSecond;
                        QFont fontTitle = font();
                        fontTitle.setPointSize(QSOFTBUTTON_IME_FONT_SIZE);
                        p.setFont(fontTitle);
                        rcUpdated.setRect(option.rect.x(), option.rect.y(), option.rect.width(),
                                          option.rect.height() * 2 / 3);
                        p.drawItemText(rcUpdated, m_eAlign, option.palette, this->isEnabled(), strFirst);
                        rcText.setY(rcText.y()  + option.rect.height() * 4 / 7);
                        fontTitle.setPointSize(QSOFTBUTTON_DEFAULT_FONT_SIZE);
                        p.setFont(fontTitle);
                        // IME按钮下方字体颜色为灰色
                        QPen pen = p.pen();
                        pen.setColor(QColor(128, 128, 128));
                        p.setPen(pen);
                    }
                }
                else
                {
                    QFont fontTitle = font();
                    fontTitle.setPointSize(24);
                    p.setFont(fontTitle);
                    // 未找到分隔符，只有一行文字，上半部分显示当前输入法
                    rcUpdated.setRect(option.rect.x(), option.rect.y(), option.rect.width(),
                                      option.rect.height() * 2 / 3);
                    p.drawItemText(rcUpdated, m_eAlign, option.palette, this->isEnabled(), tx);
                    // 下半部分显示IME 字符串
                    fontTitle.setPointSize(QSOFTBUTTON_NORMAL_FONT_SIZE);
                    p.setFont(fontTitle);
                    tx = LANG_TRANSLATE(TRID_INPUT_METHOD_SHORT);
                    rcText.setY(rcText.y()  + 33);
                }
                // 若是Text为空，就没必要绘制
                if (tx.isEmpty())
                {
                    return ;
                }
                // 绘制Item的Text
                p.drawItemText(rcText, m_eAlign, pal, this->isEnabled(), tx);
                bDrawnIcon = false;
                bDrawUpdated = false;
            }
            else
            {
                if (bDrawnIcon == true)
                {
                    // 普通按钮设置文字颜色为白色
                    yIcon = option.rect.y() + m_iIconYOffSet + 5;
                    xIcon = option.rect.x() + (option.rect.width() - pmIcon.width()) / 2;

                    // 判断Icon坐标是否为有效坐标
                    if (xIcon >= 0 && xIcon < option.rect.x() + option.rect.width() &&
                            yIcon >= 0 && yIcon < option.rect.y() + option.rect.height())
                    {
                        rcIcon.setRect(xIcon, yIcon, pmIcon.width(), pmIcon.height());
                        // 坐标有效绘制Icon
                        p.drawPixmap(rcIcon, pmIcon);
                        // 通过Icon的位置大小，计算绘制文本的y坐标
                        // +m_iTextYOffSet 在ICON和文字之间留一点空隙
                        rcText.setY(rcText.y()  + yIcon + pmIcon.height() + m_iTextYOffSet - 2);
                    }
                    // 若是Text为空，就没必要绘制
                    if (tx.isEmpty())
                    {
                        return ;
                    }
                    // 绘制Item的Text
                    p.drawItemText(rcText, Qt::AlignHCenter | Qt::AlignTop, pal, this->isEnabled(), tx);
                }
                else
                {
                    // 没有图标时，文字居中显示
                    rcText = option.rect;
                    // 若是Text为空，就没必要绘制
                    if (tx.isEmpty())
                    {
                        return ;
                    }
                    // 绘制Item的Text
                    p.drawItemText(rcText, m_eAlign, pal, this->isEnabled(), tx);

                }
            }

            // 点击效果数据获取
            if ((this->isDown()) && (this->isEnabled()))
            {
                // 如果是图标在文字上方的绘制，目前是IDLE界面的软键样式，它们的点击效果通过图标显示不通过按钮背景
                // 且对应的图标点击效果图放在m_strEnablePic字段中，选中状态在m_strFocusPic字段中
                // 获取点击效果的图标图片
                pm = THEME_GET_BMP(m_strEnablePic.toUtf8().data());
                bDrawUpdated = true;
                int y1 = option.rect.y();
                int y2 = option.rect.height();
                int y3 = pm.height();
                rcUpdated.setRect(option.rect.x() + (option.rect.width() - pm.width()) / 2,
                                  option.rect.y() + option.rect.height() / 2 - pm.height() / 2, pm.width(), pm.height());
                // 有点击先绘制点击效果
                p.drawPixmap(rcUpdated, pm);
            }
        }
        break;
    }
}
//////////////////////////////////////////////////////////////////////////
qSoftKeyBar::qSoftKeyBar(QWidget * parent) : QFrame(parent)
{
    this->setObjectName(QString::fromUtf8("softkeybar"));

    // 初始化4个softkey
    for (int i = 0; i < BUTTONS_PER_PAGE; i++)
    {
        qButtonItem * pButton = new qButtonItem(this);
        pButton->setObjectName(QString("SoftkeyButton%1").arg(i));
        m_arrPushButton[i] = pButton;
    }

    // 初始化More Button
    m_pButtonMore = new qSoftButton;
    m_pButtonMore->m_strButtonTitle = LANG_TRANSLATE(TRID_MORE);
    m_pButtonMore->m_pSignalReceiver = this ;
    m_pButtonMore->m_strSingalMember = SLOT(OnButtonMore());
    m_pButtonMore->m_strIconPath = PIC_SOFTKEY_IDLE_MORE;
    m_pSoftKeyData = new dataSoftKey;

    // 设置自己父窗口的指针
    m_parent = parent;

    // 如果父窗口不存在给出报错的提示
    if (m_parent == NULL)
    {
        chWARNING(m_parent != NULL);
    }
}

void qSoftKeyBar::SetParent(QWidget * parent)
{
    m_parent = parent;

    // 如果父窗口不存在给出报错的提示
    if (m_parent == NULL)
    {
        chWARNING(m_parent != NULL);
    }

    setParent(parent);
}

qSoftKeyBar::~qSoftKeyBar()
{
    // 释放资源
    delete m_pButtonMore;
    delete m_pSoftKeyData;
}

void qSoftKeyBar::SetBackground(QString strBkgnd, QString strDivide)
{
    // 设置绘制背景图需要的图片资源
    m_strBackgroud = strBkgnd;
    m_strDivide = strDivide;
}

void qSoftKeyBar::SetAllBtn3StatePic(const QString & normal_pic,
                                     const QString & enable_pic,
                                     const QString & disable_pic)
{
    for (int i = 0; i < BUTTONS_PER_PAGE; i++)
    {
        // no in-ner more button
        m_arrPushButton[i]->set3StatePic(normal_pic, enable_pic, disable_pic);
    }
}

void qSoftKeyBar::SetBtn3StatePic(int index, const QString & normal_pic,
                                  const QString & enable_pic,
                                  const QString & disable_pic)
{
    // no in-ner more button
    m_arrPushButton[index]->set3StatePic(normal_pic, enable_pic, disable_pic);
}

void qSoftKeyBar::SetButtonHide(int iIndex)
{
    // 入口参数检查
    if (iIndex < 0 || iIndex >= BUTTONS_PER_PAGE)
    {
        return ;
    }

    m_arrPushButton[iIndex]->hide();
}

void qSoftKeyBar::SetButtonShow(int iIndex)
{
    // 入口参数检查
    if (iIndex < 0 || iIndex >= BUTTONS_PER_PAGE)
    {
        return ;
    }

    m_arrPushButton[iIndex]->show();
}

qSoftButton * qSoftKeyBar::SetButton(int iIndex, qSoftButton * pButton)
{
    m_pSoftKeyData->setButton(iIndex, pButton);

    // 设置Btn的数据后更新UI
    UpdateButton2UI();
    return pButton;
}

qSoftButton * qSoftKeyBar::SetButton(int iIndex, LPCSTR lpszName, LPCSTR lpszTitle,
                                     QObject * receiver, const char * member)
{
    // 找寻Button是否已经存在
    qSoftButton * pButton = GetButtonByIndex(iIndex);
    if (pButton == NULL)
    {
        pButton = new qSoftButton();
    }

    if (pButton != NULL)
    {
        // 设置Button的数据
        pButton->m_strButtonTitle = lpszTitle;
        pButton->m_strButtonName = lpszName;
        pButton->m_pSignalReceiver = receiver;
        pButton->m_strSingalMember = member;
        m_pSoftKeyData->setButton(iIndex, pButton);

        // 数据变化后刷新UI
        UpdateButton2UI();
    }
    else
    {
        // 给出指针为空的警告
        chWARNING(pButton != NULL);
    }
    return pButton;
}
qSoftButton * qSoftKeyBar::SetButton(int iIndex, LPCSTR lpszName, QString lpszTitle,
                                     QObject * receiver, const char * member)
{
    // 找寻Button是否已经存在
    qSoftButton * pButton = GetButtonByIndex(iIndex);
    if (pButton == NULL)
    {
        //luse_syl
        pButton = new qSoftButton();
    }

    if (pButton != NULL)
    {
        // 设置Button的数据
        pButton->m_strButtonTitle = lpszTitle;
        pButton->m_strButtonName = lpszName;
        pButton->m_pSignalReceiver = receiver;
        pButton->m_strSingalMember = member;
        m_pSoftKeyData->setButton(iIndex, pButton);

        // 数据变化后刷新UI
        UpdateButton2UI();
    }
    else
    {
        // 给出指针为空的警告
        chWARNING(pButton != NULL);
    }
    return pButton;
}
qSoftButton * qSoftKeyBar::AddButton(qSoftButton * pButton)
{
    m_pSoftKeyData->addButton(pButton);
    UpdateButton2UI();
    return pButton;
}

qSoftButton * qSoftKeyBar::GetButtonByName(LPCSTR lpszName)
{
    // 监测保存数据的数组是否为空
    if (m_pSoftKeyData == NULL || textEMPTY(lpszName))
    {
        return NULL;
    }
    return m_pSoftKeyData->findDataByButtonName(lpszName);
}

qSoftButton * qSoftKeyBar::GetButtonByIndex(int iIndex)
{
    // 判断参数的有效性
    if (m_pSoftKeyData == NULL
            || iIndex < 0
            || iIndex >= m_pSoftKeyData->GetSize())
    {
        return NULL;
    }

    return (*m_pSoftKeyData)[iIndex];
}

void qSoftKeyBar::OnButtonMore()
{
    // 将数据翻页，并且设置到UI上
    m_pSoftKeyData->PageDown();
    // 更新按钮状态，个数可能会变
    //ResetBtnStatus();
    // 更新按钮数据
    UpdateButton2UI();
}

void qSoftKeyBar::UpdateButton2UI()
{
    // 更新按钮状态，个数可能会变
    ResetBtnStatus();
    // 清空全有数据
    for (int i = 0; i < BUTTONS_PER_PAGE; i++)
    {
        m_arrPushButton[i]->setData(NULL);
    }

    // 获取当前软键个数
    int nSoftKeyData = m_pSoftKeyData->GetNumOfCurrentPage();
    // 将需要数据设置到UI上
    for (int i = 0; i < nSoftKeyData; i++)
    {
        // no in-ner more button
        m_arrPushButton[i]->setData(m_pSoftKeyData->findDataByButtonIndex(i));
        // 更新按钮
        m_arrPushButton[i]->update();
        m_arrPushButton[i]->setButtonID(i);
    }

    // 如果软键个数大于一页可显示的软件个数，无论当前是第几页，最后一个都变为翻页按钮
    if (m_pSoftKeyData->GetSize() > BUTTONS_PER_PAGE)
    {
        m_arrPushButton[nSoftKeyData - 1]->setData(m_pButtonMore);
    }
}

void qSoftKeyBar::setKeyRelease()
{
    // 循环设置每个按键抬起
    for (int i = 0; i < BUTTONS_PER_PAGE; i++)
    {
        if (m_arrPushButton[i]->isDown())
        {
            m_arrPushButton[i]->setDown(false);
        }
    }
}

//清空数据
void qSoftKeyBar::Clear()
{
    for (int i = 0; i < BUTTONS_PER_PAGE; i++)
    {
        m_arrPushButton[i]->setData(NULL);
    }

    if (NULL != m_pSoftKeyData)
    {
        m_pSoftKeyData->Clear();
    }
}

bool qSoftKeyBar::eventFilter(QObject * pObj, QEvent * pEvent)
{
    if (pEvent->type() == QEvent::KeyPress)
    {
        if (isHidden())
        {
            UIKERNEL_INFO("qSoftKeyBar: MainWnd inactive, isHidden(%d), actioveWindow=%s",  isHidden(),
                          QApplication::activeWindow()->metaObject()->className());
            // unvisible or root window is not actived then skip it
            return false;
        }

        if (isChildOf(pObj, m_parent))
        {
            QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;
            if (pKeyEvent->isAutoRepeat())
            {
                return false;
            }

            int iSoftKeyIdx = -1;
            // 话机的4个按键分别对应4个softkey
            switch (pKeyEvent->key())
            {
            case PHONE_KEY_FN1:
                iSoftKeyIdx = 0;
                break;
            case  PHONE_KEY_FN2:
                iSoftKeyIdx = 1;
                break;
            case PHONE_KEY_FN3:
                iSoftKeyIdx = 2;
                break;
            case PHONE_KEY_FN4:
                iSoftKeyIdx = 3;
                break;
            }// end switch(pKeyEvent->key())

            UIKERNEL_INFO("qSoftKeyBar: SoftkeyPressed(%d)", iSoftKeyIdx);

            if (iSoftKeyIdx != -1)
            {
//#ifdef _VPX
                m_arrPushButton[iSoftKeyIdx]->setDown(true);
// #else
//              m_arrPushButton[iSoftKeyIdx]->OnClicked();
// #endif
                return true;
            }// end if (iSoftKeyIdx != -1)
        }//end if (isChildOf(pObj,m_parent))
    }
    else if (pEvent->type() == QEvent::KeyRelease)
    {
        if (isHidden()
                || m_parent->window() != QApplication::activeWindow())
        {
            // unvisible or root window is not actived then skip it
            setKeyRelease();//取消之前的按下状态，避免之前因按下但后来弹窗使得窗体Deactive，结果无法释放按键问题。
            return false;
        }

        if (isChildOf(pObj, m_parent))
        {
            QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;
            if (pKeyEvent->isAutoRepeat())
            {
                return false;
            }

            int iSoftKeyIdx = -1;
            // 话机的4个按键分别对应4个softkey
            switch (pKeyEvent->key())
            {
            case PHONE_KEY_FN1:
                iSoftKeyIdx = 0;
                break;
            case PHONE_KEY_FN2:
                iSoftKeyIdx = 1;
                break;
            case PHONE_KEY_FN3:
                iSoftKeyIdx = 2;
                break;
            case PHONE_KEY_FN4:
                iSoftKeyIdx = 3;
                break;
            }//end switch(pKeyEvent->key())

            if (iSoftKeyIdx != -1)
            {
//#ifdef _VPX
                //先判断是否为按下状态，实现前后按下释放匹配于同一状态softkey才对外发送信号。
                if (m_arrPushButton[iSoftKeyIdx]->isDown())
                {
                    m_arrPushButton[iSoftKeyIdx]->OnClicked();
                }
                m_arrPushButton[iSoftKeyIdx]->setDown(false);
                return true;
//#endif
            }// end if(iSoftKeyIdx != -1)
        }//end if (isChildOf(pObj,m_parent))
    }// end else if (pEvent->type() == QEvent::KeyRelease)

    return false;
}


void qSoftKeyBar::paintEvent(QPaintEvent * pe)
{
    QStylePainter p(this);
    QPixmap pm;
    QRect rect = frameRect();
    // 有按钮才绘制背景
    if (m_pSoftKeyData->GetSize() > 0)
    {
        switch (m_eSoftkeyType)
        {
        case STYLE_TYPE_ICONTRAY:
            {
                pm = THEME_GET_BMP(m_strBackgroud.toUtf8().data());
                // 画按钮背景图片
                if (!pm.isNull())
                {
                    p.drawPixmap(rect.x() + (rect.width() - pm.width()) / 2,
                                 rect.y() + rect.height() / 2 - pm.height() / 2, pm);
                }
            }
            break;
        case STYLE_TYPE_NORMAL:
            {

            }
            break;
        case STYLE_TYPE_DIVIDE:
        default:
            {
                pm = THEME_GET_BMP(m_strBackgroud.toUtf8().data());
                // 画按钮背景图片
                if (!pm.isNull())
                {
                    p.drawPixmap(rect.x() + (rect.width() - pm.width()) / 2,
                                 rect.y() + rect.height() / 2 - pm.height() / 2, pm);
                }
                pm = THEME_GET_BMP(m_strDivide.toUtf8().data());
                // 画按钮背景图片
                if (!pm.isNull())
                {
                    // 获取当前软键个数
                    int nSoftKeyData = m_pSoftKeyData->GetNumOfCurrentPage();
                    if (nSoftKeyData == 0)
                    {
                        return;
                    }
                    int w = width() / nSoftKeyData;
                    int h = height();
                    // 设置个softkey位置大小
                    for (int i = 1; i < nSoftKeyData; i++)
                    {
                        p.drawPixmap(rect.x() + i * w,
                                     rect.y() + rect.height() / 2 - pm.height() / 2, pm);
                    }

                }
            }
            break;
        }
    }
}

// 重新设置按钮的位置和大小
void qSoftKeyBar::ResetBtnStatus()
{
    // 获取当前软键个数
    int nSoftKeyData = m_pSoftKeyData->GetNumOfCurrentPage();
    // 需要更新的数据
    int w = 0;
    int h = 0;
    // 个数为0，不再设置软键位置大小，但需要更新显示状态，隐藏所有软键
    if (nSoftKeyData != 0)
    {
        // 小心不能整除的背景图像素宽度值
        w = width() / nSoftKeyData;
        h = height();
    }
    // 设置个softkey位置大小
    for (int i = 0; i < BUTTONS_PER_PAGE; i++)
    {
        if (i < nSoftKeyData)
        {
            m_arrPushButton[i]->setGeometry(0 + i * w, 0, w, h);
            if (i == (nSoftKeyData - 1))
            {
                // 最后一个有效按钮的宽度必须
                ;
            }
            m_arrPushButton[i]->show();
        }
        else
        {
            m_arrPushButton[i]->hide();
        }
    }
}

// 设置图标显示类型，以及对应图标和文字的位置的偏移
void qSoftKeyBar::SetSoftkeyType(SoftkeyType eIconType)
{
    if (m_eSoftkeyType != eIconType)
    {
        // 只有设置类型与当前不同时，才重新设置类型，提高效率
        m_eSoftkeyType = eIconType;
    }
    switch (eIconType)
    {
    case STYLE_TYPE_ICONTRAY:
        {
            for (int i = 0; i < BUTTONS_PER_PAGE; i++)
            {
                // 设置真正显示的软键，图标向上对齐
                m_arrPushButton[i]->SetSoftkeyType(eIconType);
                // 设置真正显示的软键，文字为黑色
                m_arrPushButton[i]->setTextColor(CustomTheme_GetCustomFontColorByKey(CUSTOM_FONT_ID_SOFAKEY_IDLE));
            }
            m_pButtonMore->m_strIconPath = PIC_SOFTKEY_IDLE_MORE;
            IDLE_DEBUG("m_pButtonMore->m_strIconPath is %s.", m_pButtonMore->m_strIconPath.toUtf8().data());
        }
        break;
    case STYLE_TYPE_DIVIDE:
    case STYLE_TYPE_NORMAL:
    default:
        {
            for (int i = 0; i < BUTTONS_PER_PAGE; i++)
            {
                // 设置真正显示的软键，图标向上对齐
                m_arrPushButton[i]->SetSoftkeyType(eIconType);
                // 设置真正显示的软键，文字为白色
                m_arrPushButton[i]->setTextColor(CustomTheme_GetCustomFontColorByKey(
                                                     CUSTOM_FONT_ID_SOFAKEY_NORMAL));
            }
            m_pButtonMore->m_strIconPath = PIC_SOFTKEY_MORE;
        }
        break;
    }

}

// 设置图标加载的路径
void qSoftKeyBar::SetIconPath(int iIndex, QString strPath)
{
    // 找寻Button是否已经存在
    qSoftButton * pButton = GetButtonByIndex(iIndex);
    // 设置图标路径
    pButton->m_strIconPath = strPath;
    // 数据变化后刷新UI
    UpdateButton2UI();
}


// 获取软键图标显示类型
SoftkeyType qSoftKeyBar::GetSoftkeySoftkeyType()
{
    return m_eSoftkeyType;
}
void qButtonItem::UpdateText()
{
    QFontMetrics fm(this->font());
    int nShowLenght = 0;

    while (1)
    {
        if (fm.width(m_strRealText.mid(m_nBegin, ++nShowLenght)) > m_nWidth
                || (m_nBegin + nShowLenght) > m_strRealText.length())
        {
            break;
        }
    }

    nShowLenght--;
    QString strShowText = m_strRealText.mid(m_nBegin, nShowLenght);
    SetShowText(strShowText);

    int nEnd = m_nCount + nShowLenght;
    if (nEnd >= m_strRealText.length())
    {
        if (nEnd < m_strRealText.length() + _COUNT_STOP)
        {
            m_nCount++;
        }
        else
        {
            m_nBegin = 0;
            m_nCount = 0;
        }
    }
    else
    {
        m_nBegin++;
        m_nCount++;
    }
}

void qButtonItem::StartScrollTimer()
{
    if (m_timerScrollText.isActive())
    {
        m_timerScrollText.stop();
    }

    m_timerScrollText.start(GetTimeIntervalSoftkey());
}

void qButtonItem::SetShowText(const QString & text)
{
    m_strShowText = text;
    update();
}

QString qButtonItem::GetShowText() const
{
    return m_strShowText;
}

void qButtonItem::StopScrollTimer()
{
    if (m_timerScrollText.isActive())
    {
        m_timerScrollText.stop();
    }
}

qButtonItem::~qButtonItem()
{
    StopScrollTimer();
}

void qButtonItem::CheckStartTimer()
{
    StopScrollTimer();

    QFontMetrics fm(this->font());
    m_nWidth = this->rect().width() - CUTOffTEXT_LENGTH;
    m_nDiffLength = fm.width(m_strRealText) - m_nWidth;

    if (m_nDiffLength > 0 && EDK_IsTipModeEnable())
    {
        m_nBegin = 0;
        m_nCount = 0;

        int nShowLenght = 0;
        while (1)
        {
            if (fm.width(m_strRealText.left(++nShowLenght)) > m_nWidth)
            {
                break;
            }
        }

        nShowLenght--;
        QString strShowText = m_strRealText.left(nShowLenght);
        SetShowText(strShowText);

        StartScrollTimer();
    }
    else
    {
        SetShowText(m_strRealText);
    }
}