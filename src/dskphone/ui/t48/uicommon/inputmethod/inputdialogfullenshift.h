#ifndef _INPUTMETHOD_INPUTDIALOGFULLENSHIFT_H
#define _INPUTMETHOD_INPUTDIALOGFULLENSHIFT_H

#pragma once
#include "baseinputdlg.h"
#include "inputbutton.h"
#include "longpressbutton.h"

class CInputDialogFullEnShift: public CBaseInputDlg
{
    Q_OBJECT
public:
    CInputDialogFullEnShift(void);
    virtual ~CInputDialogFullEnShift(void);

    // 获取全键盘类型
    INPUT_DLGTYPE GetDlgType();

    virtual void SetInputDlgStyle(INPUT_DIALOG_STYLE enStyle);

protected:
    // 加载背景图等
    void LoadBkGroundPic();
    // 创建全键盘按键
    void NewButton();
};

#endif
