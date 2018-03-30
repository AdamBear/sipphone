#include "dsskey_inc.h"

static LRESULT OnMessageReceive(msgObject & msg)
{
    return _DsskeyUIData.OnMessage(msg);
}

CDsskeyUIData::CDsskeyUIData() : m_iLineKeyPerPage(0), m_FunCheckFilter(NULL), m_iConfMsgDepth(0)
{
    SingleMsgReg(CONFIG_MSG_BCAST_CHANGED, OnMessageReceive);
    SingleMsgReg(DSSKEY_CONF_CHANGE, OnMessageReceive);
    m_iLineKeyPerPage = GetKeyPerPage();
    RegisterHandle();
}

void CDsskeyUIData::RegisterHandle()
{
    m_FunCheckFilter = dsskey_data_check_imp;
}

LRESULT CDsskeyUIData::OnMessage(msgObject & msg)
{
    switch (msg.message)
    {
    case CONFIG_MSG_BCAST_CHANGED:
        {
            IF_TimeTrace;
            ++m_iConfMsgDepth;

            if (ST_DSSKEY == msg.wParam)
            {
                Reload();
                m_FunLogic.SaveRegexp(DSS_TEMP_REGEXP_FILE);
                SaveFile(DSS_TEMP_FRAM_FILE);
            }
            else if (Find(m_vecMsgHook, msg.wParam))
            {
                // 创建静态函数结果
                if (m_FunLogic.CreateFuctionResult())
                {
                    SaveFile(DSS_TEMP_FRAM_FILE);
                    dsskey_ReloadDSSKeyData();
                    // 重置多余的Dsskey
                    ResetNotAvailDsskey();
                }
            }

            --m_iConfMsgDepth;
            return true;
        }
    case DSSKEY_CONF_CHANGE:
        {
            if (m_iConfMsgDepth != 0)
            {
                return false;
            }

            // dsskey改变时先计算静态函数结果
            if (m_FunLogic.CreateFuctionResult())
            {
                SaveFile(DSS_TEMP_FRAM_FILE);
            }

            return true;
        }
    default:
        break;
    }

    return false;
}

// 特殊类型处理
DIR_TYPE CDsskeyUIData::GetDIRType(int iDsskeyType)
{
    return (DIR_TYPE)(GetDsskeyXmlData(iDsskeyType).iClassType);
}

const yl::string & CDsskeyUIData::GetDefaultLabel(DssKey_Type eType)
{
    return GetDsskeyXmlData(eType).strDisplayName;
}

bool CDsskeyUIData::IsSupportType(const DsskeyPageInfo & pageInfo, DssKey_Type eType)
{
    if (!IsShowType(eType))
    {
        return false;
    }

    return FindDsskeyInSet(pageInfo, eType, true) || FindDsskeyInSet(pageInfo, eType, false);
}

bool CDsskeyUIData::IsEditable(DssKey_Type eType)
{
    CDsskeyInrInfo & dsskeyXmlData = GetDsskeyXmlData(eType);
    return IsAuth(MODE_LEVEL_WRITE_AUTH_INDEX, dsskeyXmlData.chAuth);
}

bool CDsskeyUIData::IsShowType(DssKey_Type eType)
{
    // 三级权限和PSTN均有读权限
    CDsskeyInrInfo & dsskeyXmlData = GetDsskeyXmlData(eType);
    return IsAuth(MODE_LEVEL_BROWSE_AUTH_INDEX, dsskeyXmlData.chAuth)
           && IsAuth(PSTN_BROWSE_AUTH_INDEX, dsskeyXmlData.chAuth);
}

bool CDsskeyUIData::IsNeedPassword(DssKey_Type eType)
{
    yl::string strPsw = configParser_GetConfigString(kszDsskeyLockPassword);

    if (strPsw.empty())
    {
        return false;
    }

    CDsskeyInrInfo & dsskeyXmlData = GetDsskeyXmlData(eType);
    return !IsAuth(MODE_LEVEL_PASSWORD_AUTH_INDEX, dsskeyXmlData.chAuth);
}

bool CDsskeyUIData::IsAuth(int iIndex, LPCSTR strAuth)
{
    if (iIndex < 0 && iIndex > PSTN_BROWSE_AUTH_INDEX)
    {
        return false;
    }

    /* PSTN权限，网络不可用的时候需要隐藏部分内容（PSTN不需要网络，故需求通过网络是否连接上判断是否开启PSTN） */
    int iLevel = iIndex == PSTN_BROWSE_AUTH_INDEX ?
                 (netIsCableConnected() ? 1 : 0) : (int)feature_UserModeGetCurrType();
    return iLevel >= (strAuth[iIndex] - '0');
}

CDsskeyItemAttr * CDsskeyUIData::GetItemAttr(DssKey_Type eType, DSSKEY_ATTR_ITEM eItem)
{
    CDsskeyInrInfo & dsskeyXmlData = GetDsskeyXmlData(eType);
    CDsskeyItemAttr * pAttr = dsskeyXmlData.Find(eItem);

    if (pAttr == NULL && !IsDefaultData(dsskeyXmlData))
    {
        pAttr = m_DefaultData.Find(eItem);
    }

    return pAttr;
}

vecDsskeySet * CDsskeyUIData::GetDsskeySet(DSSKEY_UI_TYPE eMoudle, bool bEventType /*= false*/)
{
    mapDsskeyList::iterator it = m_mapSet.find(eMoudle);

    if (it != m_mapSet.end())
    {
        DsskeyMoudleSet * pMoudleSet = it->second;
        return bEventType ? &(pMoudleSet->vecDsskeyEvent) : &(pMoudleSet->vecDsskeyList);
    }

    return NULL;
}

bool CDsskeyUIData::FindDsskeyInSet(const DsskeyPageInfo & pageInfo, DssKey_Type eType,
                                    bool bEventType)
{
    vecDsskeySet * pvecList = GetDsskeySet(pageInfo.m_eUIType, bEventType);
    return pvecList != NULL ? Find(*pvecList, eType) : false;
}

