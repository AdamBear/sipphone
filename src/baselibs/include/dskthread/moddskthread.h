#ifndef __MOD_DSK_THREAD_H__
#define __MOD_DSK_THREAD_H__
#include "dskthreaddefine.h"
#include "mkthread.h"

#if LINUX_SYSTEM
#include "xInc/xMsgSystem.h"
#else
#include "wInc/wMsgSystem.h"
#endif
// //////////////////////////////////////////////////////////////////////////
//线程相关

//////////////////////////////////////////////////////////////////////////

#ifdef  __cplusplus
extern "C" {
#endif

THREAD_ID chCreateThread(LPTHREAD_MAIN pMeth, LPCVOID lpData, int iPriority);

void chJoinThread(THREAD_ID idThreadID);

void chCancelThread(THREAD_ID idThreadID);

// for main thread
DSK_TARGET_T msgRegistMainThread(LPCSTR lpszThreadName, THREAD_MSG_PROC_EX pMsgCallbackEx);

void msgUnregistMainThread(DSK_TARGET_T hThread);

BOOL msgLaunchWorkThread(THREAD_MSG_PROC_EX lpMeth, LPCSTR lpszThreadName, LPCVOID lpData);

DSK_TARGET_T msgRegistWorkThread(LPCSTR lpszThreadName, THREAD_MSG_PROC_EX pMsgCallbackEx);

void msgUnregistWorkThread(DSK_TARGET_T hThread);

void msgUnlaunchWorkThread(LPCSTR lpszThreadName);

DSK_TARGET_T msgGetThreadByName(LPCSTR lpszThreadName);

LPVOID msgGetCurrentThreadObject();


// //////////////////////////////////////////////////////////////////////////
// 消息同步发送，返回值是recevier 的reply
LRESULT msgSendMsgToThread(DSK_TARGET_T hTargetThread, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT msgSendMsgToThreadEx(DSK_TARGET_T hTargetThread, UINT uMsg, WPARAM wParam, LPARAM lParam,
                             int nExtraSize, LPCVOID lpExtraData);
LRESULT msgSendMsgToThreadTimeoutEx(DSK_TARGET_T hTargetThread, UINT uMsg, WPARAM wParam, LPARAM lParam, int nExtraSize, LPCVOID lpExtraData, int nTimeout);

LRESULT msgSendMsgToThreadRaw(DSK_TARGET_T hTargetThread, UINT uMsg, WPARAM wParam, LPARAM lParam,
                              int nExtraSize, LPCVOID lpExtraData, int nTimeout, BOOL * pbSendSuccess);

// 消息同步发送，调用结果

// 发送notify给请求的线程
LRESULT msgSendNotifyToReqThread(DSK_NOTIFY_T notifyChain, UINT uMsg, WPARAM wParam,
                                 LPARAM lParam);
LRESULT msgSendNotifyToReqThreadEx(DSK_NOTIFY_T notifyChain, UINT uMsg, WPARAM wParam,
                                   LPARAM lParam, int nExtraSize, LPCVOID lpExtraData);
LRESULT msgPostNotifyToReqThread(DSK_NOTIFY_T notifyChain, UINT uMsg, WPARAM wParam,
                                 LPARAM lParam);
LRESULT msgPostNotifyToReqThreadEx(DSK_NOTIFY_T notifyChain, UINT uMsg, WPARAM wParam,
                                   LPARAM lParam, int nExtraSize, LPCVOID lpExtraData);

// add by dlw 201-03-24
// 可以通过扩展数据，获取到多方回复的数据
LRESULT msgCallMsgToThread(DSK_TARGET_T hTargetThread, UINT uMsg, WPARAM wParam, LPARAM lParam,
                           int & nExtraSize, LPVOID lpExtraData, int nTimeout);

#define msgPostMsgToThread(tg,msg,wp,lp) msgPostMsgToThread1(tg,msg,wp,lp,__FILE__,__LINE__)

BOOL msgPostMsgToThread1(DSK_TARGET_T hTargetThread, UINT uMsg, WPARAM wParam, LPARAM lParam,
                         const char * file, int line);
BOOL msgPostMsgToThreadEx(DSK_TARGET_T hTargetThread, UINT uMsg, WPARAM wParam, LPARAM lParam,
                          int nExtraSize, LPCVOID lpExtraData);

// 广播消息 post
void msgBroadpostThreadMsgEx(UINT uMsg, WPARAM wParam, LPARAM lParam, int nExtraSize,
                             LPCVOID lpExtraData);
void msgBroadpostThreadMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);




//定时器相关函数，使用宏定义，便于调试
/**
 * @brief Timer set thread timer 1.
 *
 * @author Song
 * @date 2018/1/9
 *
 * @param nTimerID  Identifier for the timer.
 * @param uTimerOut The timer out.
 * @param file      The file.
 * @param line      The line.
 *
 * @return An UINT.
 */
#define timerSetThreadTimer(timerid,timeout)  timerSetThreadTimer1(timerid,timeout,__FILE__,__LINE__)
UINT timerSetThreadTimer1(TIMER_ID nTimerID, UINT64 uTimerOut, const char * file, int line);


/**
 * @brief Timer kill thread timer 1.
 *
 * @author Song
 * @date 2018/1/9
 *
 * @param nTimerID Identifier for the timer.
 */
#define timerKillThreadTimer(timerid) timerKillThreadTimer1(timerid);
void timerKillThreadTimer1(TIMER_ID nTimerID);

/**
 * @brief 获取当前线程handle.
 *
 * @author Song
 * @date 2018/1/9
 *
 * @return A DSK_HANDLE_T.
 */

DSK_HANDLE_T mkGetCurrent();

/**
 * @brief 获取当前线程handle，如果当前未注册到msgkit，会创建一个.
 *
 * @author Song
 * @date 2018/1/9
 *
 * @return A DSK_HANDLE_T.
 */

DSK_HANDLE_T mkGetHandle();

/**
 * @brief 通过名称查找线程.
 *
 * @author Song
 * @date 2018/1/9
 *
 * @param cszName (Optional) Name of the csz.
 *
 * @return A DSK_TARGET_T.
 */

DSK_TARGET_T mkGetTarget(const char * cszName = NULL);

/**
 * @brief Mk find target.
 *
 * @author Song
 * @date 2018/1/9
 *
 * @param handle The handle.
 * @param pid    The PID.
 *
 * @return A DSK_TARGET_T.
 */

DSK_TARGET_T mkFindTarget(DSK_HANDLE_T handle, int pid);

/**
 * @brief 获取当前线程名称.
 *
 * @author Song
 * @date 2018/1/9
 *
 * @return Null if it fails, else a pointer to a const char.
 */

const char * mkGetCurrentName();

/**
 * @brief 打印当前定时器状态
 *
 * @author Song
 * @date 2018/1/17
 */

void mkDumpTimerInfo();

/**
* @brief 记录线程信息
*
* @author Song
* @date 2018/1/17
*
* @param cszName Name of the csz.
* @param iPid    (Optional) Zero-based index of the PID.
*/

void mkAddThreadInfo(const char * cszName, int iPid = -1);

/**
 * @brief  dump thread information.
 *
 * @author Song
 * @date 2018/2/7
 */

void dskthread_DumpThreadInfo(yl::string & strOutput);
#ifdef  __cplusplus
}
#endif
#endif //__MOD_DSK_THREAD_H__
