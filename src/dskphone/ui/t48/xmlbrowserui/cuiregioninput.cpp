#include "cuiregioninput.h"
#include "cxmllabel.h"
#include "uikernel/scrollpage.h"
#include "qtcommon/qmisc.h"
#include "uimanager/rectdefine.h"
#include "xbpositiondefine.h"
#include "extended/xmlbrowser/include/modxmlbrowser.h"
#include "uikernel/qwidgetutility.h"
#include "inputmethod/inputmethod.h"

CUIRegionInput::CUIRegionInput(QWidget * parent/* = 0*/, Qt::WFlags flags/* = 0*/)
    : CUIRegionBase(parent, flags)
    , m_pInput(NULL)
    , m_eInputType(INPUT_IP)
    , m_bIsDefSoftKey(true)
{
    setObjectName("UIRegionInput");
    SetElementType(XMT_INPUT);

    // 初始化TitleLab
    m_pLabTitle = new AutoResizeLab(RECT_XMLBrowser.width(), 0, this);
    if (m_pLabTitle != NULL)
    {
        m_pLabTitle->setObjectName("LabTitle");
        m_pLabTitle->setGeometry(QRect(0, 0, RECT_XMLBrowser.width(), XML_TITLE_HEIGHT));

        // 设置Lab透明,并且字体颜色为白色
        QPalette pl = m_pLabTitle->palette();
        pl.setBrush(QPalette::Foreground, QBrush(QColor(255, 255, 255)));
        pl.setBrush(QPalette::Background, QBrush(QColor(255, 0, 0, 0)));
        m_pLabTitle->setPalette(pl);

        m_pLabTitle->setAlignment(Qt::AlignCenter);// 设置Title文字居中
        m_pLabTitle->SetCellRowHeight(XML_TITLE_HEIGHT);
        m_pLabTitle->setMaximumHeight(XML_TITLE_HEIGHT * 3);
    }

    m_pInput = new DlgInput(this);
    if (NULL != m_pInput)
    {
        m_pInput->setObjectName("Input");
        //连接焦改变信号
        connect(m_pInput, SIGNAL(FrameFocusChange()), this, SLOT(LineEditFocusChange()));
        m_pInput->setGeometry(0, XML_TITLE_HEIGHT, RECT_XMLBrowser.width()
                              , RECT_XMLBrowser.height() - XML_TITLE_HEIGHT);
    }
}

int CUIRegionInput::GetCurrentItem()
{
    //调用接口，返回当前焦点所在序号
    return m_pInput->GetFrameCurrentItem();
}

#if !IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
VecSoftkeyParam & CUIRegionInput::GetSoftkey()
{
    if (!m_bIsDefSoftKey)
    {
        return m_vecSoftkey;
    }

    m_vecSoftkey.clear();
    int iCurentIndex = GetCurrentItem();
    if (iCurentIndex  < m_listInputField.size())
    {
        InputField * pFild = m_listInputField[iCurentIndex];

        if (NULL == pFild)
        {
            m_vecSoftkey.clear();
            return m_vecSoftkey;
        }

        INPUT_TYPE eCurrentItemType = pFild->eType;

        if (INPUT_IP == eCurrentItemType)
        {
            m_vecSoftkey.push_back(TypeToSoftInfo(XB_ST_DOT));
            m_vecSoftkey.push_back(TypeToSoftInfo(XB_ST_BACKSPACE));
            m_vecSoftkey.push_back(TypeToSoftInfo(XB_ST_SUBMIT));
        }
        else if (INPUT_NUMBER == eCurrentItemType)
        {
            m_vecSoftkey.push_back(TypeToSoftInfo(XB_ST_BACKSPACE));
            m_vecSoftkey.push_back(TypeToSoftInfo(XB_ST_SUBMIT));
        }
        else if (INPUT_STRING == eCurrentItemType)
        {
            m_vecSoftkey.push_back(TypeToSoftInfo(XB_ST_CHANGEMODE));
            m_vecSoftkey.push_back(TypeToSoftInfo(XB_ST_BACKSPACE));
            m_vecSoftkey.push_back(TypeToSoftInfo(XB_ST_DOT));
            m_vecSoftkey.push_back(TypeToSoftInfo(XB_ST_NEXTSPACE));
            m_vecSoftkey.push_back(TypeToSoftInfo(XB_ST_SUBMIT));
        }
        else
        {
            m_vecSoftkey.push_back(TypeToSoftInfo(XB_ST_CHANGEMODE));
            m_vecSoftkey.push_back(TypeToSoftInfo(XB_ST_BACKSPACE));
            m_vecSoftkey.push_back(TypeToSoftInfo(XB_ST_SUBMIT));
        }
    }
    else
    {
        m_vecSoftkey.push_back(TypeToSoftInfo(XB_ST_CHANGEMODE));
        m_vecSoftkey.push_back(TypeToSoftInfo(XB_ST_BACKSPACE));
        m_vecSoftkey.push_back(TypeToSoftInfo(XB_ST_SUBMIT));
    }

    return m_vecSoftkey;
}
#endif

