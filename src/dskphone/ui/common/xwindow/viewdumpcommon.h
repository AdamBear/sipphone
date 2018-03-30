#ifndef __VIEW_DUMP_COMMON_H__
#define __VIEW_DUMP_COMMON_H__

#include <xwindow/xwindowdefine.h>
#include <ylstl/ylhashmap.h>
#include <xmlparser/xmlparser.hpp>


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define IF_VIEW_DUMP_DEBUG 0

#if IF_XWINDOW_DUMP_EXPORT
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum
{
    VIEW_DUMP_DATA_TYPE_STRING      = 0,            // 字符串，不需要再转换
    VIEW_DUMP_DATA_TYPE_INT         = 1,            // int型
    VIEW_DUMP_DATA_TYPE_LONG        = 2,            // long型
    VIEW_DUMP_DATA_TYPE_BOOL        = 3,            // bool型
    VIEW_DUMP_DATA_TYPE_RECT        = 4,            // chRect
    VIEW_DUMP_DATA_TYPE_WSTRING     = 5,            // yl::wstring
};

typedef bool (*FUN_DATA_FORMAT)(void* pData, yl::string& strData);

bool FormatIntData(void* pData, yl::string& strData);
bool FormatLongData(void* pData, yl::string& strData);
bool FormatBoolData(void* pData, yl::string& strData);
bool FormatRectData(void* pData, yl::string& strData);
bool FormatWStringData(void* pData, yl::string& strData);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class xView;
typedef yl::string(*FUN_GET_PROPERTY)(xView* pView);

class CViewDumpClassBase
{
public:
    explicit CViewDumpClassBase(const yl::string& strParentClass);
    virtual ~CViewDumpClassBase();

public:
    bool RegisterProperty(const yl::string& strName, FUN_GET_PROPERTY pFunGetProperty);
    void DumpProperty(xView* pView, xml_node& node);

    const yl::string& GetParentClass() const;

protected:
    yl::string m_strParentClass;

    typedef YLHashMap<yl::string, FUN_GET_PROPERTY> MAP_DUMP_PROPERTY;

    MAP_DUMP_PROPERTY m_mapProperty;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define DECLARE_VIEW_DUMP_CLASS(clsName) friend class CViewDumpClass_##clsName;
#define REGISTER_VIEW_DUMP_CLASS(clsName) m_nClassID = ViewDump_RegisterClass(#clsName + 1);

#define DECLARE_VIEW_DUMP_BEGIN(clsName) \
class CViewDumpClass_##clsName : public CViewDumpClassBase \
{ \
public: \
    CViewDumpClass_##clsName(); \
    virtual ~CViewDumpClass_##clsName() {} \
public: \
    static clsName* GetRealView(xView* pView) { return (clsName*)pView; }

#define DECLARE_VIEW_DUMP_PROPERTY_DATA(propertyName, clsData, datatype) \
static yl::string Get##propertyName(xView* pView) \
    { \
        yl::string strData; \
        if (NULL != pView) \
        { \
            ViewDump_FormatData(datatype, (void*)(&(GetRealView(pView)->clsData)), strData); \
        } \
        return strData; \
    }

#define DECLARE_VIEW_DUMP_PROPERTY_METHOD(propertyName, method) \
static yl::string Get##propertyName(xView* pView) \
    { \
        return GetRealView(pView)->method; \
    }

#define DECLARE_VIEW_DUMP_END \
};

#if IF_VIEW_DUMP_DEBUG
#define IMPLEMENT_VIEW_DUMP_BASE_BEGIN(clsName) \
    CViewDumpClass_##clsName::CViewDumpClass_##clsName() \
    : CViewDumpClassBase("") \
{ \
    printf("New CViewDumpClass:%p %s \n", this, #clsName + 1);

#define IMPLEMENT_VIEW_DUMP_BEGIN(clsName, parentName) \
CViewDumpClass_##clsName::CViewDumpClass_##clsName() \
: CViewDumpClassBase(#parentName + 1) \
{ \
    printf("New CViewDumpClass:%p %s ParentClass:%s \n", this, #clsName + 1, #parentName + 1);

#else
#define IMPLEMENT_VIEW_DUMP_BASE_BEGIN(clsName) \
    CViewDumpClass_##clsName::CViewDumpClass_##clsName() \
    : CViewDumpClassBase("") \
{

#define IMPLEMENT_VIEW_DUMP_BEGIN(clsName, parentName) \
CViewDumpClass_##clsName::CViewDumpClass_##clsName() \
: CViewDumpClassBase(#parentName + 1) \
{
#endif

#define IMPLEMENT_VIEW_DUMP_PROPERTY(propertyName) \
RegisterProperty(#propertyName, &Get##propertyName);

#define IMPLEMENT_VIEW_DUMP_END(clsName) \
    ViewDump_RegisterViewDumpClass(#clsName + 1, this); \
} \
CViewDumpClass_##clsName s_objViewDumpClass_##clsName;

#else
#define DECLARE_VIEW_DUMP_CLASS(clsName)
#define REGISTER_VIEW_DUMP_CLASS(clsName)
#define DECLARE_VIEW_DUMP_BEGIN(clsName)
#define DECLARE_VIEW_DUMP_PROPERTY_DATA(propertyName, clsData, datatype)
#define DECLARE_VIEW_DUMP_PROPERTY_METHOD(propertyName, method)
#define DECLARE_VIEW_DUMP_END

#define IMPLEMENT_VIEW_DUMP_BEGIN(clsName, parentName)
#define IMPLEMENT_VIEW_DUMP_PROPERTY(propertyName)
#define IMPLEMENT_VIEW_DUMP_END(clsName)
#endif

#endif
