// Detail dialog of directory module to show a contact.
// Created by yyj.

#ifndef __DLG_DIR_NEW_H__
#define __DLG_DIR_NEW_H__

#include <ylstring.h>
#include "uimanager/basedialog.h"
#include "uikernel/buttonwrapper.h"
#include "baseui/comboboxlistitem.h"
#include "baseui/framelistdelegate.h"
#include "ringplayer.h"

class CEditListItem;
class CLocalDetailLayout;
class CDirectoryListDelegate;
class CFrameList;

class CMTSWDetailLayout;

class CDlgDirNew : public CBaseDialog, public IFrameListDelegate
{
    Q_OBJECT

public:
    static CBaseDialog * CreateInstance()
    {
        return new CDlgDirNew();
    }

public:
    CDlgDirNew(QWidget * parent = NULL);
    virtual ~CDlgDirNew(void);

public:
    // 初始化
    virtual void Init();
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    virtual void UpdateWnd();

    // 子界面响应Home按钮的操作
    virtual void OnHomeButtonClick();

    void Init(bool bCopyContact, int nContactType, int nDefaultGroupId, bool bIsDsskeyAdd = false);
    int GetDirType();

    void SetPhotoPath(yl::string & strFilePath);

    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

    virtual bool eventFilter(QObject * pObj, QEvent * pEvent);

    virtual void OnKeyboardShow();

    virtual void OnKeyboardHide();

    // 调整framelist位置，确保输入框显示
    void AdjustFrameList(bool bReset);

public:
    //子界面可单独设置其背景图片，返回图片路径，如果图片为空，则按模板设置
    virtual QPixmap GetCustomBackGround();

    // IFrameListDelegate
public:
    virtual bool OnLastItemNext();
    virtual bool OnFirstItemPrevious();
    virtual void OnCleanContent(CFrameListBase * pFrameList);
    virtual void OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount);
    virtual bool IsFrameListAttach(CFrameListBase * pFrameList);
    virtual void OnFrameListDetach(CFrameListBase * pFrameList);
    virtual void OnFrameListAttach(CFrameListBase * pFrameList);

    virtual bool IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
                                 QRect & rtVolume, yl::string & strBGPath) const;

protected:
    virtual bool OnAction(const QString & strAction);

    virtual void mousePressEvent(QMouseEvent * pEvent);
    virtual void mouseReleaseEvent(QMouseEvent * pEvent);

    virtual void paintEvent(QPaintEvent * pEvent);

    bool GetRingPath(yl::string & strRingPath, int nIndex);

public:
    // 释放列表项
    void ReleaseLayout();

    void InitLocalLayout(bool bIsDsskeyAdd = false);

    void ReLayoutListItem();

    void InitGroup();

    CEditListItem * GetNameEditItem();

    // 弹窗选择头像
    void OnChooseImage();

    // 焦点变化刷新SoftKey
    void OnCtrlFocusChanged();

    // 点击保存
    bool OnBtnSaveClick();
    // 点击IME
    void OnBtnIMEClick();
    // 点击删除
    void OnBtnDeleteClick();
    // 点击返回
    void OnBtnBackClick();

    bool ProcessInputs(int nUIAction);

    int GetGroupId();

    // 设置进入之前的GroupID
    void SetBeforeGroupId(int nBeforeGroupId);

public Q_SLOTS:
    // 群组被选中
    void OnComboBoxGroupSelect(int nIndex);
    // 铃声被选中
    void OnComboBoxRingSelect(int nIndex);
    // 头像被选中
    void OnComboBoxPhotoSelect(int nIndex);

protected:
    bool OnShowEventFilter(QObject * pObj, QEvent * pEvent);
    bool OnFocusInEventFilter(QObject * pObj, QFocusEvent * pEvent);
    bool OnEnterPressEventFilter(QObject * pObj, QEvent * pEvent, bool & bFilter);
    bool OnLeftPressEventFilter(QObject * pObj, QEvent * pEvent);
    bool OnRightPressEventFilter(QObject * pObj, QEvent * pEvent);
    bool OnUpPressEventFilter(QObject * pObj, QEvent * pEvent);
    bool OnDownPressEventFilter(QObject * pObj, QEvent * pEvent);
    bool OnMousePressEventFilter(QObject * pObj, QEvent * pEvent);

    // 焦点进入两秒后播放铃声
    void OnPlayRingInFocus();
    // 焦点进入头像
    void OnComboBoxPhotoInFocus();

    void SetLocalItemStyle();
    void SetBSFTItemStyle();
    void RelayoutLocalListItem();
    void RelayoutBSFTListItem();
    void SetContactType(int nContactType);

    void SetKeyboardEnterType();

protected:
    // 图像按钮
    CButtonWrapper m_btnImg;

    // 界面布局(以本地联系人布局来实现)
    CDetailListItemLayout * m_pListItemLayout;

    CFrameList * m_pFrameList;

    // 群组组合框
    qComboBoxEx * m_pComboBoxGroup;

    // 铃声组合框
    qComboBoxEx * m_pComboBoxRing;

    // 图片组合框
    qComboBoxEx * m_pComboBoxPhoto;

    // 图片路径
    yl::string m_strImage;

    int m_nAddGroupId;
    int m_nListGroupId;
    int m_nContactType;
    int m_nFocusIndex;
    bool m_bCopyContact;

    qRingPlayer m_ring;

    bool m_bMutiPage;
};

#endif
