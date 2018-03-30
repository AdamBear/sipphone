
#ifndef __EDIT_LIST_ITEM_H__
#define __EDIT_LIST_ITEM_H__

#include <ylstring.h>
#include <ETLLib/ETLLib.hpp>
#include "detaillistitembase.h"
#include "uikernel/qlineeditutility.h"

enum
{
    LIST_ITEM_SUB_TYPE_DETAIL_EDIT      = 0x00020000,
};

class QLineEdit;

class CEditListItem : public CDetailListItemBase
{
    Q_OBJECT

public:
    CEditListItem(QWidget * parent = NULL, IListItemAction * pAction = NULL);
    virtual ~CEditListItem(void);

    static int GetEditListItemType();
    static CEditListItem* GetEditListItem(CListItemPtr pItem);

public:
    // 设置Edit框区域
    void SetEditRect(int nLeft, int nTop, int nWidth, int nHeight);
    void SetEditRect(const QRect & rcEdit);

    // 设置Edit框背景区域
    void SetEditBgRect(int nLeft, int nTop, int nWidth, int nHeight);
    void SetEditBgRect(const QRect & rcEdit);

    // 设置Edit框背景
    void SetEditBg(const char * pszEditBg);
    void SetEditBg(const yl::string & strEditBg);

    // 设置Edit焦点
    void SetEditFocus();

    void SetReadOnly(bool bReadOnly);

    void SetTitleWidth(int nTitleWidth);

    void SetLayout(bool bFrameLayout);

    // 返回所有的输入
    void ReturnAllInput();

    virtual void SetFocus(bool bFocus);

    // 设置Edit文字
    virtual void SetValue(const QString & strValue);

    // 获取Edit文字
    virtual QString GetValue() const;

    QLineEdit * GetLineEdit() const;

    void InstallEditFilter(QObject * pObject);
    void SetInputInfo(EDIT_TYPE eType, LPCSTR lpszIMEName, LPCSTR lpszDefault);
    void SetMaxLength(int nMaxLength);

protected:
    virtual void mousePressEvent(QMouseEvent * pEvent);
    virtual void paintEvent(QPaintEvent * pEvent);

    virtual void Relayout();
    void LayoutByFrameList();

    // 画Edit框背景
    void PaintEdit(QStylePainter & stylePainter, const char * pszBG = NULL);

protected:
    QLineEdit * m_pEdit;
    bool m_bFrameLayout;
    QRect m_rcEditBG;
    yl::string m_strBg;
};
typedef CEditListItem * CEditListItemPtr;

#endif
