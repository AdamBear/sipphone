#include "callcenter_inc.h"

#if defined(IF_FEATURE_ACD) || defined(IF_FEATURE_BROADSOFT_CALL_CENTER) || defined(IF_FEATURE_GENESYS_ACD) || IF_FEATURE_METASWITCH_ACD

#if IF_FEATURE_START2START_ACD
#include "broadsoft/callcenter/src/acdmodelstart2start.h"
#include "extended/xmlbrowser/include/modxmlbrowser.h"
#endif

static LRESULT ACDProcessMessage(msgObject & objMessage)
{
    if (objMessage.message == CONFIG_MSG_BCAST_CHANGED)
    {
        g_CallCenterController->OnConfigChange(objMessage);
    }

    if (NULL != g_ACDModel)
    {
        g_ACDModel->OnMessage(objMessage);
    }

    if (NULL != g_CallCenterController)
    {
        g_CallCenterController->ProcessMessage(objMessage);
    }

    return 0;
}

CallCenterController * CallCenterController::m_pInst = NULL;

// Static.
CallCenterController * CallCenterController::GetInstance()
{
    // 实例化单键.
    if (m_pInst == NULL)
    {
        m_pInst = new CallCenterController();
        if (m_pInst != NULL)
        {
            m_pInst->MyInit();
        }
    }
    return m_pInst;
}

void CallCenterController::ProcessMessage(msgObject & objMessage)
{
    CALLCENTER_INFO("Controller ProcessMessage(%d),objMessage.wParam(%d) objMessage.lParam(%d)",
                    objMessage.message, objMessage.wParam, objMessage.lParam);
    switch (objMessage.message)
    {
    case SIP_REG_UPDATE_REGISTER:
        {
            int iAccount = objMessage.wParam;
            if (!acc_IsAccountIDValid(iAccount))
            {
                return;
            }

            LINE_STATE eState = acc_MapState(objMessage.lParam);

            // 获取账号旧的可使用状态
            bool bAccountNewUsable = (eState == LS_REGISTERED) || (eState == LS_SYSTEM_DEFAULT_ACCOUNT);

            // sip 上电注册上时发送的消息
#if IF_BUG_30788
            if (ACD_GENESYS == (ACD_TYPE)configParser_GetConfigInt(kszAcdType)
#else
            if (acc_IsGenesysAccount(iAccount)
#endif
                    && objMessage.lParam == LS_INEXISTENCE)
            {
                bAccountNewUsable = TRUE;
                if (configParser_GetCfgItemIntValue(kszACDStatus, objMessage.wParam) == AS_IDLE)
                {
                    configParser_SetCfgItemIntValue(kszACDStatus, objMessage.wParam,
                                                    AS_AVAILABLE);
                }
            }

            // 判断是否是ACD账号的状态发生了改变
            if (m_pAcdModel != NULL && iAccount == m_pAcdModel->GetAccountId())
            {
                // 如果账号可以使用变为无法使用
                if (!bAccountNewUsable)
                {
#if IF_FEATURE_START2START_ACD
                    if (m_pAcdModel->Get_ACDType() == ACD_START2START)
                    {
                        xmlbrowser_ReleaseStatus(m_pAcdModel->GetSessionID());
                    }
                    else
#endif
                    {
                        // 如果
                        if (configParser_GetCfgItemIntValue(kszAccountEnableQueueStatus, iAccount) == 0
                                && m_pAcdModel->Get_ACDType() == ACD_BROADSOFT
                                && ((ACDModel *) m_pAcdModel)->m_bIsQueueStatusOn
                                && ((ACDModel *) m_pAcdModel)->GetAcdStatus() != AS_IDLE)
                        {
                            ((ACDModel *) m_pAcdModel)->ReqQueueStatus(false);
                        }

                        //请求登出ACD
                        m_pAcdModel->ReqChangeACDStatus(AA_LOGOUT);
                    }

                    //直接发送登出消息到外部模块
                    msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME),
                                       ACD_MESSAGE_STATUS_CHANGE, g_ACDModel->GetAccountId(), AS_IDLE);


                    SetACDModelByAccID(-1);//解绑定
                }
            }

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
#if !IF_BUG_25622
            if (m_pHotelingModel != NULL && iAccount == m_pHotelingModel->GetAccountId())
            {
                // 如果账号可以使用变为无法使用
                if (!bAccountNewUsable)
                {
                    hoteling_Request hotelingReq;
                    hotelingReq.m_eStatus = HOTELING_STATUS_LOGOUT;
                    //请求登出Hoteling
                    m_pHotelingModel->ReqChangeHotelingStatus(hotelingReq);

                    //直接发送Hoteling状态登出消息到外部模块
                    msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME),
                                       HOTELING_MESSAGE_STATUS_CHANGE, m_pHotelingModel->GetAccountId(), HOTELING_STATUS_LOGOUT);

                    m_pHotelingModel->SetAccountId(-1);//解绑定
                }
            }
#endif
#endif  //IF_FEATURE_BROADSOFT_CALL_CENTER

            //如acd未绑定成功且有账号注册上且其acd开启，则绑定账号
            if (m_pAcdModel != NULL && m_pAcdModel->GetAccountId() == -1 && bAccountNewUsable)
            {
                CALLCENTER_INFO("AcdSwitch = %d, accountusable = %d",
                                configParser_GetCfgItemIntValue(kszAccountEnableACD, iAccount), acc_IsAccountAvailable(iAccount));
                if (configParser_GetCfgItemIntValue(kszAccountEnableACD, iAccount)
                        && acc_IsAccountAvailable(iAccount))
                {
                    SetACDModelByAccID(iAccount);

#if IF_BUG_30788
                    if (ACD_BROADSOFT == (ACD_TYPE)configParser_GetConfigInt(kszAcdType))
#else
                    if (SST_BROADSOFT == acc_GetServerType(iAccount))
#endif
                    {
                        feature_SetFeatureKeySync(true, iAccount);
                    }
#if IF_FEATURE_START2START_ACD
                    if (m_pAcdModel->Get_ACDType() == ACD_START2START)
                    {
                        // 触发一次同步
                        m_pAcdModel->ReqChangeACDStatus(AA_REFRESH);
                    }
#endif
                }

                //绑定完账号也要通知ACD状态改变，以便刷新界面
                msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME),
                                   ACD_MESSAGE_STATUS_CHANGE, g_ACDModel->GetAccountId(), AS_IDLE);
            }

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
#if IF_BUG_25622
            bool bHotelingOn = (bAccountNewUsable && (acc_GetHotelingSwitch(iAccount)
                                || acc_GetFlexibleSeatingSwitch(iAccount)) ? true : false);
            ProcessSubscibeResult(bHotelingOn, objMessage.wParam);
