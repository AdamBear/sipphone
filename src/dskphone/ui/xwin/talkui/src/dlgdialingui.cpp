////////////////////talkui///////////////
#include "dlgdialingui.h"
#include "talkuixmlpath.h"
#include "talkui_define.h"
#include "talkuicommonfunction.h"
#include "talkuipath.h"
#include "talkuixmlitemname.h"
#include "talkuiframemanager.h"
#include "conflictmanager.h"

#include "modtalkui.h"

////////////////////xwindow/////////////////
#include "xwindow/xWindowManager.h"

#include "language/common/lang_common.h"

#include "directoryui/include/moddirectoryui.h"
#include "extended/xmlbrowser/include/modxmlbrowser.h"

#include "baseui/include/t2xpicpath.h"

namespace talkui
{
#define TALK_DAIL_INVALID_SOFTKEY_INDEX             -1
#define TALK_DAIL_DELETE_SOFTKEY_INDEX                  2

#define TALK_DIAL_NUMBER_FONT_CHANGE_NUM        23


#define TALK_DIAL_INPUT_HEIGHT_BIG                          33
#define TALK_DIAL_INPUT_HEIGHT_SMALL                        13

#ifdef IF_FEATURE_LONG_PRESS_DIGIT
//长按的时间
#define TALK_DIAL_LONG_PRESS_TIME  1000
#endif

CDlgDialingUI::CDlgDialingUI()
    : CDlgBaseTalkUI(DLG_DialingUI, eWT_Dialing)
    , m_pLineEdit(NULL)     //编辑框控件
    , m_pSearchList(NULL)
{

}

CDlgDialingUI::~CDlgDialingUI()
{
#ifdef IF_FEATURE_LONG_PRESS_DIGIT
    m_objLongPressTimer.KillTimer();
#endif
}


void CDlgDialingUI::LoadLayout()
{
    loadContent(TALKUI_DLGDIALINGUI_XML);
}

void CDlgDialingUI::loadChildrenElements(xml_node & node)
{
    xFrameLayout::loadChildrenElements(node);

    m_pLineEdit = static_cast<xEditTextView *>(getViewById(TALKUI_DIAL_LINEEDIT));
    m_pSearchList = static_cast<xListView *>(getViewById(TALKUI_DIAL_SEARCHLIST));
}

// 初始化
void CDlgDialingUI::Init()
{
    CDlgBaseTalkUI::Init();

    //先断开信号 然后在连接信号
    //防止嘟嘟声不正确的一响就消失
    if (NULL != m_pLineEdit)
    {
        //这边还要做 断开信号槽操作
        SLOT_DISCONNECT(m_pLineEdit, signalTextChanged, this, talkui::CDlgDialingUI::slotTextChanged);

        m_pLineEdit->SetDeleteSoftkeyIndex(TALK_DAIL_INVALID_SOFTKEY_INDEX);
        m_pLineEdit->SetIme(IME_123_Dial);
        m_pLineEdit->SetInitText("");

        m_pLineEdit->SetImeSoftkeyIndex(TALK_DAIL_INVALID_SOFTKEY_INDEX);

        //这边还要做连接信号槽操作
        SLOT_CONNECT(m_pLineEdit, signalTextChanged, this, talkui::CDlgDialingUI::slotTextChanged);
    }

    SLOT_CONNECT(&m_objSearchAdapter, signalSearchResultChange, this,
                 talkui::CDlgDialingUI::slotSearchResultChange);

    if (NULL != m_pLineEdit)
    {
        // 设置密码拨号的相关参数
        if (configParser_GetConfigInt(kszEnablePSWDial) == 1)
        {
            m_pLineEdit->SetPartitionPwdPrefix(configParser_GetConfigString(kszPSWDialPrefix).c_str());
            m_pLineEdit->SetPartitionPwdLength(configParser_GetConfigInt(kszPSWDialLength));
        }
        else
        {
            m_pLineEdit->SetPartitionPwdPrefix("");
            m_pLineEdit->SetPartitionPwdLength(0);
        }

#if IF_BUG_27560
        if (actURI_IsHideTalkInfo())
        {
            m_pLineEdit->SetPartitionPwdPrefix("");
            m_pLineEdit->SetPartitionPwdLength(128);
        }
#endif
    }

    if (NULL != m_pSearchList)
    {
        m_pSearchList->SetAdapter(&m_objSearchAdapter);
    }

    //搜索框初始化
    m_objSearchAdapter.Init();

    m_objSearchAdapter.OnFlushList();

#ifdef IF_FEATURE_LONG_PRESS_DIGIT
    m_objLongPressTimer.KillTimer();
#endif
}

// 释放数据，将窗口清空回复到初始化状态
void CDlgDialingUI::Uninit()
{
    if (NULL != m_pLineEdit)
    {
        //这边还要做 断开信号槽操作
        SLOT_DISCONNECT(m_pLineEdit, signalTextChanged, this, talkui::CDlgDialingUI::slotTextChanged);
    }

    if (NULL != m_pSearchList)
    {
        m_pSearchList->ClearContent();
        m_pSearchList->SetAdapter(NULL);
    }

    m_objSearchAdapter.Uninit();

#ifdef IF_FEATURE_LONG_PRESS_DIGIT
    m_objLongPressTimer.KillTimer();
#endif
}

// 刷新窗口
void CDlgDialingUI::UpdateWnd(bool bUIReturn/* = false*/)
{
    //基类的先做  因为设置焦点在最后      不在最后可能会被别的影响
    CDlgBaseTalkUI::UpdateWnd(bUIReturn);

    if (m_pProcessor->NeedReUpdate())
    {
        // http://10.3.5.199/Bug.php?BugID=63426
        //添加判断 如果是联系人选中了 那么不设置焦点
        if (!IsSearchTableSelectItem())
        {
            if (NULL != m_pLineEdit)
            {
                if (!m_pLineEdit->hasFocus())
                {
                    //给编辑框设置焦点
                    m_pLineEdit->setFocus();

                    // 设置光标到末尾
                    m_pLineEdit->SetCursorPosition(m_pLineEdit->GetText().length());
                }
            }
        }
    }
}

bool CDlgDialingUI::IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
                                    yl::string & strBGPath)
{
    iMin = 1;
    iMax = 15;
    //兼容T69和T29，T46
//      strBGPath = strDialingBGPath;

    return true;
}

