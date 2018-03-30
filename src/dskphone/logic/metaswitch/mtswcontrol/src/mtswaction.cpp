#if IF_FEATURE_METASWITCH
#include "mtswaction.h"
#include "modmtsw.h"

#include "ylstringalgorithm.h"
#include "mtswactioncmdbase.h"


CMTSWAction::CMTSWAction(const yl::string & strLocalFilePath, const yl::string & strRemoteFilePath)
    : CNetworkFileAction(strLocalFilePath, strRemoteFilePath)
{
    // 设置TaskAction的类型.
    m_eTARequestType = NS_TA::TA_REQUEST_NETWORK_FILE;
    m_pCallback = NULL;
    m_nType = MTSW_UT_None;
    m_bAsyncParse = FALSE;
    m_pcJSON = NULL;

    m_bIsSessionExpired = false;
}

CMTSWAction::~CMTSWAction()
{
    if (m_pcJSON != NULL)
    {
        cJSON_Delete(m_pcJSON);
        m_pcJSON = NULL;
    }
    m_pCallback = NULL;
}

bool CMTSWAction::Exec()
{
    // 调用基类进行下载.
    bool suc = CNetworkFileAction::Exec();

    // 如果下载成功, 则解析.
    if (IsRead() && IsOperationSucceed())
    {
        if (m_nType == ACTIONTYPE_VMFILE)
        {
            return suc;
        }

        yl::string strContent = yl::dump_file(GetFilePath());

        m_pcJSON = cJSON_Parse(strContent.c_str());

        if (!m_pcJSON)
        {
            //SetTaskActionErrorType(NS_TA::TA_OTHER_ERROR);
            APP_INFO("Not JSON FILE,Parse Error ");
            return true;
        }

        if (ProcessServerError())
        {
            if (m_bIsSessionExpired)
            {
                SetTaskActionErrorType(NS_TA::TA_SESSION_EXPIRED);
            }
            else
            {
                SetTaskActionErrorType(NS_TA::TA_OTHER_ERROR);
            }
            return false;
        }
    }

    return suc;
}


bool CMTSWAction::ProcessServerError()
{
    SessionErrorType eErroyType = ST_ERROR_OTHER;

    cJSON * pJson = cJSON_GetObjectItem(m_pcJSON, "data");

    if (pJson && pJson->child)
    {
        cJSON * pError = cJSON_GetObjectItem(pJson->child, "errors");

        if (pError && pError->child)
        {
            cJSON * pType = cJSON_GetObjectItem(pError->child, "type");

            if (pType)
            {
                if (strcmp(pType->valuestring, "sessionExpired") == 0)
                {
                    m_bIsSessionExpired = true;
                }

                return true;
            }
        }
    }

    return false;
}

cJSON * CMTSWAction::GetJsonObject()
{
    if (m_pcJSON == NULL)
    {
        return NULL;
    }

    cJSON * pJson = cJSON_GetObjectItem(m_pcJSON, "data");

    if (pJson && pJson->child)
    {
        return pJson->child;
    }

    return NULL;
}
#endif
