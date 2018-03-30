#include <QtGui>
#include "groupnormaltitle.h"
#include "directorylistaction.h"
#include "translateiddefine.h"
#include "imagemanager/modimagemanager.h"
#include "uikernel/qwidgetutility.h"
#include "baseui/t4xpicpath.h"
#include "uikernel/languagehelper.h"
#include "qtcommon/qmisc.h"
#include "qtcommon/xmlhelper.h"
namespace
{
#define GROUP_TREE_TITLE_WIDTH 256
#define GROUP_TREE_TITLE_HEIGHT 56
#define GROUP_TREE_TITLE_BTN_COMMON_TOP 1
#define GROUP_TREE_TITLE_BTN_COMMON_WIDTH 84
#define GROUP_TREE_TITLE_BTN_COMMON_HEIGHT 54
#define GROUP_TREE_TITLE_BTN_LEFT_LEFT 0
#define GROUP_TREE_TITLE_BTN_MIDDLE_LEFT 85
#define GROUP_TREE_TITLE_BTN_RIGHT_LEFT 170
#define GROUP_TREE_TITLE_SEPARATE_TOP 12
#define GROUP_TREE_TITLE_SEPARATE_WIDTH 1
#define GROUP_TREE_TITLE_SEPARATE_HEIGHT 30
#define GROUP_TREE_TITLE_SEPARATE1_LEFT 84
#define GROUP_TREE_TITLE_SEPARATE2_LEFT 169

#define GROUP_TREE_TITLE_FONT_SIZE_BTN_BAR 14
}

CGroupNormalTitle::CGroupNormalTitle(CFrameList * pFrameList/* = NULL*/,
                                     IFrameTitleAction * pAction/* = NULL*/)
    : CFrameListTitle(FRAME_LIST_TITLE_TYPE_GROUP_NOTMAL, pFrameList, pAction)
    , m_btnSearch(this)
    , m_btnAddContact(this)
    , m_btnSetting(this)
    , m_btnBar(this, false)
    , m_bReadOnly(false)
{
    setGeometry(0, 0, GROUP_TREE_TITLE_WIDTH, GROUP_TREE_TITLE_HEIGHT);
    SetShowBG(false);

    Relayout();

    qWidgetUtility::setFontStyle(this, GROUP_TREE_TITLE_FONT_SIZE_BTN_BAR);

    setObjectName("CGroupNormalTitle");
}

CGroupNormalTitle::~CGroupNormalTitle(void)
{
}

void CGroupNormalTitle::Relayout()
{
    if (m_bReadOnly)
    {
        m_btnSearch.SetRect(GROUP_TREE_TITLE_BTN_MIDDLE_LEFT, GROUP_TREE_TITLE_BTN_COMMON_TOP,
                            GROUP_TREE_TITLE_BTN_COMMON_WIDTH, GROUP_TREE_TITLE_BTN_COMMON_HEIGHT);
        m_btnSearch.SetDefaultPos();

        m_btnAddContact.SetRect(0, 0, 0, 0);
        m_btnSetting.SetRect(0, 0, 0, 0);

        m_btnBar.SetBtnCount(1);
        m_btnBar.SetBtn(0, &m_btnSearch);
    }
    else
    {
        m_btnSearch.SetRect(GROUP_TREE_TITLE_BTN_LEFT_LEFT, GROUP_TREE_TITLE_BTN_COMMON_TOP,
                            GROUP_TREE_TITLE_BTN_COMMON_WIDTH, GROUP_TREE_TITLE_BTN_COMMON_HEIGHT);
        m_btnSearch.SetDefaultPos();

        m_btnAddContact.SetRect(GROUP_TREE_TITLE_BTN_MIDDLE_LEFT, GROUP_TREE_TITLE_BTN_COMMON_TOP,
                                GROUP_TREE_TITLE_BTN_COMMON_WIDTH, GROUP_TREE_TITLE_BTN_COMMON_HEIGHT);
        m_btnAddContact.SetDefaultPos();

        m_btnSetting.SetRect(GROUP_TREE_TITLE_BTN_RIGHT_LEFT, GROUP_TREE_TITLE_BTN_COMMON_TOP,
                             GROUP_TREE_TITLE_BTN_COMMON_WIDTH, GROUP_TREE_TITLE_BTN_COMMON_HEIGHT);
        m_btnSetting.SetDefaultPos();

        m_btnBar.SetBtnCount(3);
        m_btnBar.SetBtn(0, &m_btnSearch);
        m_btnBar.SetBtn(1, &m_btnAddContact);
        m_btnBar.SetBtn(2, &m_btnSetting);
    }
}

