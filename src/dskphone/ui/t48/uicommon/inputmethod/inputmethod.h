#ifndef _BASEUI_INPUTMETHOD_H
#define _BASEUI_INPUTMETHOD_H

#include <QtGui/QWidget>
#include "inputmanager.h"
#include "datastruct.h"

//复位时显示全键盘类型
enum ShowFullKeyboardType
{
    FullKeyboardType_EN         = 0,    //显示英文键盘
    FullKeyboardType_ENShift,           //显示大写英文键盘
    FullKeyboardType_Number,            //显示数字键盘
};

void FullKeyboard_Init();
void InputMethod_Init();
bool FullKeyboardEnabled();
//接口开始
//新增接口 注册输入法添加了参数FontName 这边的参数是名称
void qInstallIME(QWidget* pWidget, QString qstrInputBaseName, QString _qstrFontBaseName);
//设置输入法窗口位置 这边的参数是名称
void qSetGeometry(QWidget* _pQwidget, QString _qstrInputBaseName, int _iLeft, int _iTop, int _iWidth, int _iHeight);
//设置是否显示输入法窗口 这边的参数是名称
void qSetIsShowWnd(const QWidget* _pQwidget, QString _qstrInputBaseName, bool bIsShowWnd);
//设置是否显示输入法窗口 这个不需要输入法名称 将整个和该编辑框相关的输入法全部隐藏
void qSetIsShowWnd(const QWidget* _pQwidget, bool bIsShowWnd);

// 隐藏全键盘
void qSetHideKeyboard();

// 暂时不显示全键盘 主要是为了framelist切页时触发的focus事件
void qBlockKeyboard(bool bBlock = false);

//有一个是显示的 就返回true 都不显示 返回false
bool qGetIsShowWnd();

// 为控件安装输入法：可同时安装多个输入法名称以','分隔
// pWidget为控件指针  lpszIMEName 最好用已经定义好的输入法 这边的参数是名称
void qInstallIME(QWidget* pWidget, LPCSTR lpszIMEName, const yl::string & strIMEType = "");

// 为控件卸载输入法
void qUninstallIME(QWidget* pWidget);

// ----------------------------------
// 接口说明： qInstallKeyBoard
// 功能：     为控件绑定全键盘
// 参数说明： p                 绑定的控件
//            bIsAutoShow       是否托管显示(true：托管，控件获取、失去焦点时，将自动显示、隐藏全键盘)
//            enTyep            全键盘类型(小写、大写、数字键盘)
//            enStyle           Enter区域的类型(Dial、Enter、换行、search等)
// ----------------------------------
void qInstallKeyBoard(QWidget* p, bool bIsAutoShow, INPUT_DLGTYPE enTyep, INPUT_DIALOG_STYLE enStyle = INPUT_DIALOG_NORMAL);

void qSetKeyBoardType(QWidget* p, INPUT_DLGTYPE enTyep);

void qSetEnterType(EnterKeyType eEnterType);

void qRaiseFullKeyboard();

// 获取控件当前输入法,对应按键的第一个字符 这边的参数是名称
QString qGetIMEFirstChar(QString lpszIMEName, int iKeyCode);

void qSetNameFormat(QWidget* pWidget, bool bNameFormat);

//将文字转成名称格式，即单词首字母大写
QString ToNameFormat(QString strInPut);

// 获取控件的当前输入法显示名 这边返回的是 title
QString qGetCurrentIMETitle(QWidget* pWidget);

// 获取控件的当前输入法名称 这边返回的是名称
QString qGetCurrentIMEName(QWidget* pWidget);

// 设置控件的当前输入法 这边设置进去的参数是名称
void qSetCurrentIME(QWidget* pWidget, QString lpszIMEName, int iCusIndex = 0, const yl::string & strIMEType = "");

// 设置控件的输入法类型
void qSetWidgetIMEType(QWidget * pWidget, const yl::string & strIMEType = "");

void qUseKeyBoardFromIME(QWidget* pWidget, INPUT_DIALOG_STYLE enStyle = INPUT_DIALOG_NORMAL, bool bIsAutoShow = false);

// 当前控件切换到下一个输入法
// 若当前为最后一个输入法会自动切换到第一个输入法 这边返回的是title
QString qShift2NextIME(QWidget* pWidget);

