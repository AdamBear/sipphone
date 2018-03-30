#include "cuiregiondirectory.h"
#include "textstylemanager.h"
#include "qtcommon/qmisc.h"
#include "keymap.h"
#include "uicommon.h"
#include "baseui/pagectrl.h"
#include "xbpositiondefine.h"
#include "uimanager/rectdefine.h"
#include "baseui/t4xpicpath.h"
#if IF_FEATURE_XMLDIRECTORY
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "inputmethod/inputmethod.h"
#include "extended/xmlbrowser/include/modxmlbrowser.h"
#include "uicommon/uikernel/qlineeditutility.h"
#include "imagemanager/modimagemanager.h"
#include "uikernel/qwidgetutility.h"
#endif

namespace
{
#define XB_DIRECTORY_LEFT_SPACING 0
// #define SEARCH_DELAY_TIME 850
#define DLG_DIR_SEARCH_MARGIN_TOP 1
#define DLG_DIR_SEARCH_EDIT_LEFT 6
#define DLG_DIR_SEARCH_EDIT_WIDTH 466
#define DLG_DIR_SEARCH_EDIT_HEIGHT 42
#define SEARCH_DELAY_TIME_MIN 0
#define SEARCH_DELAY_TIME_MAX 10000
}
///////////////////////////////////////////////////////////////
CUIRegionDirectory::CUIRegionDirectory(QWidget * parent, Qt::WFlags flags)
    : CUIRegionBase(parent, flags)
// , IPageCtrlNotify()
    , IScrollNotify()
    , m_pFrmDirectory(NULL)
    , m_pLabTitle(NULL)
    , m_pPageCtrl(NULL)
#if IF_FEATURE_XMLDIRECTORY
    , m_pInput(NULL)
#endif
{
    setObjectName("UIRegionDirectory");
    m_pLabTitle = new CTextMenuItemIS(this);
    if (m_pLabTitle != NULL)
    {
        m_pLabTitle->setObjectName("LabTitle");
        m_pLabTitle->setAlignment(Qt::AlignCenter);
        m_pLabTitle->setFocusPolicy(Qt::NoFocus);
        m_pLabTitle->setMinimumHeight(XML_TITLE_HEIGHT);
    }

    m_pFrmDirectory = new CFrmTextMenu(this);
    if (m_pFrmDirectory != NULL)
    {
        m_pFrmDirectory->setObjectName("FrmDirectory");
        connect(m_pFrmDirectory, SIGNAL(ClickItem(int)), this, SLOT(OnClickItem(int)));
        connect(m_pFrmDirectory, SIGNAL(PageChanged(int, int)), this, SLOT(OnPageChanged(int, int)));
        connect(m_pFrmDirectory, SIGNAL(CurrentItemChange(int)), this, SLOT(OnCurrentItemChange(int)));
        m_pFrmDirectory->SetSelectedItemBg(PIC_GENERAL_LIST_ITEM_CLICK_BG);
    }
    m_pPageCtrl = new CPageCtrl(this, this);

    if (NULL != m_pPageCtrl)
    {
        m_pPageCtrl->setObjectName("PageCtrl");
    }
}

void CUIRegionDirectory::OnPageChanged(int nCurrentPage, int nSumPage)
{
    if (m_pPageCtrl != NULL)
    {
        m_pPageCtrl->SetScrollInfo(nSumPage, nCurrentPage - 1);
        m_pPageCtrl->update();

        if (nSumPage <= 1)
        {
            m_pPageCtrl->hide();
        }
        else
        {
            m_pPageCtrl->show();
        }
    }
}

void CUIRegionDirectory::OnCurrentItemChange(int iIndex)
{
#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
#if IF_FEATURE_XMLDIRECTORY
    if (this->focusWidget() == m_pInput)
    {
        if (m_vecSearchSoftkey.size() > 0)
        {
            SetSoftkey(m_vecSearchSoftkey);
        }
        else
        {
            SetDefaultSoftkey(m_vecSoftkey);
        }
    }
    else
#endif
    {
        VecSoftkeyParam vecPrivateSoftkey = m_pFrmDirectory->GetFocusSoftkey();
        if (vecPrivateSoftkey.size() > 0)
        {
            SetSoftkey(vecPrivateSoftkey);
        }
        else if (m_bDefSoftKey)
        {
            SetDefaultSoftkey(m_vecSoftkey);
        }
        else
        {
            SetSoftkey(m_vecGlobalSoftkey);
        }
    }
#endif
    emit RegionAction(REGION_ACTION_CURRENT_ITEM_CHANGE);
}

