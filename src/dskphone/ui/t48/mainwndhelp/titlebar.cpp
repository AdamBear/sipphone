#include <QtGui>
#include "titlebar.h"
#include "mainwnd/accountmenuselector.h"
#include "account/include/modaccount.h"
#include "baseui/t4xpicpath.h"
#include "account/include/accountmessagedefine.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "idlescreen/include/modidlescreen.h"
#include "commonunits/modcommonunits.h"
#include "imagemanager/modimagemanager.h"
#include "mainwnd/mainwnd.h"
#include "qtcommon/qmisc.h"
#include "setting/include/modsetting.h"
#include "uikernel/languagehelper.h"
#include "uikernel/qdatetimelabel.h"
#include "uikernel/qwidgetutility.h"
#include "uimanager/moduimanager.h"
#include "broadsoft/callcenter/include/ccmessagedefine.h"
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
#include "record/include/recordmessagedefine.h"
#endif
#include "qtcommon/qmisc.h"
#include "qtcommon/xmlhelper.h"
#include "customtheme/modcustomtheme.h"
#include "setting/include/messagedefine.h"

namespace
{
// 帐号位置
#define TITLE_BAR_LINE_LEFT 0
#define TITLE_BAR_LINE_TOP 0
#if IF_FEATURE_STATION_NAME
#define TITLE_BAR_LINE_WIDTH 252
#else
#define TITLE_BAR_LINE_WIDTH 152
#endif
#define TITLE_BAR_LINE_HEIGHT 43
#define TITLE_BAR_LINE_ICON_LEFT 8
#define TITLE_BAR_LINE_ICON_TOP 9
#define TITLE_BAR_LINE_ICON_WIDTH 27
#define TITLE_BAR_LINE_ICON_HEIGHT 25
#define TITLE_BAR_LINE_TEXT_LEFT 41
#define TITLE_BAR_LINE_TEXT_TOP 0

#if IF_FEATURE_STATION_NAME
#define TITLE_BAR_STATION_NAME_LEFT 8
#define TITLE_BAR_STATION_NAME_TOP 0
#define TITLE_BAR_STATION_NAME_WIDTH 252
#define TITLE_BAR_STATION_NAME_HEIGHT 43
#endif

#if IF_BUG_26252 || IF_FEATURE_STATION_NAME
#define TITLE_BAR_LINE_TEXT_WIDTH 182
#else
#define TITLE_BAR_LINE_TEXT_WIDTH 111
#endif

#define TITLE_BAR_LINE_TEXT_HEIGHT 43

// 标题（或StatusBar）位置
#define TITLE_BAR_STATUS_LEFT 0
#define TITLE_BAR_STATUS_TOP 0
#define TITLE_BAR_STATUS_WIDTH 800
#define TITLE_BAR_STATUS_HEIGHT 43

// 时钟位置
#define TITLE_BAR_DATE_TIME_LEFT 568
#define TITLE_BAR_DATE_TIME_TOP 0
#define TITLE_BAR_DATE_TIME_WIDTH 224
#define TITLE_BAR_DATE_TIME_HEIGHT 43
#define TITLE_BAR_DATE_TIME_FONT_SIZE 18

#define TITLE_BAR_STATUS_ICON_AMOUNT 5

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
#define TITLE_BAR_RECORD_ICON_LEFT      350
#define TITLE_BAR_RECORD_ICON_TOP       2
#define TITLE_BAR_RECORD_ICON_WIDTH     42
#define TITLE_BAR_RECORD_ICON_HEIGHT    43

#define TITLE_BAR_RECORD_TIME_LEFT      392
#define TITLE_BAR_RECORD_TIME_TOP       0
#define TITLE_BAR_RECORD_TIME_WIDTH     150
#define TITLE_BAR_RECORD_TIME_HEIGHT    43

// 宽度：PHONE_WIDTH - 2 * TITLE_BAR_DATE_TIME_WIDTH
#define TILTE_BAR_TITLE_RECT QRect(235, 0, 330, 43)
#endif
}

void TitleBar_StatusNotifyCallBackFunction(const YLList<IdleStatusItem> & listStatusList);
LRESULT OnTitleBarMessage(msgObject & refMessage);

