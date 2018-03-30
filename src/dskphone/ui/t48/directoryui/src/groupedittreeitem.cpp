#include <QtGui>
#include "groupedittreeitem.h"

#include "directorycommon.h"
#include "dlgdirsetting.h"
#include "baseui/fontcolor.h"
#include "baseui/t4xpicpath.h"
#include "uicommon.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "imagemanager/modimagemanager.h"
#include "inputmethod/inputmethod.h"
#include "uikernel/languagehelper.h"
#include "uimanager/moduimanager.h"
#include "uikernel/qlineeditutility.h"
#include "qtcommon/qmisc.h"
#include "qtcommon/xmlhelper.h"

namespace
{
#define GROUP_EDIT_ITEM_POS_BTN_TOP 7
#define GROUP_EDIT_ITEM_POS_BTN_WIDTH 32
#define GROUP_EDIT_ITEM_POS_BTN_HEIGHT 32

#define GROUP_EDIT_ITEM_POS_ICON_LEFT 4
#define GROUP_EDIT_ITEM_POS_ICON_TOP 4
#define GROUP_EDIT_ITEM_POS_ICON_WIDTH 24
#define GROUP_EDIT_ITEM_POS_ICON_HEIGHT 24

#define GROUP_EDIT_ITEM_POS_BTN_DEL_LEFT 9
#define GROUP_EDIT_ITEM_POS_BTN_MODIFY_LEFT 132
#define GROUP_EDIT_ITEM_POS_BTN_RING_LEFT 170

#define GROUP_EDIT_ITEM_POS_EDIT_BG_LEFT 42
#define GROUP_EDIT_ITEM_POS_EDIT_BG_TOP 11
#define GROUP_EDIT_ITEM_POS_EDIT_BG_WIDHT 95
#define GROUP_EDIT_ITEM_POS_EDIT_BG_HEIGHT 25

#define GROUP_EDIT_ITEM_POS_EDIT_LEFT 41
#define GROUP_EDIT_ITEM_POS_EDIT_TOP 11
#define GROUP_EDIT_ITEM_POS_EDIT_WIDHT 95
#define GROUP_EDIT_ITEM_POS_EDIT_HEIGHT 25
}

CGroupEditTreeItem::CGroupEditTreeItem(QWidget * parent/* = NULL*/,
                                       IListItemAction * pAction/* = NULL*/)
    : CGroupTreeItem(parent, pAction)
    , m_btnDel(this)
    , m_btnModify(this)
    , m_btnRing(this)
    , m_btnBar(this, false)
    , m_pEdit(NULL)
    , m_bEditable(false)
    , m_bEditing(false)
#if IF_FEATURE_GBBOOK
    , m_bShowBtnRing(true)
#endif
{
    m_nType |= LIST_ITEM_SUB_TYPE_GROUP_EDIT_ITEM;

    m_btnBar.SetBtnCount(3);
    m_btnBar.SetBtn(0, &m_btnDel);
    m_btnBar.SetBtn(1, &m_btnModify);
    m_btnBar.SetBtn(2, &m_btnRing);

    setObjectName("CGroupEditTreeItem");
}

CGroupEditTreeItem::~CGroupEditTreeItem(void)
{
    ReleaseGroupLineEdit();
}

int CGroupEditTreeItem::GetGroupEditTreeItemType()
{
    return (LIST_ITEM_TYPE_TREE | LIST_ITEM_SUB_TYPE_GROUP_TREE | LIST_ITEM_SUB_TYPE_GROUP_EDIT_ITEM);
}

CGroupEditTreeItem* CGroupEditTreeItem::GetGroupEditTreeItem(CListItem* pItem)
{
    if (NULL == pItem || GetGroupEditTreeItemType() != pItem->GetType())
    {
        return NULL;
    }

    return (CGroupEditTreeItem*)pItem;
}

void CGroupEditTreeItem::ClearContent()
{
    m_bEditable = false;
    m_bEditing = false;

#if IF_FEATURE_GBBOOK
    m_bShowBtnRing = true;
#endif

    CGroupTreeItem::ClearContact();
}

void CGroupEditTreeItem::SetFocus(bool bFocus)
{
    if (m_bEditable && m_bEditing && NULL != m_pEdit)
    {
        QWidget * pFocus = QApplication::focusWidget();

        if (pFocus != m_pEdit)
        {
            m_pEdit->setFocus();
        }
        qSetIsShowWnd(m_pEdit, true);
    }

    CGroupTreeItem::SetFocus(bFocus);
}

void CGroupEditTreeItem::SetEditable(bool bEditable)
{
    bool bUpdate = false;

    if (bEditable != m_bEditable)
    {
        bUpdate = true;
    }

    m_bEditable = bEditable;

    if (bUpdate)
    {
        Relayout();

        update();
    }
}