// CUIRegionDirectory
void CUIRegionDirectory::OnNotifyScrollPrevious()
{
    if (m_pFrmDirectory != NULL)
    {
        m_pFrmDirectory->SetFocusItem(0);
        m_pFrmDirectory->OnLastPage();
    }
}

// 通知翻到下一页
void CUIRegionDirectory::OnNotifyScrollNext()
{
    if (m_pFrmDirectory != NULL)
    {
        m_pFrmDirectory->SetFocusItem(0);
        m_pFrmDirectory->OnNextPage();
    }
}

#if IF_FEATURE_XMLDIRECTORY
void CUIRegionDirectory::Search()
{
    if (m_SearchTimer.isActive())
    {
        m_SearchTimer.stop();
    }
    QString strSearchContent = qReturnEdit(m_pInput);
    if (FullKeyboardEnabled())
    {
        strSearchContent = m_pInput->text();
    }
    if (!strSearchContent.isEmpty())
    {
        xmlbrowser_OnAction(GetElementID(), XB_ACTION_SEARCH_PHONEBOOK,
                            (void *)strSearchContent.toUtf8().data());
    }
}

bool CUIRegionDirectory::OnXMLUIAction(XB_SOFTKEY_TYPE eType)
{
    bool bHandle = false;
    switch (eType)
    {
    case XB_ST_SEARCH:
        {
            bHandle = true;
            Search();
        }
        break;
    default:
        break;
    }
    return bHandle;
}
#endif

void CUIRegionDirectory::OnClickItem(int nIndex)
{
    //点击了某个Item或者按下了ok
    emit RegionAction(REGION_ACTION_CLICK_ITEM);
}

CUIRegionDirectory::~CUIRegionDirectory()
{
#if IF_FEATURE_XMLDIRECTORY
    if (m_pInput != NULL)
    {
        qUninstallIME(m_pInput);
    }

    if (m_SearchTimer.isActive())
    {
        m_SearchTimer.stop();
    }
#endif
}

