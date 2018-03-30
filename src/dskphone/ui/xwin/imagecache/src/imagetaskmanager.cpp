#include "imagetaskmanager.h"
#include <ETLLib/ETLLib.hpp>
#include "loadimagetaskaction.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "imagecachemanager.h"

CImageTaskManager::CImageTaskManager()
{

}

CImageTaskManager::~CImageTaskManager()
{
    ClearImageInfo();
}

void CImageTaskManager::Init()
{
    ClearImageInfo();
}

void CImageTaskManager::UnInit()
{
    ClearImageInfo();
}

void CImageTaskManager::ClearImageInfo()
{
    m_mapTask.clear();
}

CImageTaskManager& CImageTaskManager::GetImageTaskManager()
{
    static CImageTaskManager s_ImageTaskManager;

    return s_ImageTaskManager;
}

bool CImageTaskManager::AddImageTask(const yl::string& strPath, int nClassifyType, chSize& sizeImg/* = chSize(0, 0)*/)
{
    MAP_IMAGE_TASK::iterator iter = m_mapTask.find(strPath);

    if (iter != m_mapTask.end())
    {
        int nClassType = iter->second;
        if (nClassType == nClassifyType)
        {
            UIKERNEL_WARN("AddImageTask the same task %s, nClassifyType is %d.", strPath.c_str(), nClassifyType);
            return false;
        }
    }

    m_mapTask[strPath] = nClassifyType;

    CLoadImageTaskAction *pImageTask = new CLoadImageTaskAction;
    if (NULL != pImageTask)
    {
        pImageTask->SetRead(true);  // 读取文件.
        pImageTask->SetCallback(this);  // 回调函数.
        pImageTask->SetSync(false);  // 异步.
        pImageTask->SetImagePath(strPath);
        bool bRet = TaskAction_ExecAction(pImageTask);
        if (!bRet)
        {
            UIKERNEL_WARN("AddImageTask::TaskAction_ExecAction failed.");
            return false;
        }
    }
    return true;
}

bool CImageTaskManager::RemoveImageTask(const yl::string& strPath)
{
    MAP_IMAGE_TASK::iterator iter = m_mapTask.find(strPath);
    if (iter != m_mapTask.end())
    {
        m_mapTask.erase(iter);
        return true;
    }
    return false;
}

int CImageTaskManager::GetClassifier(const yl::string& strPath)
{
    MAP_IMAGE_TASK::iterator iter = m_mapTask.find(strPath);
    if (iter != m_mapTask.end())
    {
        return iter->second;
    }
    return IMG_CLASSIFY_NONE;
}

void CImageTaskManager::OnTaskActionCallback(CTaskAction* pTaskAction)
{
    if (NULL == pTaskAction)
    {
        return;
    }

    if (pTaskAction->GetReqType() != NS_TA::TA_REQUEST_LOCAL_FILE)
    {
        return;
    }

    // 返回结果的类型为请求图片.
    CLoadImageTaskActionBase* pImageActionBase = CLoadImageTaskActionBase::GetLoadImageTaskActionBase(pTaskAction);
    if (NULL == pImageActionBase)
    {
        return;
    }

    CLoadImageTaskAction* pImageAction = CLoadImageTaskAction::GetLoadImageTaskAction(pImageActionBase);
    if (NULL == pImageAction)
    {
        return;
    }

    if (pImageAction->IsOperationSucceed() && pImageAction->IsLoadSucceed())
    {
        yl::string strImagePath = pImageAction->GetImagePath();
        int nClassifyType = GetClassifier(strImagePath);
        RemoveImageTask(strImagePath);

        CImageInfoBase* pImageInfo = g_ImageCacheMgr.FindImageInfo(strImagePath);
        if (NULL != pImageInfo)
        {
            xPixmap& pixMap = pImageInfo->GetPixmap();

            pixMap = pImageAction->GetImage();

            CImageCacheBase::OnLoadImageInfo(pImageInfo);

            etl_NotifyAppEx(false, MSG_IMAGE_MESSAGE_LOAD_FINISH, true, nClassifyType,
                            strImagePath.size() + 1, strImagePath.c_str());
        }
    }

    // 结束任务
    pImageAction = NULL;
}
