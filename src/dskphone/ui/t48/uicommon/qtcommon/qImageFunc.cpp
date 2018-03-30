#include "ylstl/ylstring.h"
#include "commonunits/commonunits_def.h"
#include <QImage>
#include "uicommon/imagemanager/modimagemanager.h"
#include "uicommon/imagemanager/imagecachemanagerdefine.h"
#include "baselibs/include/devicelib/phonedevice.h"
#include "qmisc.h"
#include <sys/statfs.h>
#include <QFile>

namespace
{
#define WALL_PAPER_LENGTH_T54   480
#define WALL_PAPER_HEIGHT_T54   480
#define WALL_PAPER_LENGTH_T52   320
#define WALL_PAPER_HEIGHT_T52   480

#define DEFAULT_SCALE_RATIO     2
#define TEMPORARY_IMAGE_PATH    TEMP_PATH "image/"

#define DATA_REMAIN_SPACE       (1024 * 2048)//2M

#define TMP_IMAGE_NAME          "TmpImage."
}

bool LoadImageByPath(QImage & refImage, const QString & strImagePath);
IMAGE_SCALE_RETURN_TYPE ImageSave(const QString & strSrcFile, QString strDstFile,
                                  QImage & imageScaleTmp, const long & lPartionFreeSize);

int Image_Scaled(const yl::string & strSrcFile, const yl::string & strDstFile, int eImagetype,
                 int nScaleXDef /* = 0 */, int nScaleYDef /* = 0 */)
{
    struct statfs stuSpace;
    long lPartionFreeSize;
    //获取分区剩余空间
    QString strCheckFile = ROOT_PATH + toQString(strDstFile).section("/", 1, 1) + "/";
    statfs(strCheckFile.toUtf8().data(), &stuSpace);
    lPartionFreeSize = stuSpace.f_bsize * stuSpace.f_bfree;
    if (lPartionFreeSize < DATA_REMAIN_SPACE)//空间不足2M
    {
        return IMAGE_NO_ENOUGH_SPACE;
    }

    system("mkdir -p " TEMP_PATH "image");//创建缓存目录

    IMAGE_RECEIVABLE_TYPE eImageReceivable = Image_IsImageSizeReceivable(strSrcFile.c_str());
    switch (eImageReceivable)
    {
    case IRT_ERROR:
        {
            return IMAGE_SIZE_ERROR;
            break;
        }
    case IRT_SIZE_OVER:
        {
            return IMAGE_SIZE_OVER;
            break;
        }
    case IRT_RECEIVABLE:
        {
            break;
        }
    default:
        return IMAGE_SIZE_ERROR;
        break;
    }
    QImage imageTmp;

    if (!LoadImageByPath(imageTmp, toQString(strSrcFile)))
    {
        return IMAGE_TYPE_ERROR;
    }
    if (imageTmp.isNull())
    {
        return IMAGE_TYPE_CANNOT_READ;
    }

    int nScaleX = 0;
    int nScaleY = 0;

    if (devicelib_GetPhoneType() == PT_T52 && WALL_PAPER_IMAGE == IMAGE_SCALED_TYPE(eImagetype))
    {
        nScaleX = WALL_PAPER_LENGTH_T52;
        nScaleY = WALL_PAPER_HEIGHT_T52;
    }
    else if (devicelib_GetPhoneType() == PT_T54S && WALL_PAPER_IMAGE == IMAGE_SCALED_TYPE(eImagetype))
    {
        nScaleX = WALL_PAPER_LENGTH_T54;
        nScaleY = WALL_PAPER_HEIGHT_T54;
    }
    else
    {
        nScaleX = nScaleXDef;
        nScaleY = nScaleYDef;
    }

    IMAGE_SCALE_RETURN_TYPE eResult = IMAGE_SCALED_FAILE;//记录结果用

    //若长宽均小于规定值，直接对图片进行保存
    if (imageTmp.width() <= nScaleX && imageTmp.height() <= nScaleY)
    {
        eResult = ImageSave(toQString(strSrcFile), toQString(strDstFile), imageTmp, lPartionFreeSize);
    }
    //如若长宽均大于规定值
    else if (imageTmp.width() > nScaleX && imageTmp.height() > nScaleY)
    {
        if (0 == nScaleX || 0 == nScaleY)//规定长宽为0，返回
        {
            return IMAGE_SIZE_HAVE_ZERO;
        }
        //分别记录图片长宽与规定长宽的比例
        float fRatioX = float(imageTmp.width()) / float(nScaleX);
        float fRatioY = float(imageTmp.height()) / float(nScaleY);

        if (0 != fRatioX && 0 != fRatioY && (fRatioY / fRatioX >= DEFAULT_SCALE_RATIO
                                             || fRatioX / fRatioY >=
                                             DEFAULT_SCALE_RATIO))//如若图片长宽比例大于2，另外处理，防止用户上传神奇的图片，比如480*4000
        {
            float fRealScale = (fRatioX > fRatioY ? fRatioX : fRatioY) /
                               DEFAULT_SCALE_RATIO;//真实的缩放比例，针对规定比例奇葩的图片，缩放后图片最多为规定值的2倍
            if (0 == fRealScale)
            {
                return IMAGE_SIZE_HAVE_ZERO;
            }
            QImage image = imageTmp.scaled(QSize(imageTmp.width() / fRealScale,
                                                 imageTmp.height() / fRealScale));
            eResult = ImageSave(toQString(strSrcFile), toQString(strDstFile), image, lPartionFreeSize);
        }
        else//正常场景使用Qt自带的缩放
        {
            QImage image = imageTmp.scaled(QSize(nScaleX, nScaleY), Qt::KeepAspectRatioByExpanding);
            eResult = ImageSave(toQString(strSrcFile), toQString(strDstFile), image, lPartionFreeSize);
        }
    }
    else
    {
        float fRatioX = float(imageTmp.width()) / float(nScaleX);
        float fRatioY = float(imageTmp.height()) / float(nScaleY);
        if (fRatioY >= DEFAULT_SCALE_RATIO)//比例过于大则使用容错处理
        {
            QImage image = imageTmp.scaled(QSize(imageTmp.width(), nScaleY));
            eResult = ImageSave(toQString(strSrcFile), toQString(strDstFile), image, lPartionFreeSize);
        }
        else if (fRatioX >= DEFAULT_SCALE_RATIO)//比例过于大则使用容错处理
        {
            QImage image = imageTmp.scaled(QSize(nScaleX, imageTmp.height()));
            eResult = ImageSave(toQString(strSrcFile), toQString(strDstFile), image, lPartionFreeSize);
        }
        else
        {
            eResult = ImageSave(toQString(strSrcFile), toQString(strDstFile), imageTmp, lPartionFreeSize);
        }
    }
    return eResult;
}

