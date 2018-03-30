#include "subpagelistdelegate.h"
#include "inputmethod/inputmethod.h"
#include "keymap.h"
#include "uikernel/languagehelper.h"
#include "qtcommon/qmisc.h"
#include "settingitemfactory.h"
#include "baseui/framelistdefine.h"
#include "uicommon/uikernel/qcomboboxutility.h"
#include "uimanager/rectdefine.h"
#include "dsklog/log.h"
#include "baseui/fontcolor.h"
#include "baseui/t4xpicpath.h"

namespace
{
#define SETTING_ITEM_HEIGHT  48
#define SETTING_FRAME_LEFT_SPACING  16
#define SETTING_FRAME_RIGHT_SPACING  16
#define SUBPAGE_MAX_PAGE_ITEM_COUNT  7
#define DEFAULT_SUBPAGE_LIST_ITEM_SPACE 0
#define SETTING_FRAM_LIST_RECT_TOP  0
static QSize SETTING_SUBPAGE_SIZE(536, 336);
static const QRect RECT_SETTING_SUBPAGE(256, 0, 536, 336);// setting界面的subpage的位置
}

SubpageListDeleagate::SubpageListDeleagate()
    : m_pFramelist(NULL)
    , m_pItemParent(NULL)
    , m_iCurrentIndex(0)
    , m_bIsEditPage(false)
    , m_iConfigedIndex(-1)
    , m_strItemClickTopBG("")
    , m_strItemClickMidBG("")
    , m_strItemClickBottomBG("")
    , m_bIsListReady(false)
    , m_bKeyChangePage(false)
{
}

SubpageListDeleagate::~SubpageListDeleagate()
{
    m_vecItem.clear();

    m_pFramelist = NULL;
}

bool SubpageListDeleagate::IsFrameListAttach(CFrameListBase * pFrameList)
{
    if (NULL == pFrameList || pFrameList != m_pFramelist)
    {
        return false;
    }

    return true;
}

void SubpageListDeleagate::BindFramelist(CFrameList * pList)
{
    if (NULL != pList)
    {
        m_pFramelist = pList;
        m_pFramelist->setObjectName("SubpageFrameList");
        m_pFramelist ->SetDelegate(this);
    }
}

void SubpageListDeleagate::UnBindFramelist()
{
    m_pFramelist = NULL;
}

void SubpageListDeleagate::SetItemParent(QWidget * parent)
{
    m_pItemParent = parent;
}

void SubpageListDeleagate::InitSettingListStyle()
{
    if (NULL != m_pFramelist)
    {
        m_pFramelist->setFocusPolicy(Qt::StrongFocus);
        m_pFramelist->setGeometry(0, SETTING_FRAM_LIST_RECT_TOP, SETTING_SUBPAGE_SIZE.width(),
                                  SETTING_SUBPAGE_SIZE.height() - SETTING_FRAM_LIST_RECT_TOP * 2);
        m_pFramelist->SetDefaultFocus(true);
        // m_pFramelist->SetItemFontSize(16, QFont::Normal);
        m_pFramelist->SetMaxPageItemCount(GetMaxPagePageItemCount());
        m_pFramelist->SetItemMargin(0, 0, 0);
        m_pFramelist->SetContentPos(SETTING_FRAME_LEFT_SPACING, SETTING_FRAME_RIGHT_SPACING);
#ifdef FL_SUPPORT_CUSTOM_FRAME_STYLE
        m_pFramelist->SetGridLine(true);
        m_pFramelist->SetLinePosY(DEFAULT_LEFT_LIST_ITEM_SPACE);
        m_pFramelist->SetLineHeight(DEFAULT_LEFT_LIST_ITEM_SPACE);
        m_pFramelist->SetItemSpace(DEFAULT_LEFT_LIST_ITEM_SPACE);
        m_pFramelist->SetLineImage(PIC_GENERAL_LIST_ITEM_RIGHT_LINE);
        m_pFramelist->SetItemClickBG(""); //按下背景有的行要有的行不要，framelist只能做到统一有或者没有，所以要定制
#endif
        m_pFramelist->SetItemSpace(DEFAULT_SUBPAGE_LIST_ITEM_SPACE);
        m_pFramelist->SetNote(LANG_TRANSLATE(TRID_NONE));
        m_pFramelist->SetNoteFontColor(THEME_TEXT_COLOR);
    }
}

