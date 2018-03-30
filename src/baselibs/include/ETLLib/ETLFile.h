#ifndef __ETLFILE_H__
#define __ETLFILE_H__

#ifndef __ETL_H__
#error ETLFile.h requires ETL.h to be included first
#endif

namespace ETL_FileHelper
{
EXTERN_DECLARE(chTempByteArray etlDumpBinFile(LPCSTR lpszFileName));

EXTERN_DECLARE(BOOL etlWriteBinFile(LPCSTR lpszFileName, LPCVOID pData, int nSize));
EXTERN_DECLARE(yl::string etlDumpTextFileA(LPCSTR lpszFileName));
EXTERN_DECLARE(yl::wstring etlDumpTextFileW(LPCSTR lpszFileName));

class chFileStdio;
// -1 on Error else file version
EXTERN_DECLARE(int fileReadFileVersion(chFileStdio & objFile, LPCSTR lpszFileType));
EXTERN_DECLARE(BOOL fileWriteFileVersion(chFileStdio & objFile, LPCSTR lpszFileType, int nFileVer));

EXTERN_DECLARE(LPCSTR etlPeekCSVQuote(LPCSTR lpszText, yl::string & strField));
EXTERN_DECLARE(LPCSTR etlPeekCSVField(LPCSTR lpszText, yl::string & strField));
//EXTERN_DECLARE(LPCSTR etlPeekCSVRecord(LPCSTR lpszText, chListStringA& arrRecord));
EXTERN_DECLARE(yl::string CDECL etlBuildCSVLine(int nFields, ...));

//////////////////////////////////////////////////////////////////////////
// class chPathNameA
class chPathNameA : public yl::string
{
public:
    chPathNameA & operator << (LPCSTR lpszFileName);
    operator LPSTR();
public:
    chPathNameA & ReplaceFileName(LPCSTR lpszNewFileName);
    chPathNameA & ReplaceExternOnly(LPCSTR lpszNewExternName);
public:
    chPathNameA();
    chPathNameA(const yl::string & srcPath);
    chPathNameA(LPCSTR lpszPathName);
    chPathNameA(LPCSTR lpszPathName, LPCSTR lpszFileName);
};

//////////////////////////////////////////////////////////////////////////
// class chFileStdio
class chFileStdio
{
public:
    // Flag values
    enum OpenFlags
    {
        modeRead =          0x0000,
        modeWrite =         0x0001,
        modeCreateWrite =   0x1001,
        modeReadWrite =     0x0002,
        modeCreate =        0x1000,
        modeNoTruncate =    0x2000,
        typeText =          0x4000, // typeText and typeBinary are used in
        typeBinary = (int)0x8000    // derived classes only
    };

    enum SeekPosition { begin = SEEK_SET, current = SEEK_CUR, end = SEEK_END };

public:
    BOOL Open(LPCSTR lpszFileName, UINT nOpenFlags);
    void Close();
    BOOL IsFileOpened();
    BOOL IsEOF();
    // Operations
    int Read(void * lpBuf, int nItemSize, int nCount);
    BOOL Write(LPCVOID lpBuf, int nItemSize, int nCount);
    BOOL Flush();

    int SeekToEnd() const;
    void SeekToBegin() const;
    LONG Seek(LONG lOff, int nFrom = chFileStdio::begin) const;
    int GetPosition() const;

    int GetFileLength() const;
    BOOL SetFileLength(int nFileLength);

    // read & write ACHAR string
    BOOL ReadString(yl::string & strTextA, int uEndChar = '\n');
    BOOL WriteString(const yl::string & strTextA);
    BOOL WriteString(LPCSTR lpszTextA);

