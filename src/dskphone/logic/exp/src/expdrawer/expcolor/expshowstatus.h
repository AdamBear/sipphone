#ifndef _EXP_SHOW_STATUS_H
#define _EXP_SHOW_STATUS_H

#include "expdefine.h"

class CExpShowStatusContext;

class CExpShowStatusBase
{
public:
    CExpShowStatusBase(CExpShowStatusContext * pExpShowStatusContext);
    virtual ~CExpShowStatusBase();

    virtual void EnterStatusScreenSaver(int nExpIndex = EXP_MAX_COUNT) = 0;
    virtual void ExitStatusScreenSaver(int nExpIndex = EXP_MAX_COUNT) = 0;
    virtual void EnterStatusPowerSaving(int nExpIndex = EXP_MAX_COUNT) = 0;
    virtual void ExitStatusPowerSaving(int nExpIndex = EXP_MAX_COUNT) = 0;

    virtual bool IsInNormal()
    {
        return false;
    }
    virtual void StatusAction(int nExpIndex = EXP_MAX_COUNT) = 0;

protected:
    CExpShowStatusContext * m_pExpShowStatusContext;
};

class CExpShowStatusNormal : public CExpShowStatusBase
{
public:
    CExpShowStatusNormal(CExpShowStatusContext * pExpShowStatusContext);
    virtual ~CExpShowStatusNormal();

    virtual void EnterStatusScreenSaver(int nExpIndex = EXP_MAX_COUNT);
    virtual void ExitStatusScreenSaver(int nExpIndex = EXP_MAX_COUNT);
    virtual void EnterStatusPowerSaving(int nExpIndex = EXP_MAX_COUNT);
    virtual void ExitStatusPowerSaving(int nExpIndex = EXP_MAX_COUNT);

    virtual bool IsInNormal()
    {
        return true;
    }
    virtual void StatusAction(int nExpIndex = EXP_MAX_COUNT);
};

class CExpShowStatusScreenSaver : public CExpShowStatusBase
{
public:
    CExpShowStatusScreenSaver(CExpShowStatusContext * pExpShowStatusContext);
    virtual ~CExpShowStatusScreenSaver();

    virtual void EnterStatusScreenSaver(int nExpIndex = EXP_MAX_COUNT);
    virtual void ExitStatusScreenSaver(int nExpIndex = EXP_MAX_COUNT);
    virtual void EnterStatusPowerSaving(int nExpIndex = EXP_MAX_COUNT);
    virtual void ExitStatusPowerSaving(int nExpIndex = EXP_MAX_COUNT);

    virtual void StatusAction(int nExpIndex = EXP_MAX_COUNT);
};

class CExpShowStatusPowerSaving : public CExpShowStatusBase
{
public:
    CExpShowStatusPowerSaving(CExpShowStatusContext * pExpShowStatusContext);
    virtual ~CExpShowStatusPowerSaving();

    virtual void EnterStatusScreenSaver(int nExpIndex = EXP_MAX_COUNT);
    virtual void ExitStatusScreenSaver(int nExpIndex = EXP_MAX_COUNT);
    virtual void EnterStatusPowerSaving(int nExpIndex = EXP_MAX_COUNT);
    virtual void ExitStatusPowerSaving(int nExpIndex = EXP_MAX_COUNT);

    virtual void StatusAction(int nExpIndex = EXP_MAX_COUNT);
};

class CExpShowStatusScreenSaverAndPowerSaving : public CExpShowStatusBase
{
public:
    CExpShowStatusScreenSaverAndPowerSaving(CExpShowStatusContext * pExpShowStatusContext);
    virtual ~CExpShowStatusScreenSaverAndPowerSaving();

    virtual void EnterStatusScreenSaver(int nExpIndex = EXP_MAX_COUNT);
    virtual void ExitStatusScreenSaver(int nExpIndex = EXP_MAX_COUNT);
    virtual void EnterStatusPowerSaving(int nExpIndex = EXP_MAX_COUNT);
    virtual void ExitStatusPowerSaving(int nExpIndex = EXP_MAX_COUNT);

    virtual void StatusAction(int nExpIndex = EXP_MAX_COUNT);
};

class CExpShowStatusContext
{
public:
    CExpShowStatusContext();
    virtual ~CExpShowStatusContext();

public:
    virtual void EnterStatusScreenSaver(int nExpIndex = EXP_MAX_COUNT);
    virtual void ExitStatusScreenSaver(int nExpIndex = EXP_MAX_COUNT);
    virtual void EnterStatusPowerSaving(int nExpIndex = EXP_MAX_COUNT);
    virtual void ExitStatusPowerSaving(int nExpIndex = EXP_MAX_COUNT);

public:
    void SetExpShowStatusNormal()
    {
        m_pCurrentExpShowStatus = m_pExpShowStatusNormal;
    }
    void SetExpShowStatusScreenSaver()
    {
        m_pCurrentExpShowStatus = m_pExpShowStatusScreenSaver;
    }
    void SetExpShowStatusPowerSaving()
    {
        m_pCurrentExpShowStatus = m_pExpShowStatusPowerSaving;
    }
    void SetExpShowStatusScreenSaverAndPowerSaving()
    {
        m_pCurrentExpShowStatus = m_pExpShowStatusScreenSaverAndPowerSaving;
    }

    CExpShowStatusBase * GetExpShowStatus()
    {
        return m_pCurrentExpShowStatus;
    }
    bool IsInNormal();

protected:
    CExpShowStatusBase   *  m_pCurrentExpShowStatus;

    CExpShowStatusBase   *  m_pExpShowStatusNormal;
    CExpShowStatusBase   *  m_pExpShowStatusScreenSaver;
    CExpShowStatusBase   *  m_pExpShowStatusPowerSaving;
    CExpShowStatusBase   *  m_pExpShowStatusScreenSaverAndPowerSaving;
};

#endif //_EXP_SHOW_STATUS_H

