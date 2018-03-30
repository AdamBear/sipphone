#ifndef __XML_BROWSER_LOAD_MNG_H__
#define __XML_BROWSER_LOAD_MNG_H__

#ifdef IF_FEATURE_XMLBROWSER

#include <singletonclassdefine.h>
#include "taskaction/modtaskaction.h"
#include "taskaction/taskactioncallbackbase.h"
#include "taskaction/taskaction.h"
#include "logiccommon.h"
#include "yllist.h"
#include "../include/logicanduicommon.h"
using namespace NS_TA;

// 操作类型
enum LOAD_OPT_TYPE
{
    OPT_DOWNLOAD    = 0x01,     // 下载
    OPT_XML_PARSE   = 0x01 << 1, // XML解析
#define OPT_XML_LOAD_PARSE (OPT_DOWNLOAD|OPT_XML_PARSE)

    OPT_COMPRESS    = 0x01 << 2, // 压缩
#if IF_FEATURE_XMLBROWSER_HOTELING
    OPT_HOTELING    = 0x01 << 3, // Hoteling请求
#endif
};
// 类型判断
#define IS_LOAD(opts,opt) ((opts&opt) != 0)

// 加载参数
struct LoadParam
{
    LoadParam()
    {
        strPath.clear();
        strTmpPath.clear();
        nOptions = 0;
        nElementID = -1;
        eFileType = XB_FILE_UNKNOWN;
        nDsskeyId = -1;
        bScreenSaverType = false;
    }

    yl::string strPath;
    yl::string strTmpPath;
    int         nOptions;
    int         nElementID;
    LOAD_FILE_TYPE eFileType;
    int nDsskeyId;
    bool bScreenSaverType;
};
// 加载链表类型
typedef YLList<LoadParam *> listLoadParam;


// 获取随机文件
yl::string xGetUniqueLoadName();

//////////////////////////////////////////////////////////////////////////
class CXMLBrowserLoadMng : public CTaskActionCallbackBase
{
    DECLARE_SINGLETON_CLASS(CXMLBrowserLoadMng)

public:

    // 加载文件
    // | lpszPath | [in] 文件路径
    // | nOptions | [in] 操作类型
    bool LoadFile(const char * lpszPath, int nOptions, LOAD_FILE_TYPE eFileType,
                  int nElemID, void * pExtData = NULL);
    // 取消加载，目前直接取消所有
    bool CancelLoad();

    // 重写回调函数
    virtual void OnTaskActionCallback(CTaskAction * pTaskAction);

#if IF_BUG_23718
    // 上传文件到服务器
    bool UploadFile(const yl::string & strFilePath, const yl::string & strServerURL);
#endif

private:
    // 下载
    bool Download(yl::string & strNetFilePath, LOAD_FILE_TYPE eLoadFileType = XB_FILE_XML,
                  int nOptions = 0);
    void OnDownloadDone(CTaskAction * pTaskAction);

    // 解析XML
    bool ParseXml(const yl::string & strPath);
    void OnParseXmlDone(CTaskAction * pTaskAction);

    // 压缩图片
    bool CompressImage(const yl::string & strPath, const CompressImgParam & refImgParam);
    void OnCompressImageDone(CTaskAction * pTaskAction);

    // 添加
    bool AddLoadParam(const yl::string & strPath, int nOptions, LOAD_FILE_TYPE eFileType, int nElemID);
    // 查找
    LoadParam * GetLoadParam(const yl::string & strPath);
    // 删除
    void DeleteLoadParam(const yl::string & strPath, bool bDeleteFile = true);
    // 释放
    bool ReleaseLoadParam();

    //
    void FreeParam(LoadParam * pParam, bool bDeleteFile = true);

#if IF_FEATURE_XMLB_SCREENSAVER
    // 处理xml屏保文件下载解析错误
    void PressScreenSaverError();
#endif

private:
    listLoadParam   m_listLoadParam;    // 加载列表
};

#define _XMLBrowserLoadMng GET_SINGLETON_INSTANCE(CXMLBrowserLoadMng)

#endif // #ifdef IF_FEATURE_XMLBROWSER
#endif // __XML_BROWSER_LOAD_MNG_H__

