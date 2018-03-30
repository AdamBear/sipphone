#include "uimanager_header.h"
#include "moduimanager.h"
#include "testcode.h"

#if FOR_UNIT_TEST
///////////////////测试类：空闲界面窗口/////////////////////
CDlgIdleScreen::CDlgIdleScreen()
{
    m_LabelText.setParent(this);
}

CDlgIdleScreen::~CDlgIdleScreen()
{

}

// 初始化
void CDlgIdleScreen::Init()
{
    m_LabelText.setText("I am Idle Logo!");
    m_LabelText.setGeometry(5, 5, 500, 250);
}

// 释放数据，将窗口清空回复到初始化状态
void CDlgIdleScreen::Uninit()
{
    m_LabelText.setText("");
}

// 设置小窗口数据
void CDlgIdleScreen::SetData(CDlgDataBase * pData, CWndTemplateData & objWndData)
{
    objWndData.m_eWndTemplate = TEMPLATE_IDLE;
    objWndData.m_SoftkeyData[0].m_strSoftkeyName = "History";
    objWndData.m_SoftkeyData[1].m_strSoftkeyName = "Directory";
    objWndData.m_SoftkeyData[2].m_strSoftkeyName = "Message";
    objWndData.m_SoftkeyData[3].m_strSoftkeyName = "Setting";
}

///////////////////测试类：预拨号窗口/////////////////////
CDlgDialingUI::CDlgDialingUI()
{
    m_LabelText.setParent(this);
}

CDlgDialingUI::~CDlgDialingUI()
{

}

// 初始化
void CDlgDialingUI::Init()
{
    m_LabelText.setText("I am Predial content!");
    m_LabelText.setGeometry(5, 5, 500, 250);
}

// 释放数据，将窗口清空回复到初始化状态
void CDlgDialingUI::Uninit()
{
    m_LabelText.setText("");
}

// 设置小窗口数据
void CDlgDialingUI::SetData(CDlgDataBase * pData, CWndTemplateData & objWndData)
{
    objWndData.m_eWndTemplate = TEMPLATE_COMMON_TALK;
    objWndData.m_SoftkeyData[0].m_strSoftkeyName = "Account";
    objWndData.m_SoftkeyData[1].m_strSoftkeyName = "123";
    objWndData.m_SoftkeyData[2].m_strSoftkeyName = "Delete";
    objWndData.m_SoftkeyData[3].m_strSoftkeyName = "Voice Call";
}

///////////////////测试类：回铃状态窗口/////////////////////
CDlgConnectUI::CDlgConnectUI()
{
    m_LabelText.setParent(this);
}

CDlgConnectUI::~CDlgConnectUI()
{

}

// 初始化
void CDlgConnectUI::Init()
{
    m_LabelText.setText("I am Ringback content!");
    m_LabelText.setGeometry(5, 5, 500, 250);
}

// 释放数据，将窗口清空回复到初始化状态
void CDlgConnectUI::Uninit()
{
    m_LabelText.setText("");
}

// 设置小窗口数据
void CDlgConnectUI::SetData(CDlgDataBase * pData, CWndTemplateData & objWndData)
{
    objWndData.m_eWndTemplate = TEMPLATE_COMMON_TALK;
    objWndData.m_SoftkeyData[0].m_strSoftkeyName = "";
    objWndData.m_SoftkeyData[1].m_strSoftkeyName = "";
    objWndData.m_SoftkeyData[2].m_strSoftkeyName = "";
    objWndData.m_SoftkeyData[3].m_strSoftkeyName = "Cancel";
}


///////////////////测试类：通话窗口数据/////////////////////
class CDlgDataTalk : public CDlgDataBase
{
public:
    CDlgDataTalk()
    {
        m_bAllowConf = false;
        m_bHold = false;
    }

public:
    // 是否允许conf
    bool   m_bAllowConf;
    // 是否hold
    bool   m_bHold;
};

///////////////////测试类：通话状态窗口/////////////////////
CDlgTalkingUI::CDlgTalkingUI()
{
    m_LabelText.setParent(this);
}

CDlgTalkingUI::~CDlgTalkingUI()
{

}

// 初始化
void CDlgTalkingUI::Init()
{
    m_LabelText.setText("I am Talking content!");
    m_LabelText.setGeometry(5, 5, 500, 250);
}

// 释放数据，将窗口清空回复到初始化状态
void CDlgTalkingUI::Uninit()
{
    m_LabelText.setText("");
}

