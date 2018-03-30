// #ifndef TEST_IMAGE_CACHE_CPP
// #define TEST_IMAGE_CACHE_CPP

#include "imagecachemanager.h"
#include "imagecachemanagerdefine.h"
#include "imageinfo.h"
#include "modimagemanager.h"
//#include "modlyncimagecachemanager.h"
#include "imagethemeinfo.h"
#include <QtGui/QApplication>
#include "path_define.h"
#include "dsklog/log.h"
#include "baseui/t4xpicpath.h"
#include <iostream>
#include <sstream>
#include <string>
#include <QPixmap>
#include <QIODevice>
#include <QByteArray>
#include <QImageReader>
#include <QFileInfo>
#include <QBuffer>

namespace
{
#define FILE_NAME       TEMP_PATH "MyTestImageCache/"
//MyTestImageCache file
#define CACHE_ADD       FILE_NAME "test_cache_add.png"
#define FIND_ADD        FILE_NAME "test_find_add.png"
#define FIND_COMMON     FILE_NAME "test_find_common.png"
#define FIND_LRU        FILE_NAME "test_find_lru.png"
#define SOCKET_ADD      FILE_NAME "test_socket_add.png"
#define THEME_ADD       FILE_NAME "test_theme_add.png"

#define LOAD            FILE_NAME "load/"
// load file
#define LOAD_LOAD       LOAD "load.png"

#define RELOAD          FILE_NAME "reload/"
// reload file
#define RELOAD_RELOAD   RELOAD "reload.png"
}

// 打印测试用例的运行结果
void PrintfCaseResult(int nCaseNum, std::string strCase, bool bCaseResult)
{
    std::string strResult = bCaseResult ? "true" : "false";
    std::cout << "Case" << nCaseNum << ":" << strCase << "   " << bCaseResult << endl;
}

// case1:加载指定模块下的主题
bool ImageCacheManagerCase1()
{
    bool bResult;

    Image_LoadTheme(LOAD);
    int bNum = GetImageCacheManager().GetCacheImageCount();
    UIKERNEL_INFO("----------- total num[%d] ---------", bNum);
    bResult = bNum > 0;
    //Image_UnloadTheme();
    return bResult;
}

// case2:查看主题中的图片
bool ImageCacheManagerCase2()
{
    bool bResult;

    Image_LoadTheme(LOAD);
    bResult = GetImageCacheManager().IsImageInCache(LOAD_LOAD);
    //Image_UnloadTheme();
    return bResult;
}

// case3:删除主题中的一张图片
bool ImageCacheManagerCase3()
{
    bool bResult;

    Image_LoadTheme(LOAD);
    bResult = GetImageCacheManager().IsImageInCache(LOAD_LOAD);
    Image_RemoveImage(LOAD_LOAD);
    bResult = bResult && !GetImageCacheManager().IsImageInCache(LOAD_LOAD);
    //Image_UnloadTheme();
    return bResult;
}

// case4:重新加载一个主题
bool ImageCacheManagerCase4()
{
    bool bResult;

    Image_LoadTheme(LOAD);
    bResult = GetImageCacheManager().IsImageInCache(LOAD_LOAD);
    GetImageCacheManager().ReloadTheme(RELOAD);
//  bResult = !GetImageCacheManager().IsImageInCache(LOAD_LOAD)
//              && GetImageCacheManager().IsImageInCache(RELOAD_RELOAD);
    bResult = GetImageCacheManager().IsImageInCache(RELOAD_RELOAD);
    //Image_UnloadTheme();
    return bResult;
}

// case5:释放主题资源
bool ImageCacheManagerCase5()
{
    bool bResult;

    Image_LoadTheme(LOAD);
    bResult = GetImageCacheManager().GetCacheImageCount() != 0;
    Image_UnloadTheme();
    bResult = bResult && GetImageCacheManager().GetCacheImageCount() == 0;
    return bResult;
}

