//mod-base
#include "talkui_define.h"
#include "talksessiontab.h"

//mod-ImageManager
#include "imagemanager/modimagemanager.h"

//mod-QtCommon
#include "qtcommon/qmisc.h"

//mod-UIKernel
#include "uikernel/languagehelper.h"
#include "uikernel/qscrolltextlabel.h"
#include "uikernel/scrollpage.h"
#include "uikernel/qtimepiece.h"

//mod-this(TalkUI)
#include "dlgbasetalkui.h"
#include "talkuimanager.h"
#include "commonfunction.h"
#include "qpictureanimation.h"
#include "uikernel/qwidgetutility.h"

#include "baseui/pagectrl.h"
#include "baseui/t4xpicpath.h"
#include "customtheme/modcustomtheme.h"
#include "broadsoft/callpark/include/callparkcommon.h"

#include "talk/talklogic/include/modtalklogic.h"
#include "talk/talklogic/include/callinfo.h"

const int S_TAB_ITEM_WIDTH(478);
const int S_TAB_PAGE_HEIGHT(345);
const int S_TAB_FOCUS_ITEM_HEIGHT(56);
const int S_TAB_ITEM_HEIGHT(51);
const int S_TAB_SCROLLBAR_WIDTH(48);
const int S_LEFT_OFFSET(4);

#define TEST_TALK 0

//构造函数
CTalkSessionTab::CTalkSessionTab(QWidget * pParent /* = 0 */)
    : qUIPushButton(pParent)
    , m_pTimePiece(NULL)
    , m_pLabel(NULL)
    , m_bFocusItem(false)
    , m_pLabelNext(NULL)
    , m_eLayout(TALK_TAB_LAYOUT_STYLE_NORMAL)
{
    QFont font;
    font.setPointSize(18);
    font.setWeight(75);
    font.setBold(true);

    // 初始化文字控件
    m_pLabel = new qScrollTextLabel(this);
    if (NULL != m_pLabel)
    {
        m_pLabel->setObjectName("Label");
        m_pLabel->setFont(font);
        m_pLabel->setGeometry(10, 0, 320, 50);
    }

    m_pLabelNext = new qScrollTextLabel(this);
    if (NULL != m_pLabelNext)
    {
        m_pLabelNext->setFont(font);
        m_pLabelNext->setGeometry(160, 0, 150, 50);
        m_pLabelNext->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
        m_pLabelNext->hide();
    }

    //初始化生成计时器控件
    m_pTimePiece = new QTimePiece(this);
    if (m_pTimePiece != NULL)
    {
        m_pTimePiece->setObjectName("TimePiece");
        CustomTheme_SetWidgetFontProperty(m_pTimePiece, CUSTOM_FONT_ID_TALK_NORMAL_TIMEPIECE, true,
                                          QFont::Bold);
        m_pTimePiece->setGeometry(360, 0, 120, 50);
    }

    //连接点击信号槽
    connect(this, SIGNAL(clicked()), this, SLOT(OnClicked()));
    connect(this, SIGNAL(pressed()), this, SLOT(OnPressed()));
}

//析构函数
CTalkSessionTab::~CTalkSessionTab()
{
    if (m_pTimePiece != NULL)
    {
        //停止计时器
        m_pTimePiece->StopTimer();
    }
}

void CTalkSessionTab::SetTabText(const QString & strText)
{
    if (NULL != m_pLabel)
    {
        m_pLabel->setText(strText);
        m_pLabel->update();
    }
}

void CTalkSessionTab::SetAnimationPic(const QString & strPic,
                                      const QString & strEnablePic,
                                      bool bLoop/* = false*/)
{

}

//设置显示时间
void CTalkSessionTab::SetTime(UINT uHour, UINT uMinute, UINT uSecond)
{
    if (m_pTimePiece != NULL)
    {
        m_pTimePiece->SetTime(uHour, uMinute, uSecond);
    }
}

//是否显示时间
void CTalkSessionTab::ActiveTimer(bool bShow)
{
    if (m_pTimePiece != NULL)
    {
        if (bShow)
        {
            m_pTimePiece->show();
            m_pTimePiece->StartTimer();
        }
        else
        {
            m_pTimePiece->hide();
            m_pTimePiece->StopTimer();
        }
    }
}

// 清空数据
void CTalkSessionTab::ClearData()
{
    m_bFocusItem = false;

    if (NULL != m_pLabel)
    {
        m_pLabel->clear();
    }

    if (NULL != m_pTimePiece)
    {
        m_pTimePiece->SetTime(0, 0, 0);
        m_pTimePiece->StopTimer();
    }
}

//响应点击事件槽
void CTalkSessionTab::OnClicked()
{
    //发送带自身指针的信号
    emit clicked(this);
}

void CTalkSessionTab::OnPressed()
{
    emit pressed(this);
}

// 自动调整控件布局
void CTalkSessionTab::AdjustLayout(bool bHasScrollBar)
{
    if (m_pTimePiece != NULL)
    {
        int iLeftOffset = (bHasScrollBar ? 310 : 360);
        m_pTimePiece->setGeometry(iLeftOffset, 0, 120, 50);
    }
}

void CTalkSessionTab::SetLayoutStyle(TALK_TAB_LAYOUT_STYLE eLayout)
{
    if (m_eLayout == eLayout)
    {
        return;
    }

    m_eLayout = eLayout;
    ReLayoutWidget();
}

void CTalkSessionTab::SetNextLabText(const QString& strText)
{
    if (NULL != m_pLabelNext)
    {
        m_pLabelNext->setText(strText);
        m_pLabelNext->update();
    }
}

