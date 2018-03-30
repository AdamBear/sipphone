#include "confmanageritem.h"
#include "talkuicommonfunction.h"

#include "talkuixmlpath.h"
#include "talkuixmlitemname.h"
#include "talkuipath.h"

namespace talkui
{
////////////tagConfManagerData///////////
tagConfManagerData::tagConfManagerData()
{
    //重置数据
    Reset();
}

//重置数据
void tagConfManagerData::Reset()
{
    m_iCallID = -1;     //对应的CallID
    m_strText = ""; //显示的文字
    m_eAudioCodec = ACT_NONE;//高清类型
    m_bHeld = false;            //被Hold 状态
    m_bFarMute = false;     //远程静音状态
    m_bFarHold = false;     //单路hold状态
    m_bReceived = true;     //已经接起的状态
}
////////////tagConfManagerData///////////

///////////////////////// CConfManagerItem /////////////////////////////

CConfManagerItem::CConfManagerItem(IListViewItemAction * pAction /*= NULL*/)
    : xListViewItem(LIST_ITEM_TYPE_CONF_MANAGER, pAction)
    , m_pName(NULL)
    , m_pIconWidget(NULL)
{
    m_iCallID = -1;
}

CConfManagerItem::~CConfManagerItem()
{

}

int CConfManagerItem::GetConfManagerItemType()
{
    return LIST_ITEM_TYPE_CONF_MANAGER;
}

CConfManagerItem * CConfManagerItem::GetConfManagerItem(xListViewItem * pItem)
{
    if (NULL == pItem || LIST_ITEM_TYPE_CONF_MANAGER != pItem->GetType())
    {
        return NULL;
    }

    return (CConfManagerItem *)pItem;
}

xListViewItem * CConfManagerItem::CreateConfManagerItem()
{
    CConfManagerItem * pListItem = new CConfManagerItem;

    if (NULL != pListItem)
    {
        pListItem->loadContent(TALKUI_CONF_MANAGER_ITEM_XML_PATH);
    }

    return pListItem;
}

void CConfManagerItem::loadChildrenElements(xml_node & node)
{
    xListViewItem::loadChildrenElements(node);

    m_pName = static_cast<xTextView *>(getViewById(TALKUI_CONFMANAGER_ITEM_CONTACTNAME));

    m_pIconWidget = static_cast<xLinearLayout *>(getViewById(TALKUI_CONFMANAGER_ITEM_STATUSICON));

    char szXMLNode[256] = { 0 };

    //图标数组
    for (int i = 0;; i++)
    {
        sprintf(szXMLNode, "%s%d", TALKUI_CONFMANAGER_ITEM_STATUSICON, i);

        xImageView * pIcon = static_cast<xImageView *>(getViewById(szXMLNode));
        if (NULL != pIcon)
        {
            m_vectorStatusImageView.push_back(pIcon);
        }
        else        //没有获取到控件 退出循环
        {
            break;
        }
    }
}

void CConfManagerItem::ResetAllInfo()
{
    //隐藏控件
    SetName();
    SetIconHide();

    //callID置 -1
    m_iCallID = -1;
}

void CConfManagerItem::SetName(const yl::string & strName/* = ""*/, bool bShow /*= false*/,
                               bool bGone /*= true*/)
{
    if (NULL != m_pName)
    {
        m_pName->SetText(strName);

        if (bShow)
        {
            m_pName->show(xView::SHOW_CMD_BRING_TO_TOP);
        }
        else
        {
            m_pName->hide(bGone);
        }
    }
}

void CConfManagerItem::SetIconHide()
{
    int iImageViewCount = m_vectorStatusImageView.size();

    for (int i = 0; i < iImageViewCount; i++)
    {
        xImageView * pImageViewHide = m_vectorStatusImageView[i];
        if (NULL != pImageViewHide)
        {
            pImageViewHide->hide();
        }
    }

    if (NULL != m_pIconWidget)
    {
        m_pIconWidget->hide(true);
    }//这边要把位置留下来 所以需要把区域释放
}

void CConfManagerItem::SetConfManangerContactInfo(CConfManangerData & objConfManagerData)
{
    //先清除信息
    ResetAllInfo();

    //赋值联系人 callID
    m_iCallID = objConfManagerData.m_iCallID;

    //显示联系人名称的控件
    yl::string strName = objConfManagerData.m_strText;
    if (!objConfManagerData.m_bReceived)
    {
        strName += " (";
        strName += _UILOGIC_LANG_TRAN(TRID_CONNECTING);
        strName += ")";
    }
    SetName(strName, true);

    //如果是未接起的状态，则不能显示图标
    if (!objConfManagerData.m_bReceived)
    {
        if (NULL != m_pIconWidget)
        {
            m_pIconWidget->hide(true);
        }
    }
    //接起的状态下才设置图标
    else
    {
        if (NULL != m_pIconWidget)
        {
            m_pIconWidget->show(xView::SHOW_CMD_BRING_TO_TOP);
        }

        //设置图标
        //先把图片数组清空
        //控件序号
        int j = 0;
        xImageView * pImageViewShow = NULL;

        int nImageCount = m_vectorStatusImageView.size();

        //HD
        if (j < nImageCount)
        {
            yl::string strHDIcon = "";
            if (objConfManagerData.m_eAudioCodec & ACT_HD)
            {
#if IF_CONF_MANAGER_ITEM_HD_SPECIAL
                strHDIcon = PIC_TALK_ICON_HD_ITEM;
#else
                strHDIcon = PIC_TALK_ICON_HD;
#endif
            }
            else if (objConfManagerData.m_eAudioCodec & ACT_AMR)
            {
#if IF_CONF_MANAGER_ITEM_HD_SPECIAL
                strHDIcon = PIC_TALK_ICON_AMR_ITEM;
#else
                strHDIcon = PIC_TALK_ICON_AMR;
#endif
            }
            else if (objConfManagerData.m_eAudioCodec & ACT_OPUS)
            {
#if IF_CONF_MANAGER_ITEM_HD_SPECIAL
                strHDIcon = PIC_TALK_ICON_OPUS_ITEM;
#else
                strHDIcon = PIC_TALK_ICON_OPUS;
#endif
            }

            if (!strHDIcon.empty())
            {
                pImageViewShow = m_vectorStatusImageView[j];

                if (NULL != pImageViewShow)
                {
                    pImageViewShow->setPixmap(GetPixmapByString(strHDIcon));
                    pImageViewShow->show();
                }
                j++;    //控件序号要加1
            }
        }

        //held
        if ((objConfManagerData.m_bHeld || objConfManagerData.m_bFarHold)
                && j < nImageCount)
        {
            pImageViewShow = m_vectorStatusImageView[j];

            if (NULL != pImageViewShow)
            {
                pImageViewShow->setPixmap(GetPixmapByString(PIC_TALK_HELD_SMALL));
                pImageViewShow->show();
            }
            j++;    //控件序号要加1
        }

        //Farmute
        if (objConfManagerData.m_bFarMute
                && j < nImageCount)
        {
            pImageViewShow = m_vectorStatusImageView[j];

            if (NULL != pImageViewShow)
            {
                pImageViewShow->setPixmap(GetPixmapByString(PIC_TALK_MUTE_SMALL));
                pImageViewShow->show();
            }
            j++;    //控件序号要加1
        }
    }
}

//获取联系人callID
int CConfManagerItem::GetContactCallID()
{
    return m_iCallID;
}
}

