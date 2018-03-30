#include "dlgzeronetwork.h"
#include "dlgzeronetworkipmode.h"
#include "cdlgzeronetwork_vlan.h"
#include "dlgzeronetwork_vlandhcp.h"
#include "cdlgzeronetworkx_mode.h"
#include "cdlgzeronetwork_vpn.h"
#include "dlgzeronetwork_lldp.h"
#include "dlgzeronetwork_cdp.h"
#include "dlgzeronetwork_nat.h"
#include "uikernel/languagehelper.h"
#include "mainwnd/mainwnd.h"
#include "messagebox/modmessagebox.h"
#include "uimanager/rectdefine.h"
#include "settingui/src/networksettingmanager.h"
#include "include/uicommon.h"
#include "inputmethod/inputmethod.h"
#include "baseui/t4xpicpath.h"
#include "feature/include/modfeature.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"

#include "dsklog/log.h"

namespace
{
#define NEW_ZERO_NETWORK
//#define SECOND_CREATE
#define ZERO_IP_MODE        "zeroipmode"
#define ZERO_VLAN           "zerovlanactive"
#define ZERO_X_MODE         "zero8021xmode"
#define ZERO_VPN            "zerovpnactive"
#define ZERO_VLANDHCP       "zerovlandhcp"
#define ZERO_LLDP           "zerolldpstatus"
#define ZERO_CDP            "zerocdpstatus"
#define ZERO_NAT            "zeronatstatus"
}

CDlgZeroNetwork::CDlgZeroNetwork(QWidget * parent /*= NULL*/)
    : CDlgBaseSubPage(parent)
    , m_pDlgIPMode(NULL)
    , m_pDlgVLAN(NULL)
    , m_pDlgX_Mode(NULL)
    , m_pDlgVPN(NULL)
    , m_pDlgVlanDHCP(NULL)
    , m_pDlgNAT(NULL)
    , m_pDlgCDP(NULL)
    , m_pDlgLLDP(NULL)
    , m_iFocusLine(-1)
{
    this->setFixedSize(RECT_ZERO.width() - 200, RECT_ZERO.height());
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::SubWindow);

    m_pDlgIPMode = new CDlgZeroNetworkIPMode(this);
    m_pDlgVLAN = new CDlgZeroNetwork_VLAN(this);
    m_pDlgX_Mode = new CDlgZeroNetworkX_Mode(this);
    m_pDlgVPN = new CDlgZeroNetwork_VPN(this);
    m_pDlgVlanDHCP = new DlgZeroNetwork_VLANDHCP(this);
    m_pDlgNAT = new CDlgZeroNetwork_NAT(this);
    m_pDlgLLDP = new CDlgZeroNetwork_LLDP(this);
    m_pDlgCDP = new CDlgZeroNetwork_CDP(this);

    if (JudgetWgtNull() || NULL == m_pGeneralFrame)
    {
        return;
    }

    m_pGeneralFrame->setGeometry(0, 0, width(), height());
#ifdef NEW_ZERO_NETWORK
    m_pZeroListDelegate = new ZeroListDelegate();
    if (NULL != m_pZeroListDelegate)
    {
        m_pZeroListDelegate->setParent(this);
        m_pZeroListDelegate->SetItemPat(this);
        m_pZeroListDelegate->BindFramelist(m_pGeneralFrame);
        m_pZeroListDelegate->InitSettingListStyle();
        m_pZeroListDelegate->SetItemParent(this);
        m_pZeroListDelegate->PreCreateItem();
        m_pListDelegate = m_pZeroListDelegate;

        connect(m_pZeroListDelegate, SIGNAL(listItemAction(int, QString)), this, SLOT(OnListItemAction(int,
                QString)));
        connect(m_pListDelegate, SIGNAL(listItemAction(int, QString)), this, SLOT(OnListItemAction(int,
                QString)));
    }