void CTalkSessionTab::ReLayoutWidget()
{
    if (NULL == m_pLabel || NULL == m_pLabelNext)
    {
        return;
    }

    if (TALK_TAB_LAYOUT_STYLE_NORMAL == m_eLayout)
    {
        m_pLabel->setGeometry(10, 0, 320, 50);
        m_pLabel->show();
        m_pLabelNext->hide();
    }
    else
    {
        m_pLabel->setGeometry(10, 0, 180, 50);
        m_pLabelNext->setGeometry(190, 0, 120, 50);
        m_pLabel->show();
        m_pLabelNext->show();
    }
}

void CTalkSessionTab::paintEvent(QPaintEvent * pe)
{
    QStylePainter p(this);
    QPixmap pm;

    // 判断button的状态，设置对应的图片

    if (!this->isEnabled())
    {
        pm = THEME_GET_BMP(m_strDisalbePic.toUtf8().data());
    }
    else if (this->isDown()
             || this->isChecked())
    {
        pm = THEME_GET_BMP(m_strEnablePic.toUtf8().data());

    }
    else
    {
        pm = THEME_GET_BMP(m_strNormalPic.toUtf8().data());
    }

    QStyleOptionButton option;
    initStyleOption(&option);

    // 画按钮背景图片
    if (!pm.isNull())
    {
        p.drawPixmap(option.rect.x() + (option.rect.width() - pm.width()) / 2,
                     option.rect.y() + option.rect.height() / 2 - pm.height() / 2, pm);
    }

    QString tx = this->text();

    // 根据点击状态修改绘制控件的颜色
    if (this->isDown()
            || this->isChecked())
    {
        setTextColor(m_enableClrText);
    }
    else
    {
        setTextColor(m_disableClrText);
    }
}

//////////////////////////////////////////////////////////////////////////
//构造函数
CTalkTabManager::CTalkTabManager(QWidget * pParent /* = 0 */)
    : QFrame(pParent)
    , m_nCurrentPage(0)
    , m_pScrollBar(NULL)
    , m_bIsIncomingCallSessionShow(false)
{
    setObjectName("CTalkTabManager");

    m_frameTalkUIBG.setParent(this);
    m_frameTalkUIBG.setGeometry(0, 0, 486, 350);
    m_frameTalkUIBG.setObjectName("TalkUIBG");

    m_pScrollBar = new CPageCtrl(this, this);
    if (NULL != m_pScrollBar)
    {
        m_pScrollBar->setObjectName("ScrollBar");
        m_pScrollBar->SetRect(434, 3, 48, 337);
    }

    for (int nIndex = 0; nIndex < MAX_TALKMANAGER_SHOW_COUNT; ++nIndex)
    {
        m_arrayTalkTab[nIndex].setParent(this);
        m_arrayTalkTab[nIndex].setObjectName(QString("TalkTab%1").arg(nIndex));
        connect(&m_arrayTalkTab[nIndex], SIGNAL(clicked(CTalkSessionTab *)),
                this, SLOT(OnTalkManagerItemClicked(CTalkSessionTab *)));
        m_arrayTalkTab[nIndex].hide();
    }
}

//析构函数
CTalkTabManager::~CTalkTabManager()
{

}

//某个Item被选中
void CTalkTabManager::OnTalkManagerItemClicked(CTalkSessionTab * pItem)
{
    if (pItem != NULL)
    {
        //转换附加数据为整型
        bool bOk = false;
        int nSessionId = pItem->GetData().toInt(&bOk);
        if (bOk)
        {
            //发送通话选项被点击事件，带SessionId
            emit talkItemClicked(nSessionId);

            //调用Logic层的改变焦点事件
            talklogic_UIEvent(nSessionId, TALK_ACTION_FOCUSE_CHANGE, nSessionId, 0);
        }
    }
}

