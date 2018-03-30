#include "dlgdirdetail.h"
#include "directorycommon.h"
#include "translateiddefine.h"
#include "directorylistdelegate.h"
#include "directorymgr.h"
#include "dirctrlfactory.h"
#include "baseui/framelist.h"
#include "baseui/t4xpicpath.h"
#include "uicommon.h"
#include "keymap.h"
#include "commonunits/modcommonunits.h"
#include "imagemanager/modimagemanager.h"
#include "messagebox/modmessagebox.h"
#include "uimanager/rectdefine.h"
#include "uikernel/languagehelper.h"
#include "uikernel/qwidgetutility.h"

#include "voice/include/modvoice.h"
#include "qtcommon/qmisc.h"
#include "qtcommon/xmlhelper.h"

#if IF_FEATURE_GBBOOK
#include "detaildelegate.h"
#endif

namespace
{
// 分隔线位置信息
#define DLG_DIR_DETAIL_POS_SEPARATOR_LEFT 163
#define DLG_DIR_DETAIL_POS_SEPARATOR_TOP 72
#define DLG_DIR_DETAIL_POS_SEPARATOR_WIDTH 502
#define DLG_DIR_DETAIL_POS_SEPARATOR_HEIGHT 2

// 图像框位置信息
#define DLG_DIR_DETAIL_POS_IMAGE_CONTACT_LEFT 136
#define DLG_DIR_DETAIL_POS_IMAGE_CONTACT_TOP 92
#define DLG_DIR_DETAIL_POS_IMAGE_CONTACT_WEIGHT 108
#define DLG_DIR_DETAIL_POS_IMAGE_CONTACT_HEIGHT 108

// 翻页按钮信息
#define DLG_DIR_DETAIL_POS_BTN_PRE_LEFT 35
#define DLG_DIR_DETAIL_POS_BTN_NEXT_LEFT 701
#define DLG_DIR_DETAIL_POS_BTN_TOP 139
#define DLG_DIR_DETAIL_POS_BTN_WIDTH 56
#define DLG_DIR_DETAIL_POS_BTN_HEIGHT 60

// 信息
#define DLG_DIR_DETAIL_POS_PAGE_LEFT 669
#define DLG_DIR_DETAIL_POS_PAGE_TOP 310
#define DLG_DIR_DETAIL_POS_PAGE_WEIGHT 120
#define DLG_DIR_DETAIL_POS_PAGE_HEIGHT 20

// 字体大小
#define DLG_DIR_DETAIL_FONT_SIZE 16
}

CDlgDirDetail::CDlgDirDetail(QWidget * parent/* = NULL*/)
    : CBaseDialog(parent)
    , m_btnPreDetail(this)
    , m_btnNextDetail(this)
    , m_btnImage(this)
    , m_btnBar(this, false)
    , m_pFrameList(NULL)
    , m_nContactType(DIRECTORY_TYPE_NONE)
    , m_nContactId(0)
    , m_nDataIndex(0)
    , m_nTotalCount(1)
    , m_bReturn(false)
    , m_nLineOffset(0)
{
    SetWindowType(eWT_Directory);

    SetTopWnd();

    // 设置窗体位置
    SetDlgGeometry(RECT_DIRECTORY);

    SetSoftkeyType(STYLE_TYPE_DIVIDE);

    m_btnPreDetail.SetRect(DLG_DIR_DETAIL_POS_BTN_PRE_LEFT, DLG_DIR_DETAIL_POS_BTN_TOP,
                           DLG_DIR_DETAIL_POS_BTN_WIDTH, DLG_DIR_DETAIL_POS_BTN_HEIGHT);
    m_btnNextDetail.SetRect(DLG_DIR_DETAIL_POS_BTN_NEXT_LEFT, DLG_DIR_DETAIL_POS_BTN_TOP,
                            DLG_DIR_DETAIL_POS_BTN_WIDTH, DLG_DIR_DETAIL_POS_BTN_HEIGHT);
    m_btnImage.SetRect(DLG_DIR_DETAIL_POS_IMAGE_CONTACT_LEFT, DLG_DIR_DETAIL_POS_IMAGE_CONTACT_TOP,
                       DLG_DIR_DETAIL_POS_IMAGE_CONTACT_WEIGHT, DLG_DIR_DETAIL_POS_IMAGE_CONTACT_HEIGHT);

    m_btnBar.SetBtnCount(3);
    m_btnBar.SetBtn(0, &m_btnPreDetail);
    m_btnBar.SetBtn(1, &m_btnNextDetail);
    m_btnBar.SetBtn(2, &m_btnImage);

    m_pFrameList = new CFrameList(this);

    if (NULL != m_pFrameList)
    {
        m_pFrameList->SetRect(RECT_DIR_CONTACT_DETAIL);
        m_pFrameList->SetGridLine(false);
        m_pFrameList->show();
        m_pFrameList->raise();
    }

    qWidgetUtility::setFontStyle(this, DLG_DIR_DETAIL_FONT_SIZE, QFont::Bold);
}

