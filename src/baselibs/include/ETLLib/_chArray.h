#ifndef __chARRAY_H__
#define __chARRAY_H__

#ifndef __ETL_H__
#error _chArray.h only can be include in ETL.h
#endif

#define ARRAY_ROWSIZE           4
namespace ETL_ArrayHelper
{
//////////////////////////////////////////////////////////////////////////
// Alloc & Free
EXTERN_DECLARE(BOOL base_alloc_buffer(LPVOID & m_aT, int & m_nAllocCount, int nNewCount,
                                      int sizeObj));
EXTERN_DECLARE(void base_free_buffer(LPVOID & m_aT, int & m_nAllocCount));

//////////////////////////////////////////////////////////////////////////
// chAlloc_Object
// alloc or realloc size * n for array
template <class typeObj>
class chAlloc_Object
{
public:
    //////////////////////////////////////////////////////////////////////////
    // Alloc & Free
    BOOL Alloc(int nCount)
    {
        return base_alloc_buffer((LPVOID &)m_aT, m_nAllocCount, nCount, sizeof(typeObj));
    }
    void Free()
    {
        base_free_buffer((LPVOID &)m_aT, m_nAllocCount);
    }
    typeObj * GetBuffer() const
    {
        return m_aT;
    }
    int GetCount() const
    {
        return m_nAllocCount;
    }
private:
    chAlloc_Object(chAlloc_Object & srcAlloc);
    chAlloc_Object & operator = (chAlloc_Object & srcAlloc);
public:
    chAlloc_Object()
    {
        m_aT = NULL;
        m_nAllocCount = 0;
    }
    ~chAlloc_Object()
    {
        chASSERT(m_aT == NULL && m_nAllocCount == 0);
    }
protected:
    typeObj    *    m_aT;
    int             m_nAllocCount;
};

// NOTE: content will be rand value because DESTRUCT_OBJECTS and CONSTRUCT_OBJECTS DONOT reset memory content
template <class typeObj, bool bClassType, int rowCount>
class chMeth_ObjArray : public chAlloc_Object<typeObj>
{
    typedef chAlloc_Object<typeObj>         type_obj_alloc;
    typedef chBlock<typeObj>                type_obj_block;
public:
    BOOL AppendAllocSize(int nAddCount)
    {
        chASSERT(nAddCount > 0);
        nAddCount = chCEIL(nAddCount, rowCount);
        return type_obj_alloc::Alloc(m_nElementCount + nAddCount);
    }

    BOOL SetArraySize(int nNewCount)
    {
        // 重新分配空间，新空间分配并构造，
        chASSERT(nNewCount >= 0);
        if (nNewCount < m_nElementCount)
        {
            if (bClassType)
            {
                DestructObjects<typeObj>(this->m_aT + nNewCount, m_nElementCount - nNewCount);
            }
            else
            {
                DESTRUCT_OBJECTS(typeObj, this->m_aT + nNewCount, m_nElementCount - nNewCount);
            }
        }
        else if (nNewCount == m_nElementCount)
        {
            return TRUE;
        }
        else if (nNewCount <= this->m_nAllocCount)
        {
            if (bClassType)
            {
                ConstructObjects<typeObj>(this->m_aT + m_nElementCount, nNewCount - m_nElementCount);
            }
            else
            {
                CONSTRUCT_OBJECTS(typeObj, this->m_aT + m_nElementCount, nNewCount - m_nElementCount);
            }
        }
        else // need new a larger size
        {
            int nAllocCount = chCEIL(nNewCount, rowCount);
            type_obj_alloc::Alloc(nAllocCount);
            if (bClassType)
            {
                ConstructObjects<typeObj>(this->m_aT + m_nElementCount, nNewCount - m_nElementCount);
            }
            else
            {
                CONSTRUCT_OBJECTS(typeObj, this->m_aT + m_nElementCount, nNewCount - m_nElementCount);
            }
        }
        m_nElementCount = nNewCount;
        return TRUE;
    }

    int GetSize() const
    {
        return m_nElementCount;
    }

