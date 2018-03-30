#include "msgcommondelegate.h"
#include "setting/include/modsetting.h"
#include "inputmethod/inputmethod.h"
#include "keymap.h"
#include "uikernel/languagehelper.h"
#include "qtcommon/qmisc.h"
#include "settingui/src/settingitemfactory.h"
#include "baseui/framelistdefine.h"
#include "uicommon/uikernel/qcomboboxutility.h"
#include "uimanager/rectdefine.h"
#include "baselibs/include/configparser/modconfigparser.h"
#include <configiddefine.h>
#include "baseui/fontcolor.h"
#include "messagelistitem.h"
#include "baseui/t4xpicpath.h"

#include "settingui/src/bluetoothitem.h"
#include "settingui/src/settingitembase.h"
#include "messagelistitem.h"

#include "dsklog/log.h"

namespace
{
#define PAGE_ITEM_COUNT                 7

#define SETTING_ITEM_HEIGHT             48
#define SETTING_FRAME_LEFT_SPACING      17
#define SETTING_FRAME_ITEM_LEFT_MARGIN  0
#define SUBPAGE_MAX_PAGE_ITEM_COUNT     7
#define MSGUI_DEFAULT_LEFT_LIST_ITEM_SPACE  0
#define SETTING_FRAM_LIST_RECT_TOP      40//3

static QSize SETTING_SUBPAGE_SIZE(536, 336);
}

MsgCommonDelegate::MsgCommonDelegate()
    : m_pFramelist(NULL)
    , m_pItemParent(NULL)
    , m_pDataDelegate(NULL)
    , m_iCurrentIndex(0)
    , m_bIsEditPage(false)
    , m_iConfigedIndex(-1)
#ifdef LOAD_ALL_DATA_FROM_LOGIC
    , m_bView(false)
    , m_bDel(false)
#endif
{
}

MsgCommonDelegate::MsgCommonDelegate(CFrameList * pList)
    : m_pFramelist(NULL)
    , m_pItemParent(NULL)
    , m_pDataDelegate(NULL)
    , m_iCurrentIndex(0)
    , m_bIsEditPage(false)
    , m_iConfigedIndex(-1)
#ifdef LOAD_ALL_DATA_FROM_LOGIC
    , m_bView(false)
    , m_bDel(false)
#endif
{
    BindFramelist(pList);
}

MsgCommonDelegate::~MsgCommonDelegate()
{
    DelWgtOnlg();
    m_vecItem.clear();

    m_pFramelist = NULL;
    m_pDataDelegate = NULL;
}

void MsgCommonDelegate::BindDataDelegate(DataDelegate * pDatad)
{
    if (NULL != m_pDataDelegate)
    {
        m_pDataDelegate = NULL;
    }
    m_pDataDelegate = pDatad;
}

void MsgCommonDelegate::UnBindDataDelegate()
{
    m_pDataDelegate = NULL;
}

bool MsgCommonDelegate::IsFrameListAttach(CFrameListBase * pFrameList)
{
    if (NULL == pFrameList || pFrameList != m_pFramelist)
    {
        return false;
    }

    return true;
}

void MsgCommonDelegate::SaveListStatus()
{
    if (NULL != m_pFramelist)
    {
        //记录当前行
        m_iCurrentIndex = m_pFramelist->GetFocusItemIndex();

        //提交预编辑字符
        std::vector<CSettingItem *>::iterator iter = m_vecItem.begin();
        for (; iter != m_vecItem.end(); ++iter)
        {
            CSettingItem * pItem = (*iter);
            if (pItem != NULL)
            {
                QWidget * pWgt = pItem->ContentWgt();
                if (pWgt != NULL && (pWgt->inherits("QLineEdit") || pWgt->inherits("QTextEdit")))
                {
                    if (InputPreeditContent(pWgt))
                    {
                        // 一个界面只有一个输入框处于预编辑状态
                        break;
                    }
                }
            }
        }
    }
}

void MsgCommonDelegate::RestoreListStatus()
{
    UpdateList();
}

void MsgCommonDelegate::BindFramelist(CFrameList * pList)
{
    SaveListStatus();
    if (NULL != pList)
    {
        m_pFramelist = pList;
        m_pFramelist ->SetDelegate(this);
        //SetFrameList(pList);
    }
}

void MsgCommonDelegate::UnBindFramelist()
{
    m_pFramelist = NULL;
}

void MsgCommonDelegate::SetItemParent(QWidget * parent)
{
    m_pItemParent = parent;
}

void MsgCommonDelegate::SaveListStatusAndUnBindFramelist()
{
    SaveListStatus();
    UnBindFramelist();
}