CDlgDirDetail::~CDlgDirDetail(void)
{

}

void CDlgDirDetail::Init()
{
    m_bReturn = false;
}

void CDlgDirDetail::Uninit()
{
    m_bReturn = false;
}

void CDlgDirDetail::UpdateWnd()
{
    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();
#ifdef DIR_SUPPORT_SWITCH_DETAIL
    bool bReturn = false;
#endif

    bool bNeedCheckDelegate = false;
#if IF_FEATURE_GBBOOK
    CDetailDelegate * pDetailDelegate = CDetailDelegate::GetDirDelegate(pDelegate);
    if (NULL == pDetailDelegate && m_nContactType == DIRECTORY_TYPE_GENBAND)
    {
        bNeedCheckDelegate = true;
    }
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    if (!pDelegate.IsEmpty())
    {
        bNeedCheckDelegate = pDelegate->GetState() != DIR_UI_STATE_DETAIL;
        if (pDelegate->GetDirType() != m_nContactType)
        {
            g_DirectoryMgr.SetCurDirType(m_nContactType);
        }
    }
#endif

    if (pDelegate.IsEmpty()
            || bNeedCheckDelegate
       )
    {
        CDirectoryControlerPtr pControler = g_DirCtrlFactory.GetControler(m_nContactType);

        if (pControler.IsEmpty())
        {
            return;
        }

        g_DirectoryMgr.CheckContactDelegate(DIR_UI_STATE_DETAIL, pControler->GetGroupId(), false);

        pDelegate = g_DirectoryMgr.GetActiveDelegate();

        if (pDelegate.IsEmpty())
        {
            return;
        }

        m_pFrameList->SetDelegate(pDelegate.Get());
#ifdef DIR_SUPPORT_SWITCH_DETAIL
        bReturn = true;
#endif
    }

    CDirectoryControlerPtr pControler = pDelegate->GetControler();

    if (pControler.IsEmpty())
    {
        return;
    }

#ifdef DIR_SUPPORT_SWITCH_DETAIL
    if (bReturn)
    {
        pControler->RefreshTotalCount();
    }
#endif

#ifdef DIR_SUPPORT_SWITCH_DETAIL
    if (bReturn)
    {
        pControler->RefreshTotalCount();
    }
#endif

    pControler->GetDetailInfo(m_nContactId, m_nDataIndex, m_nTotalCount);
    pControler->GetContactImage(m_nContactId, m_strImage);

    DIRUI_INFO("Init Detail, ContactId:%d DataIndex:%d TotalCount:%d Image:%s",
               m_nContactId, m_nDataIndex, m_nTotalCount, m_strImage.c_str());

    pDelegate->RefreshFrameList();

    UpdateDetail();
}

void CDlgDirDetail::UpdateDetail()
{
    LoadContactImage();

    QRect rcText(DLG_DIR_DETAIL_POS_PAGE_LEFT, DLG_DIR_DETAIL_POS_PAGE_TOP,
                 DLG_DIR_DETAIL_POS_PAGE_WEIGHT, DLG_DIR_DETAIL_POS_PAGE_HEIGHT);

    update(rcText);
    update(m_btnImage.GetRect());
}

void CDlgDirDetail::LoadContactImage()
{
    if (!m_strImage.empty())
    {
        if (DIRECTORY_TYPE_BTUC == m_nContactType)
        {
            if (!TMP_GET_BMP(m_pmImage, m_strImage.c_str()))
            {
                m_pmImage = QPixmap();
            }
        }
        else
        {
            m_pmImage = THEME_GET_BMP(m_strImage.c_str());

            if (m_pmImage.isNull())
            {
                m_pmImage = STOCK_GET_BMP(m_strImage.c_str());
            }
        }
    }
    else if (DIRECTORY_TYPE_BLUETOOTH == m_nContactType)
    {
        m_pmImage = THEME_GET_BMP(PIC_CONTACTS_IMAGE_BLUETOOTH_DEFAULT);
    }
    else
    {
        m_pmImage = THEME_GET_BMP(PIC_CONTACTS_IMAGE_DEFAULT);
    }

    if (m_pmImage.isNull())
    {
        m_pmImage = THEME_GET_BMP(PIC_CONTACTS_IMAGE_DEFAULT);
    }
}

