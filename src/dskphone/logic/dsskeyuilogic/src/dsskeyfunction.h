#pragma once

#include <ylstring.h>
#include <ylvector.h>
#include "dsskey_define.h"
#include "language/common/common_function.h"

// 静态函数的基类
struct CCfgStaticFunctionBase
{
public:
    virtual bool OnCalled(const yl::string & strValue);

    virtual LPCSTR GetFlag()
    {
        return NULL;
    };

private:

    virtual bool operator()(vecString & veclist)
    {
        return false;
    };

    virtual LPCSTR GetSeparator()
    {
        return ",|";
    }
};

struct CCfgGetIntValueFunction
    : public CCfgStaticFunctionBase
{
    virtual LPCSTR GetFlag()
    {
        return XML_FUNC_GET_CFG_INT;
    }

    virtual bool operator()(vecString & veclist);

protected:
    virtual bool GetCfgValue(const yl::string & strFirstValue,
                             const yl::string & strSecondValue, const yl::string & strThirdValue);

    bool LogicOperate(int iLeft, int iRight, const yl::string & strOpType);
};

struct CCfgGetPhoneTypeFunction
    : public CCfgStaticFunctionBase
{
    virtual LPCSTR GetFlag()
    {
        return XML_FUNC_GET_PHONETYPE;
    }

    virtual bool operator()(vecString & veclist);
};

#ifdef IF_EXTERN_FUNCTION
struct CCfgGetStringValueFunction
    : public CCfgGetIntValueFunction
{
    virtual LPCSTR GetFlag()
    {
        return XML_FUNC_GET_CFG_STRING;
    }

protected:
    virtual bool GetCfgValue(const yl::string & strFirstValue,
                             const yl::string & strSecondValue, const yl::string & strThirdValue);

    template<typename T>
    bool LogicOperate(const T & tL, const T & tR, const yl::string & strOpType);
};

struct CCfgGetAccountNumFunction
    : public CCfgGetIntValueFunction
{
    virtual LPCSTR GetFlag()
    {
        return XML_FUNC_GET_ACCOUNT_NUM;
    }

protected:
    virtual bool GetCfgValue(const yl::string & strFirstValue,
                             const yl::string & strSecondValue, const yl::string & strThirdValue);
};

struct CCfgLogicORFunction
    : public CCfgStaticFunctionBase
{
    virtual LPCSTR GetFlag()
    {
        return XML_FUNC_LOGIC_OR;
    }

    virtual bool operator()(vecString & veclist);
};
#endif

struct CCfgLogicAndFunction
    : public CCfgStaticFunctionBase
{
    virtual LPCSTR GetFlag()
    {
        return XML_FUNC_LOGIC_AND;
    }

    virtual bool operator()(vecString & veclist);
};

struct CCfgLogicNotFunction
    : public CCfgLogicAndFunction
{
    virtual LPCSTR GetFlag()
    {
        return XML_FUNC_LOGIC_NOT;
    }

    virtual bool operator()(vecString & veclist);
};


int StrMap2Key(LPCSTR lpValue, const mapItem mapArr[], int iSize);
LPCSTR KeyMap2Str(int iKey, const mapItem mapArr[], int iSize);

#define StrMap2KeyEx(sstr, mapArr)  (StrMap2Key(sstr, mapArr, sizeof(mapArr)/sizeof(mapItem)))
#define KeyMap2StrEx(ikey, mapArr)  (KeyMap2Str(ikey, mapArr, sizeof(mapArr)/sizeof(mapItem)))

