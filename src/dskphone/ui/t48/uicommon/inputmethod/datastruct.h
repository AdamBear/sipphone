#ifndef DATASTRUCT_H
#define DATASTRUCT_H
#include "baseui/t4xpicpath.h"
#include "language/common/lang_common.h"
#include "language/input/include/imelogic.h"
#include <list>
#include <QtGui/QWidget>
using namespace std;

typedef unsigned char byte;

#define CUSTOME_KEYBOARD
#define CUSTOME_ENTERKEY

//image path
#define     PIC_COMMON_PATH     "images/"

//全键盘的位置信息
#define g_iFullKeyBoardX         0
#define g_iFullKeyBoardY         224
#define g_iFullKeyBoardWidth     800
#define g_iFullKeyBoardHeight    256

#define DEFAULT_QTEXTEDIT_MAXLENGTH  200

#define KEY_LONG_PRESS_TIME  2000
// 定义各种输入法对应的字符串
#define IME_123         "123"
#define IME_abc         "abc"
#define IME_ABC         "ABC"
#define IME_Abc         "Abc"
#define IME_2aB         "2aB"
#define IME_123_Dial    "123_Dial"
#define IME_123_Dial_Lync    "123_Dial_Lync"
#define IME_123_Star_Only_Dial    "123_Star_Only_Dial"  // *号键只有一种的输入法,用于预拨号界面特制
#define IME_123_IP      "123_IP"
#define IME_hebrew      "hebrew"
#define IME_symbol      "Symbol"

#define FONT_NORMALS        "FNormal"

#define FONT_NORMAL_SHIFT "FNormal_Shift"
#define FONT_NUMBER     "FNumber"   // 全键盘数字字库名

//自定义键盘字库
#define FONT_CUSTOM_KEYBOARD    "FCustom"

#define FONT_FULLKEYBOARD_PASSWORD "FontFullKeyBoardPassword"

// 定义各种输入法类型，对应的所有输入法
#define IME_ALL (IME_2aB "," IME_123 "," IME_abc "," IME_ABC "," IME_hebrew)
#define IME_DIGIT (IME_123 "," IME_abc "," IME_ABC "," IME_hebrew "," IME_2aB)
#define IME_LOWER (IME_abc "," IME_ABC "," IME_hebrew "," IME_2aB "," IME_123)
#define IME_UPPER (IME_ABC "," IME_hebrew "," IME_2aB "," IME_123 "," IME_abc)
#define IME_Number IME_123
#define IME_DIAL (IME_123_Dial "," IME_abc "," IME_ABC "," IME_2aB)
#define IME_DIAL_LYNC (IME_123_Dial_Lync "," IME_abc "," IME_ABC "," IME_2aB)
#define IME_STAR_ONLY_DIAL (IME_123_Star_Only_Dial "," IME_abc "," IME_ABC "," IME_2aB)
#define IME_DIAL_2AB (IME_2aB"," IME_123_Dial "," IME_abc "," IME_ABC)
#define IME_IP  IME_123_IP
#define IME_Name    (IME_Abc "," IME_2aB "," IME_123 "," IME_abc "," IME_ABC "," IME_hebrew)
#define IME_HEBREW_ALL (IME_hebrew "," IME_2aB "," IME_123 "," IME_abc "," IME_ABC)
#define IME_KEYBOARD_ALL (FONT_NORMAL "," FONT_NORMAL_SHIFT "," FONT_NUMBER)
#define IME_KEYBOARD_CUSTOM (FONT_CUSTOM_KEYBOARD)
#define IME_CUSTOM (IME_DIY "," IME_Abc "," IME_2aB "," IME_123 "," IME_abc "," IME_ABC)
// 密码界面输入法
#define IME_PWD (IME_abc_Pwd "," IME_ABC_Pwd "," IME_2aB_Pwd "," IME_123_Pwd)

#define IME_TITLE_123       "123"
#define IME_TITLE_symbol    "*#@"

#define KEYBOARD_NOUI "NoUI"
#define KEYBOARD_TOP9 "Top9"
#define KEYBOARD_NORMAL "Normal"
#define KEYBOARD_FULLKEYBOARD "FullKeyBoard"

enum INPUT_DIALOG_STYLE
{
    INPUT_DIALOG_NORMAL = 0,        // 初始状态
    INPUT_DIALOG_ENTER,             // Enter按键
    INPUT_DIALOG_DIAL,              // Dial按键
    INPUT_DIALOG_SEARCH,            // Search按键
    INPUT_DIALOG_NEWLINE,           // 换行按键
};