void MsgCommonDelegate::InitSettingListStyle()
{
    if (NULL != m_pFramelist)
    {
        m_pFramelist->setFocusPolicy(Qt::StrongFocus);
        m_pFramelist->setGeometry(0, SETTING_FRAM_LIST_RECT_TOP, RECT_SETTING.width(),
                                  RECT_SETTING.height() - 3 * 2);
        m_pFramelist->SetDefaultFocus(true);
        // m_pFramelist->SetItemFontSize(16, QFont::Normal);
        m_pFramelist->SetMaxPageItemCount(SUBPAGE_MAX_PAGE_ITEM_COUNT);
        m_pFramelist->SetItemMargin(SETTING_FRAME_ITEM_LEFT_MARGIN, 0, 0);
        m_pFramelist->SetContentPos(SETTING_FRAME_LEFT_SPACING, 16);
#ifdef FL_SUPPORT_CUSTOM_FRAME_STYLE
        m_pFramelist->SetGridLine(true, true);  //t46背景图片画线
#endif
        m_pFramelist->SetItemSpace(MSGUI_DEFAULT_LEFT_LIST_ITEM_SPACE);
        m_pFramelist->SetNote(LANG_TRANSLATE(TRID_NONE));
        m_pFramelist->SetNoteFontColor(THEME_TEXT_COLOR);
    }
}

//根据item的内容控件找到对应的item
CSettingItem * MsgCommonDelegate::FindItemByContentWgt(QWidget * pWidget)
{
    std::vector<CSettingItem *>::iterator iter = m_vecItem.begin();
    for (; iter != m_vecItem.end(); ++iter)
    {
        CSettingItem * pItem = (*iter);
        if (pItem != NULL)
        {
            if (pItem->ContentWgt() != NULL && pItem->ContentWgt() == pWidget)
            {
                return pItem;
            }
        }
    }

    return NULL;
}

void MsgCommonDelegate::SelectItemByContentWgt(QWidget * pWidget)
{
    if (NULL == pWidget)
    {
        return;
    }

    std::vector<CSettingItem *>::iterator iter = m_vecItem.begin();
    for (; iter != m_vecItem.end(); ++iter)
    {
        CSettingItem * pItem = (*iter);
        if (pItem != NULL && pItem->ContentWgt() == pWidget)
        {
            m_pFramelist->SelectItemByDataIndex(pItem->GetDataIndex());
            return;
        }
    }
}

void MsgCommonDelegate::SetErrorFocus(QLineEdit * pLineEdit)
{
    if (pLineEdit != NULL)
    {
        if (m_pFramelist == NULL)
        {
            return;
        }

        CSettingItem * pItem = FindItemByContentWgt(pLineEdit);
        if (pItem != NULL)
        {
            int iDataIndex = pItem->GetDataIndex();
            if (m_pFramelist->GetItemByDataIndex(iDataIndex) == NULL)
            {
                //翻到错误控件所在页
                int iItemPerPage = m_pFramelist->GetMaxPageItemCount();
                if (iItemPerPage <= 0)
                {
                    return;
                }
                int iFirstIndex = iDataIndex;
                if (iFirstIndex % iItemPerPage != 0)
                {
                    iFirstIndex = (iDataIndex / iItemPerPage) * iItemPerPage;
                }
                m_pFramelist->RefreshData(iFirstIndex);
            }

            //选中所在行
            m_pFramelist->SelectItemByDataIndex(pItem->GetDataIndex());

            //设置焦点
            pLineEdit->setFocus();
            pLineEdit->end(false);
        }
    }
}

void MsgCommonDelegate::SelectIndex(int nIndex/* = 0*/, bool bEnter/* = false*/)
{
    if (nIndex < 0 || NULL == m_pFramelist || nIndex >= m_pFramelist->GetTotalDataCount())
    {
        return;
    }

    if (!m_pFramelist->isVisible())
    {
        m_iCurrentIndex = nIndex;
        return;
    }

    int iPageItemCount = m_pFramelist->GetMaxPageItemCount();
    if (iPageItemCount <= 0)
    {
        return;
    }
    if (NULL == m_pFramelist->GetItemByDataIndex(nIndex))
    {
        m_pFramelist->RefreshData((nIndex / iPageItemCount) * iPageItemCount);
    }

    m_iCurrentIndex = nIndex;

    if (bEnter)
    {
        m_pFramelist->SelectItemByDataIndex(nIndex);
    }
    else
    {
        m_pFramelist->FocusItemByDataIndex(nIndex);
    }
}

//重新给当前行的控件设置焦点
void MsgCommonDelegate::RefreshFocus()
{
    if (m_pFramelist != NULL && m_pFramelist->GetTotalItemCount() > 0)
    {
        CListItemPtr pItem = m_pFramelist->GetFocusItem();
        if (NULL != pItem)
        {
            int IfocusInex = pItem->GetDataIndex();
            m_pFramelist->FocusItemByDataIndex(IfocusInex);
        }
        else
        {
            m_pFramelist->setFocus();
        }
    }
}

// 刷新控件状态 : 编辑属性
void MsgCommonDelegate::UpdateItemStatus(bool bEanbled)
{
    SettingItemList::iterator itr = m_vecItem.begin();
    for (; itr != m_vecItem.end(); ++itr)
    {
        if (NULL == (*itr))
        {
            continue ;
        }

        if (bEanbled && (*itr)->isEnabled())
        {
            (*itr)->SetItemEnabled(true);
        }
        else
        {
            (*itr)->SetItemEnabled(false);
        }
    }
}

