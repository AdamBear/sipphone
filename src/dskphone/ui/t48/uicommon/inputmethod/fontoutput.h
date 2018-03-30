#pragma once
#include <qwindowsystem_qws.h>

#if LINUX_SYSTEM
#include <QWSInputMethod>
#endif

#if LINUX_SYSTEM
class CFontOutput : public QWSInputMethod
#else
class CFontOutput
#endif
{
public:
    CFontOutput(void);
    ~CFontOutput(void);
    // 确定字符显示
    bool SubmitCommitData(QWidget* pWidget, QString strInput);
    bool SubmitCurrentCommitData(QWidget* pWidget);

    // 发送预编辑字符
    bool SubmitCurrentPreeditData(QWidget* pWidget, QString strInput);
    // 处理删除按键功能
    bool DealBackspace(QWidget* pWidget);
    // 预输入字符是否为空
    bool IsPreeditEmpty();
    // 判断输入字符是否与预输入字符相同
    bool IsOutStrEqual(QString strInput);

protected:
    bool sendPreeditString(QWidget* pKeyWidget, const QString& preeditString, int cursorPosition, int selectionLength = 0);
    bool sendCommitString(QWidget* pKeyWidget, const QString& commitString, int replaceFrom = 0, int replaceLength = 0);

private:
    QString m_strPreeditData;
};