#else
            //如hoteling未绑定成功且有账号注册上且其acd开启，则绑定账号
            if (m_pHotelingModel != NULL && m_pHotelingModel->GetAccountId() == -1 && bAccountNewUsable)
            {
                CALLCENTER_INFO("Hoteling = %d, accountusable = %d, FlexibleSeatingEnable = %d",
                                configParser_GetCfgItemIntValue(kszAccountEnableHoteling, iAccount),
                                acc_IsAccountAvailable(iAccount),
                                configParser_GetCfgItemIntValue(kszAccountEnableFlexibleSeating, iAccount));
                if ((configParser_GetCfgItemIntValue(kszAccountEnableHoteling, iAccount)
                        || configParser_GetCfgItemIntValue(kszAccountEnableFlexibleSeating, iAccount))
                        && acc_IsAccountAvailable(iAccount))
                {
                    m_pHotelingModel->SetAccountId(iAccount);

#if IF_BUG_30788
                    if (ACD_BROADSOFT == (ACD_TYPE)configParser_GetConfigInt(kszAcdType))
#else
                    if (SST_BROADSOFT == acc_GetServerType(iAccount))
#endif
                    {
                        feature_SetFeatureKeySync(true, iAccount);
                    }
                }

                //绑定完账号也要通知Hoteling状态改变，以便刷新界面
                msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME),
                                   HOTELING_MESSAGE_STATUS_CHANGE, m_pHotelingModel->GetAccountId(), HOTELING_STATUS_LOGOUT);
            }
