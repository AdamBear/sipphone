#include "imagecachemanager.h"
#include "imagecacheaider.h"
#include <QFileInfo>
#include "interfacedefine.h"
#include "imagethemeinfo.h"
#include "commonapi/systemhelperapi.h"
#include "qtcommon/qmisc.h"
#include "etlmsghandler/modetlmsghandler.h"
// #include "usbinterface/cusbuilogic.h"
#ifdef TEST_DELETE_MOST_IMAGES_EFFICIENTY
#include <sstream>
#endif
#include "imagepoint9manager.h"
#include "imagepathconvertor.h"

namespace
{
#define CACHE_MAX_COUNTS         8
#define CACHE_ELASTICITY_COUNTS  8

// 单位 Bit
#define CACHE_MAX_MEM_SIZE    3100000
#define CACHE_MEM_ELASTICITY  500000

#if IF_FEATURE_DUMP_CACHE_INFO
#define DUMP_CACHE_INFO_TIME 600000
#endif

#define DELETE_IMAGE_LATER_TIME 100

// 允许缓存到内存中的最大图片分辨率
// 屏幕大小分辨率*2+1
#define MAX_CACHE_RESOLUTION            768001

// 允许加入的最大图片分辨率
// 从测试结果得到的最大不死机分辨率
#define MAX_PICTURE_RESOLUTION          2000000
}

LRESULT CImageCacheManager::OnMessage(msgObject & refMessage)
{
    if (CONFIG_MSG_BCAST_CHANGED == refMessage.message
            && (ST_WALLPAPER == refMessage.wParam || ST_XLOAD_CONTACT_IMAGE == refMessage.wParam
                || ST_SCREEN_SAVER_PIC == refMessage.wParam)
            && 1 == refMessage.lParam)
    {
        const char * pszImgPath = (const char *)refMessage.GetExtraData();

        if (NULL != pszImgPath && pszImgPath != "")
        {
            UIKERNEL_INFO("[CImageCacheManager::OnMessage][%s]", pszImgPath);
            GetImageCacheManager().RemoveImage(std::string(pszImgPath));
        }
        else
        {
            UIKERNEL_INFO("CImageCacheManager Remove All NotTheme Image.");
            GetImageCacheManager().ClearImageResourceByImageType(DIT_NOTTHEME);
        }

        return TRUE;
    }

    return FALSE;
}

CImageCacheManager::CImageCacheManager()
#if ADD_CACHE_ALGORITHM
    : m_ImageCache(CACHE_MAX_COUNTS, CACHE_ELASTICITY_COUNTS, CACHE_MAX_MEM_SIZE, CACHE_MEM_ELASTICITY)
#endif
{
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                          &CImageCacheManager::OnMessage);

    connect(&m_TimeToDelete, SIGNAL(timeout()), this, SLOT(OnTimerOut()));

#ifdef IF_FEATURE_DUMP_CACHE_INFO
    connect(&m_timerDumpCacheInfo, SIGNAL(timeout()), this, SLOT(OnDumpCacheInfoTimeout()));

    m_timerDumpCacheInfo.start(DUMP_CACHE_INFO_TIME);
#endif
}

CImageCacheManager::~CImageCacheManager()
{
    // 释放Map的图片资源
    ClearImageResource(false);
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                            &CImageCacheManager::OnMessage);
}

void CImageCacheManager::ClearImageResource(bool bLync /*= true*/)
{
#ifdef TEST_DELETE_MOST_IMAGES_EFFICIENTY
    // 遍历判断图片是否缓存
    ImageInfoIter iterMap = m_mapImageInfo.begin();
    for (int i = 0 ; iterMap != m_mapImageInfo.end(), i < 1500; ++i)
    {
        ReleaseImageInfo(iterMap->second);
        m_mapImageInfo.erase(iterMap++);
    }
#else
    // 遍历判断图片是否缓存
    ImageInfoIter iterMap = m_mapImageInfo.begin();
    if (bLync)
    {
        for (; iterMap != m_mapImageInfo.end();)
        {
            if (NULL != iterMap->second && iterMap->second->IsDeleteWhileClear())
            {
                ReleaseImageInfo(iterMap->second, true);
                m_mapImageInfo.erase(iterMap++);
            }
            else
            {
                ++iterMap;
            }
        }
    }
    else
    {
        for (; iterMap != m_mapImageInfo.end(); ++iterMap)
        {
            ReleaseImageInfo(iterMap->second, true);
        }

        m_mapImageInfo.clear();
    }
#endif
}

