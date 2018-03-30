#ifndef __chDEFINE_H__
#define __chDEFINE_H__

#ifndef __ETL_H__
#error _chDefine.h only can be include in ETL.h
#endif

#if WINDOWS_SYSTEM
#define DECLARE_EXPORT __declspec(dllexport)
#define DECLARE_IMPORT __declspec(dllimport)
#define PRINTF_FORMAT_STRING_FLAG _Printf_format_string_
#define PRINTF_FORMAT(formatIndex, argListIndex)
#else
#define DECLARE_EXPORT
#define DECLARE_IMPORT
#define PRINTF_FORMAT_STRING_FLAG
#define PRINTF_FORMAT(formatIndex, argListIndex) __attribute__((format(printf, formatIndex, argListIndex)));
#endif

//*/
#define hide_namespace          namespace
#define hide_class              class
#define hide_struct             struct
#define hide_typedef            typedef

#define DEFINE_HANDLE(name)                     struct name##__ { int unused; }; typedef struct _##name##_ *name

#define DECLARE_ALIAS(base, name, alias)        typedef base::name  alias
#define DECLARE_TYPE(typeName, alias)           typedef typeName    alias

#define EXTERN_DECLARE(type_name)               extern type_name
#define EXTERN_DECLARE_C(type_name)             extern "C" type_name
#define EXTERN_DECLARE_CPP(type_name)           extern type_name

#define UNUSED_ALWAYS(x)                        x

EXTERN_DECLARE(const double doubleMAX);
EXTERN_DECLARE(const double doubleMIN);
EXTERN_DECLARE(const double doubleINF);
EXTERN_DECLARE(const double doubleQNAN);

#define POINTER             uintptr_t
#define MAX_POINTER         (~(POINTER)0)
#define MAX_UINT            (~(UINT)0)
#define MIN_INT             (1 << (sizeof(int)*8-1))
#define MAX_INT             (~MIN_INT)
#define MAX_UINT_PTR        (~(UINT_PTR)0)
#define MIN_INT_PTR         (1 << (sizeof(INT_PTR)*8-1))
#define MAX_INT_PTR         (~MIN_INT_PTR)
#define NO_RESULT           ((LRESULT)-1)
#define NEVER_TIMEOUT       ((int)-1)

//////////////////////////////////////////////////////////////////////////
// globle object
#define REFERENCE_GLOBLE_OBJECT(type)       type##getGlobalObject()
#define DECLARE_GLOBLE_OBJECT(type)         type& REFERENCE_GLOBLE_OBJECT(type)
#define IMPLEMENT_GLOBLE_OBJECT(type)       DECLARE_GLOBLE_OBJECT(type) \
                                                        { \
                                                            static type* pObject = NULL; \
                                                            if(pObject == NULL) \
                                                            { \
                                                                pObject = (type*)MAX_POINTER; \
                                                                static type onlyGlobalObject; \
                                                                pObject = &onlyGlobalObject; \
                                                            } \
                                                            while(pObject == (type*)MAX_POINTER) \
                                                            { \
                                                                chWARNING2(FALSE, "globle object constructting !"); \
                                                                etlSwitchToThread(); \
                                                            } \
                                                            return *pObject;\
                                                        }
#define ASSERT_GLOBAL_OBJECT()              static void* pData = NULL; chASSERT(pData == NULL); pData = this

////////////////////////////// chINRANGE Macro ////////////////////////////////
// This macro returns TRUE if a number is between two others
#define chINRANGE(low, Num, High)       (((low) <= (Num)) && ((Num) <= (High)))

#define chMAX(a, b)                     ((a)>(b)?(a):(b))
#define chMIN(a, b)                     ((a)<(b)?(a):(b))
#define chMID(a, b, c)                  ((a)>(b)?((b)>(c)?(b):chMIN(a,c)):((a)>(c)?(a):chMIN(b,c)))

#define chCHECK_BITS(u, flag)           (((u) & (flag)) == (flag))
#define chSET_BITS(u, mask, bSet)       ((bSet) ? ((u) |= (mask)) : ((u) &= ~(mask)))

#define _B4(b0,b1,b2,b3)                (((b0)<<0)|((b1)<<1)|((b2)<<2)|((b3)<<3))
#define _B16(b0,b1,b2,b3)               (((b0)<<0)|((b1)<<4)|((b2)<<8)|((b3)<<12))