// 内部函数定义
void TitleBar_StatusNotifyCallBackFunction(const YLList<IdleStatusItem> & listStatusList)
{
    MAINWND_INFO("TitleBar_StatusNotifyCallBackFunction");
    QWidget * pWgt = _MainWND.GetTitle();
    if (NULL != pWgt
            && pWgt->inherits("CTitleBar"))
    {
        CTitleBar * pTitleBar = static_cast<CTitleBar *>(pWgt);
        pTitleBar->RefreshStatusIcon(listStatusList);
    }
}

LRESULT OnTitleBarMessage(msgObject & refMessage)
{
    QWidget * pWgt = _MainWND.GetTitle();

    if (NULL != pWgt
            && pWgt->inherits("CTitleBar"))
    {
        CTitleBar * pTitleBar = static_cast<CTitleBar *>(pWgt);
        return pTitleBar->OnMessage(refMessage);
    }

    return -1;
}

CTitleBar::CTitleBar(QWidget * parent)
    : QWidget(parent)
    , m_btnLine(this)
    , m_pDateTime(NULL)
    , m_strIcon("")
    , m_iSelectAccountID(-1)
    , m_bAccountUsable(false)
    , m_TitleBarStatusBar(this)
#if IF_FEATURE_STATION_NAME
    , m_pLabScrollStationName(NULL)
    , m_bShowStation(false)
#endif
{
    Init();

    RefreshAccount();
    RefreshDateTimeFormat();

    // 注册Status回调和个数
    idleScreen_SetNotifyFunByCount(TitleBar_StatusNotifyCallBackFunction, TITLE_BAR_STATUS_ICON_AMOUNT);

    // 注册消息
    etl_RegisterMsgHandle(ACCOUNT_DEFAULT_CHANGED, ACCOUNT_DEFAULT_CHANGED, OnTitleBarMessage);
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, OnTitleBarMessage);

    //处理Hoteling消息
    etl_RegisterMsgHandle(HOTELING_MESSAGE_STATUS_CHANGE, HOTELING_MESSAGE_STATUS_CHANGE,
                          OnTitleBarMessage);
    etl_RegisterMsgHandle(LANGUAGE_MESSAGE_LANGUAGE_CHANGED, LANGUAGE_MESSAGE_LANGUAGE_CHANGED,
                          OnTitleBarMessage);
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    etl_RegisterMsgHandle(RECORD_STORAGE_RECORD_RESULT, RECORD_STORAGE_RECORD_RESULT,
                          OnTitleBarMessage);
#endif
}

CTitleBar::~CTitleBar(void)
{
    etl_UnregisterMsgHandle(ACCOUNT_DEFAULT_CHANGED, ACCOUNT_DEFAULT_CHANGED, OnTitleBarMessage);
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, OnTitleBarMessage);
    etl_UnregisterMsgHandle(HOTELING_MESSAGE_STATUS_CHANGE, HOTELING_MESSAGE_STATUS_CHANGE,
                            OnTitleBarMessage);
    etl_UnregisterMsgHandle(LANGUAGE_MESSAGE_LANGUAGE_CHANGED, LANGUAGE_MESSAGE_LANGUAGE_CHANGED,
                            OnTitleBarMessage);
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    etl_UnregisterMsgHandle(RECORD_STORAGE_RECORD_RESULT, RECORD_STORAGE_RECORD_RESULT,
                            OnTitleBarMessage);
#endif
}

