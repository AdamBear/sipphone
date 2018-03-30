
#include "dsskeybtnbase.h"

#include "baseui/t4xpicpath.h"
#include "dsskeyui/include/moddsskeyui.h"
#include <QStylePainter>
#include "imagemanager/modimagemanager.h"
#include <QtGui>
#include <QPixmap>
#include "qtcommon/qmisc.h"
#include "qtcommon/xmlhelper.h"
#include "uikernel/languagehelper.h"
#include <ylstl/ylstringalgorithm.h>
#include "setting/src/languagecontroller.h"
#include <devicelib/phonedevice.h>
#include "baseui/transwidgetbgcontroler.h"
#include "customtheme/modcustomtheme.h"

#define DSSKEY_LONG_PRESS_TIME  1000
#define DSSKEYBUTTON_SETTING_FONT_SIZE  14

// Dsskey显示Text的模式
typedef enum tagDSSKEY_PIC_MODE
{
    PIC_MODE_NORMAL,    // 正常
    PIC_MODE_HILIGHT,   // 高亮
    PIC_MODE_DISABLE,   // 禁用
    PIC_MODE_USING,     // 正在使用当中
    PIC_MODE_MAX
} DSSKEY_PIC_MODE;

// 更换背景图片
QString TransLongOrNormalPic(DSSKEY_PIC_MODE ePicMode, DSSKEY_DISPLAY_MODE eDisplayMode)
{
    QString strTemp;
    if (DDM_FULL == eDisplayMode)
    {
        switch (ePicMode)
        {
        case PIC_MODE_NORMAL:
            {
                strTemp = PIC_THEME1_LONG_LINEKEY;
            }
            break;
        case PIC_MODE_HILIGHT:
            {
                strTemp = PIC_THEME1_LONG_HIGHLIGHT_LINEKEY;
            }
            break;
        case PIC_MODE_DISABLE:
            {
                strTemp = PIC_THEME1_LONG_DISABLED_LINEKEY;
            }
            break;
        case PIC_MODE_USING:
            {
                strTemp = PIC_THEME1_LONG_DIAL_LINEKEY;
            }
            break;
        default:
            {
                strTemp = PIC_THEME1_LONG_LINEKEY;
            }
            break;
        }
    }
    else
    {
        switch (ePicMode)
        {
        case PIC_MODE_NORMAL:
            {
                strTemp = PIC_THEME1_LINEKEY;
            }
            break;
        case PIC_MODE_HILIGHT:
            {
                strTemp = PIC_THEME1_HIGHLIGHT_LINEKEY;
            }
            break;
        case PIC_MODE_DISABLE:
            {
                strTemp = PIC_THEME1_DISABLED_LINEKEY;
            }
            break;
        case PIC_MODE_USING:
            {
                strTemp = PIC_THEME1_DIAL_LINEKEY;
            }
            break;
        default:
            {
                strTemp = PIC_THEME1_LINEKEY;
            }
            break;
        }
    }

    return strTemp;
}

qDsskeyButton::qDsskeyButton(int iIndex, QWidget * parent)
    : qUIPushButton(parent),
      m_iIndex(iIndex),
      m_strCurrIcon(""),
      m_strPresenceIcon(""),
      m_bFlash(false),
      m_eDssKeyType(-1),
      m_eFlashType(REFRESH_TYPE_NO_FLASH),
      m_eTextMode(TEXT_MODE_FIRST_ROW),
      m_eGroundPhoto(GROUNDPHOTO_MODE_NORMAL),
      m_strLastGroud(""),
      m_strLastIcon(""),
      m_strSecText(""),
      m_bLonePress(false),
      m_nFlashCnt(0),
      m_bRealBtnDown(false),
      m_bFlashGround(false),
      m_iDsskeyID(-1),
      m_bFlushEnable(false)
    , m_bMousePressed(false)
{
    initData();
    m_eDisplayMode = DDM_NORMAL;
    m_pDisplayDelegate = NULL;
}

qDsskeyButton::~qDsskeyButton()
{
    // 如果定时器启动，则停止
    if (m_longPressTimer.isActive())
    {
        m_longPressTimer.stop();
    }
}

bool qDsskeyButton::eventFilter(QObject * pObj, QEvent * pEvent)
{
    if (pEvent == NULL)
    {
        return false;
    }

    if (pEvent->type() == QEvent::Show)
    {
        m_bMousePressed = false;
    }

    return qUIPushButton::eventFilter(pObj, pEvent);
}

// 初始化数据
void qDsskeyButton::initData()
{
    // 初始化为无效位置
    m_point.setX(-1);
    m_point.setY(-1);
    m_longPressTimer.setParent(this);
    connect(&m_longPressTimer, SIGNAL(timeout()), this, SLOT(OnBtnLongPress()));
    this->setIconPicAlign(Qt::AlignLeft); //设置图标显示
    this->setTextAlign(Qt::AlignLeft | Qt::AlignVCenter);
    this->setTextColor(Qt::white); //字体显示颜色
    this->setFocusPolicy(Qt::NoFocus); //无焦点事件

    // 设置图标和文字水平偏移像素
    setTextXOffSet(5);
    setIconLeftOffSet(5);
    int iFontSize = 16;
    // 设置字体大小
    QFont objFont;
    objFont.setPointSize(iFontSize);
    objFont.setWeight(75);
    objFont.setBold(true);
    this->setFont(objFont);
}

void qDsskeyButton::FlashButton(bool bEnable)
{
    if (bEnable == m_bFlushEnable)
    {
        return;
    }

    m_bFlushEnable = bEnable;

    // 闪动背景图
    if (m_bFlashGround)
    {
        m_strFlashGround = !bEnable ? g_TransWidgetBgControler.GetNormalDsskeyBg().c_str() :
                           m_strCurrGroundPath;
    }

    // 闪动图标
    if (m_bFlash == true)
    {
        // 设置闪动的图标
        //setIconPic((bEnable == false) ? "" : m_strCurrIcon);
        if (bEnable)
        {
            if (REFRESH_TYPE_ALTERNATE_FLASH == m_eFlashType)
            {
                m_strCurrIcon = m_strFlashIcon2;
            }
            else
            {
                m_strCurrIcon = m_strIconPic;
            }
        }
        else
        {
            if (REFRESH_TYPE_ALTERNATE_FLASH == m_eFlashType)
            {
                m_strCurrIcon = m_strFlashIcon1;
            }
            else
            {
                m_strCurrIcon = ""; //getNormalPic() ;
            }
        }
    }
    else
    {
        // 恢复默认图标
        m_strCurrIcon = m_strIconPic;
    }

    update();
}

int qDsskeyButton::GetIndex()
{
    return m_iIndex;
}

int qDsskeyButton::GetDsskeyType()
{
    return m_eDssKeyType;
}

DSS_KEY_FLASH_TYPE qDsskeyButton::GetFlashType()
{
    return m_eFlashType;
}

