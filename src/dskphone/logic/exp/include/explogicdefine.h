#ifndef _EXP_LOGIC_DEFINE_HEADER_
#define _EXP_LOGIC_DEFINE_HEADER_
#include "innermsgdefine.h"
#include "ETLLib.hpp"

#define EXP_PROCESS_DELAY_CLOSE_TIME    60000
#define EXP_INFO_SEND_TIME                          100

#define EXP_PAGE_SIZE 20
#define EXP38_PAGE_SIZE 19

#define EXP_SEND_LIST_MAX_COUNT     30

//显示文字的最大长度
#define EXP_LABEL_MAX_SIZE 32
//传入的图片路径字符串最大长度
#define EXP_ICON_PATH_MAX_SIZE  128
#define EXP_PIC_PATH_MAX_SIZE       512
//测试模式用整个屏幕显示文字的最大长度
#define EXP_SHOW_TEXT_MAX_SIZE  128

#define EXP50_PAGEKEY_LINE_INDEX        10
#define EXP50_PAGEKEY_START             20
#define EXP50_PAGEKEY_COUNT             3

#define EXP_KEY_PAGE1 20
#define EXP_KEY_PAGE2 21
#define EXP_KEY_PAGE3 22

#define EXP_KEY_NONE                    -1
#define EXP_KEY_STATUS_PRESS            1
#define EXP_KEY_STATUS_RELEASE          0

//扩展台绘制消息定义
enum EXP_MESSAGE
{
    EXP_MESSAGE_BEGIN = INNER_MSG_EXP_MESSAGE_BEGIN,

    //要求刷新的消息
    //wParam:当前扩展台总个数
    //extraData：要刷新的数据
    EXP_MESSAGE_UPDATE_KEY,

    //背景改变的消息
    //wParam:当前扩展台总个数
    //extraData：当前背景图片和按钮背景的路径 ExpBgData
    EXP_MESSAGE_BG_CHANGE,

    //要求重新刷新背景的消息
    //wParam:当前扩展台总个数
    //lParam:需要刷新的扩展台index
    EXP_MESSAGE_BG_REFRESH,

    //背光灯亮度改变的消息
    //wParam:当前扩展台总个数
    //lParam：调整后的亮度等级
    EXP_MESSAGE_BACKLIGHT,

    //重置全部灯
    //wParam:当前扩展台总个数
    //lParam:要刷新的扩展台index
    EXP_MESSAGE_RESET_LIGHT,

    //设定编辑中的提示文本
    //extraData：编辑提示文本
    EXP_MESSAGE_EDITING_TEXT,

    //扩展台数量变化
    //wParam：当前扩展台总个数
    EXP_MESSAGE_COUNT_CHANGE,

    //扩展台进程启动完成
    EXP_MESSAGE_PROCESS_READY,

    //扩展台重新加载驱动映射的内存
    //wParam: 1:扩展台从无到有  0:扩展台从有到无
    EXP_MESSAGE_PROCESS_RELOAD,

    //扩展台进入、退出屏保
    //wParam: 1:进入屏保  0:退出屏保
    EXP_MESSAGE_SCREENSAVER,

    // switch to the specify page of LCD exp
    // wParam: exp index
    // lParam: page index
    EXP_MESSAGE_SWITCH_PAGE,

    // set the contrast of LCD exp
    // wParam: exp index
    // lParam: contrast val
    EXP_MESSAGE_SET_CONTRAST,
    /******************************************测试模式使用***********************************************/

    //使用特定颜色填充扩展台屏幕
    //wParam:当前扩展台总个数
    //lParam:要刷新的扩展台index
    //extraData：颜色结构体
    EXP_MESSAGE_FILL_COLOR,


    //使用特定图片填充扩展台屏幕
    //wParam:当前扩展台总个数
    //lParam:要刷新的扩展台index
    //extraData：图片路径
    EXP_MESSAGE_FILL_PICTURE,

    //写特定文字
    //wParam:当前扩展台总个数
    //lParam:要刷新的扩展台index
    //extraData：文本字符串
    EXP_MESSAGE_DRAW_TEXT,

    //显示welcome界面
    //wParam:当前扩展台总个数
    //lParam:要刷新的扩展台index
    EXP_MESSAGE_SHOW_LOGO,

    EXP_MESSAGE_END = INNER_MSG_EXP_MESSAGE_END
};
//显示底色模式，临时添加
enum EXP_DSS_KEY_GROUNDPHOTO_MODE
{
    EXP_DSS_GROUNDPHOTO_MODE_DISALBE,   //显示disable状态的底色
    EXP_DSS_GROUNDPHOTO_MODE_HIGHLIGHT, //显示高亮状态的底色
    EXP_DSS_GROUNDPHOTO_MODE_NORMAL,    //显示一般状态的底色
    EXP_DSS_GROUNDPHOTO_MODE_USING,     //显示处理通话状态的底色
};