void CTitleBar::Init()
{
    // 设置背景
#if IF_BUG_26252
    SetBackgroundImage(PIC_THEME1_TITLEBAR_WITHOUT_ACCOUNT);
#else
    SetBackgroundImage(PIC_THEME1_TITLELITTLE);
#endif
    qWidgetUtility::setFontStyle(this,
                                 CustomTheme_GetCustomFontSizeByKey(CUSTOM_FONT_ID_TITLEBAR_NORMAL));

    // 设置palette样式
    QPalette pal = palette();

    pal.setColor(QPalette::Normal, QPalette::WindowText, Qt::white);
    pal.setColor(QPalette::Active, QPalette::WindowText, Qt::white);
    pal.setColor(QPalette::Disabled, QPalette::WindowText, Qt::white);
    pal.setColor(QPalette::Inactive, QPalette::WindowText, Qt::white);

    setPalette(pal);

    m_btnLine.SetIconRect(TITLE_BAR_LINE_ICON_LEFT, TITLE_BAR_LINE_ICON_TOP,
                          TITLE_BAR_LINE_ICON_WIDTH, TITLE_BAR_LINE_ICON_HEIGHT);

    m_btnLine.SetTextRect(TITLE_BAR_LINE_TEXT_LEFT, TITLE_BAR_LINE_TEXT_TOP,
                          TITLE_BAR_LINE_TEXT_WIDTH, TITLE_BAR_LINE_TEXT_HEIGHT);

    m_btnLine.SetRect(TITLE_BAR_LINE_LEFT, TITLE_BAR_LINE_TOP, TITLE_BAR_LINE_WIDTH,
                      TITLE_BAR_LINE_HEIGHT);
#if IF_BUG_26252
    m_btnLine.SetTextScroll(true);
#endif

#if IF_FEATURE_STATION_NAME
    if (NULL == m_pLabScrollStationName)
    {
        m_pLabScrollStationName = new qScrollTextLabel(this);
    }

    if (m_pLabScrollStationName != NULL)
    {
        m_pLabScrollStationName->setGeometry(TITLE_BAR_STATION_NAME_LEFT, TITLE_BAR_STATION_NAME_TOP,
                                             TITLE_BAR_STATION_NAME_WIDTH, TITLE_BAR_STATION_NAME_HEIGHT);
        RefreshStationName();
    }
#endif

    QRect rcStatusBar(TITLE_BAR_STATUS_LEFT, TITLE_BAR_STATUS_TOP,
                      TITLE_BAR_STATUS_WIDTH, TITLE_BAR_STATUS_HEIGHT);

    m_TitleBarStatusBar.SetGeometry(rcStatusBar);

    m_TitleBarStatusBar.RefreshStatusIcon();

    m_pDateTime = new qDateTimeLabel(this);
    m_pDateTime->setObjectName("DateTimeLabel");

    if (NULL == m_pDateTime)
    {
        return;
    }

    m_pDateTime->setGeometry(TITLE_BAR_DATE_TIME_LEFT, TITLE_BAR_DATE_TIME_TOP,
                             TITLE_BAR_DATE_TIME_WIDTH, TITLE_BAR_DATE_TIME_HEIGHT);


    qWidgetUtility::setFontStyle(m_pDateTime, TITLE_BAR_DATE_TIME_FONT_SIZE);
    m_pDateTime->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    CustomTheme_SetWidgetFontProperty(m_pDateTime, CUSTOM_FONT_ID_TITLEBAR_TIME);

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    m_RecordIcon.setParent(this);
    m_RecordIcon.setObjectName("RecordIcon");
    m_RecordIcon.setGeometry(TITLE_BAR_RECORD_ICON_LEFT, TITLE_BAR_RECORD_ICON_TOP,
                             TITLE_BAR_RECORD_ICON_WIDTH, TITLE_BAR_RECORD_ICON_HEIGHT);
    m_RecordIcon.setPixmap(THEME_GET_BMP(PIC_ICON_TITLEBAR_RECORD_START));

    m_RecordTime.setParent(this);
    m_RecordTime.setObjectName("RecordTime");
    m_RecordTime.setGeometry(TITLE_BAR_RECORD_TIME_LEFT, TITLE_BAR_RECORD_TIME_TOP,
                             TITLE_BAR_RECORD_TIME_WIDTH, TITLE_BAR_RECORD_TIME_HEIGHT);
    m_RecordTime.setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    //m_RecordTime.setPalette(pal);
    CustomTheme_SetWidgetFontProperty(&m_RecordTime, CUSTOM_FONT_ID_TITLEBAR_NORMAL);
    m_RecordTime.StopTimer();
    StopUSBRecord();
#endif

    setObjectName("CTitleBar");
}

void CTitleBar::SetBackgroundImage(const yl::string & strImg)
{
    m_strBGImg = strImg;
}

