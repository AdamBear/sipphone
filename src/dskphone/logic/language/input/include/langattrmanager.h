#pragma once

#include "language/common/lang_config.h"
#include "language/common/singletonbase.h"
#include "language/common/lang_common.h"

enum LANG_ATTR
{
    IA_NONE = 0,
    IA_SECTION          = 0x01 << 0, // 是否是一个区间
    IA_ULLTI_SECTION    = 0x01 << 1, // 多重区间
    IA_FEATURE_CODE     = 0x01 << 2, // 特征字符串

    IA_CASE          = 0x01 << 5,
    IA_IME           = 0x01 << 6, // 输入法支持
    IA_IME_BUILDIN   = 0x01 << 7, // 是否内置输入法，类似韩语

    IA_RTL          = 0x01 << 10,
    IA_LIGATURES    = 0x01 << 11, // 阿拉伯语连写

    IA_DATE_BASIC    = 0x01 << 15,
    IA_DATE_CHINESE  = 0x01 << 16,
};

struct CLangAttr
{
    LPCTSTR strName; // 对应ime.xml文件中的名字
    LPCTSTR strSetName;
    const UINT  iAttr;   // 属性
    const UINT  Rang[2];
};

extern const CLangAttr s_imeAttr[];

class CLangAttrManager
    : public SingletonBase<CLangAttrManager>
{
public:

    CLangAttrManager();
    ~CLangAttrManager();

    void Reload();

    // 统计出现次数
    void CalRefCount(const yl::string & strCode);

    bool CreatDefaultLocation();

    LPCTSTR GetName()
    {
        return s_imeAttr[m_iFocus].strName;
    }

    LPCTSTR GetSetName()
    {
        return s_imeAttr[m_iFocus].strSetName;
    }

    UINT GetLangAttr()
    {
        return s_imeAttr[m_iFocus].iAttr;
    }

    bool IsInRang(UINT iValue, int iIndex = -1);
private:
    UINT          *         m_pRefCount;
    int                     m_iFocus;
};

#define _LangAttrManager GET_SINGLETON_INSTANCE(CLangAttrManager)
