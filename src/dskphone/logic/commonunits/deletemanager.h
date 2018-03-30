#ifndef __DELETE_MANAGER_H__
#define __DELETE_MANAGER_H__

#if 0

class CBaseDeleteLater;
// deleteLater
class CDeleteManager
{
public:
    // 异步删除
    static void deleteLater(CBaseDeleteLater * pTarget);
    // 定时器处理
    static BOOL OnTimer(UINT uTimerID);

private:
    // 删除
    static void DeleteTraget();
};

#endif

#endif // __DELETE_MANAGER_H__