    template<class typeArray>
    BOOL ReadObjArray(typeArray & objArray, int nCount)
    {
        objArray.SetArraySize(nCount);
        return Read(objArray.GetBuffer(), sizeof(*(objArray.GetBuffer())), nCount) == nCount;
    }
    template<class typeArray>
    BOOL WriteObjArray(typeArray & objArray)
    {
        return Write(objArray.GetBuffer(), sizeof(*(objArray.GetBuffer())), objArray.GetSize());
    }
public:
    // Constructors
    chFileStdio(FILE * pFile = NULL);
    chFileStdio(LPCSTR lpszFileName, UINT nOpenFlags);
    ~chFileStdio();
private:
    chFileStdio(const chFileStdio & src);
    chFileStdio & operator = (chFileStdio & objIniLine);
protected:
    // Attributes
    FILE  * m_pFile;
};
#define nullFile    (*(chFileStdio*)NULL)

/*
template<class typeFile, class typeObj>
inline BOOL fileRead(typeFile& file, typeObj& obj)
{
    return file.Read(&obj, sizeof(obj), 1) == 1;
}

template<class typeFile, class typeObj>
inline BOOL fileWrite(typeFile& file, const typeObj& obj)
{
    return file.Write(&obj, sizeof(obj), 1);
}
*/

// write all type as pain-object
#define FILE_WRITE(file, obj)           \
    if(!file.Write(&obj, sizeof(obj), 1))\
        return FALSE

// read all type as pain-object
#define FILE_READ(file, obj)            \
    if(file.Read(&obj, sizeof(obj), 1) != 1)\
        return FALSE

// write yl::string as int and pain-text
#define FILE_WRITE_chString(file, str)  \
    do{\
        int len = str.GetLength();\
        FILE_WRITE(file, len);\
        if(!file.Write((LPCTSTR)str, sizeof(TCHAR), len))\
            return FALSE;\
    }while(FALSE)

// read chString as int and pain-text
#define FILE_READ_chString(file, str)   \
    do{\
        int len = 0;\
        FILE_READ(file, len);\
        LPTSTR lpszText = str.GetBufferSetLength(len);\
        if(file.Read(lpszText, sizeof(TCHAR), len) != len)\
            return FALSE;\
    }while(FALSE)

EXTERN_DECLARE_CPP(bool operator == (const chFileStdio & src, const chFileStdio & other));
EXTERN_DECLARE_CPP(bool operator != (const chFileStdio & src, const chFileStdio & other));

EXTERN_DECLARE_CPP(chFileStdio & operator << (chFileStdio & file, const int & other));
EXTERN_DECLARE_CPP(chFileStdio & operator << (chFileStdio & file, const UINT & other));
EXTERN_DECLARE_CPP(chFileStdio & operator << (chFileStdio & file, const double & other));
EXTERN_DECLARE_CPP(chFileStdio & operator << (chFileStdio & file, const yl::string & other));
EXTERN_DECLARE_CPP(chFileStdio & operator << (chFileStdio & file, LPCSTR other));

EXTERN_DECLARE_CPP(chFileStdio & operator >> (chFileStdio & file, int & other));
EXTERN_DECLARE_CPP(chFileStdio & operator >> (chFileStdio & file, UINT & other));
EXTERN_DECLARE_CPP(chFileStdio & operator >> (chFileStdio & file, double & other));
EXTERN_DECLARE_CPP(chFileStdio & operator >> (chFileStdio & file, yl::string & other));




#if WINDOWS_SYSTEM
#define _szFileNewLineA_            "\r\n"
#define _szFileNewLineW_            L"\r\n"
#else
#define _szFileNewLineA_            "\n"
#define _szFileNewLineW_            L"\n"
#endif

//////////////////////////////////////////////////////////////////////////
// enum iniLineType
enum iniLineType
{
    LT_INVALID,
    LT_COMMENT,
    LT_KEY,
    LT_SECTION,
    LT_BLANK,
};

class phraseStringA;
class phraseConstTextA : public yl::string
{
private:
    bool operator == (const phraseStringA & s);
private:
    phraseConstTextA & operator = (LPCSTR src);
    phraseConstTextA & operator = (const yl::string & src);
//  phraseConstTextA& operator = (const phraseConstTextA& src);
    phraseConstTextA & operator = (const phraseStringA & src);
public:
    phraseConstTextA();
    phraseConstTextA(const phraseStringA & strText);
    phraseConstTextA(LPCSTR lpszText);
private:
    explicit phraseConstTextA(const yl::string & strText);
};

//////////////////////////////////////////////////////////////////////////
// class phraseStringA
class phraseStringA : public yl::string
{
private:
    bool operator == (const phraseConstTextA & s);
private:
//  phraseStringA& operator = (const phraseStringA& src);
public:
    phraseStringA & operator = (const phraseConstTextA & src);
    phraseStringA & operator = (LPCSTR src);
    phraseStringA & operator = (const yl::string & src);
public:
    phraseStringA();
    explicit phraseStringA(const phraseConstTextA & strText);

