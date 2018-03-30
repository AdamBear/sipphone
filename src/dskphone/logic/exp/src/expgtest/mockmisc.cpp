#include "mockmisc.h"
#include "stdarg.h"

MockMisc * g_pMockMisc;

#ifdef  __cplusplus
extern "C" {
#endif

int pw_data_encrypt(const unsigned char * src, unsigned char dest[120]);
int pw_data_decrypt(const unsigned char * src, unsigned char dest[120]);



#ifdef  __cplusplus
}
#endif

BOOL pathFileExist(LPCSTR lpszFileName);

int pw_data_encrypt(const unsigned char * src, unsigned char dest[120])
{
    return g_pMockMisc->pw_data_encrypt(src, dest);
}
int pw_data_decrypt(const unsigned char * src, unsigned char dest[120])
{
    return g_pMockMisc->pw_data_decrypt(src, dest);
}

bool talklogic_SessionExist(bool bIngoreAutoRedialSession = false)
{
    return g_pMockMisc->talklogic_SessionExist(bIngoreAutoRedialSession);
}


yl::string commonAPI_FormatString(const char * szFormat, ...)
{
    // Use va_list to format.
    va_list vl;
    va_start(vl, szFormat);

    int len = -1;

    yl::string strRet("");
#if defined(_WIN32)
    //_vsnprintf(szBuffer, sizeof(szBuffer), szFormat, vl);
#else

    len = vsnprintf(NULL, 0, szFormat, vl);

    if (len > 0)
    {
        char * pBuffer = new char[len + 1];
        if (pBuffer)
        {
            memset(pBuffer, 0, len + 1);
            vsnprintf(pBuffer, len + 1, szFormat, vl);
            strRet = pBuffer;
            delete[] pBuffer;
        }
    }

#endif  // _WIN32

    // Append '\0' to the end of string.
    va_end(vl);

    return strRet;
}

BOOL netIsNetworkReady()
{
    return g_pMockMisc->IsNetworkReady();
}


LINE_STATE modVoip_QueryLineState(int iAccountID)
{
    return g_pMockMisc->modVoip_QueryLineState(iAccountID);
}

// 获取机器标识
PHONE_TYPE_DEF devicelib_GetPhoneType()
{
    return g_pMockMisc->devicelib_GetPhoneType();
}


LPCSTR netGetMACText(ACHAR cSplit)
{
    return g_pMockMisc->netGetMACText(cSplit);
}

BOOL pathFileExist(LPCSTR lpszFileName)
{
    return g_pMockMisc->pathFileExist(lpszFileName);
}

void AdapterBox_ResetFactory(int iResetType)
{
    g_pMockMisc->AdapterBox_ResetFactory(iResetType);
}



bool idleScreen_SetStatus(int nStatus, const yl::string strHint = "")
{


    return g_pMockMisc->idl_SetStatus(nStatus, strHint);

}

bool idleScreen_DropStatus(int nStatus)
{
    return g_pMockMisc->idl_DropStatus(nStatus);
}
