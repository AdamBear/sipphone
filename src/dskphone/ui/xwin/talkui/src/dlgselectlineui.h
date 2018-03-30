#ifndef _DLG_SELECTLINE_UI_H_
#define _DLG_SELECTLINE_UI_H_

#include "dlgtalkmenubase.h"
#include "lineselectadapter.h"

namespace talkui
{
class LineSelectResult
{
public:
    virtual void OnLineSelectResult(LINE_SELECT_RESULT eResult, int nSelectIndex, int nSelectID) = 0;
};

// 进入选择界面
bool talkui_OpenSelectPage(int nCallerID, LineSelectResult * pSelectResult, vecLineSelectData & vecLineData,
                           yl::string strTitle, bool bShowIndex, int nSelectID);

// 退出选择界面
void talkui_ExitSelectPage();

struct tagAccountData
{
    //账号ID
    int m_iAccountID;
    //账号显示的文字
    yl::string m_strAccountText;

    tagAccountData()
    {
        Reset();
    }

    void Reset()
    {
        //账号ID
        m_iAccountID = -1;
        //账号显示的文字
        m_strAccountText = "";
    }

    tagAccountData & operator=(const tagAccountData & data)
    {
        if (this == &data)
        {
            return *this;
        }

        this->m_iAccountID = data.m_iAccountID;
        this->m_strAccountText = data.m_strAccountText;

        return *this;
    }
};

typedef struct tagAccountData CAccountData;

class CDlgSelectLineUI : public CDlgTalkMenuBase
{
public:
    //取得实例
    static CBaseDialog * CreateInstance();

public:
    CDlgSelectLineUI();
    virtual ~CDlgSelectLineUI();

public:
    // 初始化
    virtual void Init();

    // 释放数据，将窗.口清空回复到初始化状态
    virtual void Uninit();

    virtual void SetData(void * pData) {}

    virtual bool IsRejectReturnToIdle()
    {
        return false;
    }

public:
    // 设置SessionId
    void SetSessionData(int nCallerID, LineSelectResult * pSelectResult, vecLineSelectData & vecLineData,
                        yl::string strTitle, bool bShowIndex, int nSelectID);

public:
    //获取Softkey数据
    virtual void GetSoftkey(CArraySoftKeyBarData & objSoftkeyData);

public:
    virtual bool onKeyPressedEvent(int key);

public:
    //处理softkey 继承自 talkbaseui
    virtual bool OnSoftkeyPress(SoftKey_TYPE eSoftType, bool bLongPress);

protected:
    //根据序号选择账号
    void SelectAccountByIndex(int iSelectIndex);

protected:
    int m_nCallerID;

    //保存的相关数据
    vecLineSelectData m_vecLineData;
    yl::string m_strTitle;
    bool m_bShowIndex;
    int m_nSelectID;

    CLineSelectAdapter  m_objLineSelectAdapter;
    LineSelectResult    *   m_pSelectResult;
};

}       //namespace talkui


#endif