//根据item的内容控件找到对应的item
CSettingItem * SubpageListDeleagate::FindItemByContentWgt(QWidget * pWidget)
{
    std::vector<CSettingItem *>::iterator iter = m_vecItem.begin();
    for (; iter != m_vecItem.end(); ++iter)
    {
        CSettingItem * pItem = (*iter);
        if (pItem != NULL)
        {
            if (pItem->ContentWgt() != NULL && isChildOf(pWidget, pItem->ContentWgt()))
            {
                return pItem;
            }
        }
    }

    return NULL;
}

void SubpageListDeleagate::SetErrorFocus(QLineEdit * pLineEdit)
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
            SetKeyboardEnterType();
            AdjustFrameList(false);
        }
    }
}

void SubpageListDeleagate::SelectIndex(int nIndex/* = 0*/, bool bEnter/* = false*/)
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
void SubpageListDeleagate::RefreshFocus()
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
void SubpageListDeleagate::UpdateItemStatus(bool bEanbled)
{
    SettingItemList::iterator itr = m_vecItem.begin();
    for (; itr != m_vecItem.end(); ++itr)
    {
        if (NULL == (*itr))
        {
            continue ;
        }

        if (bEanbled && (*itr)->IsItemEnabled())
        {
            (*itr)->SetItemEnabled(true);
        }
        else
        {
            (*itr)->SetItemEnabled(false);
        }
    }
}

void SubpageListDeleagate::UpdateList()
{
    if (m_pFramelist != NULL)
    {
        m_pFramelist->setFocus(Qt::ActiveWindowFocusReason);//页面捕获去焦点，焦点始终给list
        int iPageItemCount = m_pFramelist->GetMaxPageItemCount();
        if (m_pFramelist->GetTotalItemCount() == 0 && m_iCurrentIndex < iPageItemCount)
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
        m_pFramelist->RefreshData((m_iCurrentIndex / iPageItemCount) * iPageItemCount);
    }
}

bool SubpageListDeleagate::ProcessMouse(QObject* pObj)
{
    if (m_pFramelist == NULL)
    {
        return false;
    }

    if (pObj->inherits("CSettingItem"))
    {
        bool bOneEditItem = GetEditItemCount() == 1;
        CListItemPtr pItem = CListItemPtr(pObj);

        // 点击item此时还未focus,要判断Item的dataindex
        int index = pItem->GetDataIndex();
        qSetEnterType(bOneEditItem
                      ? ENTER_TYPE_ICON
                      : (index == GetLastEditItemIndex()
                         ? ENTER_TYPE_DONE
                         : ENTER_TYPE_NEXT));
        if (index != m_pFramelist->GetFocusItemDataIndex())
        {
            pItem->SetFocus(true);
            AdjustFrameList(false);
        }
    }
    else if (pObj->inherits("QLineEdit"))
    {
        CSettingItem* pItem = this->FindItemByContentWgt((QWidget*)pObj);
        if (pItem != NULL)
        {
            int index = m_pFramelist->GetIndexByItem(pItem);
        }
        AdjustFrameList(false);
        SetKeyboardEnterType();
    }
    else
    {
        SetKeyboardEnterType();
    }

    return false;
}

int SubpageListDeleagate::GetEditItemCount()
{
    if (m_pFramelist == NULL)
    {
        return 0;
    }

    int nCntEditItem = 0;
    int nTotal = m_pFramelist->GetTotalItemCount();

    for (int i = 0; i != nTotal; ++i)
    {
        if (IsEditItemByIndex(i))
        {
            nCntEditItem++;
        }
    }
    return nCntEditItem;
}

