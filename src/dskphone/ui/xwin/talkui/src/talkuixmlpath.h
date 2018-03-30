#ifndef _TALKUI_XML_PATH
#define _TALKUI_XML_PATH

namespace talkui
{
//通话UI中xml放置的路径
#define TALKUI_XML_PATH "./talkui/"

//dlgtalkuiframe
#define TALKUI_DLGTALKUIFRAME_XML_PATH TALKUI_XML_PATH "dlgtalkuiframe.xml"
//dlgtalkingui
#define TALKUI_DLGTALKINGUI_XML TALKUI_XML_PATH "dlgtalkuitalk.xml"
//dlgdialingui
#define TALKUI_DLGDIALINGUI_XML TALKUI_XML_PATH "dlgdialingui.xml"
//dlgringing
#define TALKUI_DLGRINGINGUI_XML TALKUI_XML_PATH "dlgringing.xml"
//dlgconnect
#define TALKUI_DLGCONNECTUI_XML TALKUI_XML_PATH "dlgconnect.xml"
//dlgfinish
#define TALKUI_DLGFINISHUI_XML TALKUI_XML_PATH "dlgfinishui.xml"
//dlgautoredial
#define TALKUI_DLGAUTOREDIAL_XML TALKUI_XML_PATH "dlgautoredial.xml"
//dlgconfui
#define TALKUI_DLGCONFUI_XML TALKUI_DLGTALKINGUI_XML
//dlgparknotify
#define TALKUI_DLGPARKNOFITY_XML TALKUI_XML_PATH "dlgparknotify.xml"
//dlgrtpstatus
#define TALKUI_DLGRTPSTATUSUI_XML TALKUI_XML_PATH "dlgrtpstatusui.xml"
//dlgacdinfoui
#define TALKUI_DLGACDINFOUI_XML TALKUI_XML_PATH "dlgacdcallinfoui.xml"
//dlgconfmanager
#define TALKUI_DLGCONFMANAGER_XML TALKUI_XML_PATH "dlgconfmanagerui.xml"
//dlgnetconf
#define TALKUI_DLGNETCONF_XML TALKUI_XML_PATH "dlgnetconfui.xml"


//dlgselectlineui和pickupmenu、scamenu是可以共用
//dlgselectlineui
#define TALKUI_DLGSELECTLINE_XML_PATH TALKUI_DLGTALKUIMENU_XML_PATH
/////pickupmenu  scamenu/////
#define TALKUI_DLGTALKUIMENU_XML_PATH TALKUI_XML_PATH "dlgtalkuimenu.xml"

#define TALKUI_NORMAL_ITEM_XML_PATH TALKUI_XML_PATH "talknormalitem.xml"
#define TALKUI_SEARCH_ITEM_XML_PATH TALKUI_XML_PATH "talksearchitem.xml"
#define TALKUI_CONF_MANAGER_ITEM_XML_PATH TALKUI_XML_PATH "talkconfmanageritem.xml"

}       //namespace talkui


#endif

