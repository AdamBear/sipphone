#ifndef _RECT_DEFINE_H_
#define _RECT_DEFINE_H_

// 话机屏幕的宽度
/*#ifdef _T4X
const int PHONE_WIDTH  = 800;
#else
const int PHONE_WIDTH  = 480;
#endif*/
////---- T48
const int PHONE_WIDTH  = 800;

// 话机屏幕的高度
/*#ifdef _T4X
const int PHONE_HEIGHT = 480;
#else
const int PHONE_HEIGHT = 272;
#endif*/
////---- T48
const int PHONE_HEIGHT = 480;

const QRect RECT_FULL_SCREEN(0, 0, PHONE_WIDTH, PHONE_HEIGHT);

// softkey高度
////---- T48
const int SOFTKEYBAR_HEIGHT = 64;

// Title的高度
const int TITLE_HEIGHT = 43;

// lineEdit的高度
const int LINEEDIT_HEIGHT = 36;

#define DEFAULT_TREE_ITEM_WIDTH 256
#define DEFAULT_TREE_ITEM_HEGITH 46
#define CONTACT_LIST_ITEM_CONTENT_LEFT_MARGIN 16
#define CONTACT_LIST_ITEM_CONTENT_RIGHT_MARGIN 16
#define DEFAULT_DLG_TOP 53
#define DEFAULT_LEFT_LIST_ITEM_SPACE 1
#define DEFAULT_RIGHT_LIST_ITEM_SPACE 1

// Directory界面位置
const QRect RECT_DIRECTORY(4, DEFAULT_DLG_TOP, 792, 336);
const QRect RECT_DIR_GROUP(0, 0, 255, 336);
const QRect RECT_DIR_SEARCH_TITLE(0, 0, 255, 336);
const QRect RECT_DIR_CONTACT(257, 1, 533, 334);
const QRect RECT_DIR_CONTACT_DETAIL(251, 0, 414, 332);

// Talk界面位置
#ifdef _T4X
#ifdef _T48
const QRect RECT_TALK_FRAME(0, 0, PHONE_WIDTH, 390);
#else
const QRect RECT_TALK_FRAME(0, 0, PHONE_WIDTH, 232);
#endif
#else
const QRect RECT_TALK_FRAME(0, 0, 360, 215);
#endif //_T4X

//通话UI位置
#ifdef _T3X
const QRect RECT_TALK_PAGE(0, 36, 360, 196);
#endif
#ifdef _T4X
const QRect RECT_TALK_PAGE(105, 39, 270, 188);
#endif

// Zero界面位置
const QRect RECT_ZERO(4, 57, 792, 336);

// Menu界面的位置
const QRect RECT_MENU(0, 57, PHONE_WIDTH, 344);

// setting界面位置
//改成与其他界面 如dir 相同
const QRect RECT_SETTING(4, DEFAULT_DLG_TOP, 792, 336);//(4, 60, 792, 329);

const QRect RECT_VOLUMEBAR(278, 350, 244, 34);  // 音量条控件的位置

// XMLBrowser界面位置
// XMLBrowser窗口大小
const QRect RECT_XML_BROWSER_DIALOG(156, 49, 486, 353);
// XMLBrowserFrame大小
const QRect RECT_XMLBrowser(4, 4, 478,
                            345);//RECT_XML_BROWSER_DIALOG的高和宽各减4*2为边缘距离


#endif // _RECT_DEFINE_H_