// case6:设置为不缓存模式，查看加载主题情况
bool ImageCacheManagerCase6()
{
    bool bResult;

    // cache add test
    Image_AddImage(CACHE_ADD, DIT_TALK);
    bResult = GetImageCacheManager().IsImageInCache(CACHE_ADD);
    UIKERNEL_INFO("----- cache add %s", bResult ? "success" : "fail");
    // socket add test
    Image_AddImage(SOCKET_ADD, DIT_SOCKET);
    bResult = GetImageCacheManager().IsImageInCache(SOCKET_ADD);
    UIKERNEL_INFO("----- socket add %s", bResult ? "success" : "fail");
    // theme add test
    Image_AddImage(THEME_ADD, DIT_THEME);
    bResult = GetImageCacheManager().IsImageInCache(THEME_ADD);
    UIKERNEL_INFO("----- theme add %s", bResult ? "success" : "fail");
    // find add test
    Image_AddImage(FIND_ADD, DIT_DIR);
    bResult = GetImageCacheManager().IsImageInCache(FIND_ADD);
    UIKERNEL_INFO("----- find add %s", bResult ? "success" : "fail");
    // find lru test
    bResult = !GetImageCacheManager().IsImageInCache(FIND_LRU);
    QPixmap pic = GetImageCacheManager().FindImage(FIND_LRU);
    bResult = bResult && !(pic.isNull());
    //bResult = GetImageCacheManager().IsImageInCache(FIND_LRU);
    UIKERNEL_INFO("----- find lru %s", bResult ? "success" : "fail");
    // find common test
    Image_AddImage(FIND_COMMON, DIT_FAVORITE);
    pic = GetImageCacheManager().FindImage(FIND_COMMON);
    bResult = bResult && !(pic.isNull());
    UIKERNEL_INFO("----- find common %s", bResult ? "success" : "fail");
    // Lync clear test
    int bNum = GetImageCacheManager().GetCacheImageCount();
    UIKERNEL_INFO("----- Cur number pic %d", bNum);
    GetImageCacheManager().ClearImageResource();
    bNum = GetImageCacheManager().GetCacheImageCount();
    UIKERNEL_INFO("----- After clear Lync pic, now has %d", bNum);
    return bResult;
}

// case7:加载一张临时图片
bool ImageCacheManagerCase7()
{
    bool bResult;

    Image_GetImage(PIC_MENU_AA);
    int bNum = GetImageCacheManager().GetCacheImageCount();
    UIKERNEL_INFO("----------- Getpixmap num[%d] ----------", bNum);
    bResult = Image_AddImage(PIC_MENU_AA, DIT_THEME);
    bNum = GetImageCacheManager().GetCacheImageCount();
    QPixmap pic = Image_GetImage(PIC_MENU_AA);
    pic.scaled(QSize(10, 100));
    UIKERNEL_INFO("----------- Add Theme pic, num[%d] ----------", bNum);
    return bResult;
}

// case8:释放一张临时图片
bool ImageCacheManagerCase8()
{
    bool bResult;

    Image_AddImage(PIC_MENU_AA, DIT_SOCKET);
    int bNum = GetImageCacheManager().GetCacheImageCount();
    UIKERNEL_INFO("---------- temp add pic [%d] -------", bNum);
    bResult = Image_RemoveImage(PIC_MENU_AA);
    bNum = GetImageCacheManager().GetCacheImageCount();
    UIKERNEL_INFO("---------- temp del pic [%d] -------", bNum);
    return bResult;
}

bool ImageCacheManagerCase9()
{
#ifdef TEST_DELETE_MOST_IMAGES_EFFICIENTY
    GetImageCacheManager().Add1500Images();
    int bNum = GetImageCacheManager().GetCacheImageCount();
    UIKERNEL_INFO("---------- after add pic [%d] -------", bNum);
    GetImageCacheManager().ClearImageResource();
    bNum = GetImageCacheManager().GetCacheImageCount();
    UIKERNEL_INFO("---------- after clear pic [%d] -------", bNum);
#endif
    return true;
}

