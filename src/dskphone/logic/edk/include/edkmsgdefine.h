#ifndef __EDK_MSG_DEFINE_H__
#define __EDK_MSG_DEFINE_H__

enum EdkMessageDefine
{
    EDK_MSG_BEGIN = INNER_MSG_EDK_MESSAGE_BEGIN,
    EDK_MSG_LIGHTING,       // wParam:ExternalLightingType 亮灯类型 lParam:0 EX:亮灯规则数据
};

// 外部亮灯类型定义, 配合消息EDK_MSG_LIGHTING使用
// 注意: -1及以上值不能定义, 否则会和Dsskey id冲突
enum ExternalLightingType
{
    LIGHTING_EXP_FLIP = -2,     // EXP翻页灯
};

#endif // __EDK_MSG_DEFINE_H__
