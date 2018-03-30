#include <QtGui>
#include "contactdetailtitle.h"
#include "directorylistaction.h"
#include "baseui/t4xpicpath.h"
#include "imagemanager/modimagemanager.h"
#include "uikernel/qwidgetutility.h"
#include "qtcommon/qmisc.h"
#include "qtcommon/xmlhelper.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "uikernel/languagehelper.h"
#include "translateiddefine.h"
#include "directorymgr.h"

namespace
{
#define CONTACT_DETAIL_TITLE_HEIGHT 73

#define CONTACT_DETAIL_TITLE_BTN_COMMON_TOP 0
#define CONTACT_DETAIL_TITLE_BTN_COMMON_WIDTH 82
#define CONTACT_DETAIL_TITLE_BTN_COMMON_HEIGHT 71

#define CONTACT_DETAIL_TITLE_ICON_COMMON_TOP 15
#define CONTACT_DETAIL_TITLE_TEXT_COMMON_TOP 39

#define CONTACT_DETAIL_TITLE_BTN_RIGHT 0
#define CONTACT_DETAIL_TITLE_BTN_SPACE 1
#define CONTACT_DETAIL_TITLE_BTN_SEPARATE 1

#define CONTACT_DETAIL_TREE_TITLE_SEPARATE_TOP 21
#define CONTACT_DETAIL_TREE_TITLE_SEPARATE_WIDTH 1
#define CONTACT_DETAIL_TREE_TITLE_SEPARATE_HEIGHT 30

#define CONTACT_DETAIL_TITLE_FONT_SIZE_BTN_BAR 14
}

CContactDetailTitle::CContactDetailTitle(CFrameList * pFrameList/* = NULL*/,
        IFrameTitleAction * pAction/* = NULL*/)
    : CFrameListTitle(FRAME_LIST_TITLE_TYPE_CONTACT_DETAIL_TITLE, pFrameList, pAction)
    , m_btnDial(this)
    , m_btnMove(this)
    , m_btnEdit(this)
    , m_btnBlackList(this)
    , m_btnBar(this, false)
    , m_btnDel(this)
#if IF_FEATURE_METASWITCH_DIRECTORY
    , m_btnAddMtswContact(this)
#endif
#if IF_FEATURE_FAVORITE_DIR
    , m_btnFavorite(this)
    , m_btnRemoveFavorite(this)
#endif

    , m_nBtnCount(0)
{
    setGeometry(0, 0, 0, CONTACT_DETAIL_TITLE_HEIGHT);

    qWidgetUtility::setFontStyle(this, CONTACT_DETAIL_TITLE_FONT_SIZE_BTN_BAR);

    setObjectName("CContactDetailTitle");
}

CContactDetailTitle::~CContactDetailTitle(void)
{
}

bool CContactDetailTitle::IsContactDetailTitle(CFrameListTitlePtr pTitle)
{
    if (NULL != pTitle && FRAME_LIST_TITLE_TYPE_CONTACT_DETAIL_TITLE == pTitle->GetType())
    {
        return true;
    }

    return false;
}

void CContactDetailTitle::SetBtnCount(int nCount)
{
    if (nCount > MAX_BTN_COUNT)
    {
        return;
    }

    m_btnBar.SetBtnCount(nCount);

    m_nBtnCount = 0;

    m_btnDial.SetRect(0, 0, 0, 0);
    m_btnMove.SetRect(0, 0, 0, 0);
    m_btnEdit.SetRect(0, 0, 0, 0);
    m_btnBlackList.SetRect(0, 0, 0, 0);
    m_btnDel.SetRect(0, 0, 0, 0);
#if IF_FEATURE_METASWITCH_DIRECTORY
    m_btnAddMtswContact.SetRect(0, 0, 0, 0);
#endif
#if IF_FEATURE_FAVORITE_DIR
    m_btnFavorite.SetRect(0, 0, 0, 0);
    m_btnRemoveFavorite.SetRect(0, 0, 0, 0);
#endif


    m_btnDial.ResetDown();
    m_btnMove.ResetDown();
    m_btnEdit.ResetDown();
    m_btnBlackList.ResetDown();
    m_btnDel.ResetDown();
#if IF_FEATURE_METASWITCH_DIRECTORY
    m_btnAddMtswContact.ResetDown();
#endif
#if IF_FEATURE_FAVORITE_DIR
    m_btnFavorite.ResetDown();
    m_btnRemoveFavorite.ResetDown();
#endif

    for (int i = 0; i < MAX_BTN_COUNT; ++i)
    {
        m_arrTitle[i] = "";
    }
}