void MsgCommonDelegate::UpdateList()
{
    if (m_pFramelist != NULL)
    {
        m_pFramelist->setFocus(Qt::ActiveWindowFocusReason);//页面捕获去焦点，焦点始终给list
        int iPageItemCount = m_pFramelist->GetMaxPageItemCount();
        int iDataIdx = m_pFramelist->GetFocusItemDataIndex();
        MESSAGEUI_INFO("UpdateList dataIdx[%d], size[%d]"
                       , iDataIdx, m_pFramelist->GetTotalDataCount());
#ifdef LOAD_ALL_DATA_FROM_LOGIC
        MESSAGEUI_INFO("UpdateList dataIdx[%d], size[%d], del[%d], view[%d]"
                       , iDataIdx, m_pFramelist->GetTotalDataCount(), m_bDel, m_bView);
        // deal msg view unread msg and del msg
        if (m_bDel)
        {
            if (0 <= iDataIdx && iDataIdx < iPageItemCount)
            {
                m_pFramelist->DelItemByDataIndex(iDataIdx);
            }
            if (0 <= iDataIdx && iDataIdx < m_vecData.size())
            {
                m_vecData.remove(iDataIdx);
            }
            MESSAGEUI_INFO("------------------ after delete dataIdx[%d], size[%d], pageCnt[%d] --------"
                           , iDataIdx, m_vecData.size(), iPageItemCount);
            if (iDataIdx == m_vecData.size() && 0 < iDataIdx && (0 == iDataIdx % iPageItemCount))
            {
                --iDataIdx;
                bResetF = true;
                m_pFramelist->SetItemFocusByIndex(iPageItemCount - 1);
            }
            else if (0 < iDataIdx && (iDataIdx % iPageItemCount > 0))
            {
                --iDataIdx;
                m_pFramelist->SetItemFocusByIndex(iDataIdx % iPageItemCount);
            }
        }
        else if (m_bView)
        {
            if (m_bView && iDataIdx < m_vecData.size() && iDataIdx >= 0)
            {
                if (0 == m_vecData[iDataIdx].strIcon.compare(PIC_GENERAL_MESSAGE_UNREAD))
                {
                    m_vecData[iDataIdx].strIcon = PIC_GENERAL_MESSAGE_READ;
                }
            }
        }

        if (m_pFramelist->GetTotalItemCount() == 0 || iDataIdx < iPageItemCount
                || m_vecData.size() <= iDataIdx)
        {
            //第一次进入页面时加载第一页
            m_pFramelist->RefreshData(0);
            return;
        }

        if (iPageItemCount <= 0)
        {
            return;
        }

        //重新回到这个页面时刷新到之前浏览的页
        m_pFramelist->RefreshData((iDataIdx / iPageItemCount) * iPageItemCount);
        if (bResetF)
        {
            m_pFramelist->SetItemFocusByIndex(iPageItemCount - 1);
        }

        m_bView = false;
        m_bDel = false;
#else
        if (m_pFramelist->GetTotalItemCount() == 0 || iDataIdx < iPageItemCount
                || m_pFramelist->GetTotalDataCount() <= iDataIdx)
        {
            //第一次进入页面时加载第一页
            m_pFramelist->RefreshData(0);
            return;
        }

        if (iPageItemCount <= 0)
        {
            return;
        }
        MESSAGEUI_INFO("not 0, FocusIdx[%d]", iDataIdx);
        //重新回到这个页面时刷新到之前浏览的页
        m_pFramelist->RefreshData((iDataIdx / iPageItemCount) * iPageItemCount);
#endif
    }
}

void MsgCommonDelegate::DelWgtOnlg()
{
    std::vector<CSettingItem *>::iterator iter = m_vecItem.begin();
    for (; iter != m_vecItem.end(); ++iter)
    {
        CSettingItem * pItem = (*iter);
        if (pItem != NULL)
        {
            if (pItem->ContentWgt() != NULL)
            {
                pItem->ContentWgt()->setParent(NULL);
                pItem->ContentWgt()->deleteLater();
                pItem->SetContentWgt(NULL);
            }
        }
    }
    m_vecData.clear();
}


void MsgCommonDelegate::ClearContent(bool bDeleteContenWgt, bool bDelWgtOnlg /*= false*/)
{
    HideAllItem();
    if (bDelWgtOnlg)
    {
        DelWgtOnlg();
        return;
    }

    m_pFramelist->DetachAllItem();
    std::vector<CSettingItem *>::iterator iter = m_vecItem.begin();
    for (; iter != m_vecItem.end(); ++iter)
    {
        CSettingItem * pItem = (*iter);
        if (pItem != NULL)
        {
            if (pItem->ContentWgt() != NULL)
            {
                if (bDeleteContenWgt)
                {
                    pItem->ContentWgt()->setParent(NULL);
                    pItem->ContentWgt()->deleteLater();
                    pItem->SetContentWgt(NULL);
                }
                else
                {
                    pItem->ContentWgt()->setParent(m_pItemParent);
                }
            }
            (*iter)->deleteLater();
            (*iter) = NULL;
        }
    }
    m_vecItem.clear();
    m_iCurrentIndex = 0;
    m_iConfigedIndex = -1;
    m_pFramelist->ClearContent();
}

