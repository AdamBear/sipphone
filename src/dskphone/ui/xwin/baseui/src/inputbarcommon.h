#ifndef __INPUT_BAR_COMMON_H__
#define __INPUT_BAR_COMMON_H__

#include "interfacedefine.h"

// type需要连续定义
enum INPUTBAR_TYPE
{
    BAR_CANDIDATE = 0,
    BAR_INPUT = 1,
    BAR_MAX = 2,
};
#define BAR_ITEM_COUNT 5

extern const char * INPUTBAR_CHAR_ITEM_WIDTH;
extern const char * INPUTBAR_IME_ITEM_WIDTH;
extern const char * INPUTBAR_SOFTBAR_HEIGHT;
extern const char * INPUTBAR_BACKGROUND_COLOR_FOCUS;
extern const char * INPUTBAR_BACKGROUND_COLOR_NORMAL;
extern const char * INPUTBAR_TEXT_COLOR_FOCUS;
extern const char * INPUTBAR_TEXT_COLOR_NORMAL;
extern const char * INPUTBAR_FRAME_COLOR;
extern const char * INPUTBAR_FRAME_VISIBLE;

#endif // #ifndef __INPUT_BAR_COMMON_H__
