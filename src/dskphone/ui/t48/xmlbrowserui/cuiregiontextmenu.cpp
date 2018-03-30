#include "cuiregiontextmenu.h"
#include "xbpositiondefine.h"
#include "textstylemanager.h"
#include "qtcommon/qmisc.h"
#include "keymap.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "extended/xmlbrowser/include/modxmlbrowser.h"
#include "modxmlbrowserui.h"
#include "uimanager/rectdefine.h"
#include "uicommon.h"
#include "baseui/pagectrl.h"
#include "baseui/t4xpicpath.h"
#include "imagemanager/modimagemanager.h"

namespace
{
#define XB_TEXT_MENU_LEFT_SPACING 0
#define FRM_TEXT_MENU_ITEM_MARGIN_LEFT 10
}

/////////////////////////////////////////////////////////////////////////
CTextMenuItemIS::CTextMenuItemIS(QWidget * parent):
    CBaseLabel(parent)
    , m_nIndex(0)
    , m_bSelected(false)
{
    this->setObjectName("TextMenuItemIS");

    this->setAlignment(Qt::AlignVCenter);

    this->installEventFilter(this);

    this->setFocusPolicy(Qt::StrongFocus);

    this->setFixedWidth(RECT_XMLBrowser.width() - XML_SCROLL_WIDTH);

    connect(this, SIGNAL(clicked()), this, SLOT(OnItemClick()));
}

CTextMenuItemIS::~CTextMenuItemIS()
{
    //
}

void CTextMenuItemIS::SetItemIndex(int nIndex)
{
    m_nIndex = nIndex;
}

int CTextMenuItemIS::GetItemIndex()
{
    return m_nIndex;
}

void CTextMenuItemIS::OnItemClick()
{
    emit ItemClicked(m_nIndex);
}

void CTextMenuItemIS::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter painter(this);
    if (m_bSelected && !m_strSelectBg.isEmpty())
    {
        painter.drawPixmap(0, 3, this->width(), this->height(),
                           THEME_GET_BMP(m_strSelectBg.toUtf8().data()));
    }
    CBaseLabel::paintEvent(pEvent);
}

