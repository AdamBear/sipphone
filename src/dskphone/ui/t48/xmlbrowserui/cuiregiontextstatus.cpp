#include "cuiregiontextstatus.h"
#include "cxmllabel.h"
#include "xbpositiondefine.h"
#include "idlescreen/include/idlescreen_def.h"


const QSize LAB_ICON_SIZE(28, 28);  // label Icon的大小

CUIRegionTextStatus::CUIRegionTextStatus(QWidget * parent/* = 0*/, Qt::WFlags flags/* = 0*/)
    : CUIRegionBase(parent, flags)
    , m_pAutoResizeView(NULL)
{
    setObjectName("UIRegionTextStatus");
    this->setGeometry(0, 0, 360, 200);
    SetElementType(XMT_STATUS);

    m_pAutoResizeView = new AutoResizeListView(this);
    if (NULL != m_pAutoResizeView)
    {
        m_pAutoResizeView->setObjectName("AutoResize");
        m_pAutoResizeView->SetWrapWord(true);// 默认不换行
        m_pAutoResizeView->SetRowHeight(0);// 默认自适应行高
        m_pAutoResizeView->SetLabMinimumHeight(
            LAB_ICON_SIZE.height());// 默认最小行高不得小于Icon的高度
    }
}

CUIRegionTextStatus::~CUIRegionTextStatus()
{

}

// 根据数据更新界面
void CUIRegionTextStatus::Update(void * pData)
{
    typeCusNotifyList * pListData = static_cast<typeCusNotifyList *>(pData);
    if (pListData == NULL)
    {
        return;
    }

    //默认不换行
    SetWrapText(false);

    //设置大小位置
    SetStatusFrameGeometry(this->geometry());

    if (NULL != m_pAutoResizeView)
    {
        // 进行数据转换
        list<StatusItem> listStatusItem;
        typeCusNotifyList::iterator iter = pListData->begin();
        for (; iter != pListData->end(); iter++)
        {
            CustomNotify & refNotifyData = *iter;
            StatusItem objItem;
            objItem.iAlign = refNotifyData.iAlign;
            objItem.strIconPath = refNotifyData.strIcon;
            objItem.eColor = (TEXT_FONT_COLOR)refNotifyData.iColor;
            objItem.eSize = (X_FONT_TYPE)refNotifyData.iSize;
            objItem.strMsg = refNotifyData.strText;
            objItem.strAccount = refNotifyData.strAccount;
            listStatusItem.push_back(objItem);
        }

        m_pAutoResizeView->SetViewData(listStatusItem);
        m_pAutoResizeView->ShowThePartLabByHeight(this->height());
    }

    //m_iQuitTime = pStatusData->stAttr.iTimeOut;
    m_iQuitTime = 0;

    this->raise();
    this->show();

    if (NULL != m_pAutoResizeView)
    {
        m_pAutoResizeView->raise();
        m_pAutoResizeView->show();
    }
}

bool CUIRegionTextStatus::eventFilter(QObject * pObj, QEvent * pEvent)
{
    if (pObj == NULL || pEvent == NULL)
    {
        return false;
    }
    switch (pEvent->type())
    {
    case QEvent::Show:
        {
            if (NULL != m_pAutoResizeView)
            {
                m_pAutoResizeView->ShowThePartLabByHeight(this->height());
            }
        }
        break;
    default:
        break;
    }
    return false;
}


void CUIRegionTextStatus::SetStatusFrameGeometry(QRect objRect)
{
    if (objRect.isValid())
    {
        // 设置控件的位置
        this->setGeometry(objRect);
        if (NULL != m_pAutoResizeView)
        {
            // 里面的View和Frame一样大
            m_pAutoResizeView->setGeometry(0, 0, objRect.width(), objRect.height());
            m_pAutoResizeView->SetItemFixWidth(objRect.width());
        }
    }
}

void CUIRegionTextStatus::SetWrapText(bool bWrap)
{
    if (NULL != m_pAutoResizeView)
    {
        m_pAutoResizeView->SetWrapWord(bWrap);
    }
}