#endif
#endif  //IF_FEATURE_BROADSOFT_CALL_CENTER
        }
        break;
    case DSK_MSG_ACD_RESULT:
        {
            if (m_pAcdModel == NULL)
            {
                return;
            }

            if (m_ACDLoginTimer.IsTimerRuning())
            {
                //关闭超时计时器
                m_ACDLoginTimer.KillTimer();
            }

            if (objMessage.wParam == m_pAcdModel->GetAccountId())
            {
                //保存状态到本地
                m_pAcdModel->SetStatus((ACD_STATUS)objMessage.lParam);

#if defined(IF_FEATURE_BROADSOFT_CALL_CENTER) || defined(IF_FEATURE_GENESYS_ACD)
                yl::string strReason;//Reason Code
                yl::string strInfo = (char *)objMessage.GetExtraData();
                strInfo = strInfo.substr(0, objMessage.GetExtraSize());
                strReason = strInfo;

                if (ACD_BROADSOFT == m_pAcdModel->Get_ACDType())
                {
                    ((ACDModel *)m_pAcdModel)->SetCurReason(strReason);
                    //获取对应的CodeName
                    strReason = ((ACDModel *)m_pAcdModel)->GetReasonCodeNameByCode(strReason);

                    CALLCENTER_INFO("Current Un Reason[%s]", strReason.c_str());
                }

                etl_NotifyAppEx(false,
                                ACD_MESSAGE_STATUS_CHANGE, objMessage.wParam, objMessage.lParam, strReason.size(),
                                (LPCVOID)strReason.c_str());

                if (m_bIsLaterLogoutHoteling && m_pAcdModel->GetAcdStatus() == AS_IDLE)
                {
                    hoteling_Request hotelingReq;
                    hotelingReq.m_eStatus = HOTELING_STATUS_LOGOUT;
                    if (m_pHotelingModel != NULL)
                    {
                        m_pHotelingModel->ReqChangeHotelingStatus(hotelingReq);
                    }

                    m_bIsLaterLogoutHoteling = false;

                }

                if (ACD_BROADSOFT != m_pAcdModel->Get_ACDType())
                {
                    return;
                }

                ACD_STATUS eStatus = ((ACDModel *) m_pAcdModel)->GetAcdStatus();
                QUEUE_STATUS eQueueStatus = ((ACDModel *) m_pAcdModel)->GetQueueStatus().m_eStatus;
                bool bIsQueueStatusOn = ((ACDModel *) m_pAcdModel)->m_bIsQueueStatusOn;
                bool bIslaterQeqQueueStatus = ((ACDModel *) m_pAcdModel)->m_bIslaterQeqQueueStatus;

                CALLCENTER_INFO("acd status [%d] queue status[%d] laterQeqQueueStatus[%d] is queue on[%d]", eStatus, eQueueStatus, bIslaterQeqQueueStatus, bIsQueueStatusOn);

                //登陆acd后是否需要订阅queueStatus
                //队列功能开启，状态登陆上，且队列状态为未知，则重新订阅队列状态（容错）
                if (bIslaterQeqQueueStatus
                        || (eQueueStatus == QUEUE_NONE
                            &&  eStatus != AS_IDLE
                            && bIsQueueStatusOn))
                {
                    ((ACDModel *)m_pAcdModel)->ReqQueueStatus();
                    ((ACDModel *)m_pAcdModel)->m_bIslaterQeqQueueStatus = false;
                }


                //登出acd成功，如有订阅Status，则取消订阅
                //登陆acd后是否需要订阅queueStatus
                if ((eStatus == AS_IDLE && bIsQueueStatusOn))
                {
                    ((ACDModel *)m_pAcdModel)->ReqQueueStatus(false);

                    powerled_FlushPowerLed();
                }

#endif  //IF_FEATURE_BROADSOFT_CALL_CENTER

                //调用UI刷新接口
                //UIManager_UpdateWnd();
            }
        }
        break;
    case SIP_SUB_UPDATE_ACD_STATUS:
        {
            if (objMessage.lParam <= 0)
            {
                return;
            }

            sip_acd_status_info_t * pData = (sip_acd_status_info_t *)objMessage.GetExtraData();

            if (pData == NULL)
            {
                return;
            }

            if (m_ACDLoginTimer.IsTimerRuning())
            {
                //关闭超时计时器
                m_ACDLoginTimer.KillTimer();
            }

            ACD_TYPE eACDType = (m_pAcdModel != NULL) ? m_pAcdModel->Get_ACDType() : ACD_BASE;
            if (ACD_COSMOCOM != eACDType && ACD_GENESYS != eACDType)
            {
                return;
            }

            ACD_STATUS eStatus = AS_IDLE;

#if IF_BUG_30788
            bool bGenAdv = ACD_GENESYS == (ACD_TYPE)configParser_GetConfigInt(kszAcdType);
#else
            bool bGenAdv = acc_GetServerType(m_pAcdModel->GetAccountId()) == SST_GENESYS_ADV;
#endif
            switch (pData->status)
            {
            case SIP_ACD_STATUS_LOGIN_FAIL:
                {
                    etl_NotifyApp(false, ACD_MESSAGE_ACTION_RESULT, m_pAcdModel->GetAccountId(), 1);
                    return;
                }
                break;
            case SIP_ACD_STATUS_LOGIN_SUCCESS:
                {
                    etl_NotifyApp(false, ACD_MESSAGE_ACTION_RESULT, m_pAcdModel->GetAccountId(), 0);
                    if (bGenAdv)
                    {
                        m_pAcdModel->ReqChangeACDStatus(AA_AVAIL);
                    }
                    return;
                }
                break;
            case SIP_ACD_STATUS_AVAILABLE:
                {
                    eStatus = AS_AVAILABLE;
                }
                break;
            case SIP_ACD_STATUS_UNAVAILABLE:
                {
                    eStatus = AS_UNAVAILABLE;
                }
                break;
            case SIP_ACD_STATUS_LOGOUT:
                {
                    eStatus = AS_IDLE;
                }
                break;
            case SIP_ACD_STATUS_NOTIFY_200OK:
                {
                    if (bGenAdv && eACDType == ACD_GENESYS)
                    {
                        ((CACDModelGenesys *)m_pAcdModel)->OnACDNotify();
                        return;
                    }
                }
                break;
            default:
                return;
            }

            if (m_pAcdModel->GetAcdStatus() == eStatus)
            {
                if (eACDType == ACD_GENESYS && bGenAdv
                        && !((CACDModelGenesys *)m_pAcdModel)->GetACDNotifyFlag())
                {
                    // Genesys Adv 没有设置Notify标志位
                    return;
                }

                //http://10.2.1.199/Bug.php?BugID=43851
                //服务器会回目前状态
                etl_NotifyApp(false, ACD_MESSAGE_LOGIN_FAIL, m_pAcdModel->GetAccountId(), 0);
                return;
            }

            m_pAcdModel->SetStatus(eStatus);

            //发送Hoteling状态改变消息到外部模块
            etl_NotifyApp(false, ACD_MESSAGE_STATUS_UPDATE, m_pAcdModel->GetAccountId(), eStatus);
        }
        break;
