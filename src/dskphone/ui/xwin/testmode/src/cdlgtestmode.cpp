#include "cdlgtestmode.h"
#include "interfacedefine.h"
#include "ETLLib/ETLLib.hpp"
#include "voice/include/modvoice.h"
#include "keymap.h"
#include "devicelib/phonedevice.h"
#include "globalinputhook/modinputhook.h"
#include "uimanager/uimanager_common.h"
#include "uimanager/moduimanager.h"
#include "testexp.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "modtestmode.h"
#include "dsklog/log.h"
#include "service_ipvp.h"
#include "uimanager/autovolumebar.h"
#include "dsskey/include/moddsskey.h"
#include "test_include.h"
#include "testphonecp.h"
#include "testmodeinterface.h"
#include "exp/include/modexp.h"
#ifdef _CP920
#include "phonedev/cp920keypad.h"
#endif

namespace
{
const char * TESTMODE_UI_LAYOUT_FILE = "testmode/testmodeui.xml";
const char * TESTMODE_NAME_TEXT_TITLE = "textTitle";
const char * TESTMODE_NAME_TEXT_CONTENT_NORMAL = "textNormal";
const char * TESTMODE_NAME_TEXT_CONTENT_CENTER = "textCenter";
const char * TESTMODE_NAME_TEXT_CONTENT_SECOND_HALF = "textSecondHalf";
const char * TESTMODE_NAME_TEXT_CONTENT_FULLSCREEN = "textFullScreen";
#ifdef _CP920
const char * KB_TEST_RESULT_LAYOUT_FILE = "testmode/kb-test-result.xml";
const char * KB_TEST_RESULT_NAME_BTN_HK = "btnHK";
const char * KB_TEST_RESULT_NAME_BTN_PK = "btnPK";
const char * KB_TEST_RESULT_NAME_BTN_STAR = "btnStar";
const char * KB_TEST_RESULT_NAME_BTN_POUND = "btnPound";
const char * KB_TEST_RESULT_NAME_BTN_OFFHOOK = "btnOffHook";
const char * KB_TEST_RESULT_NAME_BTN_BT = "btnBT";
const char * KB_TEST_RESULT_NAME_BTN_OK = "btnOK";
const char * KB_TEST_RESULT_NAME_BTN_ONHOOK = "btnOnHook";
const char * KB_TEST_RESULT_NAME_BTN_UP = "btnUp";
const char * KB_TEST_RESULT_NAME_BTN_DOWN = "btnDown";
const char * KB_TEST_RESULT_NAME_BTN_VOL_DEC = "btnVolDec";
const char * KB_TEST_RESULT_NAME_BTN_MUTE = "btnMute";
const char * KB_TEST_RESULT_NAME_BTN_VOL_INC = "btnVolInc";
#endif
}

static LRESULT OnTestModeMessage(msgObject& objMsg)
{
    if (!exp_IsEXPKey(objMsg.lParam))
    {
        return 1;
    }

    CTestPhone * pTestPhone = testmode_GetCurrentTestPhone();
    if (NULL != pTestPhone)
    {
        pTestPhone->onKeyPressed(objMsg.lParam, true);
    }

    return 1;
}

static bool TestModeHookCallbackFun(int nKeyCode, bool bPress)
{
    TESTMODE_INFO("TestModeHookCallbackFun  nKeyCode %d  bPress %d ", nKeyCode, bPress);
    CDlgTestMode * pTestDlg = static_cast<CDlgTestMode *>(UIManager_GetPrivateDlg(DLG_CDlgTestMode));
    if (NULL != pTestDlg
            && (UIManager_GetTopWindow() == pTestDlg
                || PHONE_KEY_HANDFREE == nKeyCode
                || PHONE_KEY_HANDSET_ON_HOOK == nKeyCode)
       )
    {
        return pTestDlg->ProcessKeyEvent(nKeyCode, bPress);
    }

    return false;
}

CBaseDialog * CDlgTestMode::CreateInstance()
{
    CDlgTestMode * pTestModeUI = new CDlgTestMode();

    if (NULL != pTestModeUI)
    {
        pTestModeUI->loadContent(TESTMODE_UI_LAYOUT_FILE);
    }

    return pTestModeUI;
}

CDlgTestMode::CDlgTestMode()
    : CBaseDialog(this, eWT_TestMode)
    , m_pTextTitle(NULL)
    , m_pTextNormal(NULL)
    , m_pTextCenter(NULL)
    , m_pTextSecondHalf(NULL)
    , m_pTextFullScreen(NULL)