// 根据类型释放缓存的图片资源
void CImageCacheManager::ClearImageResourceByImageType(IMAGE_TYPE eType /*= DIT_NONE*/)
{
    if (DIT_NONE == eType)
    {
        return;
    }

    std::map<std::string, lru::Node<std::string, CImageCachePixmap>*> cacheDa =
        m_ImageCache.getCache();
    ImageCacheIter iterCache = cacheDa.begin();
    for (; iterCache != cacheDa.end(); ++iterCache)
    {
        if (NULL != iterCache->second && (eType & iterCache->second->value.GetType()))
        {
            m_ImageCache.remove(iterCache->first);
        }
    }

    // 遍历判断图片是否缓存
    ImageInfoIter iterMap = m_mapImageInfo.begin();

    for (; iterMap != m_mapImageInfo.end();)
    {
        if (NULL != iterMap->second && (eType & iterMap->second->GetType()))
        {
            ReleaseImageInfo(iterMap->second);
            m_mapImageInfo.erase(iterMap++);
        }
        else
        {
            ++iterMap;
        }
    }
}

int CImageCacheManager::GetCacheImageCount() const
{
    return m_mapImageInfo.size();
}

bool CImageCacheManager::IsImageInCache(const std::string & strPath)
{
    bool bIn = m_mapImageInfo.find(strPath) != m_mapImageInfo.end();
#if ADD_CACHE_ALGORITHM
    // 先从缓存算法中获取图片
    CImageCachePixmap tmpImageInfo;
    if (m_ImageCache.tryGet(strPath, tmpImageInfo))
    {
        bIn = true;
    }
#endif
    return bIn;
}

// 加载文件夹下的图片资源
bool CImageCacheManager::LoadTheme(const QString & strFileImagePath)
{
    // 判断路径的正确性
    if (!CheckPath(strFileImagePath.toUtf8().data()) || !QFile::exists(strFileImagePath))
    {
        UIKERNEL_INFO("[CImageCacheManager::LoadTheme] File(%s) not Exsit!",
                      strFileImagePath.toUtf8().data());
        return false;
    }
    UIKERNEL_INFO("[CImageCacheManager::LoadTheme](filepath[%s])", strFileImagePath.toUtf8().data());

    QFileInfo fileInfo(strFileImagePath);
    if (!fileInfo.exists())
    {
        return false;
    }

    // 判断传入的路径是否是路径
    if (fileInfo.isDir())
    {
        // 设置加载图片的路径
        yl::string strImageFilePath = strFileImagePath.toUtf8().data();

        // 若路径为文件夹
        // 读取路径下面所有的图片名到list
        YLList<yl::string> listFilePath;
        commonAPI_getFilesInDirectoryByExt(&listFilePath, strImageFilePath, "png,jpg,bmp,gif", true, false,
                                           true);
        UIKERNEL_INFO("[CImageCacheManager::LoadTheme] size[%d]", listFilePath.size());
        // 遍历list,加载图片
        YLList<yl::string>::iterator it = listFilePath.begin();
        for (/* */; it != listFilePath.end(); ++it)
        {
            yl::string strTemp = *it;
            AddImage(strTemp.c_str(), DIT_THEME);
        }
    }
    else
    {
        // 若路径不是文件夹，尝试加载
        AddImage(strFileImagePath.toUtf8().data(), DIT_THEME);
    }

    return true;
}

// 释放缓存的主题图片资源
void CImageCacheManager::UnloadFreeTheme()
{
    ClearImageResource(false);
}

// 加载新的主题
bool CImageCacheManager::ReloadTheme(const QString & strFileImagePath)
{
    if (!CheckPath(strFileImagePath.toUtf8().data()))
    {
        return false;
    }
    UnloadFreeTheme();
    return LoadTheme(strFileImagePath);
}

