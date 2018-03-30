#include <QtGui>
#include "groupedittitle.h"
#include "directorylistaction.h"
#include "translateiddefine.h"
#include "baseui/t4xpicpath.h"
#include "imagemanager/modimagemanager.h"
#include "uikernel/languagehelper.h"
#include "uikernel/qwidgetutility.h"


namespace
{
#define GROUP_TREE_TITLE_WIDTH 256
#define GROUP_TREE_TITLE_HEIGHT 56
#define GROUP_TREE_TITLE_BTN_COMMON_TOP 1
#define GROUP_TREE_TITLE_BTN_COMMON_WIDTH 100
#define GROUP_TREE_TITLE_BTN_COMMON_HEIGHT 54
#define GROUP_TREE_TITLE_BTN_ADDGROUP_LEFT 73
#define GROUP_TREE_TITLE_BTN_CLICK_LEFT 81
#define GROUP_TREE_TITLE_BTN_CLICK_WIDTH 84

#define GROUP_TREE_TITLE_FONT_SIZE_BTN_BAR 14
}

CGroupEditTitle::CGroupEditTitle(CFrameList * pFrameList/* = NULL*/,
                                 IFrameTitleAction * pAction/* = NULL*/)
    : CFrameListTitle(FRAME_LIST_TITLE_TYPE_GROUP_EDIT, pFrameList, pAction)
    , m_btnAddGroup(this)
{
    setGeometry(0, 0, GROUP_TREE_TITLE_WIDTH, GROUP_TREE_TITLE_HEIGHT);
    SetShowBG(false);

    m_btnAddGroup.SetRect(GROUP_TREE_TITLE_BTN_ADDGROUP_LEFT, GROUP_TREE_TITLE_BTN_COMMON_TOP,
                          GROUP_TREE_TITLE_BTN_COMMON_WIDTH, GROUP_TREE_TITLE_BTN_COMMON_HEIGHT);
    m_btnAddGroup.SetDefaultPos();

    qWidgetUtility::setFontStyle(this, GROUP_TREE_TITLE_FONT_SIZE_BTN_BAR);

    setObjectName("CGroupEditTitle");
}

CGroupEditTitle::~CGroupEditTitle(void)
{
}

CGroupEditTitle * CGroupEditTitle::GetGroupEditTitle(CFrameListTitlePtr pTitle)
{
    if (NULL != pTitle && FRAME_LIST_TITLE_TYPE_GROUP_EDIT == pTitle->GetType())
    {
        return (CGroupEditTitle *)pTitle;
    }

    return NULL;
}

void CGroupEditTitle::SetBtnVisible(bool bVisible)
{
    if (bVisible)
    {
        m_btnAddGroup.SetRect(GROUP_TREE_TITLE_BTN_ADDGROUP_LEFT, GROUP_TREE_TITLE_BTN_COMMON_TOP,
                              GROUP_TREE_TITLE_BTN_COMMON_WIDTH, GROUP_TREE_TITLE_BTN_COMMON_HEIGHT);
        m_btnAddGroup.SetDefaultPos();
    }
    else
    {
        m_btnAddGroup.SetRect(0, 0, 0, 0);
        m_btnAddGroup.SetIconRect(0, 0, 0, 0);
        m_btnAddGroup.SetTextRect(0, 0, 0, 0);
    }
}

void CGroupEditTitle::mousePressEvent(QMouseEvent * pEvent)
{
    m_btnAddGroup.ProcessMousePress(pEvent);
}

void CGroupEditTitle::mouseReleaseEvent(QMouseEvent * pEvent)
{
    bool bDown = m_btnAddGroup.IsDown();

    if (!bDown)
    {
        return;
    }

    if (!m_btnAddGroup.ProcessMouseRelease(pEvent))
    {
        bDown = false;
    }

    if (bDown)
    {
        DoAction(LIST_TITLE_ACTION_DIR_ADD_GROUP);
    }
}

void CGroupEditTitle::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);

    // 画搜索按钮（选中效果在最上层）
    m_btnAddGroup.PaintVButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_ADD_GROUP,
                               LANG_TRANSLATE(TRID_NEW_GROUP));

    QRect rcClick(GROUP_TREE_TITLE_BTN_CLICK_LEFT, GROUP_TREE_TITLE_BTN_COMMON_TOP,
                  GROUP_TREE_TITLE_BTN_CLICK_WIDTH, GROUP_TREE_TITLE_BTN_COMMON_HEIGHT);

    m_btnAddGroup.PaintButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_BTN_CLICK, NULL, rcClick);
}
