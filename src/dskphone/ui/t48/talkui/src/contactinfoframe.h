#ifndef _CONTACT_INFO_FRAME_H_
#define _CONTACT_INFO_FRAME_H_

//mod-Qt
#include <QFrame>
#include <QLabel>
#include "uikernel/qscrolltextlabel.h"
#include <ylstring.h>

class qWidgetList;
class qUIPushButton;
class IconLabel;

class qLabContactImage : public QLabel
{
    Q_OBJECT

public:
    qLabContactImage(QWidget * parent = 0);
    virtual ~qLabContactImage();

    //设置联系人头像
    void SetImage(const yl::string & strImagePath);

    // 添加或者删除上的Icon
    // strIconPath | Icon的路径
    // bAdd | true 添加  false删除
    void AddOrDeleteIcon(LPCSTR strIconPath, bool bAdd);

    //清除所有 ICON
    void DeleteAllIcon();

    // 清空控件数据
    void Clear();

    // 设置Tip信息
    void SetTipInfo(const QString & strTip, const QString & strPicPath = "");

    // 清空
    void ClearTip();

    QSize GetIconSize(LPCSTR strIconPath);

private:
    qWidgetList     *    m_pWidgetList;
    yl::string           m_strContactImagePath;
    QLabel               m_labCover;    // 使会议图标变成变成灰色的控件
    IconLabel      *     m_pIconLabel;  // 显示提示信息的Icon
};

//////////////////////////////////////////////////////////////////////////
//显示联系人信息UI类，主要用于作为需要显示各种联系人信息界面
class CContactInfoFrame : public QFrame
{
    Q_OBJECT

public:
    //构造函数
    CContactInfoFrame(QWidget * parent = 0);

    //析构函数
    virtual ~CContactInfoFrame();

    //重设数据
    void Clear();

public:
    //设置联系人名称
    void SetContactName(const yl::string & strName);

    //设置联系人名称
    void SetContactName(const QString & strName);

    //设置联系人部门等信息
    void SetContactInfo(const yl::string & strInfo);

    //设置联系人号码
    void SetContactNumber(const yl::string & strNumber);

    // 设置Merger的开关
    void SetMergerSwitch(bool bMerger);

    // 获取Merger的开关
    bool GetMergerSwitch();

    //设置联系人头像
    void SetContactImage(const yl::string & strImagePath);

    // 添加或者删除上的Icon
    // strIconPath | Icon的路径
    // bAdd | true 添加  false删除
    void AddOrDeleteIcon(LPCSTR strIconPath, bool bAdd);

    //清除所有 ICON
    void DeleteAllIcon();

    // 调整控件布局
    // | [in] bDisplayContactPhoto | 是否显示头像
    void AdjustLayout(bool bDisplayContactPhoto);

#if IF_FEATURE_NFON_RTP
    void StopAllScrollTimer();
#endif

    void AdjustScrollTimer();

private:
    // 初始化界面数据
    void InitData();

    // 名字和号码一样的时候合并为一个
    void MergerNameAndNumber();



private:
    qScrollTextLabel     m_labContactName;
    qScrollTextLabel     m_labContactInfo;
    qScrollTextLabel     m_labContactNumber;
    bool                 m_bMergerNameAndNumber;
    qLabContactImage     m_labImage;
};

//////////////////////////////////////////////////////////////////////////
class CConferenceInfoFrame : public QFrame
{
    Q_OBJECT

public:
    //构造函数
    CConferenceInfoFrame(QWidget * parent = 0);

    //析构函数
    virtual ~CConferenceInfoFrame();

    // 设置CallID
    void SetCallID(int iCallID);

    // 获取CallID
    int GetCallID()
    {
        return m_iCallID;
    }

    //重设数据
    void Clear();

    //设置联系人名称
    void SetContactName(const yl::string & strName);

    // 获取联系人名字
    QString GetContactName()
    {
        return m_labContactName.text();
    }

    //设置联系人头像
    void SetContactImage(const yl::string & strImagePath);

    // 添加或者删除上的Icon
    // strIconPath | Icon的路径
    // bAdd | true 添加  false删除
    void AddOrDeleteIcon(LPCSTR strIconPath, bool bAdd);

    //清除所有 ICON
    void DeleteAllIcon();

    // 设置Tip信息
    void SetTipInfo(const QString & strTip, const QString & strPicPath = "");

    // 清空
    void ClearTip();

    // 设置Mute的状态
    void SetFarMuteStatus(bool bEnable);

    // 设置FarHold的状态
    void SetFarHoldStatus(bool bEnable);

    // 调整控件布局
    // | [in] bDisplayContactPhoto | 是否显示头像
    void AdjustLayout(bool bDisplayContactPhoto);
    // 设置是否显示FarMute、FarHold
    void SetFarMuteHoldShow(bool bShow);

protected slots:
    void OnBtnClicked(QString strBtnAction);

signals:
    void BtnClicked(CConferenceInfoFrame * pInfoFrame, const QString & strBtnAction);

private:
    int                  m_iCallID;
    qScrollTextLabel     m_labContactName;
    qLabContactImage     m_labImage;
    qUIPushButton    *   m_pBtnMute;
    qUIPushButton    *   m_pBtnRemove;
    qUIPushButton    *   m_pBtnFarHold;

};

#endif // _CONTACT_INFO_FRAME_H_