#if IF_FEATURE_GBBOOK
void CGroupEditTreeItem::SetShowBtnRing(bool bShow)
{
    bool bUpdate = false;

    if (bShow != m_bShowBtnRing)
    {
        bUpdate = true;
    }

    m_bShowBtnRing = bShow;

    if (bUpdate)
    {
        Relayout();

        update();
    }
}
#endif

void CGroupEditTreeItem::SetEditing(bool bEditing)
{
    bool bUpdate = false;

    if (bEditing != m_bEditing)
    {
        bUpdate = true;
    }

    m_bEditing = bEditing;

    if (bUpdate)
    {
        Relayout();

        update();

        if (m_bEditing)
        {
            qSetIsShowWnd(m_pEdit, true);
        }
    }
}

// 是否可以编辑
bool CGroupEditTreeItem::IsEditable() const
{
    return m_bEditable;
}

// 是否编辑中
bool CGroupEditTreeItem::IsEditing() const
{
    return m_bEditing;
}

void CGroupEditTreeItem::Relayout()
{
    if (!m_bEditable)
    {
        m_btnDel.SetRect(0, 0, 0, 0);
        m_btnModify.SetRect(0, 0, 0, 0);
        m_btnRing.SetRect(0, 0, 0, 0);

        ReleaseGroupLineEdit();
        return;
    }

    m_btnDel.SetRect(GROUP_EDIT_ITEM_POS_BTN_DEL_LEFT, GROUP_EDIT_ITEM_POS_BTN_TOP,
                     GROUP_EDIT_ITEM_POS_BTN_WIDTH, GROUP_EDIT_ITEM_POS_BTN_HEIGHT);

    if (m_bEditing)
    {
        m_btnModify.SetRect(GROUP_EDIT_ITEM_POS_BTN_RING_LEFT, GROUP_EDIT_ITEM_POS_BTN_TOP,
                            GROUP_EDIT_ITEM_POS_BTN_WIDTH, GROUP_EDIT_ITEM_POS_BTN_HEIGHT);

        m_btnRing.SetRect(0, 0, 0, 0);

        InitGroupLineEdit();
    }
    else
    {
        m_btnModify.SetRect(GROUP_EDIT_ITEM_POS_BTN_MODIFY_LEFT, GROUP_EDIT_ITEM_POS_BTN_TOP,
                            GROUP_EDIT_ITEM_POS_BTN_WIDTH, GROUP_EDIT_ITEM_POS_BTN_HEIGHT);

#if IF_FEATURE_GBBOOK
        if (!m_bShowBtnRing)
        {
            m_btnRing.SetRect(0, 0, 0, 0);
        }
        else
#endif
        {
            m_btnRing.SetRect(GROUP_EDIT_ITEM_POS_BTN_RING_LEFT, GROUP_EDIT_ITEM_POS_BTN_TOP,
                              GROUP_EDIT_ITEM_POS_BTN_WIDTH, GROUP_EDIT_ITEM_POS_BTN_HEIGHT);
        }


        ReleaseGroupLineEdit();
    }
}

void CGroupEditTreeItem::InitGroupLineEdit()
{
    if (NULL == m_pEdit)
    {
        // 暂时不用控件池
        m_pEdit = new QLineEdit(this);

        if (NULL == m_pEdit)
        {
            return;
        }
    }

    DirUI_SetNameIME(m_pEdit);

    qUseKeyBoardFromIME(m_pEdit, INPUT_DIALOG_ENTER);

    qLineEditUtility::SetLineEditStyle(m_pEdit, THEME_TEXT_COLOR, THEME_EDIT_HL_COLOR,
                                       THEME_EDIT_HL_TEXT_COLOR);

    m_pEdit->setGeometry(GROUP_EDIT_ITEM_POS_EDIT_LEFT, GROUP_EDIT_ITEM_POS_EDIT_TOP,
                         GROUP_EDIT_ITEM_POS_EDIT_WIDHT, GROUP_EDIT_ITEM_POS_EDIT_HEIGHT);
    m_pEdit->setMaxLength(MAX_LEN_DISPLAY_NAME);
    m_pEdit->setReadOnly(false);
    m_pEdit->setText(LANG_PARSE(m_strGroupName));
    m_pEdit->selectAll();
    m_pEdit->show();
    m_pEdit->raise();
    m_pEdit->installEventFilter(this);
    m_pEdit->setFocus();
}

void CGroupEditTreeItem::ReleaseGroupLineEdit()
{
    if (NULL == m_pEdit)
    {
        return;
    }

    bool bFocusChange = false;
    QWidget * pWidget = QApplication::focusWidget();

    if (NULL != pWidget && pWidget == m_pEdit)
    {
        bFocusChange = true;
    }

    // 暂时不用控件池
    /*qUninstallIME(m_pEdit);*/

    QLineEdit* pEdit = m_pEdit;
    m_pEdit = NULL;

    qSetIsShowWnd(pEdit, false);

    pEdit->removeEventFilter(this);
    pEdit->setParent(NULL);
    pEdit->hide();
    pEdit->deleteLater();
    pEdit = NULL;

    if (bFocusChange)
    {
        NotifyFocusChange();
    }
}