bool CImageCacheManager::RemoveImage(const std::string & strPath)
{
    if (!CheckPath(strPath))
    {
        return true;
    }
#if ADD_CACHE_ALGORITHM
    // 删除缓存图片
    CImageCachePixmap tmpImageInfo;
    if (m_ImageCache.tryGet(strPath, tmpImageInfo))
    {
        m_ImageCache.remove(strPath);
        return true;
    }
#endif

    // 遍历list查找图片
    ImageInfoIter iterMap = m_mapImageInfo.find(strPath);
    if (iterMap != m_mapImageInfo.end())
    {
        ReleaseImageInfo(iterMap->second);
        m_mapImageInfo.erase(iterMap);
        return true;
    }

    return false;
}

// 图片实际扩展名跟保存扩展名不一致时，容错方案
QSize CImageCacheManager::CheckCanRead(const std::string & strPath)
{
    // JPG
    QImageReader imageReader(strPath.c_str(), "JPG");
    if (imageReader.canRead())
    {
        return imageReader.size();
    }
    // JPEG
    QImageReader imageReader2(strPath.c_str(), "JPEG");
    if (imageReader2.canRead())
    {
        return imageReader2.size();
    }
    // PNG
    QImageReader imageReader3(strPath.c_str(), "PNG");
    if (imageReader3.canRead())
    {
        return imageReader3.size();
    }
    // GIF
    QImageReader imageReader4(strPath.c_str(), "GIF");
    if (imageReader4.canRead())
    {
        return imageReader4.size();
    }
    // BMP
    QImageReader imageReader5(strPath.c_str(), "BMP");
    if (imageReader5.canRead())
    {
        return imageReader5.size();
    }

    return QSize(-1, -1);
}

// 图片大小是否在限制范围内
IMAGE_RECEIVABLE_TYPE CImageCacheManager::IsReceivableRange(const std::string & strPath)
{
    QImageReader imageReader(strPath.c_str());
    QSize sz = imageReader.size();

    if (!imageReader.canRead())
    {
        sz = CheckCanRead(strPath);
    }

    if (sz == QSize(-1, -1))
    {
        return IRT_ERROR;
    }

    if (sz.width() <= 0 || sz.height() <= 0
            || (sz.width() * sz.height() > MAX_PICTURE_RESOLUTION))
    {
        return IRT_SIZE_OVER;
    }

    return IRT_RECEIVABLE;
}

bool CImageCacheManager::AddImage(const std::string & strPath, IMAGE_TYPE eType,
                                  bool bDelBeforeSamePic /*= false*/)
{
    // restrict max resolution
    //http://10.2.1.199/Bug.php?BugID=76736
    //改了IsReceivableRange的返回类型导致原来有些系统图片没有加到缓存里
    if (IRT_RECEIVABLE != IsReceivableRange(strPath))
    {
        return false;
    }

    if (IsImageInCache(strPath))
    {
        if (bDelBeforeSamePic)
        {
            bool bP = RemoveImage(strPath);
        }
        else
        {
            return false;
        }
    }

    // 将图片添加进缓存
    CImageBaseInfo * pImageInfo = GetImageInfoByType(eType);
    if (NULL == pImageInfo)
    {
        return false;
    }


    pImageInfo->LoadImageInfo(strPath.c_str());
    GetImagePoint9Manager().GetPoint9Info(pImageInfo);

    // 判断图片是否加载成功
    if (pImageInfo->IsNull())
    {
        UIKERNEL_INFO("[CImageCacheManager::AddImage] CImageCacheManager Load Image(%s) Fail!",
                      strPath.c_str());
        ReleaseImageInfo(pImageInfo);
        return false;
    }

    if (pImageInfo->IsCache())
    {
        InsertToCacheMap(strPath, pImageInfo);
    }
    else
    {
        UIKERNEL_INFO("release no cache image");
        pImageInfo->SetRemoveImage(false);
        ReleaseImageInfo(pImageInfo);
    }
    return true;
}