bool CGroupNormalTitle::IsGroupNormalTitle(CFrameListTitlePtr pTitle)
{
    if (NULL != pTitle && FRAME_LIST_TITLE_TYPE_GROUP_NOTMAL == pTitle->GetType())
    {
        return true;
    }

    return false;
}

void CGroupNormalTitle::SetReadOnly(bool bReadOnly)
{
    bool bRelayout = false;

    if (m_bReadOnly != bReadOnly)
    {
        bRelayout = true;
    }

    m_bReadOnly = bReadOnly;

    if (bRelayout)
    {
        Relayout();
    }
}

void CGroupNormalTitle::mousePressEvent(QMouseEvent * pEvent)
{
    m_btnBar.ProcessMousePress(pEvent);
}

void CGroupNormalTitle::mouseReleaseEvent(QMouseEvent * pEvent)
{
    CButtonWrapper * pBtn = m_btnBar.ProcessMouseRelease(pEvent);

    if (NULL == pBtn)
    {
        return;
    }

    if (pBtn == &m_btnSearch)
    {
        DoAction(LIST_TITLE_ACTION_DIR_SEARCH);
    }
    else if (pBtn == &m_btnAddContact)
    {
        DoAction(LIST_TITLE_ACTION_DIR_ADD_CONTACT);
    }
    else if (pBtn == &m_btnSetting)
    {
        DoAction(LIST_TITLE_ACTION_DIR_SETTING);
    }
}

void CGroupNormalTitle::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);

    // 画搜索按钮（选中效果在最上层）
    m_btnSearch.PaintVButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_SEARCH,
                             LANG_TRANSLATE(TRID_SEARCH));
    m_btnSearch.PaintButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_BTN_CLICK);

    if (m_bReadOnly)
    {
        return;
    }

    // 画分隔线
    QRect rcSeparate(GROUP_TREE_TITLE_SEPARATE1_LEFT, GROUP_TREE_TITLE_SEPARATE_TOP,
                     GROUP_TREE_TITLE_SEPARATE_WIDTH, GROUP_TREE_TITLE_SEPARATE_HEIGHT);
    QPixmap pmSeparate = THEME_GET_BMP(PIC_DIRECTORY_DIRECTORY_SEPARATE);

    if (!pmSeparate.isNull())
    {
        stylePainter.drawPixmap(rcSeparate, pmSeparate);
    }

    // 画添加按钮（选中效果在最底层）
    m_btnAddContact.PaintVButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_ADD_CONTACT,
                                 LANG_TRANSLATE(TRID_ADD));
    m_btnAddContact.PaintButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_BTN_CLICK);

    // 画分隔线
    rcSeparate.setLeft(GROUP_TREE_TITLE_SEPARATE2_LEFT);
    rcSeparate.setWidth(GROUP_TREE_TITLE_SEPARATE_WIDTH);

    if (!pmSeparate.isNull())
    {
        stylePainter.drawPixmap(rcSeparate, pmSeparate);
    }

    // 画更多按钮（选中效果在最底层）
    m_btnSetting.PaintVButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_SETTING,
                              LANG_TRANSLATE(TRID_SETTING));
    m_btnSetting.PaintButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_BTN_CLICK);
}

#ifdef IF_ENABLE_TESTABILITY
QString CGroupNormalTitle::GetTestInfo()
{
    xml_document doc;
    xml_node nodeRoot = doc.append_child("testinfo");

    AddBtnNode(nodeRoot, m_btnSearch, BTN_SEARCH, fromQString(LANG_TRANSLATE(TRID_SEARCH)));
    AddBtnNode(nodeRoot, m_btnAddContact, BTN_ADD, fromQString(LANG_TRANSLATE(TRID_ADD)));
    AddBtnNode(nodeRoot, m_btnSetting, BTN_SETTING, fromQString(LANG_TRANSLATE(TRID_SETTING)));

    QString strTestinfo;
    GetStringFromXml(doc, strTestinfo);

    return strTestinfo;
}
#endif