void CDlgDialingUI::GetSoftkey(CArraySoftKeyBarData & objSoftkeyData)
{
    CDlgBaseTalkUI::GetSoftkey(objSoftkeyData);

    if (NULL != m_pLineEdit)
    {
        int nDeleteSoftkeyIndex = 0;
        CArraySoftKeyBarData::iterator iter = objSoftkeyData.begin();
        for (; iter != objSoftkeyData.end(); ++iter, ++nDeleteSoftkeyIndex)
        {
            CSoftKeyBarData & objData = *iter;
            if (ST_IMECHANGE == objData.m_iType)
            {
                objData.m_strSoftkeyTitle = m_pLineEdit->GetImeName();
            }
        }
    }
}

// 功能键处理
bool CDlgDialingUI::OnFunckeyPress(int iKey)
{
    bool bResult = false;

    //按上下键
    if (PHONE_KEY_UP == iKey
            || PHONE_KEY_DOWN == iKey)
    {
        if (NULL != m_pLineEdit && m_pLineEdit->hasFocus())
        {
            if (NULL != m_pSearchList && m_pSearchList->GetTotalDataCount() > 0)
            {
                bResult =  false;
            }
            else
            {
                bResult = true;
            }
        }
    }

    //这边不能返回  一定要交给UI逻辑在处理一次  因为需要发送按键action给通话逻辑层
    bool bBaseResult = CDlgBaseTalkUI::OnFunckeyPress(iKey);

    if (bResult)
    {
        return true;
    }

    return bBaseResult;
}

// 数字键处理
bool CDlgDialingUI::OnDigitkeyPress(int iKey)
{
    //交给基类去处理  //基类有处理那么直接返回true
    if (CDlgBaseTalkUI::OnDigitkeyPress(iKey))
    {
        return true;
    }

    if (NULL != m_pLineEdit)
    {
        if (!m_pLineEdit->hasFocus())
        {
            //给编辑框设置焦点
            m_pLineEdit->setFocus();

            // 设置光标到末尾
            //与T46体验不同
            m_pLineEdit->SetCursorPosition(m_pLineEdit->GetText().length());

            //搜索框会自己失去焦点
//              m_objSearchAdapter.SetNoFocus();

            //模拟进行一次输入
//             if (m_pLineEdit->SendImeKey(iKey))
//             {
//                 return true;
//             }
        }
    }

#ifdef IF_FEATURE_LONG_PRESS_DIGIT
    m_objLongPressTimer.SetTimer(TALK_DIAL_LONG_PRESS_TIME, TIMER_METHOD(this,
                                 CDlgDialingUI::OnLongPressTimer));
#endif

    return false;
}

