// This file is used to test the setting module.
// Author: Roland.
// 2011-8-12

#if 0 // by kjf

#include <fstream>
#include <iostream>

#include "setting/include/common.h"

#include "autoanswercontroller.h"
#include "callforwardcontroller.h"
#include "callwaitingcontroller.h"
#include "statuscontroller.h"

namespace NS_SETTING
{

namespace NS_TEST
{
// Log file path.
static const char * const kszOutFile = "test_setting.log";
// Click action definition.
// Status click action.
static const char * const kszCAStatus = "OpenMenu(status_list.xml)";
// Call forward click cation.
static const char * const kszCACallForward = "OpenPage(call_forward_list.xml)";
// Call waiting click action.
static const char * const kszCACallWaiting = "OpenPage(call_waiting_list.xml)";
// Auto answer click action.
static const char * const kszCAAutoAnswer = "OpenPage(auto_answer_list.xml)";

// BOOL to status(ON/OFF)
yl::string Bool2Status(bool isOn)
{
    return isOn ? "ON" : "OFF";
}

// Test menu item.
bool TestMenuItem(const yl::string & strClickAction, std::ofstream * pOf)
{
    if (strClickAction.empty() || pOf == NULL)
    {
        return false;
    }

    std::ofstream & refStream = *pOf;

    // Get title.
    const yl::string strTitle = g_pStatusController->GetTitleByClickAction(strClickAction);
    if (strTitle.empty())
    {
        refStream << "[ERROR]\tFailed to get page title.";
    }
    else
    {
        refStream << "[OK]\tPate title is " << strTitle << std::endl;
    }

    MenuItemListData stMID;
    if (!g_pStatusController->GetPageMenuListByClickAction(&stMID, strClickAction))
    {
        refStream << "[ERROR]\tFailed to get page item list."
                  << " Please check the config path." << std::endl;
        return false;
    }

    if (stMID.m_strClickAction != strClickAction || stMID.m_pMenuItemList == NULL)
    {
        refStream << "[ERROR]\tAction not equal! OR list is NULL." << std::endl;
        return false;
    }

    if (stMID.m_pMenuItemList->size() <= 0)
    {
        refStream << "[ERROR]\tNo page item found!" << std::endl;
    }
    else
    {
        refStream << "[OK]\tpage item found!" << std::endl;
    }

    // Iterate the output the menu item.
    int nCount = 0;
    MenuItemListCIter citer = stMID.m_pMenuItemList->begin();
    for (; citer != stMID.m_pMenuItemList->end(); ++citer)
    {
        ++nCount;
        if (*citer != NULL)
        {
            refStream << "Item " << nCount << std::endl;
            refStream << "\tMenu Item Id name: " << (*citer)->m_strIdName << std::endl;
            refStream << "\tMenu Item display name: " << (*citer)->m_strDisplayName << std::endl;
            refStream << "\tMenu Item icon name: " << (*citer)->m_strIconName << std::endl;
            refStream << "\tMenu Item click action: " << (*citer)->m_strClickAction << std::endl;
            refStream << "\tMenu Item long click action: " << (*citer)->m_strLongClickAction << std::endl;
            refStream << "\tMenu Item user level: " << (*citer)->m_strUserLevel << std::endl;
        }
        else
        {
            refStream << "[WARNING]\t Empty item!" << std::endl;
        }
    }

    return true;
}

// Test status.
bool TestStatus()
{
    // Get page menu list.
    if (!g_pStatusController)
    {
        return false;
    }

    std::ofstream ofFile;
    ofFile.open(kszOutFile, std::ifstream::trunc);
    if (!ofFile.is_open())
    {
        return false;
    }

    // Begin to log.
    ofFile << "------------------- Test Status begin -------------------" << std::endl;

    // Test page menu item.
    TestMenuItem(kszCAStatus, &ofFile);
    // Get phone status.
    PhoneStatusData stPSD;
    if (g_pStatusController->GetPhoneStatusData(&stPSD))
    {
        ofFile << "[OK]\tPhone status found!" << std::endl;
        ofFile << "\tPhone model is " << stPSD.m_strModel << std::endl
               << "\tPhone hardware is " << stPSD.m_strHardWare << std::endl
               << "\tPhone firmware is " << stPSD.m_strFirmWare << std::endl
               << "\tPhone product ID is" << stPSD.m_strProductId << std::endl
               << "\tPhone MAC is " << stPSD.m_strMac << std::endl;
    }
    else
    {
        ofFile << "[ERROR]\t Failed to get phone status!" << std::endl;
    }

    // Get phone network status.
    PhoneNetworkStatusData stPNSD;
    if (g_pStatusController->GetPhoneNetworkStatus(&stPNSD))
    {
        ofFile << "[OK]\tPhone network status found!" << std::endl;
        ofFile << "\tNetwork WAN type is " << stPNSD.m_eWanType << std::endl
               << "\tPhone WAN IP is " << stPNSD.m_strWanIp << std::endl
               << "\tPhone WAN mask is " << stPNSD.m_strMask << std::endl
               << "\tPhone gateway is" << stPNSD.m_strGateway << std::endl
               << "\tPhone primary DNS is " << stPNSD.m_strDnsPrimary << std::endl
               << "\tPhone secondary DNS is " << stPNSD.m_strDnsSecond << std::endl;
    }
    else
    {
        ofFile << "[ERROR]\t Failed to get phone network status!" << std::endl;
    }

    // Get phone account status.

    // End log.
    ofFile << std::endl;
    ofFile.close();

    return true;
}

bool TestCallForward()
{
    // Get page menu list.
    if (!g_pCallForwardController)
    {
        return false;
    }

    std::ofstream ofFile;
    ofFile.open(kszOutFile, std::ifstream::app);
    if (!ofFile.is_open())
    {
        return false;
    }

    // Begin to log.
    ofFile << "------------------- Test Call Forward begin -------------------" << std::endl;

    // Test page menu item.
    TestMenuItem(kszCACallForward, &ofFile);

    // Get forward data.
    ForwardData stFD;
    if (g_pCallForwardController->GetForwardDataByType(&stFD, FORWARD_TYPE_ALWAYS))
    {
        ofFile << "[OK]\tForward always data found!" << std::endl;
        ofFile << "\tForward status is " << (stFD.m_isOn ? "ON" : "OFF") << std::endl
               << "\tForward target is " << stFD.m_strTarget << std::endl
               << "\tForward on code is " << stFD.m_strOnCode << std::endl
               << "\tForward off code is" << stFD.m_strOffCode << std::endl;
    }
    else
    {
        ofFile << "[ERROR]\t Failed to get forward data!" << std::endl;
    }

    // Set forward data.
    ForwardData stFDToSet;
    stFDToSet.m_isOn = true;
    stFDToSet.m_strTarget = "111";
    stFDToSet.m_strOnCode = "#77";
    stFDToSet.m_strOffCode = "#88";
    stFDToSet.m_nRingTimes = 10;
    g_pCallForwardController->SetForwardDataByType(FORWARD_TYPE_NOANSWER, stFDToSet);

    // Check.
    ForwardData stFDCur;
    g_pCallForwardController->GetForwardDataByType(&stFDCur, FORWARD_TYPE_NOANSWER);
    if (stFDCur == stFDToSet)
    {
        ofFile << "[OK]\tSet forward no-answer succeed!" << std::endl;
    }
    else
    {
        ofFile << "[ERROR]\tFail to set forward no-answer!" << std::endl;
    }

    // Get phone account status.

    // End log.
    ofFile << std::endl;
    ofFile.close();

    return true;
}

// Test call waiting.
bool TestCallWaiting()
{
    // Get page menu list.
    if (!g_pCallWaitingController)
    {
        return false;
    }

    std::ofstream ofFile;
    ofFile.open(kszOutFile, std::ifstream::app);
    if (!ofFile.is_open())
    {
        return false;
    }

    // Begin to log.
    ofFile << "------------------- Test Call Waiting begin -------------------" << std::endl;

    // Test page menu item.
    TestMenuItem(kszCAAutoAnswer, &ofFile);

    // Get forward data.
    CallWaitingData stCWD;
    if (g_pCallWaitingController->GetCallWaitingData(&stCWD))
    {
        ofFile << "[OK]\tCall waiting data found!" << std::endl;
        ofFile << "\tCall waiting status is " << Bool2Status(stCWD.m_isOn) << std::endl
               << "\tCall waiting play tone is " << Bool2Status(stCWD.m_isPlayTone) << std::endl;
    }
    else
    {
        ofFile << "[ERROR]\t Failed to get call waiting data!" << std::endl;
    }

    // Set forward data.
    CallWaitingData stCWDToSet;
    stCWDToSet.m_isOn = true;
    stCWDToSet.m_isPlayTone = false;
    g_pCallWaitingController->SetCallWaitingData(stCWDToSet);

    // Check.
    CallWaitingData stCWDCur;
    g_pCallWaitingController->GetCallWaitingData(&stCWDCur);
    if (stCWDCur == stCWDToSet)
    {
        ofFile << "[OK]\tSet call waiting data succeed!" << std::endl;
    }
    else
    {
        ofFile << "[ERROR]\tFail to set call waiting data!" << std::endl;
    }

    // Get phone account status.

    // End log.
    ofFile << std::endl;
    ofFile.close();

    return true;
}

// Test auto answer.
bool TestAutoAnswer()
{
    // Get page menu list.
    if (!g_pCallWaitingController)
    {
        return false;
    }

    std::ofstream ofFile;
    ofFile.open(kszOutFile, std::ifstream::app);
    if (!ofFile.is_open())
    {
        return false;
    }

    // Begin to log.
    ofFile << "------------------- Test Call Waiting begin -------------------" << std::endl;

    // Test page menu item.
    TestMenuItem(kszCAAutoAnswer, &ofFile);

    // Get forward data.
    AutoAnswerData stAAD;
    if (g_pAutoAnswerController->GetAutoAnswerData(&stAAD))
    {
        ofFile << "[OK]\tAuto answer data found!" << std::endl;
        for (int i = 0; i < AutoAnswerData::knMaxAntoAnswerCount; ++i)
        {
            ofFile << "\tLine " << i << " auto answer status is " << Bool2Status(stAAD.m_isOn[i]) << std::endl;
        }
    }
    else
    {
        ofFile << "[ERROR]\t Failed to get answer data!" << std::endl;
    }

    // Set forward data.
    AutoAnswerData stAADToSet;
    for (int i = 0 ; i < AutoAnswerData::knMaxAntoAnswerCount; ++i)
    {
        stAADToSet.m_isOn[i] = i % 2 ? true : false;
    }
    g_pAutoAnswerController->SetAutoAnswerData(stAADToSet);

    // Check.
    AutoAnswerData stAADCur;
    g_pAutoAnswerController->GetAutoAnswerData(&stAADCur);
    if (stAADCur == stAADToSet)
    {
        ofFile << "[OK]\tSet auto answer data succeed!" << std::endl;
    }
    else
    {
        ofFile << "[ERROR]\tFail to set auto answer data!" << std::endl;
    }

    // End log.
    ofFile << std::endl;
    ofFile.close();

    return true;
}

void ReleaseSingleton()
{
    // Release singleton instances.
    CStatusController::ReleaseInstance();
    CCallForwardController::ReleaseInstance();
    CCallWaitingController::ReleaseInstance();
    CAutoAnswerController::ReleaseInstance();
}

// Output log content to console.
void OutputLogFileToConsole()
{
    std::ifstream ifFile;
    ifFile.open(kszOutFile);
    if (!ifFile.is_open())
    {
        return;
    }

    // Get file content length.
    ifFile.seekg(0, std::ifstream::end);
    size_t uLen = ifFile.tellg();
    ifFile.seekg(0, std::ifstream::beg);
    if (uLen > 0)
    {
        // Read content from file.
        char * pszBuffer = new char [uLen + 1];
        if (pszBuffer != NULL)
        {
            memset(pszBuffer, 0, uLen + 1);
            ifFile.read(pszBuffer, (std::streamsize)uLen);
            pszBuffer[uLen] = '\0';

            // Output content to console.
            std::cout << pszBuffer << std::endl;

            // Free memory.
            delete [] pszBuffer;
            pszBuffer = NULL;
        }
    }

    ifFile.close();
}

}  // namespace NS_TEST

}  // namespace NS_SETTING

int main()
{
    namespace NS_CUR = NS_SETTING::NS_TEST;
    // Test status.
    NS_CUR::TestStatus();
    // Test call forward.
    NS_CUR::TestCallForward();
    // Test call waiting.
    NS_CUR::TestCallWaiting();
    // Test auto answer.
    NS_CUR::TestAutoAnswer();

    // Output log to console.
    NS_CUR::OutputLogFileToConsole();

    // Release.
    NS_CUR::ReleaseSingleton();

    std::cin.get();

    return 0;
}

#endif