bool CTextMenuItemIS::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (pEvent->type() == QEvent::FocusIn)
    {
        m_bSelected = true;
        update();

        return true;
    }
    else if (pEvent->type() == QEvent::FocusOut)
    {
        m_bSelected = false;
        update();

        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
CFrmTextMenu::CFrmTextMenu(QWidget * parent):
    QFrame(parent),
    m_nSumPage(0),
    m_nCurrentPage(0),
    /*m_bIsShowIndex(true),*/
    m_nCurrentFocusItem(0)
{
    setObjectName("FrmTextMenu");
    this->installEventFilter(this);
#if IF_FEATURE_XMLDIRECTORY
    m_bHasSearchDlg = false;
#endif

    InitTextMenuItem();
}

CFrmTextMenu::~CFrmTextMenu()
{
    //
}

void CFrmTextMenu::SetData(XBTextMenuData * pData)
{
    if (pData == NULL)
    {
        return;
    }

    m_vecItemData.clear();

    TextMenuItemData vecData;
    vecData.m_bIsWrap = pData->stAttr.bWrapList;

    //解析数据
    List_Menu_Item::iterator it = pData->listItem.begin();
    if (pData->stAttr.eStyle == STYLE_NUMBERED)
    {
        //显示序号
        int nIndex = 1;
        QString strIndex;
        for (/* */; it != pData->listItem.end(); it++)
        {
            vecData.m_strText = strIndex.setNum(nIndex) + "." +
                                toQString((*it)->strPrompt.c_str());

            vecData.m_nIndex = nIndex - 1;

            nIndex++;

#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
            vecData.m_vecSoftkey = (*it)->vecSoftkey;
#endif

            m_vecItemData.push_back(vecData);
        }
    }
    else
    {
        int nIndex = 0;
        for (/* */; it != pData->listItem.end(); it++)
        {
            vecData.m_strText = toQString((*it)->strPrompt.c_str());

            vecData.m_nIndex = nIndex;
            nIndex++;

#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
            vecData.m_vecSoftkey = (*it)->vecSoftkey;
#endif

            m_vecItemData.push_back(vecData);
        }
    }

    //设置数据后进行相应初始化及显示
    if (InitData())
    {
        int nDefaultPage = 1;
        int nDefaultItem = 0;

        GetDefaultData(pData->stAttr.iDefaultIndex, nDefaultPage, nDefaultItem);
        m_nCurrentFocusItem = nDefaultItem;

        m_nCurrentPage = nDefaultPage;
        UpdatePageData(m_nCurrentPage);


    }
}

void CFrmTextMenu::SetSelectedItemBg(QString strPicPath)
{
    CTextMenuItemIS * pTextMenuItem = NULL;
    std::vector<CTextMenuItemIS *>::iterator iter = m_vecTextMenuItem.begin();

    for (; iter != m_vecTextMenuItem.end(); ++iter)
    {
        pTextMenuItem = *iter;
        if (NULL != pTextMenuItem)
        {
            pTextMenuItem->SetSelectedBg(strPicPath);
        }
    }
}

void CFrmTextMenu::SetData(XBDirectoryData * pData)
{
    if (pData == NULL)
    {
        return;
    }

    m_vecItemData.clear();

    TextMenuItemData vecData;
    vecData.m_bIsWrap = false/*pData->pAttr->bWrapTitle*/;

    //解析数据
    List_Dir_Menu_Item::iterator it = pData->listItem.begin();

    int nIndex = 0;
    for (/* */; it != pData->listItem.end(); it++)
    {
        vecData.m_strText = toQString((*it)->strPrompt.c_str());

        vecData.m_nIndex = nIndex;
        nIndex++;

#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
        vecData.m_vecSoftkey = (*it)->vecSoftkey;
#endif

        m_vecItemData.push_back(vecData);
    }

    //设置数据后进行相应初始化及显示
    if (InitData())
    {
        int nDefaultPage = 1;
        int nDefaultItem = 0;
        GetDefaultData(pData->stAttr.iDefaultIndex, nDefaultPage, nDefaultItem);
        m_nCurrentFocusItem = nDefaultItem;
        m_nCurrentPage = nDefaultPage;
        UpdatePageData(m_nCurrentPage);


    }

#if IF_FEATURE_XMLDIRECTORY
    m_bHasSearchDlg = !pData->stInputField.strParam.empty();
#endif
}
bool CFrmTextMenu::InitData()
{
    //计算每个Item所需要的高度
    if (m_vecItemData.size() > 0)
    {
        CTextMenuItemIS * pTempItem = new CTextMenuItemIS(this);
        if (pTempItem == NULL)
        {
            return false;
        }
        //pTempItem->setWordWrap(true);
        pTempItem->SetLabLength(this->width());

        for (std::vector<TextMenuItemData>::size_type qx = 0; qx != m_vecItemData.size();
                ++qx)
        {
            pTempItem->SetTextWrap(m_vecItemData[qx].m_bIsWrap);
            pTempItem->SetTextContent(m_vecItemData[qx].m_strText);
            m_vecItemHeight.push_back(pTempItem->GetAutoResizeHeight());
        }
        pTempItem->deleteLater();
    }
    else
    {
        return false;
    }

    //根据每个Item的高度，计算所需页数和各页显示的条数
    m_nSumPage = GetSumPage(this->height(), m_vecItemHeight, m_vecPageItem);

    if (m_nSumPage > 0)
    {
        m_nCurrentPage = 1;
    }
    else
    {
        return false;
    }
    return true;
}

void CFrmTextMenu::GetDefaultData(int nDefaultIndex, int & nDefaultPage, int & nDefaultItem)
{
    //总页数为1
    if (m_nSumPage == 1)
    {
        nDefaultPage = 1;

        if (nDefaultIndex >= 0)
        {
            nDefaultItem = nDefaultIndex;
        }
        else
        {
            nDefaultItem = 0;
        }


        return;
    }
    if (m_vecPageItem.size() == 0
            || nDefaultIndex >= m_vecItemData.size())
    {
        return;
    }
    int nTempCount = 0;
    int nIndex = 0;
    for (nIndex = 0; nIndex < m_vecPageItem.size(); ++nIndex)
    {
        nTempCount += m_vecPageItem[nIndex];
        if (nTempCount >= nDefaultIndex + 1)
        {
            break;
        }
    }
    nDefaultPage = nIndex + 1;
    nDefaultItem = nDefaultIndex - (nTempCount - m_vecPageItem[nIndex]);
}

void CFrmTextMenu::UpdatePageData(int nCurrentPage /* = 1 */)
{
    //刷新数据前，清除页数数据
    ClearTextMenuItem();

    //根据当前页数，刷新页面数据
    std::vector<TextMenuItemData> vecPageData;
    std::vector<int> vecPageItemHeight;
    GetPageItemData(nCurrentPage, vecPageData, vecPageItemHeight);

    //当前Item的起始纵坐标
    int nCurrentHeight = 0;

    if (vecPageData.size() > 0)
    {
        for (int i = 0; i < vecPageData.size() && i < MAX_NUM_ITEM; ++i)
        {
            m_vecTextMenuItem[i]->SetLabLength(this->width());
            m_vecTextMenuItem[i]->SetTextContent(vecPageData[i].m_strText);
            m_vecTextMenuItem[i]->SetTextWrap(vecPageData[i].m_bIsWrap);

            m_vecTextMenuItem[i]->SetItemIndex(vecPageData[i].m_nIndex);
            m_vecTextMenuItem[i]->UpdateDiaplay();

#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
            m_vecTextMenuItem[i]->SetItemSoftkey(vecPageData[i].m_vecSoftkey);
#endif

            m_vecTextMenuItem[i]->move(0, nCurrentHeight);

            QRect rect = m_vecTextMenuItem[i]->rect();

            nCurrentHeight += vecPageItemHeight[i];
        }
        //隐藏没有数据的Item
        for (int i = vecPageData.size(); i < MAX_NUM_ITEM; ++i)
        {
            m_vecTextMenuItem[i]->hide();
        }
    }

    //每次更新时发出信号，传递当前页和总页数
    emit PageChanged(m_nCurrentPage, m_nSumPage);
}

int CFrmTextMenu::GetCurrentIndex()
{
    /*return m_nCurrentItem;*/
    int nIndex = 0;
    for (int i = 0; i < m_vecTextMenuItem.size(); i++)
    {
        if (m_vecTextMenuItem[i]->hasFocus())
        {
            nIndex = m_vecTextMenuItem[i]->GetItemIndex();
            return nIndex;
        }

    }
    return nIndex;
}

#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
//得到焦点编辑框的输入类型
VecSoftkeyParam CFrmTextMenu::GetFocusSoftkey()
{
    VecSoftkeyParam vTempSoftkey;
    //遍历所有lineEdit，找到当前焦点所在位置
    for (int i = 0; i < m_vecTextMenuItem.size(); i++)
    {
        if (m_vecTextMenuItem[i]->hasFocus())
        {
            vTempSoftkey = m_vecTextMenuItem[i]->GetItemSoftkey();
        }
    }
    return vTempSoftkey;
}
#endif

void CFrmTextMenu::InitTextMenuItem()
{
    //插入8个Item，每页显示最多使用8个
    CTextMenuItemIS * pTextMenuItem = NULL;

    for (int i = 0; i < MAX_NUM_ITEM; i++)
    {
        pTextMenuItem = new CTextMenuItemIS(this);
        if (pTextMenuItem == NULL)
        {
            return;
        }
        connect(pTextMenuItem, SIGNAL(ItemClicked(int)), this, SLOT(OnItemClick(int)));
        pTextMenuItem->SetLabLength(this->width());
        pTextMenuItem->setContentsMargins(FRM_TEXT_MENU_ITEM_MARGIN_LEFT, 0, 0, 0);
        m_vecTextMenuItem.push_back(pTextMenuItem);
        //预先设置每个Item的位置
        pTextMenuItem->setGeometry(0, HEIGHT_LINE * i, this->width(), HEIGHT_LINE);
    }
}

void CFrmTextMenu::OnItemClick(int nIndex)
{
    //保存当前选中Item索引
    m_nCurrentItem = nIndex;

    //发送点击信号
    emit ClickItem(m_nCurrentItem);
}

void CFrmTextMenu::ClearTextMenuItem()
{
    //清除页面数据
    CTextMenuItemIS * pTextMenuItem = NULL;
    for (int i = 0; i < m_vecTextMenuItem.size(); i++)
    {
        //取得item指针
        pTextMenuItem = m_vecTextMenuItem.at(i);
        //指针判空
        if (pTextMenuItem != NULL)
        {
            pTextMenuItem->ClearDisplay();
        }
    }
}

int CFrmTextMenu::GetItemNumByPage(int nCurrentPage)
{
    if (m_vecPageItem.size() > 0)
    {
        return m_vecPageItem[nCurrentPage - 1];
    }
    return 0;
}

void CFrmTextMenu::GetPageItemData(int nCurrentPage,
                                   std::vector<TextMenuItemData> & vecPageItemData,
                                   std::vector<int> & vecPageItemHeight)
{
    int nCountItem = 0;
    for (int i = 1; i < nCurrentPage; i++)
    {
        nCountItem += GetItemNumByPage(i);
    }
    for (int nItemIndex = nCountItem; nItemIndex < nCountItem + GetItemNumByPage(nCurrentPage);
            nItemIndex ++)
    {
        vecPageItemData.push_back(m_vecItemData[nItemIndex]);

        vecPageItemHeight.push_back(m_vecItemHeight[nItemIndex]);
    }
}

void CFrmTextMenu::SelectItemIndex(int nIndex)
{
    m_nCurrentItem = nIndex;
}

void CFrmTextMenu::SetFocusItem(int nItemIndex)
{
    //添加对索引的容错
    if (nItemIndex >= MAX_NUM_ITEM || nItemIndex < 0)
    {
        nItemIndex = 0;
    }
    if (m_vecTextMenuItem.size() > 0 && m_vecTextMenuItem.size() > nItemIndex)
    {
        if (m_vecTextMenuItem[nItemIndex]->isVisible())
        {
            SelectItemIndex(m_vecTextMenuItem[nItemIndex]->GetItemIndex());
            m_vecTextMenuItem[nItemIndex]->setFocus();
            m_nCurrentFocusItem = nItemIndex;
            emit CurrentItemChange(m_nCurrentItem);
        }
    }
}

void CFrmTextMenu::paintEvent(QPaintEvent * pEvent)
{
    //paintEvent负责给每行画分割线
    QStylePainter painter(this);
    std::vector<CTextMenuItemIS *>::iterator it;
    CTextMenuItemIS * pItem;

    for (it = m_vecTextMenuItem.begin(); it != m_vecTextMenuItem.end(); ++it)
    {
        pItem = *it;
        if (NULL != pItem && pItem->isVisible())
        {
            QRect rect = pItem->geometry();
            painter.drawPixmap(0, rect.y(), width(), 2, THEME_GET_BMP(PIC_GENERAL_LIST_ITEM_RIGHT_LINE));
        }
    }
}

bool CFrmTextMenu::eventFilter(QObject * pObj, QEvent * pEvent)
{
    if (pObj == NULL || pEvent == NULL)
    {
        return false;
    }

    //如果传递对象不是当前对象的子对象，则不做操作
    if (!isChildOf(pObj, this))
    {
        return false;
    }

    //判断事件类型
    switch (pEvent->type())
    {
    case QEvent::Show:
        {
#if IF_FEATURE_XMLDIRECTORY
            if (!m_bHasSearchDlg)
            {
                SetFocusItem(m_nCurrentFocusItem);
            }
#else
            //点击下载进入下一层页面，返回时会丢失焦点，这里重新设置焦点
            SetFocusItem(m_nCurrentFocusItem);
#endif
            return true;
        }
        break;
    case QEvent::KeyPress:
        {
            QKeyEvent * pKeyEvent = static_cast<QKeyEvent *>(pEvent);
            if (pKeyEvent != NULL)
            {
                switch (pKeyEvent->key())
                {
                case PHONE_KEY_RIGHT:
                    {
                        OnNextPage();
                        SetFocusItem(0);
                        return true;
                    }
                    break;
                case PHONE_KEY_LEFT:
                    {
                        OnLastPage();
                        SetFocusItem(0);
                        return true;
                    }
                    break;
                case PHONE_KEY_UP:
                    {
                        //向上切页
                        //OnLastPage();
                        //如果焦点在第一个，则向上翻页，并把焦点设置为上一个的最后一个
                        for (int i = 0; i < m_vecTextMenuItem.size(); i++)
                        {
                            //取得item指针
                            if (m_vecTextMenuItem[i]->hasFocus())
                            {
                                if (i == 0)
                                {
                                    OnLastPage();
                                    //m_vecTextMenuItem[m_vecPageItem[m_nCurrentPage - 1] -1]->setFocus();
                                    SetFocusItem(m_vecPageItem[m_nCurrentPage - 1] - 1);
                                }
                                else
                                {
                                    SetFocusItem(i - 1);
                                }
                                return true;
                            }
                        }
                        return true;
                    }
                    break;
                case PHONE_KEY_DOWN:
                    {
                        //向下切页
//                      OnNextPage();
//                      return true;
                        for (int i = 0; i < m_vecTextMenuItem.size(); i++)
                        {
                            //取得item指针
                            if (m_vecTextMenuItem[i]->hasFocus())
                            {
                                if (i == m_vecPageItem[m_nCurrentPage - 1] - 1)
                                {
                                    //焦点在最后一个item
                                    OnNextPage();
                                    SetFocusItem(0);
                                }
                                else
                                {
                                    SetFocusItem(i + 1);
                                }
                                return true;
                            }
                        }
                        return true;
                    }
                    break;
                case PHONE_KEY_OK:
                    {
                        for (int i = 0; i < m_vecTextMenuItem.size(); i++)
                        {
                            //取得item指针
                            if (m_vecTextMenuItem[i]->hasFocus())
                            {
                                emit ClickItem(m_vecTextMenuItem[i]->GetItemIndex());
                                return true;
                            }
                        }
                        return false;
                    }
                    break;
                default:
                    {
                        if (pKeyEvent->key() >= PHONE_KEY_1 && pKeyEvent->key() <= PHONE_KEY_9)
                        {
                            //添加对按键的响应1~9
                            int iIndex = pKeyEvent->key() - PHONE_KEY_1;

                            int iItemCount = 0;
                            int iFocusItem = 0;
                            for (int i = 0; i < m_vecPageItem.size(); i++)
                            {
                                iItemCount += m_vecPageItem[i];
                                if (iIndex + 1 > iItemCount)
                                {
                                    continue;
                                }
                                else
                                {
                                    // 设置页数
                                    m_nCurrentPage = i + 1;
                                    iFocusItem = (iIndex >= (iItemCount - m_vecPageItem[i]) ? iIndex - (iItemCount - m_vecPageItem[i]) :
                                                  iIndex);
                                    UpdatePageData(m_nCurrentPage);
                                    SetFocusItem(iFocusItem);
                                    SelectItemIndex(m_vecTextMenuItem[iFocusItem]->GetItemIndex());
                                    break;
                                }
                            }
                        }
                    }
                    break;
                }
            }
        }
        break;
    case QEvent::MouseMove:
        {
            QMouseEvent * pMouseEvent = (QMouseEvent *)pEvent;

            if (this->geometry().contains(pMouseEvent->pos()))
            {
                if (m_vecTextMenuItem.size() > 0)
                {
                    for (int nIndex = 0; nIndex < m_vecTextMenuItem.size(); ++ nIndex)
                    {
                        if (m_vecTextMenuItem[nIndex]->geometry().contains(pMouseEvent->pos()))
                        {
                            /*m_vecTextMenuItem[qx]->setFocus();*/
                            SetFocusItem(nIndex);
                        }
                    }
                }

            }
            return true;
        }
        break;
    default:
        {
        }
        break;
    }
    //其他事件交给系统处理
    return false;
}

void CFrmTextMenu::OnLastPage()
{
    XMLBROWSERUI_INFO("CFrmTextMenu::OnLastPage");
    if (m_nSumPage == 1)
    {
        //总数为一页，不需要翻页
        return;
    }
    if ((m_nCurrentPage - 1) < 1)
    {
        //当前在第一页，跳到最后一页
        m_nCurrentPage = m_nSumPage;
    }
    else
    {
        //后退一页
        m_nCurrentPage--;
    }

    //刷新页面数据
    UpdatePageData(m_nCurrentPage);
}

void CFrmTextMenu::OnNextPage()
{
    XMLBROWSERUI_INFO("CFrmTextMenu::OnNextPage");
    if (m_nSumPage == 1)
    {
        //总数为一页，不需要翻页
        return;
    }
    if (m_nCurrentPage == m_nSumPage)
    {
        //在最后一页，回到第一页
        m_nCurrentPage = 1;
    }
    else
    {
        //前进一页
        m_nCurrentPage++;
    }

    //刷新页面数据
    UpdatePageData(m_nCurrentPage);
}


////////////////////////////////////////////////////////////////////////////////
CUIRegionTextMenu::CUIRegionTextMenu(QWidget * parent, Qt::WFlags flags)
    : CUIRegionBase(parent, flags)
//  , IPageCtrlNotify()
    , m_pTextMenu(NULL)
    , m_pLabTitle(NULL)
    , m_pPageCtrl(NULL)
{
    setObjectName("UIRegionTextMenu");
    m_pLabTitle = new CTextMenuItemIS(this);
    if (m_pLabTitle != NULL)
    {
        m_pLabTitle->setObjectName("LabTitle");
        m_pLabTitle->setAlignment(Qt::AlignCenter);
        m_pLabTitle->setFocusPolicy(Qt::NoFocus);
        m_pLabTitle->setMinimumHeight(XML_TITLE_HEIGHT);
    }

    m_pTextMenu = new CFrmTextMenu(this);
    if (m_pTextMenu != NULL)
    {
        m_pTextMenu->setObjectName("TextMenu");
        connect(m_pTextMenu, SIGNAL(ClickItem(int)), this, SLOT(OnClickItem(int)));
        connect(m_pTextMenu, SIGNAL(PageChanged(int, int)), this, SLOT(OnPageChanged(int, int)));
        connect(m_pTextMenu, SIGNAL(CurrentItemChange(int)), this, SLOT(OnCurrentItemChange(int)));
        m_pTextMenu->SetSelectedItemBg(PIC_GENERAL_LIST_ITEM_CLICK_BG);
    }
    m_pPageCtrl = new CPageCtrl(this, this);
    if (NULL != m_pPageCtrl)
    {
        m_pPageCtrl->setObjectName("PageCtrl");
    }
}

void CUIRegionTextMenu::OnPageChanged(int nCurrentPage, int nSumPage)
{
    if (m_pPageCtrl != NULL)
    {
        m_pPageCtrl->SetScrollInfo(nSumPage, nCurrentPage - 1);
        m_pPageCtrl->update();

        /*if (nSumPage <= 1)
        {
            m_pPageCtrl->hide();
        }
        else
        {
            m_pPageCtrl->show();
        }*/
    }
}

// 通知翻到前一页
void CUIRegionTextMenu::OnNotifyScrollPrevious()
{
    if (m_pTextMenu != NULL)
    {
        m_pTextMenu->SetFocusItem(0);
        m_pTextMenu->OnLastPage();
    }
}

// 通知翻到下一页
void CUIRegionTextMenu::OnNotifyScrollNext()
{
    if (m_pTextMenu != NULL)
    {
        m_pTextMenu->SetFocusItem(0);
        m_pTextMenu->OnNextPage();
    }
}

void CUIRegionTextMenu::OnClickItem(int nIndex)
{
    //点击了某个Item或者按下了ok
    emit RegionAction(REGION_ACTION_CLICK_ITEM);
}

void CUIRegionTextMenu::OnCurrentItemChange(int iIndex)
{
#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
    VecSoftkeyParam vecPrivateSoftkey = m_pTextMenu->GetFocusSoftkey();
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
#endif
    emit RegionAction(REGION_ACTION_CURRENT_ITEM_CHANGE);
}

CUIRegionTextMenu::~CUIRegionTextMenu()
{

}

void CUIRegionTextMenu::Update(void * pData)
{
    if (pData == NULL)
    {
        return;
    }

    XBTextMenuData * pNewData = static_cast<XBTextMenuData *>(pData);

    if (m_pLabTitle == NULL
            || m_pTextMenu == NULL
            || m_pPageCtrl == NULL)
    {
        return;
    }
    m_pLabTitle->setGeometry(XB_TEXT_MENU_LEFT_SPACING, 0,
                             this->width() - XB_TEXT_MENU_LEFT_SPACING - XML_SCROLL_RIGHT_SPACING, XML_TITLE_HEIGHT);
    m_pLabTitle->SetTextWrap(pNewData->stAttr.bWrapTitle);
    m_pLabTitle->SetTextContent(toQString(pNewData->stAttr.strTitle.c_str()));

    m_pLabTitle->UpdateDiaplay();

    //设置数据并显示默认项
    m_pTextMenu->setGeometry(XB_TEXT_MENU_LEFT_SPACING, m_pLabTitle->height(),
                             this->width() - XML_SCROLL_WIDTH
                             - XML_SCROLL_LEFT_SPACING - XML_SCROLL_RIGHT_SPACING - XB_TEXT_MENU_LEFT_SPACING,
                             this->height() - m_pLabTitle->height());

    m_pTextMenu->SetData(pNewData);

    //m_pTextMenu->move(XB_TEXT_MENU_LEFT_SPACING, m_pLabTitle->height());

    m_pPageCtrl->SetRect(this->width() - XML_SCROLL_WIDTH - XML_SCROLL_RIGHT_SPACING,
                         m_pLabTitle->height() + XML_SCROLL_BAR_TOP_SPACING,
                         XML_SCROLL_WIDTH, m_pTextMenu->height() - XML_SCROLL_BAR_TOP_SPACING -
                         XML_SCROLL_BAR_BOTTOM_SPACING);
    m_iQuitTime = pNewData->stAttr.iTimeOut;

    m_bLock = pNewData->stAttr.bLockIn;

#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
    m_bDefSoftKey = (pNewData->vecSoftkey.size() == 0);
    m_vecGlobalSoftkey = pNewData->vecSoftkey;
    VecSoftkeyParam vecPrivateSoftkey = m_pTextMenu->GetFocusSoftkey();
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

bool CUIRegionTextMenu::eventFilter(QObject * pObj, QEvent * pEvent)
{
    if (pEvent->type() == QEvent::KeyPress)
    {
        if (OnKeyPress(pObj, pEvent))
        {
            return true;
        }
    }
    if (m_pTextMenu != NULL)
    {
        return m_pTextMenu->eventFilter(pObj, pEvent);
    }
    return false;
}

void CUIRegionTextMenu::SetDefaultSoftkey(VecSoftkeyParam & softkeyParam)
{
    softkeyParam.clear();

    //softkeyParam.push_back(TypeToSoftInfo(XB_ST_EXIT));
    //softkeyParam.push_back(TypeToSoftInfo(XB_ST_EMPTY));
    //softkeyParam.push_back(TypeToSoftInfo(XB_ST_EMPTY));
    softkeyParam.push_back(TypeToSoftInfo(XB_ST_SELECT));
}

bool CUIRegionTextMenu::RegionBaseKeypadHook(int nKeyCode, bool bPress)
{
    if (bPress && m_bLock
            && nKeyCode == PHONE_KEY_REDIAL)
    {
        return true;
    }
    if (bPress
            && (nKeyCode == PHONE_KEY_HANDSET_OFF_HOOK
                || nKeyCode == PHONE_KEY_HANDFREE))
    {
        //处理相应按键
        return ProcessKey(nKeyCode);
    }

    return false;
}

int CUIRegionTextMenu::GetCurrentItem()
{
    if (m_pTextMenu != NULL)
    {
        return m_pTextMenu->GetCurrentIndex();
    }
    return 0;
}

