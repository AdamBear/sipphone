#include "dialplan_inc.h"

CDialRuleController& GetControllerInstance()
{
    static CDialRuleController s_DialRuleControllerController;
    return s_DialRuleControllerController;
}

// 判断指定账号是否规则中要限制的账号
bool CRuleData::IsValidLineId(int iLineID)
{
    YLList<int>::iterator itr = m_listLineId.begin();
    YLList<int>::iterator itEnd = m_listLineId.end();
    if (itr == itEnd)
    {
        //没有为本规则指定账号则所有账号都可用
        return true;
    }

    while (itr != itEnd)
    {
        if (iLineID == *itr)
        {
            return true;
        }

        itr++;
    }
    return false;
}

// 设置账号
void CRuleData::SetLineId(const yl::string& strLineId)
{
    yl::string strTemp = strLineId;
    m_listLineId.clear();
    // 解析账号字符串
    while (true)
    {
        if (strTemp.empty())
        {
            break;
        }

        // 查找逗号
        size_t uPosFound = strTemp.find(",");
        int iTempLineId = 0;
        // 没有找到,直接取字符串
        if (uPosFound == strLineId.npos)
        {
            iTempLineId = atoi(strTemp.c_str());
            if (0 != iTempLineId)
            {
                m_listLineId.push_back(iTempLineId);
            }
            break;
        }
        else
        {
            iTempLineId = atoi(strTemp.substr(0, uPosFound).c_str());
            if (0 != iTempLineId)
            {
                m_listLineId.push_back(iTempLineId);
            }
            strTemp = strTemp.substr(uPosFound + 1);
        }
    }
}

CDialRuleController::CDialRuleController()
{
    m_iMaxLen = 0;
    m_iMinLen = 0;
}

CDialRuleController::~CDialRuleController()
{
    UnLoadDialRule();
}

// 加载语言文件
bool CDialRuleController::LoadDialRule(int iDRType/* = DR_ALL*/)
{
    TALK_INFO("LoadDialRule Type[%d]", iDRType);
    if (DR_PLAN & iDRType)
    {
        LoadDialPlan();
    }

    if (DR_NOW & iDRType)
    {
        LoadDialNow();
    }

    if (DR_AREA & iDRType)
    {
        LoadAreaCode();
    }

    if (DR_BLOCKOUT & iDRType)
    {
        LoadBlockOut();
    }
    return true;
}


// 释放资源
void CDialRuleController::UnLoadDialRule(int iDRType/* = DR_ALL*/)
{
    ReleaseDialRuleByType(iDRType);
}

