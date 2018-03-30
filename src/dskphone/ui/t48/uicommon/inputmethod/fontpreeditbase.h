#pragma once

#include "fontbase.h"

class CFontPreeditBase: public CFontBase
{
public:
    CFontPreeditBase(void);
    virtual ~CFontPreeditBase(void);

public:
    //按了几次计数
    int m_iAllPressTimes;

public:

    //根据输入Keycode获取输出
    virtual QString GetFontData(QObject* pObject, int iKeyCode, int iKeyboardStatus);
    // 初始化
    virtual void Init();

    virtual void InitData() = 0;
    // 释放
    virtual void Release();

    // 获取第一个字符
    virtual QString GetFirstChar(int iKeyCode);
    // 是否需要支持预输入
    virtual bool IsPerInput(int iKeyCode);

    void    ResetPressTimes();

protected:
    // 获取按键对应的字符
    virtual QString DealPreeditKeyCode(int iKeyCode, QString qstrArry);

protected:
    QString m_qstrArry0;
    QString m_qstrArry1;
    QString m_qstrArry2;
    QString m_qstrArry3;
    QString m_qstrArry4;
    QString m_qstrArry5;
    QString m_qstrArry6;
    QString m_qstrArry7;
    QString m_qstrArry8;
    QString m_qstrArry9;
    QString m_qstrArry10;
    QString m_qstrArry11;

};