bool CDlgDialingUI::OnKeyRelease(int iKey)
{
    //交给基类去处理  //基类有处理那么直接返回true
    if (CDlgBaseTalkUI::OnKeyRelease(iKey))
    {
        return true;
    }

#ifdef IF_FEATURE_LONG_PRESS_DIGIT
    if (talkuiIsDigitKey(iKey))
    {
        m_objLongPressTimer.KillTimer();
    }
#endif

    return false;
}

//获取 processor
CBaseTalkUIProcessor * CDlgDialingUI::GetTalkUIProcessor()
{
    return GetBindUIProcessor();
}

//是否显示DTMF条
bool CDlgDialingUI::IsShowDtmfLine() const
{
    return false;
}

// 是否显示切换话路的滚动条
bool CDlgDialingUI::IsShowSessionScrollBar()
{
    return false;
}

// 能否切换Session
bool CDlgDialingUI::IsChangeSession(int iKey)
{
    return false;
}

#if IF_TALKUI_BG_PICTURE
////////////////背景///////////////////////////
//设置整个大背景的图片（不是界面自身的背景）
xBrush CDlgDialingUI::GetBackGroundBrush(bool & bResult)
{
#if IF_TALKUI_BG_IDLE_STYLE
    return CDlgBaseTalkUI::GetBackGroundBrush(bResult);
#else
    xBrush brushBG;
#if IF_TALKUI_BIG_DIAL_BG
    brushBG.setPixmap(GetPixmapByString(PIC_TALK_BACKGROUND_DIALING));
#endif
    return brushBG;
#endif
}

//界面自身背景
yl::string CDlgDialingUI::getTalkUIBackGround()
{
#if IF_TALKUI_BIG_DIAL_BG
    return "";
#else
    return PIC_DIAL_TALKFRAME_BACKGROUND;
#endif
}

///////////////////////////////////////////////
#endif

////////////////////////////////////////////////
xView * CDlgDialingUI::GetFocusWidget()
{
    return m_pLineEdit;
}
////////////////////////////////////////////////


void CDlgDialingUI::OnFocusChanged(bool bFocused)
{
    if (bFocused)
    {
        RefreshTalkUIData();
    }

    CDlgBaseTalkUI::OnFocusChanged(bFocused);
}