// 获取当前控件全部输入法名称 根据qt意见这边返回的是title
QString qGetAllIMEName(QWidget* pWidget);

//重新从文件 加载输入法
void qReloadIME();

// 设置QTextEdit的最大长度
bool qSetQTextEditMaxLength(QWidget* pWidget, int maxLen);

// 获取QTextEdit的最大长度，返回-1表示失败
int qGetQTextEditMaxLength(QWidget* pWidget);

// 把事件传递到输入控件，QObject必须是输入控件
bool qPassEventToEdit(QObject* pObj, QEvent *pEvent);

// 点击空白键调用的操作
// pWidget是控件对应的窗体
void qOnBackspaceClicked(QWidget* pWidget);

// 将预编辑的字符串输入控件，并返回控件上的内容
QString qReturnEdit(QWidget* pWidget);

// 输入预编辑内容
// 返回true表示处于预编辑状态
// 返回false表示不处于预编辑状态
bool InputPreeditContent(QWidget* pWidget);

// 判断控件是否处于预编辑状态
bool qIsPreedit(QWidget* pWidget);

// 是否播放动画
void SetIsPlayKeyboardAnimation(bool bPlay);

bool qChangeKeyBoardStyle(const QWidget* pWidget, INPUT_DIALOG_STYLE enStyle = INPUT_DIALOG_NORMAL);

const QWidget* GetCurrInputWgt();

// 设置长按按键变化的相关数据
//
void qStartLongPressInput(QObject* pObj, int iSrcKeyCode, int iDestKeyCode, int iLongPressTime);

void qStopLongPressInput(int iKeyCode);

bool qIsLongInputWork();

//将因全键盘变化引起的界面变化所需要执行的操作注册到输入法管理模块中
bool inputMethod_RegisterKeyBoardShowHookProc(KEYBOARD_SHOW_HOOK_PROC keyboardShowHook, KEYBOARD_SHOW_HOOK_MODULE priority);
bool inputMethod_UnRegisterKeyBoardShowHookProc(KEYBOARD_SHOW_HOOK_PROC keyboardShowHook);


// 旧输入法实现
// 用来临时存放输入法，用于输入法改变时重新加载
struct WidgetImeAttr
{
    QWidget    *    pWidget;
    LPCSTR          lpSet;
    yl::string      strCurrentImeName;

    WidgetImeAttr() : pWidget(0), lpSet(0) {}
    operator bool() const
    {
        return pWidget && lpSet && !strCurrentImeName.empty();
    }
};

/************************************************************************/
/*                         about  class  imeItem                        */
/************************************************************************/
// 重命名
typedef ImeKeyMap                    mapKeys;
typedef YLList<mapKeys *>             listKeyMaps;

// 管理输入法的类，本质上是一种过滤器
// 一个管理类对应一个控件
class imeItem : public QObject
{
    Q_OBJECT

public:
    // 构造函数
    explicit imeItem();

    // 添加一种输入法
    void AddKeymaps(mapKeys * pKeyMaps);

    // 设置当前输入法
    void SetCurrentkeymaps(mapKeys * pKeyMaps);

    // 获得当前输入法的下一种输入法
    mapKeys * GetNextKeymaps();

    bool IsNameFormat()
    {
        return m_bNameFormat;
    }

    //设置是否为名称格式，如果是，单词首字母自动大写
    void SetNameFormat(bool bNameFormat);

    // 事件过滤器
    virtual bool eventFilter(QObject * pObj, QEvent * pEvent);

    void SetHostWidget(QWidget * pWidget);

    QWidget * GetWidget()
    {
        return m_pHostWidget;
    }
    void SetImeSet(LPCSTR lpSet)
    {
        m_pImeSet = lpSet;
    }
    WidgetImeAttr GetAttr();

    size_t GetImeCount()
    {
        return m_listValidKeyMaps.size();
    }

    void SetIMEType(const yl::string & strIMEType)
    {
        m_strIMEType = strIMEType;
    }

public:
    // 输入处于预编辑状态的字符
    bool commitInput();

public:
    // 启动计时器
    void startTimer();

    // 停止计时器
    void stopTimer();

private:
    // 重写的虚函数
    // 当类本身设置的计时器到时调用
    virtual void timerEvent(QTimerEvent * pEvent);

