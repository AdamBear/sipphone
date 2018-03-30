// This file is used to test Menu module.
// Author: Roland.
// Date: 2011-7-27
#if 0 // by kjf
#include <assert.h>

#include <fstream>
#include <iostream>

#include "menucontroller.h"

void TestMenuController()
{
    std::ofstream ofFile;
    ofFile.open("menu_controller_test.log");
    if (!ofFile.is_open())
    {
        return;
    }

    ofFile << "---------------Test menu controller begin---------------" << std::endl;

    assert(g_pMenuController);
    // Enter main menu.
    g_pMenuController->EnterMenu("OpenXml(menu_list.xml)");

    // Get main menu item list.
    const MenuItemList * plistMenuItem =
        g_pMenuController->GetMenuListByClickAction("OpenXml(menu_list.xml)");
    if (plistMenuItem == NULL)
    {
        return;
    }
    // Output main menu item list.
    ofFile << "---------------Main menu item list begin---------------" << std::endl;
    MenuItemListCIter citer = plistMenuItem->begin();
    for (; citer != plistMenuItem->end(); ++citer)
    {
        if (const MenuItemData * pMenuItemData = *citer)
        {
            ofFile << "ID NAME=\"" << pMenuItemData->m_strIdName << "\"" << std::endl
                   << "DISPLAY NAME=\"" << pMenuItemData->m_strDisplayName << "\"" << std::endl
                   << "ICON NAME=\"" << pMenuItemData->m_strIconName << "\"" << std::endl
                   << "CLICK ACTION=\"" << pMenuItemData->m_strClickAction << "\"" << std::endl
                   << "USER LEVEL=\"" << pMenuItemData->m_strUserLevel << "\"" << std::endl;

            ofFile << std::endl;
        }
    }
    ofFile << "---------------Main menu item list end---------------" << std::endl;
    ofFile << std::endl;

    // Get application menu item list.
    const MenuItemList * plistAppMenuItem =
        g_pMenuController->GetMenuListByClickAction("OpenXml(application_list.xml)");
    if (plistAppMenuItem == NULL)
    {
        return;
    }
    // Output application menu item list.
    ofFile << "---------------Application menu item list begin---------------" << std::endl;
    MenuItemListCIter citerApp = plistAppMenuItem->begin();
    for (; citerApp != plistAppMenuItem->end(); ++citerApp)
    {
        if (const MenuItemData * pMenuItemData = *citerApp)
        {
            ofFile << "ID NAME=\"" << pMenuItemData->m_strIdName << "\"" << std::endl
                   << "DISPLAY NAME=\"" << pMenuItemData->m_strDisplayName << "\"" << std::endl
                   << "ICON NAME=\"" << pMenuItemData->m_strIconName << "\"" << std::endl
                   << "CLICK ACTION=\"" << pMenuItemData->m_strClickAction << "\"" << std::endl
                   << "USER LEVEL=\"" << pMenuItemData->m_strUserLevel << "\"" << std::endl;

            ofFile << std::endl;
        }
    }
    ofFile << "---------------Application item list end---------------" << std::endl;
    ofFile << std::endl;

    // Get basic setting menu item list.
    const MenuItemList * plistBasicSettingMenuItem =
        g_pMenuController->GetMenuListByClickAction("OpenXml(basic_setting_list.xml)");
    if (plistBasicSettingMenuItem == NULL)
    {
        return;
    }
    // Output basic setting menu item list.
    ofFile << "---------------Setting menu item list begin---------------" << std::endl;
    MenuItemListCIter citerBasic = plistBasicSettingMenuItem->begin();
    for (; citerBasic != plistBasicSettingMenuItem->end(); ++citerBasic)
    {
        if (const MenuItemData * pMenuItemData = *citerBasic)
        {
            ofFile << "ID NAME=\"" << pMenuItemData->m_strIdName << "\"" << std::endl
                   << "DISPLAY NAME=\"" << pMenuItemData->m_strDisplayName << "\"" << std::endl
                   << "ICON NAME=\"" << pMenuItemData->m_strIconName << "\"" << std::endl
                   << "CLICK ACTION=\"" << pMenuItemData->m_strClickAction << "\"" << std::endl
                   << "USER LEVEL=\"" << pMenuItemData->m_strUserLevel << "\"" << std::endl;

            ofFile << std::endl;
        }
    }
    ofFile << "---------------Setting item list end---------------" << std::endl;
    ofFile << std::endl;

    // Get directory menu item list.
    const MenuItemList * plistDirectoryMenuItem =
        g_pMenuController->GetMenuListByClickAction("OpenXml(directory_list.xml)");
    if (plistDirectoryMenuItem == NULL)
    {
        return;
    }
    // Output directory menu item list.
    ofFile << "---------------Directory menu item list begin---------------" << std::endl;
    MenuItemListCIter citerDirectory = plistDirectoryMenuItem->begin();
    for (; citerDirectory != plistDirectoryMenuItem->end(); ++citerDirectory)
    {
        if (const MenuItemData * pMenuItemData = *citerDirectory)
        {
            ofFile << "ID NAME=\"" << pMenuItemData->m_strIdName << "\"" << std::endl
                   << "DISPLAY NAME=\"" << pMenuItemData->m_strDisplayName << "\"" << std::endl
                   << "ICON NAME=\"" << pMenuItemData->m_strIconName << "\"" << std::endl
                   << "CLICK ACTION=\"" << pMenuItemData->m_strClickAction << "\"" << std::endl
                   << "USER LEVEL=\"" << pMenuItemData->m_strUserLevel << "\"" << std::endl;

            ofFile << std::endl;
        }
    }
    ofFile << "---------------Directory menu item list begin---------------" << std::endl;
    ofFile << std::endl;

    // Get call feature menu item list.
    const MenuItemList * plistCallFeatureMenuItem =
        g_pMenuController->GetMenuListByClickAction("OpenXml(call_feature_list.xml)");
    if (plistCallFeatureMenuItem == NULL)
    {
        return;
    }
    // Output call feature menu item list.
    ofFile << "---------------Call feature menu item list begin---------------" << std::endl;
    MenuItemListCIter citerCallFeature = plistCallFeatureMenuItem->begin();
    for (; citerCallFeature != plistCallFeatureMenuItem->end(); ++citerCallFeature)
    {
        if (const MenuItemData * pMenuItemData = *citerCallFeature)
        {
            ofFile << "ID NAME=\"" << pMenuItemData->m_strIdName << "\"" << std::endl
                   << "DISPLAY NAME=\"" << pMenuItemData->m_strDisplayName << "\"" << std::endl
                   << "ICON NAME=\"" << pMenuItemData->m_strIconName << "\"" << std::endl
                   << "CLICK ACTION=\"" << pMenuItemData->m_strClickAction << "\"" << std::endl
                   << "USER LEVEL=\"" << pMenuItemData->m_strUserLevel << "\"" << std::endl;

            ofFile << std::endl;
        }
    }
    ofFile << "---------------Call feature menu item list end---------------" << std::endl;
    ofFile << std::endl;

    // Get resource menu item list.
    const MenuItemList * plistResourceMenuItem =
        g_pMenuController->GetMenuListByClickAction("OpenXml(resource_list.xml)");
    if (plistResourceMenuItem == NULL)
    {
        return;
    }
    // Output resource menu item list.
    ofFile << "---------------Resource menu item list begin---------------" << std::endl;
    MenuItemListCIter citerResource = plistResourceMenuItem->begin();
    for (; citerResource != plistResourceMenuItem->end(); ++citerResource)
    {
        if (const MenuItemData * pMenuItemData = *citerResource)
        {
            ofFile << "ID NAME=\"" << pMenuItemData->m_strIdName << "\"" << std::endl
                   << "DISPLAY NAME=\"" << pMenuItemData->m_strDisplayName << "\"" << std::endl
                   << "ICON NAME=\"" << pMenuItemData->m_strIconName << "\"" << std::endl
                   << "CLICK ACTION=\"" << pMenuItemData->m_strClickAction << "\"" << std::endl
                   << "USER LEVEL=\"" << pMenuItemData->m_strUserLevel << "\"" << std::endl;

            ofFile << std::endl;
        }
    }
    ofFile << "---------------Resource menu item list end---------------" << std::endl;
    ofFile << std::endl;

    ofFile << "---------------Test menu controller end---------------" << std::endl;

    ofFile.close();
};

void OutputLogToConsole()
{
    std::ifstream ifFile;
    ifFile.open("menu_controller_test.log");
    if (ifFile.is_open())
    {
        // Get log content length.
        ifFile.seekg(0, std::ifstream::end);
        size_t uSize = ifFile.tellg();
        ifFile.seekg(0, std::ifstream::beg);
        // Allocate buffer.
        char * pszBuffer = new char [uSize + 1];
        if (pszBuffer != NULL)
        {
            memset(pszBuffer, 0, uSize);
            // Copy content from log into buffer.
            ifFile.read(pszBuffer, uSize);
            pszBuffer[uSize] = '\0';

            // output.
            std::cout << pszBuffer << std::endl;

            delete [] pszBuffer;
            pszBuffer = NULL;
        }
    }
}

void Release()
{
    // Release menu controller instance.
    g_pMenuController->ReleaseInstance();
}

int main()
{
    // Test menu controller.
    TestMenuController();

    // Output log contents.
    OutputLogToConsole();

    // Release the singleton instance.
    Release();

    std::cin.get();

    return 0;
}
#endif