//刷新通话UI数据  这边需要从processor 获取 数据 设置到通话UI子界面上
void CDlgDialingUI::RefreshTalkUIData()
{
    //从Processor 获取数据
    CDialUIData * pDialUIData = NULL;
    if (NULL != m_pProcessor)
    {
        pDialUIData = m_pProcessor->GetDialUIData();
    }

    //判断获取到的数据 或者 编辑框控件 为空 直接返回
    if ((NULL == pDialUIData) || (NULL == m_pLineEdit))
    {
        return ;
    }

    ////只要是transfer 类型那么就要设置穿梭数据  在设置穿梭数据中有判断 穿梭数据相同不会重复设置
    //// 若从Transfer类型转换到其他类型需要清空穿梭数据
    if (DIAL_UI_TRANSFER == pDialUIData->m_eDialUIType)
    {
        ////设置穿梭数据, 在内部会判断是否重复设置
        m_objSearchAdapter.SetTranCallData(pDialUIData->m_listTranCallInfo);
    }
    else if (DIAL_UI_CONFERENCE == pDialUIData->m_eDialUIType)
    {
        ////设置会议的数据, 在内部会判断是否重复设置
        m_objSearchAdapter.SetTranCallData(pDialUIData->listConfCallInfo);
    }
    else
    {
        ////设置穿梭数据, 在内部会判断是否重复设置
        //把穿梭数据清除掉
        YLList<CCallInfo *> listCallInfoEmpty;
        m_objSearchAdapter.SetTranCallData(listCallInfoEmpty);
    }

#if IF_TALKUI_PSTN
    //如果是PSTN，强制关闭psw
    if (CALL_LINE_PSTN == pDialUIData->m_eCallLineType)
    {
        m_pLineEdit->SetPartitionPwdPrefix("");
        m_pLineEdit->SetPartitionPwdLength(0);
    }
#endif //IF_TALKUI_PSTN

    //如果 编辑框中的数据 和 逻辑层中保存的数据不相同 那么 把逻辑层数据设置给编辑框就可以了
    if (m_pLineEdit->GetText() != pDialUIData->m_strNumber)
    {
        m_pLineEdit->SetInitText(pDialUIData->m_strNumber);
        //这边不能再调用 OnTextChange函数了  因为本身setText 就会产生编辑框字符改变的信号
#ifdef IF_FEATURE_LONG_PRESS_DIGIT
        if (pDialUIData->m_strNumber.size() == 1 && talkuiIsDigitKey(pDialUIData->m_iKeyCode))
        {
            m_objLongPressTimer.SetTimer(TALK_DIAL_LONG_PRESS_TIME, TIMER_METHOD(this,
                                         CDlgDialingUI::OnLongPressTimer));
        }
#endif

        // http://10.3.5.199/Bug.php?BugID=71400
        //有两种情况， 如果没有焦点 那么在updatewnd的时候会获取焦点 并且设置光标的位置
        //如果此时有焦点那么 在这边会设置光标的位置
        // 设置光标到末尾
        if (m_pLineEdit->hasFocus())
        {
            m_pLineEdit->SetCursorPosition(m_pLineEdit->GetText().length());
        }
    }
    else    //如果编辑框字符与逻辑层中保存的相同  那么不要赋值  但是有可能是都为空
    {
        //如果是为空  那么要给编辑框对应的输入法传事件
        if (pDialUIData->m_strNumber.empty())
        {
            //这个根据逻辑层的说法 UI不用  因为逻辑层自己会把这个转成字符
//              if(pDialUIData->m_iKeyCode != PHONE_KEY_NONE)
//              {
//                  //QKeyEvent keyEvent(QEvent::KeyPress, pDialUIData->m_iKeyCode, Qt::NoModifier);
//                  //qPassEventToEdit(&m_objPswLineEdit, &keyEvent);
//              }
//              else
            {
                ////这边不用担心 是从别的拨号界面 进入到 当前拨号界面  因为拨号界面只有一个 在进入新的拨号界面的时候 会放回缓存中 执行一次Init，UInit函数
                ////编辑框没有字符  并且输入的为PHONE_KEY_NONE即可能是免提 或者 手柄进入  或者是dsskey进入拨号界面
                //if(NULL != m_pSearchTable)
                //{
                //  //这边如果编辑框中的字符为空 并且有穿梭数据 是不需要去搜索的
                //  //如果 没有穿梭数据 那么还是要去搜索一下， 因为为空的时候 也可以搜索到数据出来
                //  if(m_pSearchTable->GetTableCount() <= 0)
                //  {
                //      //###############################
                //      //发送编辑框字符变化给逻辑
                //      OnTextChanged("");

                //  }
                //}
                //内部会判断是否需要搜索
                m_objSearchAdapter.OnTextChange("");
            }
        }
        else
        {
            //数据内容没有改变，可以直接返回
            //因为后续还有设置光标的操作， 所以 这边要直接返回 防止光标出现问题
            return ;
        }
    }

    // http://10.3.5.199/Bug.php?BugID=63426
    //添加判断 如果是联系人选中了 那么不设置焦点
    //移到 updatewnd处理
}
////////////////////////////////继承自  dlgbasetalkui 结束/////////////////////



////////////////////////////////继承自  CUILogicDialUIBase /////////////////////
//设置UILogic 数据
void CDlgDialingUI::SetUILogicData(void * pData, UPDATE_REASON eReason)
{
    //不做处理  统一放到RefreshTalkUIData处理
}

