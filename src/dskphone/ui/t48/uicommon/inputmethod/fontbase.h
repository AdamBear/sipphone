#pragma once
#include <QObject>
#include "datastruct.h"
#include "imeproxy.h"

class CFontBase : public QObject
{
    Q_OBJECT

public:
    CFontBase(void);
    virtual ~CFontBase(void);

public:
    //根据输入Keycode获取输出
    virtual QString GetFontData(QObject* pObject, int iKeyCode, int iKeyboardStatus)        = 0;

    virtual bool DealKeyRelease(QObject* pObject, int iKeyCode);

public:
    //获取keycode为iKeyCode的第一个字符
    virtual QString GetFirstChar(int iKeyCode);
    // 是否是需要支持预输入
    virtual bool IsPerInput(int iKeyCode);
    // 初始化
    virtual void Init();
    // 释放
    virtual void Release();

    virtual void InitData() = 0;
public:
    //Name是该输入法名称  title是该输入法显示的名称
    virtual void SetName(QString _qstrName);
    virtual void SetTitle(QString _qstrTitle);
    //获取输入法名称、显示名称
    virtual QString GetName();
    virtual QString GetTitle();

public:
    //加载文件数据
    virtual void LoadIMEFile();


private:
    QString m_qstrName;
    QString m_qstrTitle;

};


