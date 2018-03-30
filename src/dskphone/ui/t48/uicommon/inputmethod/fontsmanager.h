#pragma once
#include "fontbase.h"
#include "fontoutput.h"
#include <QString>
#include <map>

using namespace std;


class CFontsManager : public QObject
{
public:
    CFontsManager(void);
    ~CFontsManager(void);

public:
    bool SetFontByType(string strFont);

    QString GetCurrFontName();

    QString GetCurrFontTitle();

    QString GetFontTitleByName(std::string strName);

    bool DealKeyPress(QObject *pObj, int iKeyCode);

    bool DealKeyRelease(QObject *pObj, int iKeyCode);

    void OnBackspaceClicked(QWidget* pWidget);

    //处理控件失去光标事件
    void DealFocusOut();

    //处理控件获得光标事件
    void DealFocusIn();
    // 处理鼠标点击事件
    void DealMousePress();
    // 获取keycode对应的第一个字符
    QString GetIMEFirstChar(QString lpszIMEName, int iKeyCode);
    // 立即输入当前预输入字符
    void EnterEditData(QWidget* pWidget);

    void ReloadIMEFile();

    bool InputPreeditContent(QWidget* pWidget);
    bool IsPreedit(QWidget* pWidget);

private:
    // 初始化
    void Init();
    // 释放
    void Release();
    // 初始化字库对象
    void InitFontsObj();
    // 根据字库名称创建字库对象
    CFontBase*  CreatFontObjByType(string strFont);

private:
    // 启动计时器
    void startTimer();

    // 停止计时器
    void stopTimer();
    // 定时器事件
    void timerEvent(QTimerEvent* pEvent);
    // 处理删除功能
    bool DeleteEditData(QWidget* pWidget);

    bool DealDirectionKey(QObject *pObj, int iKeyCode);

private:
    CFontBase*          m_pCurrFont;
    std::map<std::string, CFontBase*> m_mapFonts;
    int                 m_iLastKeyCode;

private:
    //定时器
    int m_nTimerID;
    CFontOutput m_FontOutput;
};
