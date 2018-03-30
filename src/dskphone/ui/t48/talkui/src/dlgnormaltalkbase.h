#ifndef _DLG_NORMAL_TALK_BASE_H
#define _DLG_NORMAL_TALK_BASE_H

//mod-UIManager
#include "uimanager/uimanager_inc.h"
#include "contactinfoframe.h"

//mod-this
#include "dlgbasetalkui.h"

//mod-Talk
#include "talk/talklogic/include/uiandlogic_common.h"

#include "customtime.h"


class CDlgNormalTalkBase: public CDlgBaseTalkUI
{
    Q_OBJECT
public:
    CDlgNormalTalkBase(const yl::string & strTalkUIName, TALK_UI_TYPE eTalkUIType);
    virtual ~CDlgNormalTalkBase(void);

public:
    // 初始化
    virtual void Init();

    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();


#ifdef _T48

public:
    //添加或删除图标
    virtual void AddOrDeleteIcon(LPCSTR strIconPath, bool bAdd);

    //删除所有图标
    virtual void DeleteAllIcon();

    void SetCustomImageMark(std::vector<bool>&  vecContactCustomImage);

    void SetCustomImageMark(yl::string strImagePath);

    //返回值返回当前加载的图片是否用户自定义头像
    bool IsCustomImage(const yl::string & strImagePath);

signals:
    //是否获取到头像的标志变化
    void LoadPhotoMarkChange();

protected:
    // 显示用户信息
    CContactInfoFrame   m_cConInfoFrame;
#endif


};



#endif
