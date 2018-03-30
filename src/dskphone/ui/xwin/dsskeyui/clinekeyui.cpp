#include "clinekeyui.h"
#include "cdsskeybtnicon.h"
#include "dsskeyuilogic.h"
#include "moddsskeyui.h"
#include "dsskey/include/moddsskey.h"
#include "configparser/modconfigparser.h"
#include "configiddefine/configiddefine.h"
#include "xwindow/xWindowManager.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "dsskeystyle.h"
namespace
{
const char * DSSKEYBTN_LEFT_LAYOUT_FILE = "dsskey/btnalignlefticon.xml";
const char * DSSKEYBTN_RIGHT_LAYOUT_FILE = "dsskey/btnalignrighticon.xml";
const char * LINEKEY_UI_LAYOUT_FILE = "dsskey/linekeyui.xml";
const char * PAGEBTN_UI_LAYOUT_FILE = "dsskey/pagebutton.xml";
const char * LINEKEY_UI_LEFT_FRAME = "dlgLeftLinekey";
const char * LINEKEY_UI_RIGHT_FRAME = "dlgRightLinekey";

#define SHORTEN_DSSKEY_OFFSET 145
}

const char * DSSKEY_UI_ATTR_KEY_PER_PAGE = "keynumperpage";
const char * DSSKEY_UI_ATTR_MAX_PAGE = "maxpage";
const char * DSSKEY_UI_ATTR_KEY_TOTAL = "keytotalnum";
const char * DSSKEY_UI_ATTR_SHOW_FRAME_BG = "showframebg";
const char * DSSKEY_UI_ATTR_WIDTH_FULL = "keyfullwidth";
const char * DSSKEY_UI_ATTR_WIDTH_EXTEND = "keyextendwidth";
const char * DSSKEY_UI_ATTR_WIDTH_NORMAL = "keynormalwidth";
const char * DSSKEY_UI_ATTR_WIDTH_TWO_CHARS = "keytwocharswidth";
const char * DSSKEY_UI_ATTR_WIDTH_NO_LABEL = "keynolabelswidth";

#define SAFE_DELETE_POINTER(p)      if (p != NULL) \
                                    { \
                                    delete p; \
                                    p = NULL; \
                                    }

/************************************************************************/
/* CLinekeyUI                                                           */
/************************************************************************/

LRESULT CLinekeyUI::OnDlgLineKeyMessage(msgObject& objMessage)
{
    if (CONFIG_MSG_BCAST_CHANGED == objMessage.message)
    {
        if (ST_SHORT_LINEKEY_LABEL == objMessage.wParam)
        {
            g_pLinekeyUI->ChangeDsskeyLabelType();
        }
    }

    return FALSE;
}

// 定时器处理
LRESULT dsskey_OnTimer(msgObject & msg)
{
    return g_pLinekeyUI->OnTimer(msg.wParam);
}

CLinekeyUI::CLinekeyUI()
    : m_bLongPress(false)
    , m_iMaxNumPerPage(0)
    , m_iMaxPage(0)
    , m_iTotalKey(0)
    , m_iKeyFullWidth(0)
    , m_iKeyExtendWidth(0)
    , m_iKeyNormalWidth(0)
    , m_iKeyTwoCharWidth(0)
    , m_iKeyNoLabelWidth(0)
    , m_bShowFrameBg(false)
    , m_pPageBtn(NULL)
    , m_pBtnClickAction(NULL)
    , m_pBtnPressAction(NULL)
    , m_pBtnLongPressAction(NULL)
    , m_pDataDelegate(NULL)
{
    /*loadContent(LINEKEY_UI_LAYOUT_FILE);
    m_pWindow = m_pHostStation->createWindow(this, TML_USER + 2);
    SLOT_CONNECT(&ownerStation(), signalKeyEvent, this, CLinekeyUI::slotKeyEvent);*/
    //SLOT_CONNECT(&ownerStation(), signalLongKeyEvent, this, CLinekeyUI::slotKeyLongPressEvent);
}


CLinekeyUI::~CLinekeyUI()
{
}