// 加载替换规则
bool CDialRuleController::LoadDialPlan()
{
    //读取DialPlan规则
    ReleaseDialRuleByType(DR_PLAN);
    // 文件是否完整
    bool bFileOK = false;
    // 获取到的是v73格式，存为v80格式
    bool bSaveFile = false;
    xml_document doc;
    if (doc.load_file(DIALPLAN_PATH))
    {
        //若文件存在，且读取正确
        xml_node nodeRoot = doc.first_child();

        BOOL bParseOK = FALSE;
        if (nodeRoot)
        {
            chString name = nodeRoot.name();
            // http://192.168.1.99/Bug.php?BugID=24565 支持大小写
            bParseOK = ("dialrule" == name.MakeLower());
        }

        if (bParseOK)
        {
            bFileOK = true;
            xml_node nodeChild = nodeRoot.first_child();
            while (nodeChild)
            {
                chString strName = nodeChild.name();
                if ("data" != strName.MakeLower())
                {
                    nodeChild = nodeChild.next_sibling();
                    continue;
                }

                // 取出数据
                CRuleData* pRuleData = new CRuleData();
                pRuleData->m_strPattern = "";
                pRuleData->m_strReplace = "";

                if (nodeChild.attribute("Prefix"))
                {
                    pRuleData->m_strPattern = nodeChild.attribute("Prefix").value();
                }
                else if (nodeChild.attribute("rule"))
                {
                    pRuleData->m_strPattern = nodeChild.attribute("rule").value();
                    bSaveFile = true;
                }

                if (nodeChild.attribute("Replace"))
                {
                    pRuleData->m_strReplace = nodeChild.attribute("Replace").value();
                }
                else if (nodeChild.attribute("replace"))
                {
                    pRuleData->m_strReplace = nodeChild.attribute("replace").value();
                    bSaveFile = true;
                }

                // 字符转换
                TransformString(pRuleData->m_strPattern);
                if (!pRuleData->m_strPattern.empty() && !pRuleData->m_strReplace.empty())
                {
                    // 解析账号
                    yl::string strLine = "";
                    if (nodeChild.attribute("LineID"))
                    {
                        strLine = nodeChild.attribute("LineID").value();
                    }
                    else if (nodeChild.attribute("lines"))
                    {
                        strLine = nodeChild.attribute("lines").value();
                        bSaveFile = true;
                    }
                    pRuleData->SetLineId(strLine);
                    // 加入到链表
                    m_listReplace.push_back(pRuleData);
                }
                else
                {
                    delete pRuleData;
                    pRuleData = NULL;
                }

                // 查找下一规则
                nodeChild = nodeChild.next_sibling();
            }
        }
    }

    if (!bFileOK)
    {
        TALK_WARN("Create new DialPlan xml file");
        // 创建新文件
        xml_document docTmp;
        xml_node nodeRoot = docTmp.append_child("dialrule");
        //保存到文件
        docTmp.save_file(DIALPLAN_PATH);
    }

    if (bSaveFile)
    {
        TALK_WARN("Change DialPlan xml file");

        // 创建新文件
        xml_document docTmp;
        xml_node nodeRoot = docTmp.append_child("dialrule");

        ListData::iterator it = m_listReplace.begin();

        for (; it != m_listReplace.end(); it++)
        {
            CRuleData* pRuleData = *it;

            xml_node nodeChild = nodeRoot.append_child("Data");
            nodeChild.append_attribute("Prefix", pRuleData->m_strPattern.c_str());
            nodeChild.append_attribute("Replace", pRuleData->m_strReplace.c_str());

            yl::string strlistLineId = "";

            YLList<int>::iterator itr = pRuleData->m_listLineId.begin();

            for (; itr != pRuleData->m_listLineId.end(); itr++)
            {
                if (itr == pRuleData->m_listLineId.begin())
                {
                    strlistLineId = commonAPI_FormatString("%d", *itr);
                }
                else
                {
                    strlistLineId = commonAPI_FormatString("%s,%d", strlistLineId.c_str(), *itr);
                }
            }

            TALK_WARN("strlistLineId[%s]", strlistLineId.c_str());

            nodeChild.append_attribute("LineID", strlistLineId.c_str());
        }

        //保存到文件
        docTmp.save_file(DIALPLAN_PATH);
    }

    return true;
}

// 加载立即呼出
bool CDialRuleController::LoadDialNow()
{
    // 清空旧数据
    ReleaseDialRuleByType(DR_NOW);
    // 文件是否完整
    bool bFileOK = false;
    xml_document doc;
    if (doc.load_file(DIALNOW_PATH))
    {
        //若文件存在，且读取正确
        xml_node nodeRoot = doc.first_child();

        BOOL bParseOK = FALSE;
        if (nodeRoot)
        {
            chString name = nodeRoot.name();
            bParseOK = ("dialnow" == name.MakeLower());
        }

        if (bParseOK)
        {
            bFileOK = true;
            xml_node nodeChild = nodeRoot.first_child();
            while (nodeChild)
            {
                chString strName = nodeChild.name();
                if ("data" != strName.MakeLower())
                {
                    nodeChild = nodeChild.next_sibling();
                    continue;
                }

                yl::string strDialNow = nodeChild.attribute("DialNowRule").value();
                if (!strDialNow.empty())
                {
                    CRuleData* pRuleData = new CRuleData();
                    pRuleData->m_strPattern = strDialNow;
                    // 字符转换
                    if (PT_DECT != devicelib_GetPhoneType())
                    {
                        TransformString(pRuleData->m_strPattern);
                    }
                    yl::string strLine = nodeChild.attribute("LineID").value();
                    pRuleData->SetLineId(strLine);
                    m_listDialNow.push_back(pRuleData);
                }

                nodeChild = nodeChild.next_sibling();
            }
        }
    }

    if (!bFileOK)
    {
        TALK_INFO("Create new DialNow xml file");
        //文件不存在或文件存在问题，重新创建
        xml_document docTmp;
        xml_node nodeRoot = docTmp.append_child("dialnow");
        //保存到文件
        docTmp.save_file(DIALNOW_PATH);
    }

    return true;
}