bool ImageCacheManagerCase10()
{
    std::string strPath = FILE_NAME"del_";
    UIKERNEL_INFO("--------------------- start add =============");
    for (int i = 1; i < 13; ++i)
    {
//      std::ostringstream ss;
//      ss << i;
//      std::string s(ss.str());
//      std::string strT = strPath + s;
        std::ostringstream ss;
        ss << i;
        std::string s(ss.str());
        std::string str = strPath + s + ".png";
        bool bE = Image_AddImage(str, DIT_TALK);
        UIKERNEL_INFO("-------- add pic[%s], result[%d] =======", str.c_str(), bE);
    }
    UIKERNEL_INFO("--------------------- end add start del =============");
    for (int i = 1; i < 13; ++i)
    {
        std::ostringstream ss;
        ss << i;
        std::string s(ss.str());
        std::string str = strPath + s + ".png";
        bool bE = Image_RemoveImage(str);
        UIKERNEL_INFO("-------- del pic[%s], result[%d] =======", str.c_str(), bE);
    }
    UIKERNEL_INFO("--------------------- end del =============");

    return true;
}

bool ImageCacheManagerCase11()
{
    /*
    int bNum = GetImageCacheManager().GetCacheImageCount();
    UIKERNEL_INFO("---------- before add pic [%d] -------", bNum);

    //QPixmap pic(FILE_NAME"test_lync_add.png");
    QImage ima(FILE_NAME"test_lync_add.png");
    bool bR = !ima.isNull();

    if (bR)
    {
        QByteArray arr;
        QBuffer bu(&arr);
        bu.open(QIODevice::WriteOnly);
        ima.save(&bu, "png");
        bu.close();

        bu.open(QIODevice::ReadOnly);
        QImageReader rd(&bu);
        QImage image1 = rd.read();
        if (image1.isNull())
        {
            UIKERNEL_INFO("------------- image1 is Null ==========");
        }
        else
        {
            UIKERNEL_INFO("------------- byte[%s] ==========", arr.data());
        }
        //bu.close();

        yl::string strP = LyncImageCache_AddImage("lync_add.png", DIT_TALK, arr.data(), arr.length());
        UIKERNEL_INFO("-------------- path[%s] =============", strP.c_str());
        std::string strPath(strP.c_str());
        bR = GetImageCacheManager().IsImageInCache(strPath);
        bu.close();
    }

    return bR;
    */
    return true;
}

bool ImageCacheManagerCase12()
{
    /*
    int bNum = GetImageCacheManager().GetCacheImageCount();
    UIKERNEL_INFO("---------- before two add pic [%d] -------", bNum);

    //QPixmap pic(FILE_NAME"test_lync_add.png");
    QImage ima(FILE_NAME"test_lync_add.png");
    bool bR = !ima.isNull();
    if (bR)
    {
        QByteArray arr;
        QBuffer bu(&arr);
        bu.open(QIODevice::WriteOnly);
        ima.save(&bu, "png");
        bu.close();

        bu.open(QIODevice::ReadOnly);
        QImageReader rd(&bu);
        QImage image1 = rd.read();
        if (image1.isNull())
        {
            UIKERNEL_INFO("------------- image1 is Null ==========o");
        }
        else
        {
            UIKERNEL_INFO("------------- byte[%s] ==========", arr.data());
        }
        //bu.close();

        //bR = GetImageCacheManager().IsImageInCache(strPath);
        yl::string strP = LyncImageCache_AddImage("lync_add.png", DIT_TALK, arr.data(), arr.length());
        UIKERNEL_INFO("-------------- path[%s] =============", strP.c_str());
        std::string strPath(strP.c_str());
        bR = GetImageCacheManager().IsImageInCache(strPath);
        bu.close();
    }

    bNum = GetImageCacheManager().GetCacheImageCount();
    UIKERNEL_INFO("---------- after two add pic [%d] -------", bNum);

    return bR;
    */
    return true;
}

bool ImageCacheManagerCase13()
{
    int bNum = GetImageCacheManager().GetCacheImageCount();
    UIKERNEL_INFO("---------- before two add pic [%d] -------", bNum);

    bool bResult = !GetImageCacheManager().IsImageInCache(FILE_NAME"test_lync_add.png");
    bResult = bResult && GetImageCacheManager().AddImage(FILE_NAME"test_lync_add.png", DIT_THEME, true);

    bNum = GetImageCacheManager().GetCacheImageCount();
    UIKERNEL_INFO("---------- after add pic [%d] -------", bNum);

    bResult = GetImageCacheManager().IsImageInCache(FILE_NAME"test_lync_add.png");
    bResult = bResult && GetImageCacheManager().AddImage(FILE_NAME"test_lync_add.png", DIT_THEME, true);

    bNum = GetImageCacheManager().GetCacheImageCount();
    UIKERNEL_INFO("---------- after two add pic [%d] -------", bNum);

    return bResult;
}