void CTitleBar::SetLayoutInfo(const TitleLayoutInfo & objTitleInfo)
{
    if (m_titleInfo != objTitleInfo)
    {
        CheckShowStatusIcon(objTitleInfo.eMidType);

        bool bTypeChange = (objTitleInfo.eLeftType != m_titleInfo.eLeftType);
        m_titleInfo = objTitleInfo;

        // 从其他状态切换为账号状态需要重新刷新数据
        if (m_titleInfo.eLeftType == TITLE_MODE_ACCOUNT_INFO
                && bTypeChange)
        {
            m_iSelectAccountID = -1; // 选中默认账号
            RefreshAccount();
        }
        else if (m_titleInfo.eLeftType == TITLE_MODE_TALK_INFO)
        {
#if IF_FEATURE_STATION_NAME
            if (!m_bShowStation)
            {
                m_strLeftAreaText = "";
            }
#else
            m_strLeftAreaText = "";
#endif
        }

        update();
    }
}

void CTitleBar::SetMidAreaType(TITLE_AREA_MODE eType)
{
    if (m_titleInfo.eMidType != eType)
    {
        CheckShowStatusIcon(eType);

        m_titleInfo.eMidType = eType;
        update(TITLE_BAR_STATUS_LEFT, TITLE_BAR_STATUS_TOP, TITLE_BAR_STATUS_WIDTH,
               TITLE_BAR_STATUS_HEIGHT);
    }
}

void CTitleBar::SetAccountDisplayName(const QString & strText)
{
    if (m_titleInfo.eLeftType == TITLE_MODE_ACCOUNT_INFO)
    {
        return;
    }

#if IF_FEATURE_STATION_NAME
    if (m_bShowStation)
    {
        return;
    }
#endif

    m_strLeftAreaText = strText;
    update();
}

void CTitleBar::SetAccountIcon(const QString & strIcon)
{
    // 非自定义模式
    if (m_titleInfo.eLeftType == TITLE_MODE_ACCOUNT_INFO)
    {
        return;
    }

    m_strIcon = strIcon;
    update();
}

void CTitleBar::SetSelectAccountID(int nAccountID)
{
    m_iSelectAccountID = nAccountID;
}

void CTitleBar::SetTitle(const QString & strTitle)
{
    m_strTitle = strTitle;
}

QString CTitleBar::GetTitle() const
{
    return m_strTitle;
}

void CTitleBar::RefreshAccount()
{
    if (m_titleInfo.eLeftType != TITLE_MODE_ACCOUNT_INFO
            && m_titleInfo.eLeftType != TITLE_MODE_CALL_MANAGER)
    {
        return;
    }

#if IF_FEATURE_SHOW_DEFAULT_ACCOUNT
    if (configParser_GetConfigInt(kszEnableIdleDefaultAccount) == 0)
    {
        return;
    }
#endif

    int nDefAccountId = acc_GetDefaultAccount();

    m_bAccountUsable = (-1 != nDefAccountId && !acc_IsDirectIPAccount(nDefAccountId));
#if IF_BUG_25367
    yl::string strAccountText = acc_GetAccountShowText(nDefAccountId, true).c_str();
    commonUnits_FormatNumber(strAccountText);

    SetLeftText(m_bAccountUsable ?  toQString(strAccountText.c_str()) : LANG_TRANSLATE(
                    TRID_NO_SERVICE));
#else

#if IF_FEATURE_32591
    yl::string strLabel = acc_GetAccountStatusShowText(nDefAccountId);
#else
    yl::string strLabel = acc_GetAccountShowText(nDefAccountId);
#endif

    SetLeftText(m_bAccountUsable ?
                toQString(strLabel) : LANG_TRANSLATE(TRID_NO_SERVICE));
#endif
}

void CTitleBar::SetLeftText(const QString & strText)
{
#if IF_FEATURE_STATION_NAME
    if (m_bShowStation)
    {
        return;
    }
#endif

    m_strLeftAreaText = strText;

    if (isVisible())
    {
        update();
    }
}