    void CopyFromObject(const char * pSrc);
private:
    explicit phraseStringA(LPCSTR lpszText);
    explicit phraseStringA(const yl::string & strText);
};

// EXTERN_DECLARE_CPP(phraseStringA& operator = (phraseStringA& dst, const yl::string& src));

EXTERN_DECLARE_CPP(bool stringSame(const yl::string & s1, const yl::string & s2));
EXTERN_DECLARE_CPP(bool stringSameNC(const yl::string & s1, const yl::string & s2));

//////////////////////////////////////////////////////////////////////////
// class iniObject
class iniObject
{
    friend class iniObjList;
    friend class iniDocument;
    friend class iniSection;
public:
    iniLineType type();
    const phraseStringA & key();
    virtual void exportObject(chFileStdio & file) = 0;
public:
    iniObject(iniLineType lineType);
    virtual ~iniObject();
private:
    iniObject(const iniObject & objIniLine);
    iniObject & operator = (const iniObject & objIniLine);
protected:
    iniLineType         m_eType;
    phraseStringA       m_strBaseText;
};

//////////////////////////////////////////////////////////////////////////
// class iniObjList
class iniObjList : public chObjList< iniObject * >
{
public:
    void ExportToFile(chFileStdio & file);
    void ClearObjList();

    iniObject * FindObject(iniLineType eType, const phraseConstTextA & strName);
protected:
    iterator list_find_object(iniLineType eType, const phraseConstTextA & strName);
    void list_erase_object(const iniObject & src);
public:
    ~iniObjList();
};

//////////////////////////////////////////////////////////////////////////
// class iniComment
class iniComment : public iniObject
{
public:
    virtual void exportObject(chFileStdio & file);
public:
    iniComment(const yl::string & strComment);
};

//////////////////////////////////////////////////////////////////////////
// class iniKey
class iniKey : public iniObject
{
public:
    const phraseStringA & value();
    yl::string GetValue(LPCSTR lpszDefaultValue);
    BOOL SetValue(const phraseConstTextA & strValue);
    virtual void exportObject(chFileStdio & file);
public:
    iniKey(const phraseConstTextA & strKeyName);
protected:
    phraseStringA       m_strValue;
    BOOL                m_bValueUndefined;
};

//////////////////////////////////////////////////////////////////////////
// class iniSection
class iniSection : public iniObject
    , public iniObjList
{
public:
    virtual void exportObject(chFileStdio & file);
public:
    iniKey * FindKey(const phraseConstTextA & strKeyName);
    iniKey & GetKey(const phraseConstTextA & strKeyName);
    void     RemoveKey(iniKey * pKey);
    void     RemoveKey(const phraseConstTextA & strKeyName);
public:
    iniSection(const phraseConstTextA & strName);
};

//////////////////////////////////////////////////////////////////////////
// class iniDocument
class iniDocument : public iniObjList
{
public:
    //////////////////////////////////////////////////////////////////////////
    // section operator
    iniSection & GetSection(const phraseConstTextA & strSectionName);
    iniSection * FindSection(const phraseConstTextA & strSectionName);
    void     RemoveSection(iniSection * pSection);
    void     RemoveSection(const phraseConstTextA & strKeyName);
private:
//  phraseStringA* FindKeyValue(const phraseConstTextA& strSectionName, const phraseConstTextA& strKeyName);
//  phraseStringA& GetKeyValue(const phraseConstTextA& strSectionName, const phraseConstTextA& strKeyName);

public:
    BOOL IsKeyExist(const phraseConstTextA & strSectionName, const phraseConstTextA & strKeyName);
    yl::string GetTextValue(const phraseConstTextA & strSectionName,
                            const phraseConstTextA & strKeyName, LPCSTR lpszDefValue);
    BOOL SetTextValue(const phraseConstTextA & strSectionName, const phraseConstTextA & strKeyName,
                      LPCSTR lpszDefValue);

    int GetIntValue(const phraseConstTextA & strSectionName, const phraseConstTextA & strKeyName,
                    int nDefValue);
    double GetFloatValue(const phraseConstTextA & strSectionName, const phraseConstTextA & strKeyName,
                         double lfValue);
};

//////////////////////////////////////////////////////////////////////////
// class iniFile
class iniFile : public iniDocument
{
public:
    BOOL LoadFromFile(LPCSTR lpszFileName);
    BOOL SaveToFile(LPCSTR lpszFileName = NULL);
private:
    iniLineType ParseIniLineText(LPCSTR lpszLineText, phraseConstTextA & strKeyName,
                                 phraseConstTextA & strValue);
public:
    iniFile(LPCSTR lpszFileName = NULL);
private:
    yl::string          m_strFileName;
};

typedef iniFile                 chIniFileA;
typedef iniSection              chIniSectionA;
typedef iniKey                  chIniLineA;

};
using namespace ETL_FileHelper;

#if IF_EXPORT_SYMBOLS
#include "ETLFile.cpp"
#endif


#endif //__ETLFILE_H__