bool SubpageListDeleagate::IsEditItemByIndex(int nIndex)
{
    if (NULL == m_pFramelist)
    {
        return false;
    }
    CSettingItem* pTmp = (CSettingItem*)(m_pFramelist->GetItemByIndex(nIndex));
    if (pTmp != NULL)
    {
        return pTmp->IsEditItem();
    }
    return false;
}

bool SubpageListDeleagate::IsEditItemByDataIndex(int nIndex)
{
    if (NULL == m_pFramelist)
    {
        return false;
    }

    CSettingItem* pTmp = (CSettingItem*)(m_pFramelist->GetItemByDataIndex(nIndex));
    if (pTmp != NULL)
    {
        return pTmp->IsEditItem();
    }
    return false;
}

bool SubpageListDeleagate::IsLastEditItem()
{
    return (GetFocusItemDataIndex() == GetLastEditItemIndex());
}

int SubpageListDeleagate::GetLastEditItemIndex()
{
    if (NULL == m_pFramelist)
    {
        return -1;
    }
    for (int i = m_vecItem.size() - 1; i != -1; --i)
    {
        CSettingItem* pTmp = m_vecItem.at(i);
        if (pTmp != NULL)
        {
            if (pTmp->IsEditItem())
            {
                return i;
            }
        }
    }
    return -1;
}

void SubpageListDeleagate::SetKeyboardEnterType()
{
    bool bOneEditItem = GetEditItemCount() == 1;

    qSetEnterType(bOneEditItem
                  ? ENTER_TYPE_ICON
                  : (IsLastEditItem()
                     ? ENTER_TYPE_DONE
                     : ENTER_TYPE_NEXT));
}

void SubpageListDeleagate::AdjustFrameList(bool bReset)
{
    if (!FullKeyboardEnabled())
    {
        return;
    }
    if (NULL == m_pFramelist)
    {
        return;
    }

    if (bReset)
    {
        m_pFramelist->setGeometry(0, SETTING_FRAM_LIST_RECT_TOP, SETTING_SUBPAGE_SIZE.width(), SETTING_SUBPAGE_SIZE.height() - SETTING_FRAM_LIST_RECT_TOP * 2);
        return;
    }

    const QWidget* pWgt = GetCurrInputWgt();
    if (pWgt != NULL && m_pFramelist->enableAdjust())
    {
        if (pWgt->inherits("QLineEdit")
                || pWgt->inherits("QTextEdit")
           )
        {
            QPoint ptInput = pWgt->mapToGlobal(pWgt->pos());
            QRect rcFlist = m_pFramelist->geometry();

            // 焦点向下移动判断是否遮挡
            if (ptInput.y() + pWgt->height() + 4 > g_iFullKeyBoardY)
            {
                rcFlist.setY(rcFlist.y() - ptInput.y() - pWgt->height() - 4 + g_iFullKeyBoardY);
                rcFlist.setHeight(SETTING_SUBPAGE_SIZE.height());
                m_pFramelist->SetRect(rcFlist);
            }
            // 焦点向上移动判断是否被界面边缘覆盖
            else if (ptInput.y() - 6 < RECT_SETTING.y())
            {
                rcFlist.setY(rcFlist.y() + RECT_SETTING.y() - ptInput.y() + 6);
                rcFlist.setHeight(SETTING_SUBPAGE_SIZE.height());
                m_pFramelist->SetRect(rcFlist);
            }
        }
    }
}

bool SubpageListDeleagate::IsListReady()
{
    return m_bIsListReady;
}

