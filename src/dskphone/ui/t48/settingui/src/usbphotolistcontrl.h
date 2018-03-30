#ifndef USBPHOTOLLISTCONTRL_H
#define USBPHOTOLLISTCONTRL_H

#include "settingui/src/subpagelistdelegate.h"
#include "record/include/modrecord.h"
#include "settingui/src/usbaudioitem.h"
#include "uicommon/messagebox/messageboxcallback.h"
#include "usbphotoselector.h"
#include "setting/include/common.h"

enum USB_Photo_Dlg_Type
{
    Dlg_list,
    Dlg_detail,
};

class USBPhotoListContrl : public CMsgBoxCallBackAction
{
public:
    USBPhotoListContrl();
    ~USBPhotoListContrl();

    static USBPhotoListContrl * CreateInstance();
    void SetCurrentIndex(int iIndex);

    void DeleteCurrentIndex(bool bDeleteAll = false);

    void ReloadPhotoList();

    void OnChangePhoto(int iIndex);

    void SetAs(const yl::string & strWallPaperPath, const yl::string & strWallPaperName);

    int GetCurrentIndex();

    YLVector<RecordFileInfo>    m_vecRecordList;
    USB_PIC_SET_TYPE    m_eUSBPicSetType;
private:
    // 弹框回调函数
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

    bool FileIsExistInList(WallPaperListData & lstData, yl::string strName);
    bool FileIsExist(USB_PIC_SET_TYPE nSelec);
    void OnSetAs();
private:
    int             m_iCurrentIndex;
    yl::string      m_strWallPaperPath;
    yl::string      m_strWallPaperName;
    CUSBPhotoSelector * m_pUSBSelector;


#define g_pUSBPhotoContrl USBPhotoListContrl::CreateInstance()
};

#endif //USBPHOTOLLISTCONTRL_H