void MsgCommonDelegate::DelItemByDataIdx(int iDataIdx, bool bUpdate /*= false*/)
{
#ifdef LOAD_ALL_DATA_FROM_LOGIC
    m_pFramelist->DelItemByDataIndex(iDataIdx, bUpdate);
    if (iDataIdx < m_vecData.size())
    {
        m_vecData.remove(iDataIdx);
    }
    int iMaxPageItem = m_pFramelist->GetMaxPageItemCount();
    if (iDataIdx == m_vecData.size() && 0 < iDataIdx && (0 == iDataIdx % iMaxPageItem))
    {
        --iDataIdx;
        m_pFramelist->RefreshData(iDataIdx / iMaxPageItem * iMaxPageItem);
        m_pFramelist->SetItemFocusByIndex(iMaxPageItem - 1);
    }
    else
    {
        m_pFramelist->RefreshData(iDataIdx / iMaxPageItem * iMaxPageItem);
    }
#else
    m_pFramelist->DelItemByDataIndex(iDataIdx, bUpdate);
#endif
}

void MsgCommonDelegate::SetCurrentIndex(int iIndex)
{
    m_iCurrentIndex = iIndex;
}

int MsgCommonDelegate::GetCurrentIndex()
{
    return m_iCurrentIndex;
}

bool MsgCommonDelegate::ProcessKeyPress(int keyCode)
{
    if (NULL == m_pFramelist)
    {
        return false;
    }

    switch (keyCode)
    {
    case PHONE_KEY_LEFT:
    {
        QWidget * pFocusWidget = m_pFramelist->focusWidget();
        if (pFocusWidget != NULL
                && pFocusWidget->inherits("QComboBox"))
        {
            //焦点在下拉框时，屏蔽右键的作用
            QComboBox * pCmbBox = (QComboBox *)pFocusWidget;
            qComboBoxUtility::SwitchItem(pCmbBox, true);
            return true;
        }
        if (pFocusWidget != NULL
                && pFocusWidget->inherits("QLineEdit"))
        {
            QLineEdit * pLineEdit = (QLineEdit *)pFocusWidget;

            if (pLineEdit != NULL
                    && !pLineEdit->isReadOnly())
            {
                // 判断是否选中所有文字
                if (pLineEdit->selectedText() == pLineEdit->text())
                {
                    // 设置光标到最前方
                    pLineEdit->setCursorPosition(0);
                    return true;
                }
            }
        }
    }
    break;
    case PHONE_KEY_RIGHT:
    {
        QWidget * pFocusWidget = m_pFramelist->focusWidget();
        if (pFocusWidget != NULL
                && pFocusWidget->inherits("QComboBox"))
        {
            //焦点在下拉框时，屏蔽右键的作用
            QComboBox * pCmbBox = (QComboBox *)pFocusWidget;
            qComboBoxUtility::SwitchItem(pCmbBox, false);
            return true;
        }
        if (pFocusWidget != NULL
                && pFocusWidget->inherits("QLineEdit"))
        {
            QLineEdit * pLineEdit = (QLineEdit *)pFocusWidget;

            if (pLineEdit != NULL
                    && !pLineEdit->isReadOnly())
            {
                // 判断是否选中所有文字
                if (pLineEdit->selectedText() == pLineEdit->text())
                {
                    // 设置光标到最前方
                    pLineEdit->end(false);
                    return true;
                }
            }
        }
    }
    break;
    case PHONE_KEY_1:
    case PHONE_KEY_2:
    case PHONE_KEY_3:
    case PHONE_KEY_4:
    case PHONE_KEY_5:
    case PHONE_KEY_6:
    case PHONE_KEY_7:
    case PHONE_KEY_8:
    case PHONE_KEY_9:
    {
        QWidget * pFocusWidget = m_pFramelist->focusWidget();
        if (pFocusWidget != NULL
                && pFocusWidget->inherits("QLineEdit"))
        {
            QLineEdit * pLineEdit = (QLineEdit *)pFocusWidget;

            // 判断是否有选中文字
            if (pLineEdit != NULL
                    && !pLineEdit->isReadOnly()
                    && pLineEdit->hasSelectedText())
            {
                // 删除选中文字
                pLineEdit->del();
                //继续相应的输入
                return false;
            }
        }

        if (pFocusWidget != NULL)
        {
            //如果当前焦点控件中QLineEdit，且可编辑状态，不启用按数据键选中项功能
            if (pFocusWidget->inherits("QLineEdit"))
            {
                QLineEdit * pLeWd = static_cast<QLineEdit *>(pFocusWidget);
                if (!pLeWd->isReadOnly())
                {
                    break;
                }
            }
            //如果当前焦点控件中QTextEdit，且可编辑状态，不启用按数据键选中项功能
            else if (pFocusWidget->inherits("QTextEdit"))
            {
                QTextEdit * pLeWd = static_cast<QTextEdit *>(pFocusWidget);
                if (!pLeWd->isReadOnly())
                {
                    break;
                }
            }
        }

        SelectIndex(keyCode - PHONE_KEY_1, true);
        return true;
    }
    break;
    case PHONE_KEY_UP:
    {
        m_pFramelist->OnNotifyScrollPrevious();
        return true;
    }
    break;
    case PHONE_KEY_DOWN:
    {
        m_pFramelist->OnNotifyScrollNext();
        return true;
    }
    break;
    default:
        break;
    }
    return false;
}

