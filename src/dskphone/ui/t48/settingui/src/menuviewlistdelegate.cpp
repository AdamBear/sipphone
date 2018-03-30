#include "menuviewlistdelegate.h"
#include "setting/include/common.h"
#include "baseui/framelist.h"
#include "uimanager/rectdefine.h"
#include "qtcommon/qmisc.h"
#include "globalinputhook/ckeymap.h"
#include <QKeyEvent>
#include "baseui/t4xpicpath.h"
#include "dsklog/log.h"
#include "textmenuitem.h"
#include "uikernel/qwidgetutility.h"
#include "uikernel/languagehelper.h"

namespace
{
#define  SUBMENU_MAX_PAGE_ITEM_COUNT    7
#define  SUBMENU_LEFT_SPACING 17
#define  DEFAULT_SUBMENU_LIST_ITEM_SPACE    0
#define  MENUVIEW_ITEM_HEIGHT   48
#define  MENUVIEW_ITEM_FONT_SIZE    16

static const QRect MENUVIEW_GEOMETRY = QRect(0, 0, 255, 336);//(-2, -2, 257, 340);
static const QSize MENUVIEW_ITEM_SIZE(255, 48);
}

CMenuviewDelegate::CMenuviewDelegate(QObject * parent)
    : QObject(parent)
    , CFrameListDelegate()
    , m_iPreSelectIndex(-1)
    , m_iCurrentIndex(-1)
{
    init();
}

void CMenuviewDelegate::init()
{
    m_vecItem.clear();
    for (int i = 0; i < SUBMENU_MAX_PAGE_ITEM_COUNT; i++)
    {
        CTextMenuItem * pItem = new CTextMenuItem();
        if (pItem != NULL)
        {
            pItem->setObjectName(QString("TextMenuItem%1").arg(i));
            pItem->SetChangeColorOnPress(true);
            pItem->setFixedHeight(MENUVIEW_ITEM_HEIGHT);
            qWidgetUtility::setFontStyle(pItem, MENUVIEW_ITEM_FONT_SIZE);

            pItem->CListItem::SetAction(this);
        }

        m_vecItem.push_back(pItem);
    }
}

void CMenuviewDelegate::ClearAllContent()
{
    if (NULL != m_pFrameList)
    {
        m_pFrameList->DetachAllItem();
    }

    ClearListData();
    ResetIndex();

    m_iCurrentIndex = -1;
    m_iPreSelectIndex = -1;
}

void CMenuviewDelegate::InitFrameList()
{
    if (NULL != m_pFrameList)
    {
        m_pFrameList->setFocusPolicy(Qt::NoFocus);
        m_pFrameList->setGeometry(MENUVIEW_GEOMETRY);
        m_pFrameList->SetDefaultFocus(false);
        m_pFrameList->SetMaxPageItemCount(SUBMENU_MAX_PAGE_ITEM_COUNT);
        m_pFrameList->SetItemMargin(0, 0, 0);
        m_pFrameList->SetContentPos(SUBMENU_LEFT_SPACING, 0);
#ifdef FL_SUPPORT_CUSTOM_FRAME_STYLE
        m_pFrameList->SetGridLine(true);
        m_pFrameList->SetLinePosY(DEFAULT_LEFT_LIST_ITEM_SPACE);
        m_pFrameList->SetLineHeight(DEFAULT_LEFT_LIST_ITEM_SPACE);
        m_pFrameList->SetItemSpace(DEFAULT_LEFT_LIST_ITEM_SPACE);
        m_pFrameList->SetLineImage(PIC_GENERAL_LIST_ITEM_LEFT_LINE);
        m_pFrameList->SetItemClickBG(PIC_SETTING_LIST_SELECT_TOP, PIC_SETTING_LIST_SELECT_MIDDLE,
                                     PIC_SETTING_LIST_SELECT_BOTTOM);
        m_pFrameList->SetItemSelectBG(PIC_SETTING_LIST_SELECT_TOP, PIC_SETTING_LIST_SELECT_MIDDLE,
                                      PIC_SETTING_LIST_SELECT_BOTTOM);
#endif
        m_pFrameList->SetItemSpace(DEFAULT_SUBMENU_LIST_ITEM_SPACE);
    }
}

void CMenuviewDelegate::ClearListData()
{
    m_listData.clear();
}

bool CMenuviewDelegate::SetContent(MenuItemListData * pListData)
{
    if (NULL == pListData)
    {
        return false;
    }

    MenuItemList * pItemList = pListData->m_pMenuItemList;
    if (NULL == pItemList)
    {
        return false;
    }

    ClearListData();

    MenuItemListIter iter = pItemList->begin();
    for (; iter != pItemList->end(); ++iter)
    {
        if (NULL != *iter)
        {
            m_listData.push_back(**iter);
        }
    }

    return true;
}

void CMenuviewDelegate::SelectByIndex(int iDataIndex, bool bEnter/* = true*/)
{
    if (iDataIndex < 0 &&  iDataIndex != INVALID_ITEM_INDEX)
    {
        return;
    }

    if (NULL != m_pFrameList)
    {
        SetPageDataIndex(iDataIndex);
        SetDefaultFocus(true);
        if (bEnter)
        {
            SetFocusIndex(iDataIndex);
        }

        if (m_pFrameList->isVisible())
        {
            RefreshFrameList();
        }
    }
}

void CMenuviewDelegate::SetCurrentItem(const QString & strItemClickAction, bool bEnter/* = true*/)
{
    if (m_listData.size() > 0)
    {
        for (int i = 0; i < m_listData.size(); i++)
        {
            MenuItemData itemData =  m_listData.at(i);
            if (strItemClickAction == toQString(itemData.m_strClickAction))
            {
                SelectByIndex(i, bEnter);
                return;
            }
        }
    }
}

