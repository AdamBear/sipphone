#include <QtGui>
#include "framelist.h"
#include "framelisttitle.h"
#include "t4xpicpath.h"
#include "imagemanager/modimagemanager.h"

CFrameListTitle::CFrameListTitle(int nType, CFrameList * pFrameList/* = NULL*/,
                                 IFrameTitleAction * pAction/* = NULL*/)
    : QWidget(pFrameList)
    , m_pAction(pAction)
    , m_nType(nType)
    , m_bShowBg(false)
    , m_clrBg(Qt::darkGray)
    , m_strBgPic("")
{
    setObjectName("CFrameListTitle");
}

CFrameListTitle::~CFrameListTitle()
{

}

void CFrameListTitle::SetAction(IFrameTitleAction * pAction)
{
    m_pAction = pAction;
}

// 是否显示背景
void CFrameListTitle::SetShowBG(bool bShow)
{
    m_bShowBg = bShow;
}

void CFrameListTitle::SetBgColor(const QColor & color)
{
    m_clrBg = color;
}

void CFrameListTitle::SetBgPic(const yl::string & strBgPic)
{
    m_strBgPic = strBgPic;
}

int CFrameListTitle::GetType() const
{
    return m_nType;
}

void CFrameListTitle::RefreshTitle()
{
    Relayout();

    update();
}

void CFrameListTitle::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);

    PaintBG(stylePainter);
}


void CFrameListTitle::PaintBG(QStylePainter & stylePainter)
{
    if (!m_bShowBg)
    {
        return;
    }

    QRect rcTitle = rect();

    if (!m_strBgPic.empty())
    {
        QPixmap pmBg = THEME_GET_BMP(PIC_GENERAL_LIST_TITLE_BG);

        if (!pmBg.isNull())
        {
            stylePainter.drawPixmap(rcTitle, pmBg);
            return;
        }
    }

    stylePainter.fillRect(rcTitle, m_clrBg);
}

void CFrameListTitle::DoAction(int nAction)
{
    if (NULL == m_pAction || !isVisible())
    {
        return;
    }

    CFrameList * pFrameList = (CFrameList *)parent();

    if (NULL == pFrameList)
    {
        return;
    }

    m_pAction->OnTitleAction(pFrameList, this, nAction);
}

void CFrameListTitle::resizeEvent(QResizeEvent * pEvent)
{
    Relayout();
}

void CFrameListTitle::Relayout()
{

}