bool ImageCacheManagerCase14()
{
    //bResult = !GetImageCacheManager().IsImageInCache(FIND_LRU);
    bool bResult = false;
    QPixmap pic = GetImageCacheManager().FindImage(TEMP_PATH "80072@yealinkuc.com");
    bResult = !(pic.isNull());
    if (bResult)
    {
        UIKERNEL_INFO("--- size[%d, %d] ===", pic.width(), pic.height());
    }

//  pic = GetImageCacheManager().FindImage(TEMP_PATH "80072@yealinkuc.com", DIT_TALK);
//  bResult = !(pic.isNull());
//  if (bResult)
//  {
//      UIKERNEL_INFO("---222 size[%d, %d] ===", pic.width(), pic.height());
//  }
//  Image_AddImage(CACHE_ADD, DIT_TALK);
    //bResult = GetImageCacheManager().IsImageInCache(FIND_LRU);
    UIKERNEL_INFO("----- download and find pic %s", bResult ? "success" : "fail");
    return bResult;
}

bool ImageCacheManagerCase15()
{
    bool bF = true;
    QPixmap pic;
    GetImageCacheManager().AddImage(TEMP_PATH "del_1.png", DIT_USB);
    pic = GetImageCacheManager().FindImage(TEMP_PATH "del_1.png", DIT_USB);
    bF &= pic.isNull() ? false : true;
    pic = GetImageCacheManager().FindImage(TEMP_PATH "del_3.png", DIT_PROPORTION);
    bF &= pic.isNull() ? false : true;
    pic = GetImageCacheManager().FindImage(TEMP_PATH "del_2.png", DIT_USB);
    bF &= pic.isNull() ? false : true;

    UIKERNEL_INFO("----- bF[%d] ======", bF);
    bF = GetImageCacheManager().AddImage(TEMP_PATH "del_2.png", DIT_USB, true);
    UIKERNEL_INFO("----- bF[%d] ======", bF);
    GetImageCacheManager().RemoveImage(TEMP_PATH "del_2.png");
    //GetImageCacheManager().ClearImageResourceByImageType(IMAGE_TYPE(DIT_USB | DIT_PROPORTION));
    return true;
}