bool CDsskeyUIData::GetAllDsskey(const DsskeyPageInfo & pageInfo, VecCandidateValue & vecData,
                                 bool bEventType)
{
    vecDsskeySet * pVecSet = GetDsskeySet(pageInfo.m_eUIType, bEventType);

    if (pVecSet == NULL)
    {
        return false;
    }

    for (vecDsskeySet::iterator it = pVecSet->begin(); it != pVecSet->end(); ++it)
    {
        DssKey_Type eType = (DssKey_Type)(*it);

        if (!IsShowType(eType) || !OnFunctionCheck(eType))
        {
            IF_LOG_TRACE("id=[%d]", eType);
            continue;
        }

        // Exp Switch  不是所有EXP都支持，需要过滤
        if (eType == DT_EXP_SWITCH)
        {
            if (!dsskey_IsSupportModuleType())
            {
                continue;
            }

            if (!(pageInfo.m_iIndex == 0 || pageInfo.m_iIndex == 20)
                    && pageInfo.m_eUIType == DSSKEY_UI_EXPKEY)
            {
                continue;
            }
        }

        // 如果是KeyEvent的第一个则直接显示为KeyEvent
        if (!bEventType && eType == DT_KEYEVENT)
        {
            vecData.push_back(CDsskeyPair(DT_KEYEVENT, TRID_KEY_EVENT));
        }
        else
        {
            vecData.push_back(CDsskeyPair(eType, GetDefaultLabel(eType)));
        }
    }

    return vecData.size() > 0;
}

bool CDsskeyUIData::GetAllDsskeyEx(DSSKEY_UI_TYPE eUIType, vecDsskeySet & vecOut)
{
    return GetAllDsskeyEx(eUIType, GetDsskeySet(eUIType, false), vecOut);
}

bool CDsskeyUIData::GetAllDsskeyEx(DSSKEY_UI_TYPE eUIType, vecDsskeySet * pVecIn,
                                   vecDsskeySet & vecOut)
{
    if (pVecIn == NULL)
    {
        return false;
    }

    for (vecDsskeySet::iterator it = pVecIn->begin(); it != pVecIn->end(); ++it)
    {
        DssKey_Type eType = (DssKey_Type)(*it);

        // keyevent不支持，switch上面已经处理了
        if (eType == DT_KEYEVENT)
        {
            GetAllDsskeyEx(eUIType, GetDsskeySet(eUIType, true), vecOut);
            continue;
        }

        if (eType == DT_EXP_SWITCH || !IsShowType(eType) || !OnFunctionCheck(eType))
        {
            continue;
        }

        if (Find(vecOut, eType))
        {
            continue;
        }

        vecOut.push_back(eType);
    }

    return vecOut.size() > 0;
}

bool CDsskeyUIData::LogicDataToUIData(const DsskeyPageInfo & pageInfo, DssKeyUIData & dsskeyData,
                                      DssKeyLogicData & dsskeyLogicData)
{
    dsskeyData.m_eType = dsskeyLogicData.eDKType;
    dsskeyData.m_strLabel = dsskeyLogicData.strLabel;
    dsskeyData.m_strShortLabel = dsskeyLogicData.strShortLabel;
    dsskeyData.m_strValue = dsskeyLogicData.strValue;
    dsskeyData.m_strExtension = dsskeyLogicData.strExtension;
    dsskeyData.m_iLineIndex = dsskeyLogicData.iLineIndex;
    DssKey_Type eType = dsskeyData.m_eType;

    // 不可见的Dsskey默认加载为NA
    if (!IsSupportType(pageInfo, eType))
    {
        DSSKEY_WARN("dsskey hide [%d]", eType);
        dsskeyData.Reset();
        return false;
    }

    return true;
}

bool CDsskeyUIData::UIDataToLogicData(const DsskeyPageInfo & pageInfo, DssKeyUIData & dsskeyData,
                                      DssKeyLogicData & dsskeyLogicData)
{
    // 只读属性的Dsskey只有Label可编辑
    dsskeyLogicData.strLabel = dsskeyData.m_strLabel;
    dsskeyLogicData.strShortLabel = dsskeyData.m_strShortLabel;
    dsskeyLogicData.eDKType = dsskeyData.m_eType;
    dsskeyLogicData.strExtension = dsskeyData.m_strExtension;
    dsskeyLogicData.iLineIndex = dsskeyData.m_iLineIndex;
    dsskeyLogicData.iLineID = dsskeyData.m_iLineIndex;
    dsskeyLogicData.strValue = dsskeyData.m_strValue;
    bool bRet = true;

    switch (dsskeyData.m_eType)
    {
    case DT_LINE:
        {
            // 账号不需要写server 和username
            dsskeyLogicData.strExtension.clear();
            dsskeyLogicData.strValue = commonAPI_FormatString("%d", dsskeyData.m_iIndex);
            dsskeyLogicData.eLockType = (DssKeyLockType)dsskeyData.m_iIndex;
        }
        break;

    case DT_LOCALGROUP:
    case DT_XMLGROUP:
    case DT_BSGROUP:

    // 需要特殊处理, 黑白屏特有的类型
    case DT_IDLE_CALLLIST:
    case DT_NETWORKCALLLOG_SINGLE:
        {
            int iIndex = 0;
            // 取得是实际下标
            VecCandidateValue & vecValue = dsskeyData.m_vecData;
            int iSize = vecValue.size();

            for (int i = 0; i < iSize; ++i)
            {
                if (vecValue[i].iIndex == dsskeyData.m_iIndex)
                {
                    iIndex = i;
                    break;
                }
            }

            // index取自line
            dsskeyLogicData.iLineIndex = iIndex;
            dsskeyLogicData.iLineID = iIndex;
        }
        break;

    case DT_BTLINE:
        {
            // http://10.2.1.199/Bug.php?BugID=78103
            dsskeyLogicData.iLineIndex = dsskey_GetDsskeyLine(pageInfo.m_iDsskeyID);
        }
        break;
#if IF_BUG_32812

    case DT_BLFLIST:
        {
            dsskeyLogicData.strValue.clear();
            int iIndex = dsskeyData.m_iIndex;
            VecCandidateValue & vecValue = dsskeyData.m_vecData;

            if (iIndex >= 0 && iIndex < vecValue.size())
            {
                dsskeyLogicData.strValue = vecValue[iIndex].strValue;
            }
        }
        break;
#endif

    case DT_KEYEVENT:
        {
            dsskeyLogicData.eDKType = DT_NA;
        }
        break;

    default:
        bRet = false;
        break;
    }

    return true;
}

