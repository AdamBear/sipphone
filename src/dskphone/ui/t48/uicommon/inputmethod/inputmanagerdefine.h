#ifndef INPUT_MANAGER_DEFINE_H
#define INPUT_MANAGER_DEFINE_H

#include "path_define.h"

typedef YLList<yl::string>          listString;

#define KEYBOARD_LANG_ERROR_INDEX           -1
#define KEYBOARD_IME_ERROR_INDEX                -1
#define KEYBOARD_INPUTBOX_ERROR_INDEX   -1

////////////////////File///////////////////////
#define INPUT_FILES_FACTORY_PATH                    FACTORY_PATH "keyboard/"
#define INPUT_FILES_CONFIG_PATH                 CONFIG_PATH "keyboard/"

#define INPUT_FILES_PREFIX_LAYOUT                   "keyboard_layout_"
#define INPUT_FILES_PREFIX_LAYOUT_LENGTH    16

#define INPUT_FILES_PREFIX_IME                          "keyboard_ime_"
#define INPUT_FILES_PREFIX_IME_LENGTH           13

#define INPUT_FILES_LANG                                    "keyboard_lang.xml"
#define INPUT_FILES_LANG_LENGTH                 13

#define INPUT_LAYOUT_NAME                           "KeyboardLayout"
#define INPUT_IME_NAME                                  "KeyboardIme"
#define INPUT_LANG_NAME                             "KeyboardLang"

#define INPUT_XML_NODE_LINE                         "Line"
#define INPUT_XML_NODE_KEY                          "Key"
#define INPUT_XML_NODE_LANG                     "Lang"
#define INPUT_XML_NODE_IME                          "Ime"
//////////////////////////////////////////////

//////////////////Layout////////////////////
#define INPUT_LAYOUT_ATTRI_LEFTINDENT       "LeftIndent"
#define INPUT_LAYOUT_ATTRI_TOPINDENT        "TopIndent"
#define INPUT_LAYOUT_ATTRI_WIDTH                "Width"
#define INPUT_LAYOUT_ATTRI_HEIGHT               "Height"

//Layout中单个按钮的位置
struct SKeyboardKeyRect
{
    int             m_nLeft;
    int             m_nTop;
    int             m_nWidth;
    int             m_nHeight;
};

struct SKeyboardKeyPosition
{
    int             m_nLeftIndent;
    int             m_nTopIndent;
    int             m_nWidth;
    int             m_nHeight;
};

//Layout中单个按钮的信息
struct SKeyboardLayoutKey
{
    SKeyboardKeyPosition                                        m_KeyPosition;      //从文件中读到的信息

    //经过计算后可以直接使用的位置
    SKeyboardKeyRect                                                m_rectButton;           //按钮显示位置
    SKeyboardKeyRect                                                m_rectRespond;      //按钮的实际响应范围
};

typedef QVector< SKeyboardLayoutKey >           vecKeyboardLayoutLine;

struct SKeyboardLayoutLine
{
    SKeyboardKeyPosition                                        m_LinePosition;                     //该行的默认值
    vecKeyboardLayoutLine                                       m_vecKeyboardLine;
};

typedef QVector< SKeyboardLayoutLine >          vecKeyboardLayoutTable;

struct SKeyboardLayout
{
    yl::string                                                              m_strLayoutName;                    //布局的文件名
    vecKeyboardLayoutTable                                  m_vecKeyboardLayoutTable;   //存放每个按钮的位置信息
};

typedef QVector< SKeyboardLayout >              vecKeyboardLayout;
/////////////////////////////////////////////

//////////////////Ime////////////////////
#define INPUT_IME_ATTRI_LAYOUT                      "Layout"
#define INPUT_IME_ATTRI_DISPLAYNAME         "DisplayName"
#define INPUT_IME_ATTRI_CAPITALNAME         "CapitalName"
#define INPUT_IME_ATTRI_IMETYPE                 "ImeType"

#define INPUT_IME_ATTRI_DISPLAY                     "Display"
#define INPUT_IME_ATTRI_FUCTION                 "Function"
#define INPUT_IME_ATTRI_IME_NORMAL          "ImeNormal"
#define INPUT_IME_ATTRI_IME_CAPITAL             "ImeCapital"

////ImeType Define////
#define KEYBOARD_IME_TYPE_CHAR                  "Char"
#define KEYBOARD_IME_TYPE_SYMBOL                "Symbol"

////Function define////
#define INPUT_IME_FUNCTION_BACKSPACE        "backspace"
#define INPUT_IME_FUNCTION_SPACE                "space"
#define INPUT_IME_FUNCTION_ENTER                "enter"

#define INPUT_IME_FUNCTION_HIDE                 "hide"
#define INPUT_IME_FUNCTION_SHIFT                    "shift"
#define INPUT_IME_FUNCTION_LANG                 "lang"
#define INPUT_IME_FUNCTION_SWITCH               "switch"

#define INPUT_IME_FUNCTION_LEFT                 "left"
#define INPUT_IME_FUNCTION_RIGHT                "right"
#define INPUT_IME_FUNCTION_UP                       "up"
#define INPUT_IME_FUNCTION_DOWN             "down"
/////////////////////////

struct SKeyBoardImeKey
{
    yl::string                                                              m_strDisplayName;   //显示名
    yl::string                                                              m_strFunction;          //功能定义
    yl::string                                                              m_strImeList;               //备选列表
};

typedef QVector< SKeyBoardImeKey >              vecKeyboardImeKeyLine;
typedef QVector< vecKeyboardImeKeyLine >        vecKeyboardImeKeyTable;

//键盘类型
enum IME_TYPE
{
    IME_TYPE_ERROR = 0,
    //字符键盘
    IME_TYPE_CHAR,
    //大写字符
    IME_TYPE_CAPITAL,
    //符号键盘（数字键盘）
    IME_TYPE_SYMBOL,
};

struct SKeyboardIme
{
    int                                                                     m_nImeIndex;                //ime的序号
    IME_TYPE                                                            m_eImeType;             //Ime类型
    yl::string                                                              m_strImeName;           //该Ime的文件名
    yl::string                                                              m_strLayoutName;        //对应的布局文件名
    yl::string                                                              m_strDisplayName;   //该Ime的显示名
    vecKeyboardImeKeyTable                                  m_vecImeKey;                //每个按钮对应的功能列表
};

typedef QVector< SKeyboardIme >                     vecKeyboardIme;
/////////////////////////////////////////////

//////////////////Layout////////////////////
#define INPUT_LANG_ATTRI_NAME                       "Name"

typedef QVector< int >                                          vecKeyboardImeIndex;

struct SKeyBoardLang
{
    yl::string                                                              m_strLangName;  //语言名字
    vecKeyboardImeIndex                                     m_vecImeIndex;      //所包含的ime序号列表
};

typedef QVector< SKeyBoardLang >                    vecKeyboardLang;
/////////////////////////////////////////////

#endif // INPUT_MANAGER_DEFINE_H