bool ImageCacheManagerCase16()
{
    QPixmap pic;

    // 8M
    /*
    UIKERNEL_INFO("---------------------- add 8M ============");
    GetImageCacheManager().AddImage(TEMP_PATH "image_8.jpg", DIT_PROPORTION);
    UIKERNEL_INFO("---------------------- find 8M =============");
    pic = GetImageCacheManager().FindImage(TEMP_PATH "image_8.jpg", DIT_PROPORTION);
    if (pic.isNull())
    {
        UIKERNEL_INFO("--------------------------- null ================\n\n");
    }
    else
    {
        UIKERNEL_INFO("--------------------------- not null ============= \n\n");
    }
    // 7M
    UIKERNEL_INFO("---------------------- add 7M ============");
    GetImageCacheManager().AddImage(TEMP_PATH "image_7.jpg", DIT_PROPORTION);
    UIKERNEL_INFO("---------------------- find 7M =============");
    pic = GetImageCacheManager().FindImage(TEMP_PATH "image_7.jpg", DIT_PROPORTION);
    if (pic.isNull())
    {
        UIKERNEL_INFO("--------------------------- null ================\n\n");
    }
    else
    {
        UIKERNEL_INFO("--------------------------- not null ============= \n\n");
    }
    // 4M
    UIKERNEL_INFO("---------------------- add 4M ============");
    GetImageCacheManager().AddImage(TEMP_PATH "image_4.jpg", DIT_PROPORTION);
    UIKERNEL_INFO("---------------------- find 4M =============");
    pic = GetImageCacheManager().FindImage(TEMP_PATH "image_4.jpg", DIT_PROPORTION);
    if (pic.isNull())
    {
        UIKERNEL_INFO("--------------------------- null ================\n\n");
    }
    else
    {
        UIKERNEL_INFO("--------------------------- not null ============= \n\n");
    }
    // 2M
    UIKERNEL_INFO("---------------------- add 2M ============");
    GetImageCacheManager().AddImage(TEMP_PATH "image_2.jpg", DIT_PROPORTION);
    UIKERNEL_INFO("---------------------- find 2M =============");
    pic = GetImageCacheManager().FindImage(TEMP_PATH "image_2.jpg", DIT_PROPORTION);
    if (pic.isNull())
    {
        UIKERNEL_INFO("--------------------------- null ================\n\n");
    }
    else
    {
        UIKERNEL_INFO("--------------------------- not null ============= \n\n");
    }
    // 1.9M
    UIKERNEL_INFO("---------------------- add 1.9M ============");
    GetImageCacheManager().AddImage(TEMP_PATH "image_1_9.jpg", DIT_PROPORTION);
    UIKERNEL_INFO("---------------------- find 1.9M =============");
    pic = GetImageCacheManager().FindImage(TEMP_PATH "image_1_9.jpg", DIT_PROPORTION);
    if (pic.isNull())
    {
        UIKERNEL_INFO("--------------------------- null ================\n\n");
    }
    else
    {
        UIKERNEL_INFO("--------------------------- not null ============= \n\n");
    }*/
    // 1.8M
//  UIKERNEL_INFO("---------------------- add 1.8M ============");
//  GetImageCacheManager().AddImage(TEMP_PATH "image_1_8.jpg", DIT_PROPORTION);
//  UIKERNEL_INFO("---------------------- find 1.8M =============");
//  pic = GetImageCacheManager().FindImage(TEMP_PATH "image_1_8.jpg", DIT_PROPORTION);
//  if (pic.isNull())
//  {
//      UIKERNEL_INFO("--------------------------- null ================\n\n");
//  }
//  else
//  {
//      UIKERNEL_INFO("--------------------------- not null ============= \n\n");
//  }

    // 4.7K
    UIKERNEL_INFO("---------------------- add 4.7K ============");
//  GetImageCacheManager().AddImage(TEMP_PATH "image_A.png", DIT_PROPORTION);
//  UIKERNEL_INFO("---------------------- find 4.7K =============");
//  pic = GetImageCacheManager().FindImage(TEMP_PATH "image_A.png", DIT_PROPORTION);
//  if (pic.isNull())
//  {
//      UIKERNEL_INFO("--------------------------- null ================\n\n");
//  }
//  else
//  {
//      UIKERNEL_INFO("--------------------------- not null ============= \n\n");
//  }

//  QFileInfo fileInfo(TEMP_PATH "image_A.png");
//  QFile fl(TEMP_PATH "image_A.png");
//  UIKERNEL_INFO("------------ file size[%lld][%I64d] =======", fileInfo.size(), fl.size());
//  UIKERNEL_INFO("------------- test 1000 times start =================");
//  for (int i=0; i<1000; ++i)
//  {
//      QImageReader imageReader(TEMP_PATH "image_8.jpg");
//      QSize sz = imageReader.size();
//      if (sz.width() * sz.height() <= 1650000)
//      {
//          printf("----------------------- hehe you're pass ==========");
//      }
//      //UIKERNEL_INFO("------------ file size(%d, %d) ===========", sz.width(), sz.height());
//  }
//  UIKERNEL_INFO("------------- test 1000 times end =================");

//  UIKERNEL_INFO("---------------------- add 1.8M ============");
//  QFileInfo fileInfo2(TEMP_PATH "image_1_8.jpg");
//  QFile fl2(TEMP_PATH "image_A.png");
//  UIKERNEL_INFO("------------ file size[%lld][%I64d] =======", fileInfo2.size(), fl2.size());
//  QImageReader imageReader2(TEMP_PATH "image_1_8.jpg");
//  sz = imageReader2.size();
//  UIKERNEL_INFO("------------ file size(%d, %d) ===========", sz.width(), sz.height());
//  // 8M
//  UIKERNEL_INFO("---------------------- add 8M ============");
//  QFileInfo fileInfo3(TEMP_PATH "image_8.jpg");
//  QFile fl3(TEMP_PATH "image_A.png");
//  UIKERNEL_INFO("------------ file size[%lld][%I64d] =======", fileInfo3.size(), fl3.size());
//  QImageReader imageReader3(TEMP_PATH "image_8.jpg");
//  sz = imageReader3.size();


    QSize sz(-1, -1);
    UIKERNEL_INFO("--------------------------- 1000 test st ========");
    for (int i = 0; i < 1000; ++i)
    {
        QImageReader imageR(TEMP_PATH "sl1.jpg");
        sz = imageR.size();

        if (!imageR.canRead())
        {
            //UIKERNEL_INFO("------------------ enter ==========");
            // JPG
            QImageReader imageReader(TEMP_PATH "sl1.jpg", "JPG");
            if (imageReader.canRead())
            {
                sz = imageReader.size();
                continue;
            }
            // JPEG
            QImageReader imageReader2(TEMP_PATH "sl1.jpg", "JPEG");
            if (imageReader2.canRead())
            {
                sz = imageReader2.size();
                continue;
            }
            // PNG
            QImageReader imageReader3(TEMP_PATH "sl1.jpg", "PNG");
            if (imageReader3.canRead())
            {
                sz = imageReader3.size();
                continue;
            }
            // GIF
            QImageReader imageReader4(TEMP_PATH "sl1.jpg", "GIF");
            if (imageReader4.canRead())
            {
                sz = imageReader4.size();
                continue;
            }
            // BMP
            QImageReader imageReader5(TEMP_PATH "sl1.jpg", "BMP");
            if (imageReader5.canRead())
            {
                sz = imageReader5.size();
                continue;
            }

            sz = QSize(-1, -1);
        }
    }
    UIKERNEL_INFO("--------------------------- 1000 test en ========");
    UIKERNEL_INFO("------------ image size[%d][%d]", sz.width(), sz.height());

//  imageR.setScaledSize(QSize(800, 600));
//  QImage im;
//  UIKERNEL_INFO("\n\n--------------------------- test st ================");
//  if (imageR.read(&im))
//  {
//      if (!im.isNull())
//      {
//          UIKERNEL_INFO("------------ image size[%d][%d]", im.rect().width(), im.rect().height());
//      }
//  }
//  else
//  {
//      UIKERNEL_INFO("----- read error =======");
//  }
    UIKERNEL_INFO("--------------------------- test en ================\n\n");

//  UIKERNEL_INFO("------------ file size(%d, %d) ===========", sz.width(), sz.height());
//  UIKERNEL_INFO("---------------- 8 ===========");
//  GetImageCacheManager().RemoveImage(TEMP_PATH "image_8.jpg");
//  UIKERNEL_INFO("---------------- 7 ===========");
//  GetImageCacheManager().RemoveImage(TEMP_PATH "image_7.jpg");
//  UIKERNEL_INFO("---------------- 4 ===========");
//  GetImageCacheManager().RemoveImage(TEMP_PATH "image_4.jpg");
//  UIKERNEL_INFO("---------------- 2 ===========");
//  GetImageCacheManager().RemoveImage(TEMP_PATH "image_2.jpg");
//  UIKERNEL_INFO("---------------- 1.9 ===========");
//  GetImageCacheManager().RemoveImage(TEMP_PATH "image_1_9.jpg");
//  UIKERNEL_INFO("---------------- 1.8 ===========");
//  GetImageCacheManager().RemoveImage(TEMP_PATH "image_1_8.jpg");
    UIKERNEL_INFO("------------------------- end ========================");
    return true;
}