void SubpageListDeleagate::ClearContent(bool bDeleteContenWgt)
{
    m_bIsListReady = false;

    HideAllItem();

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

void SubpageListDeleagate::SetCurrentIndex(int iIndex)
{
    m_iCurrentIndex = iIndex;
}

int SubpageListDeleagate::GetCurrentIndex()
{
    return m_iCurrentIndex;
}

bool SubpageListDeleagate::ProcessKeyPress(int keyCode)
{
    if (NULL == m_pFramelist  || !m_pFramelist->isVisible())
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
        if (NULL == m_pFramelist)
        {
            return false;
        }

        if (!m_bIsEditPage)
        {
            //非编辑型页面大于一页时翻页
            if (m_pFramelist->GetTotalDataCount() > m_pFramelist->GetMaxPageItemCount())
            {
                GoPrePage();
            }

            ///非编辑型界面不选中行，如ringtone、language
            //m_pFramelist->FocusItemByIndex(m_pFramelist->GetTotalItemCount() - 1);

            return true;
        }

        QWidget * pwgt =  m_pFramelist->focusWidget();
        if (NULL != FindItemByContentWgt(pwgt))
        {
            if (FocusPreNextEditableWidget(false))
            {
                return true;
            }
        }

        m_bKeyChangePage = true;

        GoPrePage();
        m_pFramelist->FocusItemByIndex(m_pFramelist->GetTotalItemCount() - 1);

        // 到新的页面，也需进行判断处理不可切换的类型
        if (m_pFramelist->GetFocusItem() != NULL)
        {
            CSettingItem * pItem = (CSettingItem *)m_pFramelist->GetFocusItem();
            QWidget * pwgt = pItem->ContentWgt();
            if (pwgt == NULL || pwgt->inherits("QLabel") || !pwgt->isEnabled())
            {
                FocusPreNextEditableWidget(false);
            }
            return true;
        }

        return true;
    }
    break;
    case PHONE_KEY_DOWN:
    {
        if (NULL == m_pFramelist)
        {
            return false;
        }

        if (!m_bIsEditPage)
        {
            //非编辑型页面大于一页时翻页
            if (m_pFramelist->GetTotalDataCount() > m_pFramelist->GetMaxPageItemCount())
            {
                GoNextPage();
            }

            ///非编辑型界面不选中行，如ringtone、language
            //m_pFramelist->FocusItemByIndex(0);

            return true;
        }

        QWidget * pwgt =  m_pFramelist->focusWidget();
        if (NULL != FindItemByContentWgt(pwgt))
        {
            if (FocusPreNextEditableWidget(true))
            {
                return true;
            }
        }

        m_bKeyChangePage = true;

        GoNextPage();
        m_pFramelist->FocusItemByIndex(0);

        // 到新的页面，也需进行判断处理不可切换的类型
        if (m_pFramelist->GetFocusItem() != NULL)
        {
            CSettingItem * pItem = (CSettingItem *)m_pFramelist->GetFocusItem();
            QWidget * pwgt = pItem->ContentWgt();
            if (pwgt == NULL || pwgt->inherits("QLabel") || !pwgt->isEnabled())
            {
                FocusPreNextEditableWidget(true);
            }
            return true;
        }

        return true;
    }
    break;
    default:
        break;
    }
    return false;
}

