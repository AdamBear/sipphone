#ifndef _C_LOAD_IMAGE_TASKACTION_H
#define _C_LOAD_IMAGE_TASKACTION_H

#include "taskaction/modtaskaction.h"
#include "taskaction/taskaction.h"
#include "taskaction/taskactioncallbackbase.h"
#include "imagecachedefine.h"
#include "xwindow/xGraphic.h"

using namespace NS_TA;

enum LoadImageTaskType
{
    LOAD_IMAGE_TASK_BASE                        = 0x00000000,
    LOAD_IMAGE_TASK_BY_PATH                     = 0x01000000,
    LOAD_IMAGE_TASK_BY_STREAM                   = 0x01010000,
};

class CLoadImageTaskActionBase : public CTaskAction
{
public:
    explicit CLoadImageTaskActionBase(int nType = LOAD_IMAGE_TASK_BASE);

    virtual ~CLoadImageTaskActionBase();

    static CLoadImageTaskActionBase* GetLoadImageTaskActionBase(CTaskAction* pTaskAction);

    // 重写执行函数
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

    xPixmap& GetImage();

    void SetImageSize(chSize sizeImage);
    chSize  GetImageSize();
    int GetType();

    bool IsLoadSucceed()
    {
        return m_bLoadSucceed;
    }

protected:
    bool            m_bLoadSucceed;
    chSize          m_sizeImage;
    int             m_nTaskType;
    xPixmap         m_pmImg;
};

class CLoadImageTaskAction : public CLoadImageTaskActionBase
{
public:
    explicit CLoadImageTaskAction(int nType = LOAD_IMAGE_TASK_BY_PATH);

    virtual ~CLoadImageTaskAction();

    static int GetLoadImageTaskActionType();
    static CLoadImageTaskAction* GetLoadImageTaskAction(CLoadImageTaskActionBase* pTaskAction);

    // 重写执行函数
    virtual bool Exec();

    void SetImagePath(const yl::string & strPath);
    yl::string GetImagePath();

private:
    yl::string  m_strPath;
};

class CLoadStreamImageTaskAction : public CLoadImageTaskActionBase
{
public:
    explicit CLoadStreamImageTaskAction(int nType = LOAD_IMAGE_TASK_BY_STREAM);

    virtual ~CLoadStreamImageTaskAction();

    static int GetLoadStreamImageTaskActionType();
    static CLoadStreamImageTaskAction* GetLoadStreamImageTaskAction(CLoadImageTaskActionBase* pTaskAction);

    // 重写执行函数
    virtual bool Exec();

    bool SetStreamInfo(const char* pFlow, int iFlowSize);

private:
    char    *m_pFlowCache;
    int     m_nFlowSize;
};

#endif // _C_LOAD_IMAGE_TASKACTION_H