enum INPUT_DLGTYPE
{
    INPUT_DLGTYPE_NONE = 0,         // 无全键盘
    INPUT_DLGTYPE_NUM,              // 数字键盘
    INPUT_DLGTYPE_EN,               // 小写键盘
    INPUT_DLGTYPE_ENCAPITAL,        // 大写键盘
    INPUT_DLGTYPE_CUSTOM,           //自定义生成类型
};

enum KButtonType
{
    KButtonType_Normal = 0, //普通按钮
    KButtonType_LongPress,
};

enum INPUT_BOX_TYPE
{
    INPUT_BOX_HORIZONTAL_TYPE = 0,   //水平显示
    INPUT_BOX_VERTICAL_TYPE          //竖直排列
};

enum EnterKeyType
{
    ENTER_TYPE_NEXT = 0,
    ENTER_TYPE_DONE = 1,
    ENTER_TYPE_ICON = 2,
};

#define PIC_INPUTMETHOD_PATH    PIC_COMMON_PATH "inputmethod/"
const QString g_qstrFullKeyBoardEnterIcon               = PIC_INPUTMETHOD_PATH "btn_icon_enter.png";
const QString g_qstrFullKeyBoardSearchIcon              = PIC_INPUTMETHOD_PATH "btn_icon_search.png";
const QString g_qstrFullKeyBoardDialIcon                = PIC_INPUTMETHOD_PATH "btn_icon_dial.png";
const QString g_qstrFullKeyBoardNormal                  = PIC_INPUTMETHOD_PATH "fullkeyboardNormal.png";
const QString g_qstrFullKeyBoardNewLine                 = PIC_INPUTMETHOD_PATH "fullKeyboardnewline.png";

#define  g_qstrDialogFullEnBackGroundPic                QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEn.png")
#define  g_qstrDialogFullEnBackGroundWithEnterPic       QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnWithEnter.png")
#define  g_qstrDialogFullEnShiftBackGroundPic           QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnShift.png")
#define  g_qstrDialogFullEnShiftBackGroundWithEnterPic  QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnShiftWithEnter.png")
#define  g_qstrDialogFullNumberBackGoundPic             QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardNumber.png")
#define  g_qstrDialogFullNumberBackGoundWithEnterPic    QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardNumberWithEnter.png")

#define  g_qstrFullKeyBoardEnDeletePic          QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnDelete.png")
#define  g_qstrFullKeyBoardEnEnterPic           QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnEnter.png")
#define  g_qstrFullKeyBoardEnSendPic            QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnSend.png")
#define  g_qstrFullKeyBoardEnHidePic            QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnHide.png")
#define  g_qstrFullKeyBoardLeftPic              QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardLeft.png")
#define  g_qstrFullKeyBoardRightPic             QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardRight.png")
#define  g_qstrFullKeyBoardSpacePic             QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardSpace.png")


#define  g_qstrFullKeyBoardEnA                  QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnA.png")
#define  g_qstrFullKeyBoardEnB                  QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnB.png")
#define  g_qstrFullKeyBoardEnC                  QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnC.png")
#define  g_qstrFullKeyBoardEnD                  QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnD.png")
#define  g_qstrFullKeyBoardEnE                  QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnE.png")
#define  g_qstrFullKeyBoardEnF                  QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnF.png")
#define  g_qstrFullKeyBoardEnG                  QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnG.png")
#define  g_qstrFullKeyBoardEnH                  QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnH.png")
#define  g_qstrFullKeyBoardEnI                  QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnI.png")
#define  g_qstrFullKeyBoardEnJ                  QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnJ.png")
#define  g_qstrFullKeyBoardEnK                  QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnK.png")
#define  g_qstrFullKeyBoardEnL                  QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnL.png")
#define  g_qstrFullKeyBoardEnM                  QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnM.png")
#define  g_qstrFullKeyBoardEnN                  QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnN.png")
#define  g_qstrFullKeyBoardEnO                  QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnO.png")
#define  g_qstrFullKeyBoardEnP                  QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnP.png")
#define  g_qstrFullKeyBoardEnQ                  QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnQ.png")
#define  g_qstrFullKeyBoardEnR                  QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnR.png")
#define  g_qstrFullKeyBoardEnS                  QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnS.png")
#define  g_qstrFullKeyBoardEnT                  QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnT.png")
#define  g_qstrFullKeyBoardEnU                  QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnU.png")
#define  g_qstrFullKeyBoardEnV                  QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnV.png")
#define  g_qstrFullKeyBoardEnW                  QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnW.png")
#define  g_qstrFullKeyBoardEnX                  QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnX.png")
#define  g_qstrFullKeyBoardEnY                  QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnY.png")
#define  g_qstrFullKeyBoardEnZ                  QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnZ.png")


