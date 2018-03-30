#ifndef __EDK_LEDCONTROL_ACTION_H__
#define __EDK_LEDCONTROL_ACTION_H__
#include "ylhashmap.h"
#include "edkactionbase.h"
#include "../include/edk_def.h"

typedef YLHashMap<int, LedActionData> MAP_ID_TO_LED_ACTION;
typedef MAP_ID_TO_LED_ACTION::iterator MAP_ID_TO_LED_ACTION_ITER;


class CEdkLedControlAction :
    public CEdkActionBase
{
public:
    CEdkLedControlAction(void);
public:
    ~CEdkLedControlAction(void);

    virtual bool DoAction(int iActionId, edk_action_data_t & tEdkAction, bool & bResult);

    // 处理消息
    virtual bool OnMessage(msgObject & msg);

    virtual bool ParseAction(edk_action_data_t & tEdkAction);

    // 解析灯的状态
    bool ParseLedRule(edk_action_data_t & edkAction, LedActionData & objLedInfo);

    // 获取dsskey的显示数据
    bool GetDsskeyDisDataById(int iDsskeyId, EdkCtrlDsskeyDispData & objDisplayData);

    // 删除数据
    bool RemoveData(int iDsskeyId, bool bLightOff = false);

private:
    // 解析灯亮规则
    bool ParseLedRule(const yl::string & strContect, LedActionData & objLedData);

    // 获取灯规则数据
    bool GetLedActionData(UINT iTimerId, MAP_ID_TO_LED_ACTION_ITER & iterLedAction);

    // 保存到map
    bool Save2Map(LedActionData & objLedAction);

    // 点灯或亮图标
    bool LightCtrl(int iDsskeyId, bool & bRemove);

    // 获取灯颜色
    bool GetLedColorType(const yl::string & strColor, COLOR_TYPE & eType);

    // 更新灯或dsskey的图标，label
    void UpdateDsskey(int iDsskeyId, LedRuleData & objLedRule, DsskeyExpRefresh eFlushType);

    // 定时器处理
    static LRESULT OnTimer(msgObject & msg);

    static LRESULT OnDsskeyAttrChange(msgObject & msg);

    // 定时器回来处理
    bool ProcessTimerBack(UINT uTimerId);

    // 外部亮灯类型是否支持
    bool IsExternalLightingTypeSupported(int iType);

private:

    // dsskeyid 对应的闪烁规则
    MAP_ID_TO_LED_ACTION m_mapLedAction;

    int m_iUpdateTimer;

};
#endif // __EDK_LEDCONTROL_ACTION_H__