void CTitleBar::RefreshStatusIcon(const YLList<IdleStatusItem> & listStatusItem)
{
    m_TitleBarStatusBar.RefreshStatusIcon(listStatusItem);
}

void CTitleBar::RefreshDateTimeFormat()
{
    if (NULL == m_pDateTime)
    {
        return;
    }

    //直接调用C接口
    m_pDateTime->SetTimeFmtType(Setting_GetCurDateFormatType(), Setting_GetCurTimeFormatType());
}

void CTitleBar::CheckShowStatusIcon(int nModeType)
{
    if (TITLE_MODE_STATUS_BAR == nModeType)
    {
        m_TitleBarStatusBar.show();
    }
    else
    {
        m_TitleBarStatusBar.hide();
    }
}

LRESULT CTitleBar::OnMessage(msgObject & refMessage)
{
    switch (refMessage.message)
    {
    case ACCOUNT_DEFAULT_CHANGED:
        {
            MAINWND_DEBUG("CTitleBar received ACCOUNT_DEFAULT_CHANGED message");
            RefreshAccount();
        }
        break;
    case HOTELING_MESSAGE_STATUS_CHANGE:
        {
            MAINWND_DEBUG("CTitleBar received HOTELING_MESSAGE_STATUS_CHANGE message");
            // Hoteling登录/登出需要变更默认账号的显示
            if (1 == refMessage.lParam)
            {
                int nDefAccountId = acc_GetDefaultAccount();
                if (refMessage.wParam == nDefAccountId)
                {
                    SetLeftText(toQString(acc_GetCustomShowText(nDefAccountId).c_str()));
                }
            }
            else
            {
                RefreshAccount();
            }
        }
        break;
    case CONFIG_MSG_BCAST_CHANGED:
        {
            if (ST_SETTIME == refMessage.wParam || ST_LANG == refMessage.wParam)
            {
                MAINWND_DEBUG("CTitleBar received CONFIG_MSG_BCAST_CHANGED:ST_SETTIME message");
                RefreshDateTimeFormat();
            }
#if IF_FEATURE_STATION_NAME
            else if (ST_STATION_NAME == refMessage.wParam)
            {
                RefreshStationName();
            }
#endif
#if IF_FEATURE_SHOW_DEFAULT_ACCOUNT
            else if (ST_DEFAULT_ACCOUNT == refMessage.wParam)
            {
                UIManager_UpdateTopWnd();
            }
#endif
        }
        break;
    case LANGUAGE_MESSAGE_LANGUAGE_CHANGED:
        {
            MAINWND_DEBUG("CTitleBar received LANGUAGE_MESSAGE_LANGUAGE_CHANGED message");
            RefreshDateTimeFormat();
            RefreshAccount();
        }
        break;
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    case RECORD_STORAGE_RECORD_RESULT:
        {
            MAINWND_INFO("CTitleBar received RECORD_STORAGE_RECORD_RESULT [%d] [%d]", refMessage.wParam,
                         refMessage.lParam);
            if (refMessage.wParam == 0
                    && refMessage.lParam == 1)
            {
                StartUSBRecord();
            }
            else if (refMessage.wParam == 0
                     && refMessage.lParam == 0)
            {
                StopUSBRecord();
            }
            else if (refMessage.wParam == 1
                     && refMessage.lParam != 0)
            {
                StopUSBRecord();
            }
            else if (refMessage.wParam == 3
                     && refMessage.lParam == 1)
            {
                PauseUSBRecord();
            }
            else if (refMessage.wParam == 4
                     && refMessage.lParam == 1)
            {
                StartUSBRecord();
            }
        }
        break;
#endif
    default:
        break;
    }

    return 0;
}

void CTitleBar::mousePressEvent(QMouseEvent * pEvent)
{
    if (m_TitleBarStatusBar.GetMousePressBtnId(pEvent) > 0)
    {
        m_btnLine.ResetDown();
        return;
    }

    m_btnLine.ProcessMousePress(pEvent);
}

void CTitleBar::mouseReleaseEvent(QMouseEvent * pEvent)
{
    int nBtnId = m_TitleBarStatusBar.GetMouseReleaseBtnId(pEvent);

    if (nBtnId > 0)
    {
        OnStatusBtnClick(nBtnId);
    }
    else if (m_btnLine.ProcessMouseRelease(pEvent))
    {
        OnLineBtnClick();
    }
}