bool CImageCacheManager::AddImage(const std::string & strPath, IMAGE_TYPE eType, QPixmap & pic,
                                  bool bDelBeforeSamePic /*= false*/)
{
    if (pic.isNull())
    {
        UIKERNEL_INFO("[CImageCacheManager::AddImage] path(%s),type(%d),sameDel(%d) pic is NULL!"
                      , strPath.c_str(), eType, bDelBeforeSamePic);
        return false;
    }

    if (bDelBeforeSamePic)
    {
        RemoveImage(strPath);
    }
    else if (IsImageInCache(strPath))
    {
        UIKERNEL_INFO("[CImageCacheManager::AddImage] path(%s),type(%d),sameDel(%d) pic in cache!"
                      , strPath.c_str(), eType, bDelBeforeSamePic);
        return false;
    }

    // 将图片添加进缓存
    CImageBaseInfo * pImageInfo = GetImageInfoByType(eType);
    if (NULL == pImageInfo)
    {
        UIKERNEL_INFO("[CImageCacheManager::AddImage] path(%s),type(%d),sameDel(%d) Get Object Fail!"
                      , strPath.c_str(), eType, bDelBeforeSamePic);
        return false;
    }

    pImageInfo->LoadImageInfo(strPath.c_str(), pic);
    GetImagePoint9Manager().GetPoint9Info(pImageInfo);

    if (pImageInfo->IsCache())
    {
        InsertToCacheMap(strPath, pImageInfo);
    }
    else
    {
        UIKERNEL_INFO("release no cache image");
        pImageInfo->SetRemoveImage(false);
        ReleaseImageInfo(pImageInfo);
    }

    return true;
}

#if ADD_CACHE_ALGORITHM
void CImageCacheManager::ClearLRUCache()
{
    std::map<std::string, lru::Node<std::string, CImageCachePixmap>*> cacheDa =
        m_ImageCache.getCache();
    ImageCacheIter iterCache = cacheDa.begin();
    for (; iterCache != cacheDa.end(); ++iterCache)
    {
        if (NULL != iterCache->second)
        {
            m_ImageCache.remove(iterCache->first);
        }
    }
}

bool CImageCacheManager::IsExistInLRUCache(const std::string & strPath, QPixmap & pic)
{
    CImageCachePixmap tmpImageInfo;
    if (m_ImageCache.tryGet(strPath, tmpImageInfo))
    {
        pic = tmpImageInfo.GetPixmap();
        return true;
    }

    return false;
}

bool CImageCacheManager::InsertLRUCache(const std::string & strPath, QPixmap & pic)
{
    // restrict max resolution
    QImageReader imageReader(QString::fromStdString(strPath));
    QSize sz = imageReader.size();

    if (sz.width() <= 0 || sz.height() <= 0
            || (sz.width() * sz.height() > MAX_PICTURE_RESOLUTION))
    {
        return false;
    }

    CImageCachePixmap tmpImageInfo;
    if (m_ImageCache.tryGet(strPath, tmpImageInfo))
    {
        return false;
    }
    else
    {
        if (!tmpImageInfo.LoadImageInfo(QString::fromStdString(strPath), pic))
        {
            return false;
        }

        m_ImageCache.insert(strPath, tmpImageInfo);
    }

    return true;
}
#endif