void CGroupEditTreeItem::OnFinishEdit()
{
    if (NULL != m_pEdit)
    {
        qReturnEdit(m_pEdit);

        m_strGroupName = m_pEdit->text();
    }

    DoAction(LIST_ITEM_ACTION_GROUP_MODIFY);
}

void CGroupEditTreeItem::NotifyFocusChange()
{
    CDlgDirSetting * pDlgDirSetting = (CDlgDirSetting *)UIManager_GetPrivateDlg(DLG_CDlgDirSetting);

    if (NULL != pDlgDirSetting && pDlgDirSetting == UIManager_GetTopWindow())
    {
        pDlgDirSetting->OnCtrlFocusChanged();
    }
}

bool CGroupEditTreeItem::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (NULL != pObject && pObject == m_pEdit && NULL != pEvent)
    {
        bool bFocusChange = false;
        int nType = pEvent->type();

        if (QEvent::FocusIn == nType)
        {
            bFocusChange = true;
        }
        else if (QEvent::FocusOut == nType)
        {
            bFocusChange = true;

            if (0 != GetId())
            {
                SetEditing(false);
            }
            else
            {
                DoAction(LIST_ITEM_ACTION_GROUP_DEL);
            }
        }

        if (bFocusChange)
        {
            NotifyFocusChange();
        }
    }

    return CGroupTreeItem::eventFilter(pObject, pEvent);
}

void CGroupEditTreeItem::mousePressEvent(QMouseEvent * pEvent)
{
    if (!m_bEditable)
    {
        CGroupTreeItem::mousePressEvent(pEvent);
        return;
    }

    CButtonWrapper * pBtn = m_btnBar.ProcessMousePress(pEvent);

    if (NULL == pBtn)
    {
        CGroupTreeItem::mousePressEvent(pEvent);
    }
}

void CGroupEditTreeItem::mouseReleaseEvent(QMouseEvent * pEvent)
{
    CGroupTreeItem::mouseReleaseEvent(pEvent);

    if (!m_bEditable)
    {
        return;
    }

    CButtonWrapper * pBtn = m_btnBar.ProcessMouseRelease(pEvent);

    if (&m_btnDel == pBtn)
    {
        if (!IsFocus())
        {
            SelectItem(true);
        }

        DoAction(LIST_ITEM_ACTION_GROUP_DEL);
    }
    else if (&m_btnModify == pBtn)
    {
        if (m_bEditing)
        {
            OnFinishEdit();
        }
        else
        {
            if (!IsFocus())
            {
                SelectItem(true);
            }

            DoAction(LIST_ITEM_ACTION_GROUP_MODIFY);
        }
    }
    else if (&m_btnRing == pBtn)
    {
#if IF_FEATURE_GBBOOK
        if (!m_bShowBtnRing)
        {
            return;
        }
#endif
        if (!IsFocus())
        {
            SelectItem(true);
        }

        DoAction(LIST_ITEM_ACTION_GROUP_SET_RING);
    }
}

bool CGroupEditTreeItem::SaveGroup()
{
    if (m_bEditable && m_bEditing)
    {
        OnFinishEdit();

        return true;
    }

    return false;
}