void CLinekeyUI::Init()
{
    // TODO Init DsskeyUiLogic
    // DsskeyUI逻辑控制初始化
    DSSKEYUI_INFO(" ====== CLinekeyUI Init ====== ");
    _DssKeyUILogic.SetMaxNumPerPage(m_iMaxNumPerPage);
    _DssKeyUILogic.SetMaxPage(m_iMaxPage);
    _DssKeyUILogic.SetMaxNumber(m_iTotalKey);

    InitData();
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                          &CLinekeyUI::OnDlgLineKeyMessage);
    SingleMsgReg(TM_TIMER, dsskey_OnTimer);
}

void CLinekeyUI::Uninit()
{
    //
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                            &CLinekeyUI::OnDlgLineKeyMessage);
    SingleMsgUnReg(TM_TIMER, dsskey_OnTimer);
}

void CLinekeyUI::InitData()
{
    //CDssKeyPageButton * pPushBtn = NULL;
    CAbstractDsskeyBtnAction *pAction = NULL;
    CDsskeyDataDelegateBase *pDelegate = NULL;
#if IF_FEATURE_DSSKEY_ALIGN_MODE
    int iDsskeyAlignMode = configParser_GetConfigInt(kszDsskeyShowType);
#endif
    PHONE_TYPE_DEF ePhoneType = devicelib_GetPhoneType();
    for (int nIndex = 0; nIndex != m_iMaxNumPerPage; nIndex++)
    {
        CDsskeyButton *pBtn = new CDsskeyBtnIcon();
        if (pBtn != NULL)
        {
            pBtn->SetIndex(nIndex);
            if (PT_T23 == ePhoneType
                    || PT_T21 == ePhoneType)
            {
#if IF_FEATURE_DSSKEY_ALIGN_MODE
                if (iDsskeyAlignMode == 1)
                {
                    pBtn->loadContent(DSSKEYBTN_RIGHT_LAYOUT_FILE);
                    pBtn->SetBtnAlign(gravity_right | gravity_y_center);
                }
                else
                {
                    pBtn->loadContent(DSSKEYBTN_LEFT_LAYOUT_FILE);
                }
#else
                pBtn->loadContent(DSSKEYBTN_LEFT_LAYOUT_FILE);
#endif
                if (m_pRightFrame != NULL)
                {
                    m_pRightFrame->addChildView(pBtn);
                }
            }
            else
            {
                if (nIndex < m_iMaxNumPerPage / 2)
                {
                    pBtn->loadContent(DSSKEYBTN_LEFT_LAYOUT_FILE);

                    if (m_pLeftFrame != NULL)
                    {
                        m_pLeftFrame->addChildView(pBtn);
                    }
                }
                else
                {
#if IF_FEATURE_DSSKEY_ALIGN_MODE
                    if (iDsskeyAlignMode == 1)
                    {
                        pBtn->loadContent(DSSKEYBTN_RIGHT_LAYOUT_FILE);
                        pBtn->SetBtnAlign(gravity_right | gravity_y_center);
                    }
                    else
                    {
                        pBtn->loadContent(DSSKEYBTN_LEFT_LAYOUT_FILE);
                    }
#else
                    pBtn->loadContent(DSSKEYBTN_LEFT_LAYOUT_FILE);
#endif
                    if (m_pRightFrame != NULL)
                    {
                        m_pRightFrame->addChildView(pBtn);
                    }
                }
            }

            pBtn->SetupPressSlot(SLOT_METHOD(this, CLinekeyUI::slotOnLinekeypress));
            pBtn->SetupLongPressSlot(SLOT_METHOD(this, CLinekeyUI::slotOnLineKeyLonngPress));
            m_vecDsskeyBtns.push_back(pBtn);
        }
    }

    // 设置按钮动作
    // 设置点击按钮动作
    pAction = new CDsskeyFeatureAction;
    if (pAction != NULL)
    {
        SetBtnClickAction(pAction);
    }
    else
    {
        DSSKEY_WARN("pClickAction is NULL!.\n");
        return;
    }
    // 设置长按按钮动作
    pAction = NULL;
    pAction = new CDsskeyLongClickAction;
    if (pAction != NULL)
    {
        SetBtnLongPressAction(pAction);
    }
    else
    {
        DSSKEY_WARN("pLongAction is NULL!.\n");
        return;
    }

    // 设置切页按钮
    if (m_iMaxPage > 1)
    {
        m_pPageBtn = new CPageButton;

        if (m_pPageBtn != NULL)
        {
            m_pPageBtn->loadContent(PAGEBTN_UI_LAYOUT_FILE);
            m_pPageBtn->SetMaxPage(m_iMaxPage);
            m_pPageBtn->SetupPressSlot(SLOT_METHOD(this, CLinekeyUI::slotOnPageButtonClick));
            m_pRightFrame->addChildView(m_pPageBtn);
            //m_pPageBtn->hide(true);
        }
    }
    SetPageBtnVisible(false);

    // 初始化DsskeyBtn位置信息
    SetDisplayMode(DDM_NORMAL);


    // 设置Dsskey数据代理
    pDelegate = new CDsskeyDataDelegateBase;
    if (pDelegate != NULL)
    {
        SetDataDelegate(pDelegate);
        pDelegate->BindDsskeyFrame(this);
    }
    else
    {
        IDLE_WARN("pDelegate is NULL!.\n");
        return;
    }
}

