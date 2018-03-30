#include <QtGui>
#include "contactphotolistitem.h"
#include "baseui/t4xpicpath.h"
#include "imagemanager/modimagemanager.h"
#include "uikernel/qwidgetutility.h"

namespace
{
#define CONTACT_PHOTO_ITEM_POS_HEIGHT 130
#define CONTACT_PHOTO_ITEM_FONT_SIZE 16

#define CONTACT_PHOTO_ITEM_POS_BTN_LEFT 0
#define CONTACT_PHOTO_ITEM_POS_BTN_TOP 2
#define CONTACT_PHOTO_ITEM_POS_BTN_WIDTH 122
#define CONTACT_PHOTO_ITEM_POS_BTN_HEIGHT 122

#define CONTACT_PHOTO_ITEM_POS_BTN_ICON_LEFT 86
#define CONTACT_PHOTO_ITEM_POS_BTN_ICON_TOP 76
#define CONTACT_PHOTO_ITEM_POS_BTN_ICON_WIDTH 28
#define CONTACT_PHOTO_ITEM_POS_BTN_ICON_HEIGHT 28

#define CONTACT_PHOTO_ITEM_POS_BTN_IMAGE_LEFT 10
#define CONTACT_PHOTO_ITEM_POS_BTN_IMAGE_TOP 0
#define CONTACT_PHOTO_ITEM_POS_BTN_IMAGE_WIDTH 108
#define CONTACT_PHOTO_ITEM_POS_BTN_IMAGE_HEIGHT 108

#define CONTACT_PHOTO_ITEM_POS_BTN_TEXT_LEFT 10
#define CONTACT_PHOTO_ITEM_POS_BTN_TEXT_TOP 106
#define CONTACT_PHOTO_ITEM_POS_BTN_TEXT_WIDTH 108
#define CONTACT_PHOTO_ITEM_POS_BTN_TEXT_HEIGHT 22
}

CContactPhotoListItem::CContactPhotoListItem(QWidget * parent/* = NULL*/,
        IListItemAction * pAction/* = NULL*/)
    : CListItem(LIST_ITEM_TYPE_CONTACT_PHOTO, parent, pAction)
    , m_btnBar(this, true)
    , m_nSelIndex(-1)
{
    setObjectName("CContactPhotoListItem");
    setGeometry(0, 0, 0, CONTACT_PHOTO_ITEM_POS_HEIGHT);
    qWidgetUtility::setFontStyle(this, CONTACT_PHOTO_ITEM_FONT_SIZE);

    m_btnBar.SetBtnCount(CONTACT_PHOTO_COUNT_PER_ROW);

    for (int i = 0; i < CONTACT_PHOTO_COUNT_PER_ROW; ++i)
    {
        CTitleButton * pBtnImage = new CTitleButton(this);

        if (NULL == pBtnImage)
        {
            m_arrImage[i] = NULL;
        }
        else
        {
            m_arrImage[i] = pBtnImage;
        }

        m_btnBar.SetBtn(i, pBtnImage);
    }

    ClearPhoto();
}

CContactPhotoListItem::~CContactPhotoListItem(void)
{
}

int CContactPhotoListItem::GetPhotoCountPerRow()
{
    return CONTACT_PHOTO_COUNT_PER_ROW;
}

int CContactPhotoListItem::GetContactPhotoItemType()
{
    return LIST_ITEM_TYPE_CONTACT_PHOTO;
}

void CContactPhotoListItem::ClearPhoto()
{
    m_nSelIndex = -1;

    for (int i = 0; i < CONTACT_PHOTO_COUNT_PER_ROW; ++i)
    {
        CTitleButton * pBtnImage = m_arrImage[i];

        if (NULL != pBtnImage)
        {
            pBtnImage->SetRect(0, 0, 0, 0);
        }

        m_arrPath[i] = "";
        m_arrName[i] = QLatin1String("");
        m_arrValid[i] = false;
    }
}