bool CDsskeyUIData::GetData(const DsskeyPageInfo & pageInfo, DssKeyUIData & dsskeyData)
{
    DssKeyLogicData dsskeyLogicData;
    bool bRet = dsskey_GetData(pageInfo.m_iDsskeyID, dsskeyLogicData);
    bRet &= LogicDataToUIData(pageInfo, dsskeyData, dsskeyLogicData);
    bRet |= GetExtraData(pageInfo, dsskeyData, dsskeyLogicData);
    IF_LOG_TRACE("id=[%d] type=[%d] line=[%d] index=[%d] label=[%s] short label=[%s] ext=[%s] value=[%s]",
                 pageInfo.m_iDsskeyID, dsskeyData.m_eType, dsskeyData.m_iLineIndex, dsskeyData.m_iIndex,
                 dsskeyData.m_strLabel.c_str(), dsskeyData.m_strShortLabel.c_str(),
                 dsskeyData.m_strExtension.c_str(), dsskeyData.m_strValue.c_str());
    return bRet;
}

bool CDsskeyUIData::GetExtraData(const DsskeyPageInfo & pageInfo, DssKeyUIData & dsskeyData,
                                 const DssKeyLogicData & dsskeyLogicData)
{
    VecCandidateValue & vecValue = dsskeyData.m_vecData;
    vecValue.clear();
    bool bRet = true;

    switch (dsskeyData.m_eType)
    {
    case DT_LINE:
        {
            // 优先用配置保存的类型
            if (dsskeyData.m_strLabel.empty())
            {
                dsskeyData.m_strLabel = acc_GetAccountShowText(dsskeyData.m_iLineIndex);
            }

            // 重设默认账号
            if (pageInfo.m_eUIType == DSSKEY_UI_LINEKEY
                    && dsskeyData.m_iLineIndex == AUTO_SELECT_LINE_ID)
            {
                dsskeyData.m_iLineIndex = pageInfo.m_iIndex < acc_AccountNum() ? pageInfo.m_iIndex : 0;
            }

            dsskeyData.m_strExtension = acc_GetServerName(dsskeyData.m_iLineIndex);
            dsskeyData.m_strValue = acc_GetUsername(dsskeyData.m_iLineIndex);
            dsskeyData.m_iIndex = dsskeyLogicData.eLockType;
#if !FEATURE_T2X_LINE_FLOAT

            if (dsskeyData.m_iIndex == DSS_KEY_LOCK_TYPE_FLOAT)
            {
                dsskeyData.m_iIndex = DSS_KEY_LOCK_TYPE_LOCK;
            }

#endif
            vecValue.push_back(CDsskeyPair(DSS_KEY_LOCK_TYPE_DEFAULT, TRID_DEFAULT));
#if FEATURE_T2X_LINE_FLOAT
            vecValue.push_back(CDsskeyPair(DSS_KEY_LOCK_TYPE_FLOAT, TRID_DEFAULT));
#endif

            if (pageInfo.m_eUIType == DSSKEY_UI_LINEKEY
                    && dsskeyData.m_iLineIndex != AUTO_SELECT_LINE_ID)
            {
                if (dsskeyData.m_iIndex == DSS_KEY_LOCK_TYPE_LOCK
                        || pageInfo.m_iIndex < m_iLineKeyPerPage)
                {
                    vecValue.push_back(CDsskeyPair(DSS_KEY_LOCK_TYPE_LOCK, TRID_LOCK));
                }
            }
        }
        break;

    case DT_LOCALGROUP:
    case DT_XMLGROUP:
    case DT_BSGROUP:

    // 黑白屏特有的类型
    case DT_NETWORKCALLLOG_SINGLE:
        {
            YLVector<int> vecGroupId;
            DIR_TYPE eDirType = GetDIRType(dsskeyData.m_eType);

            if (eDirType == BSFT_DIR)
            {
                DM_GetDsskeyBsftGroupList(vecGroupId);
            }
            else
            {
                Dir_GetConfigMenuIdList(vecGroupId, eDirType);
            }

            yl::string strDisplayName;

            for (YLVector<int>::iterator it = vecGroupId.begin(); it != vecGroupId.end(); ++it)
            {
                // 自动更新关闭，不需要显示custom下面的组
                if (!BsftDir_UpdateEnable()
                        && eDirType == BSFT_DIR
                        && Dir_GetParentIdById(*it, BSFT_DIR) == kszCustomId)
                {
                    continue;
                }

                GetGroupDisplayName(eDirType, *it, strDisplayName);
                vecValue.push_back(CDsskeyPair(*it, strDisplayName));
            }

            int iIndex = dsskeyLogicData.iLineIndex;
            iIndex = (iIndex >= 0 && iIndex < vecValue.size()) ? vecValue[iIndex].iIndex : 0;
            dsskeyData.m_iIndex = iIndex;
        }
        break;

    case DT_IDLE_CALLLIST:
        {
            int iIndex = 0;
            vecValue.push_back(CDsskeyPair(iIndex++, TRID_LOCAL_HISTORY));

            if (BsftLog_IsEnable())
            {
                vecValue.push_back(CDsskeyPair(iIndex++, TRID_NETWORK_CALLLOG));
            }

#if IF_FEATURE_METASWITCH_CALLLOG

            //添加 mtsw历史记录 dsskey
            if (MTSWLog_IsEnable())
            {
                vecValue.push_back(CDsskeyPair(iIndex++, TRID_MTSW_CALLLOG));
            }

#endif // IF_FEATURE_METASWITCH_CALLLOG
            dsskeyData.m_iIndex = dsskeyLogicData.iLineIndex;
        }
        break;
#if IF_BUG_32812

    case DT_BLFLIST:
        {
            if (dsskeyData.m_iLineIndex == AUTO_SELECT_LINE_ID)
            {
                dsskeyData.m_iLineIndex = 0;
            }

            // 重置序号
            dsskeyData.m_iIndex = 0;
            YLList<yl::string> listMonitor;
            blf_GetBLFListMonitorList(dsskeyData.m_iLineIndex, listMonitor);
            YLList<yl::string>::iterator iter = listMonitor.begin();

            for (int iIndex = 0; iter != listMonitor.end(); ++iter, ++iIndex)
            {
                yl::string & strValue = *iter;

                if (strValue == dsskeyData.m_strValue)
                {
                    dsskeyData.m_iIndex = iIndex;
                }

                vecValue.push_back(CDsskeyPair(iIndex, strValue));
            }
        }
        break;
#endif

    default:
        bRet = false;
        break;
    }

    // 没有分组时使用空的替代
    if (bRet && vecValue.size() == 0)
    {
        vecValue.push_back(CDsskeyPair(0, TRID_NA));
    }

    // 不存在为-1的情况
    if (dsskeyData.m_iLineIndex == AUTO_SELECT_LINE_ID)
    {
        dsskeyData.m_iLineIndex = 0;
    }

    SetDefaultValue(dsskeyData.m_strLabel, dsskeyData.m_eType, DSSKEY_ATTR_ITEM_LABEL);
    SetDefaultValue(dsskeyData.m_strValue, dsskeyData.m_eType, DSSKEY_ATTR_ITEM_VALUE);
    SetDefaultValue(dsskeyData.m_strExtension, dsskeyData.m_eType, DSSKEY_ATTR_ITEM_EXT);
    return vecValue.size() > 0;
}

