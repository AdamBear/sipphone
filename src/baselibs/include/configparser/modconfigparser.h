#ifndef __MODULE_CONFIG_PARSER_H__
#define __MODULE_CONFIG_PARSER_H__

#include <ylstl/ylstring.h>
#include <ylstl/yllist.h>
#include <ETLLib/ETLLib.hpp>

struct Config_Item
{
    /** @brief  配置名称. */
    yl::string      m_strKey;

    /** @brief  配置数据类型. */
    int             m_iType;

    /** @brief  配置属性. */
    int             m_iAttr;

    /** @brief  配置数据. */
    yl::string      m_strValue;
};

/**********************************************************************************************//**
 * @fn  void configParser_Init();
 *
 * @brief   Configuration parser init.
 **************************************************************************************************/

void configParser_Init();

/**********************************************************************************************//**
 * @fn  void configParser_Uninit();
 *
 * @brief   Configuration parser uninit.
 **************************************************************************************************/

void configParser_Uninit();

/**********************************************************************************************//**
 * @fn  float configParser_GetConfigFloat(const char* lpszConfigName, CONFIG_LEVEL_TYPE eType = CONFIG_LEVEL_IGNORE);
 *
 * @brief   Configuration parser get configuration float.
 *
 * @param   lpszConfigName  Name of the configuration.
 * @param   eType           (Optional) The type.
 *
 * @return  A float.
 **************************************************************************************************/

float configParser_GetConfigFloat(const char * lpszConfigName,
                                  CONFIG_LEVEL_TYPE eType = CONFIG_LEVEL_IGNORE);

/**********************************************************************************************//**
 * @fn  bool configParser_SetConfigFloat(const char* lpszConfigName, float fValue, CONFIG_LEVEL_TYPE eType = CONFIG_LEVEL_IGNORE);
 *
 * @brief   Configuration parser set configuration float.
 *
 * @param   lpszConfigName  Name of the configuration.
 * @param   fValue          The value.
 * @param   eType           (Optional) The type.
 *
 * @return  True if it succeeds, false if it fails.
 **************************************************************************************************/

bool configParser_SetConfigFloat(const char * lpszConfigName, float fValue,
                                 CONFIG_LEVEL_TYPE eType = CONFIG_LEVEL_IGNORE);

/**********************************************************************************************//**
 * @fn  yl::string configParser_GetCfgItemStringValue(const char* lpszConfigName, int itemIndex, CONFIG_LEVEL_TYPE eType = CONFIG_LEVEL_IGNORE);
 *
 * @brief   Configuration parser get configuration item string value.
 *
 * @param   lpszConfigName  Name of the configuration.
 * @param   itemIndex       Zero-based index of the item.
 * @param   eType           (Optional) The type.
 *
 * @return  A yl::string.
 **************************************************************************************************/
yl::string configParser_GetConfigString(const char * lpszConfigName,
                                        CONFIG_LEVEL_TYPE eType = CONFIG_LEVEL_IGNORE);

yl::string configParser_GetCfgItemStringValue(const char * lpszConfigName, int itemIndex,
        CONFIG_LEVEL_TYPE eType = CONFIG_LEVEL_IGNORE);

yl::string configParser_GetCfgItemStringValue(const char * lpszConfigName, int secIndex,
        int itemIndex, CONFIG_LEVEL_TYPE eType = CONFIG_LEVEL_IGNORE);

/**********************************************************************************************//**
 * @fn  int configParser_GetCfgItemIntValue(const char* lpszConfigName, int itemIndex, CONFIG_LEVEL_TYPE eType = CONFIG_LEVEL_IGNORE);
 *
 * @brief   Configuration parser get configuration item int value.
 *
 * @param   lpszConfigName  Name of the configuration.
 * @param   itemIndex       Zero-based index of the item.
 * @param   eType           (Optional) The type.
 *
 * @return  An int.
 **************************************************************************************************/