// 设置按钮按下
void qDsskeyButton::SetBtnDown()
{
    setDown(true);
    m_longPressTimer.start(DSSKEY_LONG_PRESS_TIME);
    m_bRealBtnDown = true;
}

void qDsskeyButton::OnBtnClicked()
{
    DSSKEYUI_INFO("CDssKeyButton::OnBtnClicked...:%d", m_iIndex);
    if (m_bRealBtnDown && m_longPressTimer.isActive())
    {
        m_longPressTimer.stop();
        // 发送单击信号，通知框架
        emit btnClicked(m_iDsskeyID);
        m_bRealBtnDown = false;
    }
    setDown(false);

}

bool qDsskeyButton::CheckLightChange(DsskeyDisplayData & objDisplayData,
                                     DsskeyDisplayData & newDisplayData)
{
    bool bChanged = false;

    if (newDisplayData.eDssKeytype != DT_LINE)
    {
        return true;
    }

    if (objDisplayData.eDssKeytype != newDisplayData.eDssKeytype
            || objDisplayData.eLightColorType != newDisplayData.eLightColorType
            || objDisplayData.eGroundPhoto != newDisplayData.eGroundPhoto)
    {
        return true;
    }

    if (objDisplayData.iStatus != newDisplayData.iStatus)
    {
        bChanged = true;
    }
    else
    {
        if (objDisplayData.listAuxiliaryState.size() != newDisplayData.listAuxiliaryState.size())
        {
            bChanged = true;
        }
        else
        {
            YLList<int>::iterator olditr = objDisplayData.listAuxiliaryState.begin();
            YLList<int>::iterator olditrEnd = objDisplayData.listAuxiliaryState.end();
            YLList<int>::iterator newitr = newDisplayData.listAuxiliaryState.begin();
            YLList<int>::iterator newitrEnd = newDisplayData.listAuxiliaryState.end();
            for (; olditr != olditrEnd && newitr != newitrEnd; olditr++, newitr++)
            {
                if (*olditr != *newitr)
                {
                    bChanged = true;
                    break;
                }
            }
        }
    }

    return bChanged;
}

// 更新显示内容
bool qDsskeyButton::UpdateContent(const DsskeyDisplayData & sData)
{
    bool bUpdate = false;

    m_iDsskeyID = sData.iDsskeyID;
    m_eDssKeyType = sData.eDssKeytype;

    yl::string strtmp = m_strCurrIcon.toUtf8().data();
    if (strtmp != sData.strIconPath)
    {
        bUpdate = true;
        m_strCurrIcon = toQString(sData.strIconPath.c_str());
    }

    // 设置显示的图标,当前图标可能在闪烁
    setIconPic(m_strCurrIcon);

    strtmp = m_strDownCurrIcon.toUtf8().data();
    if (strtmp != sData.strDownIconPath)
    {
        m_strDownCurrIcon = toQString(sData.strDownIconPath.c_str());
        bUpdate = true;
    }
    // 闪烁图标
    strtmp = m_strFlashIcon1.toUtf8().data();
    if (strtmp != sData.strFlashIcon1)
    {
        m_strFlashIcon1 = toQString(sData.strFlashIcon1.c_str());
        bUpdate = true;
    }

    strtmp = m_strFlashIcon2.toUtf8().data();
    if (strtmp != sData.strFlashIcon2)
    {
        m_strFlashIcon2 = toQString(sData.strFlashIcon2.c_str());
        bUpdate = true;
    }

    strtmp = m_strPresenceIcon.toUtf8().data();
    if (strtmp != sData.strPresenceIcon)
    {
        m_strPresenceIcon = toQString(sData.strPresenceIcon.c_str());
        bUpdate = true;
    }

    if (m_eTextMode != sData.eTextMode)
    {
        m_eTextMode = sData.eTextMode;
        bUpdate = true;
        m_txStatus.bContentChanged = true;
    }

    DsskeyDisplayData newData = sData;
    bool bLightChange = CheckLightChange(m_dispData, newData);
    //http://10.2.1.199/Bug.php?BugID=87303
    if (m_eGroundPhoto != sData.eGroundPhoto
            && bLightChange/* 旧解析的背景图没变，不刷新 */)
    {
        m_eGroundPhoto = sData.eGroundPhoto;
        bUpdate = true;
    }

    strtmp = this->text().toUtf8().data();
    if (strtmp != sData.strFirstRowText)
    {
        setText(toQString(sData.strFirstRowText.c_str()));
        bUpdate = true;
        m_txStatus.bContentChanged = true;
    }

    if (m_eTextMode == TEXT_MODE_SEC_ROW)
    {
        strtmp = m_strSecText.toUtf8().data();
        if (strtmp != sData.strSecRowText)
        {
            m_strSecText = toQString(sData.strSecRowText.c_str());
            bUpdate = true;
            m_txStatus.bContentChanged = true;
        }
        DSSKEYUI_DEBUG("m_strSecText() is %s.", m_strSecText.toUtf8().data());
    }

    return bUpdate;
}

// 更新闪烁内容
bool qDsskeyButton::UpdateFlashStatus(const DSS_KEY_FLASH_TYPE & flashType)
{
    bool bUpdate = false;
    // 先判断
    if (m_eFlashType != flashType)
    {
        // 闪烁状态改变，先停止闪烁，再开启新闪烁类型
        m_bFlash = false;
        // 由于停止闪烁时，需要外部会判断当前按键的新闪烁类型来询问
        // 是否正确停止窗体的定时器，所以缓存上次的闪烁类型进行停止
        DSS_KEY_FLASH_TYPE lastFlashType = m_eFlashType;
        m_eFlashType = REFRESH_TYPE_NO_FLASH;

        emit stopFlash(lastFlashType);

        if (flashType != REFRESH_TYPE_NO_FLASH)
        {
            m_bFlash = true;
            emit startFlash(flashType);
        }
        m_eFlashType = flashType;

        bUpdate = true;
    }

    return bUpdate;
}


//更新显示状态
void qDsskeyButton::UpdateDisplayStatus(const DsskeyDisplayData & sData)
{
    //m_iIndex = sData.iIndex;
    bool bUpdate = false;


    // 判断是否要刷新整个按键
    if (UpdateContent(sData))
    {
        bUpdate = true;
    }

    // 图标刷新更新
    if (UpdateFlashStatus(sData.eRefreshType))
    {
        bUpdate = true;
    }

    DSSKEYUI_DEBUG("m_eGroundPhoto is %d, m_eTextMode is %d.", m_eGroundPhoto, m_eTextMode);
    // 缓存显示数据
    m_dispData = sData;

//  CreateTextPiccache(m_pmTextPiccache);
    if (bUpdate)
    {
        update();
    }

#if IF_BUG_24956
    //话机自动更新的dsskey状态，一定不会出现背景图片闪烁
    m_bFlashGround = false;
#endif
}

// 设置显示代理
bool qDsskeyButton::SetDisplayDelegate(DsskeyBtnDisplayDelegete * pDelegate)
{
    bool bRet = false;
    if (NULL != pDelegate && m_pDisplayDelegate != pDelegate)
    {
        m_pDisplayDelegate = pDelegate;
        update();
        bRet = true;
    }
    return bRet;
}