// 加载地区号码
bool CDialRuleController::LoadAreaCode()
{
    // 清除旧数据
    ReleaseDialRuleByType(DR_AREA);

#if IF_BUG_27045
    //关闭AreaCode则不加载AreaCode
    if (configParser_GetConfigInt(kszAreaCodeEnable) == 0)
    {
        return false;
    }
#endif
    //读取AreaCode规则
    m_dataAreaCode.m_strPattern = configParser_GetConfigString(kszAreaCode);
    m_iMinLen = configParser_GetConfigInt(kszAreaCodeMinLen);
    m_iMaxLen = configParser_GetConfigInt(kszAreaCodeMaxLen);
    m_dataAreaCode.SetLineId(configParser_GetConfigString(kszAreaCodeAccount));

    return true;
}

// 导出配置文件中限制呼出的模式与号码
bool CDialRuleController::LoadBlockOut()
{
    // 清除旧的限制呼出的号码和用户列表
    ReleaseDialRuleByType(DR_BLOCKOUT);

    // 文件是否完整
    bool bFileOK = false;
    xml_document doc;
    if (doc.load_file(DIALBLOCK_PATH).status == status_ok)
    {
        //若文件存在，且读取正确
        xml_node nodeRoot = doc.first_child();

        if (nodeRoot && 0 == strcmp("dialblock", nodeRoot.name()))
        {
            bFileOK = true;
            xml_node nodeChild = nodeRoot.first_child();
            while (nodeChild)
            {
                chString strName = nodeChild.name();
                if ("data" != strName.MakeLower())
                {
                    nodeChild = nodeChild.next_sibling();
                    continue;
                }

                yl::string strBlokOut = nodeChild.attribute("rule").value();
                if (!strBlokOut.empty())
                {
                    CRuleData* pRuleData = new CRuleData();
                    pRuleData->m_strPattern = strBlokOut;
                    // 字符转换
                    TransformString(pRuleData->m_strPattern);
                    yl::string strLine = nodeChild.attribute("lines").value();
                    pRuleData->SetLineId(strLine);
                    m_listBlocked.push_back(pRuleData);
                }

                nodeChild = nodeChild.next_sibling();
            }
        }
    }

    if (!bFileOK)
    {
        TALK_INFO("Create new BlockOut xml file");
        // 文件不存在或文件存在问题，重新创建
        xml_document docTmp;
        xml_node nodeRoot = docTmp.append_child("dialblock");
        //保存到文件
        docTmp.save_file(DIALBLOCK_PATH);
    }

    return true;
}

// 判断是否是可立即呼出的号码
bool CDialRuleController::IsDialNowNumber(const yl::string& strDialText, int iLineId)
{
    if (!IsDialRuleEnabled(iLineId))
    {
        return false;
    }

    if (iLineId == -1)
    {
        return false;
    }

    ListData::iterator itr = m_listDialNow.begin();
    ListData::iterator itEnd = m_listDialNow.end();
    ++iLineId;

    while (itr != itEnd)
    {
        if (*itr != NULL
                && (*itr)->IsValidLineId(iLineId))
        {
            // 是否匹配立即呼出的规则
            if (0 == regExp_IsMatchedExact((*itr)->m_strPattern.c_str(), strDialText.c_str()))
            {
                return true;
            }
        }

        itr++;
    }

    //TALK_INFO("[CDialRuleController] No found dialnow number with lineid[ %d ]", iLineId);
    return false;
}

