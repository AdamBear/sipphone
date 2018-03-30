#ifndef __ELEMENT_CONFIG_H__
#define __ELEMENT_CONFIG_H__
#include "dsskey/include/dsskey_enumtypes.h"

#ifdef IF_FEATURE_XMLBROWSER

// Element Config类
class CElementConfig : public CElementBase
{
public:
    CElementConfig();
    virtual ~CElementConfig();

    // 创建之后的处理
    virtual bool AfterCreate();
    // 销毁之前的处理
    virtual void BeforeDestroy();
    // 是否播放提示音
    virtual bool IsNeedPlayBeep();

#if IF_BUG_25845
    // 定时器响应
    static bool OnTimer(UINT uTimerID);
#endif

private:
    // 修改配置
    void ModifyConfig();

    // 判断Dsskey配置是否已改变，如果改变才保存
    bool IsDsskeyConfigChange(const yl::string & strItem);

    // 解析字符串：expansion_module.1.key.1.label=Keine Funktion
    bool ParseDsskeyItemData(const yl::string & strItem,
                             DSSKEY_MODULE_TYPE & eDsskeyModuleType,
                             int & nExpIndex,
                             int & nDsskeyIndex,
                             yl::string & strKey,
                             yl::string & strValue);

#if IF_FEATURE_XMLBROWSER_HOTELING
    void ShowTips();
#endif

    IMPLEMENT_GETXDATA(XBConfigData);

private:
    // 保存属性
    // ConfigAttr    m_dataAttr;
#if IF_BUG_25845
    static yl::string m_strAutopUrl;
#endif
};

#endif // #ifdef IF_FEATURE_XMLBROWSER

#endif  // __ELEMENT_CONFIG_H__