// 清除显示代理
void qDsskeyButton::DebindDisplayDelegate()
{
    if (m_pDisplayDelegate != NULL)
    {
        m_pDisplayDelegate = NULL;
        update();
    }
}

// 绘制图片文本内容
void qDsskeyButton::DrawTextPic(QPainter & p, QRect rc, int flag, QString strTx)
{
    //进行字符串的解析翻译及重组
    QString strTemp = LANG_PARSE(strTx);
#ifdef IF_FEATURE_ARABIC
    ReverseRTLMaskString(strTemp);
#else
    // 希伯来语处理
    yl::wstring chwText = yl::to_utf16(fromQString(strTemp).c_str());
    ReverseRTLWord(chwText);
    strTemp = toQString(yl::to_utf8(chwText));
#endif

    QString strDisp = CutOffText(strTemp, p.fontMetrics(), rc.width() - 3);
    // 绘制第一行的Text阴影
    p.drawText(rc, flag, strDisp);
}

// 构造文本图片缓存内容
void qDsskeyButton::CreateTextPiccache(QPixmap & textpic)
{

    QRect rc = rect();
    int text_width = 0;
    int icon_width = 0;

    // 判断Icon的对齐方式.目前只有居左、居右、居中三种情况
    bool isIconAlignLeft = (getIconAlign() & Qt::AlignLeft) == Qt::AlignLeft;
    bool isIconAlignRight = (getIconAlign() & Qt::AlignRight) == Qt::AlignRight;
    bool isIconAlignCenter = (getIconAlign() & Qt::AlignCenter) == Qt::AlignCenter;

    if (!m_pmLastIcon.isNull())
    {
        icon_width = m_pmLastIcon.width();
    }

    if (isIconAlignLeft)
    {
        // +m_iTextXOffSet 在ICON和文字之间留一点空隙
        // 默认多偏移一个icon的宽度
        text_width = rc.width() - 36 - getTextXOffSet();
    }
    else if (isIconAlignRight)
    {
        // -m_iTextXOffSet 在ICON和文字之间留一点空隙
        text_width = rc.width() - icon_width - getTextXOffSet();
    }

    QPixmap pmFrameBG(text_width, rc.height());
    pmFrameBG.fill(QColor(0, 0, 0, 0));

    QPainter p(&pmFrameBG);
    // 设置字体大小
    QFont fontTitle = font();
    fontTitle.setPointSize(CustomTheme_GetCustomFontSizeByKey(CUSTOM_FONT_ID_LINEKEY_NORMAL));
    p.setFont(fontTitle);
    // 显示的图标 和 文字颜色
    QString strDispIcon;
    QRect rcText = QRect(0, 0, text_width, rc.height());

    QString tx = this->text();

    // 若是Text为空，就没必要绘制
    if (tx.isEmpty())
    {
        return ;
    }

    // 翻译后的文本
    QString qText = LANG_PARSE(tx);

    int iTranWordWith = p.fontMetrics().width(qText);
#if IF_BUG_25010
    int nPos = qText.indexOf('\\');
#endif
    // 绘制文本图片
    if (m_eTextMode == TEXT_MODE_SEC_ROW)
    {
        QRect rcSec;
        // 第二行的矩形区域
        rcSec.setRect(rcText.x(), rcText.height() / 2, rcText.width(), rc.height() / 2 - 5);
        // 第一行的矩形区域
        rcText.setY(rcText.y() + 5);
        rcText.setHeight(rc.height() / 2 - 5);

        QRect rcShadow = rcText;
        // 特效处理，阴影绘制在左下方
        rcShadow.adjust(-1, 1, -1, 1);
        QPen penNormal = p.pen();
        QPen penShadow = penNormal;
        penShadow.setColor(qRgb(0, 0, 0));
        penNormal.setColor(CustomTheme_GetCustomFontColorByKey(CUSTOM_FONT_ID_LINEKEY_NORMAL));
        p.setPen(penShadow);

#if IF_BUG_25010
        if (configParser_GetConfigInt(kszDsskeyLength) == 1
                && (iTranWordWith > rcText.width()
                    || nPos != -1))
        {
            // 分两行显示，计算截断位置
            int nCount = GetCutoffPos(qText, p.fontMetrics(), rcText.width() - 3);
            if (nPos != -1
                    && nCount >= nPos)
            {
                tx = qText.left(nPos);
            }
            else if (m_eDssKeyType == DT_LINE)
            {
                tx = CutOffTextExt(qText, p.fontMetrics(), rcText.width() - 3);
            }
        }
#endif
        // 绘制第一行的Text阴影
        DrawTextPic(p, rcShadow, getTextAlign(), tx);
        rcShadow = rcSec;
        // 特效处理，阴影绘制在左下方
        rcShadow.adjust(-1, 1, -1, 1);
        // 绘制第二行的Text阴影
        DrawTextPic(p, rcShadow, getTextAlign(), m_strSecText);

        p.setPen(penNormal);
        // 绘制第一行的Text
        DrawTextPic(p, rcText, getTextAlign(), tx);
        // 绘制第二行的Text
        DrawTextPic(p, rcSec, getTextAlign(), m_strSecText);
    }
    else
    {
        if (configParser_GetConfigInt(kszDsskeyLength) == 1
                && (iTranWordWith > rcText.width() - 3
#if IF_BUG_25010
                    || nPos != -1
#endif
                   ))
        {
            QRect rcSec;
            // 第二行的矩形区域
            rcSec.setRect(rcText.x(), rcText.height() / 2, rcText.width(), rc.height() / 2 - 5);
            // 第一行的矩形区域
            rcText.setY(rcText.y() + 5);
            rcText.setHeight(rc.height() / 2 - 5);

            QString strFirstLine;
            QString strSecondLine;

            // 分两行显示，计算截断位置
            int nCount = GetCutoffPos(qText, p.fontMetrics(), rcText.width() - 3);
#if IF_BUG_25010
            if (nPos != -1
                    && nCount >= nPos)
            {
                // 如果换行符前面的字符串长度超出，则忽略换行符
                strFirstLine = qText.left(nPos);
                if (m_eDssKeyType == DT_LINE)
                {
                    strSecondLine = CutOffTextExt(qText.mid(nPos + 1), p.fontMetrics(), rcSec.width() - 3);
                }
                else
                {
                    strSecondLine = CutOffText(qText.mid(nPos + 1), p.fontMetrics(), rcSec.width() - 3);
                }

            }
            else
            {
                QString strDisp;
                // 根据2行长度先获取截断后需要显示的2行内容
                if (DT_LINE == m_eDssKeyType)
                {
                    strDisp = CutOffTextExt(qText, p.fontMetrics(), (rcText.width() - 4) * 2);
                }
                else
                {
                    strDisp = CutOffText(qText, p.fontMetrics(), (rcText.width() - 4) * 2);
                }

                int nCutPos = GetCutoffPos(strDisp, p.fontMetrics(), rcText.width() - 3);
                strFirstLine = strDisp.left(nCutPos);
                strSecondLine = strDisp.mid(nCutPos);
            }

            // 不需要显示换行符
            strSecondLine.remove('\\');
#else
            strFirstLine = qText.left(nCount);
            strSecondLine = CutOffText(qText.mid(nCount), p.fontMetrics(), rcSec.width() - 3);
#endif
            QRect rcShadow = rcText;
            // 特效处理，阴影绘制在左下方
            rcShadow.adjust(-1, 1, -1, 1);
            QPen penNormal = p.pen();
            QPen penShadow = penNormal;
            penShadow.setColor(qRgb(0, 0, 0));
            penNormal.setColor(CustomTheme_GetCustomFontColorByKey(CUSTOM_FONT_ID_LINEKEY_NORMAL));
            p.setPen(penShadow);

            // 绘制第一行的Text阴影
            p.drawText(rcShadow, getTextAlign(), strFirstLine);
            rcShadow = rcSec;
            // 特效处理，阴影绘制在左下方
            rcShadow.adjust(-1, 1, -1, 1);
            // 绘制第二行的Text阴影
            p.drawText(rcShadow, getTextAlign(), strSecondLine);

            p.setPen(penNormal);
            // 绘制第一行的Text
            p.drawText(rcText, getTextAlign(), strFirstLine);
            // 绘制第二行的Text
            p.drawText(rcSec, getTextAlign(), strSecondLine);
        }
#if IF_BUG_25010
        else if (m_eDssKeyType == DT_LINE)
        {
            QRect rcShadow = rcText;
            QPen penNormal = p.pen();
            QPen penShadow = penNormal;
            penShadow.setColor(qRgb(0, 0, 0));
            penNormal.setColor(Qt::white);
            p.setPen(penShadow);

            QString strDisp = CutOffTextExt(qText, p.fontMetrics(), rcText.width() - 4);
            // 绘制第一行的Text
            p.drawText(rcShadow, getTextAlign(), strDisp);
            // 特效处理，阴影绘制在左下方
            rcShadow.adjust(-1, 1, -1, 1);
            p.setPen(penNormal);
            // 绘制第一行的Text阴影
            p.drawText(rcShadow, getTextAlign(), strDisp);
        }
#endif
        else
        {
            QRect rcShadow = rcText;
            // 特效处理，阴影绘制在左下方
            rcShadow.adjust(-1, 1, -1, 1);
            QPen penNormal = p.pen();
            QPen penShadow = penNormal;
            penShadow.setColor(qRgb(0, 0, 0));
            penNormal.setColor(CustomTheme_GetCustomFontColorByKey(CUSTOM_FONT_ID_LINEKEY_NORMAL));
            p.setPen(penShadow);
            DrawTextPic(p, rcShadow, getTextAlign(), tx);
            p.setPen(penNormal);
            // 绘制Item的Text
            DrawTextPic(p, rcText, getTextAlign(), tx);
        }
    }
    // 保存构造的图片
    textpic = pmFrameBG;
}

