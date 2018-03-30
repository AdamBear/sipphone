#ifndef __BASEFINISHED_ROUTINE_H__
#define __BASEFINISHED_ROUTINE_H__

class CBaseFinishedRoutine : public CBaseRoutine
{
public:
    CBaseFinishedRoutine();
    virtual ~CBaseFinishedRoutine();

    // 状态
    virtual ROUTINE_STATE GetRoutineState() const
    {
        return ROUTINE_FINISHED;
    }

    // 名称
    virtual LPCSTR GetName() const
    {
        return "Finished";
    }
};

#endif //__BASEFINISHED_ROUTINE_H__