    BOOL IsEmpty()
    {
        return m_nElementCount == 0;
    }
    // element operactor
    chMeth_ObjArray & Add(const typeObj & tObj)
    {
        int nOldSize = m_nElementCount;
        SetArraySize(nOldSize + 1);
        this->m_aT[nOldSize] = tObj;
        return *this;
    }
    typeObj & alloc_push_back()
    {
        int nOldSize = m_nElementCount;
        SetArraySize(nOldSize + 1);
        return this->m_aT[nOldSize];
    }
    chMeth_ObjArray & InsertAt(int iIndex, const typeObj & tObj)
    {
        chASSERT(iIndex >= 0 && iIndex <= m_nElementCount);
        int nOldSize = m_nElementCount;
        SetArraySize(nOldSize + 1);
        if (bClassType)
        {
            MoveObjects<typeObj>(this->m_aT + iIndex + 1, this->m_aT + iIndex, nOldSize - iIndex);
        }
        else
        {
            MOVE_OBJECTS(typeObj, this->m_aT + iIndex + 1, this->m_aT + iIndex, nOldSize - iIndex);
        }
        this->m_aT[iIndex] = tObj;
        return *this;
    }
    typeObj & InsertAt(int iIndex)
    {
        chASSERT(iIndex >= 0 && iIndex <= m_nElementCount);
        int nOldSize = m_nElementCount;
        SetArraySize(nOldSize + 1);
        if (bClassType)
        {
            MoveObjects<typeObj>(this->m_aT + iIndex + 1, this->m_aT + iIndex, nOldSize - iIndex);
        }
        else
        {
            MOVE_OBJECTS(typeObj, this->m_aT + iIndex + 1, this->m_aT + iIndex, nOldSize - iIndex) ;
        }
        return this->m_aT[iIndex];
    }
    chMeth_ObjArray & RemoveAt(int iIndex, int nRemoveCount)
    {
        chASSERT(iIndex >= 0 && iIndex <= m_nElementCount);

        if (iIndex >= 0 && iIndex < m_nElementCount && nRemoveCount != 0)
        {
            if (nRemoveCount < 0 || iIndex + nRemoveCount >= m_nElementCount)
            {
                // remove tail after iIndex
                nRemoveCount = m_nElementCount - iIndex;
                if (bClassType)
                {
                    DestructObjects<typeObj>(this->m_aT + iIndex, nRemoveCount);
                }
                else
                {
                    DESTRUCT_OBJECTS(typeObj, this->m_aT + iIndex, nRemoveCount);
                }
                m_nElementCount = iIndex;
            }
            else
            {
                int nMoveCount = m_nElementCount - iIndex - nRemoveCount;
                if (bClassType)
                {
                    DestructObjects<typeObj>(this->m_aT + iIndex, nRemoveCount);
                    MoveObjects<typeObj>(this->m_aT + iIndex, this->m_aT + iIndex + nRemoveCount, nMoveCount);
                }
                else
                {
                    MOVE_OBJECTS(typeObj, this->m_aT + iIndex, this->m_aT + iIndex + nRemoveCount, nMoveCount);
                    DESTRUCT_OBJECTS(typeObj, this->m_aT + iIndex + nMoveCount, nRemoveCount);
                }
                m_nElementCount -= nRemoveCount;
            }
        }
        return *this;
    }
    typeObj & operator [](const int iIndex) const
    {
        chASSERT(this->m_aT != NULL && iIndex >= 0 && iIndex < m_nElementCount);
        return this->m_aT[iIndex];
    }
    chMeth_ObjArray & Append(chMeth_ObjArray & refSrcObject)
    {
        return Append(refSrcObject.m_aT, refSrcObject.m_nElementCount);
    }
    chMeth_ObjArray & Append(typeObj * pData, int nCount)
    {
        if (pData != NULL && nCount > 0)
        {
            int nOldSize = m_nElementCount;
            SetArraySize(m_nElementCount + nCount);
            if (bClassType)
            {
                CopyObjects<typeObj>(this->m_aT + nOldSize, pData, nCount);
            }
            else
            {
                COPY_OBJECTS(typeObj, this->m_aT + nOldSize, pData, nCount);
            }
        }
        return *this;
    }
    void Clear()
    {
        SetArraySize(0);
    }
    void RemoveAll()
    {
        if (m_nElementCount > 0)
        {
            if (bClassType)
            {
                DestructObjects<typeObj>(this->m_aT, m_nElementCount);
            }
            else
            {
                DESTRUCT_OBJECTS(typeObj, this->m_aT, m_nElementCount);
            }
        }
        m_nElementCount = 0;
        type_obj_alloc::Free();
    }
    chMeth_ObjArray & FreeExtra()
    {
        if (m_nElementCount > 0)
        {
            type_obj_alloc::Alloc(m_nElementCount);
        }
        else
        {
            type_obj_alloc::Free();
        }
        return *this;
    }