void qDsskeyButton::UpdateGroudPic(const DsskeyDisplayData & sData)
{
    DSSKEYUI_INFO("m_eGroundPhoto [%d],FlashType [%d] Index [%d].", m_eGroundPhoto, sData.eRefreshType,
                  sData.iIndex);

    m_eGroundPhoto = sData.eGroundPhoto;
    DSS_KEY_FLASH_TYPE flashType = sData.eRefreshType;
    // 先判断
    if (m_eFlashType != sData.eRefreshType)
    {
        // 闪烁状态改变，先停止闪烁，再赋新闪烁类型
        m_bFlashGround = false;
        // 背景闪烁，图标闪烁必须停止
        m_bFlash = false;
        // 是否正确停止窗体的定时器，所以缓存上次的闪烁类型进行停止
        DSS_KEY_FLASH_TYPE lastFlashType = m_eFlashType;
        m_eFlashType = REFRESH_TYPE_NO_FLASH;
        emit stopFlash(lastFlashType);

        if (flashType != REFRESH_TYPE_NO_FLASH)
        {
            m_bFlashGround = true;
            emit startFlash(flashType);
        }
        m_eFlashType = flashType;
    }
    else
    {
        //dsskey原状态为闪烁，执行LED flash语句，统一为背景图片闪烁
        if (flashType != REFRESH_TYPE_NO_FLASH)
        {
            m_bFlashGround = true;
        }
    }
    update();
}

// 获取显示图标的路径
QString qDsskeyButton::GetPresenceIconPath()
{
    return m_strPresenceIcon;
}


void qDsskeyButton::OnBtnLongPress()
{
    if (m_iIndex > -1
            && m_longPressTimer.isActive())
    {
        m_longPressTimer.stop();
        DSSKEYUI_INFO("CDssKeyButton::OnLongPress():m_iIndex = %d", m_iIndex);
    }

    // 判断抬起位置是否在按钮控件内,因为按钮图片有边缘阴影效果，实际响应区域进行适当缩小
    QPoint nPos = this->mapToGlobal(QPoint(0, 0));
    QSize nSize = this->size();
    QPoint nMPos = m_point;
    if (nMPos.x() > (nPos.x() + 2) && nMPos.y() > (nPos.y() + 2)
            && nMPos.x() < (nPos.x() + nSize.width() - 2)
            && nMPos.y() < (nPos.y() + nSize.height() - 2))
    {
        // 触屏 触发长按消息，通知框架
        emit btnLongPress(m_iDsskeyID);
    }

    if (m_bRealBtnDown)
    {
        // 按键 触发长按消息，通知框架
        emit btnLongPress(m_iDsskeyID);
        m_bRealBtnDown = false;
    }

    setDown(false);
    // 重置标志
    m_bLonePress = true;
}

void qDsskeyButton::mousePressEvent(QMouseEvent * e)
{
    m_bMousePressed = true;
    // 重置标志
    m_bLonePress = false;
    //开始计时，并且设置时间长度，超过则发出timeout信号
    m_longPressTimer.start(DSSKEY_LONG_PRESS_TIME);
    emit btnPress(m_iDsskeyID);
    // 保存鼠标位置，用于长按判断
    m_point = e->globalPos();
    qUIPushButton::mousePressEvent(e);
}

void qDsskeyButton::mouseMoveEvent(QMouseEvent * e)
{
    // 保存鼠标位置，用于长按判断
    m_point = e->globalPos();
    qUIPushButton::mouseMoveEvent(e);
}