//// 返回该窗口指针
CLinekeyUI* CLinekeyUI::CreateInstance()
{
    static CLinekeyUI* pDlgLineKeyUI = NULL;

    if (NULL == pDlgLineKeyUI)
    {
        pDlgLineKeyUI = new CLinekeyUI();
        //pDlgLineKeyUI->loadContent(LINEKEY_UI_LAYOUT_FILE);
        pDlgLineKeyUI->LoadContentFile();
    }

    return pDlgLineKeyUI;
}

bool CLinekeyUI::OnTimer(UINT uTimerID)
{
    if (m_pPageBtn != NULL)
    {
        m_pPageBtn->OnFlashTimer(uTimerID);
    }

    return false;
}
//
//CLinekeyUI* CLinekeyUI::GetInstance()
//{
//    static CLinekeyUI * pDlgLineKeyUI = static_cast<CLinekeyUI *>(UIManager_GetPrivateDlg(
//                                            DLG_LinekeyUI));
//    if (NULL == pDlgLineKeyUI)
//    {
//        pDlgLineKeyUI = static_cast<CLinekeyUI *>(UIManager_CreatePrivateDlg(DLG_LinekeyUI));
//    }
//
//    return pDlgLineKeyUI;
//}

void CLinekeyUI::LoadContentFile()
{
    if (PT_T19 == devicelib_GetPhoneType())
    {
        setDisable(true);
    }
    loadContent(LINEKEY_UI_LAYOUT_FILE);
    m_pWindow = m_pHostStation->createWindow(this, TML_USER + 3);
    SLOT_CONNECT(&ownerStation(), signalKeyEvent, this, CLinekeyUI::slotKeyEvent);
}

void CLinekeyUI::loadAttributes(xml_node& node)
{
    xFrameLayout::loadAttributes(node);

    PopupAndroidInt(node, DSSKEY_UI_ATTR_KEY_PER_PAGE, m_iMaxNumPerPage);
    PopupAndroidInt(node, DSSKEY_UI_ATTR_MAX_PAGE, m_iMaxPage);
    PopupAndroidInt(node, DSSKEY_UI_ATTR_KEY_TOTAL, m_iTotalKey);
    PopupAndroidBool(node, DSSKEY_UI_ATTR_SHOW_FRAME_BG, m_bShowFrameBg);

    PopupAndroidInt(node, DSSKEY_UI_ATTR_WIDTH_FULL, m_iKeyFullWidth);
    PopupAndroidInt(node, DSSKEY_UI_ATTR_WIDTH_EXTEND, m_iKeyExtendWidth);
    PopupAndroidInt(node, DSSKEY_UI_ATTR_WIDTH_NORMAL, m_iKeyNormalWidth);
    PopupAndroidInt(node, DSSKEY_UI_ATTR_WIDTH_TWO_CHARS, m_iKeyTwoCharWidth);
    PopupAndroidInt(node, DSSKEY_UI_ATTR_WIDTH_NO_LABEL, m_iKeyNoLabelWidth);

    DSSKEYUI_INFO(" ====== %d %d %d ====== ", m_iMaxNumPerPage, m_iMaxPage, m_iTotalKey);
}

