#ifndef DIRECTORY_XPORTMANAGER_H_
#define DIRECTORY_XPORTMANAGER_H_
#include <ylstring.h>
#include <xmlparser.hpp>
#include "directoryenumtypes.h"
#include "common/common_data_define.h"
struct ContactGeneralItemData;

// Class CImportBase, 导入基类.
class CXPortBase
{
public:
    // 构造函数和析构函数.
    CXPortBase();
    virtual ~CXPortBase();

    // 导入联系人文件, 根据参数决定是否验证文件可导性.
    // | szPath | 是要导入的文件路径.
    // | isAppendContact | 为true则追加联系人, 为false则清空联系人后重新加入.
    // | isAppendGroup | 为true则追加组, 为false则清空组后重新加入.
    // | isToCheck | 为true则在导入之前要验证联系人文件是否可导, 否则
    // 不验证文件.
    // 如果导入成功则返回IMPORT_NO_ERROR, 否则返回具体的错误码.
    virtual int Import(const char * szPath, bool isAppend);

    // 导入联系人文件.
    // | szPath | 是要导入的文件路径.
    // 如果导入成功则返回IMPORT_NO_ERROR, 否则返回具体的错误码.
    virtual int Import(const char * szPath) = 0;

    // 导出联系人文件.
    // 如果导出成功则返回IMPORT_NO_ERROR, 否则返回具体的错误码.
    int Export();

    // 导入之前所做的操作.
    // | isAppendContact | 为true则追加联系人, 为false则清空联系人后重新加入.
    // | isAppendGroup | 为true则追加组, 为false则清空组后重新加入.
    // 如果操作成功则返回true, 否则返回false.
    virtual bool BeforeImport(bool isAppend);
    // 导入之后需要做的操作.
    // 如果操作成功则返回true, 否则返回false.
    bool AfterImport(bool bNeedUpload = true);
};

// class CXPortT3x, T3X导入导出类.
class CXPortT3X : public CXPortBase
{
public:
    CXPortT3X();
    virtual ~CXPortT3X();

    // Reimp. T3X联系人文件的格式为:
    // <?xml version="1.0" encoding="UTF-8"?>
    // <contactData>
    //     <group>
    //         <contact sDisplayName="ooo" sOfficeNumber="1234343242" sMobilNumber="42342342342" sOtherNumber="" sLine="0" sRing="Auto" group="" photoDefault="" photoSelect="0" />
    //         <contact sDisplayName="qt" sOfficeNumber="10002" sMobilNumber="230001" sOtherNumber="" sLine="0" sRing="Auto" group="" photoDefault="" photoSelect="0" />
    //     </group>
    //     <blacklist>
    //         <contact sDisplayName="hlll" sOfficeNumber="23434234" sMobilNumber="423423423" sOtherNumber="" sLine="0" sRing="Auto" group="" photoDefault="" photoSelect="0" />
    //         <contact sDisplayName="jkl" sOfficeNumber="12423423423" sMobilNumber="" sOtherNumber="" sLine="0" sRing="Auto" group="" photoDefault="" photoSelect="0" />
    //     </blacklist>
    //
    //     <groupinfo>
    //         <group name="1" Ring="Auto" />
    //         <group name="bar" Ring="Resource:Ring8.wav" />
    //     </groupinfo>
    // </contactData>
    virtual int Import(const char * szPath);

    int PraseContact(xml_node node, ContactGeneralItemData & stID, int nGroupId = knInvalidRId);

protected:
    // 导入组.
    // | pRoot | 是根节点.
    // 如果可以导则返回IMPORT_NO_ERROR, 否则返回其他错误码.
    int MyImportGroup(xml_node Root);

    // 导入成员.
    // | pRoot | 是根节点.
    // 如果可以导则返回IMPORT_NO_ERROR, 否则返回其他错误码.
    int MyImportContact(xml_node Root);
};

// class CXPortV40, V40导入导出类.
class CXPortV40 : public CXPortBase
{
public:
    CXPortV40();
    virtual ~CXPortV40();