#ifdef _CP920
    , m_pBoxKeyResult(NULL)
#endif
    , m_pTestPhone(NULL)
{
}

CDlgTestMode::~CDlgTestMode()
{
    m_vecAreaColor.clear();

    if (NULL != m_pTestPhone)
    {
        delete m_pTestPhone;
        m_pTestPhone = NULL;
    }
}

// 初始化
void CDlgTestMode::Init()
{
    // 设置测试模式按键钩子
    inputHook_RegisterKeyPadHookProc(TestModeHookCallbackFun, HOOK_MODULE_TESTMODE);

    etl_RegisterMsgHandle(DEV_MSG_FUNKEY_EVENT, DEV_MSG_FUNKEY_EVENT, OnTestModeMessage);
}

// 释放数据，将窗口清空回复到初始化状态
void CDlgTestMode::Uninit()
{
    // 设置测试模式按键钩子
    inputHook_UnRegisterKeyPadHookProc(HOOK_MODULE_TESTMODE);

    etl_UnregisterMsgHandle(DEV_MSG_FUNKEY_EVENT, DEV_MSG_FUNKEY_EVENT, OnTestModeMessage);
}

void CDlgTestMode::FillFullByPicture(const yl::string& strPic)
{
    pixmap.load(strPic);

    if (!pixmap.isNull())
    {
        // 两者互斥
        m_vecAreaColor.clear();
        this->setBackground(pixmap);
    }
}

void CDlgTestMode::FillRectByColor(int nAreaType, const xColor & color)
{
    if (AT_ALL == nAreaType)
    {
        // 刷全屏的区域，主动清一次区域颜色信息
        m_vecAreaColor.clear();
    }

    stAreaColor stAC;
    stAC.rcArea = GetRectByAreaType(nAreaType);
    stAC.color = color;
    m_vecAreaColor.push_back(stAC);
    update();
}

#ifdef _CP920
void CDlgTestMode::ShowErrorKeys(bool bShow)
{
    if (!bShow)
    {
        if (NULL != m_pBoxKeyResult)
        {
            m_pBoxKeyResult->hide(true);
        }

        return;
    }

    if (NULL != m_pBoxKeyResult && !m_pBoxKeyResult->checkVisible())
    {
        m_pBoxKeyResult->show();
    }

    CTestPhoneCP * pTestPhoneCP = static_cast<CTestPhoneCP *>(m_pTestPhone);

    if (NULL == pTestPhoneCP)
    {
        return;
    }

    if (NULL == m_pBoxKeyResult)
    {
        m_pBoxKeyResult = new xLinearLayout;

        if (NULL == m_pBoxKeyResult)
        {
            return;
        }

        m_pBoxKeyResult->loadContent(KB_TEST_RESULT_LAYOUT_FILE);

        addChildView(m_pBoxKeyResult);
    }

    if (NULL != m_pTextTitle)
    {
        m_pTextTitle->hide(true);
    }

    if (NULL != m_pTextCenter)
    {
        m_pTextCenter->hide(true);
    }

    if (NULL != m_pTextNormal)
    {
        m_pTextNormal->hide(true);
    }

    xButton * pBtnKey = NULL;
    yl::string strBtnID;

    for (int i = CP920_KP_KEY_TOUCH_BEGIN; i <= CP920_KP_KEY_TOUCH_END; ++i)
    {
        if (!GetBtnIdByKey(i, strBtnID))
        {
            TESTMODE_ERR("GetBtnIdByKey(%d) fail.", i);
            continue;
        }

        pBtnKey = static_cast<xButton *>(m_pBoxKeyResult->getViewById(strBtnID));

        if (NULL == pBtnKey)
        {
            TESTMODE_ERR("Get Button(%d %s) fail.", i, strBtnID.c_str());
            continue;
        }

        if (pTestPhoneCP->IsKeyTestSucceed(i))
        {
            pBtnKey->hide();
        }
        else
        {
            pBtnKey->show();
        }
    }
}