void CGroupEditTreeItem::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);
    QPoint ptIconEnd;

    PaintGroupTree(stylePainter, ptIconEnd);

    if (!m_bEditing)
    {
        int nTextRight = 0;
        const QRect & rcBtnModify = m_btnModify.GetRect();

        if (!rcBtnModify.isNull())
        {
            nTextRight = rcBtnModify.left() + GROUP_EDIT_ITEM_POS_ICON_LEFT;
        }

        PaintGroupText(stylePainter, ptIconEnd, nTextRight);
    }
    else
    {
        QPixmap pmEdit = THEME_GET_BMP(PIC_DIRECTORY_EDIT_GROUP_NAME);
        QRect rcEdit(GROUP_EDIT_ITEM_POS_EDIT_BG_LEFT, GROUP_EDIT_ITEM_POS_EDIT_BG_TOP,
                     GROUP_EDIT_ITEM_POS_EDIT_BG_WIDHT, GROUP_EDIT_ITEM_POS_EDIT_BG_HEIGHT);

        if (!pmEdit.isNull())
        {
            stylePainter.drawPixmap(rcEdit, pmEdit);
        }
    }

    if (!m_bEditable)
    {
        return;
    }

    bool bSelect = false;

    if (IsDown() || IsFocus())
    {
        bSelect = true;
    }

    // 删除按钮
    QPixmap pmDel;

    if (bSelect)
    {
        pmDel = THEME_GET_BMP(PIC_DIRECTORY_DIRECTORY_DEL_GROUP_SELECT);
    }
    else
    {
        pmDel = THEME_GET_BMP(PIC_DIRECTORY_DIRECTORY_DEL_GROUP);
    }

    const QRect & rcBtnDel = m_btnDel.GetRect();
    QRect rcIconDel(rcBtnDel.left() + GROUP_EDIT_ITEM_POS_ICON_LEFT,
                    rcBtnDel.top() + GROUP_EDIT_ITEM_POS_ICON_TOP,
                    GROUP_EDIT_ITEM_POS_ICON_WIDTH, GROUP_EDIT_ITEM_POS_ICON_HEIGHT);

    if (!pmDel.isNull())
    {
        stylePainter.drawPixmap(rcIconDel, pmDel);
    }

    m_btnDel.PaintButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_GROUP_BTN_CLICK);

    // 更改按钮
    const QRect & rcBtnModify = m_btnModify.GetRect();
    QRect rcIconModify(rcBtnModify.left() + GROUP_EDIT_ITEM_POS_ICON_LEFT,
                       rcBtnModify.top() + GROUP_EDIT_ITEM_POS_ICON_TOP,
                       GROUP_EDIT_ITEM_POS_ICON_WIDTH, GROUP_EDIT_ITEM_POS_ICON_HEIGHT);

    QPixmap pmModify;

    if (m_bEditing)
    {
        if (bSelect)
        {
            pmModify = THEME_GET_BMP(PIC_DIRECTORY_DIRECTORY_MODIFIED_GROUP_SELECT);
        }
        else
        {
            pmModify = THEME_GET_BMP(PIC_DIRECTORY_DIRECTORY_MODIFIED_GROUP);
        }
    }
    else
    {
        if (bSelect)
        {
            pmModify = THEME_GET_BMP(PIC_DIRECTORY_DIRECTORY_MODIFY_GROUP_SELECT);
        }
        else
        {
            pmModify = THEME_GET_BMP(PIC_DIRECTORY_DIRECTORY_MODIFY_GROUP);
        }
    }

    stylePainter.drawPixmap(rcIconModify, pmModify);
    m_btnModify.PaintButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_GROUP_BTN_CLICK);

    if (m_bEditing)
    {
        return;
    }

#if IF_FEATURE_GBBOOK
    if (!m_bShowBtnRing)
    {
        return;
    }
#endif

    // 铃声按钮
    QPixmap pmRing;

    if (bSelect)
    {
        pmRing = THEME_GET_BMP(PIC_DIRECTORY_DIRECTORY_GROUP_RING_SELECT);
    }
    else
    {
        pmRing = THEME_GET_BMP(PIC_DIRECTORY_DIRECTORY_GROUP_RING);
    }

    const QRect & rcBtnRing = m_btnRing.GetRect();
    QRect rcIconRing(rcBtnRing.left() + GROUP_EDIT_ITEM_POS_ICON_LEFT,
                     rcBtnRing.top() + GROUP_EDIT_ITEM_POS_ICON_TOP,
                     GROUP_EDIT_ITEM_POS_ICON_WIDTH, GROUP_EDIT_ITEM_POS_ICON_HEIGHT);

    if (!pmRing.isNull())
    {
        stylePainter.drawPixmap(rcIconRing, pmRing);
    }

    m_btnRing.PaintButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_GROUP_BTN_CLICK);
#ifdef IF_ENABLE_TESTABILITY
    ichoose = bSelect;
#endif
}

#ifdef IF_ENABLE_TESTABILITY
QString CGroupEditTreeItem::GetTestInfo()
{
    QString strTestinfo;
    xml_document doc;
    xml_node nodeRoot = doc.append_child("testinfo");

    TestInfo testInfo;
    QPoint ptAbsolute;

    AddBtnNode(nodeRoot, m_btnDel, BTN_DELETE);
    AddBtnNode(nodeRoot, m_btnRing, BTN_RING);
    AddBtnNode(nodeRoot, m_btnModify, BTN_EDIT);

    ptAbsolute = mapToGlobal(m_rcText.topLeft());
    testInfo.x_absolute = ptAbsolute.x();
    testInfo.y_absolute = ptAbsolute.y();
    testInfo.width = m_rcText.width();
    testInfo.height = m_rcText.height();
    testInfo.choose = ichoose;
    testInfo.id = LABEL_TITLE;
    testInfo.text = fromQString(m_strGroupName);
    testInfo.type = VIRTUAL_LABEL;

    AddNode(nodeRoot, testInfo);

    GetStringFromXml(doc, strTestinfo);
    return strTestinfo;
}
#endif
