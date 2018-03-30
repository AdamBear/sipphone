#ifndef SETTING_AUTOANSWERCONTROLLER_H_
#define SETTING_AUTOANSWERCONTROLLER_H_

#include <ylstring.h>

#include "commondefine.h"

#include "ETLLib.hpp"

struct AutoAnswerData;
class CAutoAnswerModel;

// Class CAutoAnswerController.
class CAutoAnswerController
{
public:
    // Get the singleton instance, it's a lazy singleton.
    static CAutoAnswerController * GetInstance();
    // Release the singleton instance.
    static void ReleaseInstance();

    // Get auto answer data.
    // | pAutoAnswerData | is the data retrieved.
    // Returns true if retrieved successfully, otherwise returns false.
    bool GetAutoAnswerDataById(AutoAnswerData * pAutoAnswerData, int nLineId);
    // Set auto answer data.
    // | refData | is the data to set.
    void SetAutoAnswerData(const AutoAnswerData & refData);

    // Get auto answer data by click action.
    // | pAutoAnswerData | is the data retrieved.
    // | strClickAction | is the click action.
    // Returns true if retrieved successfully, otherwise returns false.
    bool GetAutoAnswerStatusByLineId(int nLineId) const;
    // Set auto answer data by click action.
    // | strClickAction | is the click action.
    // | refData | is the data to set.
    void SetAutoAnswerStatusByLineId(int nLineId, bool isOn);

private:
    // Ctor and Dtor.
    CAutoAnswerController();
    ~CAutoAnswerController();

    // Initialize.
    // Returns true if initialized successfully, otherwise returns false.
    bool MyInit();
    // Finalize.
    // Returns true if finalized successfully, otherwise returns false.
    bool MyFinal();

private:
    static CAutoAnswerController * m_pAutoAnswerController; // The singleton instance.

    DISALLOW_COPY_AND_ASSIGN(CAutoAnswerController);
};

#define g_pAutoAnswerController (CAutoAnswerController::GetInstance())

#endif  // SETTING_AUTOANSWERCONTROLLER_H_