#endif
#ifdef SECOND_CREATE
    m_pZeroListDelegate = new ZeroListDelegate(m_pGeneralFrame);
    if (NULL != m_pZeroListDelegate)
    {
        m_pZeroListDelegate->setParent(this);
        m_pZeroListDelegate->BindFramelist(m_pGeneralFrame);
        m_pZeroListDelegate->SetItemPat(this);
        //m_pZeroListDelegate->PreCreateItem();
        m_pListDelegate = m_pZeroListDelegate;
        m_pListDelegate->InitSettingListStyle();
        m_pZeroListDelegate->SetItemParent(this);
    }
#endif

    connect(m_pDlgIPMode, SIGNAL(RefreshSoftkey()), this, SLOT(OnRefreshSoftkey()));
    connect(m_pDlgVLAN, SIGNAL(RefreshSoftkey()), this, SLOT(OnRefreshSoftkey()));
    connect(m_pDlgVlanDHCP, SIGNAL(RefreshSoftkey()), this, SLOT(OnRefreshSoftkey()));
    connect(m_pDlgX_Mode, SIGNAL(RefreshSoftkey()), this, SLOT(OnRefreshSoftkey()));
    connect(m_pDlgVPN, SIGNAL(RefreshSoftkey()), this, SLOT(OnRefreshSoftkey()));
    connect(m_pDlgCDP, SIGNAL(RefreshSoftkey()), this, SLOT(OnRefreshSoftkey()));
    connect(m_pDlgLLDP, SIGNAL(RefreshSoftkey()), this, SLOT(OnRefreshSoftkey()));
    connect(m_pDlgNAT, SIGNAL(RefreshSoftkey()), this, SLOT(OnRefreshSoftkey()));

    connect(m_pDlgIPMode, SIGNAL(ReLayout(CBaseDialog *, int)), this, SLOT(DoReLayout(CBaseDialog *,
            int)));
    connect(m_pDlgVLAN, SIGNAL(ReLayout(CBaseDialog *)), this, SLOT(DoReLayout(CBaseDialog *)));
    connect(m_pDlgVlanDHCP, SIGNAL(ReLayout(CBaseDialog *)), this, SLOT(DoReLayout(CBaseDialog *)));
    connect(m_pDlgX_Mode, SIGNAL(ReLayout(CBaseDialog *)), this, SLOT(DoReLayout(CBaseDialog *)));
    connect(m_pDlgCDP, SIGNAL(ReLayout(CBaseDialog *)), this, SLOT(DoReLayout(CBaseDialog *)));
    connect(m_pDlgLLDP, SIGNAL(ReLayout(CBaseDialog *)), this, SLOT(DoReLayout(CBaseDialog *)));
    connect(m_pDlgNAT, SIGNAL(ReLayout(CBaseDialog *)), this, SLOT(DoReLayout(CBaseDialog *)));
}

void CDlgZeroNetwork::Init()
{
    if (NULL == m_pDlgIPMode
            || NULL == m_pDlgVLAN
            || NULL == m_pDlgVlanDHCP
            || NULL == m_pDlgX_Mode
            || NULL == m_pDlgVPN
            || NULL == m_pDlgNAT
            || NULL == m_pDlgLLDP
            || NULL == m_pDlgCDP
            || NULL == m_pGeneralFrame)
    {
        return;
    }

    //设置成员的页面类型
    m_pDlgIPMode->Init();

    if (-1 == m_iFocusLine)
    {
        m_pZeroListDelegate->SetCurrentIndex(0);
        m_pZeroListDelegate->UpdateList();
        //
        this->blockSignals(true);
        m_pDlgIPMode->LoadPageData();
        m_pDlgVLAN->LoadPageData();
        m_pDlgX_Mode->LoadPageData();
        m_pDlgVPN->LoadPageData();
        m_pDlgVlanDHCP->LoadPageData();
        m_pDlgLLDP->LoadPageData();
        m_pDlgCDP->LoadPageData();
        m_pDlgNAT->LoadPageData();

        DoReLayout(NULL);
        this->blockSignals(false);
    }
}

void CDlgZeroNetwork::Uninit()
{
}