bool CDlgTestMode::GetBtnIdByKey(int nRawKey, yl::string & strId)
{
    switch (nRawKey)
    {
    case CP920_KP_KEY_0:
        strId = KB_TEST_RESULT_NAME_BTN_PK + yl::string("0");
        break;
    case CP920_KP_KEY_1:
        strId = KB_TEST_RESULT_NAME_BTN_PK + yl::string("1");
        break;
    case CP920_KP_KEY_2:
        strId = KB_TEST_RESULT_NAME_BTN_PK + yl::string("2");
        break;
    case CP920_KP_KEY_3:
        strId = KB_TEST_RESULT_NAME_BTN_PK + yl::string("3");
        break;
    case CP920_KP_KEY_4:
        strId = KB_TEST_RESULT_NAME_BTN_PK + yl::string("4");
        break;
    case CP920_KP_KEY_5:
        strId = KB_TEST_RESULT_NAME_BTN_PK + yl::string("5");
        break;
    case CP920_KP_KEY_6:
        strId = KB_TEST_RESULT_NAME_BTN_PK + yl::string("6");
        break;
    case CP920_KP_KEY_7:
        strId = KB_TEST_RESULT_NAME_BTN_PK + yl::string("7");
        break;
    case CP920_KP_KEY_8:
        strId = KB_TEST_RESULT_NAME_BTN_PK + yl::string("8");
        break;
    case CP920_KP_KEY_9:
        strId = KB_TEST_RESULT_NAME_BTN_PK + yl::string("9");
        break;
    case CP920_KP_KEY_STAR:
        strId = KB_TEST_RESULT_NAME_BTN_STAR;
        break;
    case CP920_KP_KEY_POUND:
        strId = KB_TEST_RESULT_NAME_BTN_POUND;
        break;
    case CP920_KP_KEY_UP:
        strId = KB_TEST_RESULT_NAME_BTN_UP;
        break;
    case CP920_KP_KEY_DN:
        strId = KB_TEST_RESULT_NAME_BTN_DOWN;
        break;
    case CP920_KP_KEY_OK:
        strId = KB_TEST_RESULT_NAME_BTN_OK;
        break;
    case CP920_KP_KEY_VOLUME_DECREASE:
        strId = KB_TEST_RESULT_NAME_BTN_VOL_DEC;
        break;
    case CP920_KP_KEY_VOLUME_INCREASE:
        strId = KB_TEST_RESULT_NAME_BTN_VOL_INC;
        break;
    case CP920_KP_KEY_HK1:
        strId = KB_TEST_RESULT_NAME_BTN_HK + yl::string("0");
        break;
    case CP920_KP_KEY_HK2:
        strId = KB_TEST_RESULT_NAME_BTN_HK + yl::string("1");
        break;
    case CP920_KP_KEY_HK3:
        strId = KB_TEST_RESULT_NAME_BTN_HK + yl::string("2");
        break;
    case CP920_KP_KEY_HK4:
        strId = KB_TEST_RESULT_NAME_BTN_HK + yl::string("3");
        break;
    case CP920_KP_KEY_BT:
        strId = KB_TEST_RESULT_NAME_BTN_BT;
        break;
    case CP920_KP_KEY_MUTE:
        strId = KB_TEST_RESULT_NAME_BTN_MUTE;
        break;
    case CP920_KP_KEY_HOOK_ON:
        strId = KB_TEST_RESULT_NAME_BTN_ONHOOK;
        break;
    case CP920_KP_KEY_HOOK_OFF:
        strId = KB_TEST_RESULT_NAME_BTN_OFFHOOK;
        break;
    default:
        {
            return false;
        }
        break;
    }

    return true;
}
#endif

chRect CDlgTestMode::GetRectByAreaType(int nAreaType)
{
    chRect rc;
    chRect rcClient = clientRect();
    int nWidth = rcClient.Width();
    int nHeight = rcClient.Height();

    switch (nAreaType)
    {
    case AT_FIRST_OF_FOUR:
        {
            rc = chRect(0, 0, nWidth, nHeight / 4);
        }
        break;
    case AT_SECOND_OF_FOUR:
        {
            rc = chRect(0, nHeight / 4, nWidth, nHeight / 2);
        }
        break;
    case AT_THIRD_OF_FOUR:
        {
            rc = chRect(0, nHeight / 2, nWidth, nHeight * 3 / 4);
        }
        break;
    case AT_FOURTH_OF_FOUR:
        {
            rc = chRect(0, nHeight * 3 / 4, nWidth, nHeight);
        }
        break;
    default:
        {
            rc = rcClient;
        }
        break;
    }

    return rc;
}