bool CDsskeyUIData::SetDefaultValue(yl::string & strValue, DssKey_Type eType,
                                    DSSKEY_ATTR_ITEM eItem)
{
    if (strValue.empty())
    {
        CDsskeyItemAttr * pAttr = GetItemAttr(eType, eItem);

        if (pAttr != NULL)
        {
            strValue = pAttr->GetDefault();
            return true;
        }
    }

    return false;
}

bool CDsskeyUIData::SaveData(const DsskeyPageInfo & pageInfo, DssKeyUIData & dsskeyData)
{
    IF_LOG_TRACE("id=[%d] type=[%d] line=[%d] index=[%d] label=[%s] shortlabel=[%s] ext=[%s] value=[%s]",
                 pageInfo.m_iDsskeyID, dsskeyData.m_eType, dsskeyData.m_iLineIndex, dsskeyData.m_iIndex,
                 dsskeyData.m_strLabel.c_str(), dsskeyData.m_strShortLabel.c_str(),
                 dsskeyData.m_strExtension.c_str(), dsskeyData.m_strValue.c_str());
    DssKeyLogicData dsskeyLogicData, dsskeyOldData;
    dsskey_GetData(pageInfo.m_iDsskeyID, dsskeyLogicData);
    dsskeyOldData = dsskeyLogicData;
    UIDataToLogicData(pageInfo, dsskeyData, dsskeyLogicData);
    bool bRet = dsskey_SetDsskeyData(pageInfo.m_iDsskeyID, dsskeyLogicData);
    // 检查Lock状态
    bRet |= CheckLineLockStatus(pageInfo, dsskeyLogicData, dsskeyOldData);
    return bRet;
}

void CDsskeyUIData::ResetDefaultDsskeyData(int iDsskeyID, DssKey_Type eType, int iFlag)
{
    IF_TimeTrace;
    DssKeyLogicData dsskeyLogicData;
    dsskey_GetData(iDsskeyID, dsskeyLogicData);
    int iIndex = dsskey_GetKeyIndex(iDsskeyID);
    DSSKEY_MODULE_TYPE eModule = dsskey_GetModuleType(iDsskeyID);

    if (GetDefaultDsskeyData(eType, eModule, iIndex, dsskeyLogicData))
    {
        DSSKEY_WARN("module=[%d] index=[%d] type=[%d]->[%d]", eModule, iIndex, eType,
                    dsskeyLogicData.eDKType);
        bool bLineType = dsskeyLogicData.eDKType == DT_LINE;
        if (bLineType)
        {
            iFlag &= ~DSSKEY_SET_REFRESH;
        }

        dsskey_SetDsskeyData(iDsskeyID, dsskeyLogicData, iFlag);

        if (bLineType)
        {
            dsskey_UpdateLineStatus(iDsskeyID);
            dsskey_refreshDSSKeyUI(iDsskeyID);
        }
    }
}

bool CDsskeyUIData::GetDefaultDsskeyData(DssKey_Type eType, DSSKEY_MODULE_TYPE eModule, int iIndex,
        DssKeyLogicData & logicData)
{
    // 默认重置为指定类型
    logicData.iLineIndex = 0;
    DssKey_Type eTmpType = (DssKey_Type)(GetDsskeyXmlData(eType).iRedirectType);
    if (eTmpType <= DT_NA)
    {
        // 还原为Factory值
        eTmpType = dsskey_GetDsskeyType(logicData.iDssKeyID, DSS_ATTR_FACTORY);
        if (eTmpType < DT_NA
                || eTmpType >= DT_LAST)
        {
            eTmpType = DT_NA;
        }

        logicData.iLineIndex = dsskey_GetDsskeyLine(logicData.iDssKeyID, DSS_ATTR_FACTORY);
        logicData.strLabel = dsskey_GetDsskeyLabel(logicData.iDssKeyID, DSS_ATTR_FACTORY);
        logicData.strValue = dsskey_GetDsskeyValue(logicData.iDssKeyID, DSS_ATTR_FACTORY);
        logicData.strExtension = dsskey_GetDsskeyExtension(logicData.iDssKeyID, DSS_ATTR_FACTORY);
    }

    // 新的值也要经得起校验
    if (eTmpType > DT_NA)
    {
        if (eTmpType == eType || !OnFunctionCheck(eTmpType))
        {
            DSSKEY_WARN("new type not allowed [%d][%d]", eType, eTmpType);
            eTmpType = DT_NA;
            logicData.strValue.clear();
            logicData.strLabel.clear();
            logicData.strExtension.clear();
        }
    }

    logicData.eDKType = eTmpType;
    logicData.eLockType = DSS_KEY_LOCK_TYPE_DEFAULT;
    return logicData.eDKType != eType || eType == DT_NA;
}

