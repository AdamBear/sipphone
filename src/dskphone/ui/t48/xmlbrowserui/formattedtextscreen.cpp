#include "formattedtextscreen.h"
#include "textstylemanager.h"
#include "xbpositiondefine.h"
#include "cxmllabel.h"
#include "extended/xmlbrowser/include/modxmlbrowser.h"
#include "uimanager/rectdefine.h"
#include "keymap.h"
#include "uikernel/scrollpage.h"

///////////////////////////////////////////////////////////////////////
CDlgFormattedTextScreen::CDlgFormattedTextScreen(QWidget * parent /* = 0 */,
        Qt::WFlags flags /* = 0 */)
    : CUIRegionBase(parent, flags)
    , m_HeaderView(NULL)
    , m_FooterView(NULL)
    , m_TextView(NULL)
    , m_pScrollPage(NULL)
{
    setObjectName("FormattedTextScreen");
    // 初始化HeaderView
    m_HeaderView = new AutoResizeListView(this->width(), this);
    if (NULL != m_HeaderView)
    {
        m_HeaderView->setObjectName("Header");
        m_HeaderView->SetMaxRow(2);
    }

    // 初始化FooterView
    m_FooterView = new AutoResizeListView(this->width(), this);
    if (NULL != m_FooterView)
    {
        m_FooterView->setObjectName("Footer");
        m_FooterView->SetMaxRow(1);
    }

    // 初始化滚动文字区域
    m_TextView = new AutoResizeListView(this->width() - XML_SCROLL_LEFT_SPACING - XML_SCROLL_WIDTH -
                                        XML_SCROLL_LEFT_SPACING, m_pScrollPage);
    m_pScrollPage = new CScrollPage(this);
    if (m_TextView != NULL && m_pScrollPage != NULL)
    {
        m_TextView->setObjectName("TextView");
        m_pScrollPage->setObjectName("ScrollPage");
        m_TextView->resize(this->width() - XML_SCROLL_WIDTH, this->height());
        //m_pScrollPage->setGeometry(0, XML_TITLE_HEIGHT, this->width(), this->height() - XML_TITLE_HEIGHT + 4);
        m_pScrollPage->SetChildPage(m_TextView, this->height() - XML_TITLE_HEIGHT * 2);

        // 设置ScrollPage背景透明
        QPalette pl = m_pScrollPage->palette();
        pl.setBrush(QPalette::Background, QBrush(QColor(255, 0, 0, 0)));
        m_pScrollPage->setPalette(pl);
    }
}

CDlgFormattedTextScreen::~CDlgFormattedTextScreen()
{

}

void CDlgFormattedTextScreen::Update(void * pData)
{
    if (pData == NULL)
    {
        return;
    }

    XBFormattedTextData * pNewData = static_cast<XBFormattedTextData *>(pData);

    // 设置锁定按键的变量
    m_bLock = pNewData->stAttr.bLockIn;

    // 设置Header的数据
    if (m_HeaderView != NULL)
    {
        // 截取小于最大显示数量的data
        list<LineData *> listHeaderData;
        List_Header_Zone::iterator iter = pNewData->listHeader.begin();
        int iCount = 0;
        while (iCount < MAX_HEADER && iter != pNewData->listHeader.end())
        {
            listHeaderData.push_back(*iter);
            ++iCount;
            iter++;
        }

        m_HeaderView->SetViewData(listHeaderData, true);

        // 进行位置的修正
        m_HeaderView->setGeometry(0, 0, this->width(), m_HeaderView->height());
        m_HeaderView->SetItemFixWidth(this->width());
    }

    // 设置Footer的数据
    if (m_FooterView != NULL)
    {
        // 截取小于最大显示数量的data
        list<LineData *> listFooterData;
        List_Footer_Zone::iterator iter = pNewData->listFooter.begin();
        int iCount = 0;
        while (iCount < MAX_FOOTER && iter != pNewData->listFooter.end())
        {
            listFooterData.push_back(*iter);
            ++iCount;
            iter++;
        }

        m_FooterView->SetViewData(listFooterData, true);
        m_FooterView->setGeometry(0, this->height() - XML_TITLE_HEIGHT,
                                  this->width(), XML_TITLE_HEIGHT);
        m_FooterView->SetItemFixWidth(this->width());
    }

    if (m_pScrollPage != NULL && NULL != m_FooterView
            && NULL != m_HeaderView && NULL != m_TextView)
    {
        m_pScrollPage->setGeometry(0, m_HeaderView->height(),
                                   this->width(),
                                   this->height() - m_FooterView->height() - m_HeaderView->height());

        // 转化数据
        list<LineData *> listData;
        List_Scroll_Zone::iterator iter = pNewData->listScroll.begin();
        for (; iter != pNewData->listScroll.end(); iter++)
        {
            listData.push_back(*iter);
        }

        // 设置数据
        m_TextView->SetViewData(listData, true);
        m_TextView->SetItemFixWidth(this->width() - XML_SCROLL_LEFT_SPACING - XML_SCROLL_WIDTH -
                                    XML_SCROLL_RIGHT_SPACING);
        //m_pScrollPage->SetSingleStep(30);
//      m_pScrollPage->SetPageStep(m_pScrollPage->height());

        if (m_TextView->height() > m_pScrollPage->height())
        {
            // 计算滚动区域的大小
            int iScrollLength   = m_pScrollPage->height();////+4
            m_TextView->ResetLabPosByHeight(iScrollLength);

            // 修正TextView的高度使其为 滚动区域的整倍数
//          int iTextViewHeight = ((m_TextView->height() / iScrollLength) + 1) * iScrollLength;
//          m_TextView->resize(m_TextView->width(), iTextViewHeight);

            m_pScrollPage->SetChildPage(m_TextView, iScrollLength);
        }
    }

    m_iQuitTime = pNewData->stAttr.iTimeOut;

    SetSoftkey(pNewData->vecSoftkey);
}

bool CDlgFormattedTextScreen::eventFilter(QObject * pObj, QEvent * pEvent)
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
                break;
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