void CLinekeyUI::loadChildrenElements(xml_node & node)
{
    xFrameLayout::loadChildrenElements(node);
    m_pLeftFrame = static_cast<xLinearLayout *>(getViewById(LINEKEY_UI_LEFT_FRAME));
    m_pRightFrame = static_cast<xLinearLayout *>(getViewById(LINEKEY_UI_RIGHT_FRAME));
}

void CLinekeyUI::slotOnLinekeypress(xSignal * sender, WPARAM wParam, LPARAM lParam,
                                    int nDataBytes, LPVOID pData)
{
    DSSKEYUI_INFO(" ====== dsskey press :%d", (int)lParam);
    //dsskey_OnDsskeyClick((int)lParam);
    if (NULL != m_pBtnClickAction)
    {
        m_pBtnClickAction->OnDsskeyAction((int)lParam);
    }
}

void CLinekeyUI::slotOnLineKeyLonngPress(xSignal * sender, WPARAM wParam, LPARAM lParam,
        int nDataBytes, LPVOID pData)
{
    DSSKEYUI_INFO(" ====== dsskey long press");
    m_bLongPress = true;
    if (NULL != m_pBtnLongPressAction)
    {
        m_pBtnLongPressAction->OnDsskeyAction((int)lParam);
    }
}

void CLinekeyUI::slotOnPageButtonClick(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData)
{
    DSSKEYUI_INFO(" ====== slotOnPageButtonClick  Press ====== ");
    _DssKeyUILogic.OnPageButtonPress();
}

void CLinekeyUI::emitHotkeySignal(int iItemIndex, xSignal* sender, bool bPress, bool bLongPressed)
{
    //

    int nDsskeyCount = m_vecDsskeyBtns.size();

    if (iItemIndex >= 0 && iItemIndex < nDsskeyCount)
    {
        if (m_vecDsskeyBtns[iItemIndex] != NULL)
        {

#if IF_FEATRUE_PTT
            if (!bPress)
            {
                int iDsskeyID = dsskeyUI_GetDsskeyIDByKeycode(iItemIndex + PHONE_KEY_LINE1);
                DSSKEYUI_INFO(" !====== dsskey_OnDsskeyRelease %d %d %d======! ", iDsskeyID, dsskey_IsSupportLongPress(iDsskeyID), dsskey_IsExistLongPress());

                if (dsskey_IsSupportLongPress(iDsskeyID) && dsskey_IsExistLongPress())
                {
                    dsskey_OnDsskeyRelease(iDsskeyID);
                    DSSKEYUI_INFO(" !====== dsskey_OnDsskeyRelease %d======! ", iDsskeyID);
                    return;
                }
            }
#endif

            if (iItemIndex == nDsskeyCount - 1
                    && m_pPageBtn != NULL
                    && m_pPageBtn->isVisible()
                    && _DssKeyUILogic.IsPageButtonEnable())
            {
                if (!m_bLongPress)
                {
                    m_pPageBtn->OnBtnClick(bPress);
                }
            }
            else
            {
                if (!bPress && !m_bLongPress)
                {
                    m_vecDsskeyBtns[iItemIndex]->OnBtnClick(bLongPressed);
                }
            }
        }
    }
}
void CLinekeyUI::RefreshDsskeyData(const DsskeyDisplayData & objDisplayData)
{
    CDsskeyButton* pBtn = findBtnByIndex(objDisplayData.iIndex);
    if (NULL != pBtn)
    {
        pBtn->UpdateDisplayStatus(objDisplayData);
    }
}

void CLinekeyUI::RefreshDsskeyData(int iIndex, const DsskeyDisplayData & objDisplayData)
{
    CDsskeyButton* pBtn = findBtnByIndex(iIndex);
    if (NULL != pBtn)
    {
        pBtn->UpdateDisplayStatus(objDisplayData);
    }
}

void CLinekeyUI::SetBtnClickAction(CAbstractDsskeyBtnAction * pAction)
{
    if (NULL == pAction)
    {
        return;
    }

    SAFE_DELETE_POINTER(m_pBtnClickAction);
    m_pBtnClickAction = pAction;
}