void CContactDetailTitle::AddButton(int nAction, const QString & strTitle)
{
    if (m_nBtnCount >= MAX_BTN_COUNT)
    {
        return;
    }

    CTitleButton * pBtn = NULL;

    switch (nAction)
    {
    case LIST_TITLE_ACTION_DEL_CONTACT:
        {
            pBtn = &m_btnDel;
        }
        break;
    case LIST_TITLE_ACTION_MOVE_CONTACT:
        {
            pBtn = &m_btnMove;
        }
        break;
    case LIST_TITLE_ACTION_ADD_BLACKLIST:
        {
            pBtn = &m_btnBlackList;
        }
        break;
    case LIST_TITLE_ACTION_DIAL_CONTACT:
        {
            pBtn = &m_btnDial;
        }
        break;
    case LIST_TITLE_ACTION_EDIT_CALLLOG:
        {
            pBtn = &m_btnEdit;
        }
        break;
#if IF_FEATURE_METASWITCH_DIRECTORY
    case LIST_TITLE_ACTION_MOVE_MTSW_CONTACT:
        {
            pBtn = &m_btnAddMtswContact;
        }
        break;
#endif
#if IF_FEATURE_FAVORITE_DIR
    case LIST_TITLE_ACTION_ADD_FAVORITE:
        {
            pBtn = &m_btnFavorite;
        }
        break;
    case LIST_TITLE_ACTION_REMOVE_FAVORITE:
        {
            pBtn = &m_btnRemoveFavorite;
        }
        break;
#endif
    default:
        break;
    }

    if (NULL == pBtn)
    {
        return;
    }

    m_btnBar.SetBtn(m_nBtnCount, pBtn);
    m_arrTitle[m_nBtnCount] = strTitle;

    ++m_nBtnCount;
}

void CContactDetailTitle::mousePressEvent(QMouseEvent * pEvent)
{
    m_btnBar.ProcessMousePress(pEvent);
}

void CContactDetailTitle::mouseReleaseEvent(QMouseEvent * pEvent)
{
    CButtonWrapper * pBtn = m_btnBar.ProcessMouseRelease(pEvent);

    if (NULL == pBtn)
    {
        return;
    }

    if (pBtn == &m_btnDel)
    {
        DoAction(LIST_TITLE_ACTION_DEL_CONTACT);
    }
    else if (pBtn == &m_btnMove)
    {
        DoAction(LIST_TITLE_ACTION_MOVE_CONTACT);
    }
    else if (pBtn == &m_btnBlackList)
    {
        DoAction(LIST_TITLE_ACTION_ADD_BLACKLIST);
    }
    else if (pBtn == &m_btnDial)
    {
        DoAction(LIST_TITLE_ACTION_DIAL_CONTACT);
    }
    else if (pBtn == &m_btnEdit)
    {
        DoAction(LIST_TITLE_ACTION_EDIT_CALLLOG);
    }
#if IF_FEATURE_METASWITCH_DIRECTORY
    else if (pBtn == &m_btnAddMtswContact)
    {
        DoAction(LIST_TITLE_ACTION_MOVE_MTSW_CONTACT);
    }
#endif
#if IF_FEATURE_FAVORITE_DIR
    else if (pBtn == &m_btnFavorite)
    {
        if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1)
        {
            DoAction(LIST_TITLE_ACTION_ADD_FAVORITE);
        }
    }
    else if (pBtn == &m_btnRemoveFavorite)
    {
        if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1)
        {
            DoAction(LIST_TITLE_ACTION_REMOVE_FAVORITE);
        }
    }
#endif
}

void CContactDetailTitle::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);

    int nBtnCount = m_nBtnCount;

    if (nBtnCount > MAX_BTN_COUNT)
    {
        nBtnCount = MAX_BTN_COUNT;
    }

    for (int i = 0; i < nBtnCount; ++i)
    {
        CTitleButton * pBtn = (CTitleButton *)m_btnBar.GetBtn(i);

        if (NULL == pBtn)
        {
            continue;
        }

        // 画按钮
        const QString & strTitle = m_arrTitle[i];

        if (pBtn == &m_btnDial)
        {
            if (g_DirectoryMgr.GetIsInSelecting())
            {
                pBtn->PaintVButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_MODIFIED_GROUP_SELECT,
                                   LANG_TRANSLATE(TRID_SELECT));
                pBtn->PaintButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_DETAIL_BTN_CLICK);
            }
            else
            {
                pBtn->PaintVButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_DIAL, strTitle);
                pBtn->PaintButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_DETAIL_BTN_CLICK);
            }
        }
        else if (pBtn == &m_btnMove)
        {
            pBtn->PaintVButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_ADD_CONTACT, strTitle);
            pBtn->PaintButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_DETAIL_BTN_CLICK);
        }
        else if (pBtn == &m_btnEdit)
        {
            pBtn->PaintVButton(stylePainter, NULL, PIC_DIRECTORY_CALLLOG_EDIT, strTitle);
            pBtn->PaintButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_DETAIL_BTN_CLICK);
        }
        else if (pBtn == &m_btnBlackList)
        {
            pBtn->PaintVButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_ADD_BLACKLIST, strTitle);
            pBtn->PaintButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_DETAIL_BTN_CLICK);
        }
        else if (pBtn == &m_btnDel)
        {
            pBtn->PaintVButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_DEL_CONTACT, strTitle);
            pBtn->PaintButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_DETAIL_BTN_CLICK);
        }