void CDlgDirDetail::SetContact(int nContactType)
{
    m_nContactType = nContactType;

    if (m_nContactType > DIRECTORY_TYPE_NONE && m_nContactType < DIRECTORY_TYPE_MAX)
    {
        SetWindowTitle(LANG_TRANSLATE(TRID_CONTACT_DETAILS));
        SetWindowType(eWT_Directory);
    }
    else if (m_nContactType >= CALL_LOG_TYPE_LOCAL && m_nContactType <= CALL_LOG_TYPE_BROADSOFT)
    {
        SetWindowTitle(LANG_TRANSLATE(TRID_HISTORY_DETAILS));
        SetWindowType(eWT_CallLog);
    }
}

int CDlgDirDetail::GetDirType() const
{
    return m_nContactType;
}

CFrameList * CDlgDirDetail::GetFrameList() const
{
    return m_pFrameList;
}

bool CDlgDirDetail::eventFilter(QObject * pObj, QEvent * pEvent)
{
    if (NULL == pEvent)
    {
        return false;
    }

    if (MessageBox_GetMessageBoxCount() > 0)
    {
        return false;
    }

    if (NULL != m_pFrameList && m_pFrameList->eventFilter(pObj, pEvent))
    {
        return true;
    }

    switch (pEvent->type())
    {
    case QEvent::KeyPress://键盘输入事件
        {
            QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;
            int nKey = pKeyEvent->key();

            switch (nKey)
            {
            case PHONE_KEY_OK:
                {
                    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();

                    if (!pControler.IsEmpty())
                    {
                        pControler->DialContact(m_nContactId);
                        return true;
                    }
                }
                break;
            case PHONE_KEY_CANCEL:
                {
                    UIManager_PutPrivateDlg(this);
                    return true;
                }
                break;
            case PHONE_KEY_LEFT:
                {
                    if (OnLeftPressEventFilter(pObj, pEvent))
                    {
                        return true;
                    }
                }
                break;
            case PHONE_KEY_RIGHT:
                {
                    if (OnRightPressEventFilter(pObj, pEvent))
                    {
                        return true;
                    }
                }
                break;
            case PHONE_KEY_STAR:
            case PHONE_KEY_POUND:
                {
                    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();

                    if (commonUnits_IsSendDigitKey(nKey) && !pControler.IsEmpty())
                    {
                        pControler->DialContact(m_nContactId);
                        return true;
                    }
                }
                break;
            default:
                break;
            }
        }
        break;
    default:
        break;
    }

    return CBaseDialog::eventFilter(pObj, pEvent);
}

void CDlgDirDetail::OnFocusChanged(bool bFocused)
{
    if (!bFocused)
    {
        m_bReturn = true;

        if (NULL != m_pFrameList)
        {
            m_pFrameList->SetDelegate(NULL);
        }
    }
}

bool CDlgDirDetail::OnLeftPressEventFilter(QObject * pObj, QEvent * pEvent)
{
    OnBtnPreDetail();

    return true;
}

bool CDlgDirDetail::OnRightPressEventFilter(QObject * pObj, QEvent * pEvent)
{
    OnBtnNextDetail();

    return true;
}

//子界面可单独设置其背景图片，返回图片路径，如果图片为空，则按模板设置
QPixmap CDlgDirDetail::GetCustomBackGround()
{
    return THEME_GET_BMP(PIC_DIRECTORY_DIRECTORY_DETAIL_BACKGROUND);
}

void CDlgDirDetail::mousePressEvent(QMouseEvent * pEvent)
{
    m_btnBar.ProcessMousePress(pEvent);
}

void CDlgDirDetail::mouseReleaseEvent(QMouseEvent * pEvent)
{
    CButtonWrapper * pBtn = m_btnBar.ProcessMouseRelease(pEvent);

    if (pBtn == &m_btnPreDetail)
    {
        OnBtnPreDetail();
    }
    else if (pBtn == &m_btnNextDetail)
    {
        OnBtnNextDetail();
    }
    else if (pBtn == &m_btnImage)
    {
        OnBtnImage();
    }
}

void CDlgDirDetail::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);

    // 画分隔线
    QRect rcSeparator(DLG_DIR_DETAIL_POS_SEPARATOR_LEFT,
                      DLG_DIR_DETAIL_POS_SEPARATOR_TOP
                      + m_nLineOffset
                      , DLG_DIR_DETAIL_POS_SEPARATOR_WIDTH
                      , DLG_DIR_DETAIL_POS_SEPARATOR_HEIGHT);

    QPixmap pmSeparator = THEME_GET_BMP(PIC_DIRECTORY_DIRECTORY_DETAIL_SEPARATEOR);

    if (!pmSeparator.isNull())
    {
        stylePainter.drawPixmap(rcSeparator, pmSeparator);
    }

    // 画按钮
    m_btnPreDetail.PaintButton(stylePainter, PIC_GENERAL_BTN_PRE_PAGE, PIC_GENERAL_BTN_PRE_PAGE_CLICK);
    m_btnNextDetail.PaintButton(stylePainter, PIC_GENERAL_BTN_NEXT_PAGE,
                                PIC_GENERAL_BTN_NEXT_PAGE_CLICK);

    m_btnImage.PaintButton(stylePainter, m_pmImage);

    // 画页码
    char szBuffer[64];

    snprintf(szBuffer, sizeof(szBuffer), "%d/%d", (m_nDataIndex + 1), m_nTotalCount);
    szBuffer[sizeof(szBuffer) - 1] = '\0';

    QLatin1String strPageInfo(szBuffer);
    QRect rcText(DLG_DIR_DETAIL_POS_PAGE_LEFT, DLG_DIR_DETAIL_POS_PAGE_TOP,
                 DLG_DIR_DETAIL_POS_PAGE_WEIGHT, DLG_DIR_DETAIL_POS_PAGE_HEIGHT);
