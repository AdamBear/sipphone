#ifndef _DIR_IMAGE_MANAGER_H_
#define _DIR_IMAGE_MANAGER_H_

#include <string>
#include <map>
#include <vector>
#include <QTimer>
#include <ETLLib/ETLLib.hpp>
#include "lrucache.hpp"
#include "baseui/t4xpicpath.h"
#include "imageinfo.h"
#include "imagecachemanagerdefine.h"
#include "pixmapvaluehelper.h"

namespace
{
//#define TEST_DELETE_MOST_IMAGES_EFFICIENTY
}

// 是否添加缓存算法
#define ADD_CACHE_ALGORITHM  1

#if ADD_CACHE_ALGORITHM
// 是否支持打印当前Lru链表中数据信息
//#define IF_FEATURE_DUMP_CACHE_INFO    1
#endif

using namespace std;
class CImageBaseInfo;

// 图片资源映射表类型定义
typedef map<string, CImageBaseInfo *>                 MapImageInfo;
typedef MapImageInfo::iterator                       ImageInfoIter;
typedef vector<CImageBaseInfo *>                      vecCImageBase;

// 图片资源管理类
class CImageCacheManager : public QObject
{
    Q_OBJECT
public:
    // 构造函数
    CImageCacheManager();
    // 析构函数
    virtual ~CImageCacheManager();

public:
    // 加载图片资源
    bool AddImage(const std::string & strPath, IMAGE_TYPE eType, bool bDelBeforeSamePic = false);
    bool AddImage(const std::string & strPath, IMAGE_TYPE eType, QPixmap & pic,
                  bool bDelBeforeSamePic = false);

    // 从缓存中移除图片资源
    bool RemoveImage(const std::string & strPath);

    // 释放缓存的图片资源，默认删除Lync图片资源
    void ClearImageResource(bool bLync = true);

    // 根据类型释放缓存的图片资源
    void ClearImageResourceByImageType(IMAGE_TYPE eType = DIT_NONE);

    // 返回缓存图片的数量
    int GetCacheImageCount() const;

    // 通过文件路径查找结构指针对象
    QPixmap FindImage(const std::string & strPath, IMAGE_TYPE eType = DIT_THEME);

    CImageBaseInfo * FindImageInfo(LPCSTR lpszFilePath);

    // 加载主题下的图片资源
    bool LoadTheme(const QString & strFileImagePath);

    // 释放缓存的主题图片资源
    void UnloadFreeTheme();

    // 加载新的主题
    bool ReloadTheme(const QString & strFileImagePath);

    //清除LRU缓存
#if ADD_CACHE_ALGORITHM
    void ClearLRUCache();
    bool InsertLRUCache(const std::string & strPath, QPixmap & pic);
    bool IsExistInLRUCache(const std::string & strPath, QPixmap & pic);
#endif

    static LRESULT OnMessage(msgObject & refMessage);

#ifdef TEST_DELETE_MOST_IMAGES_EFFICIENTY
    void Add1500Images();
#endif

    // 图片大小是否在限制范围内
    IMAGE_RECEIVABLE_TYPE IsReceivableRange(const std::string & strPath);

//private:
    // 缓存中是否存在图片资源
    bool IsImageInCache(const std::string & strPath);
private:
    // 判断是否为图片资源
    bool IsImage(LPCSTR lpszFilePath);

    bool CheckPath(const std::string & strPath);

    void ReleaseImageInfo(CImageBaseInfo * pImageInfo, bool bSync = false);

    // 图片实际扩展名跟保存扩展名不一致时，容错方案
    QSize CheckCanRead(const std::string & strPath);

    void InsertToCacheMap(const std::string & strPath, CImageBaseInfo * pImageInfo);

private:
    CImageEmpty     m_ImageEmpty;       // 本地的空图片，当出现异常的时候返回该图片
    MapImageInfo    m_mapImageInfo;     // 图片资源文件的映射表

private slots:
    void OnTimerOut();

#ifdef IF_FEATURE_DUMP_CACHE_INFO
    void OnDumpCacheInfoTimeout();
#endif

private:
    QTimer          m_TimeToDelete;     //delete操作的计时器
    vecCImageBase   m_vecDeleteImage;   //待删除的向量

    void DeleteLaterImage(CImageBaseInfo * pImageInfo);

#if ADD_CACHE_ALGORITHM
    ImageCache      m_ImageCache;
#endif

#ifdef IF_FEATURE_DUMP_CACHE_INFO
    QTimer          m_timerDumpCacheInfo;
#endif
};

CImageCacheManager & GetImageCacheManager();

#endif // _DIR_IMAGE_MANAGER_H_
