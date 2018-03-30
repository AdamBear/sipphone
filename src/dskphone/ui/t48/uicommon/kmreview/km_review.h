#ifdef KM_REVIEW_SUPPORT
#ifndef __KM_REVIEW_H__
#define __KM_REVIEW_H__

///////////////////////////////////////////////////////////////////
// 给外部使用的接口
///////////////////////////////////////////////////////////////////

#include <QObject>
#include <sys/time.h>

class CKMManager : public QObject
{
    Q_OBJECT

public:
    CKMManager();
    ~CKMManager();

    // 记录按键信息
    void Record_Key(int nCode, bool bDown);

    // 记录鼠标信息
    void Record_Mouse(int x, int y, bool bDown);

    // 记录到log文件中
    void RecordString2Log(const char * lpszFmt);

    // 重新记录log文件
    void RestartRecord();
private:

    // 初始化数据
    void InitData();
    // 记录文件
    FILE    *   m_fpFile;
    struct timeval m_pLastTime; // 上次的记录的时间
    bool        bFirst;         // 是否第一次操作
};

//获取全局唯一的弹出框管理对象
#define  _KM_Manager GetKMManagerInstance()

CKMManager & GetKMManagerInstance();

#endif //__KM_REVIEW_H__
#endif