void CTitleBar::OnLineBtnClick()
{
#if IF_FEATURE_BLOCK_LINE
    if (configParser_GetConfigInt(kszBlockLine) == 1)
    {
        return;
    }
#endif

#if IF_FEATURE_STATION_NAME
    if (m_pLabScrollStationName != NULL
            && m_pLabScrollStationName->isVisible())
    {
        return;
    }
#endif

#if IF_FEATURE_SHOW_DEFAULT_ACCOUNT
    if (configParser_GetConfigInt(kszEnableIdleDefaultAccount) == 0
            && m_titleInfo.eLeftType == TITLE_MODE_ACCOUNT_INFO)
    {
        return;
    }
#endif

    // 账号界面不需要响应点击效果
    if (m_titleInfo.eLeftType == TITLE_MODE_TALK_INFO)
    {
        return;
    }

    m_accountSelector.Init();

    // 获取选中账号的ID
    int nCurAccountId = -1;

    if (-1 == m_iSelectAccountID)
    {
        nCurAccountId = acc_GetDefaultAccount();
    }
    else
    {
        nCurAccountId = m_iSelectAccountID;
    }

    if (-1 == nCurAccountId)
    {
        return;
    }

    // 设置Index
    m_accountSelector.SetSelectIndexByAccountID(nCurAccountId);

    if (m_accountSelector.GetAccountSize() <= 0)
    {
        return;
    }

    QPoint ptMenu(0, 0);

    MessageBox_ShowTitleMenuBox(this, &m_accountSelector, ptMenu);
}

void CTitleBar::OnStatusBtnClick(int nStatusBtnId)
{
    if (!UIManager_IsInIdle())
    {
        return;
    }

    idleScreen_SetPopupStatus(nStatusBtnId, POPUP_WINDOW_POPUP);
    idleScreen_TurnStatusNotify(true);
}

void CTitleBar::paintEvent(QPaintEvent * e)
{
    QStylePainter painter(this);

#ifndef _T48
    QStyleOptionFocusRect option;

    option.initFrom(this);
    option.backgroundColor = palette().color(QPalette::Background);

    // 背景
    QPixmap * pPix = GetThemeManager().GetImage(m_strBGImg.c_str());

    if (NULL == pPix || pPix->isNull())
    {
        //从默认图片中查找
        pPix = GetImageManager().GetImage(m_strBGImg.c_str());
    }

    if (NULL != pPix && !pPix->isNull())
    {
        painter.drawPixmap(0, 0, width(), height(), *pPix);
    }
#endif

    if (m_titleInfo.eLeftType == TITLE_MODE_ACCOUNT_INFO)
    {
#if IF_FEATURE_STATION_NAME
        if (m_pLabScrollStationName != NULL
                && !m_pLabScrollStationName->text().isEmpty())
        {
            m_btnLine.PaintHButton(painter, "", "", "");
            m_pLabScrollStationName->show();
        }
        else
        {
#if IF_FEATURE_SHOW_DEFAULT_ACCOUNT
            if (configParser_GetConfigInt(kszEnableIdleDefaultAccount) == 0)
            {
                m_btnLine.PaintHButton(painter, "", "", "");
            }
            else
#endif
            {
                m_btnLine.PaintHButton(painter, "",
                                       m_bAccountUsable ? PIC_DSS_LINE_REGISTERED : PIC_DSS_DOWN_LINE_REGISTERED,
                                       m_strLeftAreaText);
            }

            m_pLabScrollStationName->hide();
        }
#else
#if IF_FEATURE_SHOW_DEFAULT_ACCOUNT
        if (configParser_GetConfigInt(kszEnableIdleDefaultAccount) == 0)
        {
            m_btnLine.PaintHButton(painter, "", "", "");
        }
        else
#endif
        {
            m_btnLine.PaintHButton(painter, "",
                                   m_bAccountUsable ? PIC_DSS_LINE_REGISTERED : PIC_DSS_DOWN_LINE_REGISTERED,
                                   m_strLeftAreaText);
        }
#endif
    }
    else
    {
        m_btnLine.PaintHButton(painter, "", m_strIcon.toUtf8().data(), m_strLeftAreaText);
#if IF_FEATURE_STATION_NAME
        if (m_pLabScrollStationName != NULL)
        {
            m_pLabScrollStationName->hide();
        }
#endif
    }

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    m_RecordTime.hide();
    m_RecordIcon.hide();
#endif

    if (m_titleInfo.eMidType == TITLE_MODE_STATUS_BAR)
    {
        // 由CStatusIconsBar绘制
    }
    else if (m_titleInfo.eMidType == TITLE_MODE_TITLE)
    {
        painter.drawItemText(TILTE_BAR_TITLE_RECT, Qt::AlignCenter, palette(), isEnabled(), m_strTitle);
    }
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    else if (m_titleInfo.eMidType == TITLE_MODE_USB_RECORDING)
    {
        if (m_RecordTime.IsTimerStart()
                || m_RecordTime.IsTimerPause())
        {
            m_RecordTime.show();
            m_RecordIcon.show();
        }
    }
#endif
    else
    {
        chWARNING2(FALSE, "Title Mid Type Error!");
    }
}


