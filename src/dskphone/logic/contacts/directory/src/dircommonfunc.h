#ifndef _DIRECTORY_DIR_COMMON_FUNC_HEDAER_
#define _DIRECTORY_DIR_COMMON_FUNC_HEDAER_

#include <ylvector.h>
#include "../include/directoryenumtypes.h"
#include "commondata.h"
#include <ylstring.h>
struct BaseDirDataRecord;

struct ContactGeneralItemData;
struct LocalDirContactRecord;

// 生成数据记录
BaseDirDataRecord * CreateDataEntry(DIR_DATA_ENTRY_TYPE type);


// Parse sub-string from input string.
// For example.
// Parse "112,123,134" by ",".
// Parse "-10-20-30" by "-".
bool ParseStringBySeparator(const yl::string & strInput, const char Separator,
                            YLList<yl::string> & OutPutList);

// Open bsft xml
// http://10.3.5.199/Bug.php?BugID=67314
// http://10.3.5.199/Bug.php?BugID=34025
bool OpenBSFTXML(const char * lpFile, xml_document & doc);

#if IF_FEATURE_METASWITCH_CALLLOG
//将月份转化为整形
int TransformMonth2Int(const char * pszMonth);

//转化时间，方便排序
yl::string TransformTime(char * strCallLog);
#endif

#endif