#if IF_FEATURE_METASWITCH_DIRECTORY
        else if (pBtn == &m_btnAddMtswContact)
        {
            pBtn->PaintVButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_ADD_CONTACT, strTitle);
            pBtn->PaintButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_DETAIL_BTN_CLICK);
        }
#endif
#if IF_FEATURE_FAVORITE_DIR
        else if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1
                 && pBtn == &m_btnFavorite)
        {
            pBtn->PaintVButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_ADD_FAVORITE, strTitle);
            pBtn->PaintButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_DETAIL_BTN_CLICK);
        }
        else if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1
                 && pBtn == &m_btnRemoveFavorite)
        {
            pBtn->PaintVButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_REMOVE_FAVORITE, strTitle);
            pBtn->PaintButton(stylePainter, NULL, PIC_DIRECTORY_DIRECTORY_DETAIL_BTN_CLICK);
        }
#endif

        // 画分隔线
        if ((i + 1) != nBtnCount)
        {
            QRect rcBtn = pBtn->GetRect();
            QRect rcSeparate(rcBtn.right() + CONTACT_DETAIL_TITLE_BTN_SEPARATE,
                             CONTACT_DETAIL_TREE_TITLE_SEPARATE_TOP,
                             CONTACT_DETAIL_TREE_TITLE_SEPARATE_WIDTH, CONTACT_DETAIL_TREE_TITLE_SEPARATE_HEIGHT);

            QPixmap pmSeparate = THEME_GET_BMP(PIC_DIRECTORY_DIRECTORY_SEPARATE);

            if (!pmSeparate.isNull())
            {
                stylePainter.drawPixmap(rcSeparate, pmSeparate);
            }
        }
    }
}

void CContactDetailTitle::Relayout()
{
    QRect rcTitle = rect();
    int nTotalWidth = CONTACT_DETAIL_TITLE_BTN_COMMON_WIDTH * m_nBtnCount +
                      CONTACT_DETAIL_TITLE_BTN_SPACE * (m_nBtnCount - 1);
    int nLeft =  rcTitle.right() - CONTACT_DETAIL_TITLE_BTN_RIGHT - nTotalWidth;

    int nBtnCount = m_nBtnCount;

    if (nBtnCount > MAX_BTN_COUNT)
    {
        nBtnCount = MAX_BTN_COUNT;
    }

    for (int i = 0; i < nBtnCount; ++i)
    {
        CTitleButton * pBtn = (CTitleButton *)m_btnBar.GetBtn(i);

        if (NULL == pBtn)
        {
            continue;
        }

        pBtn->SetRect(nLeft, CONTACT_DETAIL_TITLE_BTN_COMMON_TOP,
                      CONTACT_DETAIL_TITLE_BTN_COMMON_WIDTH, CONTACT_DETAIL_TITLE_BTN_COMMON_HEIGHT);

        pBtn->SetIconTop(CONTACT_DETAIL_TITLE_ICON_COMMON_TOP);
        pBtn->SetTextTop(CONTACT_DETAIL_TITLE_TEXT_COMMON_TOP);

        nLeft += (CONTACT_DETAIL_TITLE_BTN_COMMON_WIDTH + CONTACT_DETAIL_TITLE_BTN_SPACE);
    }
}

#ifdef IF_ENABLE_TESTABILITY
QString CContactDetailTitle::GetTestInfo()
{
    xml_document doc;
    xml_node nodeRoot = doc.append_child("testinfo");

    AddBtnNode(nodeRoot, m_btnDial, BTN_DIAL, fromQString(m_btnDial.GetText()));
    AddBtnNode(nodeRoot, m_btnMove, BTN_MOVE, fromQString(m_btnMove.GetText()));
    AddBtnNode(nodeRoot, m_btnEdit, BTN_EDIT, fromQString(m_btnEdit.GetText()));
    AddBtnNode(nodeRoot, m_btnBlackList, BTN_BLACKLIST, fromQString(m_btnBlackList.GetText()));
#if IF_FEATURE_METASWITCH_DIRECTORY
    // Metaswitch按钮
    AddBtnNode(nodeRoot, m_btnAddMtswContact, BTN_ADD, fromQString(m_btnAddMtswContact.GetText()));
#endif
#if IF_FEATURE_FAVORITE_DIR
    if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1)
    {
        AddBtnNode(nodeRoot, m_btnFavorite, BTN_ADD, fromQString(m_btnFavorite.GetText()));
        AddBtnNode(nodeRoot, m_btnRemoveFavorite, BTN_ADD, fromQString(m_btnRemoveFavorite.GetText()));
    }
#endif

    AddBtnNode(nodeRoot, m_btnDel, BTN_DELETE, fromQString(m_btnDel.GetText()));

    QString strTestinfo;
    GetStringFromXml(doc, strTestinfo);
    return strTestinfo;
}
#endif