#ifdef IF_ENABLE_TESTABILITY
    m_rcPage = rcText;
    m_strPage = strPageInfo;
#endif
    stylePainter.drawItemText(rcText, Qt::AlignCenter, palette(), isEnabled(), strPageInfo);
}

void CDlgDirDetail::OnBtnPreDetail()
{
    if (m_nTotalCount <= 1)
    {
        return;
    }

    int nIndex = m_nDataIndex;

    --nIndex;

    if (nIndex < 0)
    {
        nIndex = m_nTotalCount - 1;
    }

    if (nIndex < 0)
    {
        nIndex = 0;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    int nId = knInvalidRId;

    if (!pControler.IsEmpty() && pControler->SwitchContact(nId, nIndex))
    {
        m_nContactId = nId;
        m_nDataIndex = nIndex;

        pControler->GetContactImage(m_nContactId, m_strImage);

        CDirUIHelperPtr pUIHelper = pControler->GetUIHelper();

        if (!pUIHelper.IsEmpty())
        {
            pUIHelper->InitDetailTitle();
        }

        if (NULL != m_pFrameList)
        {
            m_pFrameList->RefreshData(0);
        }

        UpdateDetail();
    }
}

void CDlgDirDetail::OnBtnNextDetail()
{
    if (m_nTotalCount <= 1)
    {
        return;
    }

    int nIndex = m_nDataIndex;

    ++nIndex;

    if (nIndex >= m_nTotalCount)
    {
        nIndex = 0;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    int nId = knInvalidRId;

    if (!pControler.IsEmpty() && pControler->SwitchContact(nId, nIndex))
    {
        m_nContactId = nId;
        m_nDataIndex = nIndex;

        pControler->GetContactImage(m_nContactId, m_strImage);

        CDirUIHelperPtr pUIHelper = pControler->GetUIHelper();

        if (!pUIHelper.IsEmpty())
        {
            pUIHelper->InitDetailTitle();
        }

        if (NULL != m_pFrameList)
        {
            m_pFrameList->RefreshData(0);
        }

        UpdateDetail();
    }
}

void CDlgDirDetail::OnBtnImage()
{

}

bool CDlgDirDetail::KeypadHook(int nKeyCode, bool bPress)
{
    if (!bPress || (PHONE_KEY_HANDSET_OFF_HOOK != nKeyCode && PHONE_KEY_HANDFREE != nKeyCode
                    && PHONE_KEY_EHS_OPEN != nKeyCode))
    {
        return false;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();

    if (pControler.IsEmpty())
    {
        return false;
    }

    int nType = pControler->GetType();

    if (CALL_LOG_TYPE_LOCAL == nType || CALL_LOG_TYPE_BROADSOFT == nType)
    {
        //voice_OnKeyPress(nKeyCode);
        pControler->DialContact(m_nContactId);
        return true;
    }

    return false;
}

#ifdef IF_ENABLE_TESTABILITY
QString CDlgDirDetail::GetTestInfo()
{
    xml_document doc;
    xml_node nodeRoot = doc.append_child("testinfo");

    TestInfo testInfo;
    QPoint ptAbsolute;

    // page
    ptAbsolute = mapToGlobal(m_rcPage.topLeft());
    testInfo.x_absolute = ptAbsolute.x();
    testInfo.y_absolute = ptAbsolute.y();
    testInfo.width = m_rcPage.width();
    testInfo.height = m_rcPage.height();
    testInfo.id = LABEL_PAGE;
    testInfo.text = fromQString(m_strPage);
    testInfo.type = VIRTUAL_LABEL;

    AddNode(nodeRoot, testInfo);

    AddBtnNode(nodeRoot, m_btnPreDetail, BTN_PRE);
    AddBtnNode(nodeRoot, m_btnNextDetail, BTN_NEXT);
    AddBtnNode(nodeRoot, m_btnImage, BTN_IMAGE);

    QString strTestinfo;
    GetStringFromXml(doc, strTestinfo);
    return strTestinfo;
}
#endif
