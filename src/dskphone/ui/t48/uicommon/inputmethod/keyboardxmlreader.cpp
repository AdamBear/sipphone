#include "keyboardxmlreader.h"

#include "commonapi/systemhelperapi.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "commondefine/service_config.h"
#include "adapterbox/include/modadapterbox.h"

CKeyboardXmlReader::CKeyboardXmlReader()
    : m_bIsLangFileInConfigPath(false)
{

}

CKeyboardXmlReader::~CKeyboardXmlReader()
{

}

CKeyboardXmlReader& CKeyboardXmlReader::GetInstance()
{
    static CKeyboardXmlReader myKeyboardXmlReader;
    return myKeyboardXmlReader;
}

void CKeyboardXmlReader::LoadKeyboardXmlFiles()
{
    //清除原有数据
    m_listFactoryLayoutFile.clear();
    m_listConfigLayoutFile.clear();
    m_listFactoryImeFile.clear();
    m_listConfigImeFile.clear();
    m_bIsLangFileInConfigPath = false;
    m_strLangFile = "";

    //加载factory目录下的文件
    listString listFactoryFile;
    commonAPI_getFilesInDirectoryByExt(&listFactoryFile, INPUT_FILES_FACTORY_PATH, "xml", false, false, true);

    //遍历获取到的文件
    listString::iterator iterFacotryFile = listFactoryFile.begin();
    for (; iterFacotryFile != listFactoryFile.end(); ++iterFacotryFile)
    {
        yl::string& strFactoryFile = *iterFacotryFile;
        //获取其中的layout文件
        if (0 == strFactoryFile.find(INPUT_FILES_PREFIX_LAYOUT, 0, INPUT_FILES_PREFIX_LAYOUT_LENGTH))
        {
            m_listFactoryLayoutFile.push_back(strFactoryFile);
        }
        //获取其中的ime文件
        else if (0 == strFactoryFile.find(INPUT_FILES_PREFIX_IME, 0, INPUT_FILES_PREFIX_IME_LENGTH))
        {
            m_listFactoryImeFile.push_back(strFactoryFile);
        }
        //获取其中的lang文件
        else if (0 == strFactoryFile.compare(INPUT_FILES_LANG))
        {
            m_strLangFile = strFactoryFile;
        }
    }

    //加载config目录下的文件
    listString listConfigFile;
    commonAPI_getFilesInDirectoryByExt(&listConfigFile, INPUT_FILES_CONFIG_PATH, "xml", false, false, true);

    //遍历获取到的文件
    listString::iterator iterConfigFile = listConfigFile.begin();
    for (; iterConfigFile != listConfigFile.end(); ++iterConfigFile)
    {
        yl::string& strConfigFile = *iterConfigFile;
        //获取其中的layout文件
        if (0 == strConfigFile.find(INPUT_FILES_PREFIX_LAYOUT, 0, INPUT_FILES_PREFIX_LAYOUT_LENGTH))
        {
            m_listConfigLayoutFile.push_back(strConfigFile);

            //如果Config中获取到的文件和Factory中获取到的同名，则去掉factory中的对应文件
            if (m_listFactoryLayoutFile.size() > 0)
            {
                iterFacotryFile = m_listFactoryLayoutFile.begin();
                for (; iterFacotryFile != m_listFactoryLayoutFile.end(); ++iterFacotryFile)
                {
                    yl::string& strFactoryFile = *iterFacotryFile;
                    if (0 == strFactoryFile.compare(strConfigFile))
                    {
                        iterFacotryFile = m_listFactoryLayoutFile.erase(iterFacotryFile);
                        break;
                    }
                }
            }
            /////////////////////////////////
        }
        //获取其中的ime文件
        else if (0 == strConfigFile.find(INPUT_FILES_PREFIX_IME, 0, INPUT_FILES_PREFIX_IME_LENGTH))
        {
            m_listConfigImeFile.push_back(strConfigFile);

            //如果Config中获取到的文件和Factory中获取到的同名，则去掉factory中的对应文件
            if (m_listFactoryImeFile.size() > 0)
            {
                iterFacotryFile = m_listFactoryImeFile.begin();
                for (; iterFacotryFile != m_listFactoryImeFile.end(); ++iterFacotryFile)
                {
                    yl::string& strFactoryFile = *iterFacotryFile;
                    if (0 == strFactoryFile.compare(strConfigFile))
                    {
                        iterFacotryFile = m_listFactoryImeFile.erase(iterFacotryFile);
                        break;
                    }
                }
            }
            /////////////////////////////////
        }
        //获取其中的lang文件
        else if (0 == strConfigFile.compare(INPUT_FILES_LANG))
        {
            //如果config中有定义lang文件，则直接覆盖掉factory中的
            m_bIsLangFileInConfigPath = true;
            m_strLangFile = strConfigFile;
        }
    }
}