#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    case SIP_SUB_UPDATE_HOTELING_STATUS:
        {
#if IF_BUG_25622
            switch (objMessage.lParam)
            {
            case SIP_HOTELING_LOGIN_AUTH_FAIL:
                {
                    return;
                }
                break;
            case SIP_HOTELING_SUBSCRIBE_FAIL:
                {
                    ProcessSubscibeResult(false, objMessage.wParam);
                    return;
                }
                break;
            case SIP_HOTELING_SUBSCRIBE_SUCCESS:
                {
                    ProcessSubscibeResult(true, objMessage.wParam);
                    return;
                }
                break;
            default:
                break;
            }
#else
            if (objMessage.lParam >= 2)
            {
                //lParam新增值3用于上报hoteling业务订阅是否成功. 暂时不处理
                return;
            }
#endif
            CALLCENTER_INFO("HOTELING_STATUS_UPDATE Status(%d)", (HOTELING_STATUS)objMessage.wParam);
            if (m_hotelingTimer.IsTimerRuning())
            {
                //关闭超时计时器
                m_hotelingTimer.KillTimer();
            }

            if (m_pHotelingModel == NULL)
            {
                return;
            }

            // 保存状态到本地
            HOTELING_STATUS eStatus = HOTELING_STATUS_NONE;
            switch (objMessage.lParam)
            {
            case SIP_HOTELING_LOGOUT_SUCCESS:
                eStatus = HOTELING_STATUS_LOGOUT;
                break;
            case SIP_HOTELING_LOGIN_SUCCESS:
                eStatus = HOTELING_STATUS_LOGIN;
                break;
            default:
                break;
            }

            //http://10.2.1.199/Bug.php?BugID=43763
            //错误的数据，服务器直接回了一个目前hoteling状态的notif
            if (eStatus == m_pHotelingModel->GetHotelingStatus())
            {
                etl_NotifyApp(false,
                              HOTELING_MESSAGE_REQ_FAIL, m_pHotelingModel->GetAccountId(), eStatus);

                return;
            }

            //To Do for test
            m_pHotelingModel->SetStatus(eStatus);

            //Hoteling显示名
            yl::string strDispalyName;

            yl::string strInfo = (char *)objMessage.GetExtraData();
            strInfo = strInfo.substr(0, objMessage.GetExtraSize());
            strDispalyName = strInfo;

            //保存disPlayName
            m_pHotelingModel->SetDisPlayName(strDispalyName);

            //发送Hoteling状态改变消息到外部模块
            etl_NotifyAppEx(false,
                            HOTELING_MESSAGE_STATUS_CHANGE, m_pHotelingModel->GetAccountId(), eStatus, strDispalyName.size(),
                            (LPCVOID)strDispalyName.c_str());

            //做个容错，登陆请求服务器回200ok，sip回状态变为off的问题
            if (eStatus == HOTELING_STATUS_LOGIN)
            {
                m_hotelwaitTimer.SetTimer(1000, NULL, MK_MSG_ENTRY(this,
                                          CallCenterController::OnTimeoutHotelMessage));
            }

            //调用UI刷新接口
            //UIManager_UpdateWnd();
        }
        break;
    case SIP_SUB_UPDATE_CALL_CENTER_STATUS:
        {
            //解析acd队列信息
            yl::string strInfo;
            yl::string strInfo1 = (char *)objMessage.GetExtraData();
            strInfo = strInfo1;

            //memcpy(&strInfo,objMessage.GetExtraData(),objMessage.GetExtraSize());
            CALLCENTER_INFO("The xml info: %s", strInfo.c_str());
            if (m_pAcdModel != NULL)
            {
                if (ACD_BROADSOFT == m_pAcdModel->Get_ACDType()
                        && AS_IDLE != m_pAcdModel->GetAcdStatus())
                {
                    if (((ACDModel *)m_pAcdModel)->ProcessQueueStatusXml(strInfo))
                    {
                        CallCenterQueueStatus queueInfo = ((ACDModel *)m_pAcdModel)->GetQueueStatus();
                        CALLCENTER_INFO("The queue Status is %d", queueInfo.m_eStatus);

                        etl_NotifyAppEx(false, ACD_MESSAGE_QUEUE_STATUS, objMessage.wParam, queueInfo.m_eStatus, 0, NULL);

                        if (IsEnableQueueStatusLightOn())
                        {
                            powerled_FlushPowerLed();
                        }
                    }
                }
            }
        }
        break;
    case SIP_SUB_FEATURE_KEY_SUBSCRIBED_RESULT:
        {
            CALLCENTER_INFO("SIP_SUB_FEATURE_KEY_SUBSCRIBED_RESULT(%d,%d)", objMessage.wParam,
                            objMessage.lParam);
            if (NULL == objMessage.GetExtraData())
            {
                break;
            }

            sip_reason_info_t * pResult = (sip_reason_info_t *)objMessage.GetExtraData();
            if (0 == pResult->code)
            {
                //Hoteling登陆成功后，如还需登陆Acd则进入ACD登陆流程
                if (m_bIsLaterLoginACD)
                {
                    if (m_pAcdModel != NULL
                            && m_pAcdModel->GetAccountId() == objMessage.wParam)
                    {
                        if (m_hotelwaitTimer.IsTimerRuning())
                        {
                            //关闭超时计时器
                            m_hotelwaitTimer.KillTimer();
                        }
                        m_pAcdModel->ReqLogin();
                    }
                    m_bIsLaterLoginACD = false;
                }
            }
        }
        break;
#endif  //IF_FEATURE_BROADSOFT_CALL_CENTER
#if IF_FEATURE_START2START_ACD
    case ACD_MESSAGE_ACTION_AUTO_REFRESH:
        {
            if (m_pAcdModel == NULL
                    || ACD_START2START != m_pAcdModel->Get_ACDType())
            {
                return;
            }

            m_pAcdModel->ReqChangeACDStatus(AA_REFRESH);
        }
        break;
    case ACD_MESSAGE_S2S_STATUS_CHANGE:
        {
            if (m_pAcdModel == NULL
                    || ACD_START2START != m_pAcdModel->Get_ACDType())
            {
                return;
            }

            m_pAcdModel->SetStatus((ACD_STATUS)objMessage.lParam);
        }
        break;
#endif
#if IF_FEATURE_METASWITCH_ACD
    case MTSW_MESSAGE_LOGIN:
        {
            if (IsMTSWACDEnable())
            {
                if (m_pAcdModel != NULL
                        && m_pAcdModel->Get_ACDType() != ACD_METASWITCH)
                {
                    int iAccID = acc_GetAcdAccountID();
                    SetACDModelByAccID(iAccID);
                }

                if (m_pAcdModel != NULL
                        && m_pAcdModel->Get_ACDType() == ACD_METASWITCH)
                {
                    ((CACDModelMTSW *)m_pAcdModel)->DownloadData(EMTSWLOGDTYPE_All);
                }
            }
        }
        break;
    case MTSW_MESSAGE_AUTH_ERR:
        {
            // mtsw 登录失败则刷新界面
            if (m_pAcdModel != NULL
                    && m_pAcdModel->Get_ACDType() == ACD_METASWITCH)
            {
                SetACDModelByAccID(-1);
                // 通知ui刷新
                etl_NotifyApp(false, ACD_MESSAGE_STATUS_CHANGE, 0, 0);
            }
        }
        break;
#endif
    default:
        break;
    }

}