void qDsskeyButton::mouseReleaseEvent(QMouseEvent * e)
{
    // 停止长按定时器，防止再响应长按效果
    m_longPressTimer.stop();
    qUIPushButton::mouseReleaseEvent(e);

    // 重置为无效位置
    m_point.setX(-1);
    m_point.setY(-1);
    //不是长按，响应clicked
    if (!m_bLonePress)
    {
        QPoint nPos = this->mapToGlobal(QPoint(0, 0));
        QSize nSize = this->size();
        QPoint nMPos = e->globalPos();
        // 判断抬起位置是否在按钮控件内,因为按钮图片有边缘阴影效果，实际响应区域进行适当缩小
        if (nMPos.x() > (nPos.x() + 2) && nMPos.y() > (nPos.y() + 2)
                && nMPos.x() < (nPos.x() + nSize.width() - 2)
                && nMPos.y() < (nPos.y() + nSize.height() - 2))
        {
            if (m_bMousePressed)
            {
                emit btnClicked(m_iDsskeyID);
            }
        }
    }
    m_bLonePress = false;

    m_bMousePressed = false;
}

// 判断使用的图标是否改变,绘制时根据状态获取真正需要显示图标
bool qDsskeyButton::IsUsedIconCache(QString strDispIcon)
{
    return (m_strLastIcon == strDispIcon && !m_pmLastIcon.isNull()
            && (strDispIcon.indexOf(TEMP_PATH) == -1));
}

// 检测使用的文本是否改变
bool qDsskeyButton::CheckTextChanged(const bool & bUC, const BtnStatus & status)
{
    yl::string CurLang = g_pLanguageController->GetCurrentLanguage();
    bool bNewLineMode = configParser_GetConfigInt(kszDsskeyLength) == 1;

    bool bNeedCreateTxCache = false;
    // T48的文字显示不随按键状态和UC状态变化而变化
    if (m_txStatus.languageLast != CurLang || m_txStatus.m_bNewLineMode != bNewLineMode \
            /*|| m_txStatus.bUCEnableLast != bCurUCEnable*/ || m_txStatus.bContentChanged \
            /*|| m_txStatus.btnStatusLast != CurbtnStatus*/ || m_pmTextPiccache.isNull())
    {
        bNeedCreateTxCache = true;
        CreateTextPiccache(m_pmTextPiccache);
        m_txStatus.bContentChanged = false;
        m_txStatus.languageLast = CurLang;
        m_txStatus.m_bNewLineMode = bNewLineMode;
        DSSKEYUI_INFO("Text changed. ContentChanged(%d)", m_txStatus.bContentChanged);
    }


    return bNeedCreateTxCache;
}

