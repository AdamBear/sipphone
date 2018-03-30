#ifndef __Q_PUSH_BUTTON_UTILITY_H__
#define __Q_PUSH_BUTTON_UTILITY_H__

#include <ETLLib/ETLLib.hpp>
#include <QPushButton>

class QPushButtonUtility
{
public:
    QPushButtonUtility(void);
    ~QPushButtonUtility(void);

public:
    // 设置Btn没有背景框
    static void SetButtonNoFrame(QPushButton * pBtn);

    // 设置Btn上的突破
    static void SetButtonIcon(QPushButton * pBtn, LPCSTR lpcsIcon);

    // 设置Btn透明
    static void SetButtonTransparent(QPushButton * pBtn);
};

#endif
