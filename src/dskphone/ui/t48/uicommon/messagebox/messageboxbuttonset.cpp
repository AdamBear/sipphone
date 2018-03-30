#include "messageboxbuttonset.h"

#include <ETLLib/ETLLib.hpp>
#include "baseui/maskframe.h"
#include "messageboxbase.h"
#include "uicommon.h"
#include "keymap.h"
#include "imagemanager/modimagemanager.h"
#include "baseui/t4xpicpath.h"
#include <QPalette>
#include "uimanager/moduimanager.h"
#include "uimanager/uimanager_common.h"

#define MESSAGEBOX_LINE_WIDTH 1
#define MESSAGEBOX_LINE_HEIGHT 39
#define MESSAGEBOX_BTN_TOP 7

static QColor LINE_COLOR(150, 150, 150, 255);

CMessageBoxButtonSet::CMessageBoxButtonSet(QWidget * parent)
    : QWidget(parent)
    , m_iMaxBtnCount(0)
    , m_iCurrentBtnCount(0)
{
    setObjectName("CMessageBoxButtonSet");
    m_btnVec.clear();
}

CMessageBoxButtonSet::~CMessageBoxButtonSet()
{
}

void CMessageBoxButtonSet::SetBackGround(yl::string strBackground)
{
    m_strBackground = strBackground;
}

void CMessageBoxButtonSet::AddButton(qUIPushButton * pBtn)
{
    if (NULL == pBtn || m_iCurrentBtnCount >= m_iMaxBtnCount)
    {
        //达到最大限制数则不添加
        return;
    }
    m_iCurrentBtnCount++;
    pBtn->setParent(this);
    m_btnVec.push_back(pBtn);
}

void CMessageBoxButtonSet::DetachAllBtn()
{
    for (int i = 0; i < m_btnVec.size(); i++)
    {
        m_btnVec[i]->hide();
    }
    m_btnVec.clear();
    m_iCurrentBtnCount = 0;
}

bool CMessageBoxButtonSet::eventFilter(QObject * pObj, QEvent * pEvent)
{
    return false;
}

void CMessageBoxButtonSet::paintEvent(QPaintEvent * pEvent)
{
    //计算每个button的大小
    int iBtnWidth = (rect().width() - (m_btnVec.size() - 1) * MESSAGEBOX_LINE_WIDTH) / m_btnVec.size();
    int iBtnHeight = rect().height();

    //起始位置
    int iLeft = 0;
    int iTop = 0;

    QPainter painter(this);
    //画背景
    painter.drawPixmap(0, 0, width(), height(), THEME_GET_BMP(m_strBackground.c_str()));
    painter.setPen(LINE_COLOR);

    int i = 0;
    //画分割线，最后一个button后面不用画
    for (i = 0; i < m_btnVec.size() - 1; ++i)
    {
        if (NULL == m_btnVec[i])
        {
            continue;
        }

        //设置位置
        QRect btnRect(iLeft, iTop, iBtnWidth, iBtnHeight);
        m_btnVec[i]->setGeometry(btnRect);

        iLeft += iBtnWidth;

        painter.drawLine(iLeft, MESSAGEBOX_BTN_TOP, iLeft, MESSAGEBOX_BTN_TOP + MESSAGEBOX_LINE_HEIGHT - 1);

        iLeft += MESSAGEBOX_LINE_WIDTH;
    }
    //画最后一个button
    QRect btnRect(iLeft, iTop, iBtnWidth, iBtnHeight);
    m_btnVec[i]->setGeometry(btnRect);
}