void CLinekeyUI::SetBtnPressAction(CAbstractDsskeyBtnAction * pAction)
{
    if (NULL == pAction)
    {
        return;
    }

    SAFE_DELETE_POINTER(m_pBtnPressAction);
    m_pBtnPressAction = pAction;
}

void CLinekeyUI::SetBtnLongPressAction(CAbstractDsskeyBtnAction * pAction)
{
    if (NULL == pAction)
    {
        return;
    }

    SAFE_DELETE_POINTER(m_pBtnLongPressAction);
    m_pBtnLongPressAction = pAction;
}

void CLinekeyUI::SetDataDelegate(CDsskeyDataDelegateBase * pDelegate)
{
    if (NULL == pDelegate)
    {
        return;
    }

    SAFE_DELETE_POINTER(m_pDataDelegate);
    m_pDataDelegate = pDelegate;
}

CDsskeyButton* CLinekeyUI::findBtnByIndex(int iBtnIndex)
{
    DSSKEY_BTN_IT it = m_vecDsskeyBtns.begin();
    CDsskeyButton* pDsskeyBtn = NULL;
    for (/* */; it != m_vecDsskeyBtns.end(); ++it)
    {
        if (NULL != (*it) && (*it)->GetIndex() == iBtnIndex)
        {
            pDsskeyBtn = (CDsskeyButton*)(*it);
        }
    }
    return pDsskeyBtn;
}

void CLinekeyUI::SetBtnDown(int iIndex, bool bPressed)
{
    CDsskeyButton* pBtn = findBtnByIndex(iIndex);
    if (NULL != pBtn)
    {
        pBtn->SetBtnDown(bPressed);
    }
}

// 设置显示模式
void CLinekeyUI::SetDisplayMode(DSSKEY_DISPLAY_MODE eDispMode)
{
    PHONE_TYPE_DEF ePhoneType = devicelib_GetPhoneType();
    if (PT_T19 == ePhoneType)
    {
        return;
    }
    // T69只有Normal长度
    if ((PT_T69 == ePhoneType || PT_T69S == ePhoneType)
            && DDM_FULL == eDispMode)
    {
        eDispMode = DDM_NORMAL;
    }
    m_eDisplayMode = eDispMode;

    PrepareWidgetBeforePaint();
    ChangeDssFrameGeometory();
}

int CLinekeyUI::GetDsskeyWidth()
{
    int nBtnwidth = 0;

    // 按键长度控制
    if (DDM_FULL == m_eDisplayMode)
    {
        nBtnwidth = m_iKeyFullWidth;
    }
    else if (DDM_EXTEND == m_eDisplayMode)
    {
        nBtnwidth = m_iKeyExtendWidth;
    }
    else if (DDM_NORMAL == m_eDisplayMode)
    {
        nBtnwidth = m_iKeyNormalWidth;
    }
    else if (DDM_TWO_CHARS == m_eDisplayMode)
    {
        nBtnwidth = m_iKeyTwoCharWidth;
    }
    else if (DDM_NO_LABEL == m_eDisplayMode)
    {
        nBtnwidth = m_iKeyNoLabelWidth;
    }
    else
    {
        nBtnwidth = m_iKeyNormalWidth;
    }

    return nBtnwidth;
}

bool CLinekeyUI::IsShowFrameBg() const
{
    return m_bShowFrameBg;
}

void CLinekeyUI::PrepareWidgetBeforePaint()
{
    int nDsskeyWidth = GetDsskeyWidth();

    PrepareLeftDssFrameBg();
    PrepareRightDssFrameBg();

    SetDsskeysLayout(nDsskeyWidth);
    SetPageBtnLayout(nDsskeyWidth);

    onChildrenLayoutChanged();

    DSSKEY_BTN_IT it = m_vecDsskeyBtns.begin();
    for (; it != m_vecDsskeyBtns.end(); ++it)
    {
        if (NULL != (*it))
        {
            (*it)->SetDisplayMode(m_eDisplayMode);
        }
    }

    if (NULL != m_pPageBtn)
    {
        m_pPageBtn->SetDisplayMode(m_eDisplayMode);
    }
}

