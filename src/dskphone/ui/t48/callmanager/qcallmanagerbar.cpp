#include "qcallmanagerbar.h"
#include "qcallmanageritem.h"
#include "QStylePainter"
#include "QPainter"
#include "QRect"
#include "QStyleOptionButton"
#include "imagemanager/modimagemanager.h"
#include "baseui/t4xpicpath.h"
#include "uicommon/qtcommon/qmisc.h"
#include "uicommon/uikernel/languagehelper.h"

namespace
{
#define WIDTH_OF_BTN_ITEM_BAR                       480
#define HEIGHT_OF_BTN_ITEM_BAR                      87
}

qCallManagerBar::qCallManagerBar(QWidget * parent /* = 0 */)
    : CCallManagerBarBase(parent)
    , m_iNumOfBtnItem(0)
    , m_iCurSelectItem(-1)
    , m_strDivideLinePic(PIC_SOFTKEY_DIVIDE_LINE)
    , m_strBackgroundPic("")
{
    for (int i = 0; i < MaxNumOfBtnItem; ++i)
    {
        //  m_pBtnItem[i] = new qCallManagerBarItem(this);
        m_pBtnItem[i] = NULL;
    }

    this->resize(WIDTH_OF_BTN_ITEM_BAR, HEIGHT_OF_BTN_ITEM_BAR);
}

qCallManagerBar::~qCallManagerBar(void)
{
    m_iCurSelectItem = -1;
}

//设置所有 item 信息
void qCallManagerBar::SetCallMGRListInfo(YLList<CCallMGRInfo *> & listCallMGRInfo)
{
    //这边判断 如果已经有创建了 就不需要再创建了
    if (m_iNumOfBtnItem > 0)
    {
        return ;
    }

    CCallMGRInfo * pCallMGRInfo = NULL;
    YLList<CCallMGRInfo *>::iterator ite = listCallMGRInfo.begin();

    //循环创建按钮
    for (; listCallMGRInfo.end() != ite; ite++)
    {
        pCallMGRInfo = *ite;

        if (NULL != pCallMGRInfo)
        {
            CreateItemBar(pCallMGRInfo);

            pCallMGRInfo = NULL;
        }
    }
}

//设置焦点 item
void qCallManagerBar::SetFocusMGRInfo(const yl::string & strUIName)
{
    for (int i = 0; i < m_iNumOfBtnItem; i++)
    {
        qCallManagerBarItem * pItem = m_pBtnItem[i];

        if (NULL != pItem)
        {
            if (pItem->GetClickAction() == toQString(strUIName))
            {
                pItem->SetDrawBg(true);
            }
            else
            {
                pItem->SetDrawBg(false);
            }
        }
    }
}

//创建单个按钮
void qCallManagerBar::CreateItemBar(CCallMGRInfo * pCallMGRInfo)
{
    if (NULL == pCallMGRInfo)
    {
        return ;
    }

    qCallManagerBarItem * pItem = new qCallManagerBarItem(this);
    if (NULL != pItem)
    {
        pItem->SetSelectPicture(toQString(pCallMGRInfo->callMGRItemInfo.strSelectBKImg));
        pItem->SetText(LANG_TRANSLATE(pCallMGRInfo->callMGRItemInfo.strDisplayName.c_str()));
        pItem->SetIconPicture(toQString(pCallMGRInfo->callMGRItemInfo.strIcon));
        pItem->SetClickAction(toQString(pCallMGRInfo->strUIName));

        //加入到队列中  这里面会连接槽函数
        this->AddBtnItem(pItem);
    }
}

// 设置Background
void qCallManagerBar::SetBackgroundPicture(const QString & strBackgroundPic)
{
    if (!strBackgroundPic.isNull())
    {
        m_strBackgroundPic = strBackgroundPic;
    }
}

// 获取Background
QString qCallManagerBar::GetBackGroundPicture()
{
    if (!m_strBackgroundPic.isNull())
    {
        return m_strBackgroundPic;
    }
    else
    {
        return "";
    }
}