void CallCenterController::OnConfigChange(msgObject & objMessage)
{
    if (objMessage.wParam == ST_CALLCENTER_UNREASON)
    {
        if (m_pAcdModel != NULL
                && ACD_BROADSOFT == m_pAcdModel->Get_ACDType()
                && m_pAcdModel->GetAccountId() == objMessage.lParam)
        {
            //((ACDModel*)m_pAcdModel)->GetReasonMap();
        }
    }
#if IF_FEATURE_METASWITCH_ACD
    else if (objMessage.wParam == ST_MTSW_ACD_CONFIG)
    {
        CALLCENTER_INFO("ST_MTSW_ACD_CONFIG ACDType(%d), isMtswAcdEnable(%d)", m_pAcdModel->Get_ACDType(),
                        IsMTSWACDEnable());
        if (m_pAcdModel != NULL)
        {
            if (IsMTSWACDEnable()
                    && m_pAcdModel->GetAccountId() == -1)
            {
                int iAccID = acc_GetAcdAccountID();
                SetACDModelByAccID(iAccID);

                if (m_pAcdModel->Get_ACDType() == ACD_METASWITCH)
                {
                    ((CACDModelMTSW *)m_pAcdModel)->DownloadData(EMTSWLOGDTYPE_All);
                }
            }
            else  if (m_pAcdModel->Get_ACDType() == ACD_METASWITCH)
            {
                SetACDModelByAccID(-1);
                // 通知ui刷新
                etl_NotifyApp(false, ACD_MESSAGE_STATUS_CHANGE, 0, 0);
            }
        }
    }
#endif
    //else if (objMessage.wParam == ST_SIPLINE)
    //{
    //    CALLCENTER_INFO("callcenter config change wParam[%d] lParam[%d]", objMessage.wParam, objMessage.lParam);
    //    if (NULL == m_pAcdModel)
    //    {
    //        return;
    //    }

    //    if (m_pAcdModel->GetAccountId() != objMessage.lParam)
    //    {
    //        return;
    //    }

    //    bool bQueueStatus = ((ACDModel *) m_pAcdModel)->m_bIsQueueStatusOn;

    //    if (bQueueStatus &&
    //            0 == configParser_GetCfgItemIntValue(kszAccountEnableQueueStatus, objMessage.lParam))
    //    {
    //        ((ACDModel *) m_pAcdModel)->ReqQueueStatus(false);

    //        ((ACDModel *) m_pAcdModel)->SetAccountId(objMessage.lParam);

    //        // 通知ui刷新
    //        etl_NotifyApp(false, ACD_MESSAGE_QUEUE_STATUS, 0, 0);
    //    }
    //}
}

bool CallCenterController::Login(ACDLogin & refData)
{
    bool succ = false;
#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    if (!refData.m_bIsUseHost)
    {
        //HostelIng流程
        hoteling_Request hostReq;
        hostReq.m_bIsAuto = refData.m_bIsAuto;
        hostReq.m_eStatus = HOTELING_STATUS_LOGIN;
        hostReq.m_strUser = refData.m_strUser;
        hostReq.m_strPassword = refData.m_strPassword;

        int hotelingAccountId = -1;


        if (m_pHotelingModel != NULL)
        {
            succ = m_pHotelingModel->ReqChangeHotelingStatus(hostReq);

            hotelingAccountId = m_pHotelingModel->GetAccountId();
        }

        if (!succ)
        {
            return false;
        }
        int nInterVal = 5 * 1000; //设置超时时间5s

        m_hotelingTimer.SetTimer(nInterVal, NULL, MK_MSG_ENTRY(this,
                                 CallCenterController::OnTimeoutMessage));

        if (hotelingAccountId != -1
                && configParser_GetCfgItemIntValue(kszAccountEnableACD, hotelingAccountId) == 1
                && m_pAcdModel != NULL)
        {
            CALLCENTER_INFO("later login ACD!");

            SetACDModelByAccID(hotelingAccountId);
            //ACD流程
            m_bIsLaterLoginACD =
                true;//Hoteling 登陆成功才登陆ACD，不然可能会导致Hoteling登陆不成功
        }
    }
    else
#endif  //IF_FEATURE_BROADSOFT_CALL_CENTER
    {
        if (m_pAcdModel != NULL)
        {
            if (ACD_BROADSOFT == m_pAcdModel->Get_ACDType())
            {
                succ = m_pAcdModel->ReqLogin();
            }
            else if (ACD_COSMOCOM == m_pAcdModel->Get_ACDType()
                     || ACD_GENESYS == m_pAcdModel->Get_ACDType())
            {
                int nInterVal = 5 * 1000; //设置超时时间30s
                m_ACDLoginTimer.SetTimer(nInterVal, NULL, MK_MSG_ENTRY(this,
                                         CallCenterController::OnACDTimeoutMessage));


                // 设置登录ACD的账号参数
                acd_accout tAcd;
                strcpy(tAcd.user, refData.m_strUser.c_str());
                strcpy(tAcd.password, refData.m_strPassword.c_str());

                succ = m_pAcdModel->ReqLogin(&tAcd);
            }
        }
    }

    return succ;
}

bool CallCenterController::IsACDOn()
{
    if (m_pAcdModel == NULL)
    {
        return false;
    }
    return m_pAcdModel->GetAccountId() != -1;
}

// bool CallCenterController::ReqChangeACDStatus(ACD_ACTION eACDAction)
// {
//  if (m_pAcdModel == NULL)
//  {
//      return false;
//  }
//  return m_pAcdModel->ReqChangeACDStatus(eACDAction);
// }

CallCenterController::CallCenterController(void)
{
    m_pAcdModel = NULL;
#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    m_pHotelingModel = NULL;
    m_bIsLaterLogoutHoteling = false;
    m_bIsLaterLoginACD = false;
#endif  //IF_FEATURE_BROADSOFT_CALL_CENTER
}

CallCenterController::~CallCenterController(void)
{
    if (m_pInst != NULL)
    {
        MyFinal();
        delete m_pInst;
        m_pInst = NULL;
    }
}

void CallCenterController::MyInit()
{
#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    m_bIsLaterLoginACD = false;
#endif  //IF_FEATURE_BROADSOFT_CALL_CENTER

    if (m_pAcdModel == NULL)
    {
        int iAccID = acc_GetAcdAccountID();
        SetACDModelByAccID(iAccID);
    }

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    if (m_pHotelingModel == NULL)
    {
        m_pHotelingModel = new HotelingModel();
    }
#endif  //IF_FEATURE_BROADSOFT_CALL_CENTER

    static CDsskeyAcdTrace dsskeyAcdTrace;
    static CDssKeyACD dsskeyAcd;
    static CDssKeyHoteling dsskeyHotel;
    static CDsskeyDispositionCode dsskyeDis;
    static CDsskeyEmergencyEscalation dsskyeEsc;
}