void CUIRegionInput::LineEditFocusChange()
{
#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
    VecSoftkeyParam vecPrivateSoftkey = m_pInput->GetInputFrame()->GetFocusSoftkey();
    if (vecPrivateSoftkey.size() > 0)
    {
        SetSoftkey(vecPrivateSoftkey);
    }
    else if (m_bIsDefSoftKey)
    {
        SetDefaultSoftkey(m_vecSoftkey);
    }
    else
    {
        SetSoftkey(m_vecGlobalSoftkey);
    }
#endif
    //焦点改变，发送信号
    emit RegionAction(REGION_ACTION_CURRENT_ITEM_CHANGE);
}

CUIRegionInput::~CUIRegionInput()
{

}

// 根据数据更新界面
void CUIRegionInput::Update(void * pData)
{
    // 入口参数检查
    if (pData == NULL)
    {
        return;
    }

    // 强制转换指针
    XBInputData * pNewData = static_cast<XBInputData *>(pData);
#if !IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
    m_listInputField.clear();
#endif
    // 更新属性
    if (m_pLabTitle != NULL)
    {
        // 设置Title的显示模式
        m_pLabTitle->setWordWrap(pNewData->stAttr.bWrapTitle);
        m_pLabTitle->SetLabText(toQString(pNewData->stAttr.strTitle.c_str()));
        qWidgetUtility::setFontStyle(m_pLabTitle, XB_INPUT_FRAME_DEFAULT_FONT_SIZE, QFont::Normal, false);
    }

    // 跟新锁定按键的变量
    m_bLock = pNewData->stAttr.bLockIn;

    //刷新input
    if (NULL != m_pInput)
    {
        m_pInput->setGeometry(0, m_pLabTitle->height(), RECT_XMLBrowser.width(),
                              this->height() - m_pLabTitle->height());
        m_pInput->SetFrameDisplayMode(pNewData->stAttr.eDisplayMode);  //调用接口，设置显示类型

        // 对数据进行转换
        std::list<InputField *> listInputField;
        List_Input_Field::iterator iter = pNewData->listItem.begin();
        for (int i = 0; iter != pNewData->listItem.end(); iter++, ++i)
        {
            listInputField.push_back(*iter);
#if !IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
            m_listInputField.push_back(*iter);
#endif
            if (i == pNewData->stAttr.iDefaultIndex)
            {
                // 默认焦点的输入框类型
                m_eInputType = (*iter)->eType;
            }
        }

        m_pInput->InitInputFrame(listInputField);       //生成input显示界面
        m_pInput->SetFocusItem(pNewData->stAttr.iDefaultIndex);     //设置默认焦点
    }

    m_iQuitTime = pNewData->stAttr.iTimeOut;
    //m_eInputType = pNewData->stAttr.eType;

    m_bIsDefSoftKey = (pNewData->vecSoftkey.size() == 0);

#if IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY
    m_vecGlobalSoftkey = pNewData->vecSoftkey;
    VecSoftkeyParam vecPrivateSoftkey = m_pInput->GetInputFrame()->GetFocusSoftkey();
    if (vecPrivateSoftkey.size() > 0)
    {
        SetSoftkey(vecPrivateSoftkey);
    }
    else
#endif
    {
        SetSoftkey(pNewData->vecSoftkey);
    }
}

