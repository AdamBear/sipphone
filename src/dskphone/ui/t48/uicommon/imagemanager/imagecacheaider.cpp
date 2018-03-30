#include "imagecacheaider.h"
#include "imagethemeinfo.h"
// #include "usbinterface/cusbuilogic.h"

CImageBaseInfo * GetImageInfoByType(IMAGE_TYPE eType)
{
    CImageBaseInfo * pImageInfo = NULL;
    switch (eType)
    {
    case DIT_TALK:
        {
            pImageInfo = new CImageCachePixmap();
            pImageInfo->SetImageSize(QSize(212, 212));
        }
        break;
    case DIT_DIR:
        {
            pImageInfo = new CImageNoCache();
            pImageInfo->SetImageSize(QSize(114, 114));
        }
        break;
    case DIT_FAVORITE:
        {
            pImageInfo = new CImageNoCache();
            pImageInfo->SetImageSize(QSize(114, 114));
        }
        break;
    case DIT_SEARCH:
        {
            pImageInfo = new CImageNoCache();
            pImageInfo->SetImageSize(QSize(114, 114));
        }
        break;
    case DIT_USB:
        {
            pImageInfo = new CImageNoCache();
            pImageInfo->SetImageSize(QSize(58, 58));
        }
        break;
    case DIT_ACCOUNT:
        {
            pImageInfo = new CImageCachePixmap();
            pImageInfo->SetImageSize(QSize(62, 62));
        }
        break;
    case DIT_PROPORTION:
        {
            pImageInfo = new CImageNoCache();
            if (NULL != pImageInfo)
            {
                QPixmap pic = pImageInfo->GetPixmap();
                if (!pic.isNull())
                {
                    QRect rc;// = g_pUSBUILogic->CalculateImageRect(pic.rect().width(), pic.rect().height());
                    pImageInfo->SetImageSize(QSize(rc.width(), rc.height()));
                }
            }
        }
        break;
    case DIT_THEME:
    case DIT_SOCKET:
        {
            pImageInfo = new ImageThemeInfo();
        }
        break;
    case DIT_NOCACHE:
        {
            pImageInfo = new CImageNoCache();
        }
        break;
    default:
        {
            pImageInfo = NULL;
        }
        break;
    }

    if (NULL != pImageInfo)
    {
        pImageInfo->SetType(eType);
    }

    return pImageInfo;
}