bool CDsskeyUIData::CheckLineLockStatus(const DsskeyPageInfo & pageInfo,
                                        const DssKeyLogicData & dsskeyLogicData, const DssKeyLogicData & dsskeyOldData)
{
    if (pageInfo.m_eUIType != DSSKEY_UI_LINEKEY)
    {
        return false;
    }

    if (pageInfo.m_iIndex < m_iLineKeyPerPage)
    {
        int iDsskeyPage2ID = dsskey_GetDsskeyID(DMT_LINEKEY_MODULE, 0,
                                                pageInfo.m_iIndex + m_iLineKeyPerPage);
        int iDsskeyPage3ID = dsskey_GetDsskeyID(DMT_LINEKEY_MODULE, 0,
                                                pageInfo.m_iIndex + m_iLineKeyPerPage * 2);

        // 如果是第一页的linekey且类型为line且locktype为lock，那么将数据设置到下一页的linekey
        if (dsskeyLogicData.eDKType == DT_LINE
                && dsskeyLogicData.eLockType == DSS_KEY_LOCK_TYPE_LOCK)
        {
            // 锁定第二、三页
            dsskey_SetDsskeyData(iDsskeyPage2ID, dsskeyLogicData);
            dsskey_SetDsskeyData(iDsskeyPage3ID, dsskeyLogicData);
        }
        //如果是第一页由line + lock改为其它类型，那么下两页对应的linekey设置为NA
        else if ((dsskeyOldData.eDKType == DT_LINE && dsskeyOldData.eLockType == DSS_KEY_LOCK_TYPE_LOCK)
                 && !(dsskeyLogicData.eDKType == DT_LINE && dsskeyLogicData.eLockType == DSS_KEY_LOCK_TYPE_LOCK))
        {
            // 重设第二、三页
            ResetDefaultDsskeyData(iDsskeyPage2ID, DT_NA, DSSKEY_SET_NORMAL);
            ResetDefaultDsskeyData(iDsskeyPage3ID, DT_NA, DSSKEY_SET_NORMAL);
        }
    }

    return pageInfo.m_iIndex < m_iLineKeyPerPage;
}

bool CDsskeyUIData::IsDataValid(const DsskeyPageInfo & pageInfo, DssKeyUIData & dsskeyData,
                                CDsskeyPair & errorTips)
{
    DSSKEY_READONLY_MASK eMask = IsDsskeyReadOnly(pageInfo, dsskeyData);

    if (eMask == READONLY_ALL_ITEM)
    {
        errorTips.strValue = TRID_PAGE_READ_ONLY;
        return false;
    }

    int iDsskeyID = pageInfo.m_iDsskeyID;
    CDsskeyInrInfo & dsskeyXmlData = GetDsskeyXmlData(dsskeyData.m_eType);
    bool bCheckType = true;

    // Line可以且只可以修改Lable
    if (dsskeyData.m_eType == DT_LINE)
    {
        DssKeyLogicData dsskeyLogicData;
        dsskey_GetData(iDsskeyID, dsskeyLogicData);

        if (dsskeyLogicData.eDKType == dsskeyData.m_eType
                && dsskeyLogicData.iLineIndex == dsskeyData.m_iLineIndex
                && dsskeyData.m_iIndex == dsskeyLogicData.eLockType)
        {
            yl::string strAcclabel = acc_GetLabel(dsskeyLogicData.iLineIndex);
            DSSKEY_INFO("DT_LINE label key=[%d] line=[%d] label=[%s] [%s] [%s]",
                        iDsskeyID, dsskeyData.m_iLineIndex, dsskeyLogicData.strLabel.c_str(), dsskeyData.m_strLabel.c_str(),
                        strAcclabel.c_str());

            if ((strAcclabel == dsskeyLogicData.strLabel || dsskeyLogicData.strLabel.empty())
                    && strAcclabel == dsskeyData.m_strLabel)
            {}
            else if (dsskeyLogicData.strLabel != dsskeyData.m_strLabel)
            {
                bCheckType = false;
            }
        }
    }

    if (bCheckType && !m_FunLogic.OnSaveAction(dsskeyXmlData.strFunID))
    {
        errorTips.iIndex = DSSKEY_ATTR_ITEM_TYPE;
        errorTips.strValue = m_FunLogic.GetErrorString();
        IF_LOG_TRACE("OnSaveAction failed [%s]", dsskeyXmlData.strFunID.c_str());
        return false;
    }

    // 自定义校验函数
    if (m_FunCheckFilter != NULL)
    {
        if (!(*m_FunCheckFilter)(pageInfo.m_iDsskeyID, dsskeyData, errorTips))
        {
            return false;
        }
    }

    // 进行动态函数的校验
    VecAttrItem & vecInfo = dsskeyXmlData.vecInrAttr;
    yl::string strValue;
    // 暂时只校验这三项内容
    const int iAttrSize = 3;
    DSSKEY_ATTR_ITEM s_arr_attr[iAttrSize] = {DSSKEY_ATTR_ITEM_LABEL, DSSKEY_ATTR_ITEM_VALUE, DSSKEY_ATTR_ITEM_EXT};

    for (int i = 0; i < iAttrSize; ++i)
    {
        CDsskeyItemAttr * pAttr = GetItemAttr(dsskeyData.m_eType, s_arr_attr[i]);

        if (pAttr == NULL || !pAttr->IsVisible() || pAttr->IsReadOnly())
        {
            continue;
        }

        switch (pAttr->iAttrType)
        {
        case DSSKEY_ATTR_ITEM_LABEL:
            {
                strValue = dsskeyData.m_strLabel;
            }
            break;

        case DSSKEY_ATTR_ITEM_VALUE:
            {
                strValue = dsskeyData.m_strValue;
            }
            break;

        case DSSKEY_ATTR_ITEM_EXT:
            {
                strValue = dsskeyData.m_strExtension;
            }
            break;

        default:
            continue;
        }

        bool bRet = m_FunLogic.OnSaveAction(pAttr->GetRegexp(), strValue);
        IF_LOG_TRACE("type=[%d] reg=[%s] value=[%s] ret=[%d]",
                     pAttr->iAttrType, pAttr->GetRegexp().c_str(), strValue.c_str(), bRet);

        // Save操作校验失败
        if (!bRet)
        {
            errorTips.iIndex = pAttr->iAttrType;
            errorTips.strValue = m_FunLogic.GetErrorString();
            DSSKEY_INFO("regrex failed [%s][%s]", pAttr->GetRegexp().c_str(), strValue.c_str());
            return false;
        }
    }

    return true;
}