int CMenuviewDelegate::GetCurrentIndex()
{
    if (NULL != m_pFrameList)
    {
        return m_pFrameList->GetFocusItemDataIndex();
    }

    return -1;
}

QString CMenuviewDelegate::GetCurrentAction()
{
    if (NULL != m_pFrameList)
    {
        int iDataIndex = m_pFrameList->GetFocusItemDataIndex();
        if (iDataIndex >= 0 && iDataIndex < m_listData.size())
        {
            MenuItemData itemData =  m_listData.at(iDataIndex);
            return toQString(itemData.m_strClickAction);
        }
    }

    return "";
}

void CMenuviewDelegate::OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount)
{
    if (m_pFrameList && m_listData.size() >= nDataIndex)
    {
        //先清空当前数据
        m_pFrameList->DetachAllItem(false);

        m_pFrameList->SetItemAttach(); //delegate托管数据，不删除

        m_pFrameList->SetTotalDataCount(m_listData.size());

        int iPageItemCount = m_pFrameList->GetMaxPageItemCount();

        int iShowCount = 0;
        //重新添加数据
        for (int i = nDataIndex; i < nDataIndex + nDataCount && i < m_listData.size()
                && iShowCount < iPageItemCount; i++, iShowCount++)
        {
            CTextMenuItem * pItem = m_vecItem.at(iShowCount);
            if (NULL == pItem)
            {
                continue;
            }

            MenuItemData itemData = m_listData.at(i);
            pItem->SetMenu(LANG_TRANSLATE(itemData.m_strDisplayName.c_str()));
            pItem->SetDataIndex(i);
            pItem->SetFocus(false);

            m_pFrameList->AddItem(pItem);

            if (i == m_iCurrentIndex)
            {
                m_pFrameList->FocusItemByDataIndex(m_iCurrentIndex);
            }
        }

        //隐藏不显示的item
        for (int iHideIndex = iShowCount; iHideIndex < m_vecItem.size(); iHideIndex++)
        {
            CTextMenuItem * pItem = m_vecItem.at(iHideIndex);
            if (NULL == pItem)
            {
                continue;
            }
            pItem->hide();
        }
    }
}

void CMenuviewDelegate::OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction)
{
    if (pFrameList != m_pFrameList)
    {
        return;
    }

    if (nAction ==  LIST_ITEM_ACTION_FOCUS || nAction ==  LIST_ITEM_ACTION_SELECT)
    {
        SetFocusIndex(m_pFrameList->GetFocusItemIndex());

        if (m_pFrameList->GetFocusItemDataIndex() != m_iCurrentIndex)
        {
            m_iPreSelectIndex = m_iCurrentIndex;
            m_iCurrentIndex = pItem->GetDataIndex();

            emit ProcessCurrentItem(GetCurrentAction());
        }

        return;
    }

    return CFrameListDelegate::OnItemAction(pFrameList, pItem, nAction);
}

void CMenuviewDelegate::GoNextPage()
{
    if (NULL == m_pFrameList)
    {
        return;
    }

    int iPageItemCount = m_pFrameList->GetMaxPageItemCount();
    int iTotalDataCount = m_pFrameList->GetTotalDataCount();

    if (iPageItemCount < 1 || iTotalDataCount <= iPageItemCount)
    {
        return;
    }

    CListItem * pItem =  m_pFrameList->GetItemByIndex(0);
    if (NULL == pItem)
    {
        return;
    }

    int iFirstIndex = pItem->GetDataIndex();

    if (iFirstIndex + iPageItemCount > iTotalDataCount)
    {
        m_pFrameList->RefreshData(0);
    }
    else
    {
        m_pFrameList->RefreshData(iFirstIndex + iPageItemCount);
    }
}

void CMenuviewDelegate::GoPrePage()
{
    if (NULL == m_pFrameList)
    {
        return;
    }

    int iPageItemCount = m_pFrameList->GetMaxPageItemCount();
    int iTotalDataCount = m_pFrameList->GetTotalDataCount();

    if (iPageItemCount < 1 || iTotalDataCount <= iPageItemCount)
    {
        return;
    }

    CListItem * pItem =  m_pFrameList->GetItemByIndex(0);
    if (NULL == pItem)
    {
        return;
    }

    int iFirstIndex = pItem->GetDataIndex();

    if (iFirstIndex > 0)
    {
        m_pFrameList->RefreshData(iFirstIndex - iPageItemCount);
    }
    else if (iFirstIndex == 0)
    {
        int iPageCount = (iTotalDataCount - 1) / iPageItemCount + 1;
        int iLastPageFirstIndex = (iPageCount - 1) * iPageItemCount;
        m_pFrameList->RefreshData(iLastPageFirstIndex);
    }
}

bool CMenuviewDelegate::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (pEvent->type() == QEvent::KeyPress)
    {
        QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;
        if (pKeyEvent->key() == PHONE_KEY_LEFT)
        {
            if (NULL != m_pFrameList)
            {
                GoPrePage();
                return true;
            }
        }

        if (pKeyEvent->key() == PHONE_KEY_RIGHT)
        {
            if (NULL != m_pFrameList)
            {
                GoNextPage();
                return true;
            }
        }
    }

    if (NULL != m_pFrameList)
    {
        return m_pFrameList->eventFilter(pObject, pEvent);
    }

    return false;
}

void CMenuviewDelegate::RollackPreItem()
{
    if (NULL != m_pFrameList)
    {
        m_pFrameList->SelectItemByDataIndex(m_iPreSelectIndex);
    }
}

void CMenuviewDelegate::RefreshFrameList()
{
    CFrameListDelegateBase::RefreshFrameList();

    SetDefaultFocus(false);
}