// 设置分隔线
void qCallManagerBar::SetDivideLinePicture(const QString & strDivideLinePic)
{
    if (!strDivideLinePic.isNull())
    {
        m_strDivideLinePic = strDivideLinePic;
    }
}

// 获取分隔线
QString qCallManagerBar::GetDivideLinePicutre()
{
    if (!m_strDivideLinePic.isNull())
    {
        return m_strDivideLinePic;
    }
    else
    {
        return "";
    }
}

// 设置选中的Index
void qCallManagerBar::SetCurSelectIndex(const int iCurSelIndex)
{
    if (iCurSelIndex >= -1 && iCurSelIndex < m_iNumOfBtnItem)
    {
        m_iCurSelectItem = iCurSelIndex;
    }
}

// 获取选中的Index
int qCallManagerBar::GetCurSelectIndex()
{
    return m_iCurSelectItem;
}

// 设置第Index个BtnBarItem的Action
void qCallManagerBar::SetIndexAction(const QString & strClickAction, int iIndex /* = 0 */)
{
    if (iIndex < m_iNumOfBtnItem && NULL != m_pBtnItem[iIndex])
    {
        m_pBtnItem[iIndex]->SetClickAction(strClickAction);
    }
}

// 获取第Index个BtnBarItem的Action
QString qCallManagerBar::GetIndexAction(int iIndex /* = 0 */)
{
    if (iIndex < m_iNumOfBtnItem && NULL != m_pBtnItem[iIndex])
    {
        return m_pBtnItem[iIndex]->GetClickAction();
    }

    return "";
}

// 设置第Index个BtnBarItem的Icon
void qCallManagerBar::SetIndexIcon(const QString & strIcon, int iIndex /* = 0 */)
{
    if (iIndex < m_iNumOfBtnItem && NULL != m_pBtnItem[iIndex])
    {
        m_pBtnItem[iIndex]->SetIconPicture(strIcon);
    }
}

// 获取第Index个BtnBarItem的Icon
QString qCallManagerBar::GetIndexIcon(int iIndex /* = 0 */)
{
    if (iIndex < m_iNumOfBtnItem && NULL != m_pBtnItem[iIndex])
    {
        return m_pBtnItem[iIndex]->GetIconPicture();
    }

    return "";
}

// 设置第Index个BtnBarItem的Text
void qCallManagerBar::SetIndexText(const QString & strText, int iIndex /* = 0 */)
{
    if (iIndex < m_iNumOfBtnItem && NULL != m_pBtnItem[iIndex])
    {
        m_pBtnItem[iIndex]->SetText(strText);
    }
}

// 获取第Index个BtnBarItem的Text
QString qCallManagerBar::GetIndexText(int iIndex /* = 0 */)
{
    if (iIndex < m_iNumOfBtnItem && NULL != m_pBtnItem[iIndex])
    {
        return m_pBtnItem[iIndex]->GetText();
    }

    return "";
}

// 设置第Index个BtnBarItem的SelectPic
void qCallManagerBar::SetIndexSelectPic(const QString & strSelectPic, int iIndex /* = 0 */)
{
    if (iIndex < m_iNumOfBtnItem && NULL != m_pBtnItem[iIndex])
    {
        m_pBtnItem[iIndex]->SetSelectPicture(strSelectPic);
    }
}

// 获取第Index个BtnBarItem的SelectPic
QString qCallManagerBar::GetIndexSelectPic(int iIndex /* = 0 */)
{
    if (iIndex < m_iNumOfBtnItem && NULL != m_pBtnItem[iIndex])
    {
        return m_pBtnItem[iIndex]->GetSelectPicture();
    }

    return "";
}

void qCallManagerBar::paintEvent(QPaintEvent * pPaintEvent)
{
    //直接交给大背景去绘制
    return ;
}

int qCallManagerBar::FindBtnByClickAction(const QString & strClickAction)
{
    for (int i = 0; i < m_iNumOfBtnItem; ++i)
    {
        if (NULL != m_pBtnItem[i] && m_pBtnItem[i]->GetClickAction() == strClickAction)
        {
            return i;
        }
    }

    return -1;
}