// 设置小窗口数据
void CDlgTalkingUI::SetData(CWndTemplateData & objWndData, void * pData/* = NULL*/)
{
    objWndData.m_strDlgName = DLG_Talking;
    objWndData.m_eWndTemplate = TEMPLATE_COMMON_TALK;
    objWndData.m_strSoftkey[0] = "Transfer";

    if (pData != NULL)
    {
        bool bHold = *(bool *)pData;
        if (bHold)
        {
            objWndData.m_strSoftkey[1] = "Resume";
        }
        else
        {
            objWndData.m_strSoftkey[1] = "Hold";
        }
    }

    objWndData.m_strSoftkey[2] = "Conference";
    objWndData.m_strSoftkey[3] = "Cancel";
}

///////////////////测试类：呼叫记录窗口/////////////////////
CDlgCalllist::CDlgCalllist()
{
    m_LabelText.setParent(this);
}

CDlgCalllist::~CDlgCalllist()
{

}

// 初始化
void CDlgCalllist::Init()
{
    m_LabelText.setText("I am Calllist content!");
    m_LabelText.setGeometry(5, 5, 500, 250);
}

// 释放数据，将窗口清空回复到初始化状态
void CDlgCalllist::Uninit()
{
    m_LabelText.setText("");
}

// 设置小窗口数据
void CDlgCalllist::SetData(CDlgDataBase * pData, CWndTemplateData & objWndData)
{
    objWndData.m_eWndTemplate = TEMPLATE_CALLLIST;
    objWndData.m_SoftkeyData[0].m_strSoftkeyName = "Video Call";
    objWndData.m_SoftkeyData[1].m_strSoftkeyName = "Voice Call";
    objWndData.m_SoftkeyData[2].m_strSoftkeyName = "Add";
    objWndData.m_SoftkeyData[3].m_strSoftkeyName = "Back";
}

////////////////////////////////////////////////////////////
// 用来抓取全局按键事件，达到测试目的
class CHookEvent : public QObject
{
public:
    CHookEvent()
    {
        m_pWndTalk = NULL;
        m_pWndCallList = NULL;
        m_pWndTalk2 = NULL;
        // hook全局事件
        qApp->installEventFilter(this);
    }

    // 界面消息处理(包括按键处理)
    bool eventFilter(QObject * obj, QEvent * ev)
    {
        if (ev->type() == QEvent::KeyPress)
        {
            QKeyEvent * keyEvent = static_cast<QKeyEvent *>(ev);
            return TestProcessKey(keyEvent->key());
        }

        return false;
    }