bool MsgCommonDelegate::CheckCanReused(int iStDataIdx)
{
#ifndef LOAD_ALL_DATA_FROM_LOGIC
    iStDataIdx = 0;
#endif
    if (0 == m_vecItem.size() || m_vecItem.size() < m_vecData.size())
    {
        return false;
    }
    bool bCan = true;
    for (int i = 0; i < m_vecItem.size() && i + iStDataIdx < m_vecData.size(); ++i)
    {
        switch (m_vecData[i + iStDataIdx].eType)
        {
        case SETTING_ITEM_NORMAL:
        {
            bCan = bCan && (NULL != m_vecItem[i]) && (m_vecItem[i]->inherits("CSettingItem"));
            bool bC = false;
            bC = bC || ((NULL != m_vecItem[i]) && (m_vecItem[i]->inherits("CSettingMenuItem")));
            bC = bC || ((NULL != m_vecItem[i]) && (m_vecItem[i]->inherits("CSettingRadioItem")));
            bC = bC || ((NULL != m_vecItem[i]) && (m_vecItem[i]->inherits("CSettingBluetoothItem")));
            bC = bC || ((NULL != m_vecItem[i]) && (m_vecItem[i]->inherits("CSettingBluetoothDeviceListItem")));
            bC = bC || ((NULL != m_vecItem[i]) && (m_vecItem[i]->inherits("CSettingT46BluetoothItem")));
            bC = bC || ((NULL != m_vecItem[i]) && (m_vecItem[i]->inherits("CMessageListItem")));
            if (bC)
            {
                return false;
            }
        }
        break;
        case SETTING_ITEM_MENU:
        {
            bCan = bCan && (NULL != m_vecItem[i]) && (m_vecItem[i]->inherits("CSettingMenuItem"));
            if (!bCan)
            {
                return bCan;
            }
        }
        break;
        case SETTING_ITEM_RIDIO:
        {
            bCan = bCan && (NULL != m_vecItem[i]) && (m_vecItem[i]->inherits("CSettingRadioItem"));
            if (!bCan)
            {
                return bCan;
            }
        }
        break;
        case SETTING_ITEM_BLUETOOTH:
        {
            bCan = bCan && (NULL != m_vecItem[i]) && (m_vecItem[i]->inherits("CSettingBluetoothItem"));
            if (!bCan)
            {
                return bCan;
            }
        }
        break;
        case SETTING_ITEM_BLUETOOTHDEVICELIST:
        {
            bCan = bCan && (NULL != m_vecItem[i])
                   && (m_vecItem[i]->inherits("CSettingBluetoothDeviceListItem"));
            if (!bCan)
            {
                return bCan;
            }
        }
        break;
        case MESSAGE_LIST_ITEM:
        {
            bCan = bCan && (NULL != m_vecItem[i]) && (m_vecItem[i]->inherits("CMessageListItem"));
            if (!bCan)
            {
                return bCan;
            }
        }
        break;
        default:
        {
            bCan = false;
        }
        }
    }
    return bCan;
}

