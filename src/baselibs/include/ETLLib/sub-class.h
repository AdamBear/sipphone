#ifndef __SUB_CLASS_H__
#define __SUB_CLASS_H__

namespace ETL_SubClass
{

EXTERN_DECLARE(BOOL etlFindFile(LPCSTR lpszPathName, LPCSTR lpszFilter,
                                chListStringA & listFilePath, BOOL bRecursion));
EXTERN_DECLARE(BOOL etlClipMemcpy2D(LPVOID pDst, const chSize & sizeDst, LPCVOID pSrc,
                                    const chSize & sizeSrc, const chPoint & ptDst, const chRect & clipDst));
EXTERN_DECLARE(BOOL etlMemcpy2D(LPVOID pDst, const chSize & sizeDst, LPCVOID pSrc,
                                const chSize & sizeSrc, const chPoint & ptDst));

yl::string profileGetString(HINIFILE hIniFile, LPCSTR lpszSectionName, LPCSTR lpszKeyName,
                            LPCSTR lpszValue);

yl::string etlGetProfileString(LPCSTR lpszConfigFile, LPCSTR lpszSectionName, LPCSTR lpszKeyName,
                               LPCSTR lpszDefaultValue);

yl::string registryGetString(HREGISTRY hRegistrySection, LPCSTR lpszKeyName,
                             LPCSTR lpszDefaultValue);

yl::string etlGetRegistryString(LPCSTR lpszConfigFile, LPCSTR lpszSectionName, LPCSTR lpszKeyName,
                                LPCSTR lpszDefaultValue);

BOOL etlCopyFile(LPCSTR lpszDstFile, LPCSTR lpszSrcFile);
//////////////////////////////////////////////////////////////////////////
// class chFileFilter
#if 0
class chFileFilter
{
    typedef chObjList<chRegularExpressA>    typeListRegularExpress;
public:
    BOOL SetFilter(LPCSTR lpszFilter);
    BOOL IsAcceptFileName(LPCSTR lpszFileName);
    BOOL IsDenyFileName(LPCSTR lpszFileName);
private:
    yl::string fileExpress2REText(LPCSTR lpszText);
public:
    chFileFilter(LPCSTR lpszFilter = NULL);
private:
    typeListRegularExpress      m_filterAccept;
    typeListRegularExpress      m_filterDeny;
};
#endif

};
using namespace ETL_SubClass;

#if IF_EXPORT_SYMBOLS
#include "sub-class.cpp"
#endif

#endif //__SUB_CLASS_H__