// 根据替换规则,替换号码
bool CDialRuleController::GetReplaceNumber(const yl::string& strDialText,
        yl::string& strReplaceNumber,
        bool bReplaceAll,
        int iLineId/* = -1*/)
{
    strReplaceNumber = strDialText;

    if (!IsDialRuleEnabled(iLineId))
    {
        return false;
    }

    if (-1 == iLineId)
    {
        return false;
    }

    yl::string strReplace = "";
    yl::string strFist = "";
    yl::string strReplaceNum = strReplaceNumber;
    ++iLineId;

    ListData::iterator it = m_listReplace.begin();
    ListData::iterator itEnd = m_listReplace.end();

    for (; it != itEnd; it++)
    {
        CRuleData* pRuleData = *it;
        if (!pRuleData->IsValidLineId(iLineId))
        {
            //TALK_INFO("[CDialRuleController] GetReplaceNumber:No Found %d", iLineId);
            continue;
        }

        if (0 == regExp_IsMatchedExact(pRuleData->m_strPattern.c_str(), strDialText.c_str()))
        {
            char tempBuf[1025];
            // 替换号码
            int iRet = regExp_Replace(pRuleData->m_strPattern.c_str(), strDialText.c_str(),
                                      pRuleData->m_strReplace.c_str(), tempBuf, 1024);
            if (0 == iRet)
            {
                strReplaceNumber = tempBuf;
            }
            else
            {
                //TALK_INFO("replace number fail[%d]", iRet);
            }

            TALK_INFO("Replace Dialnum  [%s]-->[%s]", strDialText.c_str(), strReplaceNumber.c_str());
            return true;
        }
    }

    return false;
}

// 获取地区号码
bool CDialRuleController::GetAreaCodeNumber(const yl::string& strDialText,
        yl::string& strAreaCodeNumber, int iLineId)
{
    if (!IsDialRuleEnabled(iLineId))
    {
        return false;
    }

    if (strDialText.empty() || iLineId < 0)
    {
        return false;
    }

#if IF_BUG_27045
    if (configParser_GetConfigInt(kszAreaCodeEnable) == 0)
    {
        return false;
    }
#endif

    ++iLineId;

    strAreaCodeNumber = strDialText;

    // 是否是有效号码
    if (!m_dataAreaCode.IsValidLineId(iLineId))
    {
        return false;
    }

    if (!m_dataAreaCode.m_strPattern.empty())
    {
        //区号不为空
        if (0 != m_iMaxLen && 0 != m_iMinLen)
        {
            // 判别是否是本地号码
            if (!strAreaCodeNumber.empty()
                    && (int)strAreaCodeNumber.length() >= m_iMinLen
                    && (int)strAreaCodeNumber.length() <= m_iMaxLen)
            {
                // 加上本地区号
                strAreaCodeNumber.insert(0, m_dataAreaCode.m_strPattern.c_str());
                TALK_INFO("Replace Dialarea code [%s]-->[%s]", strDialText.c_str(), strAreaCodeNumber.c_str());
                return true;
            }
        }
    }

    return false;
}