    // 测试按键
    bool TestProcessKey(int iKey)
    {
        bool bRetVal = false;

        switch (iKey)
        {
        case Qt::Key_1:
            {
                bRetVal = true;

                if (m_pWndTalk == NULL)
                {
                    // 创建窗口模板，加入堆栈
                    m_pWndTalk = UIManager_CreateWindowData();

                    // 创建预拨号窗口，并设置数据
                    CBaseDialog * pDlg = UIManager_GetPrivateDlg(DLG_Dial);
                    if (pDlg != NULL)
                    {
                        m_pWndTalk->SetWndDataAndDlg(pDlg, NULL);
                    }

                    // 更新界面
                    //UIManager_UpdateWnd(m_pWndTalk);
                    UIManager_UpdateWnd(pDlg);
                }
                else
                {
                    // 销毁窗口，退出窗口堆栈
                    //UIManager_DestroyWindowData(m_pWndTalk);
                    UIManager_DestroyWindowData(m_pWndTalk->GetPrivateDlg());
                    m_pWndTalk = NULL;
                }
            }
            break;
        case Qt::Key_2:
            {
                bRetVal = true;

                if (m_pWndCallList == NULL)
                {
                    // 创建窗口模板，加入堆栈
                    m_pWndCallList = UIManager_CreateWindowData();

                    // 创建呼叫记录窗口，并设置数据
                    CBaseDialog * pDlg = UIManager_GetPrivateDlg(DLG_Calllist);
                    if (pDlg != NULL)
                    {
                        m_pWndCallList->SetWndDataAndDlg(pDlg, NULL);
                    }

                    // 更新界面
                    UIManager_UpdateWnd(m_pWndCallList);
                }
                else
                {
                    // 销毁窗口，退出窗口堆栈
                    UIManager_DestroyWindowData(m_pWndCallList);
                    m_pWndCallList = NULL;
                }
            }
            break;
        case Qt::Key_3:
            {
                bRetVal = true;

                // 将通话窗口设置为顶层
                if (m_pWndTalk != NULL)
                {
                    UIManager_SetTopWindow(m_pWndTalk);
                }
            }
            break;
        case Qt::Key_4:
            {
                bRetVal = true;

                // 将呼叫记录窗口设置为顶层
                if (m_pWndCallList != NULL)
                {
                    CBaseDialog * pDlg = m_pWndCallList->GetPrivateDlg();
                    if (pDlg != NULL)
                    {
                        UIManager_SetTopWindow(UIManager_GetWndTemplateByDlg(pDlg));
                    }
                }
            }
            break;
        case Qt::Key_5:
            {
                bRetVal = true;

                if (m_pWndTalk != NULL)
                {
                    // 将原来的窗口还回去
                    UIManager_PutPrivateDlg(m_pWndTalk->GetPrivateDlg());

                    // 从预拨号界面跳转到回铃界面
                    CBaseDialog * pDlg = UIManager_GetPrivateDlg(DLG_Connect);
                    if (pDlg != NULL)
                    {
                        m_pWndTalk->SetWndDataAndDlg(pDlg, NULL);
                        UIManager_UpdateWnd(m_pWndTalk);
                    }
                }
            }
            break;
        case Qt::Key_6:
            {
                bRetVal = true;

                if (m_pWndTalk != NULL)
                {
                    // 将原来的窗口还回去
                    UIManager_PutPrivateDlg(m_pWndTalk->GetPrivateDlg());

                    // 从回铃界面跳转到通话界面
                    CBaseDialog * pDlg = UIManager_GetPrivateDlg(DLG_Talking);
                    if (pDlg != NULL)
                    {
                        bool bHold = false;
                        m_pWndTalk->SetWndDataAndDlg(pDlg, &bHold);
                        UIManager_UpdateWnd(m_pWndTalk);
                    }
                }
            }
            break;
        case Qt::Key_7:
            {
                bRetVal = true;

                if (m_pWndTalk2 == NULL)
                {
                    // 创建窗口模板，加入堆栈
                    m_pWndTalk2 = UIManager_CreateWindowData();

                    // 创建预拨号窗口，并设置数据
                    CBaseDialog * pDlg = UIManager_GetPrivateDlg(DLG_Talking);
                    if (pDlg != NULL)
                    {
                        bool bHold = true;
                        m_pWndTalk2->SetWndDataAndDlg(pDlg, &bHold);
                        UIManager_UpdateWnd(m_pWndTalk2);
                    }
                }
                else
                {
                    // 销毁窗口，退出窗口堆栈
                    UIManager_DestroyWindowData(m_pWndTalk2);
                    m_pWndTalk2 = NULL;
                }
            }
            break;
        case Qt::Key_8:
            {
                bRetVal = true;

                // 返回Idle界面
                UIManager_ReturnToIdle();

                m_pWndCallList = NULL;
                m_pWndTalk = NULL;
                m_pWndTalk2 = NULL;
            }
            break;
        case Qt::Key_9:
            {
                bRetVal = true;

                // 更改Talk界面的softkey
                if (m_pWndTalk != NULL)
                {
                    m_pWndTalk->SetSoftkey(0, "Yes");
                    m_pWndTalk->SetSoftkey(1, "No");
                    m_pWndTalk->SetSoftkey(2, "hi");
                    m_pWndTalk->SetSoftkey(3, "good");
                    UIManager_UpdateWnd(m_pWndTalk);
                }
            }
            break;
        default:
            break;
        }

        return bRetVal;
    }

private:
    CWndTemplate * m_pWndTalk;
    CWndTemplate * m_pWndCallList;
    CWndTemplate * m_pWndTalk2;
};

////////////////////////////////////////////////
int main(int argc, char * argv[])
{
    // 必须在UIManager_Init()之前注册
    REGISTER_DLG(CDlgIdleScreen);
    REGISTER_DLG(CDlgDialingUI);
    REGISTER_DLG(CDlgConnectUI);
    REGISTER_DLG(CDlgTalkingUI);
    REGISTER_DLG(CDlgCalllist);

    // 定义为static是为了保证QApplication在其他static变量之后再析构
    // 否则如果某些QT控件早QApplication之后析构会出现异常
    static QApplication a(argc, argv);

    UIManager_Init();
    UIManager_ShowMainWnd();

    CHookEvent objHookEvent;

    // 显示Idle界面
    CWndTemplate * pIdleWnd = UIManager_CreateWindowData();
    if (pIdleWnd != NULL)
    {
        CBaseDialog * pDlg = UIManager_GetPrivateDlg(DLG_T3XIdleScreen);
        if (pDlg != NULL)
        {
            pIdleWnd->SetWndDataAndDlg(pDlg, NULL);
        }

        UIManager_UpdateWnd(pIdleWnd);
    }

    a.exec();

    return 0;
}

#endif // #if FOR_UNIT_TEST