void CLinekeyUI::PrepareLeftDssFrameBg()
{
    /*if (!IsShowFrameBg())
    {
        return;
    }*/

    if (NULL == m_pLeftFrame)
    {
        return;
    }

    int nFrameHeight = m_pLeftFrame->height();
    int nFrameWidth = m_pLeftFrame->width();

    int nDsskeyWidth = GetDsskeyWidth();

    int nLeftCutLineTotal = (_DssKeyUILogic.GetMaxNumPerPage() >> 1) - 1;

    if (m_pLeftFrame != NULL)
    {
        m_pLeftFrame->layout_width = nDsskeyWidth;
    }

    //xPixmap pmLeftFrameBg;
    //pmLeftFrameBg.Create(nFrameWidth, nFrameHeight);
    //pmLeftFrameBg.SetHasAlphaChannel(true);
    //// 初始化为透明色
    //pmLeftFrameBg.fill(QColor(0, 0, 0, 0));
    //xPainter objPainter(&pmLeftFrameBg);

    //Theme_Draw_9_Pix(objPainter, Trans_GetLeftDssKeyBg().c_str(),
    //                 QRect(0, 0, nDsskeyWidth + DSSKEY_FRAME_SHADOW_HORIZONTAL, nFrameHeight));

    //objPainter.setPen(DSSKEY_FRAME_CUTLINE_COLOR);

    //// 绘制左侧分割线
    //for (int nLeftCutLineIndex = 1; nLeftCutLineIndex <= nLeftCutLineTotal; ++nLeftCutLineIndex)
    //{
    //    objPainter.drawLine(0, nLeftCutLineIndex * DSSKEY_DSSKEYUI_V_DISTANCE + DSSKEY_DSSKEYUI_TOP_OFFSET,
    //                        nDsskeyWidth - DSSKEY_CUTLINE_HORIZONTAL_RETRACT,
    //                        nLeftCutLineIndex * DSSKEY_DSSKEYUI_V_DISTANCE + DSSKEY_DSSKEYUI_TOP_OFFSET);
    //}

    //objPainter.end();

    //m_pLeftFrame->setPalette(objPalette);
}

void CLinekeyUI::PrepareRightDssFrameBg()
{
    /*if (!IsShowFrameBg())
    {
        return;
    }*/

    if (NULL == m_pRightFrame)
    {
        return;
    }

    int nFrameHeight = m_pRightFrame->height();
    int nFrameWidth = m_pRightFrame->width();

    int nDsskeyWidth = GetDsskeyWidth();

    int nRightCutLineTotal = (_DssKeyUILogic.GetMaxNumPerPage() >> 1) - 1;

    if (m_pRightFrame != NULL)
    {
        chRect rc = m_pRightFrame->geometry();
        m_pRightFrame->layout_width = nDsskeyWidth;
    }

    //QPixmap pmRightFrameBg(nFrameWidth, nFrameHeight);
    //// 初始化为透明色
    //pmRightFrameBg.fill(QColor(0, 0, 0, 0));

    //if (IsChangePageBtnShow())
    //{
    //    --nRightCutLineTotal;
    //    nFrameHeight -= DSSKEY_DSSKEYUI_V_DISTANCE;
    //}

    //QPainter objPainter(&pmRightFrameBg);

    //Theme_Draw_9_Pix(objPainter, Trans_GetRightDssKeyBg().c_str(),
    //                 QRect(nFrameWidth - nDsskeyWidth - DSSKEY_FRAME_SHADOW_HORIZONTAL, 0,
    //                       nDsskeyWidth + DSSKEY_FRAME_SHADOW_HORIZONTAL, nFrameHeight));

    //objPainter.setPen(DSSKEY_FRAME_CUTLINE_COLOR);

    //// 绘制右侧分割线
    //for (int nRightCutLineIndex = 1; nRightCutLineIndex <= nRightCutLineTotal; ++nRightCutLineIndex)
    //{
    //    objPainter.drawLine(nFrameWidth - nDsskeyWidth + DSSKEY_CUTLINE_HORIZONTAL_RETRACT,
    //                        nRightCutLineIndex * DSSKEY_DSSKEYUI_V_DISTANCE + DSSKEY_DSSKEYUI_TOP_OFFSET,
    //                        nFrameWidth,
    //                        nRightCutLineIndex * DSSKEY_DSSKEYUI_V_DISTANCE + DSSKEY_DSSKEYUI_TOP_OFFSET);
    //}

    //objPainter.end();

    //QPalette objPalette = m_pRightDsskeyFrame->palette();
    //objPalette.setBrush(QPalette::Background, pmRightFrameBg);
    //m_pRightDsskeyFrame->setPalette(objPalette);
}

