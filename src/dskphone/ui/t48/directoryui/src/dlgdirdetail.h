// Detail dialog of directory module to show a contact.
// Created by yyj.

#ifndef __DLG_DIR_DETAIL_H__
#define __DLG_DIR_DETAIL_H__

#include "uimanager/basedialog.h"
#include "uikernel/btnbarwrapper.h"

class CFrameList;

class CDlgDirDetail : public CBaseDialog
{
    Q_OBJECT
#ifdef IF_ENABLE_TESTABILITY
    Q_PROPERTY(QString testinfo READ GetTestInfo)
public:
    virtual QString GetTestInfo();

    QRect m_rcPage;
    QString m_strPage;
#endif

public:
    static CBaseDialog * CreateInstance()
    {
        return new CDlgDirDetail();
    }

public:
    CDlgDirDetail(QWidget * parent = NULL);
    virtual ~CDlgDirDetail(void);

public:
    // 初始化
    virtual void Init();
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    virtual void UpdateWnd();

    //virtual bool eventFilter(QObject *pObj, QEvent *pEvent);

    virtual void SetContact(int nContactType);
    int GetDirType() const;

    // 获取列表
    CFrameList * GetFrameList() const;

    virtual bool eventFilter(QObject * pObj, QEvent * pEvent);

    // 焦点改变.
    virtual void OnFocusChanged(bool bFocused);

    bool KeypadHook(int nKeyCode, bool bPress);

public:
    //子界面可单独设置其背景图片，返回图片路径，如果图片为空，则按模板设置
    virtual QPixmap GetCustomBackGround();

protected:
    virtual void mousePressEvent(QMouseEvent * pEvent);
    virtual void mouseReleaseEvent(QMouseEvent * pEvent);

    virtual void paintEvent(QPaintEvent * pEvent);

protected:
    void UpdateDetail();
    void LoadContactImage();
    virtual void OnBtnPreDetail();
    virtual void OnBtnNextDetail();
    virtual void OnBtnImage();

protected:
    virtual bool OnLeftPressEventFilter(QObject * pObj, QEvent * pEvent);
    virtual bool OnRightPressEventFilter(QObject * pObj, QEvent * pEvent);

protected:
    // 左边箭头按钮
    CButtonWrapper m_btnPreDetail;
    // 右边箭头按钮
    CButtonWrapper m_btnNextDetail;
    // 头像按钮
    CButtonWrapper m_btnImage;
    // 按钮工具栏
    CBtnBarWrapper m_btnBar;
    // 详细信息
    CFrameList * m_pFrameList;

    // 当前联系人类型
    int m_nContactType;
    // 当前联系人Id;
    int m_nContactId;
    // 当前联系人序号
    int m_nDataIndex;
    // 当前所有联系人总数
    int m_nTotalCount;
    // 头像图片
    yl::string m_strImage;
    QPixmap m_pmImage;

    bool m_bReturn;

    // 全键盘弹出使界面上移，line图片需要调整位置
    int m_nLineOffset;
};

#endif
