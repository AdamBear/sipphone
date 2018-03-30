#ifndef QBTNBAR_H
#define QBTNBAR_H

#include "callmanagerbarbase.h"

//#include "QFrame"

class qCallManagerBarItem;

const int MaxNumOfBtnItem = 4;                  // BtnItem最多个数

class qCallManagerBar : public CCallManagerBarBase
{
    Q_OBJECT

public:
    explicit qCallManagerBar(QWidget * parent = 0);
    ~qCallManagerBar(void);

public:
    //设置所有 item 信息
    virtual void SetCallMGRListInfo(YLList<CCallMGRInfo *> & listCallMGRInfo);

    //设置焦点 item
    virtual void SetFocusMGRInfo(const yl::string & strUIName);

protected:
    //创建单个按钮
    void CreateItemBar(CCallMGRInfo * pCallMGRInfo);

public:
    // 设置、获取Background
    void SetBackgroundPicture(const QString & strBackgroundPic);
    QString GetBackGroundPicture();
    // 设置、获取分隔线
    void SetDivideLinePicture(const QString & strDivideLinePic);
    QString GetDivideLinePicutre();
    // 设置、获取选中的Index
    void SetCurSelectIndex(const int iCurSelIndex);
    int GetCurSelectIndex();

    // 添加BtnItem
    bool AddBtnItem(qCallManagerBarItem * pBtnItem);
    void AddBtnItems(qCallManagerBarItem * pBtnItems[], int iCnt);

    // 设置、获取第Index个BtnBarItem的Action
    void SetIndexAction(const QString & strClickAction, int iIndex = 0);
    QString GetIndexAction(int iIndex = 0);
    // 设置、获取第Index个BtnBarItem的Icon
    void SetIndexIcon(const QString & strIcon, int iIndex = 0);
    QString GetIndexIcon(int iIndex = 0);
    // 设置、获取第Index个BtnBarItem的Text
    void SetIndexText(const QString & strText, int iIndex = 0);
    QString GetIndexText(int iIndex = 0);
    // 设置、获取第Index个BtnBarItem的SelectPic
    void SetIndexSelectPic(const QString & strSelectPic, int iIndex = 0);
    QString GetIndexSelectPic(int iIndex = 0);

    // 设置选中的Index
    bool SetSelectIndex(const int iIndex);
    // 设置选中的Action
    bool SetSelectAction(const QString & strSelectAction);

protected:
    void paintEvent(QPaintEvent * pPaintEvent);

private:
    int FindBtnByClickAction(const QString & strClickAction);
    int FindBtnByIndex(const int iIndex);
    // 设置BtnItem位置
    void SetBtnItemLoaction();

    // 更新BtnBarItem状态、设置选中项
    bool UpdateBtnBarItem(int iIndex);

signals:
    void ClickAction(const QString & strClickAction);

private slots:
    void OnBtnClick(QString strClickAction);

private:
    QString         m_strBackgroundPic;             // 背景图片
    QString         m_strDivideLinePic;             // 分隔线图片
    qCallManagerBarItem  *  m_pBtnItem[MaxNumOfBtnItem];    // BtnItem
    int             m_iNumOfBtnItem;                // BtnItem个数
    int             m_iCurSelectItem;               // 当前选中的Index
};

#endif
