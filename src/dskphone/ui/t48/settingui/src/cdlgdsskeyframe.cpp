
#include "cdlgdsskeyframe.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "baseui/fontcolor.h"
#include "commonunits/modcommonunits.h"
// 图片ICON宏定义
#include "baseui/t4xpicpath.h"
// DsskeyUI
#include "dsskeyui/src/dsskeyframe.h"
#include "dsskeyui/include/moddsskeyui.h"
#include "dsskey/include/moddsskey.h"
#include "commonapi/stringhelperapi.h"

#define DSSKEY_SET_LEFT_OFFSET_L    5
#define DSSKEY_SET_TOP_OFFSET       5
#define DSSKEY_SET_WIDTH            103
#define DSSKEY_SET_HEIGHT           45
#define DSSKEY_SET_V_DISTANCE       46
#define DSSKEY_SET_H_DISTANCE       104
const QRect RECT_SETTING_DSSKEYFRAME(4, 26, 528, 284);// Dsskey悬浮框的位置

CDlgDsskeyFrame::CDlgDsskeyFrame(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pSettingDsskey(NULL)
{
    InitData();
}

void CDlgDsskeyFrame::InitData()
{
    if (NULL != m_pGeneralFrame)
    {
        m_pGeneralFrame->setEnabled(false);
        m_pGeneralFrame->hide();
    }

    InitSettingDsskeyFrame();
}

// 释放数据，将窗口清空回复到初始化状态
void CDlgDsskeyFrame::Uninit()
{
}

bool CDlgDsskeyFrame::LoadPageData()
{
    return false;
}

bool CDlgDsskeyFrame::IsStatusChanged()
{
    //密码界面不需要对比是否有改变，因此不做操作
    return false;
}


bool CDlgDsskeyFrame::SavePageData()
{
    return false;
}

CDlgDsskeyFrame::~CDlgDsskeyFrame()
{
    if (NULL != m_pSettingDsskey)
    {
        m_pSettingDsskey->SetBtnClickAction(NULL);
    }
}

// 设置子页面的SoftKey
bool CDlgDsskeyFrame::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    for (int i = 0; i != SOFTKEY_NUM; ++i)
    {
        objWndData[i].m_strSoftkeyAction = "";
        objWndData[i].m_strSoftkeyTitle = "";
    }
    return true;
}

//初始化Dsskey按钮
void CDlgDsskeyFrame::InitSettingDsskeyFrame()
{
    // Dsskey部分初始化
    m_pSettingDsskey = new qSettingDsskeyFrame(this);
    int i = 0;
    qDsskeyButton * pDsskeyBtn = NULL;
    qUIPushButton * pPushBtn = NULL;
    //
    if (m_pSettingDsskey != NULL)
    {
        m_pSettingDsskey->setGeometry(RECT_SETTING_DSSKEYFRAME);
        // 设置按钮
        for (i = 0; i < 29; ++i)
        {
            if (i < 6)
            {
                pDsskeyBtn = new qDsskeySettingButton(i, m_pSettingDsskey);
            }
            else if (i < 12)
            {
                pDsskeyBtn = new qDsskeySettingButton(i + 5, m_pSettingDsskey);
            }
            else if (i < 18)
            {
                pDsskeyBtn = new qDsskeySettingButton(i + 5, m_pSettingDsskey);
            }
            else if (i < 24)
            {
                pDsskeyBtn = new qDsskeySettingButton(i + 5, m_pSettingDsskey);
            }
            else
            {
                pDsskeyBtn = new qDsskeySettingButton(i - 18, m_pSettingDsskey);
            }
            m_pSettingDsskey->AddDsskeyBtn(pDsskeyBtn);
            if (pDsskeyBtn != NULL)
            {
                if (i < 6)
                {
                    // 第1排Dsskey按钮
                    pDsskeyBtn->setGeometry(DSSKEY_SET_LEFT_OFFSET_L, DSSKEY_SET_TOP_OFFSET + i * DSSKEY_SET_V_DISTANCE,
                                            DSSKEY_SET_WIDTH, DSSKEY_SET_HEIGHT);
                }
                else if (i < 12)
                {
                    // 第2排Dsskey按钮
                    pDsskeyBtn->setGeometry(DSSKEY_SET_LEFT_OFFSET_L + DSSKEY_SET_H_DISTANCE,
                                            DSSKEY_SET_TOP_OFFSET + (i - 6) * DSSKEY_SET_V_DISTANCE, DSSKEY_SET_WIDTH, DSSKEY_SET_HEIGHT);
                }
                else if (i < 18)
                {
                    // 第3排Dsskey按钮
                    pDsskeyBtn->setGeometry(DSSKEY_SET_LEFT_OFFSET_L + 2 * DSSKEY_SET_H_DISTANCE,
                                            DSSKEY_SET_TOP_OFFSET + (i - 12) * DSSKEY_SET_V_DISTANCE, DSSKEY_SET_WIDTH, DSSKEY_SET_HEIGHT);
                }
                else if (i < 24)
                {
                    // 第3排Dsskey按钮
                    pDsskeyBtn->setGeometry(DSSKEY_SET_LEFT_OFFSET_L + 3 * DSSKEY_SET_H_DISTANCE,
                                            DSSKEY_SET_TOP_OFFSET + (i - 18) * DSSKEY_SET_V_DISTANCE, DSSKEY_SET_WIDTH, DSSKEY_SET_HEIGHT);
                }
                else
                {
                    // 第3排Dsskey按钮
                    pDsskeyBtn->setGeometry(DSSKEY_SET_LEFT_OFFSET_L + 4 * DSSKEY_SET_H_DISTANCE,
                                            DSSKEY_SET_TOP_OFFSET + (i - 24) * DSSKEY_SET_V_DISTANCE, DSSKEY_SET_WIDTH, DSSKEY_SET_HEIGHT);
                }
                if (i == 0)
                {
                    pDsskeyBtn->set3StatePic("", PIC_DSS_SETTING_TOPLEFT, "");
                }
                else if (i == 5)
                {
                    pDsskeyBtn->set3StatePic("", PIC_DSS_SETTING_BOTTOMLEFT, "");
                }
                else if (i == 24)
                {
                    pDsskeyBtn->set3StatePic("", PIC_DSS_SETTING_TOPRIGHT, "");
                }
                else
                {
                    pDsskeyBtn->set3StatePic("", PIC_DSS_SETTING_MIDDLE, "");
                }

                pDsskeyBtn->setIconPicAlign(Qt::AlignLeft);

            }
            else
            {
                IDLE_WARN("qDsskeyButton is NULL!.\n");
            }
        }

        // 设置按下按钮动作
        m_pSettingDsskey->SetBtnClickAction(this);


    }
    else
    {
        IDLE_WARN("m_pSettingDsskey is NULL!.\n");
    }

    m_pSettingDsskey->installEventFilter(this);
    m_pSettingDsskey->show();
    m_pSettingDsskey->raise();
}

void CDlgDsskeyFrame::showEvent(QShowEvent * pEvent)
{
    if (NULL != m_pSettingDsskey)
    {
        m_pSettingDsskey->RefreshAllDsskey();
    }
}

void CDlgDsskeyFrame::OnDsskeyAction(int iDsskeyID)
{
    SETTINGUI_DEBUG("OnDsskeyAction : iDsskeyID is %d.", iDsskeyID);

    int iBtnIndex = dsskey_GetKeyIndex(iDsskeyID);
    yl::string strAction;
    strAction = commonAPI_FormatString("OpenSubPage(line_key_item_%d)", iBtnIndex + 1);
    emit openpage(strAction);
}

