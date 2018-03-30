#include "talkhelp_inc.h"

#ifdef IF_FEATURE_TALK_ERROR_OPT

IMPLEMENT_GETINSTANCE(CTalkErrorInfoManager)

CTalkErrorInfoManager::CTalkErrorInfoManager()
{
}

CTalkErrorInfoManager::~CTalkErrorInfoManager()
{
}

int CTalkErrorInfoManager::GetErrorInfoByString(LPCSTR lpReason)
{
    if (lpReason == NULL)
    {
        return TALK_ERROR_NONE;
    }

    struct ErrorInfo
    {
        int             iErrorCode;
        LPCSTR  const   lpErrorType;
    };

    const ErrorInfo s_arr_err_info[] =
    {
        {  TALK_ERROR_CALLOUT_SLU,      "SLU" },
        {  TALK_ERROR_CALLOUT_MSE,      "MSE" },
        {  TALK_ERROR_CALLOUT_NU,       "NU"  },
        {  TALK_ERROR_CALLOUT_IA,       "IA"  },
        {  TALK_ERROR_CALLOUT_BH,       "BH"  },
        {  TALK_ERROR_CALLOUT_FN,       "FN"  },
        {  TALK_ERROR_CALLOUT_TO,       "TO"  },
        {  TALK_ERROR_CALL_RELEASED,    "CR"  },
        {  TALK_ERROR_CALLOUT_NOAUTH,   "AUTH"},
        {  TALK_ERROR_DEFAULT,          "*"   },
    };

    for (int i = 0; i < sizeof(s_arr_err_info) / sizeof(s_arr_err_info[0]); ++i)
    {
        if (strcasecmp(lpReason, s_arr_err_info[i].lpErrorType) == 0)
        {
            return s_arr_err_info[i].iErrorCode;
        }
    }

    return atoi(lpReason);
};

BOOL CTalkErrorInfoManager::Init()
{
    m_mapErrorInfo.clear();

#define MAX_ERROR_INFO_NUM  255

    for (int i = 0; i <= MAX_ERROR_INFO_NUM; ++i)
    {
        yl::string strReason = configParser_GetCfgItemStringValue(kszTalkErrorInfoCode, i);
        if (strReason.empty())
        {
            continue;
        }

        YLVector<yl::string> veclist;
        if (ParseString(veclist, strReason, ", "))
        {
            yl::string strPrompt = configParser_GetCfgItemStringValue(kszTalkErrorInfoPrompt, i);

            for (YLVector<yl::string>::iterator it = veclist.begin();
                    it != veclist.end(); ++it)
            {
                int iType = GetErrorInfoByString(it->c_str());
                if (TALK_ERROR_NONE < iType && iType < TALK_ERROR_MAX)
                {
                    m_mapErrorInfo[iType] = strPrompt;
                }
            }
        }
    }

    return m_mapErrorInfo.size() > 0;
}

LPCSTR CTalkErrorInfoManager::GetErrorInfo(int iType)
{
    if (iType <= TALK_ERROR_NONE || iType >= TALK_ERROR_MAX)
    {
        return NULL;
    }

    TALK_DBG("GetErrorInfo [%d]", iType);

    TALK_ERROR_MAP::iterator it = m_mapErrorInfo.find(iType);
    if (it != m_mapErrorInfo.end())
    {
        return it->second.c_str();
    }

    // 匹配默认规则
    it = m_mapErrorInfo.find(TALK_ERROR_DEFAULT);
    return it != m_mapErrorInfo.end() ? it->second.c_str() : NULL;
}

#endif
