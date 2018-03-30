#ifndef __BLF_CUSTOM_DATA_H__
#define __BLF_CUSTOM_DATA_H__

#include "blfcommon.h"
#include <ylstring.h>
#include <ylhashmap.h>
#include <xmlparser/xmlparser.hpp>

class CBLFCustomData
{
public:
    CBLFCustomData();
    virtual ~CBLFCustomData();

    // 获取状态
    yl::string GetStatus()
    {
        return m_strStatus;
    }

    // 获取优先级
    int GetPriority()
    {
        return m_nPriority;
    }

    // 自定义是否可用
    bool IsEnableCustom()
    {
        return m_bEnable;
    }

    // 获取灯信息
    yl::string GetLedInfo()
    {
        return m_strLed;
    }

    // 获取操作类型/值
    eBLFOperateType GetOperationType(const yl::string & strSessState);
    yl::string GetOperationValue(const yl::string & strSessState);

    // 解析自定义的数据
    bool ParseFromXmlNode(const xml_node & xmlNode);

private:
    // 将操作映射为枚举值
    eBLFOperateType MapDsskeyAction(const yl::string & strDsskeyAction);

private:
    // 唯一标识Key
    yl::string m_strStatus;

    // 是否启用自定义方式
    bool m_bEnable;

    // 优先级
    int m_nPriority;

    // 亮灯方式
    yl::string m_strLed;

    // 按键行为
    YLHashMap<yl::string, yl::string> m_mapDsskeyAction;

};

#endif
