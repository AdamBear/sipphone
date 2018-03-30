#include "anonymouscallcontroller.h"

#include "commonapi/stringhelperapi.h"
#include "feature/include/modfeature.h"
#include "setting/include/common.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "interfacedefine.h"

#include "anonymouscallmodel.h"
#include "basemethod.h"

#include "account/include/modaccount.h"
#include "account/include/accountmessagedefine.h"

#include <stdlib.h>

namespace NS_SETTING
{
//////////////////////////////////////////////////////////////////////////
// Class CAnonymousCallController
CAnonymousCallController * CAnonymousCallController::m_pController = NULL;
// 最大的账号数.
static const int knMaxAccountNumber = 4;

#define INVALID_LINE_ID -1

CAnonymousCallController * CAnonymousCallController::GetInstance()
{
    // Instantiate the singleton if not existed.
    if (m_pController == NULL)
    {
        m_pController = new CAnonymousCallController();
        if (m_pController != NULL)
        {
            // Do initialize.
            m_pController->MyInit();
        }
    }
    return m_pController;
}

void CAnonymousCallController::ReleaseInstance()
{
    // Release the singleton if existed.
    if (m_pController != NULL)
    {
        // Do finalize.
        m_pController->MyFinal();

        delete m_pController;
        m_pController = NULL;
    }
}

bool CAnonymousCallController::GetPageMenuListByClickAction(
    MenuItemListData * pData, const yl::string & strClickAction) const
{
    // Check the click action.
    if (pData == NULL || strClickAction.empty())
    {
        return false;
    }

    // Call the base method.
    return NS_SETTING::GetPageMenuListByClickAction(pData, strClickAction);
}

const yl::string CAnonymousCallController::GetTitleByClickAction(
    const yl::string & strClickAction) const
{
    if (strClickAction.empty())
    {
        return "";
    }

    // Call the base method.
    return NS_SETTING::GetTitleByClickAction(strClickAction);
}

bool CAnonymousCallController::GetAnonymousCallDataByLineId(
    AnonymousCallData * pData, int nLineId) const
{
    if (pData == NULL || m_pModel == NULL)
    {
        return false;
    }

    // Read anonymous call status.
    pData->m_isAnonymousCallOn = m_pModel->IsAnonymousOn(nLineId);
    pData->m_isAnonymousSendCode = m_pModel->GetAnonymousSendCode(nLineId);

    // Read anonymous call on code.
    const yl::string strAnonyOnCode = m_pModel->GetAnonymousOnCode(nLineId);
    strncpy(pData->m_stCodeInfoAnonymousCall.m_szCodeOn,
            strAnonyOnCode.c_str(),
            chMIN(sizeof(pData->m_stCodeInfoAnonymousCall.m_szCodeOn), strAnonyOnCode.length()));
    pData->m_stCodeInfoAnonymousCall.m_szCodeOn[CodeInfo::knMaxCodeLength] = '\0';

    // Read anonymous call off code.
    const yl::string strAnonyOffCode = m_pModel->GetAnonymousOffCode(nLineId);
    strncpy(pData->m_stCodeInfoAnonymousCall.m_szCodeOff,
            strAnonyOffCode.c_str(),
            chMIN(sizeof(pData->m_stCodeInfoAnonymousCall.m_szCodeOff), strAnonyOffCode.length()));
    pData->m_stCodeInfoAnonymousCall.m_szCodeOff[CodeInfo::knMaxCodeLength] = '\0';

    // Read local rejection status.
    pData->m_isRejectAnonymousCallOn = m_pModel->IsRejectionOn(nLineId);
    // Read remote rejection status.
    pData->m_AnonymousRejectSendCode = m_pModel->GetAnonymousRejectSendCode(nLineId);

    // Read rejection on code.
    const yl::string strRejectOnCode = m_pModel->GetRejectionOnCode(nLineId);
    strncpy(pData->m_stCodeInfoRejectAnonymousCall.m_szCodeOn,
            strRejectOnCode.c_str(),
            chMIN(sizeof(pData->m_stCodeInfoRejectAnonymousCall.m_szCodeOn), strRejectOnCode.length()));
    pData->m_stCodeInfoRejectAnonymousCall.m_szCodeOn[CodeInfo::knMaxCodeLength] = '\0';

    // Read anonymous call off code.
    const yl::string strRejectOffCode = m_pModel->GetRejectionOffCode(nLineId);
    strncpy(pData->m_stCodeInfoRejectAnonymousCall.m_szCodeOff,
            strRejectOffCode.c_str(),
            chMIN(sizeof(pData->m_stCodeInfoRejectAnonymousCall.m_szCodeOff), strRejectOffCode.length()));
    pData->m_stCodeInfoRejectAnonymousCall.m_szCodeOff[CodeInfo::knMaxCodeLength] = '\0';
    return true;
}

void CAnonymousCallController::SetAnonymousCallDataByLineId(
    int nLineId, const AnonymousCallData & refData)
{
    if (m_pModel != NULL)
    {
        // 获取以前的状态.
        //bool isAnonyPrev = m_pModel->IsAnonymousOn(nLineId);

        // Set anonymous call status.
        m_pModel->SetAnonymousStatus(
            nLineId, refData.m_isAnonymousCallOn);

        m_pModel->SetAnonymousSendCode(
            nLineId, refData.m_isAnonymousSendCode);

        // Set anonymous call on code.
        m_pModel->SetAnonymousOnCode(
            nLineId, refData.m_stCodeInfoAnonymousCall.m_szCodeOn);

        // Set anonymous call off code.
        m_pModel->SetAnonymousOffCode(
            nLineId, refData.m_stCodeInfoAnonymousCall.m_szCodeOff);

        // 获取以前的状态.
        // bool isRejectAnonyPrev = m_pModel->IsRejectionOn(nLineId);

        // Set rejection status.
        m_pModel->SetRejectionStatus(nLineId,
                                     refData.m_isRejectAnonymousCallOn);

        // Set rejection code swtich
        m_pModel->SetAnonymousRejectSendCode(
            nLineId, refData.m_AnonymousRejectSendCode);

        // Set rejection call on code.
        m_pModel->SetRejectionOnCode(nLineId,
                                     refData.m_stCodeInfoRejectAnonymousCall.m_szCodeOn);

        // Set rejection call off code.
        m_pModel->SetRejectionOffCode(
            nLineId, refData.m_stCodeInfoRejectAnonymousCall.m_szCodeOff);

        //Notify Config Change
        msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, nLineId);
        //异步通知账号配置改变
        etl_NotifyApp(false, CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, nLineId);
    }
}

