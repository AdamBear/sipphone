#include "cuiregiontextscreen.h"
#include "xmlbrowserui/xmlwidgetpool.h"
#include "cxmllabel.h"
#include "uikernel/scrollpage.h"
#include "xbpositiondefine.h"
#include "extended/xmlbrowser/include/modxmlbrowser.h"
#include "uimanager/rectdefine.h"
//#include "baseui/fontcolor.h"
namespace
{
#define XB_TEXT_SCREENT_LEFT_SPACING 10
}

CUIRegionTextScreen::CUIRegionTextScreen(QWidget * parent/* = 0*/, Qt::WFlags flags/* = 0*/)
    : CUIRegionBase(parent, flags)
    , m_pLabTitle(NULL)
    , m_pLabText(NULL)
    , m_pScrollPage(NULL)
{
    setObjectName("UIRegionTextScreen");
    SetElementType(XMT_TEXTSCREEN);

    // 初始化TitleLab
    m_pLabTitle = g_pXMLWidgetPool->GetWidgetFromPool<AutoResizeLab>(this);
    m_pLabTitle->setMinimumHeight(XML_TITLE_HEIGHT);
    if (m_pLabTitle != NULL)
    {
        m_pLabTitle->setObjectName("LabTitle");
        m_pLabTitle->SetFixWidth(RECT_XMLBrowser.width() - XB_TEXT_SCREENT_LEFT_SPACING -
                                 XML_SCROLL_RIGHT_SPACING);

        // 设置Lab透明,并且字体颜色为白色
        QPalette pl = m_pLabTitle->palette();
        pl.setBrush(QPalette::Foreground, QBrush(QColor(255, 255, 255)));
        pl.setBrush(QPalette::Background, QBrush(QColor(255, 0, 0, 0)));
        m_pLabTitle->setPalette(pl);

        m_pLabTitle->setAlignment(Qt::AlignHCenter);// 设置Title文字居中
    }

    // 初始化滚动区域属性
    m_pScrollPage = new CScrollPage(this);
    m_pLabText = g_pXMLWidgetPool->GetWidgetFromPool<AutoResizeLab>(this);
    if (NULL != m_pScrollPage && NULL != m_pLabText)
    {
        m_pLabText->setObjectName("LabText");
        m_pLabText->SetFixWidth(RECT_XMLBrowser.width() - XML_SCROLL_WIDTH - XML_SCROLL_RIGHT_SPACING);
        m_pLabText->SetCellRowHeight(0);

        m_pScrollPage->setObjectName("ScrollPage");
        m_pScrollPage->setGeometry(0, XML_TITLE_HEIGHT, RECT_XMLBrowser.width(),
                                   RECT_XMLBrowser.height() - XML_TITLE_HEIGHT);

        // 设置ScrollPage背景透明
        QPalette pl = m_pScrollPage->palette();
        pl.setBrush(QPalette::Background, QBrush(QColor(255, 0, 0, 0)));
        m_pScrollPage->setPalette(pl);

        // 设置LabText为其子页面
        m_pScrollPage->SetChildPage(m_pLabText, RECT_XMLBrowser.height() - XML_TITLE_HEIGHT);

        m_pLabText->setWordWrap(true);
        // 设置Text区域最小高度，无意义，引出Bug：http://10.2.1.199/Bug.php?BugID=48008
        //m_pLabText->SetMinimumHeight(RECT_XMLBrowser.height() - 100/*XML_TITLE_HEIGHT*/);

        // 设置LabText字体颜色
        QPalette objQPalette;
        objQPalette.setColor(QPalette::WindowText, Qt::white);
        m_pLabText->setPalette(objQPalette);
        m_pLabText->setAlignment(Qt::AlignTop);
    }
}

CUIRegionTextScreen::~CUIRegionTextScreen()
{
    g_pXMLWidgetPool->PutWidget(m_pLabText);
    g_pXMLWidgetPool->PutWidget(m_pLabTitle);
    //g_pXMLWidgetPool->PutWidget(m_pScrollPage);
}