void qDsskeyButton::paintEvent(QPaintEvent * pe)
{
//  DSSKEYUI_INFO("Test qDsskeyButton ::paintEvent Start.m_iDsskeyID(%d).", m_iDsskeyID);
    if ((m_eGroundPhoto == GROUNDPHOTO_MODE_NORMAL)          //普通状态
            && this->isEnabled()                                    //且非禁用
            && !(this->isDown() || this->isChecked())                //且非按下状态
            //以上为判断 是否需要绘制背景
            && m_strCurrIcon.isEmpty()          //图标为空
            && this->text().isEmpty()           //文字为空
       )
    {
        //到这 说明这个btn 是不需要绘制的
        return ;
    }

    QStylePainter p(this);

    if (NULL != m_pDisplayDelegate)
    {
        DSSKEYUI_INFO("Custom display Btn m_iDsskeyID(%d).", m_iDsskeyID);
        m_pDisplayDelegate->PaintDsskeyBtn(p, m_dispData, this);
        return ;
    }


    QPixmap pm;
    QStyleOptionButton option;
    initStyleOption(&option);
    QPalette pal = option.palette;
    // 设置文字颜色为黑色
    pal.setColor(QPalette::Text, qRgb(0, 0, 0));
    // 设置字体大小
    QFont fontTitle = font();
    fontTitle.setPointSize(CustomTheme_GetCustomFontSizeByKey(CUSTOM_FONT_ID_LINEKEY_NORMAL));
    p.setFont(fontTitle);
    // 显示的图标 和 文字颜色
    QString strDispIcon;
    QPen pen = p.pen();
    QString strBkGround;
    BtnStatus CurbtnStatus = BTN_STASTUS_NORMAL;

    if (m_eGroundPhoto == GROUNDPHOTO_MODE_NORMAL)
    {
        // 判断button的状态，设置对应的图片
        if (!this->isEnabled())
        {
            strBkGround = g_TransWidgetBgControler.GetDisableDsskeyBg().c_str();
            strDispIcon = m_strCurrIcon;
            pen.setColor(Qt::white);
            CurbtnStatus = BTN_STASTUS_DISABLE;
        }
        else if (this->isDown() || this->isChecked())
        {
            strBkGround = g_TransWidgetBgControler.GetEnableDsskeyBg().c_str();
            // 去除反白效果
            // strDispIcon = m_strDownCurrIcon;
            strDispIcon = m_strCurrIcon;
            pen.setColor(Qt::white);
            CurbtnStatus = BTN_STASTUS_DOWN;
        }
        else
        {
            //          strBkGround = getNormalPic();
            strDispIcon = m_strCurrIcon;
            pen.setColor(Qt::white);
            CurbtnStatus = BTN_STASTUS_NORMAL;
        }
    }
    else if (m_eGroundPhoto == GROUNDPHOTO_MODE_HIGHLIGHT)
    {
        // 高亮状态背景
        strBkGround = g_TransWidgetBgControler.GetHeightlightDsskeyBg().c_str();
        // 去除反白效果
        // strDispIcon = m_strDownCurrIcon;
        strDispIcon = m_strCurrIcon;
        pen.setColor(Qt::white);
        CurbtnStatus = BTN_STASTUS_HIGHLIGHT;
    }
    else if (m_eGroundPhoto == GROUNDPHOTO_MODE_USING)
    {
        // 使用状态背景
        strBkGround = g_TransWidgetBgControler.GetUsingDsskeyBg().c_str();
        // 去除反白效果
        // strDispIcon = m_strDownCurrIcon;
        strDispIcon = m_strCurrIcon;
        pen.setColor(Qt::white);
        CurbtnStatus = BTN_STASTUS_USING;
    }
    else if (m_eGroundPhoto == GROUNDPHOTO_MODE_GREEN)
    {
        // 使用状态背景
        strBkGround = g_TransWidgetBgControler.GetGreenDsskeyBg().c_str();
        m_strCurrGroundPath = g_TransWidgetBgControler.GetGreenDsskeyBg().c_str();
        if (m_bFlashGround && !m_strFlashGround.isEmpty())
        {
            strBkGround = m_strFlashGround;
        }
        // 去除反白效果
        // strDispIcon = m_strDownCurrIcon;
        strDispIcon = m_strCurrIcon;
        pen.setColor(Qt::white);
    }
    else if (m_eGroundPhoto == GROUNDPHOTO_MODE_RED)
    {
        // 使用状态背景
        strBkGround = g_TransWidgetBgControler.GetRedDsskeyBg().c_str();
        m_strCurrGroundPath = g_TransWidgetBgControler.GetRedDsskeyBg().c_str();
        if (m_bFlashGround && !m_strFlashGround.isEmpty())
        {
            strBkGround = m_strFlashGround;
        }
        // 去除反白效果
        // strDispIcon = m_strDownCurrIcon;
        strDispIcon = m_strCurrIcon;
        pen.setColor(Qt::white);
    }
    else if (m_eGroundPhoto == GROUNDPHOTO_MODE_ORANGE)
    {
        // 使用状态背景
        strBkGround = g_TransWidgetBgControler.GetOrangeDsskeyBg().c_str();
        m_strCurrGroundPath = g_TransWidgetBgControler.GetOrangeDsskeyBg().c_str();
        if (m_bFlashGround && !m_strFlashGround.isEmpty())
        {
            strBkGround = m_strFlashGround;
        }
        // 去除反白效果
        // strDispIcon = m_strDownCurrIcon;
        strDispIcon = m_strCurrIcon;
        pen.setColor(Qt::white);
    }
    else
    {
        strBkGround = g_TransWidgetBgControler.GetDisableDsskeyBg().c_str();
        strDispIcon = m_strCurrIcon;
        pen.setColor(Qt::black);
    }
    if (m_strLastGroud == strBkGround && !m_pmLastGroudPic.isNull())
    {
        // 图片没变，直接使用缓存的图片
        pm = m_pmLastGroudPic;
    }
    else
    {
        pm = THEME_GET_BMP(strBkGround.toUtf8().data());
        // 保存上次类型和使用的图片
        m_strLastGroud = strBkGround;
        m_pmLastGroudPic = pm;
    }
    // 设置绘制的文字颜色
    p.setPen(pen);
    // 画按钮背景图片
    if (!pm.isNull())
    {
        p.drawPixmap(option.rect.x(), option.rect.y()
                     , option.rect.width(), option.rect.height(), pm);
    }
//  DSSKEYUI_INFO("Test qDsskeyButton ::paintEvent strBkGround.");
    // 画按钮上的ICON
    QPixmap pmIcon;

    // 判断Icon的对齐方式.目前只有居左、居右、居中三种情况
    bool isIconAlignLeft = (getIconAlign() & Qt::AlignLeft) == Qt::AlignLeft;
    bool isIconAlignRight = (getIconAlign() & Qt::AlignRight) == Qt::AlignRight;
    bool isIconAlignCenter = (getIconAlign() & Qt::AlignCenter) == Qt::AlignCenter;

    QRect rcIcon;
    int x = 0;
    bool bCurUCEnable = false;
    if (!strDispIcon.isEmpty())
    {
        //加载图片 因为有可能是自定义图标
        ////---- T48 加载图片的方式需要优化

        //      pmIcon.load(getIconPic().toUtf8().data());
        // 由于上面那种加载图片的方式无法正确加载图片，暂时用下述的加载方式
        // UC的/tmp图标路径地址可能相同，但是图标可能会改变
        if (IsUsedIconCache(strDispIcon))
        {
            // 图标没变，直接使用缓存的图标
            pmIcon = m_pmLastIcon;
        }
        else
        {
            if (configParser_GetConfigInt(kszEnableUC) == 1
                    && configParser_GetConfigInt(kszBroadsoftActive) == 1)
            {
                TMP_GET_BMP(pmIcon, strDispIcon.toUtf8().data());
            }
            else
            {
                pmIcon = THEME_GET_BMP(strDispIcon.toUtf8().data());
            }
            // 保存上次类型和使用的图片
            m_strLastIcon = strDispIcon;
            m_pmLastIcon = pmIcon;
        }

        // 通过Icon的对齐方式，计算绘制的x坐标
        if (isIconAlignLeft)
        {
            x = option.rect.x() + getIconLeftOffSet() + 3;
        }
        else if (isIconAlignRight)
        {
            x = option.rect.x() + option.rect.width() - pmIcon.width() - getIconLeftOffSet();
        }
        else if (isIconAlignCenter)
        {
            x = option.rect.x() + (option.rect.width() - pmIcon.width()) / 2;
        }

        // Icon的Y坐标默认居中
        int y = option.rect.y() + option.rect.height() / 2 - pmIcon.height() / 2;

        rcIcon.setRect(x, y, pmIcon.width(), pmIcon.height());

        // 如果存在状态图标
        if ((configParser_GetConfigInt(kszEnableUC) == 1) && (!m_strPresenceIcon.isEmpty())
                && configParser_GetConfigInt(kszBroadsoftActive) == 1)
        {
            QPixmap pmPresence;
            DSSKEYUI_INFO("m_strPresenceIcon path (%s)", m_strPresenceIcon.toUtf8().data());
            pmPresence = THEME_GET_BMP(m_strPresenceIcon.toUtf8().data());
            // 画按钮状态图片
            if (!pmPresence.isNull())
            {
                //坐标无效时，缩小图片
                rcIcon.setRect(option.rect.x() + 6, option.rect.y() + 13, 29, 29);
                p.drawPixmap(rcIcon, pmIcon);
                QPixmap pmFrame;
                pmFrame = THEME_GET_BMP(PIC_DSS_FRAME);
                // 画按钮状态图片
                if (!pmFrame.isNull())
                {
                    QRect rectFrame;
                    rectFrame.setRect(option.rect.x() + 5, option.rect.y() + 12, pmFrame.width(), pmFrame.height());
                    p.drawPixmap(rectFrame, pmFrame);
                }

                //是否显示新状态图标
                bool bNewIconMode = configParser_GetConfigInt(kszPresenceIconMode) == 1;
                QRect rectPresence;
                // 状态图标在功能图标的右下角
                if (bNewIconMode)
                {
                    rectPresence.setRect(rcIcon.x() + rcIcon.width() - pmPresence.width(),
                                         rcIcon.y() + rcIcon.height() - pmPresence.height(), pmPresence.width(), pmPresence.height());
                }
                else
                {
                    rectPresence.setRect(rcIcon.x() + rcIcon.width() - pmPresence.width() / 2 - 2,
                                         rcIcon.y() + rcIcon.height() - pmPresence.height() / 2 - 2, pmPresence.width(),
                                         pmPresence.height());
                }
                p.drawPixmap(rectPresence, pmPresence);
                bCurUCEnable = true;
            }
        }
        else
        {
            // 判断Icon坐标是否为有效坐标
            if (x >= 0 && x < option.rect.x() + option.rect.width() &&
                    y >= 0 && y < option.rect.y() + option.rect.height())
            {
                // 坐标有效绘制Icon
                p.drawPixmap(rcIcon, pmIcon);
            }
            else
            {
                //坐标无效时，缩小图片
                rcIcon.setRect(option.rect.x() + 8, option.rect.y() + 14, 25, 25);
                p.drawPixmap(rcIcon, pmIcon);
            }
        }

    }
//  DSSKEYUI_INFO("Test qDsskeyButton ::paintEvent strDispIcon.");
    QString tx = this->text();

    // 若是Text为空，就没必要绘制
    if (tx.isEmpty())
    {
        return ;
    }

    // 画按钮上的文本, 如果已经画了ICON, 则需要进行相应的偏移
    QRect rcText = option.rect;

    if (isIconAlignLeft)
    {
        // +m_iTextXOffSet 在ICON和文字之间留一点空隙
        // 默认多偏移一个icon的宽度
        rcText.setX(rcText.x()  + 36 + getTextXOffSet());
    }
    else if (isIconAlignRight)
    {
        // -m_iTextXOffSet 在ICON和文字之间留一点空隙
        rcText.setWidth(rcText.width() - rcIcon.right() - getTextXOffSet());
    }

    // 状态变化
    bool bRet = CheckTextChanged(bCurUCEnable, CurbtnStatus);

    if (!m_pmTextPiccache.isNull())
    {
        p.drawPixmap(rcText, m_pmTextPiccache);
    }
    else
    {
        if (m_eTextMode == TEXT_MODE_SEC_ROW)
        {
            QRect rcSec;
            // 第二行的矩形区域
            rcSec.setRect(rcText.x(), rcText.height() / 2, rcText.width(), option.rect.height() / 2 - 5);
            // 第一行的矩形区域
            rcText.setY(rcText.y() + 5);
            rcText.setHeight(option.rect.height() / 2 - 5);

            QRect rcShadow = rcText;
            // 特效处理，阴影绘制在左下方
            rcShadow.adjust(-1, 1, -1, 1);
            QPen penNormal = p.pen();
            QPen penShadow = penNormal;
            penShadow.setColor(qRgb(0, 0, 0));
            p.setPen(penShadow);

            // 绘制第一行的Text阴影
            p.drawItemText(rcShadow, getTextAlign(), pal, this->isEnabled(), tx);
            rcShadow = rcSec;
            // 特效处理，阴影绘制在左下方
            rcShadow.adjust(-1, 1, -1, 1);
            // 绘制第二行的Text阴影
            p.drawItemText(rcShadow, getTextAlign(), pal, this->isEnabled(), m_strSecText);

            p.setPen(penNormal);
            // 绘制第一行的Text
            p.drawItemText(rcText, getTextAlign(), pal, this->isEnabled(), tx);
            // 绘制第二行的Text
            p.drawItemText(rcSec, getTextAlign(), pal, this->isEnabled(), m_strSecText);
        }
        else
        {
            // 翻译后的文本
            QString qText = LANG_PARSE(tx);
            int iTranWordWith = p.fontMetrics().width(qText);
            if (configParser_GetConfigInt(kszDsskeyLength) == 1
                    && iTranWordWith > rcText.width())
            {
                QRect rcSec;
                // 第二行的矩形区域
                rcSec.setRect(rcText.x(), rcText.height() / 2, rcText.width(), option.rect.height() / 2 - 5);
                // 第一行的矩形区域
                rcText.setY(rcText.y() + 5);
                rcText.setHeight(option.rect.height() / 2 - 5);

                // 分两行显示，计算截断位置
                int nCount = GetCutoffPos(qText, p.fontMetrics(), rcText.width() - 3);
                QString strFirstLine = qText.left(nCount);
                QString strSecondLine = CutOffText(qText.mid(nCount), p.fontMetrics(), rcSec.width() - 3);

                QRect rcShadow = rcText;
                // 特效处理，阴影绘制在左下方
                rcShadow.adjust(-1, 1, -1, 1);
                QPen penNormal = p.pen();
                QPen penShadow = penNormal;
                penShadow.setColor(qRgb(0, 0, 0));
                p.setPen(penShadow);

                // 绘制第一行的Text阴影
                p.drawText(rcShadow, getTextAlign(), strFirstLine);
                rcShadow = rcSec;
                // 特效处理，阴影绘制在左下方
                rcShadow.adjust(-1, 1, -1, 1);
                // 绘制第二行的Text阴影
                p.drawText(rcShadow, getTextAlign(), strSecondLine);

                p.setPen(penNormal);
                // 绘制第一行的Text
                p.drawText(rcText, getTextAlign(), strFirstLine);
                // 绘制第二行的Text
                p.drawText(rcSec, getTextAlign(), strSecondLine);
            }
            else
            {
                QRect rcShadow = rcText;
                // 特效处理，阴影绘制在左下方
                rcShadow.adjust(-1, 1, -1, 1);
                QPen penNormal = p.pen();
                QPen penShadow = penNormal;
                penShadow.setColor(qRgb(0, 0, 0));
                p.setPen(penShadow);
                p.drawItemText(rcShadow, getTextAlign(), pal, this->isEnabled(), tx);
                p.setPen(penNormal);
                // 绘制Item的Text
                p.drawItemText(rcText, getTextAlign(), pal, this->isEnabled(), tx);
            }
        }
    }
//  DSSKEYUI_INFO("Test qDsskeyButton ::paintEvent End.");
}

