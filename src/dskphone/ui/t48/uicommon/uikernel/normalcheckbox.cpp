#include <QtGui>
#include "normalcheckbox.h"
#include "imagemanager/modimagemanager.h"

qNormalCheckBox::qNormalCheckBox(QWidget * parent/* = NULL*/)
    : QCheckBox(parent)
    , m_strNormalPic("")
    , m_strSelectPic("")
{

}

qNormalCheckBox::~qNormalCheckBox()
{

}

void qNormalCheckBox::SetCheckBoxPic(const char * pszNormal, const char * pszSelect)
{
    if (NULL != pszNormal)
    {
        m_strNormalPic = pszNormal;
    }

    if (NULL != pszSelect)
    {
        m_strSelectPic = pszSelect;
    }
}

void qNormalCheckBox::SetCheckBoxPic(const yl::string & strNormal, const yl::string & strSelect)
{
    m_strNormalPic = strNormal;
    m_strSelectPic = strSelect;
}

void qNormalCheckBox::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);

    QRect rcCheckBox = rect();

    if (Qt::Checked == checkState())
    {
        QPixmap pmCheck = THEME_GET_BMP(m_strSelectPic.c_str());

        if (!pmCheck.isNull())
        {
            stylePainter.drawPixmap(rcCheckBox, pmCheck);
        }
    }
    else
    {
        QPixmap pmCheck = THEME_GET_BMP(m_strNormalPic.c_str());

        if (!pmCheck.isNull())
        {
            stylePainter.drawPixmap(rcCheckBox, pmCheck);
        }
    }
}
