#ifndef _IMPLEMENT_PERFORMANCE_LINE_H_
#define _IMPLEMENT_PERFORMANCE_LINE_H_
#include <ETLLib.hpp>
#include <ylstl/ylstring.h>

#include "dsskeyui/include/dsskeyuicommondefine.h"
#include "impldsskeyperformance.h"

enum Line_Mode
{
    LINE_MODE_NORMAL = 0,
    LINE_MODE_ACD,
    LINE_MODE_SCA
};

class  ImplDsskeyPerformanceLine : public ImplDsskeyPerformanceBase
{
public:
    ImplDsskeyPerformanceLine();

    virtual ~ImplDsskeyPerformanceLine();

public:
    // 获取显示内容前的操作
    virtual bool GetPerformanceBegin(const DssKeyLogicData & logicData);

    // 获取文字显示
    virtual bool GetLabelPerformance(const DssKeyLogicData & logicData, DsskeyLabelData & LabelData);

    // 获取图标显示
    virtual bool GetIconPerformance(const DssKeyLogicData & logicData, DsskeyIconData & IconData);

    // 获取灯显示
    virtual bool GetLightPerformance(const DssKeyLogicData & logicData, DsskeyLightData & LigntData);

    // 获取背景显示
    virtual bool GetBkgroundPerformance(const DssKeyLogicData & logicData, DsskeyBkgroundData & BkData);

private:
    // 处理账号功能的ACD图标显示
    void Process_LineACDIcon(DsskeyIconData & objDisplayData);
    // 处理普通账号功能的图标显示
    void Process_LineNormalIcon(DsskeyIconData & objDisplayData, const DssKeyLogicData & sLogicData);
    // 处理SCA账号功能的图标显示
    void Process_LineSCAIcon(DsskeyIconData & objDisplayData, const DssKeyLogicData & sLogicData);
    // 处理SCA账号功能的图标显示
    void Process_LineUCIcon(DsskeyIconData & objDisplayData, const DssKeyLogicData & sLogicData);
    // 处理SCA账号功能的文本显示
    void Process_LineSCALabel(DsskeyLabelData & objDisplayData, const DssKeyLogicData & sLogicData);
    // 处理账号功能的SCA状态灯显示
    void Process_LineSCALight(DsskeyLightData & LightData, const DssKeyLogicData & sLogicData);

private:
    Line_Mode m_lineMode;
    int m_eSCAStatus;
    bool m_bSCA_Hold;
    bool m_bSCA_Local;
    int m_eTalkStatus;
};


#endif //_IMPLEMENT_PERFORMANCE_LINE_H_