//加载数据
void CTalkTabManager::LoadData(List_AllSession_Data & listSessionData, int nBegin,
                               int nCountGet, int nFocusIndex)
{
    TALKUI_INFO("CTalkTabManager::LoadData Start");
    List_AllSession_Data::const_iterator itBegin = listSessionData.begin();
    int nSessionIndex = 0;
    for (/* */; itBegin != listSessionData.end(); ++nSessionIndex, ++itBegin)
    {
        if (nSessionIndex == nBegin)
        {
            break;
        }
    }

    //这边初始化成没有显示  焦点栏
    m_bIsIncomingCallSessionShow = false;
    int iIncomingCallSessionID =
        talklogic_GetIncomingCallSessionId();  //获取当前来电的 sessionID

    int iYOffset = 3;
    int nTabIndex = 0;
    for (/* */;
              nSessionIndex <  nBegin + nCountGet && nTabIndex < MAX_TALKMANAGER_SHOW_COUNT;
              ++nSessionIndex, ++itBegin, ++nTabIndex)
    {
        if (itBegin == listSessionData.end())
        {
            //未到达最大显示数，其余项隐藏
            m_arrayTalkTab[nTabIndex].hide();
            m_arrayTalkTab[nTabIndex].ClearData();
            continue;
        }

        bool bFoucsItem = (nSessionIndex == nFocusIndex);
        bool bFirstItem = (nSessionIndex == nBegin);
        bool bLastItem = (nSessionIndex == nCountGet + nBegin - 1);

        const DataLogic2UI & objLogic2UIData = *itBegin;

        //只有在talking 和 conference 界面需要显示新来电
        //检测是否是来电的sessionID
        if (IsSessionIDValid(iIncomingCallSessionID)
                && (iIncomingCallSessionID == objLogic2UIData.iSessionID))
        {
            //sessionID 相同那么设置显示
            m_bIsIncomingCallSessionShow = true;
        }

        yl::string strUIName = GetDlgNameAndCheckLogicData(objLogic2UIData);

        //将SessionId传入，用于标识
        m_arrayTalkTab[nTabIndex].SetData(QVariant::fromValue(objLogic2UIData.iSessionID));
        m_arrayTalkTab[nTabIndex].SetFocusFlag(bFoucsItem);
        m_arrayTalkTab[nTabIndex].SetUIName(strUIName.c_str());

        //每次刷新的时候初始化下布局模式
        m_arrayTalkTab[nTabIndex].SetNextLabText("");
        m_arrayTalkTab[nTabIndex].SetLayoutStyle(TALK_TAB_LAYOUT_STYLE_NORMAL);


        if (strUIName == DLG_Connect)
        {
            //Connect
            SetConnectTab(m_arrayTalkTab[nTabIndex], objLogic2UIData, bFoucsItem);
        }
        else if (strUIName == DLG_Ringing)
        {
            //Ringing
            SetRingingTab(m_arrayTalkTab[nTabIndex], objLogic2UIData, bFoucsItem);
        }
        else if (strUIName == DLG_Talking)
        {
            //Talking
            SetTalkingTab(m_arrayTalkTab[nTabIndex], objLogic2UIData, bFoucsItem);
        }
        else if (strUIName == DLG_Finished)
        {
            //Finished
            SetFinishedTab(m_arrayTalkTab[nTabIndex], objLogic2UIData, bFoucsItem);
        }
        else if (strUIName == DLG_Conference)
        {
            //Conference
            SetConferenceTab(m_arrayTalkTab[nTabIndex], objLogic2UIData, bFoucsItem);

            if (!bFoucsItem)
            {
                DelayFocusConferenceUI();
                return;
            }
        }
        else if (strUIName == DLG_ParkNotifyUI)
        {
            //ParkNotify
            SetParkNotifyTab(m_arrayTalkTab[nTabIndex], objLogic2UIData, bFoucsItem);
        }

        QColor objNormal = CustomTheme_GetCustomFontColorByKey(CUSTOM_FONT_ID_TALK_SESSITON_TITLE_NORMAL);
        QColor objFocus = CustomTheme_GetCustomFontColorByKey(CUSTOM_FONT_ID_TALK_SESSITON_TITLE_FOCUS);

        if (bFoucsItem)
        {
            QString strBtnBg;
            if (bFirstItem)
            {
                strBtnBg = PIC_THEME1_TAB_SELECT_TOP;
            }
            else
            {
                strBtnBg = PIC_THEME1_TAB_SELECT_MID;
            }

            m_arrayTalkTab[nTabIndex].set3StatePic(strBtnBg, strBtnBg, strBtnBg);
            m_arrayTalkTab[nTabIndex].SetEnableTextColor(objFocus);
            m_arrayTalkTab[nTabIndex].SetDisableTextColor(objFocus);
            m_arrayTalkTab[nTabIndex].setGeometry(S_LEFT_OFFSET, iYOffset, S_TAB_ITEM_WIDTH,
                                                  S_TAB_FOCUS_ITEM_HEIGHT);

            iYOffset += S_TAB_FOCUS_ITEM_HEIGHT;
            int nTalkUIHeight = S_TAB_PAGE_HEIGHT - S_TAB_ITEM_HEIGHT * (nCountGet - 1) -
                                S_TAB_FOCUS_ITEM_HEIGHT - 8;

            QRect rect = geometry();
            int iFocusUIWidth = S_TAB_ITEM_WIDTH - (m_vecPageInfo.size() > 1 ? S_TAB_SCROLLBAR_WIDTH : 0);
            emit FocusUIGeometryChange(true,
                                       QRect(rect.x() + S_LEFT_OFFSET,
                                             rect.y() + iYOffset - 4,
                                             iFocusUIWidth,
                                             nTalkUIHeight + 4));


            // 设置TalkUI子页面的背景
            m_frameTalkUIBG.setGeometry(0, iYOffset - 4, S_TAB_ITEM_WIDTH, nTalkUIHeight + 4);
            m_frameTalkUIBG.show();
            if (strUIName == DLG_Conference)
            {
                qWidgetUtility::SetBackGround(&m_frameTalkUIBG, PIC_GENERAL_TALKUI_CONFERENCE_BG);
            }
            else
            {
                if (nCountGet == 1)
                {
                    qWidgetUtility::SetBackGround(&m_frameTalkUIBG, PIC_GENERAL_TALKUI_ONE_SESSION_BG);
                }
                else if (nCountGet == 2)
                {
                    qWidgetUtility::SetBackGround(&m_frameTalkUIBG, PIC_GENERAL_TALKUI_TWO_SESSION_BG);
                }
                else if (nCountGet == 3)
                {
                    qWidgetUtility::SetBackGround(&m_frameTalkUIBG, PIC_GENERAL_TALKUI_THREE_SESSION_BG);
                }
                else
                {
                    // none
                }
            }

            iYOffset += nTalkUIHeight;
        }
        else
        {
            QString strBtnBg;
            QString strEnableBtnBg;
            if (bFirstItem)
            {
                strBtnBg = PIC_THEME1_TAB_NORMAL_TOP;
                strEnableBtnBg = PIC_THEME1_TAB_CLICK_TOP;
            }
            else if (bLastItem)
            {
                strBtnBg = PIC_THEME1_TAB_NORMAL_BOTTON;
                strEnableBtnBg = PIC_THEME1_TAB_CLICK_BOTTON;
            }
            else
            {
                strBtnBg = PIC_THEME1_TAB_NORMAL_MID;
                strEnableBtnBg = PIC_THEME1_TAB_CLICK_MID;
            }

            m_arrayTalkTab[nTabIndex].set3StatePic(strBtnBg, strEnableBtnBg, strBtnBg);
            m_arrayTalkTab[nTabIndex].SetEnableTextColor(objFocus);
            m_arrayTalkTab[nTabIndex].SetDisableTextColor(objNormal);
            m_arrayTalkTab[nTabIndex].setGeometry(S_LEFT_OFFSET, iYOffset, S_TAB_ITEM_WIDTH, S_TAB_ITEM_HEIGHT);

            iYOffset += S_TAB_ITEM_HEIGHT;
        }

        //显示对应项
        m_arrayTalkTab[nTabIndex].show();
    }

    // 隐藏多余的图标
    for (/* */; nTabIndex <  MAX_TALKMANAGER_SHOW_COUNT; ++nTabIndex)
    {
        ////----需要进行 clear操作
        m_arrayTalkTab[nTabIndex].hide();
        m_arrayTalkTab[nTabIndex].ClearData();
    }

    if (nFocusIndex < nBegin
            || nFocusIndex > nBegin + nCountGet - 1)
    {
        emit FocusUIGeometryChange(false, QRect(0, 0, 0, 0));
        m_frameTalkUIBG.hide();
    }

    TALKUI_INFO("CTalkTabManager::LoadData end");
}

