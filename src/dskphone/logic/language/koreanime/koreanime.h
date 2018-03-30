#ifndef __KOREAN_IME_H__
#define __KOREAN_IME_H__

/***************************韩语输入法对外接口******************************/
// 按键定义
#define     KEY_CODE_SPACE          -1      // Spacing
#define     KEY_CODE_ENTER          -2      // Writing go down
#define     KEY_CODE_BACKSPACE      -3      // Remove character

extern yl::wstring g_strIngWord;
extern yl::wstring g_strCompleteWord;

/************************************************************************/
/* 接口  ： korean_Start()                                              */
/* 功能  ： 开始一次韩语输入过程，可连续多次按键                        */
/* 参数说明 ：                                                          */
/*     无                                                               */
/* 返回值 ：无                                                          */
/************************************************************************/
void korean_Start();

/************************************************************************/
/* 接口  ： korean_KeyIn()                                              */
/* 功能  ： 输入某个按键后返回当前输入的韩文字符                        */
/* 参数说明 ：                                                          */
/*     iKey ： 输入按键的键值                                           */
/* 返回值 ：                                                            */
/*      输入一个或连续几个按键后得到的韩文字符                          */
/* 备注  ： 必须在调用korean_Start后才能调用该接口                      */
/************************************************************************/
void korean_KeyIn(char chKey);


#endif // __KOREAN_IME_H__