DSSKEY_READONLY_MASK CDsskeyUIData::IsDsskeyReadOnly(const DsskeyPageInfo & pageInfo,
        const DssKeyUIData & dsskeyData)
{
    IF_LOG_TRACE("id=[%d] ui=[%d] type=[%d] line=[%d][%d] edit=[%d][%d][%d]",
                 pageInfo.m_iDsskeyID, pageInfo.m_eUIType, dsskeyData.m_eType, dsskeyData.m_iLineIndex,
                 dsskeyData.m_iIndex, IsEditable(dsskeyData.m_eType), m_iLineKeyPerPage, pageInfo.m_iIndex);

    // 只读翻页键, 第21个按键
    if (DT_EXP_SWITCH == dsskeyData.m_eType
            && DSSKEY_UI_EXPKEY == pageInfo.m_eUIType
            && 20 == pageInfo.m_iIndex)
    {
        // 翻页键Label是可读写的
        if (dsskey_IsExpSupportSwitchKey(GetModuleType(pageInfo.m_iDsskeyID)))
        {
            return READONLY_EXCEPT_LABEL;
        }
    }

    // 第二、三页被锁的Linekey
    if (DT_LINE == dsskeyData.m_eType
            && DSS_KEY_LOCK_TYPE_LOCK == dsskeyData.m_iIndex
            && pageInfo.m_iIndex >= m_iLineKeyPerPage)
    {
        return READONLY_ALL_ITEM;
    }

    return IsEditable(dsskeyData.m_eType) ? READONLY_NONE : READONLY_ALL_ITEM;
}

yl::string CDsskeyUIData::GetContactLabel(const DssKeyLogicData & dsskeyLogicData)
{
    switch (dsskeyLogicData.eDKType)
    {
    case DT_LOCALGROUP:
    case DT_XMLGROUP:
    case DT_BSGROUP:
        {
            int iIndex = dsskeyLogicData.iLineIndex;
            YLVector<int> vecGroupId;
            DIR_TYPE eDirType = GetDIRType(dsskeyLogicData.eDKType);

            if (eDirType == BSFT_DIR)
            {
                DM_GetDsskeyBsftGroupList(vecGroupId);
            }
            else
            {
                Dir_GetConfigMenuIdList(vecGroupId, eDirType);
            }

            if (iIndex >= 0 && iIndex < vecGroupId.size())
            {
                yl::string strLabel;
                GetGroupDisplayName(eDirType, vecGroupId[iIndex], strLabel);
                return strLabel;
            }
        }
        break;

    default:
        break;
    }

    return yl::string();
}

DssKey_Type CDsskeyUIData::GetDsskeyTypeByURI(const yl::string & strURI)
{
    mapDsskeyIncAttr::iterator it = m_mapIncAttr.begin();

    for (; it != m_mapIncAttr.end(); ++it)
    {
        if (it->second->strURI == strURI)
        {
            return (DssKey_Type)it->first;
        }
    }

    return DT_NONE;
}

int CDsskeyUIData::GetKeyPerPage()
{
    if (dsskey_IsAverageDistribution(DMT_LINEKEY_MODULE))
    {
        int iNum = dsskey_GetMaxDsskeyNumPerPage();
        return iNum > 0 ? iNum - 1 : 0;
    }

    return GetLinekeyNumber();
}

void CDsskeyUIData::GetGroupDisplayName(DIR_TYPE eDirType, int nGroupId,
                                        yl::string & strDisplayName)
{
    return DM_GetGroupDisplayName(eDirType, nGroupId, strDisplayName);
}

DSSKEY_UI_TYPE CDsskeyUIData::MapMoudle2UI(DSSKEY_MODULE_TYPE eMoudle)
{
    switch (eMoudle)
    {
    case DMT_PROGRAMEKEY_MODULE:
        return DSSKEY_UI_SOFTKEY;
    case DMT_MEMKEY_MODULE:
        return DSSKEY_UI_MEMORYKEY;
    case DMT_LINEKEY_MODULE:
        return DSSKEY_UI_LINEKEY;
    default:
        if (IsEXPModule(eMoudle))
        {
            return DSSKEY_UI_EXPKEY;
        }
        break;
    }

    return DSSKEY_UI_NONE;
}

DSSKEY_MODULE_TYPE CDsskeyUIData::MapMoudle2Logic(DSSKEY_UI_TYPE eMoudle)
{
    DSSKEY_MODULE_TYPE moduleType = DMT_NON;

    switch (eMoudle)
    {
    case DSSKEY_UI_SOFTKEY:
        moduleType = DMT_PROGRAMEKEY_MODULE;
        break;

    case DSSKEY_UI_LINEKEY:
        moduleType = DMT_LINEKEY_MODULE;
        break;

    case DSSKEY_UI_MEMORYKEY:
        moduleType = DMT_MEMKEY_MODULE;
        break;

    case DSSKEY_UI_EXPKEY:
        moduleType = (DSSKEY_MODULE_TYPE)exp_GetEXPType();
        break;

    default:
        break;
    }

    return moduleType;
}

