#ifndef __XML_BROWSER_ACTION_H__
#define __XML_BROWSER_ACTION_H__

#ifdef IF_FEATURE_XMLBROWSER

#include "taskaction/modtaskaction.h"
#include "../include/logicanduicommon.h"
#include "xmlbrowserloadmng.h"

//////////////////////////////////////////////////////////////////////////
class CXMLBrowserAction : public CTaskAction
{
public:
    CXMLBrowserAction(LOAD_OPT_TYPE eLoadType)
    {
        m_eLoadAction = eLoadType;
        m_eTARequestType = TA_REQUEST_XML_BROWSER;
    }
    virtual ~CXMLBrowserAction() { /*empty*/ }

    // 纯虚函数, 执行事件.
    virtual bool Exec() = 0;

    /***
     * @brief : Exec过程中产生的资源回收
     * @param : |bCallBackSucceed| 是否回调成功
     * @notice: 涉及异步, 只能操作自身资源
     */
    virtual bool ReleaseData(bool bCallBackSucceed) = 0;

    // 获取任务类型
    LOAD_OPT_TYPE GetLoadActionType()
    {
        return m_eLoadAction;
    }

private:
    LOAD_OPT_TYPE   m_eLoadAction;
};

//////////////////////////////////////////////////////////////////////////
class CImageCompressAction : public CXMLBrowserAction
{
public:
    CImageCompressAction(const yl::string strImageFile, const CompressImgParam & refImageParam)
        : CXMLBrowserAction(OPT_COMPRESS)
    {
        m_strImageFile = strImageFile;
        m_stImageParam = refImageParam;
        m_iErrCode = ERR_OK;
    }

    enum
    {
        ERR_OK,
        ERR_EXIST,
        ERR_PARAM,
        ERR_PIXEL,
        ERR_COMPRESS,
        ERR_UNKNOWN,
    };

    // 重写读写函数
    virtual bool Exec();

    /***
     * @brief : Exec过程中产生的资源回收
     * @param : |bCallBackSucceed| 是否回调成功
     * @notice: 涉及异步, 只能操作自身资源
     */
    virtual bool ReleaseData(bool bCallBackSucceed)
    {
        return false;
    }

    // 原文件
    yl::string GetImageFile()
    {
        return m_strImageFile;
    }
    // 压缩参数
    CompressImgParam & GetParam()
    {
        return m_stImageParam;
    }
    // 错误码
    int GetErrorCode()
    {
        return m_iErrCode;
    }

private:
    yl::string      m_strImageFile;
    CompressImgParam m_stImageParam;
    int              m_iErrCode;
};

#endif // #ifdef IF_FEATURE_XMLBROWSER

#endif // __XML_BROWSER_ACTION_H__