/**************************** 构造函数测试 开始****************************/
class BaseA
{
public:
    BaseA()
    {
        m_iT = -1;
        printf("------- BaseA() =========\n\n");
    }
    BaseA(int iT) : m_iT(iT)
    {
        printf("------- BaseA(%d) =========\n\n", iT);
    }
    //BaseA(BaseA& a) {}

    void SetT(int iT)
    {
        m_iT = iT;
        printf("------- SetT() iT[%d] =========\n\n", m_iT);
    }
    int GetT()
    {
        return m_iT;
        printf("------- GetT() iT[%d] =========\n\n", m_iT);
    }

protected:
    int     m_iT;
};

class VersionOne : public BaseA
{
public:
    VersionOne()
    {
        printf("------- VersionOne() iT[%d] =========\n\n", m_iT);
    }

private:
    QPixmap     m_pic;
};

class VersionTwo : public BaseA
{
public:
    VersionTwo()
    {
        printf("------- VersionTwo() =========\n\n");
    }
    VersionTwo(int iT) : BaseA(iT)
    {
        printf("------- VersionTwo(%d) =========\n\n", iT);
    }
};

struct nodeA
{
    VersionOne  tOne;

    nodeA(const VersionOne & tO) : tOne(tO) {}
};

std::map<int, nodeA *> mp;
std::map<int, std::map<int, VersionOne>*> mk;

