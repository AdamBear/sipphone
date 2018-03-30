#ifndef _DLG_USB_PHOTO_H_
#define _DLG_USB_PHOTO_H_

#include "settinguilogic/include/settingdefine.h"
#if defined(IF_USB_SUPPORT) && IF_SUPPORT_USB_PHOTO
#include "dlgsettingui.h"
#include "settinguiwizardtitle.h"
#include "record/include/common.h"
#include "settinguilogic/include/common.h"


class xImageView;
class CDlgUsbPhoto : public CDlgSettingUI
{
public:
    static CBaseDialog* CreateInstance();

public:
    CDlgUsbPhoto();
    virtual ~CDlgUsbPhoto();
    static LRESULT OnRecordMsg(msgObject& msg);

public:
    virtual void Init();
    virtual void loadChildrenElements(xml_node& node);
    virtual void RefreshUI(bool bReLoadData = false);
    virtual bool onKeyPressedEvent(int key);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress);
    // 打开界面附加操作
    virtual void InitExtraData(void* pData);
    virtual void MessageBoxCallBack(CMessageBoxBase* pMessageBox);

public:
    // 显示选项
    void ShowOption();

protected:
    void LoadPhotoInfo();
    bool FileIsExist(int nAction);
    bool FileIsExistInList(LIST_WALLPAPER_DATA& lstData, const yl::string& strName);
    void OnSetAction(int nAction);
    void SetCommonSoftkey();
    void OnPreviousImage();
    void OnNextImage();
    void OnSwitchImage(bool bNext);

protected:
    xImageView*                 m_pImgPhoto;
    int                         m_nPhotoIndex;
    RecordFileInfo              m_stPhotoInfo;
};
#endif

#endif //_DLG_USB_PHOTO_H_