void CLinekeyUI::ChangeDssFrameGeometory()
{
    if (NULL == m_pLeftFrame || NULL == m_pRightFrame)
    {
        return;
    }

    int nDssFrameOffset = 0;

    if (DDM_NO_LABEL == m_eDisplayMode)
    {
        nDssFrameOffset = SHORTEN_DSSKEY_OFFSET;
    }

    //m_pLeftFrame->setGeometry(-nDssFrameOffset, 0, rect().width() >> 1, rect().height());
    //m_pRightFrame->setGeometry((this->width() >> 1) + nDssFrameOffset, 0, width() >> 1, height());
}

bool CLinekeyUI::SetBtnAlignByIndex(int iIndex, int iAlign)
{

    CDsskeyButton* pBtn = findBtnByIndex(iIndex);
    if (NULL != pBtn)
    {
        if (pBtn->GetAlign() != iAlign)
        {
            DSSKEYUI_INFO(" ======SetBtnAlignByIndex :%d pBtn->GetAlign():%d aling:%d====== ", iIndex, pBtn->GetAlign(), iAlign);
            ReplaceOldBtn(iIndex, iAlign, pBtn);
            return true;
        }
    }
    return false;
}

CDsskeyButton * CLinekeyUI::ReplaceOldBtn(int iIndex, int iAlign, CDsskeyButton * pBtn)
{
    xLinearLayout * pFrame = NULL;
    int iIndexInLayout = iIndex;
    if (iIndex < (_DssKeyUILogic.GetMaxNumPerPage() >> 1))
    {
        pFrame = m_pLeftFrame;
    }
    else
    {
        iIndexInLayout = iIndex - (_DssKeyUILogic.GetMaxNumPerPage() >> 1);
        pFrame = m_pRightFrame;
    }
    if (pFrame == NULL)
    {
        return NULL;
    }
    CDsskeyButton *pNewBtn = new CDsskeyBtnIcon;
    if (pNewBtn == NULL)
    {
        return NULL;
    }
    pFrame->popupChildView(pBtn);
    pNewBtn->SetIndex(iIndex);
    pNewBtn->loadContent((iAlign & gravity_left)
                         ? DSSKEYBTN_LEFT_LAYOUT_FILE
                         : DSSKEYBTN_RIGHT_LAYOUT_FILE);
    pNewBtn->SetBtnAlign(iAlign);
    pFrame->insertChildView(iIndexInLayout, pNewBtn);
    bool bVisible = pBtn->isVisible();
    if (!bVisible)
    {
        pNewBtn->hide(false);
    }

    pNewBtn->SetupPressSlot(SLOT_METHOD(this, CLinekeyUI::slotOnLinekeypress));
    pNewBtn->SetupLongPressSlot(SLOT_METHOD(this, CLinekeyUI::slotOnLineKeyLonngPress));

    if (iIndex == m_vecDsskeyBtns.size() - 1)
    {
        m_vecDsskeyBtns.removeAt(iIndex);
        m_vecDsskeyBtns.push_back(pNewBtn);
    }
    else
    {
        m_vecDsskeyBtns.removeAt(iIndex);
        m_vecDsskeyBtns.insertAt(iIndex, pNewBtn);
    }


    SAFE_DELETE_POINTER(pBtn);
    pBtn = pNewBtn;
    return pNewBtn;
}