//EXP传递的数据类型
enum EXP_UPDATE_DATA_TYPE
{
    EXP_UPDATE_DATA_NOMAL = 0,
    EXP_UPDATE_DATA_FLASH_PAGE,
};

//扩展台绘制数据
struct ExpDisplayItemInfo
{
    int m_nDsskeyID;

    int m_nIndex;
    int m_nPage;
    int m_nRol;
    int m_nCol;

    char szLabel[EXP_LABEL_MAX_SIZE];
    int icon;
    int ledStatus;
    int flag;

    //是否仅刷新图标
    bool bPaintIcon;
    //是否仅刷新文字
    bool bPaintText;
    //是否仅刷新灯
    bool bLight;
    //是否处于按下状态
    bool bDown;
    //是否处于编辑中状态
    bool bEditing;

    //消息类型
    EXP_UPDATE_DATA_TYPE m_eDataType;

    EXP_DSS_KEY_GROUNDPHOTO_MODE m_eBgType;

    //自定义图标
    char szIcon[EXP_ICON_PATH_MAX_SIZE];
    //状态图标
    int nStatus;

    int nCurrentPage;
    ExpDisplayItemInfo()
    {
        m_nDsskeyID = 0;

        m_nIndex = 0;
        m_nPage = 0;
        m_nRol = 0;
        m_nCol = 0;

        memset(szLabel, 0, sizeof(szLabel));
        icon = 0;
        ledStatus = -1;
        flag = 0;
        nCurrentPage = 0;
        bPaintIcon = true;
        bPaintText = true;
        bLight = false;
        bDown = false;
        bEditing = false;

        memset(szIcon, 0, sizeof(szIcon));
        nStatus = 0;
        m_eBgType = EXP_DSS_GROUNDPHOTO_MODE_NORMAL;

        m_eDataType = EXP_UPDATE_DATA_NOMAL;
    }

    ExpDisplayItemInfo & operator=(const ExpDisplayItemInfo & data)
    {
        if (this != &data)
        {
            m_nDsskeyID = data.m_nDsskeyID;

            m_nIndex = data.m_nIndex;
            m_nPage = data.m_nPage;
            m_nRol = data.m_nRol;
            m_nCol = data.m_nCol;

            strcpy(szLabel, data.szLabel);

            icon = data.icon;
            ledStatus = data.ledStatus;
            flag = data.flag;
            nCurrentPage = data.nCurrentPage;
            bPaintIcon = data.bPaintIcon;
            bPaintText = data.bPaintText;
            bLight = data.bLight;
            bDown = data.bDown;
            bEditing = data.bEditing;

            strcpy(szIcon, data.szIcon);
            nStatus = data.nStatus;

            m_eBgType = data.m_eBgType;

            m_eDataType = data.m_eDataType;
        }

        return *this;
    }
};

struct ExpInfoList
{
    int m_nCount;
    ExpDisplayItemInfo listExpInfo[EXP_SEND_LIST_MAX_COUNT];
};


struct ExpBgData
{
    char szBackGround[EXP_PIC_PATH_MAX_SIZE];
    int nTransparentLevel;

    ExpBgData()
    {
        memset(szBackGround, 0, sizeof(szBackGround));
        nTransparentLevel = 1;
    }

    ExpBgData & operator=(const ExpBgData & data)
    {
        if (this != &data)
        {
            strcpy(szBackGround, data.szBackGround);
            nTransparentLevel = data.nTransparentLevel;
        }

        return *this;
    }
};

struct EXP_FILL_COLOR
{
    int nRed;
    int nGreen;
    int nBlue;

    bool bGradient;

    EXP_FILL_COLOR()
    {
        nRed = 0;
        nGreen = 0;
        nBlue = 0;

        bGradient = false;
    }

    EXP_FILL_COLOR(const EXP_FILL_COLOR & data)
    {
        if (this != &data)
        {
            nRed = data.nRed;
            nGreen = data.nGreen;
            nBlue = data.nBlue;

            bGradient = data.bGradient;
        }
    }

    EXP_FILL_COLOR & operator=(const EXP_FILL_COLOR & data)
    {
        if (this != &data)
        {
            nRed = data.nRed;
            nGreen = data.nGreen;
            nBlue = data.nBlue;

            bGradient = data.bGradient;
        }

        return *this;
    }
};

#endif //_EXP_LOGIC_DEFINE_HEADER_
