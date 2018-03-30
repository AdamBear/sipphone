#include "loadimagetaskaction.h"
#include "taskaction/common.h"
#include "imagefileinfo.h"
#include "imagecachemanager.h"
#include <ETLLib/ETLLib.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
// CLoadImageTaskActionBase
CLoadImageTaskActionBase::CLoadImageTaskActionBase(int nType/* = LOAD_IMAGE_TASK_BASE*/)
    : m_bLoadSucceed(false)
    , m_sizeImage(chSize(-1, -1))
    , m_nTaskType(nType)
{
    //必须要设置这个类型才可以使用
    m_eTARequestType = NS_TA::TA_REQUEST_LOCAL_FILE;
    CTaskAction::SetSync(false);  // 异步.
    CTaskAction::SetSync(false);  // 异步.
}

CLoadImageTaskActionBase::~CLoadImageTaskActionBase()
{
}

bool CLoadImageTaskActionBase::Exec()
{
    UIKERNEL_WARN("No task would be done in CLoadImageTaskActionBase.");
    return false;
}

xPixmap& CLoadImageTaskActionBase::GetImage()
{
    return m_pmImg;
}

void CLoadImageTaskActionBase::SetImageSize(chSize sizeImage)
{
    m_sizeImage = sizeImage;
}

chSize CLoadImageTaskActionBase::GetImageSize()
{
    return m_sizeImage;
}

int CLoadImageTaskActionBase::GetType()
{
    return m_nTaskType;
}

CLoadImageTaskActionBase* CLoadImageTaskActionBase::GetLoadImageTaskActionBase(CTaskAction* pTaskAction)
{
    if (NULL == pTaskAction || NS_TA::TA_REQUEST_LOCAL_FILE != pTaskAction->GetReqType())
    {
        return NULL;
    }

    return static_cast<CLoadImageTaskActionBase*>(pTaskAction);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// CLoadImageTaskAction
CLoadImageTaskAction::CLoadImageTaskAction(int nType /*= LOAD_IMAGE_TASK_BY_PATH*/)
    : CLoadImageTaskActionBase(nType)
{
    CTaskAction::SetRead(true);  // 读取文件.
}

CLoadImageTaskAction::~CLoadImageTaskAction()
{

}

bool CLoadImageTaskAction::Exec()
{
    if (!pathFileExist(m_strPath.c_str()) || !CImageFileInfo::IsImage(m_strPath.c_str()))
    {
        return false;
    }

    xPixmap pmImage;

    if (!pmImage.load(m_strPath))
    {
        return false;
    }

    int nImgWidth = pmImage.width();
    int nImgHeight = pmImage.height();

    if (m_sizeImage.cx > 0 && m_sizeImage.cy > 0
            && (nImgWidth != m_sizeImage.cx || nImgHeight != m_sizeImage.cy))
    {
        m_pmImg.ScaleCopy(pmImage);
    }
    else
    {
        m_pmImg = pmImage;
    }

    m_bLoadSucceed = true;

    SetTaskActionErrorType(TA_NO_ERROR);

    return true;
}

void CLoadImageTaskAction::SetImagePath(const yl::string & strPath)
{
    m_strPath = strPath;
}

yl::string CLoadImageTaskAction::GetImagePath()
{
    return m_strPath;
}

int CLoadImageTaskAction::GetLoadImageTaskActionType()
{
    return LOAD_IMAGE_TASK_BY_PATH;
}

CLoadImageTaskAction* CLoadImageTaskAction::GetLoadImageTaskAction(CLoadImageTaskActionBase* pTaskAction)
{
    if (NULL == pTaskAction || LOAD_IMAGE_TASK_BY_PATH != (LOAD_IMAGE_TASK_BY_PATH & pTaskAction->GetType()))
    {
        return NULL;
    }

    return static_cast<CLoadImageTaskAction*>(pTaskAction);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// CLoadStreamImageTaskAction

CLoadStreamImageTaskAction::CLoadStreamImageTaskAction(int nType/* = LOAD_IMAGE_TASK_BY_STREAM*/)
    : CLoadImageTaskActionBase(nType)
    , m_pFlowCache(NULL)
    , m_nFlowSize(-1)
{

}

CLoadStreamImageTaskAction::~CLoadStreamImageTaskAction()
{
    if (NULL != m_pFlowCache)
    {
        delete m_pFlowCache;
        m_pFlowCache = NULL;
        m_nFlowSize = 0;
    }
}

bool CLoadStreamImageTaskAction::Exec()
{
    if (NULL == m_pFlowCache || 0 >= m_nFlowSize)
    {
        return false;
    }

    xPixmap pmImage;

    if (!pmImage.load(m_pFlowCache, m_nFlowSize))
    {
        return false;
    }

    int nImgWidth = pmImage.width();
    int nImgHeight = pmImage.height();

    if (m_sizeImage.cx > 0 && m_sizeImage.cy > 0
            && (nImgWidth != m_sizeImage.cx || nImgHeight != m_sizeImage.cy))
    {
        m_pmImg.ScaleCopy(pmImage);
    }
    else
    {
        m_pmImg = pmImage;
    }

    m_bLoadSucceed = true;

    if (NULL != m_pFlowCache)
    {
        delete m_pFlowCache;
        m_pFlowCache = NULL;
        m_nFlowSize = 0;
    }

    SetTaskActionErrorType(TA_NO_ERROR);
    return true;
}

bool CLoadStreamImageTaskAction::SetStreamInfo(const char* pFlow, int iFlowSize)
{
    if (NULL == pFlow || 0 >= m_nFlowSize)
    {
        return false;
    }

    if (NULL != m_pFlowCache)
    {
        delete m_pFlowCache;
        m_pFlowCache = NULL;
        m_nFlowSize = 0;
    }

    m_pFlowCache = new char(iFlowSize);
    memcpy(m_pFlowCache, pFlow, iFlowSize);
    m_nFlowSize = iFlowSize;

    return true;
}

int CLoadStreamImageTaskAction::GetLoadStreamImageTaskActionType()
{
    return LOAD_IMAGE_TASK_BY_STREAM;
}

CLoadStreamImageTaskAction* CLoadStreamImageTaskAction::GetLoadStreamImageTaskAction(CLoadImageTaskActionBase* pTaskAction)
{
    if (NULL == pTaskAction || LOAD_IMAGE_TASK_BY_STREAM != (LOAD_IMAGE_TASK_BY_STREAM & pTaskAction->GetType()))
    {
        return NULL;
    }

    return static_cast<CLoadStreamImageTaskAction*>(pTaskAction);
}

