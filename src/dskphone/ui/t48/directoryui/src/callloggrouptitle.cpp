#include <QtGui>
#include "callloggrouptitle.h"
#include "directorylistaction.h"
#include "translateiddefine.h"
#include "baseui/t4xpicpath.h"
#include "imagemanager/modimagemanager.h"
#include "uikernel/languagehelper.h"
#include "uikernel/qwidgetutility.h"
#include "directorycommon.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "qtcommon/qmisc.h"
#include "qtcommon/xmlhelper.h"

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

CCallLogGroupTitle::CCallLogGroupTitle(CFrameList * pFrameList/* = NULL*/,
                                       IFrameTitleAction * pAction/* = NULL*/)
    : CFrameListTitle(FRAME_LIST_TITLE_TYPE_CALL_LOG_GROUP, pFrameList, pAction)
    , m_btnSetting(this)
#if DIR_SUPPORT_NET_CALLLOG_DELETE
    , m_bDeleteState(false)
#endif
#ifdef IF_ENABLE_TESTABILITY
    , m_strBtn("")
#endif
{
    setGeometry(0, 0, GROUP_TREE_TITLE_WIDTH, GROUP_TREE_TITLE_HEIGHT);
    SetShowBG(false);

    m_btnSetting.SetRect(GROUP_TREE_TITLE_BTN_ADDGROUP_LEFT, GROUP_TREE_TITLE_BTN_COMMON_TOP,
                         GROUP_TREE_TITLE_BTN_COMMON_WIDTH, GROUP_TREE_TITLE_BTN_COMMON_HEIGHT);
    m_btnSetting.SetDefaultPos();

    qWidgetUtility::setFontStyle(this, GROUP_TREE_TITLE_FONT_SIZE_BTN_BAR);

    setObjectName("CCallLogGroupTitle");
}

CCallLogGroupTitle::~CCallLogGroupTitle(void)
{
}


bool CCallLogGroupTitle::IsCallLogGroupTitle(CFrameListTitlePtr pTitle)
{
    if (NULL != pTitle && FRAME_LIST_TITLE_TYPE_CALL_LOG_GROUP == pTitle->GetType())
    {
        return true;
    }

    return false;
}

void CCallLogGroupTitle::SetBtnVisible(bool bVisible)
{
    if (bVisible)
    {
        m_btnSetting.SetRect(GROUP_TREE_TITLE_BTN_ADDGROUP_LEFT, GROUP_TREE_TITLE_BTN_COMMON_TOP,
                             GROUP_TREE_TITLE_BTN_COMMON_WIDTH, GROUP_TREE_TITLE_BTN_COMMON_HEIGHT);
        m_btnSetting.SetDefaultPos();
    }
    else
    {
        m_btnSetting.SetRect(0, 0, 0, 0);
        m_btnSetting.SetIconRect(0, 0, 0, 0);
        m_btnSetting.SetTextRect(0, 0, 0, 0);
    }
}

#if DIR_SUPPORT_NET_CALLLOG_DELETE
void CCallLogGroupTitle::SetDeleteState(bool bDeleteState)
{
    if (configParser_GetConfigInt(kszBsftLogDeleteEnable) == 1)
    {
        m_bDeleteState = bDeleteState;
        update();
    }
}
#endif

void CCallLogGroupTitle::mousePressEvent(QMouseEvent * pEvent)
{
    m_btnSetting.ProcessMousePress(pEvent);
}

void CCallLogGroupTitle::mouseReleaseEvent(QMouseEvent * pEvent)
{
    if (m_btnSetting.ProcessMouseRelease(pEvent))
    {
#if DIR_SUPPORT_NET_CALLLOG_DELETE
        if (configParser_GetConfigInt(kszBsftLogDeleteEnable) == 1 && m_bDeleteState)
        {
            DoAction(LIST_TITLE_ACTION_DIR_CLEAR_CROUP);
        }
        else
#endif
        {
            DoAction(LIST_TITLE_ACTION_DIR_SETTING);
        }
    }
}

void CCallLogGroupTitle::paintEvent(QPaintEvent * pEvent)
{
    if (m_btnSetting.GetRect().isNull())
    {
        return;
    }

    QStylePainter stylePainter(this);

#if DIR_SUPPORT_NET_CALLLOG_DELETE
    if (configParser_GetConfigInt(kszBsftLogDeleteEnable) == 1 && m_bDeleteState)
    {
        // 画删除按钮
        m_btnSetting.PaintVButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_DEL_CONTACT,
                                  LANG_TRANSLATE(TRID_DELETE_ALL));
#ifdef IF_ENABLE_TESTABILITY
        m_strBtn = LANG_TRANSLATE(TRID_DELETE_ALL);
#endif
    }
    else
#endif
    {
        // 画搜索按钮（选中效果在最上层）
        m_btnSetting.PaintVButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_SETTING,
                                  LANG_TRANSLATE(TRID_SETTING));
#ifdef IF_ENABLE_TESTABILITY
        m_strBtn = LANG_TRANSLATE(TRID_SETTING);
#endif
    }

    QRect rcClick(GROUP_TREE_TITLE_BTN_CLICK_LEFT, GROUP_TREE_TITLE_BTN_COMMON_TOP,
                  GROUP_TREE_TITLE_BTN_CLICK_WIDTH, GROUP_TREE_TITLE_BTN_COMMON_HEIGHT);

    m_btnSetting.PaintButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_BTN_CLICK, NULL, rcClick);
}

#ifdef IF_ENABLE_TESTABILITY
QString CCallLogGroupTitle::GetTestInfo()
{
    xml_document doc;
    xml_node nodeRoot = doc.append_child("testinfo");

    AddBtnNode(nodeRoot, m_btnSetting, BTN_SETTING, fromQString(m_strBtn));

    QString strTestinfo;
    GetStringFromXml(doc, strTestinfo);

    return strTestinfo;
}
#endif