bool ConstructFuncTest()
{
    mp.clear();
    mk.clear();
    VersionOne * b = new VersionOne();
    {
        VersionOne a;
        printf("------------a2132d a[%d], b[%d] =====\n\n", a.GetT(), b->GetT());
        a.SetT(10);
        //b = a;
        //VersionOne* ma = new VersionOne(a);
        //mp.insert(make_pair<1, ma>);
        nodeA * ma = new nodeA(a);
        mp.insert(std::map<int, nodeA *>::value_type(1, ma));
        //mp[1] = ma;
        //printf("------------ 2 a[%d], b[%d], mp[%d] =====\n\n", a.GetT(), b->GetT(), mp[1]->GetT());
    }
    //printf("------------ 3 b[%d], mp[%d] =====\n\n", b->GetT(), mp[1]->GetT());
    VersionOne * c = new VersionOne(*b);
    printf("------------ b[%d], c[%d] =====\n\n", b->GetT(), c->GetT());
    //
//  VersionTwo e, f;
//  printf("------------ e[%d], f[%d] =====\n\n", e.GetT(), f.GetT());
//  e.SetT(10);
//  f = e;
//  VersionTwo g(e);
//  VersionTwo h(e.GetT());
//  printf("------------ e[%d], f[%d], g[%d], h[%d] =====\n\n", e.GetT(), f.GetT(), g.GetT(), h.GetT());
    return true;
}

bool ConstructFuncTest2()
{
    printf("------------ kkkkdddkkkkk mp[%d] =====\n\n", mp[1]->tOne.GetT());
    return true;
}

/**************************** 构造函数测试 结束****************************/

// ImageCacheManager测试代码
void TestImageCacheManager()
{
    // 运行并打印测试用例的结果
//  PrintfCaseResult(1, "加载指定模块下的主题", ImageCacheManagerCase1());
//  PrintfCaseResult(2, "查看主题中的图片", ImageCacheManagerCase2());
//  PrintfCaseResult(3, "删除主题中的一张图片", ImageCacheManagerCase3());
//  PrintfCaseResult(4, "重新加载一个主题", ImageCacheManagerCase4());
//  PrintfCaseResult(5, "释放主题资源", ImageCacheManagerCase5());
//  PrintfCaseResult(6, "设置为不缓存模式，查看加载主题情况", ImageCacheManagerCase6());
//  PrintfCaseResult(7, "加载一张临时图片", ImageCacheManagerCase7());
//  PrintfCaseResult(8, "释放一张临时图片", ImageCacheManagerCase8());
//  //ImageCacheManagerCase9();
//  PrintfCaseResult(10, "添加、释放12张临时图片", ImageCacheManagerCase10());
//  PrintfCaseResult(11, "Lync接口测试", ImageCacheManagerCase11());
//  PrintfCaseResult(12, "Lync接口测试", ImageCacheManagerCase12());
//  //PrintfCaseResult(13, "Lync接口测试", ImageCacheManagerCase13());
//  PrintfCaseResult(14, "Lync接口测试", ImageCacheManagerCase14());
    //PrintfCaseResult(15, "type 类型图片删除接口测试", ImageCacheManagerCase15());
    PrintfCaseResult(16, "type 类型图片删除接口测试", ImageCacheManagerCase16());

    //
//  PrintfCaseResult(16, "构造函数测试", ConstructFuncTest());
//  PrintfCaseResult(17, "构造函数测试2", ConstructFuncTest2());
}

// #endif
