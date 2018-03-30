#pragma once
#include "datastruct.h"
#include <QObject>
#include <string>
using namespace std;

class CWidgetInfoManager : public QObject
{
    Q_OBJECT
public:
    CWidgetInfoManager(void);
    ~CWidgetInfoManager(void);
public:
    static CWidgetInfoManager& GetInstance();
    // 保存控件绑定全键盘
    void SaveKeyBoard(QWidget* p, bool bIsAutoShow, INPUT_DLGTYPE enTyep, INPUT_DIALOG_STYLE enStyle = INPUT_DIALOG_NORMAL);
    // 设置控件的全键盘类型
    void SetKeyBoardType(QWidget* p, INPUT_DLGTYPE enTyep);

    // 保存控件绑定输入法
    bool SaveIME(QWidget* p, QString strFonts);
    // 删除控件p的缓存
    void DeleteWidget(QWidget* p);
    // 删除控件p中的strFont类型字库，并返回删除后的下一个字库类型
    std::string DeleteFontType(const QWidget* p, std::string strFont);
    // 获取控件对应的全键盘类型并使用，会设置控件p为当前控件
    INPUT_DLGTYPE GetKeyBoardTypeToUse(const QWidget* p);
    // 获取控件对应的全键盘类型
    INPUT_DLGTYPE GetKeyBoardType(const QWidget* p);
    // 获取控件对应的全键盘样式
    INPUT_DIALOG_STYLE GetKeyBoardStyleToUse(const QWidget* p);
    // 控件对应的全键盘是否自动显示
    bool IsKeyBoardAutoShow(const QWidget* p);
    // 设置全键盘类型
    void SetKeyBoardType(const QWidget* p, INPUT_DLGTYPE enTyep);
    // 设置字库类型
    void SetFontType(const QWidget* p, string strFont);
    // 获取当前字库类型
    string GetCurrFontType(const QWidget* p);
    // 设置当前字库类型
    void SetCurrWidgetFont(const QWidget* p, string strFont);
    // 获取控件下一个字库类型
    string GetNextFontsType(const QWidget* p);
    // 获取当前输入控件
    QWidget* GetCurrentWidget() const;
    // 判断是否是当前控件
    bool IsCurrWidget(const QWidget* p);
    // 判断是否可以切换此类型全键盘
    bool IsCanTransTo(INPUT_DLGTYPE enTyep);
    // 获取控件下一个全键盘类型
    INPUT_DLGTYPE GetNextKeyBoardType(INPUT_DLGTYPE enTyep);
    // 是否需要支持预输入(当前控件是否是密码框)
    bool IsPerInput();
    // 是否输入长度以达到最大
    bool IsTextOverflow();
    // 失去焦点
    void DealFocusOut();
    // 设置控件的默认输入法
    void SetCurrentIME(const QWidget* p, std::string strIMEName);
    // 获取控件当前的输入法名称
    std::string GetWidgetCurrName(const QWidget* p);
    // 获取控件所有输入法名称
    std::string GetWidgetFontsName(const QWidget* p);

    void SetQTextEditMaxLen(const QWidget* p, int iMaxLen);

    int GetQTextEditMaxLen(const QWidget* p);

    void SetNameFormat(QWidget* pWidget, bool bNameFormat);

    bool ChangeKeyBoardStyle(const QWidget* pWidget, INPUT_DIALOG_STYLE enStyle = INPUT_DIALOG_NORMAL);

    bool IsWidgetBind(QWidget* pWidget);

public:
    //当语言切换时，需要将所有有记录的键盘转换成新语言里相同类型的键盘
    void ResetAllDlgTypeByLanguage();

private:
    // 设置当前控件
    void SetCurrWidget(const QWidget* p);
    QString ToNameFormat(QString strInPut);
private slots:
    void AutoUpper();

    void AutoUpper(const QString & strText);

private:
    std::list<CWidgetInfo> m_lstWidgetInfo;         // 控件绑定信息列表
private:
    CWidgetInfo*        m_pCurrWidgetInfo;          // 当前控件绑定信息对象指针
};
#define g_WidgetInfoManager CWidgetInfoManager::GetInstance()
