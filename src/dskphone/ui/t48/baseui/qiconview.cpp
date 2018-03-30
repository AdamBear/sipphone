////----#include "model.h"
#include "qiconview.h"
#include "imagemanager/modimagemanager.h"
#include "uikernel/languagehelper.h"
#include "uikernel/translatehelper.h"
#include "keymap.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "customtheme/modcustomtheme.h"

#include "uimanager/rectdefine.h"
#include "t4xpicpath.h"

/*去掉的模块：
    上下翻页，click,press.
*/

#define MAX_MENU_COUNT 22
//!!!!!, 尽量与VP共用
//定义ICON的长宽
static const int ICON_SIZE = 100;


////////////////////////////////////---CIconViewItem---/////////////////////////////////////////////
//默认构造函数
CIconViewItem::CIconViewItem()
{
    setObjectName("CIconViewItem");
}

//析构函数
CIconViewItem::~CIconViewItem()
{

}

//带参数构造函数
CIconViewItem::CIconViewItem(MenuItemData * pItemData)
{
    if (pItemData != NULL)
    {
        //设置点击操作行为
        SetClickAction(pItemData->m_strClickAction.c_str());
        //设置菜单显示名称
        this->setText(LANG_TRANSLATE(pItemData->m_strDisplayName.c_str()));
        this->setTextColor(CustomTheme_GetCustomFontColorByKey(
                               CUSTOM_FONT_ID_MENU_NORMAL));    //设置菜单显示名颜色为灰色
        //设置图标
        QString strNormalIconPath = PIC_MENU_PREFIX;
        strNormalIconPath.append(pItemData->m_strIconName.c_str());
        this->setIcon(QIcon(THEME_GET_BMP(strNormalIconPath.toUtf8().data())));
    }
}

//构造函数
CIconView::CIconView(QWidget * pParent /*= 0*/)
    : CSwitchPageListView(pParent)
    , m_nMenuCount(0)
    , m_bDefaultSelect(false)
{
    this->setMinimumSize(RECT_MENU.width(), RECT_MENU.height());
    this->setMaximumSize(RECT_MENU.width(), RECT_MENU.height());
    QPalette pal = this->palette();
    pal.setBrush(QPalette::Window, QBrush(QPixmap(THEME_GET_BMP(PIC_GENERAL_BACKGROUND))));

    setObjectName("CIconView");
}

//析构函数
CIconView::~CIconView()
{

}

//加载IconView的菜单项
bool CIconView::LoadMenuItemData(MenuItemListData * pMenuItemListData)
{
    //指针判空，为空时直接返回
    if (pMenuItemListData == NULL)
    {
        return false;
    }

    //定义一个计数器用于记录菜单项添加的个数
    int iCount = 0;
    //取得菜单项item要用到数据链表
    MenuItemList * pMenuItemList = pMenuItemListData->m_pMenuItemList;
    if (pMenuItemList == NULL)
    {
        return false;
    }
    MenuItemData * pMenuItemData = NULL;
    CIconViewItem * pItem = NULL;
    MenuItemListIter it_itemList = (*pMenuItemList).begin();
    MenuItemListIter it_itemListEnd = (*pMenuItemList).end();

    for (; it_itemList != it_itemListEnd; it_itemList++)
    {
        if (iCount <= MAX_MENU_COUNT)
        {
            pMenuItemData = *it_itemList;
            pItem = new CIconViewItem(pMenuItemData);
            pItem->setObjectName(QString("CIconViewItem%1").arg(iCount));
            //pItem->setSizeHint(m_itemSize);
            pItem->setTextAlignment(Qt::AlignTop | Qt::AlignHCenter);
            pItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            addItem(pItem);
            //记录已经添加菜单项数目
            iCount++;
        }
    }
    m_nMenuCount = iCount;

    //初始化时显示首页
    ShowPage(1);

    //防止菜单为空死机
    if (m_bDefaultSelect && m_nMenuCount > 0)
    {
        this->item(0)->setSelected(true);
    }
    return true;
}

// 取消Item的选择效果
void CIconView::CancelItemSelect()
{
    if (this->selectedItems().size() < 1)
    {
        return;
    }

    QListWidgetItem * pItem = this->selectedItems()[0];
    if (pItem != NULL)
    {
        pItem->setSelected(false);
    }
}

//菜单项点击时的处理
void CIconView::OnItemEntered()
{
    if (this->isVisible())
    {
        //向外发送click信号
        emit ItemEntered();
    }
}

void CIconView::OnItemClicked(QListWidgetItem * pItem)
{
    this->OnItemEntered();
}

void CIconView::SetDefaultSelect(bool bSelelct)
{
    m_bDefaultSelect = bSelelct;
}