bool SubpageListDeleagate::FocusPreNextEditableWidget(bool bNext)
{
    bool bOneEditItem = GetEditItemCount() == 1;

    if (NULL != m_pFramelist)
    {
        int iFirstDataIndex = -1;
        int iLastDataIndex = -1;

        CListItem * pItem = m_pFramelist->GetItemByIndex(0);
        if (NULL != pItem)
        {
            iFirstDataIndex = pItem->GetDataIndex();
            iLastDataIndex = iFirstDataIndex + m_pFramelist->GetTotalItemCount() - 1;
        }

        int index = m_pFramelist->GetFocusItemDataIndex();

        if (bNext)
        {
            if (index == INVALID_ITEM_INDEX)
            {
                index = iFirstDataIndex - 1;
            }

            if (index == iLastDataIndex
                    && IsEditItemByDataIndex(index))
            {
                AdjustFrameList(false);
            }

            for (++index; index <= iLastDataIndex; ++index)
            {
                CSettingItem * pTmp = (CSettingItem *)(m_pFramelist->GetItemByDataIndex(index));
                if (NULL != pTmp)
                {
                    QWidget * pwgt = pTmp->ContentWgt();
                    if (pwgt != NULL
                            && !(pwgt->inherits("QLabel")) && (pwgt->isEnabled())
                            && (!FullKeyboardEnabled() || pwgt->isVisible())
                       )
                    {
                        m_pFramelist->FocusItemByDataIndex(index);

                        SetKeyboardEnterType();
                        if (pwgt->inherits("QLineEdit")
                                || pwgt->inherits("QTextEdit"))
                        {
                            AdjustFrameList(false);
                        }
                        return true;
                    }
                }
            }
        }
        else
        {
            if (index == INVALID_ITEM_INDEX)
            {
                index = iLastDataIndex + 1;
            }

            if (index == 0
                    && IsEditItemByDataIndex(index))
            {
                AdjustFrameList(false);
            }

            for (--index; index >= iFirstDataIndex; --index)
            {
                CSettingItem * pTmp = (CSettingItem *)(m_pFramelist->GetItemByDataIndex(index));
                if (NULL != pTmp)
                {
                    QWidget * pwgt = pTmp->ContentWgt();
                    if (pwgt != NULL &&
                            !(pwgt->inherits("QLabel")) && (pwgt->isEnabled())
                            && (!FullKeyboardEnabled() || pwgt->isVisible())
                       )
                    {
                        m_pFramelist->FocusItemByDataIndex(index);

                        SetKeyboardEnterType();
                        if (pwgt->inherits("QLineEdit")
                                || pwgt->inherits("QTextEdit"))
                        {
                            AdjustFrameList(false);
                        }
                        return true;
                    }
                }
            }
        }
    }

    qSetEnterType(bOneEditItem ? ENTER_TYPE_ICON : ENTER_TYPE_NEXT);
    return false;
}

void SubpageListDeleagate::SetContent(QWPVector * pVecWP)
{
    m_bIsListReady = false;

    if (NULL == pVecWP)
    {
        return;
    }

    QWPVector::iterator iter = pVecWP->begin();
    //创建列表
    m_vecItem.clear();

    m_bIsEditPage = false;
    //int iValidEditWidget = 0;

    for (int i = 0; iter !=  pVecWP->end(); ++iter, ++i)
    {
        if (iter->second != NULL)
        {
            iter->second->setParent(m_pItemParent);
        }

        iter->iIndex = i;
        //创建Item
        CSettingItem * pItem = g_SettingItemManger->CreateItemByType(iter->eType, m_pItemParent, *iter);
        ((CListItemPtr)pItem)->SetAction(this);
        if (NULL != pItem)
        {
            pItem->setObjectName(QString("CSettingItem%1").arg(i));

            pItem->SetWgetHost(m_pItemParent);

            if (iter->second != NULL)
            {
                iter->second->setObjectName(QString("QWidgetSecond%1").arg(i));
            }
            //判断是否为编辑型界面（有下拉框或输入框或单选框的界面）
            if (iter->second != NULL && !m_bIsEditPage)
            {
                if ((iter->second->inherits("QComboBox")
                        || iter->second->inherits("QLineEdit")
                        || iter->second->inherits("qCheckBox"))
                        && iter->second->isEnabled())
                {
                    m_bIsEditPage = true;
                    //iValidEditWidget++;
                }
            }

            pItem->hide();
            pItem->setFixedHeight(SETTING_ITEM_HEIGHT);
#if IF_SETTINGUI_FRAMELIST_SUPPORT_CHANGE_COLOR
#ifdef QT_TOUCH_SUPPORT
            pItem->SetChangeStyleOnPress(iter->bChangeStyleOnPress);
#endif
            pItem->SetChangeStyleOnSelect(iter->bChangeStyleOnSelect);
#endif
            m_vecItem.push_back(pItem);
        }
    }

    if (NULL != m_pFramelist)
    {
        m_pFramelist->SetTotalDataCount(m_vecItem.size());
    }

    m_bIsListReady = true;

    /*if (iValidEditWidget >= 2)
    {
        m_bIsEditPage = true;
    }*/
}

void SubpageListDeleagate::HideAllItem()
{
    int iVectorItemCount = m_vecItem.size();

    for (int i = 0; i < iVectorItemCount; i++)
    {
        if (m_vecItem[i] != NULL)
        {
            m_vecItem[i]->hide();
        }
    }
}