bool CDsskeyUIData::CorrectDsskeyType(DssKeyLogicData & logicData, DSSKEY_MODULE_TYPE eModuleType,
                                      int iKeyIndex)
{
    IF_TimeTrace;
    DssKey_Type eType = logicData.eDKType;
    bool bRet = false;

    /* 非开关控制，不需要记录 */
    if (eType == DT_NONE)
    {
        logicData.eDKType = DT_NA;
    }
    // EXP39第21个按键一定是switch
    else if (eType != DT_EXP_SWITCH
             && 20 == iKeyIndex
             && dsskey_IsExpSupportSwitchKey(eModuleType))
    {
        logicData.eDKType = DT_EXP_SWITCH;
    }
    // EXP40 不需要Switch
    else if (eType == DT_EXP_SWITCH
             && !dsskey_IsExpSupportSwitchKey(eModuleType))
    {
        logicData.eDKType = DT_NA;
    }
    // 旧的BLA类型直接替换成SIPTrunk
    else if (eType == DT_BLA)
    {
        logicData.eDKType = DT_LINE;
    }
    else if (eType == DT_KEYEVENT)
    {
        logicData.eDKType = DT_NA;
    }
    else
    {
        if (OnFunctionCheck(eType))
        {
            bRet = false;
        }
        // 还原为默认类型
        else
        {
            bRet = GetDefaultDsskeyData(eType, eModuleType, iKeyIndex, logicData);
        }
    }

    if (eType != logicData.eDKType)
    {
        DSSKEY_INFO("CorrectDsskeyType ret=[%d] type=[%d]->[%d] modle=[%d] index=[%d]", bRet, eType,
                    logicData.eDKType, eModuleType, iKeyIndex);
    }

    return bRet;
}

bool CDsskeyUIData::CheckVisible(DssKeyLogicData & logicData)
{
    // 只检测三级权限
    DssKey_Type & eType = logicData.eDKType;

    if (IsShowType(eType))
    {
        return true;
    }

    DSSKEY_INFO("CheckVisible hide type=[%d]", eType);
    eType = DT_NA;
    logicData.strLabel.clear();
    return false;
}

void CDsskeyUIData::ResetNotAvailDsskey()
{
    IF_TimeTrace;
    YLList<int> listOutput;
    dsskey_getAllDsskeys(listOutput);

    for (YLList<int>::iterator it = listOutput.begin(); it != listOutput.end(); ++it)
    {
        int iDsskeyID = *it;
        DssKey_Type eType = dsskey_GetDsskeyType(iDsskeyID);

        // 已经是默认类型，不处理
        if (eType <= DT_NA || eType >= DT_LAST)
        {
            continue;
        }

        // 不支持的话直接重置，重置优先级比隐藏优先级高
        if (!OnFunctionCheck(eType))
        {
            int iFlag = DSSKEY_SET_ONLY | DSSKEY_SET_NOTIFY | DSSKEY_SET_REFRESH;
            ResetDefaultDsskeyData(iDsskeyID, eType, iFlag);
        }
        else if (!IsShowType(eType))
        {
            // 被隐藏的Dsskey也需要刷新
            dsskey_refreshDSSKeyUI(iDsskeyID);
        }
    }
}

bool CDsskeyUIData::OnFunctionCheck(DssKey_Type eType)
{
    CDsskeyInrInfo & dsskeyXmlData = GetDsskeyXmlData(eType);
    return m_FunLogic.OnLoadAction(dsskeyXmlData.strFunID);
}

static inline void PutXmlItemWithID(pugi::xml_node & nodeRoot, int iAttr, LPCSTR lpValue)
{
    if (lpValue != NULL)
    {
        using namespace pugi;

        xml_node nodeSubItem = nodeRoot.append_child(XML_SUB_NODE_ITEM);
        nodeSubItem.append_attribute(XML_ATTR_ID, iAttr);
        nodeSubItem.append_child(node_pcdata).set_value(lpValue);
    }
}

