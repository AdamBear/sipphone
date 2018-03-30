//
#ifndef __DLG_DIR_SETTING_H__
#define __DLG_DIR_SETTING_H__

#include "uimanager/basedialog.h"
class CFrameList;
class CTreeList;

class CDlgDirSetting : public CBaseDialog
{
    Q_OBJECT

public:
    static CBaseDialog * CreateInstance()
    {
        return new CDlgDirSetting();
    }

public:
    CDlgDirSetting(QWidget * parent = NULL);
    virtual ~CDlgDirSetting(void);

public:
    // 初始化
    virtual void Init();
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    virtual void UpdateWnd();

    // 焦点改变.
    virtual void OnFocusChanged(bool bFocused);

    // 获取联系人群组树形列表
    CTreeList * GetTreeList() const;

    // 获取联系人列表框
    CFrameList * GetFrameList() const;

    // 焦点变化刷新SoftKey
    void OnCtrlFocusChanged();

    void SetDirType(int nDirType);

    virtual void OnKeyboardShow();
    virtual void OnKeyboardHide();
    void AdjustFrameList(bool bReset);

    int GetFocusItemIndex();
    bool CancelInputGroupName();

public:
    //子界面可单独设置其背景图片，返回图片路径，如果图片为空，则按模板设置
    virtual QPixmap GetCustomBackGround();

public:
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

protected:
    virtual bool OnAction(const QString & strAction);

    // 点击IME
    void OnBtnIMEClick();
    // 点击删除
    void OnBtnDeleteClick();

private:
    // 群组树形控件
    CTreeList * m_pTreeList;
    // 联系人列表框控件
    CFrameList * m_pFrameList;
};

#endif
