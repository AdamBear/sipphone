#ifndef _C_IMAGE_TASKACTION_MANAGER_H
#define _C_IMAGE_TASKACTION_MANAGER_H

#include <ETLLib/ETLLib.hpp>
#include "ylhashmap.h"
#include "imagecachedefine.h"
#include "imageinfo.h"
#include "taskaction/taskaction.h"
#include "taskaction/taskactioncallbackbase.h"

using namespace NS_TA;

typedef YLHashMap<yl::string, int>      MAP_IMAGE_TASK;

class CImageTaskManager : public CTaskActionCallbackBase
{
public:
    CImageTaskManager();

    ~CImageTaskManager();

    static CImageTaskManager& GetImageTaskManager();

    void Init();
    void UnInit();

    // 添加执行任务函数
    bool AddImageTask(const yl::string& strPath, int nClassifyType, chSize& sizeImg);
    // 根据路径和类别移除任务
    bool RemoveImageTask(const yl::string& strPath);
    // 获取类别
    int GetClassifier(const yl::string& strPath);
    // 回调函数. 当一次异步读写完毕时, 会调用此函数.
    virtual void OnTaskActionCallback(CTaskAction* pTaskAction);

private:
    void ClearImageInfo();

private:
    MAP_IMAGE_TASK              m_mapTask;
};

#define g_ImageTaskMgr CImageTaskManager::GetImageTaskManager()

#endif // _C_IMAGE_TASKACTION_MANAGER_H