bool CDsskeyUIData::SaveFile(LPCSTR lpFileName)
{
    if (lpFileName == NULL)
    {
        return false;
    }

    DSSKEY_INFO("save file %s", lpFileName);

    using namespace pugi;
    xml_document doc;
    vecDsskeySet vec;
    char buffer[64] = { 0 };
    yl::string strOut;

    // NO.1 写dsskey排布顺序
    xml_node nodeRoot = doc.append_child(XML_NODE_SEQUENCE);

    for (int i = DSSKEY_UI_LINEKEY; i <= DSSKEY_UI_SOFTKEY; ++i)
    {
        DSSKEY_UI_TYPE eUIType = (DSSKEY_UI_TYPE)i;

        vecDsskeySet vecModule;
        if (!GetAllDsskeyEx(eUIType, vecModule))
        {
            continue;
        }

        for (vecDsskeySet::iterator it = vecModule.begin(); it != vecModule.end(); ++it)
        {
            vec.push_back(*it);
            sprintf(buffer, "%d,", *it);
            strOut += buffer;
        }

        if (!strOut.empty())
        {
            strOut.erase(strOut.size() - 1);
        }

        xml_node nodeItem = nodeRoot.append_child(XML_ITEM_KEY_LIST);
        nodeItem.append_attribute(XML_ATTR_TYPE, i);
        nodeItem.append_attribute(XML_ATTR_RANG, strOut.c_str());

        // switch 单独处理
        if (eUIType == DSSKEY_UI_EXPKEY)
        {
            bool bSuportExp = false;
            for (int j = DMT_EXP20_MODULE; j <= DMT_EXPCM69_MODULE; ++j)
            {
                if (DMT_EHS == j)
                {
                    continue;
                }

                DSSKEY_MODULE_TYPE eModule = (DSSKEY_MODULE_TYPE)j;
                if (dsskey_IsExpSupportSwitchKey(eModule)
                        && dsskey_IsSupportModuleType(eModule))
                {
                    bSuportExp = true;
                    break;
                }
            }

            if (bSuportExp)
            {
                vec.push_back(DT_EXP_SWITCH);
                sprintf(buffer, "%d,", DT_EXP_SWITCH);
                strOut = buffer + strOut;
                // 0, 20 需要特殊处理
                PutXmlItemWithID(nodeItem, 0, strOut.c_str());
                sprintf(buffer, "%d", DT_EXP_SWITCH);
                PutXmlItemWithID(nodeItem, 20, buffer);
            }
        }

        strOut.clear();
    }

#if 0
    // NO.2 写联系人、账号、blflist等信息
    nodeRoot = doc.append_child("directory");
    YLVector<BaseDirDataRecord *> output;
    for (int i = BASE_DIR + 1; i <= CLOUD_DIR; ++i)
    {
        if (!Dir_GetGroupList(output, DIR_TYPE(i)))
        {
            continue;
        }

        xml_node nodeItem = nodeRoot.append_child(XML_ATTR_CLASS);
        nodeItem.append_attribute(XML_ATTR_TYPE).set_value(i);

        YLVector<BaseDirDataRecord *>::iterator it = output.begin();
        for (; it != output.end(); ++it)
        {
            BaseDirDataRecord * pRecord = *it;
            if (pRecord == NULL)
            {
                continue;
            }

            const yl::string & strDisplayName = pRecord->GetDisplayName();
            if (i == BSFT_DIR && kszCustomId == pRecord->m_nId && strDisplayName.empty())
            {
                PutXmlItemWithID(nodeItem, pRecord->m_nId, TRID_CUSTOM_CONTACT);
            }
            else
            {
                PutXmlItemWithID(nodeItem, pRecord->m_nId, strDisplayName.c_str());
            }
        }
    }

    nodeRoot = doc.append_child("history");
    int iIndex = 0;
    PutXmlItemWithID(nodeRoot, iIndex++, TRID_LOCAL_HISTORY);

    if (BsftLog_IsEnable())
    {
        PutXmlItemWithID(nodeRoot, iIndex++, TRID_NETWORK_CALLLOG);
    }
#if IF_FEATURE_METASWITCH_CALLLOG
    //添加 mtsw历史记录 dsskey
    if (MTSWLog_IsEnable())
    {
        PutXmlItemWithID(nodeRoot, iIndex++, TRID_MTSW_CALLLOG);
    }
#endif // IF_FEATURE_METASWITCH_CALLLOG

    nodeRoot = doc.append_child("lock");
    PutXmlItemWithID(nodeRoot, DSS_KEY_LOCK_TYPE_DEFAULT, TRID_DEFAULT);
#if FEATURE_T2X_LINE_FLOAT
    PutXmlItemWithID(nodeRoot, DSS_KEY_LOCK_TYPE_FLOAT, TRID_FLOAT);
#endif
    PutXmlItemWithID(nodeRoot, DSS_KEY_LOCK_TYPE_LOCK, TRID_LOCK);

    nodeRoot = doc.append_child(XML_NODE_NAME_LINE);
    for (int i = 0; i < acc_AccountNum(); ++i)
    {
        sprintf(buffer, "Line %d", i + 1);
        PutXmlItemWithID(nodeRoot, i, buffer);
    }
//#endif

//#if IF_BUG_32812
    nodeRoot = doc.append_child("blflist");
    for (int i = 0; i < acc_AccountNum(); ++i)
    {
        YLList<yl::string> listMonitor;
        if (!blf_GetBLFListMonitorList(i, listMonitor))
        {
            continue;
        }

        xml_node nodeItem = nodeRoot.append_child(XML_NODE_NAME_LINE);
        nodeItem.append_attribute(XML_ATTR_ID).set_value(i);

        YLList<yl::string>::iterator it = listMonitor.begin();
        for (iIndex = 0; it != listMonitor.end(); ++it)
        {
            PutXmlItemWithID(nodeItem, iIndex++, (*it).c_str());
        }
    }
#endif

    mapItem s_info_arr[] =
    {
        { DSSKEY_ATTR_ITEM_LINE,            XML_NODE_NAME_LINE  },
        { DSSKEY_ATTR_ITEM_LABEL,           XML_NODE_NAME_LABEL },
        { DSSKEY_ATTR_ITEM_VALUE,           XML_NODE_NAME_VALUE },
        { DSSKEY_ATTR_ITEM_EXT,             XML_NODE_NAME_EXTENSION },
        { DSSKEY_ATTR_ITEM_INDEX,           XML_NODE_NAME_LINE  },
        { DSSKEY_ATTR_ITEM_LINE_VALUE,      XML_NODE_NAME_VALUE },
    };

    // 可读性考虑，排序，去除重复
    Sort(vec, yl::less<int>());
    vecDsskeySet::iterator it = yl::unique(vec.begin(), vec.end());
    while (it != vec.end())
    {
        it = vec.erase(it);
    }

    // NO.3 写单个dsskey属性
    nodeRoot = doc.append_child(XML_NODE_ENUMERATION);
    for (vecDsskeySet::iterator it = vec.begin(); it != vec.end(); ++it)
    {
        DssKey_Type eType = (DssKey_Type)(*it);
        xml_node nodeItem = nodeRoot.append_child(XML_SUB_NODE_ITEM);
        nodeItem.append_attribute(XML_ATTR_TYPE, eType);
        strOut = GetDefaultLabel(eType);
        nodeItem.append_attribute(XML_ATTR_NAME, eType == DT_NA ? TRID_NA : strOut.c_str());

        // 关联的联系人类型
        DIR_TYPE eDirType = GetDIRType(eType);

        if (eDirType != BASE_DIR)
        {
            nodeItem.append_attribute(XML_ATTR_CLASS, eDirType);
        }

        int iSize = sizeof(s_info_arr) / sizeof(s_info_arr[0]);

        for (int i = 0; i < iSize; ++i)
        {
            DSSKEY_ATTR_ITEM eItem = (DSSKEY_ATTR_ITEM)s_info_arr[i].iKey;
            CDsskeyItemAttr * pAttr = GetItemAttr(eType, eItem);

            if (pAttr == NULL)
            {
                continue;
            }

            // 只写可见的
            if (!pAttr->IsVisible() || pAttr->IsReadOnly())
            {
                continue;
            }

            // 密码标志
            xml_node nodeAttr = nodeItem.append_child(s_info_arr[i].strName);

            if (pAttr->GetPasswdFlag())
            {
                nodeAttr.append_attribute(XML_ATTR_PASSWD).set_value(1);
            }

            // 含默认值
            const yl::string & strDefValue = pAttr->GetDefault();

            if (!strDefValue.empty())
            {
                nodeAttr.append_attribute(XML_ATTR_DEFAULT).set_value(strDefValue.c_str());
            }

            // 校验的正则表达式
            const yl::string & strFormate = pAttr->GetRegexp();

            if (!strFormate.empty())
            {
                nodeAttr.append_attribute(XML_ATTR_FORMATE).set_value(strFormate.c_str());
            }
        }
    }

    bool bRet = doc.save_file(lpFileName);
    DSSKEY_INFO("save file finished");
    return bRet;
}