// 根据数据更新界面
void CUIRegionTextScreen::Update(void * pData)
{
    // 入口参数检查
    if (pData == NULL)
    {
        return;
    }

    // 强制转换指针
    XBTextScreenData * pNewData = static_cast<XBTextScreenData *>(pData);

    // 跟新锁定按键的变量
    m_bLock = pNewData->stAttr.bLockIn;

    // 更新属性
    if (m_pLabTitle != NULL && m_pScrollPage != NULL)
    {
        // 设置Title的显示模式
        m_pLabTitle->setGeometry(XB_TEXT_SCREENT_LEFT_SPACING, 0, RECT_XMLBrowser.width()
                                 - XB_TEXT_SCREENT_LEFT_SPACING - XML_SCROLL_RIGHT_SPACING, XML_TITLE_HEIGHT);
        m_pLabTitle->setWordWrap(pNewData->stAttr.bWrapTitle);
        m_pLabTitle->SetLabText(toQString(pNewData->stAttr.strTitle.c_str()));

        // 设置滚动区域的大小为正行Text的倍数
//      m_pScrollPage->resize(m_pScrollPage->width(),
//          ((this->height() - m_pLabTitle->height())
//           m_pLabText->FontlineSpacing()) * m_pLabText->FontlineSpacing() + 4);

        int iScrollHeiht = RECT_XMLBrowser.height() - m_pLabTitle->height();
        iScrollHeiht -= (iScrollHeiht % (m_pLabText->FontlineSpacing()));
        m_pScrollPage->setGeometry(0, m_pLabTitle->height(), RECT_XMLBrowser.width(), iScrollHeiht + 2);

        //m_pScrollPage->SetSingleStep(m_pLabText->FontlineSpacing());
        //m_pScrollPage->SetPageStep(m_pScrollPage->height());
    }

    if (m_pLabText != NULL)
    {
        m_pLabText->setFixedWidth(RECT_XMLBrowser.width() - XML_SCROLL_WIDTH - XML_SCROLL_LEFT_SPACING -
                                  XML_SCROLL_RIGHT_SPACING);
        m_pLabText->setContentsMargins(XB_TEXT_SCREENT_LEFT_SPACING, 0, 0, 0);
        // 设置Text
        m_pLabText->SetLabText(toQString(pNewData->strText.c_str()));

        // 计算滚动区域的大小
//      int iScrollLength   = m_pScrollPage->height() - 4;
//
//      // 修正TextView的高度使其为 滚动区域的整倍数
//      int iTextViewHeight = (m_pLabText->height() / iScrollLength + 1) * iScrollLength;
//
//      m_pLabText->resize(m_pLabText->width(), iTextViewHeight);
    }

    m_iQuitTime = pNewData->stAttr.iTimeOut;

    SetSoftkey(pNewData->vecSoftkey);
}

// 事件过滤器
bool CUIRegionTextScreen::eventFilter(QObject * pObj, QEvent * pEvent)
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

            // 转换按键
            QKeyEvent * pKeyEvent = static_cast<QKeyEvent *>(pEvent);
            if (NULL == pKeyEvent)
            {
                break;
            }
            int nKey = pKeyEvent->key();

            switch (nKey)
            {
            case PHONE_KEY_UP:
            case PHONE_KEY_LEFT:
                {
                    if (NULL != m_pScrollPage)
                    {
                        m_pScrollPage->OnPrePage();
                        return true;
                    }
                }
                break;
            case PHONE_KEY_DOWN:
            case PHONE_KEY_RIGHT:
                {
                    if (NULL != m_pScrollPage)
                    {
                        m_pScrollPage->OnNextPage();
                        return true;
                    }
                }
                break;
            case PHONE_KEY_OK:
                {
                    //发送按下OK的信号
                    emit RegionAction(REGION_ACTION_CLICK_ITEM);
                    return true;
                }
            default:
                break;
            }
        }
        break;
    default:
        break;
    }
    return false;
}
