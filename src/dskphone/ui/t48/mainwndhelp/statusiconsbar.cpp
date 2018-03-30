#include "statusiconsbar.h"

#include "baseui/t4xpicpath.h"
#include "qtcommon/qmisc.h"

#include "idlescreen/include/modidlescreen.h"

namespace
{
#define TITLE_STATUS_BUTTON_WIDTH       42
#define TITLE_STATUS_BUTTON_HEIGHT      43
#define TITLE_STATUS_BUTTON_FONT_SIZE   12

#define TITLE_BAR_STATUS_ICON_AMOUNT    5
}


CStatusIconsBar::CStatusIconsBar(QWidget * parent)
    : QWidget(parent)
    , m_barStatus(this, true)
    , m_bIsAllowTitleStatusClick(true)
{
    Init();
}

CStatusIconsBar::~CStatusIconsBar()
{
}

void CStatusIconsBar::SetGeometry(QRect rect)
{
    setGeometry(rect);

    m_barStatus.SetRect(this->rect());
}

void CStatusIconsBar::Init()
{
    setAttribute(Qt::WA_AcceptTouchEvents, true);
    setObjectName("CStatusIconsBar");
    m_barStatus.SetBtnShowCount(0);
}

void CStatusIconsBar::RefreshStatusIcon()
{
    RefreshStatusIcon(idleScreen_GetStatusList());
}

void CStatusIconsBar::RefreshStatusIcon(YLList<IdleStatusItem> listStatusItem)
{
    int nSize = listStatusItem.size();

    if (nSize < 0)
    {
        return;
    }

    MAINWND_INFO("CStatusIconsBar::RefreshStatusIcon IdleStatusItem Size[%d]", nSize);

    ICON_BUTTON_LIST StatusIconHintList;
    StatusIconHintList.clear();

    for (YLList<IdleStatusItem>::ListIterator iter = listStatusItem.begin();
            iter != listStatusItem.end(); ++ iter)
    {
        const IdleStatusItem& StatusItem = *iter;

        vecIconString vecstrIcon;
        vecstrIcon.clear();

        yl::string strTip = "";

        if (!GetStatusShowInfo(StatusItem, vecstrIcon, strTip))
        {
            MAINWND_INFO("GetStatusShowInfo() fail State=%d", StatusItem.m_nId);
            continue;
        }

        ICON_BUTTON pIconBtn;
        pIconBtn.m_nId = StatusItem.m_nId;
        pIconBtn.m_vecstrIcon = vecstrIcon;
        pIconBtn.m_strTip = strTip;
        pIconBtn.m_bFlash = JudgeIsFlash(StatusItem.m_nId, strTip);

        StatusIconHintList.push_back(pIconBtn);
    }

    RefreshStatusIcon(StatusIconHintList);
}

void CStatusIconsBar::RefreshStatusIcon(ICON_BUTTON_LIST & StatusIconHintList)
{
    int nSize = StatusIconHintList.size();

    if (nSize < 0)
    {
        return;
    }

    MAINWND_INFO("CStatusIconsBar::RefreshStatusIcon ICON_BUTTON_LIST Size[%d]", nSize);

    //设置需要显示的图标数量，预留图标指针空间
    m_barStatus.SetBtnCount(TITLE_BAR_STATUS_ICON_AMOUNT);

    int nShowIndex = 0;
    YLList<ICON_BUTTON>::ListIterator iter = StatusIconHintList.begin();
    for (; iter != StatusIconHintList.end(); ++iter)
    {
        ICON_BUTTON & IconButton = (*iter);

        CStatusButton * pBtnStatus = new CStatusButton(this);
        if (NULL == pBtnStatus)
        {
            continue;
        }

        pBtnStatus->SetRect(0, 0, TITLE_STATUS_BUTTON_WIDTH, TITLE_STATUS_BUTTON_HEIGHT);
        pBtnStatus->SetStatus(IconButton.m_nId, IconButton.m_vecstrIcon, IconButton.m_strTip);

        if (IconButton.m_bFlash)
        {
            connect(pBtnStatus, SIGNAL(NeedRepaint()), this, SLOT(RepainStatusBar()));
            pBtnStatus->StartFlash();
        }

        if (!m_barStatus.SetBtn(nShowIndex, pBtnStatus))
        {
            MAINWND_ERR("RefreshStatusIcon Add Button Fail, state=%d", IconButton.m_nId);
            delete pBtnStatus;
        }
        ++nShowIndex;

        //超出部分不显示
        if (nShowIndex >= TITLE_BAR_STATUS_ICON_AMOUNT)
        {
            break;
        }
    }

    m_barStatus.SetBtnShowCount(nShowIndex);

    m_barStatus.ReLayoutButton();

    if (isVisible())
    {
        update();
    }
}