// 判断是否是限制呼出的号码
bool CDialRuleController::IsBlockOutNumber(const yl::string& strNumber, int iLineId)
{
    if (!IsDialRuleEnabled(iLineId))
    {
        return false;
    }

    if (strNumber.empty())
    {
        return false;
    }

    ++iLineId;
    // 遍历被限制号码的列表，看当前呼出号码和账号是否被限制
    ListData::iterator iter = m_listBlocked.begin();
    ListData::iterator itEnd = m_listBlocked.end();
    while (iter != itEnd)
    {
        if ((*iter)->IsValidLineId(iLineId))
        {
            // 判断是否匹配模式串
            if (0 == regExp_IsMatchedExact((*iter)->m_strPattern.c_str(), strNumber.c_str()))
            {
                return true;
            }
        }

        iter++;
    }

    //TALK_INFO("IsBlockOutNumber No Find [line id = %d]", iLineId);
    return false;
}

void CDialRuleController::GetDialNowData(DialNowListData& listDialNowData)
{
    listDialNowData.m_nTotalCount = m_listDialNow.size();

    int iCount = listDialNowData.m_nCountToGet;
    if (listDialNowData.m_nIndexFrom == 0 && listDialNowData.m_nCountToGet == 0)
    {
        iCount = listDialNowData.m_nTotalCount;
    }

    int iStart = listDialNowData.m_nIndexFrom;

    for (ListData::iterator iter = m_listDialNow.begin(); iter != m_listDialNow.end(); iter++)
    {
        if (*iter == NULL)
        {
            continue;
        }

        if (--iStart > 0)
        {
            continue;
        }

        if (iCount-- <= 0)
        {
            break;
        }

        DialNowData EntryData;

        EntryData.strRule = (*iter)->m_strPattern;

        if ((*iter)->m_listLineId.size() == 0)
        {
            EntryData.iLineMask = 0xFFFF;
        }
        else
        {
            for (YLList<int>::ListIterator iterHs = (*iter)->m_listLineId.begin();
                    iterHs != (*iter)->m_listLineId.end();
                    iterHs++)
            {
                EntryData.iLineMask |= (1 << (*iterHs - 1)); // 手柄 掩码
            }
        }

        listDialNowData.m_listId.push_back(EntryData);
    } 
}

// 释放资源
void CDialRuleController::ReleaseDialRuleByType(int iDRType/* = DR_ALL*/)
{
    // 清空替换规则
    if (DR_PLAN & iDRType)
    {
        ReleaseListData(&m_listReplace);
    }

    // 清空立即呼出规则
    if (DR_NOW & iDRType)
    {
        ReleaseListData(&m_listDialNow);
    }

    // 清空地区号码
    if (DR_AREA & iDRType)
    {
        m_dataAreaCode.m_strPattern.clear();
        m_dataAreaCode.m_strReplace.clear();
        m_iMaxLen = 0;
        m_iMinLen = 0;
    }

    // 清空限制呼出
    if (DR_BLOCKOUT & iDRType)
    {
        ReleaseListData(&m_listBlocked);
    }
}

// 为保持与T38一致需要将字符串做转换处理
void CDialRuleController::TransformString(yl::string& strTransfer)
{
    // 将匹配字符串中的“.”，“x”，“*”分别替换为“.*”，“.”，“\*”
    strTransfer = commonAPI_replaceSubStringByTag(strTransfer, "*", "\\*");
    strTransfer = commonAPI_replaceSubStringByTag(strTransfer, ".", ".*");
    strTransfer = commonAPI_replaceSubStringByTag(strTransfer, "x", ".");
    strTransfer = commonAPI_replaceSubStringByTag(strTransfer, "+", "\\+");
}

// 清除链表资源
void CDialRuleController::ReleaseListData(ListData* pListData)
{
    if (NULL == pListData)
    {
        return;
    }

    // 遍历链表,清空资源
    ListData::iterator itr = pListData->begin();
    ListData::iterator itEnd = pListData->end();
    while (itr != itEnd)
    {
        delete (*itr);
        itr++;
    }

    pListData->clear();
}

bool CDialRuleController::IsDialRuleEnabled(int iLineId)
{
    // 应产品部需求，DigitMap和DialRule不能兼容
    return !DigitMap_IsEnable(iLineId);
}