void CContactPhotoListItem::SetPhoto(int nIndex, const yl::string & strPhotoPath,
                                     const QString & strPhotoName)
{
    if (nIndex < 0 || nIndex >= CONTACT_PHOTO_COUNT_PER_ROW)
    {
        return;
    }

    int nLeft = CONTACT_PHOTO_ITEM_POS_BTN_LEFT + nIndex * CONTACT_PHOTO_ITEM_POS_BTN_WIDTH;
    CTitleButton * pBtnImage = m_arrImage[nIndex];

    if (NULL != pBtnImage)
    {
        pBtnImage->SetRect(nLeft, CONTACT_PHOTO_ITEM_POS_BTN_TOP,
                           CONTACT_PHOTO_ITEM_POS_BTN_WIDTH, CONTACT_PHOTO_ITEM_POS_BTN_HEIGHT);

        int nIconLeft = nLeft + CONTACT_PHOTO_ITEM_POS_BTN_ICON_LEFT;
        int nIconTop = CONTACT_PHOTO_ITEM_POS_BTN_TOP + CONTACT_PHOTO_ITEM_POS_BTN_ICON_TOP;

        pBtnImage->SetIconRect(nIconLeft, nIconTop, CONTACT_PHOTO_ITEM_POS_BTN_ICON_WIDTH,
                               CONTACT_PHOTO_ITEM_POS_BTN_ICON_HEIGHT);

        int nTextLeft = nLeft + CONTACT_PHOTO_ITEM_POS_BTN_TEXT_LEFT;
        int nTextTop = CONTACT_PHOTO_ITEM_POS_BTN_TOP + CONTACT_PHOTO_ITEM_POS_BTN_TEXT_TOP;

        pBtnImage->SetTextRect(nTextLeft, nTextTop, CONTACT_PHOTO_ITEM_POS_BTN_TEXT_WIDTH,
                               CONTACT_PHOTO_ITEM_POS_BTN_TEXT_HEIGHT);
    }

    m_arrPath[nIndex] = strPhotoPath;
    m_arrName[nIndex] = strPhotoName;
    m_arrValid[nIndex] = true;
}

void CContactPhotoListItem::SetSelPhotoIndex(int nIndex)
{
    m_nSelIndex = nIndex;
}

int CContactPhotoListItem::GetSelPhotoIndex() const
{
    return m_nSelIndex;
}

void CContactPhotoListItem::mousePressEvent(QMouseEvent * pEvent)
{
    CButtonWrapper * pDownBtn = m_btnBar.GetDownBtn();
    CButtonWrapper * pPressBtn = m_btnBar.ProcessMousePress(pEvent);

    if (NULL != pDownBtn && NULL == pPressBtn)
    {
        DoAction(LIST_ITEM_ACTION_UNSELECT_IMAGE);
    }
    else if (pDownBtn != pPressBtn)
    {
        int nIndex = m_btnBar.GetDownBtnIndex();

        if (nIndex >= 0 && nIndex < CONTACT_PHOTO_COUNT_PER_ROW)
        {
            DoAction(LIST_ITEM_ACTION_SELECT_IMAGE_BEGIN + nIndex);
        }
    }
}

void CContactPhotoListItem::mouseReleaseEvent(QMouseEvent * pEvent)
{
    CButtonWrapper * pDownBtn = m_btnBar.GetDownBtn();
    CTitleButton * pBtn = (CTitleButton *)m_btnBar.ProcessMouseRelease(pEvent);

    if (NULL == pBtn)
    {
        if (NULL != pDownBtn)
        {
            DoAction(LIST_ITEM_ACTION_UNSELECT_IMAGE);
        }
        return;
    }

    for (int i = 0; i < CONTACT_PHOTO_COUNT_PER_ROW; ++i)
    {
        if (pBtn == m_arrImage[i])
        {
            m_nSelIndex = i;

            DoAction(LIST_ITEM_ACTION_CHOOSE_IMAGE_BEGIN + m_nSelIndex);
            break;
        }
    }
}

void CContactPhotoListItem::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);

    for (int i = 0; i < CONTACT_PHOTO_COUNT_PER_ROW; ++i)
    {
        if (!m_arrValid[i])
        {
            break;
        }

        CTitleButton * pBtnImage = m_arrImage[i];

        if (NULL == pBtnImage)
        {
            break;
        }

        yl::string & strPath = m_arrPath[i];
        QString & strName = m_arrName[i];

        QPixmap pmThemeImage = THEME_GET_BMP(strPath.c_str());
        QRect rcImage = pBtnImage->GetRect();

        rcImage.setRect(rcImage.left() + CONTACT_PHOTO_ITEM_POS_BTN_IMAGE_LEFT,
                        rcImage.top() + CONTACT_PHOTO_ITEM_POS_BTN_IMAGE_TOP,
                        CONTACT_PHOTO_ITEM_POS_BTN_IMAGE_WIDTH, CONTACT_PHOTO_ITEM_POS_BTN_IMAGE_HEIGHT);

        if (pmThemeImage.isNull())
        {
            QPixmap pmImage = STOCK_GET_BMP(strPath.c_str());

            if (!pmImage.isNull())
            {
                stylePainter.drawPixmap(rcImage, pmImage);
            }
        }
        else
        {
            stylePainter.drawPixmap(rcImage, pmThemeImage);
        }

        if (m_nSelIndex == i || pBtnImage->IsDown())
        {
            pBtnImage->PaintVButton(stylePainter, "", PIC_ICON_PHOTO_SELECTED, strName);
        }
        else
        {
            pBtnImage->PaintVButton(stylePainter, "", "", strName);
        }
    }
}