void CallCenterController::RegistMsg()
{
    //注册消息
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, ACDProcessMessage);

    etl_RegisterMsgHandle(SIP_REG_UPDATE_REGISTER, SIP_REG_UPDATE_REGISTER, ACDProcessMessage);

    etl_RegisterMsgHandle(DSK_MSG_ACD_RESULT, DSK_MSG_ACD_RESULT, ACDProcessMessage);

    etl_RegisterMsgHandle(SIP_SUB_UPDATE_HOTELING_STATUS, SIP_SUB_UPDATE_HOTELING_STATUS,
                          ACDProcessMessage);

    etl_RegisterMsgHandle(SIP_SUB_UPDATE_CALL_CENTER_STATUS, SIP_SUB_UPDATE_CALL_CENTER_STATUS,
                          ACDProcessMessage);

    etl_RegisterMsgHandle(SIP_SUB_UPDATE_ACD_STATUS, SIP_SUB_UPDATE_ACD_STATUS, ACDProcessMessage);

    etl_RegisterMsgHandle(TALK_MSG_TALK_START, TALK_MSG_TALK_START, ACDProcessMessage);

    etl_RegisterMsgHandle(TALK_MSG_SESSION_DESTORY, TALK_MSG_SESSION_DESTORY, ACDProcessMessage);

    // sip未实现
//  etl_RegisterMsgHandle(DSK_MSG_ACD_ACTION_RESULT,DSK_MSG_ACD_ACTION_RESULT,ACDProcessMessage);

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    etl_RegisterMsgHandle(SIP_SUB_FEATURE_KEY_SUBSCRIBED_RESULT, SIP_SUB_FEATURE_KEY_SUBSCRIBED_RESULT,
                          ACDProcessMessage);
#endif  //IF_FEATURE_BROADSOFT_CALL_CENTER

#if IF_FEATURE_START2START_ACD
    etl_RegisterMsgHandle(ACD_MESSAGE_ACTION_AUTO_REFRESH, ACD_MESSAGE_S2S_STATUS_CHANGE,
                          ACDProcessMessage);
#endif
#if IF_FEATURE_METASWITCH_ACD
    etl_RegisterMsgHandle(MTSW_MESSAGE_LOGIN, MTSW_MESSAGE_LOGIN, ACDProcessMessage);
    etl_RegisterMsgHandle(MTSW_MESSAGE_AUTH_ERR, MTSW_MESSAGE_AUTH_ERR, ACDProcessMessage);
#endif
}

void CallCenterController::MyFinal()
{
    if (m_pAcdModel != NULL)
    {
        delete m_pAcdModel;
        m_pAcdModel = NULL;
    }

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    if (m_pHotelingModel != NULL)
    {
        delete m_pHotelingModel;
        m_pHotelingModel = NULL;
    }
#endif  //IF_FEATURE_BROADSOFT_CALL_CENTER

    //取消注册消息
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, ACDProcessMessage);

    etl_UnregisterMsgHandle(SIP_REG_UPDATE_REGISTER, SIP_REG_UPDATE_REGISTER, ACDProcessMessage);

    etl_UnregisterMsgHandle(DSK_MSG_ACD_RESULT, DSK_MSG_ACD_RESULT, ACDProcessMessage);

    etl_UnregisterMsgHandle(SIP_SUB_UPDATE_HOTELING_STATUS, SIP_SUB_UPDATE_HOTELING_STATUS,
                            ACDProcessMessage);

    etl_UnregisterMsgHandle(SIP_SUB_UPDATE_CALL_CENTER_STATUS, SIP_SUB_UPDATE_CALL_CENTER_STATUS,
                            ACDProcessMessage);

    etl_UnregisterMsgHandle(SIP_SUB_UPDATE_ACD_STATUS, SIP_SUB_UPDATE_ACD_STATUS, ACDProcessMessage);

    // sip未实现
//  etl_UnregisterMsgHandle(DSK_MSG_ACD_ACTION_RESULT,DSK_MSG_ACD_ACTION_RESULT,ACDProcessMessage);

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    etl_UnregisterMsgHandle(SIP_SUB_FEATURE_KEY_SUBSCRIBED_RESULT,
                            SIP_SUB_FEATURE_KEY_SUBSCRIBED_RESULT, ACDProcessMessage);
#endif  //IF_FEATURE_BROADSOFT_CALL_CENTER

#if IF_FEATURE_START2START_ACD
    etl_UnregisterMsgHandle(ACD_MESSAGE_ACTION_AUTO_REFRESH, ACD_MESSAGE_S2S_STATUS_CHANGE,
                            ACDProcessMessage);
#endif

#if IF_FEATURE_METASWITCH_ACD
    etl_UnregisterMsgHandle(MTSW_MESSAGE_LOGIN, MTSW_MESSAGE_LOGIN, ACDProcessMessage);
    etl_UnregisterMsgHandle(MTSW_MESSAGE_AUTH_ERR, MTSW_MESSAGE_AUTH_ERR, ACDProcessMessage);
#endif
}