    // detach & attach
    type_obj_block DetachBlock()
    {
        FreeExtra();
        type_obj_block tBlock(this->m_aT, m_nElementCount);
        this->m_nAllocCount = m_nElementCount = 0;
        this->m_aT = NULL;
        return tBlock;
    }
    chMeth_ObjArray & AttachBlock(type_obj_block srcBlock)
    {
        RemoveAll();
        this->m_aT = (typeObj *)srcBlock.get_block_buffer();
        m_nElementCount = this->m_nAllocCount = srcBlock.get_object_count();
        srcBlock.reset();
        return *this;
    }
    typeObj * Detach()
    {
        FreeExtra();
        typeObj * pData = m_nElementCount == 0 ? NULL : this->m_aT;
        this->m_nAllocCount = m_nElementCount = 0;
        this->m_aT = NULL;
        return pData;
    }
    chMeth_ObjArray & Attach(typeObj * pData, int nCount)
    {
        chASSERT(pData != NULL && nCount > 0);
        RemoveAll();
        this->m_aT = pData;
        m_nElementCount = this->m_nAllocCount = nCount;
        return *this;
    }
public:
    chMeth_ObjArray & CopyFromObject(const chMeth_ObjArray & srcArray)
    {
        SetArraySize(srcArray.m_nElementCount);
        if (srcArray.m_nElementCount != 0)
        {
            if (bClassType)
            {
                CopyObjects<typeObj>(this->m_aT, srcArray.m_aT, srcArray.m_nElementCount);
            }
            else
            {
                COPY_OBJECTS(typeObj, this->m_aT, srcArray.m_aT, srcArray.m_nElementCount);
            }
        }
        return *this;
    }
    chMeth_ObjArray & CutFromObject(chMeth_ObjArray & srcArray)
    {
        RemoveAll();
        this->m_aT = srcArray.m_aT;
        this->m_nAllocCount = srcArray.m_nAllocCount;
        m_nElementCount = srcArray.m_nElementCount;

        srcArray.m_aT = NULL;
        srcArray.m_nAllocCount = srcArray.m_nElementCount = 0;
        return *this;
    }

private:
    chMeth_ObjArray(const chMeth_ObjArray & srcArray);
    chMeth_ObjArray & operator = (const chMeth_ObjArray & srcArray);
public:
    chMeth_ObjArray()
    {
        m_nElementCount = 0;
    }
    ~chMeth_ObjArray()
    {
        RemoveAll();
    }
protected:
    int     m_nElementCount;
};


template <class typeObj, bool bClassType = true, int rowCount = ARRAY_ROWSIZE>
class chTempObjArray : public chMeth_ObjArray<typeObj, bClassType, rowCount>
{
//  typedef chObjArray<typeObj, bClassType, rowCount>   typeObjArray;
public:
    chTempObjArray()
    {
    }
    chTempObjArray(const chTempObjArray & srcArray)
    {
        this->CutFromObject(*(chTempObjArray *)&srcArray);
    }
private:
    chTempObjArray & operator = (const chTempObjArray & srcArray)
    {
        this->CutFromObject(*(chTempObjArray *)&srcArray);
        return *this;
    }
};

template <class typeObj, bool bClassType = true, int rowCount = ARRAY_ROWSIZE>
class chObjArray : public chMeth_ObjArray<typeObj, bClassType, rowCount>
{
    typedef chTempObjArray<typeObj, bClassType, rowCount>   typeTempObjArray;
public:
    chObjArray()
    {
    }
    chObjArray(const chObjArray & srcArray)
    {
        this->CopyFromObject(srcArray);
    }
    chObjArray & operator = (const chObjArray & srcArray)
    {
        this->CopyFromObject(srcArray);
        return *this;
    }
public:
    chObjArray(const typeObj & tObj, int nRepeat)
    {
        chASSERT(nRepeat > 0);
        this->SetArraySize(nRepeat);
        typeObj * pObject = this->m_aT;
        while (nRepeat--)
        {
            *(pObject++) = tObj;
        }
    }
    chObjArray(const typeObj * pData, int nSize)
    {
        chASSERT(pData != NULL && nSize > 0);
        this->SetArraySize(nSize);
        if (bClassType)
        {
            CopyObjects<typeObj>(this->m_aT, pData, nSize);
        }
        else
        {
            COPY_OBJECTS(typeObj, this->m_aT, pData, nSize);
        }
    }
public:
    chObjArray(const typeTempObjArray & srcTempArray)
    {
        this->CutFromObject((typeTempObjArray &)srcTempArray);
    }
    chObjArray & operator = (const typeTempObjArray & srcTempArray)
    {
        this->CutFromObject((typeTempObjArray &)srcTempArray);
        return *this;
    }
};

typedef chTempObjArray<BYTE, false, 256>    chTempByteArray;
typedef chObjArray<BYTE, false, 256>        chByteArray;

typedef chTempObjArray<WORD, false, 256>    chTempWordArray;
typedef chObjArray<WORD, false, 256>        chWordArray;

typedef chTempObjArray<DWORD, false, 64>    chTempDWORDArray;
typedef chObjArray<DWORD, false, 64>        chDWORDArray;

typedef chTempObjArray<int, false, 64>      chTempINTArray;
typedef chObjArray<int, false, 64>          chINTArray;

//////////////////////////////////////////////////////////////////////////
// Tone Gernrate
#define _2PI        6.283185307179586476925286766559l
#define _PI         3.141592653589793238462643383279l
#define _PI2        1.570796326794896619231321691639l

//////////////////////////////////////////////////////////////////////////
// class chToneMixer : public typeSample
template<int t_nSample, int t_nParams, class typeSample>
class chToneMixer : public typeSample
{
public:
    BOOL CDECL SetToneEx(double dFrom, double dTime, ...)
    {
        va_list argList;
        va_start(argList, dTime);
        for (int i = 0; i < t_nParams; i++)
        {
            m_aToneParam[i].dFrequency  = va_arg(argList, double);
            m_aToneParam[i].dPhasic     = va_arg(argList, double);
            m_aToneParam[i].dAmplitude  = va_arg(argList, double);
        }
        va_end(argList);
        return BuildData(dFrom, dTime, TRUE);
    }

