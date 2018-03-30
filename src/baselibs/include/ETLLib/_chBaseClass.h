//-----------------------------------------------------------------------------
// 文件名称:    _chBaseClass.h
//
// 功能 : 基础类的头文件，包括简单数组类和IPC相关类，时间相关类，Atom相关类
//
// Copyright(c) 2005-2006 Xiamen Yealink Network Technology Co,.Ltd
// By:syz@yealink.com       2006-07-20
//-----------------------------------------------------------------------------

#ifndef __chBASECLASS_H__
#define __chBASECLASS_H__

#ifndef __ETL_H__
#error _chBaseClass.h only can be include in ETL.h
#endif

namespace ETL_BaseHelper
{

#define fnKEY_F1                0xFF70
#define fnKEY_F2                0xFF71
#define fnKEY_F3                0xFF72
#define fnKEY_F4                0xFF73
#define fnKEY_F5                0xFF74
#define fnKEY_F6                0xFF75
#define fnKEY_F7                0xFF76
#define fnKEY_F8                0xFF77
#define fnKEY_F9                0xFF78
#define fnKEY_F10               0xFF79
#define fnKEY_F11               0xFF7A
#define fnKEY_F12               0xFF7B

#define fnKEY_BACK              0x0008
#define fnKEY_TAB               0x0009
#define fnKEY_RETURN            0x000A
#define fnKEY_ESCAPE            0x001B
#define fnKEY_PRIOR             0xFF21
#define fnKEY_NEXT              0xFF22
#define fnKEY_END               0xFF23
#define fnKEY_HOME              0xFF24
#define fnKEY_LEFT              0xFF25
#define fnKEY_UP                0xFF26
#define fnKEY_RIGHT             0xFF27
#define fnKEY_DOWN              0xFF28
#define fnKEY_INSERT            0xFF2D
#define fnKEY_DELETE            0xFF2E

//////////////////////////////////////////////////////////////////////////
// class chTick
class chTick
{
public:
    bool operator < (const chTick & tick) const;
    bool operator > (const chTick & tick) const;
    chTick & operator = (const chTick & tick);
    chTick operator + (const chTick & tick);
    chTick operator - (const chTick & tick);
public:
    chTick(int nSec = 0, int nUS = 0);
public:
    int             m_nSeconds;
    int             m_nMicroSeconds;
};

//////////////////////////////////////////////////////////////////////////
// class chTimeout
class chTimeout : public chTick
{
public:
    chTimeout(int nMillisecond);
    void setTimeout(int nMillisecond);
    int GetLeftMillisecond();
    BOOL IsTimeout();
};

//////////////////////////////////////////////////////////////////////////
// class chTickTimer
class chTickTimer : public chTick
{
public:
    chTickTimer();
    int GetMicrosecond();
    int ResetTickTimer();
};

};
using namespace ETL_BaseHelper;

#if LINUX_SYSTEM
#include "xInc/xBaseClass.h"
#else
#include "wInc/wBaseClass.h"
#endif

namespace ETL_BaseHelper
{

//////////////////////////////////////////////////////////////////////////
// extern functions
EXTERN_DECLARE(yl::string etlGetConsoleLineInput());
EXTERN_DECLARE(UINT64 etlGetTickCount());

// 简单数组类
//////////////////////////////////////////////////////////////////////////
// class chSimpleArray
template <class typeObj>
class chSimpleArray
{
    typedef typeObj  *  type_PTR;
public:
    operator type_PTR() const
    {
        chASSERT(m_pThisPoint != NULL);
        return m_pThisPoint;
    }
    int size() const
    {
        return m_nArraySize;
    }
    typeObj & operator [](const int i) const
    {
        chASSERT(m_pThisPoint != NULL && i < m_nArraySize);
        return m_pThisPoint[i];
    }
public:
    chSimpleArray(int nSize)
    {
        m_pThisPoint = new typeObj[nSize];
        m_nArraySize = nSize;
    }
    ~chSimpleArray()
    {
        chASSERT(m_pThisPoint != NULL);
        delete[] m_pThisPoint;
        m_pThisPoint = NULL;
    }
protected:
    type_PTR        m_pThisPoint;
    int             m_nArraySize;
};

//////////////////////////////////////////////////////////////////////////
// class chAutoFreePointer
template <class typeObj>
class chAutoFreePointer
{
public:
    operator typeObj & () const
    {
        chASSERT(m_pThisObject != NULL);
        return *m_pThisObject;
    }
    operator typeObj * () const
    {
        return m_pThisObject;
    }
    typeObj * operator ->() const
    {
        chASSERT(m_pThisObject != NULL);
        return m_pThisObject;
    }
private:
    chAutoFreePointer & operator = (const chAutoFreePointer & objSrc);
public:
    chAutoFreePointer()
    {
        m_pThisObject = new typeObj;
    }
    chAutoFreePointer(typeObj * pObject)
    {
        m_pThisObject = pObject;
    }
    chAutoFreePointer(const chAutoFreePointer & objSrc)
    {
        chAutoFreePointer * pSrcObj = (chAutoFreePointer *)&objSrc;
        m_pThisObject = pSrcObj->m_pThisObject;
        pSrcObj->m_pThisObject = NULL;
    }
    ~chAutoFreePointer()
    {
        if (m_pThisObject != NULL)
        {
            delete m_pThisObject;
            m_pThisObject = NULL;
        }
    }
protected:
    typeObj    *    m_pThisObject;
};

//////////////////////////////////////////////////////////////////////////
// class chException
class chException
{
public:
    chException();
    ~chException();
public:
    UINT        m_uExceptID;
};

//////////////////////////////////////////////////////////////////////////
// class chAccumulater
class chAccumulater
{
public:
    void InitAccumulater(UINT uValue, UINT uMin, UINT uMax);
    UINT ExcuteAccumulate(int nStep);
public:
    chAccumulater();
public:
    UINT    m_uCurrentValue;
    UINT    m_uMinValue;
    UINT    m_uMaxValue;
};


//////////////////////////////////////////////////////////////////////////
// class chLocalTime
class chLocalTime : public SYSTEMTIME
{
public:
    chLocalTime();
};

//////////////////////////////////////////////////////////////////////////
// class chPoint
class chPoint
{
public:
    void OffsetPoint(int dx, int dy);
    chPoint operator-() const;
    BOOL operator==(const chPoint & pt) const;
    BOOL operator!=(const chPoint & pt) const;
public:
    chPoint();
    chPoint(int _x, int _y);
public:
    int x;
    int y;
};

//////////////////////////////////////////////////////////////////////////
// class chSize
class chSize
{
public:
    chSize operator-() const;
    BOOL operator==(const chSize & size) const;
    BOOL operator!=(const chSize & size) const;
public:
    chSize();
    chSize(int _cx, int _cy);
public:
    int cx;
    int cy;
};

//////////////////////////////////////////////////////////////////////////
// class chRect
class chRect
{
public:
// Attributes (in addition to RECT members)
    int Width() const;
    int Height() const;
    chSize Size() const;

