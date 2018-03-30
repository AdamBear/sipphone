#ifndef __TEST_CODE__
#define __TEST_CODE__

// 是否单元测试代码
#define FOR_UNIT_TEST 0

#if FOR_UNIT_TEST

///////////////////测试类：空闲界面窗口/////////////////////
class CDlgIdleScreen : public CBaseDialog
{
public:
    CDlgIdleScreen();
    virtual ~CDlgIdleScreen();

    // 初始化
    virtual void Init();
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();
    // 设置小窗口数据
    virtual void SetData(CWndTemplateData & objWndData, void * pData = NULL);

    static CBaseDialog * CreateInstance()
    {
        return new CDlgIdleScreen();
    }

private:
    QLabel   m_LabelText;
};


///////////////////测试类：预拨号窗口/////////////////////
class CDlgDialingUI : public CBaseDialog
{
public:
    CDlgDialingUI();
    virtual ~CDlgDialingUI();

    // 初始化
    virtual void Init();
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();
    // 设置小窗口数据
    virtual void SetData(CWndTemplateData & objWndData, void * pData = NULL);

    static CBaseDialog * CreateInstance()
    {
        return new CDlgDialingUI();
    }

private:
    QLabel   m_LabelText;
};


///////////////////测试类：回铃窗口/////////////////////
class CDlgConnectUI : public CBaseDialog
{
public:
    CDlgConnectUI();
    virtual ~CDlgConnectUI();

    // 初始化
    virtual void Init();
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();
    // 设置小窗口数据
    virtual void SetData(CWndTemplateData & objWndData, void * pData = NULL);

    static CBaseDialog * CreateInstance()
    {
        return new CDlgConnectUI();
    }

private:
    QLabel   m_LabelText;
};


///////////////////测试类：通话状态窗口/////////////////////
class CDlgTalkingUI : public CBaseDialog
{
public:
    CDlgTalkingUI();
    virtual ~CDlgTalkingUI();

    // 初始化
    virtual void Init();
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();
    // 设置小窗口数据
    virtual void SetData(CWndTemplateData & objWndData, void * pData = NULL);

    static CBaseDialog * CreateInstance()
    {
        return new CDlgTalkingUI();
    }

private:
    QLabel   m_LabelText;
};

///////////////////测试类：呼叫记录窗口/////////////////////
class CDlgCalllist : public CBaseDialog
{
public:
    CDlgCalllist();
    virtual ~CDlgCalllist();

    // 初始化
    virtual void Init();
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();
    // 设置小窗口数据
    virtual void SetData(CWndTemplateData & objWndData, void * pData = NULL);

    static CBaseDialog * CreateInstance()
    {
        return new CDlgCalllist();
    }

private:
    QLabel   m_LabelText;
};

#endif // #if FOR_UNIT_TEST

#endif // __TEST_CODE__