void MsgCommonDelegate::SetContent(QWPVector * pVecWP)
{
    if (NULL == pVecWP)
    {
        return;
    }
#ifdef LOAD_ALL_DATA_FROM_LOGIC
    m_vecData.clear();
    m_vecData = *pVecWP;

    int iPageItemCount = (NULL == m_pFramelist) ? PAGE_ITEM_COUNT : m_pFramelist->GetMaxPageItemCount();
    int iIdx = 0;

    // check item can reused or not
    if (CheckCanReused(0))
    {
        iIdx = m_vecItem.size();
    }
    else
    {
        // del all item
        ClearContent(true);
    }

    for (; iIdx < iPageItemCount && iIdx < m_vecData.size(); ++iIdx)
    {
        //创建Item
        CSettingItem * pItem = g_SettingItemManger->CreateItemByType(m_vecData[iIdx].eType, m_pItemParent,
                               m_vecData[iIdx]);
        ((CListItemPtr)pItem)->SetAction(this);
        if (NULL != pItem)
        {
            pItem->setObjectName(QString("CSettingItem%1").arg(iIdx));
            pItem->hide();
            pItem->setFixedHeight(SETTING_ITEM_HEIGHT);
            m_vecItem.push_back(pItem);
        }
    }
#else
    //m_vecData.clear();
    ClearContent(false, true);
    if (NULL == m_pDataDelegate)
    {
        return;
    }
    int iPage = 0;
    int iTot = 0;
    if (!m_pDataDelegate->GetData(iPage, iTot, &m_vecData))
    {
        MESSAGEUI_INFO("GetMsgData fail");
        return;
    }

    int iPageItemCount = (NULL == m_pFramelist) ? PAGE_ITEM_COUNT : m_pFramelist->GetMaxPageItemCount();
    int iIdx = 0;

    // check item can reused or not
    if (CheckCanReused(0))
    {
        MESSAGEUI_INFO("item can be reused");
        iIdx = m_vecItem.size();
    }
    else
    {
        MESSAGEUI_INFO("item can't be reused");
        // del all item
        ClearContent(true);
        //ClearContent(false);
    }

    for (int i = 0; i < iIdx && i < m_vecData.size(); ++i)
    {
        if (m_vecItem[i] != NULL)
        {
            ((CListItemPtr)m_vecItem[i])->SetDataIndex(i);

            if (m_vecData[i].second != NULL)
            {
                m_vecData[i].second->setParent(m_vecItem[i]);
                m_vecItem[i]->SetContentWgt(m_vecData[i].second);
            }
        }
    }

    for (; iIdx < iPageItemCount && iIdx < m_vecData.size(); ++iIdx)
    {
        //创建Item
        CSettingItem * pItem = g_SettingItemManger->CreateItemByType(m_vecData[iIdx].eType, m_pItemParent,
                               m_vecData[iIdx]);
        ((CListItemPtr)pItem)->SetAction(this);
        if (NULL != pItem)
        {
            pItem->setObjectName(QString("CSettingItem%1").arg(iIdx));
            pItem->hide();
            pItem->setFixedHeight(SETTING_ITEM_HEIGHT);
            m_vecItem.push_back(pItem);
        }
    }
#endif
}

void MsgCommonDelegate::HideAllItem()
{
    for (int i = 0; i < m_vecItem.size(); i++)
    {
        if (m_vecItem[i] != NULL)
        {
//#ifdef LOAD_ALL_DATA_FROM_LOGIC
            if (NULL != m_vecItem[i]->ContentWgt())
            {
                m_vecItem[i]->ContentWgt()->setParent(m_pItemParent);
                m_vecItem[i]->ContentWgt()->hide();
            }
//#endif
            m_vecItem[i]->hide();
        }
    }
}

