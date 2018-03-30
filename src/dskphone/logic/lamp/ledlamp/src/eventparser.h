#ifndef _EVENTPARSERH_
#define _EVENTPARSERH_
#include <ylstring.h>
#include "ledlampstruct.h"

class CEventParser
{
public:
    /*
    解析xml文档
    @param:
    strFilePath: [in] xml文档路径
    _tEventMap:  [out] 解析内容存放
    */
    static bool Parse(const yl::string & strFilePath, MAPEVENTUNIT & _tEvent);

    //解析颜色值
    static bool ParseColor(const yl::string & strFilePath, MAPLEDCOLORVALUE & _tColorMap);

private:
    // 判断是否为可用的灯序号
    static bool BeAvaliableLampId(int * pLampId, int iLampId);
    // 解析灯序号
    static void ParseLampId(MPA_LED_COLOR & MapLedColor, const yl::string & strLampId,
                            const yl::string & strStatus);

    static LED_COLOR GetColor(const yl::string & strColor);
};

#endif
