#include "talkasync_inc.h"

bool CRingHelpImpl::GetRingFileByPath(const yl::string& strPath, YLVector<yl::string>& vecFile)
{
    //通用接口使用链表存储，这里为了使用方便，将链表内容存入数组
    YLList<yl::string> listFile;
    if (commonAPI_getFilesInDirectoryByExt(&listFile, strPath, ".wav,.WAV"))
    {
        // T4X上需要将链表排序
        Sort(listFile, yl::less<yl::string>());

        vecFile.clear();
        //解析链表，生成数组
        for (YLList<yl::string>::iterator iter = listFile.begin();
                iter != listFile.end(); iter++)
        {
            yl::string& strFile = *iter;
            vecFile.push_back(strFile);
        }
    }

    return vecFile.size() > 0;
}

bool CRingHelpImpl::GetRingPathByWeb(const yl::string& strUrl, yl::string& strPath)
{
    // 网页Ring配置项总数
#define ALERTINFO_RINGER_NUM 10

    if (strUrl.empty())
    {
        return false;
    }

    yl::string strRing;
    char szBuf[MAX_PATH] = { 0 };

    // 1.匹配MyMelody方式
    if (1 == sscanf(strUrl.c_str(), "MyMelody%[^\n]", szBuf))
    {
        strRing = szBuf;
    }
    else
    {
        // 2.如果1匹配失败，则匹配配置中名字为strRingName的铃声序号
        for (int i = 0; i < ALERTINFO_RINGER_NUM; i++)
        {
            sprintf(szBuf, "AlertInfo%dText", i);
            if (strcasecmp(strUrl.c_str(), configParser_GetConfigString(szBuf).c_str()) == 0)
            {
                sprintf(szBuf, "AlertInfo%dRinger", i);
                strRing = configParser_GetConfigString(szBuf);
                break;
            }
        }
    }

    return GetRingPathByIndexOrName(strRing, strPath, true);
}

bool CRingHelpImpl::GetRingPathByKeyword(const yl::string& strKey, yl::string& strPath)
{
    // 通过关键字查找铃声
    const char* szLocalHost = "127.0.0.1/";
    int iPos = strKey.find(szLocalHost);
    if (iPos == yl::string::npos)
    {
        return false;
    }

    iPos += strlen(szLocalHost);
    yl::string strKeyWord = strKey.substr(iPos).to_lower();
    if (strKeyWord.empty())
    {
        return false;
    }

    yl::string strResPath = res_getPathByName(RESOURCE_PATH_RING);

    LPCSTR arr[2] = { USER_RING_FILE_PATH, strResPath.c_str() };
    for (int i = 0; i < sizeof(arr) / sizeof(arr[0]); ++i)
    {
        YLVector<yl::string> strFileArray;
        GetRingFileByPath(arr[i], strFileArray);
        for (YLVector<yl::string>::iterator it = strFileArray.begin(); it != strFileArray.end(); ++it)
        {
            if (yl::string(*it).to_lower().find(strKeyWord) != yl::string::npos)
            {
                strPath = chPathNameA(arr[i], it->c_str());
                return pathFileExist(strPath.c_str());
            }
        }
    }

    return false;
}

bool CRingHelpImpl::GetRingFromSIPInfo(int CallId, int iAccount, CallInUserInfo* pCallData,
                                       yl::string& strPath, int& iStatus, int& iRingType)
{
    if (pCallData == NULL)
    {
        return false;
    }

    TALK_INFO("Ring: Local[%s] URL[%s] Web[%s]", pCallData->strRingLocal.c_str(),
              pCallData->strRingURL.c_str(), pCallData->strRingWeb.c_str());

    // 过滤不支持的URL
    yl::string strUrl = pCallData->strRingURL;
    int iPos = strUrl.find("://");
    if (iPos != yl::string::npos)
    {
        // 去掉http/https/ftp/tftp：//等头部比较
        strUrl.erase(0, iPos + 3);
    }

    // 只判断关键域名：notused
    // http://192.168.1.99/Bug.php?BugID=28885
    if (strUrl.find("notused") == yl::string::npos
            && strUrl != "127.0.0.1")
    {
        strUrl = pCallData->strRingURL;
    }
    else
    {
        strUrl.clear();
    }

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    if (GetRingPathByKeyword(pCallData->strRingURL, strPath))
    {
        return true;
    }
#endif

    if (!strUrl.empty()
            && configParser_GetCfgItemIntValue(kszAccountDisableAlertInfoURL, iAccount) == 0)
    {
        // SIP 指定铃声URL > Web > Local
        if (talkasync_CreateGetRingFile(CallId, strUrl))
        {
            iStatus = TALK_RING_LS_LOADING;
        }
    }

    // 解析Web铃声路径,如果Web铃声存在，使用Web指定铃声，否则使用Local指定铃声
    if (GetRingPathByWeb(pCallData->strRingWeb, strPath))
    {
        return true;
    }

    if (pCallData->strRingLocal.empty())
    {
        return false;
    }

    // 读出当前来电指定铃声
    char szRingBuffer[MAX_PATH] = { 0 };
    if (1 == sscanf(pCallData->strRingLocal.c_str(), "ringtone-%[^\n]", szRingBuffer))
    {
        //解析出铃声格式为ringtone-*
        return GetRingPathByIndexOrName(szRingBuffer, strPath);
    }
    else if (1 == sscanf(pCallData->strRingLocal.c_str(), "Bellcore-dr%[^\n]", szRingBuffer))
    {
        //解析出铃声格式为Bellcore-dr*
        return GetRingPathByIndexOrName(szRingBuffer, strPath, false,
                                        configParser_GetConfigInt(kszAlertInfo) != 0);
    }

    return false;
}