void CUIRegionDirectory::Update(void * pData)
{
    //!!!!! 逻辑有问题
    if (pData == NULL)
    {
        return;
    }

    XBDirectoryData * pNewData = static_cast<XBDirectoryData *>(pData);

    if (m_pLabTitle == NULL
            || m_pFrmDirectory == NULL
            || m_pPageCtrl == NULL)
    {
        return;
    }

    m_pLabTitle->setGeometry(XB_DIRECTORY_LEFT_SPACING, 0, this->width() - XB_DIRECTORY_LEFT_SPACING
                             - XML_SCROLL_RIGHT_SPACING, XML_TITLE_HEIGHT);
    m_pLabTitle->SetTextWrap(pNewData->stAttr.bWrapTitle);
    m_pLabTitle->SetTextContent(toQString(pNewData->stAttr.strTitle.c_str()));

    m_pLabTitle->UpdateDiaplay();

#if IF_FEATURE_XMLDIRECTORY
    if (!pNewData->stInputField.strParam.empty())
    {
        m_pInput = new QLineEdit(this);
        if (m_pInput != NULL)
        {
            qWidgetUtility::setFontStyle(m_pInput, 18, QFont::Normal, false);

            // 显示搜索的值
            if (!pNewData->stInputField.strDefault.empty())
            {
                m_pInput->setText(toQString(pNewData->stInputField.strDefault.c_str()));
            }

            qInstallIME(m_pInput, IME_Name);
#if IF_FEATURE_AUTOP_IME
            yl::string strDefaultIME = configParser_GetConfigString(kszXmlInputStringDefaultIME);
            if (!strDefaultIME.empty())
            {
                qSetCurrentIME(m_pInput, strDefaultIME.c_str(), 0, kszXmlInputStringDefaultIME);
            }
#endif
            qUseKeyBoardFromIME(m_pInput, INPUT_DIALOG_SEARCH, true);
            connect(m_pInput, SIGNAL(textChanged(const QString &)), this,
                    SLOT(OnKeyWordChanged(const QString &)));
            connect(&m_SearchTimer, SIGNAL(timeout()), this, SLOT(OnTimerOut()));
            m_iSearchDelay = configParser_GetConfigInt(kszXMLPhonebookSearchDelay);

            if (m_iSearchDelay < SEARCH_DELAY_TIME_MIN)
            {
                m_iSearchDelay = SEARCH_DELAY_TIME_MIN;
            }

            if (m_iSearchDelay > SEARCH_DELAY_TIME_MAX)
            {
                m_iSearchDelay = SEARCH_DELAY_TIME_MAX;
            }

            m_pInput->setGeometry(DLG_DIR_SEARCH_EDIT_LEFT, m_pLabTitle->height() + DLG_DIR_SEARCH_MARGIN_TOP,
                                  DLG_DIR_SEARCH_EDIT_WIDTH, DLG_DIR_SEARCH_EDIT_HEIGHT);
            m_pInput->setFocus();
            m_pInput->show();
            m_pInput->raise();

            m_pFrmDirectory->setGeometry(XB_DIRECTORY_LEFT_SPACING,
                                         m_pLabTitle->height() + m_pInput->height() + DLG_DIR_SEARCH_MARGIN_TOP * 2,
                                         this->width() - XML_SCROLL_WIDTH
                                         - XML_SCROLL_LEFT_SPACING - XML_SCROLL_RIGHT_SPACING - XB_DIRECTORY_LEFT_SPACING,
                                         this->height() - m_pLabTitle->height() - m_pInput->height());

            m_pFrmDirectory->SetData(pNewData);

            m_pPageCtrl->SetRect(this->width() - XML_SCROLL_WIDTH - XML_SCROLL_RIGHT_SPACING,
                                 m_pLabTitle->height() + XML_SCROLL_BAR_TOP_SPACING + m_pInput->height() + DLG_DIR_SEARCH_MARGIN_TOP
                                 * 2,
                                 XML_SCROLL_WIDTH, m_pFrmDirectory->height() - XML_SCROLL_BAR_TOP_SPACING -
                                 XML_SCROLL_BAR_BOTTOM_SPACING - DLG_DIR_SEARCH_MARGIN_TOP * 2);
        }
    }

    if (m_pInput == NULL)
#endif
    {
        m_pFrmDirectory->setGeometry(XB_DIRECTORY_LEFT_SPACING, m_pLabTitle->height(),
                                     this->width() - XML_SCROLL_WIDTH
                                     - XML_SCROLL_LEFT_SPACING - XML_SCROLL_RIGHT_SPACING - XB_DIRECTORY_LEFT_SPACING,
                                     this->height() - m_pLabTitle->height());

        m_pFrmDirectory->SetData(pNewData);

        m_pPageCtrl->SetRect(this->width() - XML_SCROLL_WIDTH - XML_SCROLL_RIGHT_SPACING,
                             m_pLabTitle->height() + XML_SCROLL_BAR_TOP_SPACING,
                             XML_SCROLL_WIDTH, m_pFrmDirectory->height() - XML_SCROLL_BAR_TOP_SPACING -
                             XML_SCROLL_BAR_BOTTOM_SPACING);
    }

    m_bLock = pNewData->stAttr.bLockIn;

    m_iQuitTime = pNewData->stAttr.iTimeOut;

#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
    m_bDefSoftKey = (pNewData->vecSoftkey.size() == 0);
    m_vecGlobalSoftkey = pNewData->vecSoftkey;
#if IF_FEATURE_XMLDIRECTORY
    //保存搜索自定义softkey
    m_vecSearchSoftkey = pNewData->stInputField.vecSoftkey;
    if (m_vecSearchSoftkey.size() > 0)
    {
        SetSoftkey(m_vecSearchSoftkey);
    }
    else
    {
        SetDefaultSoftkey(m_vecSoftkey);
    }
    return;
#endif
    VecSoftkeyParam vecPrivateSoftkey = m_pFrmDirectory->GetFocusSoftkey();
    if (vecPrivateSoftkey.size() > 0)
    {
        SetSoftkey(vecPrivateSoftkey);
    }
    else
#endif
    {
        SetSoftkey(pNewData->vecSoftkey);
    }
}

// 获取页面的默认softkey数据
void CUIRegionDirectory::SetDefaultSoftkey(VecSoftkeyParam & softkeyParam)
{
    softkeyParam.clear();
#if IF_FEATURE_XMLDIRECTORY
    if (m_pInput != NULL
            && this->focusWidget() == m_pInput)
    {
        softkeyParam.push_back(TypeToSoftInfo(XB_ST_EXIT));
        softkeyParam.push_back(TypeToSoftInfo(XB_ST_CHANGEMODE));
        softkeyParam.push_back(TypeToSoftInfo(XB_ST_BACKSPACE));
        softkeyParam.push_back(TypeToSoftInfo(XB_ST_SEARCH));
    }
    else
#endif
    {
        softkeyParam.push_back(TypeToSoftInfo(XB_ST_DIAL));
        softkeyParam.push_back(TypeToSoftInfo(XB_ST_PREVIOUS));
        softkeyParam.push_back(TypeToSoftInfo(XB_ST_NEXT));
    }

//  softkeyParam.push_back(TypeToSoftInfo(XB_ST_EXIT));
}