QPixmap CImageCacheManager::FindImage(const std::string & strPath, IMAGE_TYPE eType /*= DIT_THEME*/)
{
    if (strPath.empty())
    {
        return m_ImageEmpty.GetPixmap();
    }

    QSize sz;
    if (DIT_THEME != eType)
    {
        QString strFullPath = "";
        if (!GetImagePathConvertor().ConvertToFullPath(strPath.c_str(), strFullPath))
        {
            strFullPath = strPath.c_str();
        }

        if (!CheckPath(strFullPath.toUtf8().data()))
        {
            return m_ImageEmpty.GetPixmap();
        }

        // restrict max resolution
        QImageReader imageReader(strFullPath);
        sz = imageReader.size();

        if (!imageReader.canRead())
        {
            sz = CheckCanRead(strFullPath.toUtf8().data());
        }
        if (sz.width() <= 0 || sz.height() <= 0
                || (sz.width() * sz.height() > MAX_PICTURE_RESOLUTION))
        {
            return m_ImageEmpty.GetPixmap();
        }
    }

#if ADD_CACHE_ALGORITHM
    // 先从缓存算法中获取图片
    CImageCachePixmap tmpImageInfo;
    if (m_ImageCache.tryGet(strPath, tmpImageInfo))
    {
        // DIT_USB特殊处理
        if (DIT_USB == eType)
        {
            QSize si = tmpImageInfo.GetImageSize();
            if (si != QSize(58, 58))
            {
                if (RemoveImage(strPath.c_str()))
                {
                    tmpImageInfo.SetImageSize(QSize(58, 58));
                    tmpImageInfo.SetType(eType);
                    tmpImageInfo.LoadImageInfo(strPath.c_str());

                    if (!tmpImageInfo.IsNull())
                    {
                        m_ImageCache.insert(strPath, tmpImageInfo);
                        return tmpImageInfo.GetPixmap();
                    }
                }
            }
        }

        return tmpImageInfo.GetPixmap();
    }
#endif

    // 遍历寻找图片是否存在
    ImageInfoIter iterMap = m_mapImageInfo.find(strPath);
    if (iterMap == m_mapImageInfo.end())
    {
#if ADD_CACHE_ALGORITHM
        // 未缓存的图片才加入缓存算法
        //if (!iterMap->second->IsCache() && iterMap == m_mapImageInfo.end())
        if (sz.width() * sz.height() < MAX_CACHE_RESOLUTION)
        {
            // 下载的时候已经拉伸压缩过,这里不需要拉伸
            tmpImageInfo.SetImageSize(G_NO_SCALED);
            tmpImageInfo.SetRemoveImage(false);
            tmpImageInfo.SetType(eType);
            if (DIT_USB == eType)
            {
                tmpImageInfo.SetImageSize(QSize(58, 58));
            }
            else if (DIT_PROPORTION == eType)
            {
                QRect rc;// = g_pUSBUILogic->CalculateImageRect(sz.width(), sz.height());
                tmpImageInfo.SetImageSize(QSize(rc.width(), rc.height()));
            }

            tmpImageInfo.LoadImageInfo(strPath.c_str());
            GetImagePoint9Manager().GetPoint9Info(&tmpImageInfo);

            if (!tmpImageInfo.IsNull())
            {
                m_ImageCache.insert(strPath, tmpImageInfo);
                return tmpImageInfo.GetPixmap();
            }
            else
            {
                return m_ImageEmpty.GetPixmap();
            }
        }
        else
        {
            QPixmap pix;
            CImageBaseInfo::LoadPixmapByPath(pix, strPath.c_str());

            if (DIT_USB == eType)
            {
                pix = pix.scaled(QSize(58, 58), Qt::KeepAspectRatio);
            }
            else if (DIT_PROPORTION == eType)
            {
                QRect rc;// = g_pUSBUILogic->CalculateImageRect(sz.width(), sz.height());
                pix = pix.scaled(QSize(rc.width(), rc.height()), Qt::KeepAspectRatio);
            }

            return pix;
        }

#else
        if (DIT_SOCKET != eType && DIT_NOCACHE != eType)
        {
            return m_ImageEmpty.GetPixmap();
        }
        else
        {
            CImageBaseInfo * pImageInfo = GetImageInfoByType(eType);
            if (NULL == pImageInfo)
            {
                return m_ImageEmpty.GetPixmap();
            }

            pImageInfo->LoadImageInfo(strPath.c_str());
            GetImagePoint9Manager().GetPoint9Info(pImageInfo);

            // 判断图片是否加载成功
            if (pImageInfo->IsNull())
            {
                return m_ImageEmpty.GetPixmap();
            }

            InsertToCacheMap(strPath, pImageInfo);
            return pImageInfo->GetPixmap();
        }
#endif
    }
    else if (NULL == iterMap->second)
    {
        RemoveImage(strPath);
        // 不在缓存时从硬盘读取加入
        CImageBaseInfo * pImageInfo = GetImageInfoByType(eType);
        if (NULL == pImageInfo)
        {
            return m_ImageEmpty.GetPixmap();
        }

        pImageInfo->LoadImageInfo(strPath.c_str());
        GetImagePoint9Manager().GetPoint9Info(pImageInfo);

        // 判断图片是否加载成功
        if (pImageInfo->IsNull())
        {
            UIKERNEL_INFO("[FindImage]NULL pixmap");
            return m_ImageEmpty.GetPixmap();
        }

        if (pImageInfo->IsCache())
        {
            InsertToCacheMap(strPath, pImageInfo);
        }

        return pImageInfo->GetPixmap();
    }

    return iterMap->second->GetPixmap();
}