void CTitleBar::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    int nAccountId = m_accountSelector.GetAccountId();

    if (-1 != nAccountId)
    {
        emit SelectAccount(nAccountId);
    }
}

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
void CTitleBar::StartUSBRecord()
{
    if (m_RecordTime.IsTimerStart())
    {
        m_RecordTime.PauseTimer();
    }
    m_RecordTime.StartTimer();
    m_RecordIcon.setPixmap(THEME_GET_BMP(PIC_ICON_TITLEBAR_RECORD_START));
}

void CTitleBar::PauseUSBRecord()
{
    m_RecordTime.PauseTimer();
    m_RecordIcon.setPixmap(THEME_GET_BMP(PIC_ICON_TITLEBAR_RECORD_PAUSE));
}

void CTitleBar::StopUSBRecord()
{
    m_RecordTime.StopTimer();
}
#endif

#if IF_FEATURE_STATION_NAME
// 刷新station name
void CTitleBar::RefreshStationName()
{
    if (NULL == m_pLabScrollStationName)
    {
        return;
    }

    yl::string strStationName = configParser_GetConfigString(kszStationName).c_str();
    m_pLabScrollStationName->setText(toQString(strStationName));
    bool bScroll = 0 != configParser_GetConfigInt(kszStationNameScrolling);
    m_pLabScrollStationName->setScroll(bScroll);

    if (!strStationName.empty())
    {
        m_bShowStation = true;
        m_strLeftAreaText = toQString(strStationName);
#if IF_BUG_26252
        m_btnLine.SetTextScroll(bScroll);
#endif
    }
    else
    {
        m_bShowStation = false;
#if IF_BUG_26252
        m_btnLine.SetTextScroll(true);
#endif
    }
    RefreshAccount();

    if (isVisible())
    {
        update();
    }
}
#endif

#ifdef IF_ENABLE_TESTABILITY
QString CTitleBar::GetTestInfo()
{
    xml_document doc;
    xml_node nodeRoot = doc.append_child("testinfo");

    TestInfo testInfo;
    QPoint ptAbsolute;

    // title
    ptAbsolute = mapToGlobal(TILTE_BAR_TITLE_RECT.topLeft());
    testInfo.x_absolute = ptAbsolute.x();
    testInfo.y_absolute = ptAbsolute.y();
    testInfo.width = TILTE_BAR_TITLE_RECT.width();
    testInfo.height = TILTE_BAR_TITLE_RECT.height();
    testInfo.id = LABEL_TITLE;
    testInfo.text = fromQString(m_strTitle);
    testInfo.type = VIRTUAL_LABEL;
    AddNode(nodeRoot, testInfo);

    AddBtnNode(nodeRoot, m_btnLine, BTN_LINE, fromQString(m_strLeftAreaText));

    QString strTestinfo;
    GetStringFromXml(doc, strTestinfo);
    return strTestinfo;
}
#endif