int configParser_GetConfigInt(const char * lpszConfigName,
                              CONFIG_LEVEL_TYPE eType = CONFIG_LEVEL_IGNORE);

int configParser_GetCfgItemIntValue(const char * lpszConfigName, int itemIndex,
                                    CONFIG_LEVEL_TYPE eType = CONFIG_LEVEL_IGNORE);

int configParser_GetCfgItemIntValue(const char * lpszConfigName, int secIndex, int itemIndex,
                                    CONFIG_LEVEL_TYPE eType = CONFIG_LEVEL_IGNORE);

/**********************************************************************************************//**
 * @fn  bool configParser_SetCfgItemStringValue(const char* lpszConfigName, int index, const yl::string &strValue, CONFIG_LEVEL_TYPE eType = CONFIG_LEVEL_IGNORE);
 *
 * @brief   Configuration parser set configuration item string value.
 *
 * @param   lpszConfigName  Name of the configuration.
 * @param   index           Zero-based index of the.
 * @param   strValue        The value.
 * @param   eType           (Optional) The type.
 *
 * @return  True if it succeeds, false if it fails.
 **************************************************************************************************/
bool configParser_SetConfigString(const char * lpszConfigName, const yl::string & strValue,
                                  CONFIG_LEVEL_TYPE eType = CONFIG_LEVEL_IGNORE);

bool configParser_SetCfgItemStringValue(const char * lpszConfigName, int index,
                                        const yl::string & strValue, CONFIG_LEVEL_TYPE eType = CONFIG_LEVEL_IGNORE);

bool configParser_SetCfgItemStringValue(const char * lpszConfigName, int secIndex, int itemIndex,
                                        const yl::string & strValue, CONFIG_LEVEL_TYPE eType = CONFIG_LEVEL_IGNORE);

/**********************************************************************************************//**
 * @fn  bool configParser_SetCfgItemIntValue(const char* lpszConfigName, int index, int value, CONFIG_LEVEL_TYPE eType = CONFIG_LEVEL_IGNORE);
 *
 * @brief   Configuration parser set configuration item int value.
 *
 * @param   lpszConfigName  Name of the configuration.
 * @param   index           Zero-based index of the.
 * @param   value           The value.
 * @param   eType           (Optional) The type.
 *
 * @return  True if it succeeds, false if it fails.
 **************************************************************************************************/
bool configParser_SetConfigInt(const char * lpszConfigName, int nValue,
                               CONFIG_LEVEL_TYPE eType = CONFIG_LEVEL_IGNORE);

bool configParser_SetCfgItemIntValue(const char * lpszConfigName, int index, int nValue,
                                     CONFIG_LEVEL_TYPE eType = CONFIG_LEVEL_IGNORE);

bool configParser_SetCfgItemIntValue(const char * lpszConfigName, int secIndex, int itemIndex,
                                     int nValue, CONFIG_LEVEL_TYPE eType = CONFIG_LEVEL_IGNORE);

/**********************************************************************************************//**
 * @fn  bool configParser_CleanPassword(const char* lpszPwdPath, const char* lpszOriPath);
 *
 * @brief   将lpszPwdPath路径下的密码（对应原始目录路径lpszOriPath的位置）进行清空
 *           如/tmp/system/system.ini::Autop:Password清空，其对应 /config/system/system.ini::Autop:Password
 *            则调用:configParser_CleanPassword("/tmp", "/config")..
 *           注：配置设置为非注册表方式!!，针对导出配置时清空密码
 *
 * @param   lpszPwdPath 需要清空密码的路径
 * @param   lpszOriPath 密码位置的原始路径
 *
 * @return  True if it succeeds, false if it fails.
 **************************************************************************************************/

bool configParser_CleanPassword(const char * lpszPwdPath, const char * lpszOriPath);