void CDlgZeroNetwork::ClearFocusMark()
{
    m_iFocusLine = -1;
}

bool CDlgZeroNetwork::IsStatusChanged()
{
    return (m_pDlgIPMode->IsStatusChanged()
            || m_pDlgVLAN->IsStatusChanged()
            || m_pDlgX_Mode->IsStatusChanged()
            || m_pDlgVPN->IsStatusChanged()
            || m_pDlgVlanDHCP->IsStatusChanged()
            || m_pDlgNAT->IsStatusChanged()
            || m_pDlgLLDP->IsStatusChanged()
            || m_pDlgCDP->IsStatusChanged()
           );
}

bool CDlgZeroNetwork::SaveNetworkData()
{
    if (!IsStatusChanged())
    {
        _NetworkChangeFlagManager.SetNetworkChangeFlag(false);
        return true;
    }

    ReturnAllEdit();

    if (CheckInputData())
    {
        bool bSuccess = (m_pDlgIPMode->SavePageData()
                         && m_pDlgVLAN->SavePageData()
                         && m_pDlgX_Mode->SavePageData()
                         && m_pDlgVPN->SavePageData()
                         && m_pDlgVlanDHCP->SavePageData()
                         && m_pDlgLLDP->SavePageData()
                         && m_pDlgCDP->SavePageData()
                         && m_pDlgNAT->SavePageData()
                        );
        return bSuccess;
    }

    return false;
}

bool CDlgZeroNetwork::CheckInputData()
{
    InputDataErrorInfo objErrorInfo;
    if (m_pDlgIPMode->IsInputDataValid(objErrorInfo)
            && m_pDlgVLAN->IsInputDataValid(objErrorInfo)
            && m_pDlgX_Mode->IsInputDataValid(objErrorInfo)
            && m_pDlgVlanDHCP->IsInputDataValid(objErrorInfo)
            && m_pDlgLLDP->IsInputDataValid(objErrorInfo)
            && m_pDlgCDP->IsInputDataValid(objErrorInfo)
            && m_pDlgNAT->IsInputDataValid(objErrorInfo)
       )
    {
        // VPN 特殊处理，，InputDataErrorInfo暂只处理QLineEdit焦点
        if (m_pDlgVPN->IsInputDataValid(objErrorInfo))
        {
            return true;
        }
        else
        {
            MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(objErrorInfo.strErrorContent)
                                        , MESSAGEBOX_Cancel, MESSAGE_BOX_EXIST_TIME);

            // set error date's qwidget with focus
            QWPVector::Iterator it = m_vecWidgetPair.begin();
            QWidget * pWidget = NULL;
            for (int i = 0; it != m_vecWidgetPair.end(); ++it, i++)
            {
                pWidget = it->second;
                if (NULL != pWidget && pWidget->inherits("qCheckBox"))
                {
                    qCheckBox * pErrorWidget = (qCheckBox *)pWidget;
                    if (pErrorWidget == m_pDlgVPN->GetCkbPoint())
                    {
                        m_pZeroListDelegate->SelectIndex(i, true);
                    }
                }
            }

            if (NULL != m_pGeneralFrame && 0 != m_pGeneralFrame->GetFocusItemDataIndex())
            {
                DoFocusChanged(m_pGeneralFrame->GetFocusItemDataIndex());
            }
            return false;
        }
    }
    else
    {
        MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(objErrorInfo.strErrorContent)
                                    , MESSAGEBOX_Cancel, MESSAGE_BOX_EXIST_TIME);

        // set error date's qwidget with focus
        if (NULL != objErrorInfo.pLineEdit)
        {
            SetErrorFocus(objErrorInfo.pLineEdit);
        }

        if (NULL != m_pGeneralFrame && 0 != m_pGeneralFrame->GetFocusItemDataIndex())
        {
            DoFocusChanged(m_pGeneralFrame->GetFocusItemDataIndex());
        }

        return false;
    }
}