void CTalkTabManager::ClearData()
{
    // 初始化数据
    m_nCurrentPage = 0;
    m_vecPageInfo.clear();

    for (int index = 0; index < MAX_TALKMANAGER_SHOW_COUNT; ++index)
    {
        m_arrayTalkTab[index].ClearData();
    }
}

void CTalkTabManager::RefreshContent(int iCurrentPage)
{
#if TEST_TALK
    List_AllSession_Data listSessionTemp;
    int nFocusIndex = 0;

    DataTalking2UI objTalkingData;
    DataLogic2UI objLogicData;

    objLogicData.iSessionID = 0;
    objLogicData.strUIName = DLG_Conference;
    objLogicData.pData = &objTalkingData;
    listSessionTemp.push_back(objLogicData);

    objLogicData.iSessionID = 1;
    objLogicData.strUIName = DLG_Talking;
    objLogicData.pData = &objTalkingData;
    listSessionTemp.push_back(objLogicData);
//
//  objLogicData.iSessionID = 2;
//  objLogicData.strUIName = DLG_Talking;
//  objLogicData.pData = &objTalkingData;
//  listSessionTemp.push_back(objLogicData);
//
//  objLogicData.iSessionID = 3;
//  objLogicData.strUIName = DLG_Talking;
//  objLogicData.pData = &objTalkingData;
//  listSessionTemp.push_back(objLogicData);
//
//  objLogicData.iSessionID = 4;
//  objLogicData.strUIName = DLG_Talking;
//  objLogicData.pData = &objTalkingData;
//  listSessionTemp.push_back(objLogicData);
#else
    //获取焦点列表与焦点索引
    YLVector<int> vecSessionData;
    vecSessionData.clear();
    talklogic_GetAllSessionID(vecSessionData);
    int nFocusIndex = talklogic_GetFocusedSessionIndex();
    /*
        const List_AllSession_Data& listSessionTemp = _TalklogicManager.GetSessionData(0, -1);
        int nFocusIndex = _TalklogicManager.GetFocusedSessionIndex();
    */
    /*
        const List_AllSession_Data& listSessionTemp = _TalklogicManager.GetSessionData(0, -1);
        int nFocusIndex = _TalklogicManager.GetFocusedSessionIndex();
    */
#endif
    TALKUI_INFO("RefreshContent SessionSize[%d]", vecSessionData.size());
    TALKUI_INFO("RefreshContent FocusIndex[%d]", nFocusIndex);

    // 去除类型为Dial的Session,将会议置顶
    List_AllSession_Data listSessionData;
    listSessionData.clear();
    for (int nSessionIndex = 0; nSessionIndex < vecSessionData.size(); ++nSessionIndex)
    {
        DataLogic2UI objDataToUI;
        bool bRet = talklogic_GetBaseDataBySessionID(vecSessionData[nSessionIndex], objDataToUI);
        if (bRet && GetDlgNameAndCheckLogicData(objDataToUI) != DLG_Dial
                && GetDlgNameAndCheckLogicData(objDataToUI) != DLG_Conference)
        {
            listSessionData.push_back(objDataToUI);
        }
        else if (bRet && GetDlgNameAndCheckLogicData(objDataToUI) == DLG_Conference)
        {
            //本地会议固定放在单独的第一页，解决：http://bugfree.yealink.com/Bug.php?BugID=115652
            listSessionData.push_front(objDataToUI);
            if (nFocusIndex == nSessionIndex)
            {
                nFocusIndex = 0;
            }
            else if (nFocusIndex < nSessionIndex)
            {
                nFocusIndex++;
            }

        }
        else
        {
            if (nSessionIndex < nFocusIndex)
            {
                --nFocusIndex;
            }
        }
    }

    if (listSessionData.size() == 0)
    {
        return;
    }

    /*
        // 去除类型为Dial的Session
        List_AllSession_Data listSessionData;
        List_AllSession_Data::const_iterator itBegin = listSessionTemp.begin();
        for (int iSessionIndex = 0; itBegin != listSessionTemp.end(); ++itBegin, ++iSessionIndex)
        {
            if (itBegin->strUIName != DLG_Dial)
            {
                listSessionData.push_back(*itBegin);
            }
            else
            {
                if (iSessionIndex < nFocusIndex)
                {
                    --nFocusIndex;
                }
            }
        }

        if (listSessionData.empty())
        {
            return;
        }
    */
    ////---- 需要确认边界值
    int nIndex = 0;
    int nCount = 0;
    m_vecPageInfo.clear();
    List_AllSession_Data::const_iterator itBegin = listSessionData.begin();
    for (/* */; itBegin != listSessionData.end(); ++nIndex, ++itBegin)
    {
        if (-1 == iCurrentPage && nFocusIndex == nIndex)
        {
            iCurrentPage = m_vecPageInfo.size();
        }

        if (GetDlgNameAndCheckLogicData(*itBegin) == DLG_Conference)
        {
            m_vecPageInfo.push_back(nIndex);
            continue;
        }

        ++nCount;
        if (3 == nCount)
        {
            m_vecPageInfo.push_back(nIndex);
            nCount = 0;
            continue;
        }
    }

    if (itBegin == listSessionData.end()
            && (m_vecPageInfo.empty() || m_vecPageInfo[m_vecPageInfo.size() - 1] != nIndex - 1))
    {
        m_vecPageInfo.push_back(nIndex - 1);
    }

    // 修正索引值
    if (iCurrentPage < 0 || iCurrentPage > m_vecPageInfo.size() - 1)
    {
        iCurrentPage = 0;
    }

    m_nCurrentPage = iCurrentPage;

    ////---- test code
    for (int i = 0; i < m_vecPageInfo.size(); ++i)
    {
        TALKUI_INFO("RefreshContent PageInfo[%d]", m_vecPageInfo[i]);
    }

    int nBegin = (m_nCurrentPage - 1 < 0) ? 0 : m_vecPageInfo[m_nCurrentPage - 1] + 1;
    int nGetCount = m_vecPageInfo[m_nCurrentPage] - nBegin + 1;

    TALKUI_INFO("RefreshContent BeginIndex[%d]", nBegin);
    TALKUI_INFO("RefreshContent PageCount[%d]", nGetCount);

    //加载数据
    LoadData(listSessionData, nBegin, nGetCount, nFocusIndex);

    // 设置滚动条属性
    if (m_vecPageInfo.size() > 1)
    {
        if (m_pScrollBar->isHidden())
        {
            m_pScrollBar->show();
            m_pScrollBar->raise();
            AdjustItemLayout();
        }

        m_pScrollBar->SetScrollInfo(m_vecPageInfo.size(), m_nCurrentPage);
        m_pScrollBar->update();
    }
    else
    {
        if (!m_pScrollBar->isHidden())
        {
            m_pScrollBar->hide();
            AdjustItemLayout();
        }
    }
}