void CLinekeyUI::SetDsskeysLayout(int nBtnWidth)
{
    if (NULL == m_pLeftFrame || NULL == m_pRightFrame)
    {
        return;
    }

#if IF_FEATURE_DSSKEY_ALIGN_MODE
    int iDsskeyAlignMode = configParser_GetConfigInt(kszDsskeyShowType);
    DSSKEYUI_INFO(" ====== SetDsskeysLayout iDsskeyAlignMode %d ====== ", iDsskeyAlignMode);
#endif

    bool bChangeAlign = false;
    for (int i = 0; i < _DssKeyUILogic.GetMaxNumPerPage(); ++i)
    {
        if (PT_T69 == devicelib_GetPhoneType() || PT_T69S == devicelib_GetPhoneType())
        {

        }
        else
        {
#if IF_FEATURE_DSSKEY_ALIGN_MODE
            if (i < (_DssKeyUILogic.GetMaxNumPerPage() >> 1))
            {
            }
            else
            {
                if (iDsskeyAlignMode == 1)
                {
                    bChangeAlign |= SetBtnAlignByIndex(i, gravity_right | gravity_y_center);
                }
                else
                {
                    bChangeAlign |= SetBtnAlignByIndex(i, gravity_left | gravity_y_center);
                }
            }
#endif
        }
    }

    if (bChangeAlign)
    {
        _DssKeyUILogic.RefreshMultipleDsskeyData(m_iMaxNumPerPage / 2, m_iMaxNumPerPage / 2);
    }
}

void CLinekeyUI::SetPageBtnLayout(int nBtnWidth)
{
    if (NULL == m_pRightFrame)
    {
        return;
    }
}

void CLinekeyUI::SetPageBtnVisible(bool bShow)
{
    CDsskeyButton * pLastBtn = *(--m_vecDsskeyBtns.end());
    if (NULL != m_pPageBtn && NULL != pLastBtn)
    {
        if (bShow)
        {
            m_pPageBtn->show();
            pLastBtn->hide(true);
        }
        else
        {
            m_pPageBtn->hide(true);
            //          pLastBtn->show();
        }

        PrepareRightDssFrameBg();
    }

#if IS_COLOR
    PHONE_TYPE_DEF ePhoneType = devicelib_GetPhoneType();
    if (PT_T54S == ePhoneType
            || PT_T52 == ePhoneType)
    {
        if (m_pRightFrame != NULL)
        {
            xStyle *style = xStyle::getObjectByName("CDsskeyFrameRightBGStyle");
            if (style != NULL)
            {
                static_cast<CDsskeyFrameRightBGStyle *>(style)->SetPageBtn(bShow);
            }
        }

    }
#endif
    update();
}

// 设置翻页键闪烁图标
void CLinekeyUI::FlashPageIcon(int pageNum, DSS_KEY_COLOR_TYPE eColor,
                               DSS_KEY_FLUSH_TYPE eType /*= REFRESH_TYPE_NO_FLUSH*/)
{
    if (NULL != m_pPageBtn)
    {
        if (LED_COLOR_OFF == eColor)
        {
            // 关灯，即停止闪烁
            m_pPageBtn->StopFlashPageIcon(pageNum);
        }
        else
        {
            m_pPageBtn->FlashPageIcon(pageNum, eColor, eType);
        }
    }
}

void CLinekeyUI::SetPageInfo(int iPageNum, int iPageIndex)
{
    if (NULL != m_pPageBtn)
    {
        m_pPageBtn->SetPageInfo(iPageNum, iPageIndex);
    }
}

bool CLinekeyUI::IsDsskeyInAnimation()
{
    return m_bInAnimation;
}

void CLinekeyUI::PlayShowDsskeyAnimation(DSSKEY_DISPLAY_MODE eDispMode)
{
    //m_eDisplayMode = eDispMode;
    // TODO ANIMATION
}

void CLinekeyUI::ChangeDsskeyLabelType()
{
    DSSKEY_LABEL_TYLE eLabelType = DSS_LT_NORMAL;

    if (configParser_GetConfigInt(kszDsskeySmallLabel) == 1)
    {
        eLabelType = DSS_LT_SHORT;
    }
    // TODO set label type
}

xView & currentLinekeyUI()
{
    return *g_pLinekeyUI;
}

//////////////////////////////////////////////////////////////////////////