#define  g_qstrFullKeyBoardEnShiftA                 QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnShiftA.png")
#define  g_qstrFullKeyBoardEnShiftB                 QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnShiftB.png")
#define  g_qstrFullKeyBoardEnShiftC                 QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnShiftC.png")
#define  g_qstrFullKeyBoardEnShiftD                 QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnShiftD.png")
#define  g_qstrFullKeyBoardEnShiftE                 QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnShiftE.png")
#define  g_qstrFullKeyBoardEnShiftF                 QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnShiftF.png")
#define  g_qstrFullKeyBoardEnShiftG                 QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnShiftG.png")
#define  g_qstrFullKeyBoardEnShiftH                 QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnShiftH.png")
#define  g_qstrFullKeyBoardEnShiftI                 QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnShiftI.png")
#define  g_qstrFullKeyBoardEnShiftJ                 QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnShiftJ.png")
#define  g_qstrFullKeyBoardEnShiftK                 QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnShiftK.png")
#define  g_qstrFullKeyBoardEnShiftL                 QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnShiftL.png")
#define  g_qstrFullKeyBoardEnShiftM                 QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnShiftM.png")
#define  g_qstrFullKeyBoardEnShiftN                 QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnShiftN.png")
#define  g_qstrFullKeyBoardEnShiftO                 QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnShiftO.png")
#define  g_qstrFullKeyBoardEnShiftP                 QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnShiftP.png")
#define  g_qstrFullKeyBoardEnShiftQ                 QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnShiftQ.png")
#define  g_qstrFullKeyBoardEnShiftR                 QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnShiftR.png")
#define  g_qstrFullKeyBoardEnShiftS                 QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnShiftS.png")
#define  g_qstrFullKeyBoardEnShiftT                 QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnShiftT.png")
#define  g_qstrFullKeyBoardEnShiftU                 QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnShiftU.png")
#define  g_qstrFullKeyBoardEnShiftV                 QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnShiftV.png")
#define  g_qstrFullKeyBoardEnShiftW                 QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnShiftW.png")
#define  g_qstrFullKeyBoardEnShiftX                 QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnShiftX.png")
#define  g_qstrFullKeyBoardEnShiftY                 QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnShiftY.png")
#define  g_qstrFullKeyBoardEnShiftZ                 QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEnShiftZ.png")


#define  g_qstrFullKeyBoard0                            QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoard0.png")
#define  g_qstrFullKeyBoard1                            QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoard1.png")
#define  g_qstrFullKeyBoard2                            QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoard2.png")
#define  g_qstrFullKeyBoard3                            QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoard3.png")
#define  g_qstrFullKeyBoard4                            QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoard4.png")
#define  g_qstrFullKeyBoard5                            QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoard5.png")
#define  g_qstrFullKeyBoard6                            QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoard6.png")
#define  g_qstrFullKeyBoard7                            QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoard7.png")
#define  g_qstrFullKeyBoard8                            QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoard8.png")
#define  g_qstrFullKeyBoard9                            QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoard9.png")


#define  g_qstrFullKeyBoardAmpersand                    QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardAmpersand.png")
#define  g_qstrFullKeyBoardAsciiCircum                  QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardAsciiCircum.png")
#define  g_qstrFullKeyBoardAsciiTilde                   QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardAsciiTilde.png")
#define  g_qstrFullKeyBoardAsterisk                     QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardAsterisk.png")
#define  g_qstrFullKeyBoardAt                           QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardAt.png")
#define  g_qstrFullKeyBoardBackslash                    QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardBackslash.png")
#define  g_qstrFullKeyBoardBar                          QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardBar.png")
#define  g_qstrFullKeyBoardBraceLeft                    QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardBraceLeft.png")
#define  g_qstrFullKeyBoardBraceRight                   QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardBraceRight.png")
#define  g_qstrFullKeyBoardBracketLeft                  QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardBracketLeft.png")
#define  g_qstrFullKeyBoardBracketRight                 QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardBracketRight.png")
#define  g_qstrFullKeyBoardColon                        QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardColon.png")