//切换至上一页
void CTalkTabManager::OnNotifyScrollPrevious()
{
    // 页数为1页的时候不需要翻页
    if (m_vecPageInfo.size() < 2)
    {
        return;
    }

    //计算新的页面索引
    int iPage = (m_nCurrentPage - 1 < 0) ? m_vecPageInfo.size() - 1 : m_nCurrentPage - 1;

    // 防止索引越界
    if (iPage < 0 || iPage > m_vecPageInfo.size() - 1)
    {
        iPage = 0;
    }

    //索引不一致才进行刷新
    if (iPage != m_nCurrentPage)
    {
        m_nCurrentPage = iPage;
        RefreshContent(m_nCurrentPage);
    }

    //通知已经有滚动
    emit SigNotifyScroll();
}

//切换至下一页
void CTalkTabManager::OnNotifyScrollNext()
{
    // 页数为1页的时候不需要翻页
    if (m_vecPageInfo.size() < 2)
    {
        return;
    }

    //计算新的页面索引
    int iPage = (m_nCurrentPage + 1 > m_vecPageInfo.size() - 1) ? 0 : m_nCurrentPage + 1;

    // 防止索引越界
    if (iPage < 0 || iPage > m_vecPageInfo.size() - 1)
    {
        iPage = 0;
    }

    //索引不一致才进行刷新
    if (iPage != m_nCurrentPage)
    {
        m_nCurrentPage = iPage;
        RefreshContent(m_nCurrentPage);
    }


    //通知已经有滚动
    emit SigNotifyScroll();
}

//返回当前是否已经显示了 来电session
bool CTalkTabManager::GetIsShowIncomingCallSession()
{
    return m_bIsIncomingCallSessionShow;
}

int CTalkTabManager::GetPageSize()
{
    return m_vecPageInfo.size();
}

//设置连接类型项
void CTalkTabManager::SetConnectTab(CTalkSessionTab & objItem,
                                    const DataLogic2UI & objData, bool bFocus)
{
    if (GetDlgNameAndCheckLogicData(objData) != DLG_Connect)
    {
        //数据非连接相关类型，不处理
        return;
    }

    DataConnect * pConnectData = dynamic_cast<DataConnect *>(objData.pData);
    if (pConnectData != NULL)
    {
        if (pConnectData->pCallInfo != NULL)
        {
            QString strTitleTip = "";
            if (pConnectData->bConnectByTran)
            {
                strTitleTip = LANG_TRANSLATE(TRID_TRANSFERING);
            }
            else
            {
                strTitleTip = LANG_TRANSLATE(TRID_OUTGOING_CALL);
            }

#if IF_BUG_31481
            if (!pCallInfo->m_tRemoteInfo.m_strCallTextInfo.empty())
            {
                //设置提示文本
                objItem.SetTabText(toQString(pCallInfo->m_tRemoteInfo.m_strCallTextInfo.c_str()));
            }
            else
#endif
            {
                //设置提示文本
                objItem.SetTabText(GetTabTip(strTitleTip, pConnectData->pCallInfo));
            }
        }
    }

    //设置时间
    objItem.SetTime(0, 0, 0);
    objItem.ActiveTimer(false);

    objItem.SetAnimationPic(bFocus ? PIC_ICON_TAB_OUTGOINGCALL_ENABLE :
                            PIC_ICON_TAB_OUTGOINGCALL_DISENABLE,
                            PIC_ICON_TAB_OUTGOINGCALL_ENABLE);
}