    // Reimp.
    // V40的联系人文件格式如下:
    // <xml version="1.0" encoding="UTF-8"?>
    // <contactData>
    //     <group sGroupName="Black List" sGroupRing="/phone/resource/wav/ring.wav" bCustomGroup="0">
    //         <contact sLocalRename="qt" sSIPName="" sDisplayName="" sServerName="" sOfficeNumber="12343" sMobilNumber="242343" sPrivateNumber="" sOtherNumber="" sRingFile="" sImagePath="/phone/resource/sysdata/contact-image/icon_blacklist_b.png" sE_mail=""/>
    //         <contact sLocalRename="qt2" sSIPName="" sDisplayName="" sServerName="" sOfficeNumber="1434" sMobilNumber="4234324" sPrivateNumber="" sOtherNumber="" sRingFile="" sImagePath="/phone/resource/sysdata/contact-image/icon_friend_b.png" sE_mail=""/>
    //     </group>
    //     <group sGroupName="Anonymous" sGroupRing="/phone/resource/wav/ring.wav" bCustomGroup="0">
    //         <contact sLocalRename="rxw" sSIPName="" sDisplayName="" sServerName="" sOfficeNumber="1234546" sMobilNumber="4234324" sPrivateNumber="432432" sOtherNumber="" sRingFile="" sImagePath="/phone/resource/sysdata/contact-image/icon_blacklist_b.png" sE_mail=""/>
    //         <contact sLocalRename="rxw1" sSIPName="" sDisplayName="" sServerName="" sOfficeNumber="131002" sMobilNumber="4243" sPrivateNumber="" sOtherNumber="" sRingFile="" sImagePath="/phone/resource/contact-image/icon_contact.png" sE_mail=""/>
    //     </group>
    virtual int Import(const char * szPath);

protected:
    // 导入成员节点.
    // | pGRoot | 是成员所在的父节点, 即组节点.
    // | nGroupId | 是组id.
    // 如果导入成功则返回IMPORT_NO_ERROR, 否则返回其他错误码.
    int MyImportContact(xml_node GRoot, int nGroupId, const yl::string & strGroupName);
};

class CXPortBlackList : public CXPortBase
{
public:
    CXPortBlackList();
    virtual ~CXPortBlackList();

    virtual int Import(const char * szPath);

protected:
    int MyImportContact(xml_node CRoot);

};

// class CXPortV50, V50导入导出类.
class CXPortV50 : public CXPortBase
{
public:
    CXPortV50();
    virtual ~CXPortV50();

    // Reimp.
    virtual int Import(const char * szPath);

protected:
    // 导入组.
    // | pGRoot | 是组的根节点.
    // 如果可以导则返回IMPORT_NO_ERROR, 否则返回其他错误码.
    int MyImportGroup(xml_node GRoot);

    // 导入成员.
    // | pCRoot | 是成员的根节点.
    // 如果可以导则返回IMPORT_NO_ERROR, 否则返回其他错误码.
    int MyImportContact(xml_node CRoot);
};

// namespace XPortFactory.
namespace XPortFactory
{
// 导入指定路径的文件.
// | szPath | 是要导入的文件路径.
// | isAppendContact | 为true则追加联系人, 为false则清空联系人后重新加入.
// | isAppendGroup | 为true则追加组, 为false则清空组后重新加入.
// | isToCheck | 为true则在导入之前要验证联系人文件是否可导, 否则不验证.
// 如果导入成功则返回IMPORT_NO_ERROR, 否则返回具体错误码.
int ImportByPath(const char * szPath, bool isAppend);

// 导出联系人组和成员到指定文件.
// 如果导出成功则返回IMPORT_NO_ERROR, 否则返回具体的错误码.
int ExportToPath();

//解析xml文件为那种机型的xml文件
int ParseContactFileType(const char * szPath);

};  // namespace XPortFactory.


#endif  // DIRECTORY_XPORTMANAGER_H_