#define chTEXT2DWORD(s0, s1, s2, s3)    (((DWORD)(BYTE)(s0))<<0) | (((DWORD)(BYTE)(s1))<<8) | (((DWORD)(BYTE)(s2))<<16) | (((DWORD)(BYTE)(s3))<<24)

#define chMAKEWORD(lb, hb)              ((WORD)(((BYTE)(lb)) | ((WORD)((BYTE)(hb))) << 8))
#define chMAKEDWORD(lw, hw)             ((DWORD)(((WORD)(lw)) | ((DWORD)((WORD)(hw))) << 16))
#define chMAKELONG                      chMAKEDWORD

#define chLOWORD(l)                     ((WORD)(l))
#define chHIWORD(l)                     ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#define chLOBYTE(w)                     ((BYTE)(w))
#define chHIBYTE(w)                     ((BYTE)(((WORD)(w) >> 8) & 0xFF))

#define chCEIL(n, row)                  (((n)+(row)-1) - ((n)+(row)-1) % (row))
#define chFLOOR(n, row)                 ((n) - ((n) % (row)))

#define TYPE2UINT(t)                    ((~(0xFFFFFFFF << (sizeof(t) * 8))) & (t))

#define castTYPE(type, value)           ((type)(value))

//////////////////////////////////////////////////////////////////////////
/* address offset map */
#define castOBJECT(pMember, group, member)      (group*)((POINTER)pMember - (POINTER)(&((group*)pMember)->member) + (POINTER)pMember)

//////////////////////////////// chLENOF Macro ////////////////////////////////
/*This macro evaluates to the number of elements in an array. */
#define chLENOF(Array) (int)(sizeof(Array) / sizeof(Array[0]))

#define textSAME(s1, s2) (ACC::chStrcmp(s1, s2) == 0)
#define textSAME_NC(s1, s2) (ACI::chStrcmp(s1, s2) == 0)

#define textBEGIN(lpszText, lpszSub) (ACC::chIsBeginText(lpszText, lpszSub))
#define textBEGIN_NC(lpszText, lpszSub) (ACI::chIsBeginText(lpszText, lpszSub))

#define textEMPTY(str) ((str) == NULL || *(str) == '\0')

#define ZERO_OBJECT(obj) memset(&obj, 0, sizeof(obj))

///////////////////////////////////////////////////////////////////////////////
/* DebugBreak Improvement for x86 platforms */
#ifdef _X86_
#define DebugBreak()    _asm { int 3 }
#endif

//////////////////////////// Assert/Verify Macros /////////////////////////////
/* Put up a message box if an assertion fails in a debug build. */

#define chASSERT(expr) if(expr){}
#define chASSERT2(expr, msg) if(expr){}

#define chWARNING(expr)         if(expr){}
#define chWARNING2(expr, msg)   if(expr){}


#if IF_DEBUG_ENABLE
#define MEMSETD(pData, value, len)      memset(pData, value, len)
#else
#define MEMSETD(pData, value, len)
#endif

#define DEBUG_CONSTRUCT(type, pData, nSize)      MEMSETD(pData, 0x00, (nSize)*sizeof(type))
#define DEBUG_DESTRUCT(type, pData, nSize)       //MEMSETD(pData, 0xDD, (nSize)*sizeof(type))

#if IF_DEBUG_ENABLE
#define chTEST(expr, value) do{ UINT _ret_ = expr;\
     if(_ret_ != value)\
     {\
         yl::string strMsg = fmtString("%s => %.8X", #expr, _ret_);\
         etlWarningMessage(FALSE, __FILE__, __LINE__, strMsg);\
     }\
 }while(0)
#else
#define chTEST(expr, value) expr
#endif

/* Assert in debug builds, but don't remove the code in retail builds. */
#if IF_DEBUG_ENABLE
#define chVERIFY(expr) chASSERT(expr)
#else
#define chVERIFY(expr) expr
#endif

// for UNICODESUF and DEBUGSUF
#if defined(_UNICODE) || defined(UNICODE)
#define UNICODE_SUF         "u"
#else
#define UNICODE_SUF         ""
#endif

#if IF_DEBUG_ENABLE
#define DEBUG_SUF           "d"
#else
#define DEBUG_SUF           ""
#endif

#if LINUX_SYSTEM
#include "xInc/xDefine.h"
#else
#include "wInc/wDefine.h"
#endif


#if IF_EXPORT_SYMBOLS
#include "_chDefine.cpp"
#endif
#endif // __chDEFINE_H__