void CDlgZeroNetwork::SetErrorFocus(QLineEdit * pLineEdit)
{
    if (NULL != pLineEdit)
    {
        //在数组中查找控件对应的索引
        QWPVector::Iterator it = m_vecWidgetPair.begin();
        QWidget * pWidget = NULL;
        for (int i = 0; it != m_vecWidgetPair.end(); ++it, i++)
        {
            pWidget = it->second;
            if (NULL != pWidget && pWidget->inherits("QLineEdit"))
            {
                QLineEdit * pErrorWidget = (QLineEdit *)pWidget;
                if (pErrorWidget == pLineEdit)
                {
                    m_pZeroListDelegate->SelectIndex(i, true);
                    pLineEdit->end(false);
                    return;
                }
            }
        }
    }
}

bool CDlgZeroNetwork::eventFilter(QObject * pObj, QEvent * pEvent)
{
    if (NULL == pObj || NULL == pEvent)
    {
        return false;
    }

    switch (pEvent->type())
    {
    case QEvent::KeyPress:
    {
        //按键处理顺序：subpage->listDeletate->list
        QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;
        switch (pKeyEvent->key())
        {
        case PHONE_KEY_DOWN:
        case PHONE_KEY_UP:
        {
            if (NULL == m_pZeroListDelegate || NULL == m_pGeneralFrame
                    || m_pGeneralFrame->GetTotalItemCount() < 1)
            {
                break;
            }

            QWidget * pwgt = focusWidget();
            if (NULL != pwgt && pwgt->inherits("QComboBox"))
            {
                QComboBox * pComBox = (QComboBox *) pwgt;
                QAbstractItemView * pView = pComBox->view();
                if (NULL == pView || !pView->isVisible())
                {
                    break;
                }
                return false;
            }
        }
        break;
        case  PHONE_KEY_OK:
        {
            return false;
        }
        break;
        default:
            break;
        }
        if (FullKeyboardEnabled() && NULL != m_pZeroListDelegate && m_pZeroListDelegate->ProcessKeyPress(pKeyEvent->key()))
        {
            return true;
        }
    }
    break;
    case  QEvent::KeyRelease:
    {
        QKeyEvent* pKeyEvent = (QKeyEvent*)pEvent;
        switch (pKeyEvent->key())
        {
        case PHONE_KEY_DOWN:
        case PHONE_KEY_UP:
        {
            if (qGetIsShowWnd())
            {
                if (NULL != m_pZeroListDelegate)
                {
                    m_pZeroListDelegate->AdjustFrameList();
                }
            }

        }
        break;
        default:
            break;
        }
    }
    break;
    case QEvent::MouseButtonRelease:
    {
        if (!FullKeyboardEnabled())
        {
            break;
        }
        if (pObj == this || pObj == m_pGeneralFrame)
        {
            ZEROUI_ERR("Subpage MouseButton filt");
            pEvent->accept();
            qSetHideKeyboard();
            return true;
        }

        if (NULL != m_pZeroListDelegate && m_pZeroListDelegate->ProcessMouse(pObj))
        {
            return true;
        }
    }
    break;
    default:
        break;
    }

    return CDlgBaseSubPage::eventFilter(pObj, pEvent);
}

void CDlgZeroNetwork::ClearWgtpairData()
{
    // uninstall ime
    for (QWPVector::iterator iter = m_vecWidgetPair.begin();
            iter != m_vecWidgetPair.end(); ++iter)
    {
        QWidget * pWidget = iter->second;
        if (NULL != pWidget)
        {
            pWidget->setParent(this);
            pWidget->hide();
        }
    }

    m_vecWidgetPair.clear();
}