    BOOL CDECL SetTone(int nTime_ms, ...)
    {
        va_list argList;
        va_start(argList, nTime_ms);
        for (int i = 0; i < t_nParams; i++)
        {
            m_aToneParam[i].dFrequency  = va_arg(argList, double);
            m_aToneParam[i].dPhasic     = va_arg(argList, double);
            m_aToneParam[i].dAmplitude  = va_arg(argList, double);
        }
        va_end(argList);
        return BuildData(0.0, nTime_ms / 1000.0, TRUE);
    }

    BOOL CDECL AppendToneEx(double dFrom, double dTime, ...)
    {
        va_list argList;
        va_start(argList, dTime);
        for (int i = 0; i < t_nParams; i++)
        {
            m_aToneParam[i].dFrequency  = va_arg(argList, double);
            m_aToneParam[i].dPhasic     = va_arg(argList, double);
            m_aToneParam[i].dAmplitude  = va_arg(argList, double);
        }
        va_end(argList);
        return BuildData(dFrom, dTime, FALSE);
    }

    BOOL CDECL AppendTone(int dTime_ms, ...)
    {
        va_list argList;
        va_start(argList, dTime_ms);
        for (int i = 0; i < t_nParams; i++)
        {
            m_aToneParam[i].dFrequency  = va_arg(argList, double);
            m_aToneParam[i].dPhasic     = va_arg(argList, double);
            m_aToneParam[i].dAmplitude  = va_arg(argList, double);
        }
        va_end(argList);
        return BuildData(0.0, dTime_ms / 1000.0, FALSE);
    }
    void AppendMute(int dTime_ms)
    {
        double dTime = dTime_ms / 1000.0;
        int nNewSize = (int)(dTime * t_nSample + 1);
        typeSample::AppendMuteSample(nNewSize);
    }
private:
#define FLOAT_ZERO      1.0e-6
    BOOL BuildData(double tFrom, double dTime, BOOL bClearLast)
    {
        chASSERT(tFrom > -FLOAT_ZERO && fabs(dTime) > FLOAT_ZERO);
        if (bClearLast)
        {
            typeSample::Clear();
        }
        double tEnd = tFrom + dTime;

        tFrom = mathNear(tFrom, 1.0 / t_nSample, FLOAT_ZERO);
        tEnd = mathNear(tEnd, 1.0 / t_nSample, FLOAT_ZERO);

        int nNewSize = mathRound((tEnd - tFrom) * t_nSample, 0.5);
        typeSample::AllocSampleBuffer(nNewSize);

        for (int i = 0; i < nNewSize; i++)
        {
            double dA = 0;
            for (int j = 0; j < t_nParams; j++)
            {
                double dAlpha = m_aToneParam[j].dFrequency * (tFrom + i / (double)t_nSample) * _2PI +
                                m_aToneParam[j].dPhasic;
                dA += m_aToneParam[j].dAmplitude * sin(dAlpha);
            }
            typeSample::AppendSample(dA / t_nParams);
        }
        return TRUE;
    }
public:
    chToneMixer()
    {
        for (int i = 0; i < t_nParams; i++)
        {
            m_aToneParam[i].dFrequency  = 0.0;
            m_aToneParam[i].dPhasic     = 0.0;
            m_aToneParam[i].dAmplitude  = 0.0;
        }
    }
private:
    struct TONE_PARAM
    {
        double dFrequency;
        double dPhasic;
        double dAmplitude;
    } m_aToneParam[t_nParams];
};

//////////////////////////////////////////////////////////////////////////
// class aLawSample : public chObjArray<SHORT, false, 4096>
hide_class aLawSample :
public chObjArray<SHORT, false, 4096>
{
    typedef chObjArray<SHORT, false, 4096>          type_Array;
public:
    void AppendMuteSample(int nCount);
    void AllocSampleBuffer(int nAllocSize);
    void AppendSample(double fValue);
    int GetBufferSize();
    void Clear();
};

//////////////////////////////////////////////////////////////////////////
// class uLawSample : public chObjArray<BYTE, false, 4096>
hide_class uLawSample :
public chObjArray<BYTE, false, 4096>
{
    typedef chObjArray<BYTE, false, 4096>           type_Array;
public:
    void AppendMuteSample(int nCount);
    void AllocSampleBuffer(int nAllocSize);
    void AppendSample(double fValue);
    int GetBufferSize();
    void Clear();
};

//////////////////////////////////////////////////////////////////////////
// class chULawWave : public chToneMixer<t_nSample, t_nParams, uLawSample>
template<int t_nSample, int t_nParams>
class chULawWave : public chToneMixer<t_nSample, t_nParams, uLawSample>
{
};

template<int t_nSample, int t_nParams>
class chALawWave : public chToneMixer<t_nSample, t_nParams, aLawSample>
{
};

};
using namespace ETL_ArrayHelper;

#if IF_EXPORT_SYMBOLS
#include "_chArray.cpp"
#endif

#endif //__chARRAY_H__