void SubpageListDeleagate::OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount)
{
    m_bIsListReady = false;

    HideAllItem();
    int iVectorItemCount = m_vecItem.size();

    if (m_pFramelist && iVectorItemCount >= nDataIndex)
    {
        //先清空当前数据
        m_pFramelist->DetachAllItem(true);
        //重新设置父类
        for (int iIndexItem = 0; iIndexItem < iVectorItemCount; iIndexItem++)
        {
            if (NULL != m_vecItem[iIndexItem])
            {
                m_vecItem[iIndexItem]->setParent(m_pFramelist);
            }
        }

        m_pFramelist->SetTotalDataCount(iVectorItemCount);

        m_pFramelist->SetItemAttach();

        int iPageItemCount = m_pFramelist->GetMaxPageItemCount();
        int iAddCount = 1;
        //重新添加数据
        for (int i = nDataIndex, j = 0; i < nDataIndex + nDataCount && i < iVectorItemCount
                && j < iPageItemCount; i++, j++)
        {
            CSettingItem * pItem = m_vecItem[i];
            if (pItem == NULL)
            {
                continue;
            }

            pItem->SetWgetHost(m_pItemParent);

            //翻页时纠正选中项
            if (pItem->IsFocus() && pItem->GetDataIndex() != m_iCurrentIndex)
            {
                pItem->FocusItem(false);
            }

            if (i != m_iConfigedIndex && pItem->IsConfiged())
            {
                pItem->SetConfiged(false);
            }
            else if (i == m_iConfigedIndex && !pItem->IsConfiged())
            {
                pItem->SetConfiged(true);
            }

            pItem->show();

            //设置按下背景，按下背景有的行要，有的行不要，framelist实现不了，所以要定制，在item上实现
            if (iAddCount == 1)
            {
                pItem->SetClickBg(m_strItemClickTopBG);
            }
            else if (iAddCount == iPageItemCount)
            {
                pItem->SetClickBg(m_strItemClickBottomBG);
            }
            else
            {
                pItem->SetClickBg(m_strItemClickMidBG);
            }

            m_pFramelist->AddItem(pItem);
            iAddCount++;
            pItem->CListItem::SetAction(this);
        }

        // 这里设置数据准备好 为了下边设置焦点弹出全键盘
        m_bIsListReady = true;

        if (m_iCurrentIndex >= nDataIndex && m_iCurrentIndex < nDataIndex + m_pFramelist->GetTotalItemCount()
                // 上下键切页 在处理按键时处理了焦点，这里不设置焦点
                && (!FullKeyboardEnabled() || !m_bKeyChangePage)
           )
        {
            m_pFramelist->SetItemFocusByIndex(m_pFramelist->GetIndexByDataIndex(m_iCurrentIndex));
        }
        else
        {
            m_pFramelist->SelectItemByDataIndex(INVALID_ITEM_INDEX);
            m_pFramelist->setFocus();
        }
    }

    bool bOneEditItem = GetEditItemCount() == 1;
    qSetEnterType(bOneEditItem
                  ? ENTER_TYPE_ICON
                  : ENTER_TYPE_NEXT);
    m_bIsListReady = true;
    m_bKeyChangePage = false;
}

int SubpageListDeleagate::GetMaxPagePageItemCount()
{
    return SUBPAGE_MAX_PAGE_ITEM_COUNT;
}

void SubpageListDeleagate::OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction)
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

QString SubpageListDeleagate::GetCurrentItemAction()
{
    if (NULL != m_pFramelist && NULL != m_pFramelist->GetFocusItem())
    {
        CSettingItem * pItem = (CSettingItem *)(m_pFramelist->GetFocusItem());
        return pItem->GetClickAction();
    }
    else if (m_iCurrentIndex >= 0
             && m_iCurrentIndex < m_vecItem.size()
             && m_vecItem[m_iCurrentIndex] != NULL)
    {
        return m_vecItem[m_iCurrentIndex]->GetClickAction();
    }

    return "";
}