bool LoadImageByPath(QImage & refImage, const QString & strImagePath)
{
    QString strRealImagePath = QString::fromLocal8Bit(strImagePath.toUtf8().data());
    if (!QFile::exists(strRealImagePath))
    {
        return false;
    }

    if (refImage.load(strRealImagePath))
    {
        return true;
    }

    // 取文件后缀名
    QString strFileType = QFileInfo(strRealImagePath).suffix().toLower();

    // 图片实际扩展名跟保存扩展名不一致时，容错方案
    if (strFileType != "jpg" && refImage.load(strRealImagePath, "JPG"))
    {
        return true;
    }

    if (strFileType != "jpeg" && refImage.load(strRealImagePath, "JPEG"))
    {
        return true;
    }

    if (strFileType != "png" && refImage.load(strRealImagePath, "PNG"))
    {
        return true;
    }

    if (strFileType != "bmp" && refImage.load(strRealImagePath, "BMP"))
    {
        return true;
    }

    return false;
}

IMAGE_SCALE_RETURN_TYPE ImageSave(const QString & strSrcFile, QString strDstFile,
                                  QImage & imageScaleTmp, const long & lPartionFreeSize)
{
    // 取文件名
    QString strSuffixName = QFileInfo(strSrcFile).suffix();
    QString strImageTmpName = TEMPORARY_IMAGE_PATH + QString(TMP_IMAGE_NAME) + strSuffixName;

    QString strImageName = QString::fromLocal8Bit(strImageTmpName.toUtf8().data());

    bool bResult = imageScaleTmp.save(strImageName);

    if (!bResult)
    {
        return IMAGE_SCALED_FAILE;
    }

    //为linux环境，进行路径修改
    strDstFile.replace(" ", "\\ ");
    strDstFile.replace("(", "\\(");
    strDstFile.replace(")", "\\)");
    strDstFile.replace(";", "\\;");
    strDstFile.replace("'", "\\'");
    strDstFile.replace("&", "\\&");

    long lImageSize = long(QFileInfo(strImageName).size());//压缩后的图像文件大小

    if (lPartionFreeSize - lImageSize < DATA_REMAIN_SPACE)
    {
        QString strDelete = "rm -rf " + strImageTmpName;
        system(strDelete.toUtf8().data());
        return IMAGE_NO_ENOUGH_SPACE;
    }

    QString strShell = "mv " + strImageTmpName + " " + strDstFile;//移动指令
    system(strShell.toUtf8().data());

    return IMAGE_SCALED_SUCCESS;

}

