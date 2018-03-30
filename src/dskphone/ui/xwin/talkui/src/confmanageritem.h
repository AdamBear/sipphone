#ifndef _CONF_MANAGER_ITEM_H
#define _CONF_MANAGER_ITEM_H

#include "talkuilogic/talkuilogiccommon.h"
#include "ylstring.h"
#include "xwindow/xlistviewitem.h"

namespace talkui
{

////////////tagConfManagerData///////////
struct tagConfManagerData
{
    int m_iCallID;      //对应的CallID
    yl::string m_strText;   //显示的文字
    AUDIO_CODEC_TYPE m_eAudioCodec;//高清类型
    bool m_bHeld;           //被Hold 状态
    bool m_bFarMute;        //远程静音状态
    bool m_bFarHold;        //单路hold状态
    bool m_bReceived;       //是否处于已经接起的状态

    tagConfManagerData();

    //重置数据
    void Reset();

    tagConfManagerData & operator=(const tagConfManagerData & data)
    {
        //判断是否为当前实例
        if (this == &data)
        {
            return *this;
        }

        this->m_iCallID = data.m_iCallID;       //对应的CallID
        this->m_strText = data.m_strText;   //显示的文字
        this->m_eAudioCodec = data.m_eAudioCodec;           //高清类型
        this->m_bHeld = data.m_bHeld;           //被Hold 状态
        this->m_bFarMute = data.m_bFarMute;     //远程静音状态
        this->m_bFarHold = data.m_bFarHold;     //单路hold状态
        this->m_bReceived = data.m_bReceived;   //已经接起的状态

        return *this;
    }
};

typedef struct tagConfManagerData CConfManangerData;

////////////tagConfManagerData///////////


///////////////////////// CConfManagerItem /////////////////////////////
class CConfManagerItem : public xListViewItem
{
public:
    explicit CConfManagerItem(IListViewItemAction * pAction = NULL);
    virtual ~CConfManagerItem();

    static int GetConfManagerItemType();
    static CConfManagerItem * GetConfManagerItem(xListViewItem * pItem);

    static xListViewItem * CreateConfManagerItem();

    virtual void loadChildrenElements(xml_node & node);

public:
    //设置联系人信息
    void SetConfManangerContactInfo(CConfManangerData & objConfManagerData);

    //获取联系人callID
    int GetContactCallID();

protected:
    //重置所有信息
    void ResetAllInfo();

    void SetName(const yl::string & strName = "", bool bShow = false, bool bGone = true);
    void SetIconHide();

protected:
    xTextView * m_pName;    //名称

    xLinearLayout * m_pIconWidget;      //图标的整体

    talkui_vector<xImageView *>
    m_vectorStatusImageView;    //显示状态图标  有 held 、 HD、 Farmute

protected:
    int m_iCallID;      //保存的callID



};
///////////////////////// CConfManagerItem /////////////////////////////
}



#endif //_CONF_MANAGER_ITEM_H