int SubpageListDeleagate::GetFocusItemDataIndex()
{
    if (NULL != m_pFramelist)
    {
        return m_pFramelist->GetFocusItemDataIndex();
    }

    return INVALID_ITEM_INDEX;
}

int SubpageListDeleagate::GetCurrentRow()
{
    if (NULL != m_pFramelist)
    {
        return m_pFramelist->GetFocusItemIndex();
    }

    return INVALID_ITEM_INDEX;
}

void SubpageListDeleagate::GoNextPage()
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

    AdjustFrameList(true);

    CListItem * pItem =  m_pFramelist->GetItemByIndex(0);
    if (NULL == pItem)
    {
        return;
    }

    int iFirstIndex = pItem->GetDataIndex();

    if (iFirstIndex + iPageItemCount >= iTotalDataCount)
    {
        m_pFramelist->RefreshData(0);
        //m_pFramelist->FocusItemByDataIndex(0);
    }
    else
    {
        m_pFramelist->RefreshData(iFirstIndex + iPageItemCount);
        //m_pFramelist->FocusItemByDataIndex(iFirstIndex + iPageItemCount);
    }
}

void SubpageListDeleagate::GoPrePage()
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

    AdjustFrameList(true);

    CListItem * pItem =  m_pFramelist->GetItemByIndex(0);
    if (NULL == pItem)
    {
        return;
    }

    int iFirstIndex = pItem->GetDataIndex();

    if (iFirstIndex > 0)
    {
        m_pFramelist->RefreshData(iFirstIndex - iPageItemCount);
        //m_pFramelist->FocusItemByDataIndex(iFirstIndex - iPageItemCount);
    }
    else if (iFirstIndex == 0)
    {
        int iPageCount = (iTotalDataCount - 1) / iPageItemCount + 1;
        int iLastPageFirstIndex = (iPageCount - 1) * iPageItemCount;
        m_pFramelist->RefreshData(iLastPageFirstIndex);
        //m_pFramelist->FocusItemByDataIndex(iLastPageFirstIndex);
    }
}

void SubpageListDeleagate::SetConfigedIndex(int iIndex)
{
    if (iIndex != m_iConfigedIndex)
    {
        ResetConfigedItem(m_iConfigedIndex, iIndex);
        m_iConfigedIndex = iIndex;
    }
}

int SubpageListDeleagate::GetConfigedIndex()
{
    return m_iConfigedIndex;
}

void SubpageListDeleagate::ResetConfigedItem(int iOldIndex, int iNewIndex)
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

void SubpageListDeleagate::SelectIndexByAction(QString strAction)
{
    if (NULL == m_pFramelist || m_vecItem.size() == 0)
    {
        return;
    }

    SettingItemList::iterator  iter = m_vecItem.begin();
    for (; iter != m_vecItem.end(); iter++)
    {
        CSettingItem * pItem = *iter;
        if (NULL != pItem && pItem->GetClickAction() == strAction)
        {
            m_pFramelist->FocusItemByDataIndex(pItem->GetDataIndex());
            //此时可能还没有添加item，先做标记，加载时再选中
            m_iCurrentIndex = pItem->GetDataIndex();
            return;
        }
    }
}

void SubpageListDeleagate::SetNoClickBG()
{
    m_strItemClickTopBG = "";
    m_strItemClickMidBG = "";
    m_strItemClickBottomBG = "";
}

void SubpageListDeleagate::SetItemClickBG(const char * pszTopBG, const char * pszMidBG,
        const char * pszBottomBG)
{
    if (NULL != pszTopBG)
    {
        m_strItemClickTopBG = pszTopBG;
    }
    else
    {
        m_strItemClickTopBG = "";
    }

    if (NULL != pszMidBG)
    {
        m_strItemClickMidBG = pszMidBG;
    }
    else
    {
        m_strItemClickMidBG = "";
    }

    if (NULL != pszBottomBG)
    {
        m_strItemClickBottomBG = pszBottomBG;
    }
    else
    {
        m_strItemClickBottomBG = "";
    }
}