void MsgCommonDelegate::OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount)
{
    int iPreFocus = m_pFramelist->GetFocusItemIndex();
    int iPreDataFocus = m_pFramelist->GetFocusItemDataIndex();
    MESSAGEUI_INFO("OnLoadData dataIdx[%d], dataCnt[%d], preF[%d], dataF[%d]"
                   , nDataIndex, nDataCount, iPreFocus, iPreDataFocus);
    //
#ifdef LOAD_ALL_DATA_FROM_LOGIC
    // check item can reused or not
    if (!CheckCanReused(nDataIndex))
    {
        ClearContent(false);
        MESSAGEUI_INFO("item can't be reused");
        for (int iIdx = nDataIndex, j = 0; j < PAGE_ITEM_COUNT && iIdx < m_vecData.size(); ++iIdx, ++j)
        {
            //创建Item
            CSettingItem * pItem = g_SettingItemManger->CreateItemByType(m_vecData[iIdx].eType, m_pItemParent,
                                   m_vecData[iIdx]);
            ((CListItemPtr)pItem)->SetAction(this);
            if (NULL != pItem)
            {
                pItem->setObjectName(QString("CSettingItem%1").arg(iIdx));
                pItem->hide();
                pItem->setFixedHeight(SETTING_ITEM_HEIGHT);
                m_vecItem.push_back(pItem);
            }
        }
    }
    //
    HideAllItem();

    if (NULL != m_pFramelist && m_vecData.size() >= nDataIndex)
    {
        //先清空当前数据
        m_pFramelist->DetachAllItem(true);
        //重新设置父类
        for (SettingItemList::iterator iter = m_vecItem.begin(); iter != m_vecItem.end(); ++iter)
        {
            (*iter)->setParent(m_pFramelist);
        }
        m_pFramelist->SetTotalDataCount(m_vecData.size());

        m_pFramelist->SetItemAttach();

        int iPageItemCount = m_pFramelist->GetMaxPageItemCount();
        //重新添加数据
        for (int i = nDataIndex, j = 0; i < nDataIndex + nDataCount && i < m_vecData.size()
                && j < iPageItemCount; i++, j++)
        {
            if (m_vecItem[j] == NULL)
            {
                continue;
            }

            ((CListItemPtr)m_vecItem[j])->SetDataIndex(i);

            m_vecItem[j]->SetLabText(m_vecData[i].first);
            QString strLabel = m_vecData[i].first;

            if (m_vecData[i].second != NULL)
            {
                m_vecData[i].second->setParent(m_vecItem[j]);
                m_vecItem[j]->SetContentWgt(m_vecData[i].second);
            }
            if (NULL != m_vecItem[j] && m_vecItem[j]->inherits("CMessageListItem"))
            {
                ((CMessageListItem *)m_vecItem[j])->AddIcon(m_vecData[i].strIcon.c_str());
            }

            //翻页时纠正选中项
            if (m_vecItem[j]->IsFocus() && m_vecItem[j]->GetDataIndex() != m_iCurrentIndex)
            {
                m_vecItem[j]->FocusItem(false);
            }

            m_vecItem[j]->show();

            ((CListItemPtr)m_vecItem[j])->SetAction(this);
            m_pFramelist->AddItem(m_vecItem[j]);
            m_vecItem[j]->Relayout();
        }

        if (-1 != iPreFocus && iPreFocus < m_vecData.size())
        {
            m_pFramelist->SetItemFocusByIndex(iPreFocus);
        }
        else if (m_iCurrentIndex >= nDataIndex && m_iCurrentIndex < m_vecData.size()
                 && m_iCurrentIndex < nDataIndex + m_pFramelist->GetTotalItemCount())
        {
            m_pFramelist->SetItemFocusByIndex(m_pFramelist->GetIndexByDataIndex(m_iCurrentIndex));
        }
        else
        {
            m_pFramelist->SelectItemByDataIndex(-1);
        }
    }
#else
    if (NULL == m_pDataDelegate)
    {
        return;
    }
    int iTot = 0;
    int iPage = nDataIndex / m_pFramelist->GetMaxPageItemCount();
    ClearContent(false, true);
    bool bGetData = m_pDataDelegate->GetData(iPage, iTot, &m_vecData);
    if (!bGetData)
    {
        return;
    }
    if (0 == m_vecData.size() && iTot > 0 && iPage > 0)
    {
        --iPage;
        nDataIndex -= m_pFramelist->GetMaxPageItemCount();
        if (!m_pDataDelegate->GetData(iPage, iTot, &m_vecData))
        {
            return;
        }
        // reset focus
        iPreFocus = m_pFramelist->GetMaxPageItemCount() - 1;
    }
    else if (iTot == iPreDataFocus)
    {
        // reset focus
        --iPreFocus;
    }

    // check item can reused or not
    if (!CheckCanReused(nDataIndex))
    {
        //ClearContent(false);
        ClearContent(true);
        MESSAGEUI_INFO("item can't be reused");
        for (int iIdx = 0; iIdx < PAGE_ITEM_COUNT && iIdx < iTot; ++iIdx)
        {
            //创建Item
            CSettingItem * pItem = g_SettingItemManger->CreateItemByType(m_vecData[iIdx].eType, m_pItemParent,
                                   m_vecData[iIdx]);
            if (NULL != pItem)
            {
                pItem->setObjectName(QString("CSettingItem%1").arg(iIdx));
                ((CListItemPtr)pItem)->SetAction(this);
                pItem->hide();
                pItem->setFixedHeight(SETTING_ITEM_HEIGHT);
                m_vecItem.push_back(pItem);
            }
        }
    }
    //
    HideAllItem();
    MESSAGEUI_INFO("tot[%d], data[%d], item[%d]"
                   , iTot, m_vecData.size(), m_vecItem.size());

    if (NULL != m_pFramelist && iTot >= nDataIndex)
    {
        //先清空当前数据
        m_pFramelist->DetachAllItem(true);
        //重新设置父类
        for (SettingItemList::iterator iter = m_vecItem.begin(); iter != m_vecItem.end(); ++iter)
        {
            (*iter)->setParent(m_pFramelist);
        }
        m_pFramelist->SetTotalDataCount(iTot);

        m_pFramelist->SetItemAttach();

        int iPageItemCount = m_pFramelist->GetMaxPageItemCount();
        //重新添加数据
        for (int i = nDataIndex, j = 0; i < nDataIndex + nDataCount && i < iTot
                && j < iPageItemCount; i++, j++)
        {
            if (m_vecItem[j] == NULL)
            {
                continue;
            }

            ((CListItemPtr)m_vecItem[j])->SetDataIndex(i);

            m_vecItem[j]->SetLabText(m_vecData[j].first);
            QString strLabel = m_vecData[j].first;

            if (m_vecData[j].second != NULL)
            {
                m_vecData[j].second->setParent(m_vecItem[j]);
                m_vecItem[j]->SetContentWgt(m_vecData[j].second);
            }
            if (NULL != m_vecItem[j] && m_vecItem[j]->inherits("CMessageListItem"))
            {
                ((CMessageListItem *)m_vecItem[j])->AddIcon(m_vecData[j].strIcon.c_str());
            }

            //翻页时纠正选中项
            if (m_vecItem[j]->IsFocus() && m_vecItem[j]->GetDataIndex() != m_iCurrentIndex)
            {
                m_vecItem[j]->FocusItem(false);
            }

            m_vecItem[j]->show();

            ((CListItemPtr)m_vecItem[j])->SetAction(this);
            m_pFramelist->AddItem(m_vecItem[j]);
            m_vecItem[j]->Relayout();
        }

        if (-1 != iPreFocus && iPreFocus < iTot)
        {
//          m_pFramelist->SelectItemByIndex(iPreFocus);
//          m_pFramelist->SetItemFocusByIndex(iPreFocus);
        }
        else if (m_iCurrentIndex >= nDataIndex && m_iCurrentIndex < iTot
                 && m_iCurrentIndex < nDataIndex + m_pFramelist->GetTotalItemCount())
        {
            m_pFramelist->SetItemFocusByIndex(m_pFramelist->GetIndexByDataIndex(m_iCurrentIndex));
            //m_pFramelist->SelectItemByIndex(m_pFramelist->GetIndexByDataIndex(m_iCurrentIndex));
        }
        else
        {
//          m_pFramelist->SelectItemByDataIndex(-1);
        }
    }
#endif
}

