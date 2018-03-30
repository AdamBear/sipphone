// Edit dialog of directory module to edit a contact.
// Created by yyj.

#ifndef __DLG_DIR_EDIT_H__
#define __DLG_DIR_EDIT_H__

#include "dlgdirdetail.h"
#include "ringplayer.h"
#include "directorycommon.h"
#include "baseui/comboboxlistitem.h"

class CFrameList;
class CEditListItem;
class CDetailListItemLayout;

class CDlgDirEdit : public CDlgDirDetail
{
    Q_OBJECT

public:
    static CBaseDialog * CreateInstance()
    {
        return new CDlgDirEdit();
    }

public:
    CDlgDirEdit(QWidget * parent = NULL);
    virtual ~CDlgDirEdit(void);

public:
    // 初始化
    virtual void Init();
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    virtual void OnHomeButtonClick();

    virtual void OnKeyboardShow();

    virtual void OnKeyboardHide();

    // 调整framelist位置，确保输入框显示
    void AdjustFrameList(bool bReset);

    virtual void SetContact(int nContactType);

    void SetPhotoPath(yl::string & strFilePath);

    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

    virtual bool eventFilter(QObject * pObj, QEvent * pEvent);

    virtual bool IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
                                 QRect & rtVolume, yl::string & strBGPath) const;

protected:
    virtual bool OnAction(const QString & strAction);

    bool GetRingPath(yl::string & strRingPath, int nIndex);

    // 释放列表项
    void ReleaseLayout();

    // 重置代理布局
    void ResetUIHelperLayout();

    void InitLocalLayout();
    void InitBroadSoftLayout();
#if IF_FEATURE_GBBOOK
    void InitGenbandLayout();
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    void InitMetaSwitchLayout(bool isCopy);
#endif

    CEditListItem * GetNameEditItem();

    // 焦点变化刷新SoftKey
    void OnCtrlFocusChanged();

    void NotifyCtrlFocus(bool bFocus);

    // 焦点进入两秒后播放铃声
    void OnPlayRingInFocus();
    // 焦点进入头像
    void OnComboBoxPhotoInFocus();

    // 点击保存
    bool OnBtnSaveClick();
    // 点击IME
    void OnBtnIMEClick();
    // 点击删除
    void OnBtnDeleteClick();
    // 点击返回
    void OnBtnBackClick();

    bool ProcessInputs(int nUIAction);

    virtual void OnBtnPreDetail();
    virtual void OnBtnNextDetail();
    virtual void OnBtnImage();

    void OnChooseImage();

    bool OnFocusPressInEventFilter(QObject * pObj, QEvent * pEvent);
    bool OnEnterPressEventFilter(QObject * pObj, QEvent * pEvent);
    bool OnMousePressEventFilter(QObject * pObj, QEvent * pEvent);
    virtual bool OnLeftPressEventFilter(QObject * pObj, QEvent * pEvent);
    virtual bool OnRightPressEventFilter(QObject * pObj, QEvent * pEvent);
    virtual bool OnDownPressEventFilter(QObject * pObj, QEvent * pEvent, bool & bFilter);
    virtual bool OnUpPressEventFilter(QObject * pObj, QEvent * pEvent, bool & bFilter);

    void SetKeyboardEnterType();

public Q_SLOTS:
    // 铃声被选中
    void OnComboBoxRingSelect(int nIndex);
    // 头像被选中
    void OnComboBoxPhotoSelect(int nIndex);

protected:
    enum
    {
        DIR_UI_ACTION_PRE_DETAIL    = DIR_UI_ACTION_COMMON_END + 1,
        DIR_UI_ACTION_NEXT_DETAIL   = DIR_UI_ACTION_COMMON_END + 2,
    };

    // 界面布局
    CDetailListItemLayout * m_pListItemLayout;

    // 铃声组合框
    qComboBoxEx * m_pComboBoxRing;

    // 图片组合框
    qComboBoxEx * m_pComboBoxPhoto;

    qRingPlayer m_ring;

    int m_nFocusIndex;
};

#endif