    // 处理点击事件
    bool OnKeyPress(QObject * pObj, QEvent * pEvent);

private:
    // 判断编辑框是否溢出
    BOOL isTextOverflow() const;

    // 将字符串输出到输入框
    BOOL sendCommitToEdit(const yl::string & strText) const;

    // 将预编辑的字符输出到输入框
    BOOL sendPreeditToEdit(const yl::string & strText) const;
public slots:
    //转换成名称格式（单词首字母大写）
    void AutoUpper();
    void AutoUpper(const QString & strText);
    void ToAbcFormat(yl::string & strPreedit);
    //检查文字方向（目前用于希伯来语输入）
    void CheckTextDirection();
    void CheckTextDirection(const QString & strText);

public:
    QWidget  *  m_pHostWidget;   // 输入法管理类对应的控件
    mapKeys  *  m_pCurrentKeyMap;// 指向当前输入法的指针

    int         m_nLastKeyCode;  // 保存最后一次键值
    int         m_nRepeatCount;  // 统计按了同一个有效按键几次
    int         m_nTimerID;      // 计时器的ID
    yl::string  m_strPreeditString; // 预编辑的字符串
    listKeyMaps m_listValidKeyMaps; // 这个控件所支持的输入法

    int         m_nQTextEditMaxLength; // QTextEdit最大输入数
    bool        m_bNameFormat; //是否为名称格式，如果是，单词首字母自动大写
    LPCSTR      m_pImeSet;
    yl::string  m_strIMEType; // 输入框类型
};


/************************************************************************/
/*                        about  IME  Manager                           */
/************************************************************************/
// 重命名
typedef YLList<imeItem *>            listItems;

// 管理输入法和对应控件
class IMEManager
    : public listItems, public SingletonBase < IMEManager >
{
public:
    // 给控件安装输入法
    void InstallIME(QWidget * pWidget, LPCSTR lpszIMEName, const yl::string & strIMEType = "");

    // 重新加载已经安装的输入法
    void ReloadIME();

    // 重新加载输入法时，对于已经安装的输入法，也要重新加载
    void PreloadIME();

    void SetNameFormat(QWidget * pWidget, bool bNameFormat);

    // 删除控件的输入法
    void UninstallIME(QWidget * pWidget);

public:
    // 获得当前输入法的显示名
    yl::string GetCurrentIMETitle(QWidget * pWidget);

    // 获得当前输入法的名称
    yl::string GetCurrentIMEName(QWidget * pWidget);

    // 设置控件的当前输入法
    void SetCurrentIME(QWidget * pWidget, LPCSTR lpszIMEName, int iCusIndex = 0, const yl::string & strIMEType = "");

    // 设置控件输入法类型
    void SetWidgetIMEType(QWidget * pWidget, const yl::string strIMEType = "");

    // 切换到下一个输入法
    yl::string Shift2NextIME(QWidget * pWidget);

    // 设置QTextEdit的最大输入数
    bool setQTextEditMaxLength(QWidget * pWidget, int maxLen);

    // 返回QTextEdit的最大输入数
    int getQTextEditMaxLength(QWidget * pWidget);

    // 发射某个事件到对应的Edit上
    bool passEventToEdit(QObject * pObj, QEvent * pEvent);

    // 通过IME名字查找对应按键的第一个字符
    yl::string GetIMEFirstChar(LPCSTR lpszIMEName, int iKeyCode);

    // 获取当前控件全部输入法名称
    yl::string GetAllIMEName(QWidget * pWidget);
private:
    // 返回KeyMaps对应的输入法显示名
    yl::string keyMaps2Title(mapKeys * pKeyMaps);

    // 判断imeItem中是否有某种输入法
    bool IsIMEExist(LPCSTR lpszIMEName, imeItem * pItem);

public:
    // 查找pWidget对应的imeItem
    imeItem * findImeItem(QWidget * pWidget);

    // 获得控件对应的IMEItem对象
    imeItem & getIMEItem(QWidget * pWidget);

private:
    // 释放输入法
    void ReleaseIme();

public:

    ~IMEManager();

private:
    typedef std::vector<WidgetImeAttr> vecAttr;
    vecAttr m_vecImeAttr;
};

#define _ImeMagager GET_SINGLETON_INSTANCE(IMEManager)

#endif