// 设置第二行文字
void qDsskeyButton::SetSecondText(QString strSec)
{
    m_strSecText = strSec;
}

// 获取第二行文字
QString qDsskeyButton::GetSecondText()
{
    return m_strSecText;
}

// 分两行显示，计算截断位置
int qDsskeyButton::GetCutoffPos(const QString & strText, const QFontMetrics & objFontMetrics,
                                int iWidth)
{
    // 二分法确定截断位置的范围
    int iMinIndex = 0;
    int iMaxIndex = strText.size() - 1;
    int iMidIndex = (iMinIndex + iMaxIndex) >> 1;
    int iInterval = 2;

    // 循环确定截断的范围,范围大小为0~2*iInterval
    while (iMaxIndex > iMidIndex + iInterval)
    {
        if (iWidth >= objFontMetrics.width(strText.left(iMidIndex)))
        {
            iMinIndex = iMidIndex;
        }
        else
        {
            iMaxIndex = iMidIndex - 1;
        }

        iMidIndex = (iMinIndex + iMaxIndex) >> 1;
    }

    // 逆向遍历找出第一个符合的位置
    while (iMinIndex <= iMaxIndex)
    {
        if (iWidth > objFontMetrics.width(strText.left(iMaxIndex)))
        {
            break;
        }

        --iMaxIndex;
    }

    return iMaxIndex;
}

void qDsskeyButton::SetDisplayMode(DSSKEY_DISPLAY_MODE eMode)
{
    if (m_eDisplayMode == eMode)
    {
        return;
    }

    m_eDisplayMode = eMode;
    // 按键大小变化，需要重新创建绘制文本缓存
    CreateTextPiccache(m_pmTextPiccache);

    set3StatePic(TransLongOrNormalPic(PIC_MODE_NORMAL, m_eDisplayMode)
                 , TransLongOrNormalPic(PIC_MODE_HILIGHT, m_eDisplayMode)
                 , TransLongOrNormalPic(PIC_MODE_DISABLE, m_eDisplayMode));

    update();
}

//////////////////////////////////////////////////////////////////////////
//qDsskeySettingButton
//////////////////////////////////////////////////////////////////////////
qDsskeySettingButton::qDsskeySettingButton(int iIndex, QWidget * parent)
    : qDsskeyButton(iIndex, parent)
{
    setObjectName(QString("DsskeySettingButton%1").arg(iIndex));
}

qDsskeySettingButton::~qDsskeySettingButton()
{

}