//从界面获取数据
bool CDlgDialingUI::GetInputContent(yl::string & strNum, yl::string & strDisplayname)
{
    int iCallID = -1;

    //cjz的edk模块会在textchange的时候 从ui获取编辑框的数据
    //获取后 会重新刷新界面  所以这边不能把联系人列表数据设置到 编辑框中 要不会出现问题

    //t49 联系人没有焦点状态  只有点击状态
    //获取焦点item 数据 成功 返回 获取到的callid
    if (!m_pLineEdit->hasFocus()
            && m_objSearchAdapter.GetFocusItemData(strDisplayname, strNum, iCallID))
    {
        return true;
    }

    //没有获取到焦点item 就用lineedit中的数据
    strDisplayname = "";

    //这边不能直接用这个接口返回的数据   因为拨号界面用的是密码拨号，   text 函数被重写了  并且不是虚函数
    //  qReturnEdit(&m_objPswLineEdit);
    //编辑框立即输出字符
    InputLineEditData();

    if (NULL != m_pLineEdit)
    {
        strNum = m_pLineEdit->GetText();
    }

    return true;

}

//从搜索框中获取选择数据
bool CDlgDialingUI::GetSearchTableContent(yl::string & strNum, yl::string & strDisplayname)
{
    int iCallID = -1;
    return m_objSearchAdapter.GetFocusItemData(strDisplayname, strNum, iCallID);
}

//把选中的搜索结果赋值到编辑框中
void CDlgDialingUI::SetLineEditDataBySearchTable()
{
    yl::string strDisplayName = "";
    yl::string strNumber = "";
    int iCallID = -1;

    //联系人列表中有焦点item 那么才要给lineedit 设置号码，   没有焦点item那么就不设置了
    if (!m_pLineEdit->hasFocus()
            && m_objSearchAdapter.GetFocusItemData(strDisplayName, strNumber, iCallID))
    {
        if (NULL != m_pLineEdit)
        {
            //不让他搜索
            m_pLineEdit->SetInitText(strNumber);
        }
    }
}

//输入预编辑字符
void CDlgDialingUI::InputLineEditData()
{
    //涉及到密码拨号可能会导致获取到的号码不正确， 所以打开槽信号
    //不让搜索
    //还是搜索
    ////    m_objPswLineEdit.blockSignals(true);
    //InputPreeditContent(&m_objPswLineEdit);
    ////    m_objPswLineEdit.blockSignals(false);

    //输出预编辑字符
    if (NULL != m_pLineEdit)
    {
        m_pLineEdit->OnPreInputConfirm();
    }
}

//是否有选中搜索结果
bool CDlgDialingUI::IsSearchTableSelectItem()
{
    if (NULL == m_pLineEdit)
    {
        return false;
    }

    //如果输入框有焦点，则应认为是没有选中项目
    if (m_pLineEdit->hasFocused())
    {
        return false;
    }

    yl::string strDisplayName = "";
    yl::string strNumber = "";
    int iCallID = -1;

    return m_objSearchAdapter.GetFocusItemData(strDisplayName, strNumber, iCallID);
}

//编辑框是否为空
bool CDlgDialingUI::IsEditLineEmpty()
{
    if (NULL != m_pLineEdit)
    {
        //dialprocessor在判断是否改变softkey、的时候、是否响按键音的时候使用， 所以这边不能调用输入法的接口， 因为可能导致预编辑字符输出
        //  return qReturnEdit(&m_objPswLineEdit).isEmpty();
        return m_pLineEdit->GetText().empty();
    }

    return false;
}

//获取穿梭数据callID
int CDlgDialingUI::GetShuttleCallID()
{
    yl::string strDisplayName = "";
    yl::string strNumber = "";
    int iCallID = -1;

    //获取焦点item 数据 成功 返回 获取到的callid
    if (m_objSearchAdapter.GetFocusItemData(strDisplayName, strNumber, iCallID))
    {
        return iCallID;
    }

    return -1;
}

//获取编辑框中的数据
yl::string CDlgDialingUI::GetLineEditData()
{
    if (NULL == m_pLineEdit)
    {
        return "";
    }

    //dialprocessor在判断是否改变softkey、的时候、是否响按键音的时候使用， 所以这边不能调用输入法的接口， 因为可能导致预编辑字符输出
    //  return fromQString(qReturnEdit(&m_objPswLineEdit));
    //输入法 返回的 密码拨号是重载过的 但是text函数没有重载
    return m_pLineEdit->GetText();
}

//内部控件焦点改变
bool CDlgDialingUI::onFocusMove(xView * pNewFocus, xView * pOldFocus)
{
    if (NULL != m_pProcessor)
    {
        m_pProcessor->UpdateTalkSoftkey();
    }

    if (pNewFocus == m_pLineEdit)
    {
        m_objSearchAdapter.SetNoFocus();
    }

    return false;
}