// 获取页面的默认softkey数据
void CUIRegionInput::SetDefaultSoftkey(VecSoftkeyParam & softkeyParam)
{
    softkeyParam.clear();

    INPUT_TYPE eInputType = m_eInputType;
    if (m_pInput != NULL)
    {
        m_eInputType = m_pInput->GetInputFrame()->GetFoucsItemInputType();
        if (m_eInputType == -1)
        {
            // 刚显示时焦点的输入类型
            m_eInputType = eInputType;
        }
    }

    if (INPUT_IP == m_eInputType)
    {
        softkeyParam.push_back(TypeToSoftInfo(XB_ST_DOT));
        softkeyParam.push_back(TypeToSoftInfo(XB_ST_BACKSPACE));
        softkeyParam.push_back(TypeToSoftInfo(XB_ST_SUBMIT));
    }
    else if (INPUT_NUMBER == m_eInputType)
    {
        softkeyParam.push_back(TypeToSoftInfo(XB_ST_BACKSPACE));
        softkeyParam.push_back(TypeToSoftInfo(XB_ST_SUBMIT));
    }
    else if (INPUT_STRING == m_eInputType)
    {
        softkeyParam.push_back(TypeToSoftInfo(XB_ST_CHANGEMODE));
        softkeyParam.push_back(TypeToSoftInfo(XB_ST_BACKSPACE));
        softkeyParam.push_back(TypeToSoftInfo(XB_ST_DOT));
        softkeyParam.push_back(TypeToSoftInfo(XB_ST_NEXTSPACE));
        softkeyParam.push_back(TypeToSoftInfo(XB_ST_SUBMIT));
    }
    else
    {
        softkeyParam.push_back(TypeToSoftInfo(XB_ST_CHANGEMODE));
        softkeyParam.push_back(TypeToSoftInfo(XB_ST_BACKSPACE));
        softkeyParam.push_back(TypeToSoftInfo(XB_ST_SUBMIT));
    }
}

bool CUIRegionInput::ProcessKey(int nKey)
{
    if (PHONE_KEY_REDIAL == nKey)
    {
        SubmitInputInfo();
        return true;
    }

    return CUIRegionBase::ProcessKey(nKey);
}

// 事件过滤器
bool CUIRegionInput::eventFilter(QObject * pObj, QEvent * pEvent)
{
    switch (pEvent->type())
    {
    case QEvent::KeyPress:
        {
            // 调用基类的按键处理函数
            if (OnKeyPress(pObj, pEvent))
            {
                return true;
            }

            QKeyEvent * keyEvent = (QKeyEvent *)pEvent;
            int nKey = keyEvent->key();
            switch (nKey)
            {
            case PHONE_KEY_OK:
            case PHONE_KEY_REDIAL:
                {
                    SubmitInputInfo();
                    return true;
                }
                break;
            default:
                {
                    if (FullKeyboardEnabled() && NULL != m_pInput)
                    {
                        return m_pInput->eventFilter(pObj, pEvent);
                    }
                }
                break;
            }
        }
        break;
    default:
        break;
    }
    return false;
}

bool CUIRegionInput::GetPageData(MapPageData & PageData)
{
    return m_pInput->GetFrameLineEditValue(PageData);
}

void CUIRegionInput::OnKeyboardShow()
{
    if (m_pInput != NULL)
    {
        m_pInput->OnKeyboardShow();
    }
}

void CUIRegionInput::OnKeyboardHide()
{
    if (m_pInput != NULL)
    {
        m_pInput->OnKeyboardHide();
    }
}

void CUIRegionInput::SubmitInputInfo()
{
    // 获取当前界面输入的数据
    YLHashMap<int, yl::string> objMap;
    objMap.clear();
    if (!GetPageData(objMap))
    {
        LineEditFocusChange();
        return;
    }

    //按ok键confirm处理相同
    xmlbrowser_OnAction(GetElementID(), XB_ACTION_CONFIRM, (void *)&objMap);
}