void CDlgZeroNetwork::DoReLayout(CBaseDialog * parent)
{
    int iCurSelectIndex = m_pGeneralFrame->GetFocusItemDataIndex();
#ifdef NEW_ZERO_NETWORK
    ClearWgtpairData();
#else
    MakeReadyForRelayout();
#endif

    GetNetworkWidgetPair();
#ifndef NEW_ZERO_NETWORK
    AddWidget(m_vecWidgetPair);
#endif
    if (NULL != m_pListDelegate && m_pListDelegate->inherits("ZeroListDelegate"))
    {
        ZeroListDelegate * pZeroLtDelegate = static_cast<ZeroListDelegate *>(m_pListDelegate);
        pZeroLtDelegate->SetItemsData(m_vecWidgetPair);
    }

    if (iCurSelectIndex < 0 || -1 == m_iFocusLine)
    {
        m_pGeneralFrame->SelectItemByDataIndex(0);
        m_iFocusLine = 0;
        iCurSelectIndex = 0;
    }
    else
    {
        m_pGeneralFrame->SelectItemByDataIndex(iCurSelectIndex);
    }
    this->UpdateWnd();
    m_pGeneralFrame->SelectItemByDataIndex(iCurSelectIndex);
}

void CDlgZeroNetwork::DoReLayout(CBaseDialog * pDlg, int iSelectIndex)
{
    int iCurSelectIndex = m_pGeneralFrame->GetFocusItemDataIndex();
#ifdef NEW_ZERO_NETWORK
    ClearWgtpairData();
#else
    MakeReadyForRelayout();
#endif

    GetNetworkWidgetPair();
#ifndef NEW_ZERO_NETWORK
    AddWidget(m_vecWidgetPair);
#endif
    if (NULL != m_pListDelegate && m_pListDelegate->inherits("ZeroListDelegate"))
    {
        ZeroListDelegate * pZeroLtDelegate = static_cast<ZeroListDelegate *>(m_pListDelegate);
        pZeroLtDelegate->SetItemsData(m_vecWidgetPair);
    }

    if (iCurSelectIndex < 0 || -1 == m_iFocusLine)
    {
        m_pGeneralFrame->SelectItemByDataIndex(0);
        m_iFocusLine = 0;
        iCurSelectIndex = 0;
    }
    else
    {
        m_pGeneralFrame->SelectItemByDataIndex(iCurSelectIndex);
    }
    this->UpdateWnd();
    m_pGeneralFrame->SelectItemByDataIndex(iCurSelectIndex);
}

void CDlgZeroNetwork::DoFocusChanged(int nIndex)
{
    ZEROUI_INFO("CDlgZeroNetwork::DoFocusChanged(%d) ", nIndex);
    QWidget * pWidget = m_vecWidgetPair[nIndex].second;
    _MainWND.DisplayIMESoftkey(pWidget);
}

CDlgZeroNetwork::~CDlgZeroNetwork(void)
{
    disconnect(m_pDlgIPMode, SIGNAL(RefreshSoftkey()), this, SLOT(OnRefreshSoftkey()));
    disconnect(m_pDlgVLAN, SIGNAL(RefreshSoftkey()), this, SLOT(OnRefreshSoftkey()));
    disconnect(m_pDlgX_Mode, SIGNAL(RefreshSoftkey()), this, SLOT(OnRefreshSoftkey()));
    disconnect(m_pDlgVPN, SIGNAL(RefreshSoftkey()), this, SLOT(OnRefreshSoftkey()));
    disconnect(m_pDlgVlanDHCP, SIGNAL(RefreshSoftkey()), this, SLOT(OnRefreshSoftkey()));
    disconnect(m_pDlgLLDP, SIGNAL(RefreshSoftkey()), this, SLOT(OnRefreshSoftkey()));
    disconnect(m_pDlgCDP, SIGNAL(RefreshSoftkey()), this, SLOT(OnRefreshSoftkey()));
    disconnect(m_pDlgNAT, SIGNAL(RefreshSoftkey()), this, SLOT(OnRefreshSoftkey()));

    disconnect(m_pDlgIPMode, SIGNAL(ReLayout(CBaseDialog *, int)), this, SLOT(DoReLayout(CBaseDialog *,
               int)));
    disconnect(m_pDlgVLAN, SIGNAL(ReLayout(CBaseDialog *)), this, SLOT(DoReLayout(CBaseDialog *)));
    disconnect(m_pDlgVlanDHCP, SIGNAL(ReLayout(CBaseDialog *)), this, SLOT(DoReLayout(CBaseDialog *)));
    disconnect(m_pDlgX_Mode, SIGNAL(ReLayout(CBaseDialog *)), this, SLOT(DoReLayout(CBaseDialog *)));
    disconnect(m_pDlgLLDP, SIGNAL(ReLayout(CBaseDialog *)), this, SLOT(DoReLayout(CBaseDialog *)));
    disconnect(m_pDlgCDP, SIGNAL(ReLayout(CBaseDialog *)), this, SLOT(DoReLayout(CBaseDialog *)));
    disconnect(m_pDlgNAT, SIGNAL(ReLayout(CBaseDialog *)), this, SLOT(DoReLayout(CBaseDialog *)));
}