int qCallManagerBar::FindBtnByIndex(const int iIndex)
{
    if (iIndex < 0 || iIndex >= m_iNumOfBtnItem
            || NULL == m_pBtnItem[iIndex])
    {
        return -1;
    }
    else
    {
        return iIndex;
    }
}

// 设置BtnItem位置
void qCallManagerBar::SetBtnItemLoaction()
{
    int iWidth = WIDTH_OF_BTN_ITEM_BAR;

    if (0 != m_iNumOfBtnItem)
    {
        iWidth = WIDTH_OF_BTN_ITEM_BAR / m_iNumOfBtnItem;
    }

    int iX = 0;
    for (int i = 0; i < m_iNumOfBtnItem; ++i)
    {
        if (NULL != m_pBtnItem[i])
        {
            int iAdd = (i == m_iNumOfBtnItem - 1) ? 0 : 1;
            m_pBtnItem[i]->setGeometry(iX, 0, iWidth + iAdd, HEIGHT_OF_BTN_ITEM_BAR);
        }
        iX += iWidth;
    }
}

void qCallManagerBar::OnBtnClick(QString strClickAction)
{
    //int iIndex = FindBtnByClickAction(strClickAction);
    //// 点击同一个不再处理
    //if (iIndex == m_iCurSelectItem)
    //{
    //  return;
    //}
    //
    //UpdateBtnBarItem(iIndex);

    //if (m_iCurSelectItem >= 0 && m_iCurSelectItem < m_iNumOfBtnItem
    //  && NULL != m_pBtnItem[m_iCurSelectItem])
    //{
    //  emit ClickAction(m_pBtnItem[m_iCurSelectItem]->GetClickAction());
    //}
    emit ClickAction(strClickAction);
}

// 添加BtnItem
bool qCallManagerBar::AddBtnItem(qCallManagerBarItem * pBtnItem)
{
    if (m_iNumOfBtnItem < MaxNumOfBtnItem && NULL != pBtnItem
            && 0 <= m_iNumOfBtnItem)
    {
        m_pBtnItem[m_iNumOfBtnItem++] = pBtnItem;
        pBtnItem->setObjectName(QString("BtnItem%1").arg(m_iNumOfBtnItem));
        //QString str = "Click" + m_iNumOfBtnItem;
        //m_pBtnItem[m_iNumOfBtnItem - 1]->SetClickAction(str);

        connect(pBtnItem, SIGNAL(ClickAction(QString)), this, SLOT(OnBtnClick(QString)));
        SetBtnItemLoaction();
        return true;
    }
    else
    {
        return false;
    }
}

void qCallManagerBar::AddBtnItems(qCallManagerBarItem * pBtnItems[], int iCnt)
{
    for (int i = 0; i < iCnt; ++i)
    {
        if (NULL != pBtnItems[i])
        {
            AddBtnItem(pBtnItems[i]);
        }
    }
}

// 设置选中的Index
bool qCallManagerBar::SetSelectIndex(const int iIndex)
{
    if (iIndex < m_iNumOfBtnItem && iIndex >= 0
            && NULL != m_pBtnItem[iIndex])
    {
        return UpdateBtnBarItem(iIndex);
    }
    return false;
}

// 设置选中的Action
bool qCallManagerBar::SetSelectAction(const QString & strSelectAction)
{
    int iIndex = FindBtnByClickAction(strSelectAction);
    return SetSelectIndex(iIndex);
}

// 更新BtnBarItem状态、设置选中项
bool qCallManagerBar::UpdateBtnBarItem(int iIndex)
{
    if (-1 == iIndex)
    {
        return false;
    }
    if (-1 != m_iCurSelectItem)
    {
        m_pBtnItem[m_iCurSelectItem]->SetDrawBg(false);
        m_pBtnItem[m_iCurSelectItem]->update();
    }

    m_iCurSelectItem = iIndex;
    m_pBtnItem[m_iCurSelectItem]->SetDrawBg(true);
    m_pBtnItem[m_iCurSelectItem]->update();

    return true;
}