bool CAnonymousCallController::GetAnonymousCallDataByClickAction(
    AnonymousCallData * pData, const yl::string & strClickAction) const
{
    if (pData == NULL || strClickAction.empty())
    {
        return false;
    }

    // Get line id.
    int nId = commonAPI_GetIdFromStrAction(strClickAction);
//  int nId = CommonAPI_ParseIdByClickAction(strClickAction, kszCAMaskAnonymousCall);
    return nId != -1 ? GetAnonymousCallDataByLineId(pData, nId) : false;
}

bool CAnonymousCallController::IsAdministratorHasSet(const yl::string & strClickAction)
{
    if (NULL == m_pModel)
    {
        return false;
    }

    int nId = commonAPI_GetIdFromStrAction(strClickAction);

    if (INVALID_LINE_ID == nId)
    {
        return false;
    }

    return m_pModel->IsAdministratorHasSet(nId);
}

void CAnonymousCallController::SetAnonymousCallDataByClickAction(
    const yl::string & strClickAction, const AnonymousCallData & refData)
{
    if (strClickAction.empty())
    {
        return;
    }
    int nId = commonAPI_GetIdFromStrAction(strClickAction);
//  int nId = CommonAPI_ParseIdByClickAction(strClickAction, kszCAMaskAnonymousCall);
    if (nId != -1)
    {
        SetAnonymousCallDataByLineId(nId, refData);
    }
}

bool CAnonymousCallController::MyInit()
{
    // Instantiate the model instance.
    if (m_pModel == NULL)
    {
        m_pModel = new CAnonymousCallModel();

        // 初始化时, 同步code.
        if (m_pModel != NULL)
        {
            AnonymousCallData stACD;
            for (int i = 0; i < knMaxAccountNumber; ++i)
            {
                stACD.Reset();
                if (!GetAnonymousCallDataByLineId(&stACD, i))
                {
                    continue;
                }

                // 发送同步.
                /*
                // if (g_pFeatureCode != NULL)
                {
                    if (stACD.m_isAnonymousCallOn)
                    {
                        //g_pFeatureCode->SendCodeToServer(
                        //    i, stACD.m_stCodeInfoAnonymousCall.m_szCodeOn);
                        _FeatureCode.SendCodeToServer(i, stACD.m_stCodeInfoAnonymousCall.m_szCodeOn);
                    }
                    else
                    {
                        //g_pFeatureCode->SendCodeToServer(
                        //    i, stACD.m_stCodeInfoAnonymousCall.m_szCodeOff);
                        _FeatureCode.SendCodeToServer(i, stACD.m_stCodeInfoAnonymousCall.m_szCodeOff);
                    }

                    if (stACD.m_isRejectAnonymousCallOn)
                    {
                        //g_pFeatureCode->SendCodeToServer(
                        //    i, stACD.m_stCodeInfoRejectAnonymousCall.m_szCodeOn);
                        _FeatureCode.SendCodeToServer(i, stACD.m_stCodeInfoRejectAnonymousCall.m_szCodeOn);
                    }
                    else
                    {
                        //g_pFeatureCode->SendCodeToServer(
                        //    i, stACD.m_stCodeInfoRejectAnonymousCall.m_szCodeOff);
                        _FeatureCode.SendCodeToServer(i, stACD.m_stCodeInfoRejectAnonymousCall.m_szCodeOff);
                    }
                }*/
            }  //for.
        }
    }

    return m_pModel != NULL;
}

bool CAnonymousCallController::MyFinal()
{
    // Release the model instance.
    if (m_pModel != NULL)
    {
        delete m_pModel;
        m_pModel = NULL;
    }

    return m_pModel == NULL;
}

CAnonymousCallController::CAnonymousCallController()
    : m_pModel(NULL)
{

}

CAnonymousCallController::~CAnonymousCallController()
{

}

}  // namespace NS_SETTING