/**********************************************************************************************//**
 * @fn  int configParser_GetDefaultValueInt(const char* lpszConfigName);
 *
 * @brief   Configuration parser get default value int.
 *
 * @param   lpszConfigName  Name of the configuration.
 *
 * @return  An int.
 **************************************************************************************************/

int configParser_GetDefaultValueInt(const char * lpszConfigName);

/**********************************************************************************************//**
 * @fn  yl::string configParser_GetDefaultValueString(const char* lpszConfigName);
 *
 * @brief   Configuration parser get default value string.
 *
 * @param   lpszConfigName  Name of the configuration.
 *
 * @return  A yl::string.
 **************************************************************************************************/

yl::string configParser_GetDefaultValueString(const char * lpszConfigName);

bool configParse_FindCfgItem(const char * lpszWildcard, YLList<Config_Item> & listCfgItem);

yl::string configParser_GetCfgAttribute(const char * lpszConfigName,
                                        const char * lpszConfigAttribute);

yl::string configParser_GetLevelCfg(const char * lpszM7ID, const char * lpszDefValue);

bool configParser_RemoveConfig(const char * lpszConfigName, yl::string & strNewValue,
                               CONFIG_LEVEL_TYPE eType = CONFIG_LEVEL_IGNORE);

/**
 * @brief 配置节点信息.
 *
 * @author Song
 * @date 2018/2/28
 */

class ConfigParserNode
{
public:
    yl::string strValue;             // 配置值

    enum DSK_CONFIGPARSER_RESULT
    {
        CONFIG_RESULT_SUCC = 0,       // 获取成功
        CONFIG_RESULT_FAILED = -1,    // 失败
        CONFIG_RESULT_NOEXIST = -2,    // 配置值不存在
        CONFIG_RESULT_ID_NOEXIST = -3, // config id 不存在
        CONFIG_RESULT_LEVEL_NOEXIST = -4, //该 level 不存在
    };
    DSK_CONFIGPARSER_RESULT eResult; // 获取结果

    /** @brief The level.
    /**  获取当前生效值时，返回当前生效层级
    */

    CONFIG_LEVEL_TYPE eLevel;

    ConfigParserNode & operator = (const ConfigParserNode &);

    const char * as_string()const;

    int as_int()const;

    unsigned int as_uint()const;

    double as_double()const;

    float as_float()const;

    bool as_bool()const;
};

/**
* @brief 获取指定层级的配置.
*        获取失败会返回configMapper中定义的默认值
*        效率比configParser_GetString 系列接口更低一点(约0.3~0.5ms)
*
* @author Song
* @date 2018/2/28
*
* @param                 ConfigID.
* @param secIndex       (Optional) Zero-based index of the security.
* @param itemIndex      (Optional) Zero-based index of the item.
* @param eType          (Optional) The Config Level.
*                       CONFIG_LEVEL_IGNORE ：获取当前生效值(TODO)
*                       CONFIG_LEVEL_DEFAULT：获取configMapper中的defvalue
*
* @return A ConfigParserNode.
*/

ConfigParserNode configParser_GetConfigNode(const char * lpszConfigName, int secIndex = -1, int itemIndex = -1, CONFIG_LEVEL_TYPE eType = CONFIG_LEVEL_IGNORE);

/**
 * @brief 获取一个配置已配置的所有层级值. 不存在的层级会以当前生效值填充.
 *
 * @author Song
 * @date 2018/2/28
 *
 * @param          lpszConfigName ConfigID.
 * @param          listType       Type  list.
 * @param [in,out] listNode       The list node.
 * @param          secIndex       (Optional) Zero-based index of the security.
 * @param          itemIndex      (Optional) Zero-based index of the item.
 *
 * @return True if it succeeds, false if it fails.
 */

bool configParser_GetConfigNodeList(const char * lpszConfigName, const YLList<CONFIG_LEVEL_TYPE> & listType, YLList<ConfigParserNode> & listNode, int secIndex = -1, int itemIndex = -1);


#endif //__MODULE_CONFIG_PARSER_H__