//设置来电类型项
void CTalkTabManager::SetRingingTab(CTalkSessionTab & objItem,
                                    const DataLogic2UI & objData, bool bFocus)
{
    if (GetDlgNameAndCheckLogicData(objData) != DLG_Ringing)
    {
        //数据非来电相关类型，不处理
        return;
    }

    DataRing * pDataRing = dynamic_cast<DataRing *>(objData.pData);
    if (pDataRing != NULL)
    {
        if (pDataRing->pCallInfo != NULL)
        {
#if IF_BUG_31481
            if (!pCallInfo->m_tRemoteInfo.m_strCallTextInfo.empty())
            {
                //设置提示文本
                objItem.SetTabText(toQString(pCallInfo->m_tRemoteInfo.m_strCallTextInfo.c_str()));
            }
            else
#endif
            {
                //设置提示文本
                objItem.SetTabText(GetTabTip(LANG_TRANSLATE(TRID_INCOMING_CALL), pDataRing->pCallInfo));
            }
        }
    }

    //设置时间
    objItem.SetTime(0, 0, 0);
    objItem.ActiveTimer(false);

    // 设置话路图片
    objItem.SetAnimationPic(bFocus ? PIC_ICON_TAB_INCOMINGCALL_ENABLE :
                            PIC_ICON_TAB_INCOMINGCALL_DISENABLE,
                            PIC_ICON_TAB_INCOMINGCALL_ENABLE);
}

//设置通话类型项
void CTalkTabManager::SetTalkingTab(CTalkSessionTab & objItem,
                                    const DataLogic2UI & objData, bool bFocus)
{
    if (GetDlgNameAndCheckLogicData(objData) != DLG_Talking)
    {
        //数据非通话相关类型，不处理
        return;
    }

    DataTalking2UI * pTalkingData = dynamic_cast<DataTalking2UI *>(objData.pData);
    if (pTalkingData != NULL)
    {
        bool bHold = false;
        CCallInfo * pCallInfo = pTalkingData->pCallInfo;
        if (pCallInfo != NULL)
        {
            //设置提示文本
            QString strTitleTip = "";
            //http://10.2.1.199/Bug.php?BugID=89074 如果有hold则都显示hold
            if (pCallInfo->IsLocalHold())
            {
                strTitleTip = LANG_TRANSLATE(TRID_HOLD);
            }
            else if (pCallInfo->IsRemoteHold())
            {
                strTitleTip = LANG_TRANSLATE(TRID_HELD);
            }
            else
            {
                switch (pTalkingData->eTalkUIType)
                {
                case TALKING_UI_NORMAL:
                    {
                        bool bResult = (pTalkingData->listNetUserInfo.size() > 0);

                        if (bResult)
                        {
                            strTitleTip = toQString(pTalkingData->strConfName);
                        }
                        else
                        {
                            strTitleTip = LANG_TRANSLATE(TRID_TALKING);
                        }
                    }
                    break;
                case TALKING_UI_TRANSFERRING:
                    {
                        strTitleTip = LANG_TRANSLATE(TRID_TRANSFERING);
                    }
                    break;
                case TALKING_UI_TRANSFERRED:
                    {
                        //Transferred通话情况
                        strTitleTip = LANG_TRANSLATE(TRID_TRANSFERED);
                    }
                    break;
                case TALKING_UI_BETRANSFER:
                    {
                        //被转通话情况
                        strTitleTip = LANG_TRANSLATE(TRID_BE_TRANSGERRED);
                    }
                    break;
                case TALKING_UI_NETWORKCONF:
                    {
                        bool bResult = (pTalkingData->listNetUserInfo.size() > 0);

                        if (bResult)
                        {
                            strTitleTip = toQString(pTalkingData->strConfName);
                        }
                        else
                        {
                            strTitleTip = LANG_TRANSLATE(TRID_CONFERENCE);
                        }
                    }
                    break;
                default:
                    {
                        //未知类型
                        chWARNING2(FALSE, commonAPI_FormatString("Talk ui type(%d) is unknown!",
                                   pTalkingData->eTalkUIType).c_str());
                        strTitleTip = "";
                    }
                    break;
                }
            }

#if IF_BUG_31481
            if (!pCallInfo->m_tRemoteInfo.m_strCallTextInfo.empty())
            {
                //设置提示文本
                objItem.SetTabText(toQString(pCallInfo->m_tRemoteInfo.m_strCallTextInfo.c_str()));
            }
            else
#endif
            {
                if (pTalkingData->listNetUserInfo.size() > 0)
                {
                    objItem.SetTabText(strTitleTip);
                }
                else
                {
                    //设置提示文本
                    objItem.SetTabText(GetTabTip(strTitleTip, pTalkingData->pCallInfo));
                }
            }

            if (pTalkingData->listNetUserInfo.size() > 0)
            {
                int nTotal = pTalkingData->listNetUserInfo.size();
                objItem.SetNextLabText(QString::number(nTotal) + " " + LANG_TRANSLATE(TRID_LITTLE_MEMBERS));
                objItem.SetLayoutStyle(TALK_TAB_LAYOUT_STYLE_SHOW_SECOND_LAB);
            }

            //设置时间
            QCustomTime timeTalk(0, 0, 0);
            //获取窗口指针
            //  CDlgBaseTalkUI* pTalkUI = _TalkUIManager.FindTalkUIBySessionId(objData.iSessionID);
            //改成获取 通话界面的时间
            CDlgBaseTalkUI * pTalkUI = _TalkUIManager.FindTalkUIBySessionId(objData.iSessionID, DLG_Talking);
            if (pTalkUI != NULL)
            {
                //获取通话时间
                timeTalk = pTalkUI->GetTalkTime();
            }

            objItem.ActiveTimer(true);
            objItem.SetTime(timeTalk.hour(), timeTalk.minute(), timeTalk.second());

            if (bHold)
            {
                objItem.SetAnimationPic(bFocus ? PIC_ICON_TAB_HOLD_ENABLE : PIC_ICON_TAB_HOLD_DISENABLE,
                                        PIC_ICON_TAB_HOLD_ENABLE);
            }
            else
            {
                objItem.SetAnimationPic(bFocus ? PIC_ICON_TAB_TALKING_ENABLE : PIC_ICON_TAB_TALKING_DISENABLE,
                                        PIC_ICON_TAB_TALKING_ENABLE);
            }
        }
    }
}

