#include "qpushbuttonutility.h"
#include "imagemanager/modimagemanager.h"

QPushButtonUtility::QPushButtonUtility(void)
{
}

QPushButtonUtility::~QPushButtonUtility(void)
{
}


void QPushButtonUtility::SetButtonNoFrame(QPushButton * pBtn)
{
    if (pBtn != NULL)
    {
        pBtn->setFlat(true);
    }
}

void QPushButtonUtility::SetButtonIcon(QPushButton * pBtn, LPCSTR lpcsIcon)
{
    QPixmap map = THEME_GET_BMP(lpcsIcon);

    if (!map.isNull())
    {
        pBtn->setIcon(map);
        pBtn->setIconSize(QSize(map.width(), map.height()));
    }
}

// 设置Btn透明
void QPushButtonUtility::SetButtonTransparent(QPushButton * pBtn)
{
    if (pBtn != NULL)
    {
        pBtn->setFlat(true);
        QPalette pal = pBtn->palette();
        pal.setColor(QPalette::Shadow, QColor(0, 0, 0, 0));
        pal.setColor(QPalette::Button, QColor(0, 0, 0, 0));
        pBtn->setPalette(pal);
    }
}