void MsgCommonDelegate::OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction)
{
    if (NULL == pItem || !(pItem->inherits("CSettingItem")) || pFrameList != m_pFramelist)
    {
        return;
    }

    if (nAction == LIST_ITEM_ACTION_SELECT)
    {
        CSettingItem * pSetItem = (CSettingItem *)pItem;
        m_iCurrentIndex = pSetItem->GetDataIndex();
    }

    CSettingItem * pSetItem = (CSettingItem *)pItem;
    emit listItemAction(nAction, pSetItem->GetClickAction());
}

QString MsgCommonDelegate::GetFocusItemAction()
{
    if (NULL != m_pFramelist && NULL != m_pFramelist->GetFocusItem())
    {
        CSettingItem * pItem = (CSettingItem *)(m_pFramelist->GetFocusItem());
        return pItem->GetClickAction();
    }

    return "";
}

int MsgCommonDelegate::GetFocusItemDataIndex()
{
    if (NULL != m_pFramelist)
    {
        return m_pFramelist->GetFocusItemDataIndex();
    }

    return INVALID_ITEM_INDEX;
}

int MsgCommonDelegate::GetCurrentRow()
{
    if (NULL != m_pFramelist)
    {
        return m_pFramelist->GetFocusItemIndex();
    }

    return INVALID_ITEM_INDEX;
}

void MsgCommonDelegate::GoNextPage()
{
    if (NULL == m_pFramelist)
    {
        return;
    }

    int iPageItemCount = m_pFramelist->GetMaxPageItemCount();
    int iTotalDataCount = m_pFramelist->GetTotalDataCount();

    if (iPageItemCount < 1 || iTotalDataCount <= iPageItemCount)
    {
        return;
    }

    CListItem * pItem =  m_pFramelist->GetItemByIndex(0);
    if (NULL == pItem)
    {
        return;
    }

    int iFirstIndex = pItem->GetDataIndex();

    if (iFirstIndex + iPageItemCount > iTotalDataCount)
    {
        m_pFramelist->RefreshData(0);
        m_pFramelist->FocusItemByDataIndex(0);
    }
    else
    {
        m_pFramelist->RefreshData(iFirstIndex + iPageItemCount);
        m_pFramelist->FocusItemByDataIndex(iFirstIndex + iPageItemCount);
    }
}

void MsgCommonDelegate::GoPrePage()
{
    if (NULL == m_pFramelist)
    {
        return;
    }

    int iPageItemCount = m_pFramelist->GetMaxPageItemCount();
    int iTotalDataCount = m_pFramelist->GetTotalDataCount();

    if (iPageItemCount < 1 || iTotalDataCount <= iPageItemCount)
    {
        return;
    }

    CListItem * pItem =  m_pFramelist->GetItemByIndex(0);
    if (NULL == pItem)
    {
        return;
    }

    int iFirstIndex = pItem->GetDataIndex();

    if (iFirstIndex > 0)
    {
        m_pFramelist->RefreshData(iFirstIndex - iPageItemCount);
        m_pFramelist->FocusItemByDataIndex(iFirstIndex - iPageItemCount);
    }
    else if (iFirstIndex == 0)
    {
        int iPageCount = (iTotalDataCount - 1) / iPageItemCount + 1;
        int iLastPageFirstIndex = (iPageCount - 1) * iPageItemCount;
        m_pFramelist->RefreshData(iLastPageFirstIndex);
        m_pFramelist->FocusItemByDataIndex(iLastPageFirstIndex);
    }
}

void MsgCommonDelegate::SetConfigedIndex(int iIndex)
{
    if (iIndex != m_iConfigedIndex)
    {
        ResetConfigedItem(m_iConfigedIndex, iIndex);
        m_iConfigedIndex = iIndex;
    }
}

void MsgCommonDelegate::ResetConfigedItem(int iOldIndex, int iNewIndex)
{
    if (iOldIndex >= 0 && iOldIndex < m_vecItem.size() && NULL != m_vecItem[iOldIndex])
    {
        m_vecItem[iOldIndex]->SetConfiged(false);
    }

    if (iNewIndex >= 0 && iNewIndex < m_vecItem.size() && NULL != m_vecItem[iNewIndex])
    {
        m_vecItem[iNewIndex]->SetConfiged(true);
    }
}

bool MsgCommonDelegate::IsEmpty()
{
    if (0 == m_vecData.size())
    {
        return true;
    }
    return false;
}