//设置结束类型项
void CTalkTabManager::SetFinishedTab(CTalkSessionTab & objItem,
                                     const DataLogic2UI & objData, bool bFocus)
{
    if (GetDlgNameAndCheckLogicData(objData) != DLG_Finished)
    {
        //数据非结束相关类型，不处理
        return;
    }

    DataCallFinished * pFinishData = dynamic_cast<DataCallFinished *>(objData.pData);
    if (pFinishData != NULL)
    {
        //设置提示文本
        QString strTitleTip = "";
        if (pFinishData->bError)
        {
            //通话异常情况
            strTitleTip = LANG_TRANSLATE(TRID_CALL_FAIL);
        }
        else
        {
            //正常通话结束情况
            strTitleTip = LANG_TRANSLATE(TRID_CALL_FINISH);
        }

        CCallInfo * pCallInfo = pFinishData->pCallInfo;
        if (pCallInfo != NULL)
        {
#if IF_BUG_31481
            if (!pCallInfo->m_tRemoteInfo.m_strCallTextInfo.empty())
            {
                //设置提示文本
                objItem.SetTabText(toQString(pCallInfo->m_tRemoteInfo.m_strCallTextInfo.c_str()));
            }
            else
#endif
            {
                //设置提示文本
                objItem.SetTabText(GetTabTip(strTitleTip, pCallInfo));
            }
        }
    }

    //设置时间
    objItem.SetTime(0, 0, 0);
    objItem.ActiveTimer(false);

    // 设置话路图片
    objItem.SetAnimationPic(bFocus ? PIC_ICON_TAB_TALKING_ENABLE : PIC_ICON_TAB_TALKING_DISENABLE,
                            PIC_ICON_TAB_TALKING_ENABLE);
}

//设置会议类型项
void CTalkTabManager::SetConferenceTab(CTalkSessionTab & objItem,
                                       const DataLogic2UI & objData, bool bFocus)
{
    if (GetDlgNameAndCheckLogicData(objData) != DLG_Conference)
    {
        //数据非会议相关类型，不处理
        return;
    }

    DataLocalConf * pLocalConfData = dynamic_cast<DataLocalConf *>(objData.pData);
    if (pLocalConfData != NULL)
    {
        //设置提示文本
        //任意取出一路CallInfo作为Conf-Hold信息
        bool bAllHoldTip = true;
        yl::string strName;
        YLList<CCallInfo *>::iterator itCallInfo = pLocalConfData->listCallInfo.begin();
        for (int nIndex = 0; itCallInfo != pLocalConfData->listCallInfo.end()
                && nIndex < MAX_CONF_PARTY_NUM; ++itCallInfo, ++nIndex)
        {
            CCallInfo * pCallInfo = (CCallInfo *)(*itCallInfo);
            if (pCallInfo != NULL)
            {
                bAllHoldTip = bAllHoldTip && pCallInfo->IsLocalHold();

#if IF_BUG_31481
                if (!pCallInfo->m_tRemoteInfo.m_strCallTextInfo.empty())
                {
                    //设置提示文本
                    strName += pCallInfo->m_tRemoteInfo.m_strCallTextInfo;
                }
                else
#endif
                {
                    strName += pCallInfo->m_tRemoteInfo.sDisplayName.empty() ?
                               pCallInfo->m_tRemoteInfo.sSIPName :
                               pCallInfo->m_tRemoteInfo.sDisplayName;
                }

                if (nIndex != MAX_CONF_PARTY_NUM - 1)
                {
                    strName += ",";
                }
            }
        }

        if (bAllHoldTip)
        {
            // 在title显示安全级别 Security Classification 需求
            yl::string strSecurityLev = talklogic_GetTalkSecurityLevel(objData.iSessionID);
            if (pLocalConfData->bAllowSecurity && !strSecurityLev.empty())
            {
                //Hold
                objItem.SetTabText(LANG_TRANSLATE(TRID_HOLD) +
                                   " : " + LANG_TRANSLATE(TRID_CONFERENCE) + " - " + LANG_TRANSLATE(toQString(strSecurityLev)));
            }
            else
            {
                //Hold
                objItem.SetTabText(LANG_TRANSLATE(TRID_HOLD) +
                                   " : " + LANG_TRANSLATE(TRID_CONFERENCE));
            }

        }
        else
        {
            // 在title显示安全级别 Security Classification 需求
            yl::string strSecurityLev = talklogic_GetTalkSecurityLevel(objData.iSessionID);
            if (pLocalConfData->bAllowSecurity && !strSecurityLev.empty())
            {
                objItem.SetTabText(LANG_TRANSLATE(TRID_CONFERENCE) + " - " + LANG_TRANSLATE(toQString(
                                       strSecurityLev)));
            }
            else
            {
                //Normal
                objItem.SetTabText(LANG_TRANSLATE(TRID_CONFERENCE));
            }
        }

        //获取窗口指针
        //  CDlgBaseTalkUI* pTalkUI = _TalkUIManager.FindTalkUIBySessionId(objData.iSessionID);
        //改成获取会议界面的时间
        CDlgBaseTalkUI * pTalkUI = _TalkUIManager.FindTalkUIBySessionId(objData.iSessionID, DLG_Conference);
        //!!!获取时间
        QCustomTime timeTalk(0, 0, 0);
        if (pTalkUI != NULL)
        {
            timeTalk = pTalkUI->GetTalkTime();
        }

        objItem.ActiveTimer(true);
        objItem.SetTime(timeTalk.hour(), timeTalk.minute(), timeTalk.second());

        // 设置话路图片
        objItem.SetAnimationPic(bFocus ? PIC_ICON_TAB_TALKING_ENABLE : PIC_ICON_TAB_TALKING_DISENABLE,
                                PIC_ICON_TAB_TALKING_ENABLE);
    }
}
//设置ParkNotify类型
void CTalkTabManager::SetParkNotifyTab(CTalkSessionTab & objItem, const DataLogic2UI & objData,
                                       bool bFocus)
{
    if (GetDlgNameAndCheckLogicData(objData) != DLG_ParkNotifyUI)
    {
        //数据非通话相关类型，不处理
        return;
    }

    DataParkNotify * pNotifyData = dynamic_cast<DataParkNotify *>(objData.pData);
    if (pNotifyData != NULL)
    {
        //设置提示文本
        objItem.SetTabText(GetTabTip(LANG_TRANSLATE(TRID_CALL_PARKED),
                                     toQString(pNotifyData->strDiaplayName.c_str()),
                                     toQString(pNotifyData->strNumber.c_str())));
        //设置时间
        QCustomTime timeTalk(0, 0, 0);
        //获取窗口指针
        CDlgBaseTalkUI * pTalkUI = _TalkUIManager.FindTalkUIBySessionId(objData.iSessionID,
                                   DLG_ParkNotifyUI);
        if (pTalkUI != NULL)
        {
            //获取通话时间
            timeTalk = pTalkUI->GetTalkTime();

            //http://10.3.5.199/Bug.php?BugID=66210
            //qt意见容错处理， 子界面不存在的时候不显示计时
            objItem.ActiveTimer(true);
            objItem.SetTime(timeTalk.hour(), timeTalk.minute(), timeTalk.second());
        }
        else
        {
            //http://10.3.5.199/Bug.php?BugID=66210
            //qt意见容错处理， 子界面不存在的时候不显示计时
            objItem.ActiveTimer(false);
        }

        // 设置话路图片
        objItem.SetAnimationPic(bFocus ? PIC_ICON_TAB_HOLD_ENABLE : PIC_ICON_TAB_HOLD_DISENABLE,
                                PIC_ICON_TAB_HOLD_ENABLE);
    }
}

