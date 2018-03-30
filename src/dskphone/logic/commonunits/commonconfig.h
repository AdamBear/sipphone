#ifndef _COMMON_CONFIG_H
#define _COMMON_CONFIG_H

namespace commonUnits
{

class CCommonConfig
{
public:
    static CCommonConfig & GetInstance();

protected:
    CCommonConfig(void);

public:
    virtual ~CCommonConfig(void);

public:
    inline int GetEnableHebrew()        //获取西伯拉亚语配置
    {
        return m_iEnableHebrew;
    }

protected:
    int m_iEnableHebrew;        //西伯拉亚语配置


};



}

#define _CommonConfig commonUnits::CCommonConfig::GetInstance()     //获取实例引用


#define COMMONCONFIG_EnableHebrew _CommonConfig.GetEnableHebrew()       //获取西伯拉亚语配置


#endif