// 根据账号确认ACD
void CallCenterController::SetACDModelByAccID(int iAccID)
{
    if (m_pAcdModel != NULL)
    {
        delete m_pAcdModel;
        m_pAcdModel = NULL;
    }

    ACD_TYPE eType = ACD_BASE;

    if (acc_IsAccountIDValid(iAccID))
    {
#if IF_BUG_30788
        eType = (ACD_TYPE)configParser_GetConfigInt(kszAcdType);

        if (ACD_BROADSOFT == eType)
        {
            // Broadsoft功能没打开
            eType = (configParser_GetConfigInt(kszBroadsoftActive) == 0) ? ACD_COSMOCOM : ACD_BROADSOFT;
        }
#else
        SIP_SERVER_TYPE eServerType = (SIP_SERVER_TYPE)acc_GetServerType(iAccID);

        if (SST_BROADSOFT == eServerType)
        {
            // Broadsoft功能没打开
            eType = (configParser_GetConfigInt(kszBroadsoftActive) == 0) ? ACD_COSMOCOM : ACD_BROADSOFT;
        }
        else if (SST_GENESYS == eServerType
                 || SST_GENESYS_ADV == eServerType)
        {
            eType = ACD_GENESYS;
        }
#if IF_FEATURE_START2START_ACD
        else if (SST_START2START == eServerType)
        {
            eType = ACD_START2START;
        }
#endif
        else
        {
            eType = ACD_COSMOCOM;
        }
#endif

#if IF_FEATURE_METASWITCH_ACD
        if (IsMTSWACDEnable())
        {
            eType = ACD_METASWITCH;
        }
#endif

    }

    CALLCENTER_INFO("SetACDModelByAccID(iAccID = %d) eType = %d", iAccID, eType);

    // 根据新ACD类型生成ACD对象
    switch (eType)
    {
#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    case ACD_BROADSOFT:
        m_pAcdModel = new ACDModel();
        break;
#endif  //IF_FEATURE_BROADSOFT_CALL_CENTER
#ifdef IF_FEATURE_ACD
    case ACD_COSMOCOM:
        m_pAcdModel = new CACDModelCosmocom();
        break;
#endif
#ifdef IF_FEATURE_GENESYS_ACD
    case ACD_GENESYS:
        m_pAcdModel = new CACDModelGenesys();
        break;
#endif
#if IF_FEATURE_START2START_ACD
    case ACD_START2START:
        m_pAcdModel = new CACDModelStart2Start();
        break;
#endif
#if IF_FEATURE_METASWITCH_ACD
    case ACD_METASWITCH:
        m_pAcdModel = new CACDModelMTSW();
        break;
#endif
    default:
        m_pAcdModel = new CACDModelBase();
        break;
    }

    m_pAcdModel->SetAccountId(iAccID);
}

//处理ACD计时器到时消息.
BOOL CallCenterController::OnACDTimeoutMessage(msgObject & refObj)
{
    CALLCENTER_INFO("CallCenterController::OnACDTimeoutMessage:ACD Login!");
    if (m_ACDLoginTimer.IsTimerRuning())
    {
        //关闭超时计时器
        m_ACDLoginTimer.KillTimer();
    }

    //发送消息通知UI请求超时
    etl_NotifyApp(false, ACD_MESSAGE_LOGIN_TIMEOUT, 0, 0);
    return TRUE;
}

BOOL CallCenterController::OnTimeoutMessage(msgObject & refObj)
{
#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    if (m_hotelingTimer.IsTimerRuning())
    {
        //关闭超时计时器
        m_hotelingTimer.KillTimer();
    }

    if (m_bIsLaterLoginACD)
    {
        m_bIsLaterLoginACD = false;
    }

    //发送消息通知UI请求超时
    etl_NotifyApp(false,
                  HOTELING_MESSAGE_REQ_TIMEOUT, 0, 0);
#endif  //IF_FEATURE_BROADSOFT_CALL_CENTER
    return TRUE;
}

BOOL CallCenterController::OnTimeoutHotelMessage(msgObject & refObj)
{
#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    if (m_hotelwaitTimer.IsTimerRuning())
    {
        //关闭超时计时器
        m_hotelwaitTimer.KillTimer();
    }

    //Hoteling登陆成功后，如还需登陆Acd则进入ACD登陆流程
    if ((m_bIsLaterLoginACD && !IsFlexibleSeatingEnable())
            || (IsFlexibleSeatingEnable() && m_pHotelingModel->GetHotelingStatus() == HOTELING_STATUS_LOGIN))
    {
        if (m_pAcdModel != NULL)
        {
            m_pAcdModel->ReqLogin();
        }
        m_bIsLaterLoginACD = false;
    }
#endif  //IF_FEATURE_BROADSOFT_CALL_CENTER
    return TRUE;
}

bool CallCenterController::Logout()
{
    ACD_STATUS eAcdStatus = AS_IDLE;
    if (m_pAcdModel != NULL)
    {
        m_pAcdModel->ReqChangeACDStatus(AA_LOGOUT);
        eAcdStatus = m_pAcdModel->GetAcdStatus();
    }

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    if (m_pHotelingModel != NULL && (eAcdStatus == AS_IDLE))
    {
        hoteling_Request hotelingReq;
        hotelingReq.m_eStatus = HOTELING_STATUS_LOGOUT;
        m_pHotelingModel->ReqChangeHotelingStatus(hotelingReq);
    }
    else if (m_pHotelingModel != NULL)
    {
        m_bIsLaterLogoutHoteling = true;
    }
#endif  //IF_FEATURE_BROADSOFT_CALL_CENTER

    return true;

}

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER

HOTELING_STATUS CallCenterController::GetHotelingStatus()
{
    if (m_pHotelingModel != NULL)
    {
        return m_pHotelingModel->m_eStatus;
    }

    return HOTELING_STATUS_NONE;
}
//
// bool CallCenterController::OriginatedTrace()
// {
//  if (m_pAcdModel != NULL)
//  {
//      return m_pAcdModel->OriginatedTrace();
//  }
//  return false;
// }
//
// bool CallCenterController::SendDispCode(const yl::string& strCode)
// {
//  if (m_pAcdModel != NULL)
//  {
//      return m_pAcdModel->SendDispCode(strCode);
//  }
//  return false;
// }
//
// bool CallCenterController::EmergencyEscalation(const yl::string& strNumber)
// {
//  if (m_pAcdModel != NULL)
//  {
//      return m_pAcdModel->EmergencyEscalation(strNumber);
//  }
//  return false;
// }

//ACD功能是否开启