QString CTalkTabManager::GetTabTip(const QString & strStatus, const QString & strName,
                                   const QString & strNumber)
{
    yl::string strTransName = strName.toUtf8().data();
    yl::string strTransNumber = strNumber.toUtf8().data();
    strTransName = TransTalkText(strTransName);
    strTransNumber = TransTalkText(strTransNumber);


    // 号码优先需要显示号码，其他未默认
    int method = configParser_GetConfigInt(kszDisplayMethod);
    if (method == SHOW_NUMBER_AND_NAME
            || method == SHOW_ONLY_NUMBER)
    {
        strTransName = "";
    }
#if IF_BUG_29709
    else if (method == SHOW_NULL)
    {
        if ("anonymous" != strTransNumber.MakeLower()
                && "private number" != strTransNumber.MakeLower())
        {
            strTransName = "";
        }
        strTransNumber = "";
    }
#endif

    return ((strStatus.isEmpty() ? "" : strStatus  + " : ") + (strTransName.empty() ? toQString(
                strTransNumber) : toQString(strTransName)));

    //return ((strStatus.isEmpty() ? "" : strStatus  + " : ") + (strName.isEmpty() ? strNumber : strName));
}

QString CTalkTabManager::GetTabTip(const QString & strStatus, CCallInfo * pCallInfo)
{
    if (NULL == pCallInfo)
    {
        return "";
    }

    yl::string strName = "";

    switch (configParser_GetConfigInt(kszDisplayMethod))
    {
    case SHOW_NAME_AND_NUMBER:
    case SHOW_ONLY_NAME:
        {
            strName = GetNameFromCallInfo(pCallInfo);
        }
        break;
    case SHOW_NUMBER_AND_NAME:
    case SHOW_ONLY_NUMBER:
        {
            strName = GetNumberFromCallInfo(pCallInfo);
        }
        break;
    case SHOW_FULL_CONTACT_INFO:
        {
            strName = GetFullContactInfoFromCallInfo(pCallInfo);
        }
        break;
#if IF_BUG_29709
    case SHOW_NULL:
        {
            strName = GetNumberFromCallInfo(pCallInfo);
            if ("anonymous" != strName.MakeLower()
                    && "private number" != strName.MakeLower())
            {
                strName = "";
            }
        }
        break;
#endif
    default:
        {
            strName = GetNameFromCallInfo(pCallInfo);
        }
        break;
    }

    return ((strStatus.isEmpty() ? "" : strStatus  + " : ") + toQString(strName.c_str()));
}

void CTalkTabManager::AdjustItemLayout()
{
    for (int i = 0; i < MAX_TALKMANAGER_SHOW_COUNT; ++i)
    {
        m_arrayTalkTab[i].AdjustLayout(m_vecPageInfo.size() > 1);
    }
}

void CTalkTabManager::DelayFocusConferenceUI()
{
    if (m_arrayTalkTab[0].GetUIName() == DLG_Conference
            && !m_arrayTalkTab[0].GetFocusFlag())
    {
        OnTalkManagerItemClicked(&m_arrayTalkTab[0]);
    }
}
