#include "setpasswordcontroller.h"

#include "ETLLib.hpp"
#include "commonunits/modcommonunits.h"
#include "setting/include/common.h"
#include <ylstring.h>
#include <ylhashmap.h>
#include <unistd.h>
#include "path_define.h"
#include <winimp/winimp.h>
#include "basemethod.h"

namespace NS_SETTING
{
//////////////////////////////////////////////////////////////////////////
// Class CSetPasswordController.
CSetPasswordController * CSetPasswordController::m_pController = NULL;

class filePassword
{
public:
    bool IsRightUserPassword(const char * lpszUserName, const char * lpszPassword);
    bool SetUserPassword(const char * lpszUserName, const char * lpszPassword);
    bool RemoveUser(const char * lpszUserName);

    void SaveToFile(const char * lpszFileName);

    yl::string GetEncryptedText(const char * lpszPlainText, const char * lpszSaltText);
public:
    filePassword(const char * lpszFileName);
public:
    YLHashMap<yl::string, yl::string> m_users;
};

filePassword::filePassword(const char * lpszFileName)
{
    m_users.clear();
    char buf[1024] = {0};
    yl::string line;
    FILE * pFile = fopen(lpszFileName, "rb");
    if (pFile != NULL)
    {
        while (fgets(buf, 1024, pFile) != NULL)
        {
            if (buf[strlen(buf) - 1] == '\n')
            {
                buf[strlen(buf) - 1] = '\0';
            }
            line = buf;
            int iPos = line.find(":");
            if (iPos != yl::string::npos)
            {
                m_users.insert(line.substr(0, iPos), line.substr(iPos + 1));
            }
        }
        fclose(pFile);
    }

    srand(time(0));
}

bool filePassword::IsRightUserPassword(const char * lpszUserName, const char * lpszPassword)
{
    yl::string strUsername = lpszUserName;
    YLHashMap<yl::string, yl::string>::iterator itUser = m_users.find(strUsername);
    if (itUser != m_users.end())
    {
        yl::string strPasswordInFile = itUser->second;
        yl::string strPasswordInput = GetEncryptedText(lpszPassword, strPasswordInFile.c_str());
        return strPasswordInput == strPasswordInFile;
    }
    return false;
}

bool filePassword::SetUserPassword(const char * lpszUserName, const char * lpszPassword)
{
    bool bPasswordChanged = false;
    yl::string strUsername = lpszUserName;
    YLHashMap<yl::string, yl::string>::iterator itUser = m_users.find(strUsername);
    if (itUser == m_users.end())
    {
        yl::string strPasswordToFile = GetEncryptedText(lpszPassword, NULL);
        m_users.insert(strUsername, strPasswordToFile);
        bPasswordChanged = true;
    }
    else
    {
        yl::string & strPasswordInfile = itUser->second;
        yl::string strPasswordToFile = GetEncryptedText(lpszPassword, strPasswordInfile.c_str());
        if (strPasswordInfile != strPasswordToFile)
        {
            strPasswordInfile = strPasswordToFile;
            bPasswordChanged = true;
        }
    }
    return bPasswordChanged;
}

bool filePassword::RemoveUser(const char * lpszUserName)
{
    yl::string strUsername = lpszUserName;
    YLHashMap<yl::string, yl::string>::iterator itUser = m_users.find(strUsername);
    if (itUser != m_users.end())
    {
        m_users.erase(itUser);
        return true;
    }
    return false;
}

void filePassword::SaveToFile(const char * lpszFileName)
{
    FILE * pFile = fopen(lpszFileName, "w+");
    if (pFile == NULL)
    {
        return;
    }
    YLHashMap<yl::string, yl::string>::iterator it = m_users.begin();
    for (; it != m_users.end(); ++it)
    {
        fwrite(it->first.c_str(), it->first.length(), 1, pFile);
        fwrite(":", 1, 1, pFile);
        fwrite(it->second.c_str(), it->second.length(), 1, pFile);
        fwrite("\n", 1, 1, pFile);
    }

    fsync(fileno(pFile));
    fclose(pFile);
}

static const char * szSaltCharacters =
    "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
yl::string filePassword::GetEncryptedText(const char * lpszPlainTextInput,
        const char * lpszSaltText)
{
    const char * lpszPlainTextTemp = lpszPlainTextInput;
    if (lpszPlainTextTemp == NULL)
    {
        lpszPlainTextTemp = "";
    }
    // 容错，将不可见字符去除
    yl::string strText;
    char szTemp[2] = {0};
    while (*lpszPlainTextTemp)
    {
        if (*lpszPlainTextTemp >= 32
                && *lpszPlainTextTemp <= 126)
        {
            *szTemp = *lpszPlainTextTemp;
            strText += szTemp;
        }
        else
        {
            strText += "\0";
            break;
        }
        lpszPlainTextTemp++;
    }

    const char * lpszPlainText = strText.c_str();
    int nPasswordLen = strText.length();
    int nCryptLoops = nPasswordLen == 0 ? 1 : (nPasswordLen - 1) / 8 + 1;

    yl::string strSalt = lpszSaltText;
    int nSaltLen = strSalt.length();
    int nShortenBytes = nCryptLoops * 13 - nSaltLen;
    while (nShortenBytes > 0)
    {
        if (nSaltLen != 0 && nSaltLen % 13 == 0)
        {
            *szTemp = strSalt[0];
            strSalt += szTemp;
            *szTemp = strSalt[1];
            strSalt += szTemp;
            nSaltLen += 2;
            nShortenBytes -= 2;
            continue;
        }
        *szTemp = szSaltCharacters[rand() % 64];
        strSalt += szTemp;
        -- nShortenBytes;
        nSaltLen ++;
    }

    lpszSaltText = strSalt.c_str();

    yl::string strResult;
    while (nCryptLoops > 0)
    {
        strResult += crypt(lpszPlainText, lpszSaltText);
        lpszPlainText += 8;
        lpszSaltText += 13;
        -- nCryptLoops;
    }
    return strResult;

}

// 密码文件名.
static const char kszPasswordFile[] = "htpasswd";

CSetPasswordController * CSetPasswordController::GetInstance()
{
    // Instantiate the singleton instance if not existed.
    if (m_pController == NULL)
    {
        m_pController = new CSetPasswordController();
    }
    return m_pController;
}

void CSetPasswordController::ReleaseInstance()
{
    // Release the singleton instance if existed.
    DELETE_OBJ(m_pController);
}

bool CSetPasswordController::GetPageMenuListByClickAction(
    MenuItemListData * pData, const yl::string & strClickAction) const
{
    // Check the click action.
    if (strClickAction.empty() || pData == NULL)
    {
        return false;
    }

    // Call the base method.
    return NS_SETTING::GetPageMenuListByClickAction(pData, strClickAction);
}

const yl::string CSetPasswordController::GetTitleByClickAction(
    const yl::string & strClickAction) const
{
    if (strClickAction.empty())
    {
        return "";
    }

    // Call the base method.
    return NS_SETTING::GetTitleByClickAction(strClickAction);
}

bool CSetPasswordController::SetPassword(const yl::string strUser,
        const PasswordData & refData) const
{
#ifdef WIN32
    return commonAPI_SavePassword(strUser.c_str(), refData.m_strPasswordNew.c_str());
#else
    // 获取密码文件全路径.
    yl::string strFullPath(CONFIG_PATH);
    strFullPath.append("data/");
    strFullPath.append(kszPasswordFile);

    // 设置密码.
    filePassword fpFile(strFullPath.c_str());
    if (fpFile.IsRightUserPassword(strUser.c_str(), refData.m_strPasswordBefore.c_str()))
    {
        fpFile.SetUserPassword(strUser.c_str(), refData.m_strPasswordNew.c_str());
        fpFile.SaveToFile(strFullPath.c_str());
        return true;
    }
    return false;
#endif
}

CSetPasswordController::CSetPasswordController()
{

}

CSetPasswordController::~CSetPasswordController()
{

}

}  // namespace NS_SETTING