//Hoteling功能是否开启
bool CallCenterController::IsHotelingOn()
{
    if (m_pHotelingModel == NULL)
    {
        return false;
    }
    return m_pHotelingModel->GetAccountId() != -1;
}

//ACD Trace 功能是否开启
bool CallCenterController::IsTraceOn()
{
    if (m_pAcdModel == NULL || m_pAcdModel->GetAccountId() == -1)
    {
        return false;
    }
    return configParser_GetCfgItemIntValue(kszAccountEnableOriginatedTrace,
                                           m_pAcdModel->GetAccountId()) == 1;
}

//DispCode 功能是否开启
bool CallCenterController::IsDispCodeOn()
{
    if (m_pAcdModel == NULL || m_pAcdModel->GetAccountId() == -1)
    {
        return false;
    }
    return configParser_GetCfgItemIntValue(kszAccountEnableDiapositionCode,
                                           m_pAcdModel->GetAccountId()) == 1;
}

//EmergencyEscalation功能是否开启
bool CallCenterController::IsEmergencyEscalationOn()
{
    if (m_pAcdModel == NULL || m_pAcdModel->GetAccountId() == -1)
    {
        return false;
    }
    return configParser_GetCfgItemIntValue(kszAccountEnableEmergencyEscalation,
                                           m_pAcdModel->GetAccountId()) == 1;
}

int CallCenterController::GetCallInfoShowTime()
{
    if (m_pAcdModel == NULL || m_pAcdModel->GetAccountId() == -1)
    {
        return false;
    }
    return configParser_GetCfgItemIntValue(kszAccountCallinfoTime, m_pAcdModel->GetAccountId());
}

bool CallCenterController::IsCallInfoOn()
{
    if (m_pAcdModel == NULL || m_pAcdModel->GetAccountId() == -1)
    {
        return false;
    }
    return configParser_GetCfgItemIntValue(kszAccountEnableCallInfo, m_pAcdModel->GetAccountId()) == 1;
}

bool CallCenterController::IsEnableQueueStatusLightOn()
{
    if (m_pAcdModel == NULL || m_pAcdModel->GetAccountId() == -1)
    {
        return false;
    }
    return configParser_GetCfgItemIntValue(kszAccountEnableQueueStatusLight,
                                           m_pAcdModel->GetAccountId()) == 1;
}

bool CallCenterController::IsQueueStatusOn()
{
    if (m_pAcdModel == NULL || m_pAcdModel->GetAccountId() == -1)
    {
        return false;
    }
    return configParser_GetCfgItemIntValue(kszAccountEnableQueueStatus,
                                           m_pAcdModel->GetAccountId()) == 1;

}

#if IF_BUG_25622
// 处理sip返回的订阅结果
void CallCenterController::ProcessSubscibeResult(bool bHotelingOn, int nAccountId)
{
    CALLCENTER_INFO("HotelingStatus[%d], nAccountId[%d]", bHotelingOn, nAccountId);

    if (m_pHotelingModel == NULL)
    {
        return ;
    }

    // 更新Hoteling订阅状态
    m_pHotelingModel->UpdateHotelingSubscibeResult(nAccountId, bHotelingOn);

    CALLCENTER_INFO("m_pHotelingModel->GetAccountId[%d]", m_pHotelingModel->GetAccountId());

    bool bHandle = true;
    // 服务器HotelingHost状态从Off->On
    if (bHotelingOn && -1 == m_pHotelingModel->GetAccountId())
    {
        if (acc_IsAccountAvailable(nAccountId) && (acc_GetHotelingSwitch(nAccountId)
                || acc_GetFlexibleSeatingSwitch(nAccountId)))
        {
            m_pHotelingModel->SetAccountId(nAccountId);
        }
    }
    // 服务器HotelingHost状态从On->Off
    else if (!bHotelingOn && nAccountId == m_pHotelingModel->GetAccountId())
    {
        // 若账号不可用通知服务器
        // 此段代码实际上没意义，暂时先保留
        if (!acc_IsAccountAvailable(nAccountId))
        {
            hoteling_Request hotelingReq;
            hotelingReq.m_eStatus = HOTELING_STATUS_LOGOUT;
            //请求登出Hoteling
            m_pHotelingModel->ReqChangeHotelingStatus(hotelingReq);
        }

        if (!m_pHotelingModel->GetDisPlayName().empty())
        {
            m_pHotelingModel->SetDisPlayName("");
        }

        int nNextHotelingAccountId = m_pHotelingModel->GetFirstHotelingOn();
        m_pHotelingModel->SetAccountId(nNextHotelingAccountId);//解绑定
    }
    else
    {
        bHandle = false;
    }

    if (bHandle)
    {
        // 通知Hoteling状态改变，刷新界面
        msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME),
                           HOTELING_MESSAGE_STATUS_CHANGE, m_pHotelingModel->GetAccountId(), HOTELING_STATUS_LOGOUT);
    }
}
#endif

yl::string  CallCenterController::GetLoginName()
{
    return (m_pAcdModel != NULL) ? m_pAcdModel->GetLoginName() : "";
}

bool CallCenterController::IsFlexibleSeatingEnable(int iAccountId/* = -1*/)
{
    if (NULL == m_pHotelingModel)
    {
        return false;
    }

    return m_pHotelingModel->IsFlexibleSeatingEnable(iAccountId);
}

#endif //IF_FEATURE_BROADSOFT_CALL_CENTER

#if IF_FEATURE_METASWITCH_ACD
bool CallCenterController::IsMTSWACDEnable()
{
    if (configParser_GetConfigInt(kszBroadsoftActive) == 0
            && configParser_GetConfigInt(kszMetaSwitchActive) != 0
            && configParser_GetConfigInt(kszMTSWACDEnable) != 0
            && configParser_GetConfigInt(kszMTSWCommportalEnable) != 0)
    {
        return true;
    }
    return false;
}
#endif


#endif  //IF_FEATURE_ACD || IF_FEATURE_BROADSOFT_CALL_CENTER