bool CStatusIconsBar::JudgeIsFlash(int nIconID, yl::string & strTip)
{
    if (PS_STATE_USBDEVICE_DETECTING == nIconID
            || PS_STATE_WIFI_CONNECTTING == nIconID
            || PS_STATE_BLUETOOTH_CONNECTING == nIconID)
    {
        return true;
    }

    return false;
}

void CStatusIconsBar::RepainStatusBar()
{
    update();
}

int CStatusIconsBar::GetMousePressBtnId(QMouseEvent * pEvent)
{
    CStatusButton * pBtn = (CStatusButton *)m_barStatus.ProcessMousePress(pEvent);

    return NULL != pBtn ? pBtn->GetId() : -1;
}

int CStatusIconsBar::GetMouseReleaseBtnId(QMouseEvent * pEvent)
{
    CStatusButton * pBtn = (CStatusButton *)m_barStatus.ProcessMouseRelease(pEvent);

    return NULL != pBtn ? pBtn->GetId() : -1;
}

bool CStatusIconsBar::GetStatusShowInfo(const IdleStatusItem & stateData,
                                        vecIconString & vecstrIcon, yl::string & strTip)
{
    MAINWND_INFO("Get StatusShowInfo Id[%d] On CStatusIconsBar.", stateData.m_nId);

    bool bResult = false;
    switch (stateData.m_nId)
    {
    case PS_STATE_IDLE:
        {
            vecstrIcon.push_back(PIC_ICON_CALL_IN);
            bResult = true;
        }
        break;
    case PS_STATE_DND:
        {
            vecstrIcon.push_back(PIC_ICON_DND);
            bResult = true;
        }
        break;
    case PS_STATE_AA:
        {
            vecstrIcon.push_back(PIC_ICON_AUTOANSWER);
            bResult = true;
        }
        break;
    case PS_STATE_FWD:
        {
            vecstrIcon.push_back(PIC_ICON_FWD_ICON);
            bResult = true;
        }
        break;
    case PS_STATE_LOCK:
        {
            vecstrIcon.push_back(PIC_ICON_LOCK);
            bResult = true;
        }
        break;
    case PS_STATE_TALK_MUTE:
        {
            vecstrIcon.push_back(PIC_ICON_IDLE_MUTE);
            bResult = true;
        }
        break;
    case PS_STATE_MUTE:
        {
            vecstrIcon.push_back(PIC_ICON_RINGMUTE);
            bResult = true;
        }
        break;
    case PS_STATE_RING_MUTE:
        {
            vecstrIcon.push_back(PIC_ICON_RINGMUTE);
            bResult = true;
        }
        break;
    case PS_STATE_VPN:
        {
            if (!idleScreen_IsNetworkAvailable())
            {
                bResult = false;
            }
            else
            {
                vecstrIcon.push_back(PIC_ICON_VPN);
                bResult = true;
            }
        }
        break;
    case PS_STATE_NETWORK_FAILED:
        {
            vecstrIcon.push_back(PIC_ICON_NET2);
            bResult = true;
        }
        break;
    case PS_STATE_MISSCALL:
        {
            vecstrIcon.push_back(PIC_ICON_MISSCALL_ICON);
            strTip = stateData.m_strHint;
            bResult = true;
        }
        break;
    case PS_STATE_VOICEMAIL:
        {
            vecstrIcon.push_back(PIC_ICON_VOICEMAIL);
            strTip = stateData.m_strHint;
            bResult = true;
        }
        break;
    case PS_STATE_TEXTMESSAGE:
        {
            vecstrIcon.push_back(PIC_ICON_TEXTMESSAGE);
            strTip = stateData.m_strHint;
            bResult = true;
        }
        break;
    case PS_STATE_CALLFORWARD:
        {
            vecstrIcon.push_back(PIC_ICON_FWD_ICON);
            strTip = stateData.m_strHint;
            bResult = false;
        }
        break;
    case PS_STATE_REFLASH_STATE:
        {
            vecstrIcon.push_back(PIC_ICON_MISSCALL_ICON);
            bResult = true;
        }
        break;
    case PS_STATE_HEADSET:
        {
            vecstrIcon.push_back(PIC_ICON_HEADSET);
            bResult = true;
        }
        break;
    case PS_STATE_SWITCH_ACCOUNT:
        {
            vecstrIcon.push_back(PIC_ICON_SWITCH_ACCOUNT);
            bResult = false;
        }
        break;
    case PS_STATE_BLUETOOTH_MOBILECONNECT:
        {
            vecstrIcon.push_back(PIC_ICON_BLUE_TOOTH_MOBILE_CONNECT);
            bResult = true;
        }
        break;
    case PS_STATE_BLUETOOTH:
        {
            vecstrIcon.push_back(PIC_ICON_BLUE_TOOTH_TYPE_COMMON);
            bResult = true;
        }
        break;
    case PS_STATE_BLUETOOTH_CONNECT:
    case PS_STATE_BLUETOOTH_CONNECTING:
        {
            vecstrIcon.push_back(PIC_ICON_BLUE_TOOTH_CONNECT);
            bResult = true;
        }
        break;
    case PS_STATE_WIFI:
        {
            vecstrIcon.push_back(PIC_ICON_WIFI_TOPBAR_ON);
            bResult = true;
        }
        break;
    case PS_STATE_WIFI_CONNECT:
        {
            vecstrIcon.push_back(PIC_ICON_WIFI_TOPBAR_CONNECT);
            bResult = true;
        }
        break;
    case PS_STATE_WIFI_CONNECTTING:
        {
            vecstrIcon.push_back(PIC_ICON_WIFI_TOPBAR_CONNECT);
            bResult = true;
        }
        break;
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    //连接中和连接上都是同一个图标，不同在于闪烁
    case PS_STATE_USBDEVICE:
    case PS_STATE_USBDEVICE_DETECTING:
        {
            vecstrIcon.push_back(PIC_ICON_TITLEBAR_USB);
            bResult = true;
        }
        break;
    case PS_STATE_USBDEVICE_DETECTFAILED:
        {
            bResult = false;
        }
        break;
#endif
    default:
        break;
    }

    return bResult;
}

void CStatusIconsBar::paintEvent(QPaintEvent * e)
{
    QStylePainter painter(this);

    QFont fontPainter = painter.font();
    fontPainter.setPointSize(TITLE_STATUS_BUTTON_FONT_SIZE);
    painter.setFont(fontPainter);

    const char * pszBtnClick = NULL;

    if (m_bIsAllowTitleStatusClick)
    {
        pszBtnClick = PIC_IDLE_SRCEEN_STATUS_BUTTON_CLICK;
    }

    m_barStatus.PaintStatusBtnBar(painter, pszBtnClick);
}

void CStatusIconsBar::SetIconAlign(int nAlign)
{
    m_barStatus.SetAlign(nAlign);
}

#ifdef IF_ENABLE_TESTABILITY

QString CStatusIconsBar::GetTestInfo()
{
    return m_barStatus.GetTestInfo();
}

#endif