//更新显示状态
void qDsskeySettingButton::UpdateDisplayStatus(const DsskeyDisplayData & sData)
{
    //m_iIndex = sData.iIndex;
    bool bUpdate = false;

    // 判断是否要刷新整个按键
    if (UpdateContent(sData))
    {
        bUpdate = true;
    }

    DSSKEYUI_DEBUG("m_eGroundPhoto is %d, m_eTextMode is %d.", m_eGroundPhoto, m_eTextMode);

    //  CreateTextPiccache(m_pmTextPiccache);
    if (bUpdate)
    {
        update();
    }

}

void qDsskeySettingButton::paintEvent(QPaintEvent * pe)
{
    QStylePainter p(this);
    QPixmap pm;
    QStyleOptionButton option;
    initStyleOption(&option);
    QPalette pal = option.palette;
    // 设置文字颜色为黑色
    pal.setColor(QPalette::Text, qRgb(0, 0, 0));
    // 设置字体大小
    QFont fontTitle = font();
    fontTitle.setPointSize(DSSKEYBUTTON_SETTING_FONT_SIZE);
    p.setFont(fontTitle);
    // 显示的图标
    QString strDispIcon;
    // 判断button的状态，设置对应的图片
    if (!this->isEnabled())
    {
        pm = THEME_GET_BMP(getDisalbePic().toUtf8().data());
        strDispIcon = m_strCurrIcon;
    }
    else if (this->isDown()
             || this->isChecked())
    {
        pm = THEME_GET_BMP(getEnablePic().toUtf8().data());
        strDispIcon = m_strCurrIcon;
    }
    else
    {
        pm = THEME_GET_BMP(getNormalPic().toUtf8().data());
        strDispIcon = m_strCurrIcon;
    }


    // 画按钮背景图片
    if (!pm.isNull())
    {
        p.drawPixmap(option.rect.x(), option.rect.y()
                     , option.rect.width(), option.rect.height(), pm);

    }

    // 画按钮上的ICON
    QPixmap pmIcon;

    // 判断Icon的对齐方式.目前只有居左、居右、居中三种情况
    bool isIconAlignLeft = (getIconAlign() & Qt::AlignLeft) == Qt::AlignLeft;
    bool isIconAlignRight = (getIconAlign() & Qt::AlignRight) == Qt::AlignRight;
    bool isIconAlignCenter = (getIconAlign() & Qt::AlignCenter) == Qt::AlignCenter;

    QRect rcIcon;
    int x = 0;
    int y = 0;
    if (!strDispIcon.isEmpty())
    {
        //加载图片
        ////---- T48 加载图片的方式需要优化
        // pmIcon.load(getIconPic());
        if (configParser_GetConfigInt(kszEnableUC) == 1
                && configParser_GetConfigInt(kszBroadsoftActive) == 1)
        {
            TMP_GET_BMP(pmIcon, strDispIcon.toUtf8().data());
        }
        else
        {
            pmIcon = THEME_GET_BMP(strDispIcon.toUtf8().data());
        }
        // 通过Icon的对齐方式，计算绘制的x坐标
        if (isIconAlignLeft)
        {
            x = option.rect.x() + getIconLeftOffSet();
        }
        else if (isIconAlignRight)
        {
            x = option.rect.x() + option.rect.width() - pmIcon.width() - getIconLeftOffSet();
        }
        else if (isIconAlignCenter)
        {
            x = option.rect.x() + (option.rect.width() - pmIcon.width()) / 2;
        }

        // Icon的Y坐标默认居中
        x = option.rect.x() + option.rect.width() / 2 - pmIcon.width() / 2;

        // Icon的Y坐标默认居中
        y = option.rect.y() + option.rect.height() / 2 - pmIcon.height() / 2;

        // 判断Icon坐标是否为有效坐标
        if (x >= 0 && x < option.rect.x() + option.rect.width() &&
                y >= 0 && y < option.rect.y() + option.rect.height())
        {
            rcIcon.setRect(x, y, pmIcon.width(), pmIcon.height());

            // 坐标有效绘制Icon
            p.drawPixmap(rcIcon, pmIcon);
#ifdef IF_ENABLE_TESTABILITY
            m_rcIcon = rcIcon;
#endif
        }
        else
        {
            //坐标无效时，缩小图片
            p.drawPixmap(option.rect.x() + 39, option.rect.y() + 10, 25, 25, pmIcon);
#ifdef IF_ENABLE_TESTABILITY
            m_rcIcon.setRect(option.rect.x() + 39, option.rect.y() + 10, 25, 25);
#endif
        }
    }
    // 若是Text为空，就没必要绘制
    QString tx = QString::number(m_iIndex + 1);

    // 画按钮上的文本, 如果已经画了ICON, 则需要进行相应的偏移
    QRect rcText = option.rect;
    QPen penNormal = p.pen();
    if (this->isDown() || this->isChecked())
    {
        penNormal.setColor(qRgb(255, 255, 255));
    }
    else
    {
        penNormal.setColor(qRgb(128, 128, 128));
    }

    // 设置文字在按钮中的偏移
    rcText.setRect(86, 26, this->width() - 86, this->height() - 26);
    p.setPen(penNormal);
    p.drawText(rcText, Qt::AlignLeft | Qt::AlignTop, tx);
#ifdef IF_ENABLE_TESTABILITY
    m_rcIndex = rcText;
    m_strIndex = tx;
#endif
}

#ifdef IF_ENABLE_TESTABILITY
QString qDsskeySettingButton::GetTestInfo()
{
    QString strTestinfo;
    xml_document doc;
    xml_node nodeRoot = doc.append_child("testinfo");

    TestInfo testInfo;
    QPoint ptAbsolute;

    ptAbsolute = mapToGlobal(m_rcIndex.topLeft());
    testInfo.x_absolute = ptAbsolute.x();
    testInfo.y_absolute = ptAbsolute.y();
    testInfo.width = m_rcIndex.width();
    testInfo.height = m_rcIndex.height();
    testInfo.id = LABEL_INDEX;
    testInfo.text = fromQString(m_strIndex);
    testInfo.type = VIRTUAL_LABEL;

    AddNode(nodeRoot, testInfo);

    ptAbsolute = mapToGlobal(m_rcIcon.topLeft());
    testInfo.x_absolute = ptAbsolute.x();
    testInfo.y_absolute = ptAbsolute.y();
    testInfo.width = m_rcIcon.width();
    testInfo.height = m_rcIcon.height();
    testInfo.id = LABEL_ICON;
    testInfo.text = "";
    testInfo.type = VIRTUAL_LABEL;

    AddNode(nodeRoot, testInfo);

    GetStringFromXml(doc, strTestinfo);

    return strTestinfo;
}
#endif

#if IF_BUG_30875
//更新显示
void qDsskeyButton::SetPageDsskeyText(QString strText)
{
    DSSKEYUI_INFO("qDsskeyButton::UpdatePageDsskeyText.");
    setText(strText);
    CreateTextPiccache(m_pmTextPiccache);
}
#endif
