#ifndef __IME_COMMON_H__
#define __IME_COMMON_H__

#include <imeedit/imeeditdefine.h>

class CImeInputer;
class CImeTarget;

typedef CImeInputer* (*FUN_CREATE_INPUTER)(CImeTarget* pTarget);
typedef bool (*FUN_RELEASE_INPUTER)(CImeInputer* pInputer);
typedef bool (*FUN_INSTALL_IME)(CImeTarget* pTarget, const char* lpszIMEName);

extern const char* IME_ATTR_IME;
extern const char* IME_ATTR_IME_SOFTKEY;
extern const char* IME_ATTR_DELETE_SOFTKEY;
extern const char* IME_ATTR_ENABLED;
extern const char* IME_ATTR_PASSWORD;
extern const char* IME_ATTR_SUDDEN_PASSWORD;
extern const char* IME_ATTR_MAX_LENGTH;
extern const char* IME_ATTR_MAX_BYTE;
#if IF_EDITTEXTVIEW_SELECT_ENABLE
extern const char* IME_ATTR_SELECT_BG_COLOR;
extern const char* IME_ATTR_SELECT_TEXT_COLOR;
#endif
#if IF_EDIT_TEXT_VIEW_HINT_EMPTY
extern const char* IME_ATTR_HINT_TEXT;
extern const char* IME_ATTR_HINT_COLOR;
extern const char* IME_ATTR_HINT_SIZE;
#endif

#endif