    chPoint TopLeft() const;
    chPoint CenterPoint() const;
    chPoint BottomRight() const;

// Operations
    chRect & SetRect(int x1, int y1, int x2, int y2);
    chRect & SetEmpty();
    chRect & Resize(int w, int h);
    chRect & ResizeHeight(int nHeight);
    chRect & ResizeWidth(int nWidth);

    chRect & InflateRect(int x, int y);
    chRect & InflateRect(int l, int t, int r, int b);
    chRect & DeflateRect(int x, int y);
    chRect & DeflateRect(int l, int t, int r, int b);

    chRect & OffsetRect(int x, int y);
    chRect & OffsetRect(const chSize & size);

    chRect & operator &= (const chRect & rect);
    chRect & operator |= (const chRect & rect);
    chRect & operator = (const chRect & rect);

// compare
    BOOL PtInRect(const chPoint & pt) const;
    BOOL IsEmpty() const;
    void RectAnd(const chRect & rect);
    void RectOr(const chRect & rect);

    // 两个矩形是否相交
    BOOL Intersect(const chRect & rect);
    BOOL operator==(const chRect & rect) const;
    BOOL operator!=(const chRect & rect) const;
    BOOL IsSameSize(const chRect & rect) const;

    chRect operator +(const chPoint & pt) const;
    chRect operator &(const chRect & rect) const;
    chRect operator |(const chRect & rect) const;
    chRect operator +(const chSize & size) const;
public:
// Constructors
    chRect();
    chRect(int l, int t, int r, int b);
    chRect(const chRect & srcRect);
    chRect(const chPoint & pt, const chSize & size);
public:
    int left;
    int top;
    int right;
    int bottom;
};
EXTERN_DECLARE(chRect rectLTWH(int x, int y, int w, int h));
EXTERN_DECLARE(chRect InflateRect(const chRect & src, int l, int t, int r, int b));
EXTERN_DECLARE(chRect DeflateRect(const chRect & src, int l, int t, int r, int b));
EXTERN_DECLARE(chRect Resize(const chRect & src, int w, int h));
EXTERN_DECLARE(chRect ResizeHeight(const chRect & src, int h));
EXTERN_DECLARE(chRect ResizeWidth(const chRect & src, int w));
EXTERN_DECLARE(chRect OffsetRect(const chRect & src, int x, int y));


//////////////////////////////////////////////////////////////////////////
// class chAutoLock
template<class typeLock>
class chAutoLock
{
public:
    chAutoLock(typeLock * pLock)
    {
        chASSERT(pLock != NULL);
        m_pLock = pLock;
        Lock();
    }
    BOOL Lock()
    {
        return m_bLocked = m_pLock->Lock();
    }
    void Unlock()
    {
        if (m_bLocked)
        {
            m_bLocked = FALSE;
            m_pLock->Unlock();
        }
    }
    ~chAutoLock()
    {
        Unlock();
        m_pLock = NULL;
    }
private:
    typeLock  * m_pLock;
    BOOL        m_bLocked;
};

typedef chAutoLock<chCriticalSection>       chCriticalLock;
typedef chAutoLock<chMutex>                 chMutexLock;

//////////////////////////////////////////////////////////////////////////
// class chMapFile_Local
class chMapFile_Local
{
public:
    LPVOID OpenMapFile(LPCSTR lpszFileName, int nSize);
    void CloseMapFile();
    void DestroyMapFile();
    BOOL IsCreateByMe();
public:
    chMapFile_Local();
    ~chMapFile_Local();
private:
    // Private to avoid accidental use
    chMapFile_Local(const chMapFile_Local &);
    chMapFile_Local & operator=(const chMapFile_Local &);
protected:
    LPVOID      m_pMapData;
};

//////////////////////////////////////////////////////////////////////////
// class chMutex_Local
class chMutex_Local : public chCriticalSection
{
public:
    BOOL OpenMutex(LPCSTR lpszName);
    void CloseMutex();
    void DestroyMutex();
    BOOL IsCreateByMe();
    BOOL Lock(int nTimeout = NEVER_TIMEOUT);
    void Unlock();
};

//////////////////////////////////////////////////////////////////////////
// class chObjList_Lock
template <class typeObj>
class chObjList_Lock : public chObjList<typeObj>
    , public chCriticalSection
{
};


#if IF_IPC_SUPPORT
#define ipcMapFile              chMapFile
#define ipcMutex                chMutex
#else
#define ipcMapFile              chMapFile_Local
#define ipcMutex                chMutex_Local
#endif

//////////////////////////////////////////////////////////////////////////
// class ipcObjectHeader
class ipcObjectHeader
{
public:
    BOOL InitHeader(int nFileSize, int iVersion);
    BOOL AssertHeader(LPCSTR lpszObjName, int nFileSize, int iVersion);
public:
    ipcObjectHeader();
private:
    int m_nCurrentSize;
    int m_iCurrentVersion;
};

//////////////////////////////////////////////////////////////////////////
// class ipcObjectFrame
template<class typeObj, class typeMapFile, class typeMutex>
class ipcObjectFrame : public typeMutex
    , public typeMapFile
{
public:
    typeObj & getObject()
    {
        chASSERT(m_pIPCObject != NULL);
        return *m_pIPCObject;
    }
    BOOL OpenShareObject(LPCSTR lpszObjName, int nBytes)
    {
        chASSERT(m_pIPCObject == NULL);
        if (typeMutex::OpenMutex(lpszObjName))
        {
            typeMutex::Lock();
            if (OpenMapFileObject(lpszObjName, nBytes))
            {
                typeMutex::Unlock();
                return TRUE;
            }
            typeMutex::Unlock();
            typeMutex::CloseMutex();
        }
        return FALSE;
    }
    void CloseShareObject()
    {
        if (m_pIPCObject != NULL)
        {
            typeMutex::Lock();
            typeMapFile::CloseMapFile();
            m_pIPCObject = NULL;
            typeMutex::Unlock();
            typeMutex::CloseMutex();
        }
    }
    void DestroyShareObject()
    {
        if (m_pIPCObject != NULL)
        {
            typeMutex::Lock();
            typeMapFile::DestroyMapFile();
            m_pIPCObject = NULL;
            typeMutex::Unlock();
            typeMutex::DestroyMutex();
        }
    }
private:
    BOOL OpenMapFileObject(LPCSTR lpszObjName, int nBytes)
    {
        m_pIPCObject = (typeObj *)typeMapFile::OpenMapFile(lpszObjName, nBytes);
        chASSERT(m_pIPCObject != NULL);
        if (m_pIPCObject != NULL)
        {
            if (typeMapFile::IsCreateByMe())
            {
                chConstructObject(typeObj, m_pIPCObject);
                typeMapFile::Flush();
            }
            return TRUE;
        }
        typeMapFile::CloseMapFile();
        return FALSE;
    }
public:
    ipcObjectFrame()
    {
        m_pIPCObject = NULL;
    }
    ~ipcObjectFrame()
    {
        CloseShareObject();
    }
protected:
    typeObj      *      m_pIPCObject;
};

//////////////////////////////////////////////////////////////////////////
// class chTimeval
class chTimeval : public timeval
{
public:
    chTimeval(int nMillisecond);
    void setTimeout(int nMillisecond);
};

//////////////////////////////////////////////////////////////////////////
// class chFD_SET
class chFD_SET
{
public:
    void add(int fd);
    void clear();
    int getFdMax();
    fd_set & getFdSet();
    /*
    int get_fd();
    fd_set* get_set();
    */
    BOOL isSet(int fd);
    int readSelect(int nTimeout);
    int writeSelect(int nTimeout);
public:
    chFD_SET();
    chFD_SET(int file);
    ~chFD_SET();
private:
    fd_set  m_vFDSET;
    int     m_nMaxFD;
};

//////////////////////////////////////////////////////////////////////////


};
using namespace ETL_BaseHelper;

#if IF_EXPORT_SYMBOLS
#include "_chBaseClass.cpp"
#endif

#endif //__chBASECLASS_H__