bool CRingHelpImpl::GetAccountRing(int iAccountID, yl::string& strPath)
{
    if (!acc_IsAccountIDValid(iAccountID))
    {
        return false;
    }

    yl::string strRing = acc_GetAccountRing(iAccountID);

    if (strRing.empty())
    {
        return false;
    }

    // 保存铃声为Ring1.wav格式
    if (strRing.find_first_of(":") == yl::string::npos
            && strRing.find_first_of("/") == yl::string::npos
            && commonAPI_StringConvert(strRing, STRING_CONVERT_TO_LOWER) != "default"
            && commonAPI_StringConvert(strRing, STRING_CONVERT_TO_LOWER) != "common")
    {
        yl::string str = chPathNameA(res_getPathByName(RESOURCE_PATH_RING).c_str(), strRing.c_str());
        if (pathFileExist(str.c_str()))
        {
            strPath = str;
            return true;
        }

        str = chPathNameA(USER_RING_FILE_PATH, strRing.c_str());
        if (pathFileExist(str.c_str()))
        {
            strPath = str;
            return true;
        }
    }
    else
    {
        // 保存铃声文件为Resource:Ring1.wav格式
        commonUnits_Hide2Real(strRing, PHFILE_TYPE_RING_FILE);
        if (pathFileExist(strRing.c_str()))
        {
            // 账号指定的铃声
            strPath = strRing;
            return true;
        }
    }

    return false;
}

bool CRingHelpImpl::GetResRingByIndex(int iIndex, yl::string& strPath)
{
    YLVector<yl::string> strFileArray;
    yl::string strResPath = res_getPathByName(RESOURCE_PATH_RING);
    GetRingFileByPath(strResPath, strFileArray);

    if (iIndex >= 0 && iIndex < strFileArray.size())
    {
        strPath = strResPath + strFileArray[iIndex];
        return pathFileExist(strPath.c_str());
    }

    return false;
}

bool CRingHelpImpl::GetRingPathByIndexOrName(const yl::string& strRing, yl::string& strPath,
        bool bIncludeUserPath/* = false*/, bool bIncludeBsftPath/* = false*/)
{
    // 获取各铃声路径文件列表
    YLVector<yl::string> vecResourceRings;
    const yl::string cstrResourceRingPath = res_getPathByName(RESOURCE_PATH_RING);

    GetRingFileByPath(cstrResourceRingPath, vecResourceRings);
    YLVector<yl::string> vecUserRings;
    if (bIncludeUserPath)
    {
        GetRingFileByPath(USER_RING_FILE_PATH, vecUserRings);
    }

    YLVector<yl::string> vecBsftRings;
    if (bIncludeBsftPath)
    {
        GetRingFileByPath(BSFT_RING_FILE_PATH, vecBsftRings);
    }

    // 根据字符串是否含有'.'来判断是数字还是文件名
    if (strRing.find('.') == yl::string::npos)
    {
        // 参数视为数字序号
        int iRingIndex = atoi(strRing.c_str());
        if (iRingIndex <= 0)
        {
            // 铃声序号从1开始编号，0表示转换为数字序号失败
            TALK_WARN("Ring index [%s] is not a valid number!", strRing.c_str());
            return false;
        }
        // 转换为从0开始的序号，根据序号选择
        --iRingIndex;
        // 目录中取出序号对应的铃声路径，优先级：BSFT > Resource > User
        if (bIncludeBsftPath && iRingIndex < vecBsftRings.size())
        {
            strPath = chPathNameA(BSFT_RING_FILE_PATH, vecBsftRings[iRingIndex].c_str());
        }
        if (strPath.empty() && iRingIndex < vecResourceRings.size())
        {
            strPath = chPathNameA(cstrResourceRingPath.c_str(), vecResourceRings[iRingIndex].c_str());
        }

        int iUserRingIndex = iRingIndex - vecResourceRings.size();
        if (bIncludeUserPath
                && strPath.empty()
                && (iUserRingIndex >= 0)
                && (iUserRingIndex < vecUserRings.size()))
        {
            strPath = chPathNameA(USER_RING_FILE_PATH,  vecUserRings[iUserRingIndex].c_str());
        }
    }
    else
    {
        // 根据文件名查找铃声文件路径
        strPath = commonUnit_SearchRingPath(strRing, "");
    }

    TALK_DBG("GetRingPathByName ring[%s] path[%s]", strRing.c_str(), strPath.c_str());
    // 过滤查找到的空路径
    LPCSTR lpszFileName = pathGetFileNamePos(strPath.c_str());
    if (textEMPTY(lpszFileName))
    {
        return false;
    }

    return pathFileExist(strPath.c_str());
}
