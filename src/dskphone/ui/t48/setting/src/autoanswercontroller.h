#ifndef SETTING_AUTOANSWERCONTROLLER_H_
#define SETTING_AUTOANSWERCONTROLLER_H_

#include <ylstring.h>

#include "commondefine.h"

#include "ETLLib.hpp"

namespace NS_SETTING
{
class CAutoAnswerModel;

// Class CAutoAnswerController.
class CAutoAnswerController
{
public:
    // Get the singleton instance, it's a lazy singleton.
    static CAutoAnswerController * GetInstance();
    // Release the singleton instance.
    static void ReleaseInstance();

    // Get auto answer data by click action.
    // | pAutoAnswerData | is the data retrieved.
    // | strClickAction | is the click action.
    // Returns true if retrieved successfully, otherwise returns false.
    bool GetAutoAnswerStatusByLineId(int nLineId) const;
    // Set auto answer data by click action.
    // | strClickAction | is the click action.
    // | refData | is the data to set.
    void SetAutoAnswerStatusByLineId(int nLineId, bool isOn);

#ifdef IF_FEATURE_AUTO_ANSWER_TIMER
    int GetAutoAnswerTimer() const;
    void SetAutoAnswerTimer(int iTimer);
#endif

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

}  // namespace NS_SETTING

#define g_pAutoAnswerController (NS_SETTING::CAutoAnswerController::GetInstance())

#endif  // SETTING_AUTOANSWERCONTROLLER_H_