bool CImageCacheManager::CheckPath(const std::string & strPath)
{
    if (strPath.empty() || 0 == strPath.compare(""))
    {
        return false;
    }
    return true;
}

bool CImageCacheManager::IsImage(LPCSTR lpszFilePath)
{
    if (lpszFilePath == NULL)
    {
        return false;
    }

    // 取文件后缀名
    QString strFileType = QFileInfo(lpszFilePath).suffix().toLower();

    // 判断是否为图片资源
    return (strFileType == "bmp"
            || strFileType == "gif"
            || strFileType == "jpg"
            || strFileType == "jpeg"
            || strFileType == "png"
            || strFileType == "pbm"
            || strFileType == "pgm"
            || strFileType == "ppm"
            || strFileType == "xbm"
            || strFileType == "xpm"
            || strFileType.isEmpty());// 没有后缀名的也可能是图片
}

void CImageCacheManager::ReleaseImageInfo(CImageBaseInfo * pImageInfo, bool bSync)
{
    if (NULL == pImageInfo)
    {
        return;
    }

    if (bSync)
    {
        delete pImageInfo;
    }
    else
    {
        DeleteLaterImage(pImageInfo);
    }
}

CImageCacheManager & GetImageCacheManager()
{
    static CImageCacheManager sCImageCacheManager;
    return sCImageCacheManager;
}

#ifdef TEST_DELETE_MOST_IMAGES_EFFICIENTY
void CImageCacheManager::Add1500Images()
{
    std::string strPath = SYSTEM_IMAGE_PATH "general/test";
    static int i = 0;

    for (int j = 0; j < 1500; ++j, ++i)
    {
        CImageBaseInfo * pImageInfo = GetImageInfoByType(DIT_SOCKET);
        if (NULL == pImageInfo)
        {
            return ;
        }

        std::ostringstream ss;
        ss << i;
        std::string s(ss.str());
        std::string strT = strPath + s;
        m_mapImageInfo.insert(make_pair(strT, pImageInfo));
    }
}
#endif


CImageBaseInfo * CImageCacheManager::FindImageInfo(LPCSTR lpszFilePath)
{
    ImageInfoIter iterMap = m_mapImageInfo.find(lpszFilePath);
    if (iterMap == m_mapImageInfo.end())
    {
        return NULL;
    }

    return iterMap->second;
}

void CImageCacheManager::InsertToCacheMap(const std::string & strPath, CImageBaseInfo * pImageInfo)
{
    QString strFullPath = strPath.c_str();
    QString strThemePath = "";
    bool bResult = GetImagePathConvertor().ConvertToThemePath(strFullPath, strThemePath);
    if (bResult)
    {
        m_mapImageInfo.insert(make_pair(strThemePath.toUtf8().data(), pImageInfo));
    }
    else
    {
        m_mapImageInfo.insert(make_pair(strPath, pImageInfo));
    }
}

void CImageCacheManager::DeleteLaterImage(CImageBaseInfo * pImageInfo)
{
    m_vecDeleteImage.push_back(pImageInfo);
    if (!m_TimeToDelete.isActive())
    {
        m_TimeToDelete.start(DELETE_IMAGE_LATER_TIME);
    }
}

void CImageCacheManager::OnTimerOut()
{
    int nDeleteSize = m_vecDeleteImage.size();
    if (nDeleteSize == 0)
    {
        m_TimeToDelete.stop();
    }
    else
    {
        for (int i = 0; i < nDeleteSize; i++)
        {
            delete m_vecDeleteImage[i];
        }
        m_vecDeleteImage.clear();
    }
}

#ifdef IF_FEATURE_DUMP_CACHE_INFO
void CImageCacheManager::OnDumpCacheInfoTimeout()
{
    m_ImageCache.dumpDebug();

    m_timerDumpCacheInfo.start(DUMP_CACHE_INFO_TIME);
}
#endif
