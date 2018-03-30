#include <QtGui>
#include "grouptreeitem.h"
#include "qtcommon/qmisc.h"
#include "uikernel/ilistitemdelegate.h"
#include "uikernel/qwidgetutility.h"
#include "imagemanager/modimagemanager.h"
#include "baseui/t4xpicpath.h"
#include "uimanager/rectdefine.h"
#include "qtcommon/qmisc.h"
#include "qtcommon/xmlhelper.h"

namespace
{
#define DEFAULT_GROUP_ITEM_TEXT_X_SPACE 5
#define DEFAULT_GROUP_ITEM_SELECT_TEXT_COLOR QColor(255, 255, 255)
#define DEFAULT_GROUP_ITEM_NORMAL_TEXT_COLOR QColor(0, 0, 0)
#define DEFAULT_GROUP_ITEM_FONT_SIZE 16
#define DEFAULT_GROUP_ITEM_TEXT_LEVEL_SPACE 18
}

CGroupTreeItem::CGroupTreeItem(QWidget * parent/* = NULL*/, IListItemAction * pAction/* = NULL*/)
    : CTreeItem(parent, pAction)
{
    m_nType |= LIST_ITEM_SUB_TYPE_GROUP_TREE;

    setGeometry(0, 0, DEFAULT_TREE_ITEM_WIDTH, DEFAULT_TREE_ITEM_HEGITH);

    qWidgetUtility::setFontStyle(this, DEFAULT_GROUP_ITEM_FONT_SIZE);

    setObjectName("CGroupTreeItem");
}

CGroupTreeItem::~CGroupTreeItem(void)
{
}

void CGroupTreeItem::SetGroupName(const yl::string & strGroupName)
{
    m_strGroupName = toQString(strGroupName, true);
}

void CGroupTreeItem::SetGroupName(const QString & strGroupName)
{
    m_strGroupName = strGroupName;
}

const QString & CGroupTreeItem::GetGroupName() const
{
    return m_strGroupName;
}

void CGroupTreeItem::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);

    QPoint ptIconEnd;

    PaintGroupTree(stylePainter, ptIconEnd);
    PaintGroupText(stylePainter, ptIconEnd);
}

void CGroupTreeItem::PaintGroupTree(QStylePainter & stylePainter, QPoint & ptEnd)
{
    QRect rcItem = rect();

    PaintDownBg(stylePainter);

    int nLeft = rcItem.left();
    if (m_ucLevel > TREE_ITEM_LEVEL_FIRST)
    {
        nLeft += DEFAULT_GROUP_ITEM_TEXT_LEVEL_SPACE;
    }

    // 画箭头
    ptEnd.setX(nLeft);
    ptEnd.setY(rcItem.top());

    // 第一级无子节点不画图标
    if (TREE_ITEM_LEVEL_FIRST == m_ucLevel && !m_bHadChildren)
    {
        PaintRawIcon(stylePainter, ptEnd, NULL);//PIC_GENERAL_TREE_ITEM_CURL
    }
    else
    {
        if (IsDown() || IsFocus())
        {
#ifdef IF_ENABLE_TESTABILITY
            ichoose = 1;
#endif
            PaintExpand(stylePainter, ptEnd, PIC_GENERAL_TREE_ITEM_EXPAND_SELECT,
                        PIC_GENERAL_TREE_ITEM_CURL_SELECT);
        }
        else
        {
#ifdef IF_ENABLE_TESTABILITY
            ichoose = 0;
#endif
            PaintExpand(stylePainter, ptEnd, PIC_GENERAL_TREE_ITEM_EXPAND, PIC_GENERAL_TREE_ITEM_CURL);
        }
    }
}

void CGroupTreeItem::PaintGroupText(QStylePainter & stylePainter, const QPoint & ptBegin,
                                    int nTextRight/* = 0*/)
{
    bool bAdjustRightMargin = false;
    QRect rcItem = rect();

    if (0 == nTextRight)
    {
        bAdjustRightMargin = true;
        nTextRight = rcItem.width();
    }

    // 画文字
    int nX = ptBegin.x() + DEFAULT_GROUP_ITEM_TEXT_X_SPACE;

    if (m_ucLevel > TREE_ITEM_LEVEL_FIRST)
    {
        // 图标上加空格即可
        //nX += DEFAULT_GROUP_ITEM_TEXT_LEVEL_SPACE;
    }

    QRect rcText(nX, rcItem.top(), nTextRight - nX, rcItem.height());

    if (NULL != m_pDelegate && bAdjustRightMargin)
    {
        int nLeftMargin = 0;
        int nRightMargin = 0;

        m_pDelegate->GetContentPos(nLeftMargin, nRightMargin);
        rcText.setRight(rcText.right() - nRightMargin);
    }

    // 选中或按下设置为白色字体
    if (IsDown() || IsFocus())
    {
        stylePainter.setPen(DEFAULT_GROUP_ITEM_SELECT_TEXT_COLOR);
    }
    else
    {
        stylePainter.setPen(DEFAULT_GROUP_ITEM_NORMAL_TEXT_COLOR);
    }

#ifdef IF_ENABLE_TESTABILITY
    m_rcText = rcText;
#endif
    stylePainter.drawItemText(rcText, Qt::AlignLeft | Qt::AlignVCenter, palette(), isEnabled(),
                              m_strGroupName);
}

#ifdef IF_ENABLE_TESTABILITY
QString CGroupTreeItem::GetTestInfo()
{
    QString strTestinfo;
    xml_document doc;
    xml_node nodeRoot = doc.append_child("testinfo");

    TestInfo testInfo;
    QPoint ptAbsolute;

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