bool CDlgZeroNetwork::LoadPageData()
{
    if (JudgetWgtNull() || NULL == m_pGeneralFrame)
    {
        return true;
    }

    if (this->isVisible() && m_vecWidgetPair.size() > 0)
    {
        m_pGeneralFrame->SelectItemByIndex(0);
    }
    return true;
}

bool CDlgZeroNetwork::SavePageData()
{
    return true;
}

void CDlgZeroNetwork::OnRefreshSoftkey()
{
    emit RefreshSoftkey();
}

bool CDlgZeroNetwork::JudgetWgtNull()
{
    if (NULL == m_pDlgIPMode
            || NULL == m_pDlgVLAN
            || NULL == m_pDlgX_Mode
            || NULL == m_pDlgVPN
            || NULL == m_pDlgVlanDHCP
            || NULL == m_pDlgCDP
            || NULL == m_pDlgLLDP
            || NULL == m_pDlgNAT)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void CDlgZeroNetwork::OnKeyboardShow()
{
    if (m_pZeroListDelegate != NULL)
    {
        if (!(m_pZeroListDelegate->IsListReady()))
        {
            return;
        }
        m_pZeroListDelegate->AdjustFrameList(false);
    }
}

void CDlgZeroNetwork::OnKeyboardHide()
{
    if (NULL != m_pZeroListDelegate)
    {
        m_pZeroListDelegate->AdjustFrameList(true);
    }
}

void CDlgZeroNetwork::GetWidgetPair(const yl::string & strID, CDlgBaseSubPage * pSubPage)
{
    if (strID.empty() || pSubPage == NULL)
    {
        return;
    }

    if (feature_UserModeConfigItemShow(strID))
    {
        QWPVector vecWidgetPair = pSubPage->GetWPVector();

        QWidget* pWidget = NULL;
        QWPVector::const_iterator it = vecWidgetPair.begin();
        for (; it != vecWidgetPair.end(); ++it)
        {
            pWidget = it->second;
            if (pWidget != NULL)
            {
                // 安装全键盘
                if (pWidget->inherits("QLineEdit") || pWidget->inherits("QTextEdit"))
                {
                    qUseKeyBoardFromIME(pWidget, INPUT_DIALOG_NEWLINE, false);
                }

                if (feature_UserModeConfigItemReadOnly(strID))
                {
                    pWidget->setEnabled(false);
                }
            }
        }

        m_vecWidgetPair += vecWidgetPair;
    }
}

void CDlgZeroNetwork::GetNetworkWidgetPair()
{
    GetWidgetPair(ZERO_IP_MODE, m_pDlgIPMode);
    GetWidgetPair(ZERO_VLAN, m_pDlgVLAN);
    GetWidgetPair(ZERO_X_MODE, m_pDlgX_Mode);
    if (configParser_GetConfigInt(kszForbidVPN) != 1)
    {
        GetWidgetPair(ZERO_VPN, m_pDlgVPN);
    }
    GetWidgetPair(ZERO_VLANDHCP, m_pDlgVlanDHCP);
    GetWidgetPair(ZERO_LLDP, m_pDlgLLDP);
    GetWidgetPair(ZERO_CDP, m_pDlgCDP);
    GetWidgetPair(ZERO_NAT, m_pDlgNAT);
}
