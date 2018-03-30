/************************************************************************
 * FileName  : CDlgACDCallInfo.cpp (implementation file)
 * Purpose   :
 * Date      : 2012/12/24 9:50:32
 ************************************************************************/

#include "cdlgacdcallinfo.h"
#include "baseui/fontcolor.h"
#include "uikernel/languagehelper.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "talkuimanager.h"
#include "uikernel/qsoftkeybar.h"
#include "qtcommon/qmisc.h"
#include "uimanager/rectdefine.h"
#include "contactinfoframe.h"


#include "talkuidelegate.h"

#include "talk/talklogic/include/modtalklogic.h"

#ifdef _T4X
const int   ITEM_HEIGHT = RECT_TALK_PAGE.height() / 5;
const QRect RECT_WAITTIME_INFO(2, 0, RECT_TALK_PAGE.width() - 2, ITEM_HEIGHT);
#endif

CDlgACDCallInfo::CDlgACDCallInfo()
    : m_bIsShowLongestLab(false)
{
    InitData();
}

CDlgACDCallInfo::~CDlgACDCallInfo()
{
}

// 初始化页面数据
void CDlgACDCallInfo::InitData()
{
    m_labWaitTimeLab.setParent(this);
    QRect rc = RECT_WAITTIME_INFO;
    m_labWaitTimeLab.setGeometry(rc);
    SetWgtFontColor(&m_labWaitTimeLab, FONT_TYPE_TALK_TEXT);

    m_labCCNameLab.setParent(this);
    rc.moveTop(rc.top() + ITEM_HEIGHT);
    m_labCCNameLab.setGeometry(rc);
    SetWgtFontColor(&m_labCCNameLab, FONT_TYPE_TALK_TEXT);

    m_labCCUIDLab.setParent(this);
    rc.moveTop(rc.top() + ITEM_HEIGHT);
    m_labCCUIDLab.setGeometry(rc);
    SetWgtFontColor(&m_labCCUIDLab, FONT_TYPE_TALK_TEXT);

    m_labQueueLab.setParent(this);
    rc.moveTop(rc.top() + ITEM_HEIGHT);
    m_labQueueLab.setGeometry(rc);
    SetWgtFontColor(&m_labQueueLab, FONT_TYPE_TALK_TEXT);

    m_labCallsLongestLab.setParent(this);
    rc.moveTop(rc.top() + ITEM_HEIGHT);
    m_labCallsLongestLab.setGeometry(rc);
    SetWgtFontColor(&m_labCallsLongestLab, FONT_TYPE_TALK_TEXT);

    //隐藏联系人控件
    m_cConInfoFrame.hide();
}

// 初始化
void CDlgACDCallInfo::Init()
{
    CDlgACDCallInfoBase::Init();

    m_bIsShowLongestLab = false;

}

// 释放数据，将窗口清空回复到初始化状态
void CDlgACDCallInfo::Uninit()
{
    m_labWaitTimeLab.clear();
    m_labCCNameLab.clear();
    m_labCCUIDLab.clear();
    m_labQueueLab.clear();
    m_labCallsLongestLab.clear();

    CDlgACDCallInfoBase::Uninit();
}

//设置通话数据内容
void CDlgACDCallInfo::SetTalkData(DataLogic2UIBase * pData)
{
    //基类先做处理
    CDlgACDCallInfoBase::SetTalkData(pData);

    // 入口参数判断
    if (pData == NULL)
    {
        return;
    }

    //重置设置成没有显示该控件
    m_bIsShowLongestLab = false;

    // 设置联系人信息
    CallCenterInfo * pCallInfoData = dynamic_cast<CallCenterInfo *>(pData);
    if (NULL != pCallInfoData)
    {
        //Wait Time
        QString strName = LANG_TRANSLATE(TRID_WAIT_TIME);
        m_labWaitTimeLab.setText(strName + QString(": %1 s").arg(pCallInfoData->m_nWaitTime));

        //CC Name
        strName = LANG_TRANSLATE(TRID_CALL_CENTER_NAME);
        m_labCCNameLab.setText(strName + QString(": %1").arg(pCallInfoData->m_strCallCenterName.c_str()));

        //CC UID
        strName = LANG_TRANSLATE(TRID_CALL_CENTER_USER_ID);
        m_labCCUIDLab.setText(strName + QString(": %1").arg(pCallInfoData->m_strUserId.c_str()));

        //Queue
        strName = LANG_TRANSLATE(TRID_QUEUE);
        m_labQueueLab.setText(strName + QString(": %1").arg(pCallInfoData->m_nCountInQueue));

#ifdef TALKUI_DEBUG_TEST
        //Longest Time
        pCallInfoData->m_nLongestTime = 9999;
#endif

        if (pCallInfoData->m_nLongestTime != -1)
        {
            strName = LANG_TRANSLATE(TRID_LONGEST_CALL_DURATION);
            m_labCallsLongestLab.setText(strName + QString(": %1 s").arg(pCallInfoData->m_nLongestTime));
            m_labCallsLongestLab.show();

            //设置成有显示Longest
            m_bIsShowLongestLab = true;
        }
        else
        {
            m_labCallsLongestLab.hide();

            //设置成没有显示该控件
            m_bIsShowLongestLab = false;
        }
    }


}

// 是否有显示提示信息
bool CDlgACDCallInfo::IsShowTip()
{
    //有显示longest 控件的时候 返回true
    if (true == m_bIsShowLongestLab)
    {
        return true;
    }

    return false;
}

void CDlgACDCallInfo::OnTipChange(TALK_TIP_TYPE eTipType)
{
    if (true == m_bIsShowLongestLab)
    {
        switch (eTipType)
        {
        case TALK_TIP_DTMF:
        case TALK_TIP_VOLUMEBAR:
        case TALK_TIP_NETWORK:
            {
                m_labCallsLongestLab.hide();
            }
            break;
        case TALK_TIP_CUSTOM:
            {
                m_labCallsLongestLab.show();
            }
            break;
        default:
            {
                m_labCallsLongestLab.show();
            }
            break;
        }
    }
    else
    {
        m_labCallsLongestLab.hide();
    }
}