//处理IME输入法
void CDlgDialingUI::DealIMEAction()
{
    if (NULL != m_pLineEdit)
    {
        m_pLineEdit->SwitchIme();
        if (NULL != m_pProcessor)
        {
            m_pProcessor->UpdateTalkSoftkey();
        }
    }
}

//处理delete按键
void CDlgDialingUI::DealDeleteAction()
{
    if (NULL != m_pLineEdit)
    {
        m_pLineEdit->setFocus();
        m_pLineEdit->DeleteChar();
    }
}

//处理Pool的softkey
void CDlgDialingUI::DealPool()
{
    ProcessNormalOpenPool();
}

void CDlgDialingUI::DealHistory()
{
    DM_EnterDirectoryUI(CALL_LOG_TYPE_LOCAL, kszAllContactId);
}

//处理ST_Line 按键
void CDlgDialingUI::DealLineAction()
{
#if IF_TALKUI_MULTY_ACCOUNT

#else
    //
#endif
}

////////////////////////////////继承自  CUILogicDialUIBase /////////////////////

//编辑框内容变化
void CDlgDialingUI::slotTextChanged(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes,
                                    LPVOID pData)
{
    m_pLineEdit->setFocus();

    yl::string strText("");

    if (NULL != m_pLineEdit)
    {
        strText = m_pLineEdit->GetText();
    }

    //调用逻辑的 字符串变化
    if (NULL != m_pProcessor)
    {
        //先执行搜索  还是担心talkuilogic  OnTextChange  呼出 呼出那就不能执行搜索
        //  m_objSearchTable.show();
        m_objSearchAdapter.OnTextChange(strText);

        //UILogic 处理
        m_pProcessor->OnTextChange(strText);
    }
}

void CDlgDialingUI::slotSearchResultChange(xSignal * sender, WPARAM wParam, LPARAM lParam,
        int nDataBytes, LPVOID pData)
{
    if (!m_objSearchAdapter.GetListHaveData())
    {
        m_pLineEdit->setFocus();
    }
}

#ifdef IF_FEATURE_LONG_PRESS_DIGIT
void CDlgDialingUI::OnLongPressTimer(xThreadTimer * timer, LPVOID pExtraData)
{
    m_objLongPressTimer.KillTimer();

    if (m_pLineEdit != NULL)
    {
        int iCursorPos = m_pLineEdit->GetCursorPos();
        if (iCursorPos > 0)
        {
            yl::string strText = m_pLineEdit->GetText();
            yl::string strKey = GetStringByDigitKeyLongPress((char)('0' + strText[iCursorPos - 1] -
                                PHONE_KEY_0));

            if (!strKey.empty())
            {
                strText.replace(iCursorPos - 1, 1, strKey.c_str());

                m_pLineEdit->SetInitText(strText);

                m_pLineEdit->SetCursorPosition(iCursorPos);
            }
        }
    }
}

yl::string CDlgDialingUI::GetStringByDigitKeyLongPress(char chKey)
{
    switch (chKey)
    {
    case '0':
        return "+";
    default:
        return "";
    }
    return "";
}
#endif

void CDlgDialingUI::OpenLineSelectPage(vecLineSelectData & vecLineData, yl::string strTitle,
                                       bool bShowIndex, int nSelectID)
{
    talkui_OpenSelectPage(GetSessionID(), this, vecLineData, strTitle, bShowIndex, nSelectID);
}

void CDlgDialingUI::OnLineSelectResult(LINE_SELECT_RESULT eResult, int nSelectIndex, int nSelectID)
{
    CBaseTalkUIProcessor * pProcesser = GetBindUIProcessor();
    if (NULL != pProcesser)
    {
        pProcesser->OnLineSelectResult(eResult, nSelectIndex, nSelectID);
    }
}

void CDlgDialingUI::OnReturnToIdleAction()
{
    if (NULL != m_pProcessor)
    {
        m_pProcessor->DealCancel();
    }
}

bool CDlgDialingUI::NeedReUpdate()
{
    if (NULL != m_pProcessor)
    {
        return m_pProcessor->NeedReUpdate();
    }

    return true;
}

}       //namespace talkui