void CDlgTestMode::SetDisplayText(const yl::string & strContent, int nTextType, int nFontSize)
{
    YLVector<xTextView *> vecNonActive;
    xTextView* pActive = GetTextView(vecNonActive, nTextType);

    if (NULL != pActive)
    {
        SetTextFontSize(pActive, nFontSize);
        pActive->SetText(strContent);
    }

    for (int i = 0; i < vecNonActive.size(); ++i)
    {
        if (NULL != vecNonActive[i])
        {
            vecNonActive[i]->SetText("");
        }
    }
}

void CDlgTestMode::SetTextColor(int nTextType, const xColor & color)
{
    YLVector<xTextView *> vecNonActive;
    xTextView* pTextView = GetTextView(vecNonActive, nTextType);

    if (NULL != pTextView)
    {
        pTextView->SetTextColor(color);
    }
}

///////////////////////////////////////////////////////////////////////////////////////
void CDlgTestMode::InitTestPhone(int iTestType)
{
    if (NULL == m_pTestPhone)
    {
        m_pTestPhone = testmode_CreateTestPhone();
    }

    FillRectByColor(AT_ALL, xColor(0xff, 0xff, 0xff));

    if (NULL != m_pTestPhone)
    {
        m_pTestPhone->SetTestMethod(iTestType);
    }
}

bool CDlgTestMode::onPaintBackground(xPainter & p)
{
    bool bResult = xFrameLayout::onPaintBackground(p);

    for (int i = 0; i < m_vecAreaColor.size(); ++i)
    {
        const stAreaColor & stAC = m_vecAreaColor[i];

#if IF_PIXMAP_MARK_ALPHA_CHANNEL
        if (overlap)
        {
            // 刷新背景颜色
            p.blendRect(stAC.rcArea, stAC.color);
        }
        else
#endif
        {
            // 刷新背景颜色
            p.fillRect(stAC.rcArea, stAC.color);
        }
    }
    return bResult;
}

void CDlgTestMode::SetTextFontSize(xTextView * pTextView, int nFontSize)
{
    if (NULL == pTextView)
    {
        return ;
    }

    xFont font = pTextView->GetFont();
    font.setPointSize(nFontSize);
    pTextView->SetFont(font);
}

void CDlgTestMode::loadChildrenElements(xml_node & node)
{
    xFrameLayout::loadChildrenElements(node);
    m_pTextTitle = static_cast<xTextView *>(getViewById(TESTMODE_NAME_TEXT_TITLE));
    m_pTextNormal = static_cast<xTextView *>(getViewById(TESTMODE_NAME_TEXT_CONTENT_NORMAL));
    m_pTextCenter = static_cast<xTextView *>(getViewById(TESTMODE_NAME_TEXT_CONTENT_CENTER));
    m_pTextSecondHalf = static_cast<xTextView *>(getViewById(TESTMODE_NAME_TEXT_CONTENT_SECOND_HALF));
    m_pTextFullScreen = static_cast<xTextView *>(getViewById(TESTMODE_NAME_TEXT_CONTENT_FULLSCREEN));
}

bool CDlgTestMode::ProcessKeyEvent(int nKeyCode, bool bPress)
{
    if (NULL != m_pTestPhone)
    {
        if (bPress)
        {
            return m_pTestPhone->onKeyPressed(nKeyCode, false);
        }
        else
        {
            return m_pTestPhone->OnKeyRelease();
        }
    }
    return false;
}

xTextView* CDlgTestMode::GetTextView(YLVector<xTextView *>& vecNonActive, int nTextType)
{
    xTextView* pActive = NULL;

    switch (nTextType)
    {
    case TT_TITLE:
        {
            pActive = m_pTextTitle;
        }
        break;
    case TT_CONTENT_NORMAL:
        {
            pActive = m_pTextNormal;
            vecNonActive.push_back(m_pTextCenter);
            vecNonActive.push_back(m_pTextFullScreen);
        }
        break;
    case TT_CONTENT_CENTER:
        {
            pActive = m_pTextCenter;
            vecNonActive.push_back(m_pTextNormal);
            vecNonActive.push_back(m_pTextFullScreen);
        }
        break;
    case TT_CONTENT_SECOND_HALF:
        {
            pActive = m_pTextSecondHalf;
        }
        break;
    case TT_CONTENT_FULL_SCREEN:
        {
            pActive = m_pTextFullScreen;
            vecNonActive.push_back(m_pTextCenter);
            vecNonActive.push_back(m_pTextNormal);
        }
        break;
    default:
        break;
    }

    return pActive;
}