#define  g_qstrFullKeyBoardApostrophe                   QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardApostrophe.png")
#define  g_qstrFullKeyBoardComma                        QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardComma.png")
#define  g_qstrFullKeyBoardQuestion                     QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardQuestion.png")
#define  g_qstrFullKeyBoardEqual                        QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardEqual.png")
#define  g_qstrFullKeyBoardExclam                       QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardExclam.png")
#define  g_qstrFullKeyBoardGreater                      QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardGreater.png")
#define  g_qstrFullKeyBoardLess                         QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardLess.png")
#define  g_qstrFullKeyBoardMinus                        QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardMinus.png")
#define  g_qstrFullKeyBoardNumberSign                   QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardNumberSign.png")
#define  g_qstrFullKeyBoardParenLeft                    QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardParenLeft.png")
#define  g_qstrFullKeyBoardParenRight                   QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardParenRight.png")
#define  g_qstrFullKeyBoardPercent                      QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardPercent.png")
#define  g_qstrFullKeyBoardPeriod                       QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardPeriod.png")
#define  g_qstrFullKeyBoardPlus                         QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardPlus.png")
#define  g_qstrFullKeyBoardQuoteDbl                     QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardQuoteDbl.png")
#define  g_qstrFullKeyBoardSemicolon                    QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardSemicolon.png")
#define  g_qstrFullKeyBoardSlash                        QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardSlash.png")
#define  g_qstrFullKeyBoardUnderscore                   QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardUnderscore.png")
#define  g_qstrFullKeyBoardDollar                       QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardDollar.png")


#define  g_qstrFullKeyBoardNumDelete                    QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardNumDelete.png")
#define  g_qstrFullKeyBoardNumEnter                     QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardNumEnter.png")
#define  g_qstrFullKeyBoardNumSend                      QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardNumSend.png")
#define  g_qstrFullKeyBoardNumHide                      QString(PIC_COMMON_PATH "inputmethod/" "FullKeyBoardNumHide.png")


typedef struct tagKeyBoardInfo
{
    std::list<INPUT_DLGTYPE>    m_lstInputDlgType;
    INPUT_DLGTYPE               m_enInputDlgType;
    bool                        m_bAutoShow;
    INPUT_DIALOG_STYLE          m_enShowStyle;
    tagKeyBoardInfo()
    {
        m_lstInputDlgType.clear();
        m_enInputDlgType = INPUT_DLGTYPE_NONE;
        m_bAutoShow = false;
        m_enShowStyle = INPUT_DIALOG_NORMAL;
    }
    void Init()
    {
        m_lstInputDlgType.clear();
    }
    void reset()
    {
        m_enInputDlgType = m_lstInputDlgType.front();
    }

} CKeyBoardInfo;

typedef struct tagFontsInfo
{
    std::list<string>   m_lstFonts;
    std::string         m_strCurrFontName;
    tagFontsInfo()
    {
        m_lstFonts.clear();
#ifdef CUSTOME_KEYBOARD
        m_strCurrFontName = FONT_CUSTOM_KEYBOARD;
#else
        m_strCurrFontName = FONT_NORMALS;
#endif
    }
    void Init()
    {
        m_lstFonts.clear();
    }
    void reset()
    {
        m_strCurrFontName = m_lstFonts.front();
    }
} CFontsInfo;

typedef struct tagWidgetInfo
{
    QWidget*            m_pwidget;
    CKeyBoardInfo       m_KeyBoardInfo;
    CFontsInfo          m_FontsInfo;
    int                 m_iMaxLen;
    bool                m_bNameFormat;
    tagWidgetInfo()
    {
        m_iMaxLen = DEFAULT_QTEXTEDIT_MAXLENGTH;
        m_pwidget = NULL;
        m_bNameFormat = false;
    }
    void reset()
    {
        m_KeyBoardInfo.reset();
        m_FontsInfo.reset();
    }
} CWidgetInfo;



#endif