bool CUIRegionDirectory::eventFilter(QObject * pObj, QEvent * pEvent)
{
#if IF_FEATURE_XMLDIRECTORY
    if (pEvent->type() == QEvent::MouseButtonPress
            || pEvent->type() == QEvent::KeyPress)
    {
        if (m_SearchTimer.isActive())
        {
            m_SearchTimer.stop();
            m_SearchTimer.start(m_iSearchDelay);
        }
    }
#endif

    if (pEvent->type() == QEvent::KeyPress)
    {
        if (OnKeyPress(pObj, pEvent))
        {
            return true;
        }
    }

#if IF_FEATURE_XMLDIRECTORY
    bool bHandle = false;
    if (m_pInput != NULL && m_pFrmDirectory != NULL)
    {
        switch (pEvent->type())
        {
        case QEvent::KeyPress:
            {
                QKeyEvent * pKeyEvent = static_cast<QKeyEvent *>(pEvent);
                if (pKeyEvent != NULL)
                {
                    switch (pKeyEvent->key())
                    {
                    case PHONE_KEY_UP:
                        {
                            if (m_pFrmDirectory->CurrIsFirstItem()
                                    && this->focusWidget() != m_pInput)
                            {
                                bHandle = true;
                                m_pInput->setFocus();
                                OnCurrentItemChange(0);
                            }
                            else if (this->focusWidget() == m_pInput
                                     && m_pFrmDirectory->GetItmeSize() > 0)
                            {
                                bHandle = true;
                                m_pFrmDirectory->SetFocusItem();
                                OnCurrentItemChange(0);
                                m_pFrmDirectory->SetLastPageFocusItem();
                            }
                        }
                        break;
                    case PHONE_KEY_DOWN:
                        {
                            if (m_pFrmDirectory->CurrIsLastItem()
                                    && this->focusWidget() != m_pInput)
                            {
                                bHandle = true;
                                m_pInput->setFocus();
                                OnCurrentItemChange(0);
                            }
                            else if (this->focusWidget() == m_pInput
                                     && m_pFrmDirectory->GetItmeSize() > 0)
                            {
                                bHandle = true;
                                m_pFrmDirectory->SetFocusItem();
                                OnCurrentItemChange(0);
                                m_pFrmDirectory->SetFirstPageFocusItem();
                            }
                        }
                        break;
                    case PHONE_KEY_OK:
                        {
                            if (this->focusWidget() == m_pInput)
                            {
                                Search();
                            }
                        }
                        break;
                    default:
                        {
                            if (pKeyEvent->key() == PHONE_KEY_STAR
                                    || pKeyEvent->key() == PHONE_KEY_POUND
                                    || (pKeyEvent->key() >= PHONE_KEY_0
                                        && pKeyEvent->key() <= PHONE_KEY_9))
                            {
                                if (this->focusWidget() != m_pInput)
                                {
                                    m_pInput->setFocus();
                                }
                                if (qPassEventToEdit(m_pInput, pEvent))
                                {
                                    if (m_SearchTimer.isActive())
                                    {
                                        m_SearchTimer.stop();
                                        m_SearchTimer.start(m_iSearchDelay);
                                    }
                                    OnCurrentItemChange(0);
                                    bHandle = true;
                                }
                            }
                        }
                        break;
                    }
                }
            }
        }
    }

    if (bHandle)
    {
        return true;
    }
#endif

    if (m_pFrmDirectory != NULL)
    {
        return m_pFrmDirectory->eventFilter(pObj, pEvent);
    }
    return false;
}

int CUIRegionDirectory::GetCurrentItem()
{
    if (m_pFrmDirectory != NULL)
    {
        return m_pFrmDirectory->GetCurrentIndex();
    }
    return 0;
}

void CUIRegionDirectory::OnKeyWordChanged(const QString & strText)
{
#if IF_FEATURE_XMLDIRECTORY
    if (m_SearchTimer.isActive())
    {
        m_SearchTimer.stop();
    }

    m_SearchTimer.start(m_iSearchDelay);
#endif
}

void CUIRegionDirectory::OnTimerOut()
{
#if IF_FEATURE_XMLDIRECTORY
    Search();
#endif
}
