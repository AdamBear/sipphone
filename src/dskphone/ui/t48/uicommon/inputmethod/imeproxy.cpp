#include "imeproxy.h"
#include "path_define.h"
#include "dsklog/log.h"
#include "configparser/modconfigparser.h"
#include "configiddefine/configiddefine.h"
#include <string>
using namespace std;

////---- 临时数据防止代码报错，先保留着
const ACHAR szIMEFileName[] = "ime.txt";

CIMEProxy& CIMEProxy::GetInstance()
{
    static CIMEProxy s_cIMEProxy;

    return s_cIMEProxy;
}

CIMEProxy::CIMEProxy(void)
{
    for (iIndex = 0; iIndex < MIN_IME_ARRAY_COUNT ; iIndex++)
    {
        // 定义保存输入法字符串序列的指针数组
        pwText_123[iIndex] = NULL;// 保存输入法123内容的指针数组
        pwText_abc[iIndex] = NULL;// 保存输入法abc内容的指针数组
        pwText_Abc[iIndex] = NULL;// 保存输入法Abc内容的指针数组
        pwText_ABC[iIndex] = NULL;// 保存输入法ABC内容的指针数组
        pwText_2aB[iIndex] = NULL;// 保存输入法2aB内容的指针数组
        pwText_123_Dial[iIndex] = NULL;// 保存输入法123_Dial内容的指针数组
        pwText_123_IP[iIndex] = NULL;// 保存输入法123_IP内容的指针数组
        pwText_hebrew[iIndex] = NULL;  //保存输入法Hebrew内容的指针数组
        pwText_symbol[iIndex] = NULL;
    }

    //在构造的时候 先加载一次文件数据
    InitIMEText();

}

CIMEProxy::~CIMEProxy(void)
{
}

// 初始化IMEText
bool CIMEProxy::InitIMEText()
{
    //for debug
    INPUTMETHOD_INFO("Init IME Text From file!");

    //Config 下 IME 路径
    string strConfigIMEPath = CONFIG_PATH "ime/";
    //Factory 下 IME 路径
    string strFactoryIMEPath = FACTORY_PATH "ime/";
    //获取当前语言
    string strCurrentLang = configParser_GetConfigString(kszCurrentLang).c_str();

    //生成语言相关IME文件名
    string strLangIMEFileName = strCurrentLang + "_" + szIMEFileName;
    //生成普通的IME文件名
    string strNormalIMEFileName = szIMEFileName;

    FILE* fFile = NULL;

    //先获取语言相关的 IME 文件
    fFile = OpenAndCopyFactoryFile(strConfigIMEPath, strFactoryIMEPath, strLangIMEFileName);

    //如果语言相关的IME文件不存在  那么获取 普通的IME文件
    if (NULL == fFile)
    {
        fFile = OpenAndCopyFactoryFile(strConfigIMEPath, strFactoryIMEPath, strNormalIMEFileName);
    }

    //如果还是 失败 那么 直接返回FALSE
    if (NULL == fFile)
    {
        INPUTMETHOD_INFO("Failed to open ime file\n");
        return FALSE;

    }

    //实际调用加载输入法前  先把之前的数据清空
    freeIMEText();

    char strTemp[256] = {0};
    int iEndianType = 0;
    fread(strTemp, 1, 2, fFile);

    //判断字节序 FE FF
    if ((unsigned char)strTemp[0] == 0xfe
            && (unsigned char)strTemp[1] == 0xff)
    {
        iEndianType = 0; // 高字节在前
        INPUTMETHOD_INFO("IME Text is BigEndian\n");
    }
    else if ((unsigned char)strTemp[1] == 0xfe
             && (unsigned char)strTemp[0] == 0xff)
    {
        iEndianType = 1; // 低字节在前
        INPUTMETHOD_INFO("IME Text is LittleEndian\n");
    }
    int iImeType = -1;
    WCHAR wcsTemp[128] = {0};

    while (fgets(strTemp, 256, fFile) != NULL)
    {
        if (iEndianType == 1)
        {
            // 将文件指针挪一个字节
            // little endian时候fgets得到的字符串结尾少了一个0x00，故往后移动一位文件指针
            fseek(fFile, 1, SEEK_CUR);
        }

        memset(wcsTemp, 0, 128 * sizeof(WCHAR));

        //根据字节序将一行字符串转写入WCHAR数组
        int i = 0;
        if (iEndianType == 0)
        {
            while (i < 127 && !(strTemp[2 * i] == 0x00 && strTemp[2 * i + 1] == 0x00))
            {
                wcsTemp[i] = (WCHAR)strTemp[2 * i] * 0x100 + (unsigned char)strTemp[2 * i + 1];
                i++;
            }
        }
        else
        {
            while (i < 127 && !(strTemp[2 * i] == 0x00 && strTemp[2 * i + 1] == 0x00))
            {
                wcsTemp[i] = (unsigned char)strTemp[2 * i] + (WCHAR)strTemp[2 * i + 1] * 0x100;
                i++;
            }
        }

        memset(strTemp, 0, 256 * sizeof(char));

        //判断iImeType
        char szUtf8[256] = {0};
        strcpy(szUtf8, chW2UTF8(wcsTemp).c_str());

        if (strstr(szUtf8, "[123]") != NULL)// 设置输入123对应的类型
        {
            iImeType = 0;
            continue;
        }
        else if (strstr(szUtf8, "[abc]") != NULL) // 设置输入abc对应的类型
        {
            iImeType = 1;
            continue;
        }
        else if (strstr(szUtf8, "[ABC]") != NULL) // 设置输入ABC对应的类型
        {
            iImeType = 2;
            continue;
        }
        else if (strstr(szUtf8, "[2aB]") != NULL) // 设置输入2aB对应的类型
        {
            iImeType = 3;
            continue;
        }
        else if (strstr(szUtf8, "[123_Dial]") != NULL) // 设置输入123_Dial对应的类型
        {
            iImeType = 4;
            continue;
        }
        else if (strstr(szUtf8, "[123_IP]") != NULL) // 设置输入123_IP对应的类型
        {
            iImeType = 5;
            continue;
        }
        else if (strstr(szUtf8, "[Abc]") != NULL) // 设置输入Abc对应的类型
        {
            iImeType = 6;
            continue;
        }
        else if (strstr(szUtf8, "[Hebrew]") != NULL) //设置[Hebrew]对应的类型
        {
            iImeType = 7;
            continue;
        }
        else if (strstr(szUtf8, "[Symbol]") != NULL) //设置[Symbol]对应的类型
        {
            iImeType = 8;
            continue;
        }
        else
        {
            iImeType = -1;
            continue;
        }

        //判断iIndex
        int nIndex = 0;
        char szTarget[128] = {0};
        char szTarget1[128] = {0};

        if (sscanf(szUtf8, "%c%*[^\"]\"%[^\"]\"%[^\"]\"", &nIndex, szTarget, szTarget1) < 2)
        {
            continue;
        }

        switch (nIndex)
        {
        case '1':
        {
            iIndex = 0;
        }
        break;
        case '2':
        {
            iIndex = 1;
        }
        break;
        case '3':
        {
            iIndex = 2;
        }
        break;
        case '4':
        {
            iIndex = 3;
        }
        break;
        case '5':
        {
            iIndex = 4;
        }
        break;
        case '6':
        {
            iIndex = 5;
        }
        break;
        case '7':
        {
            iIndex = 6;
        }
        break;
        case '8':
        {
            iIndex = 7;
        }
        break;
        case '9':
        {
            iIndex = 8;
        }
        break;
        case '0':
        {
            iIndex = 9;
        }
        break;
        case '*':
        {
            iIndex = 10;
        }
        break;
        case '#':
        {
            iIndex = 11;
        }
        break;
        default:
        {
            continue;
        }
        break;
        }

#if LINUX_SYSTEM
        chStringW str = chUTF82W(szTarget);
        if (szTarget1[0] == 0)
        {
            str = str + chUTF82W("\"");
        }
        else if (szTarget1[0] != '\r')
        {
            str = str + chUTF82W("\"");
            str = str + chUTF82W(szTarget1);
        }

        //sizeof(WCHAR)：linux下为4，win32为2
        int iLength = sizeof(WCHAR) * (str.GetLength() + 1);

        // 根据ImeType将内容放入对应的数组中
        switch (iImeType)
        {
        case 0:
        {
            pwText_123[iIndex] = (WCHAR*)malloc(iLength);// 分配内存
            memset(pwText_123[iIndex], 0, iLength);// 初始化内存
            wcscpy(pwText_123[iIndex], str.c_str());// 负责内容到指定内存
        }
        break;
        case 1:
        {
            pwText_abc[iIndex] = (WCHAR*)malloc(iLength);
            memset(pwText_abc[iIndex], 0, iLength);
            wcscpy(pwText_abc[iIndex], str.c_str());
        }
        break;
        case 2:
        {
            pwText_ABC[iIndex] = (WCHAR*)malloc(iLength);
            memset(pwText_ABC[iIndex], 0, iLength);
            wcscpy(pwText_ABC[iIndex], str.c_str());
        }
        break;
        case 3:
        {
            pwText_2aB[iIndex] = (WCHAR*)malloc(iLength);
            memset(pwText_2aB[iIndex], 0, iLength);
            wcscpy(pwText_2aB[iIndex], str.c_str());
        }
        break;
        case 4:
        {
            pwText_123_Dial[iIndex] = (WCHAR*)malloc(iLength);
            memset(pwText_123_Dial[iIndex], 0, iLength);
            wcscpy(pwText_123_Dial[iIndex], str.c_str());
        }
        break;
        case 5:
        {
            pwText_123_IP[iIndex] = (WCHAR*)malloc(iLength);
            memset(pwText_123_IP[iIndex], 0, iLength);
            wcscpy(pwText_123_IP[iIndex], str.c_str());
        }
        break;
        case 6:
        {
            pwText_Abc[iIndex] = (WCHAR*)malloc(iLength);
            memset(pwText_Abc[iIndex], 0, iLength);
            wcscpy(pwText_Abc[iIndex], str.c_str());
        }
        break;
        case 7:
        {
            pwText_hebrew[iIndex] = (WCHAR*)malloc(iLength);
            memset(pwText_hebrew[iIndex], 0, iLength);
            wcscpy(pwText_hebrew[iIndex], str.c_str());
        }
        break;
        case 8:
        {
            pwText_symbol[iIndex] = (WCHAR*)malloc(iLength);
            memset(pwText_symbol[iIndex], 0, iLength);
            wcscpy(pwText_symbol[iIndex], str.c_str());
        }
        break;
        }
#endif
    } //while


    //close
    if (fFile != NULL)
    {
        fclose(fFile);
        fFile = NULL;
    }

    return TRUE;
}

QString CIMEProxy::WCHARStrToQString(WCHAR* wcharData)
{
    if (NULL == wcharData)
    {
        return "";
    }

    QString strResult = "";

    int iDataLen = wcslen(wcharData);

    for (int i = 0; i < iDataLen ; i++)
    {
        strResult = strResult + QChar(wcharData[i]);
    }

    return strResult;
}

// 释放读入的IME资源
void CIMEProxy::freeIMEText()
{
    // 循环释放IME的资源
    for (int i = 0; i < 12; i++)
    {
        if (pwText_123[i])
        {
            free(pwText_123[i]);// 释放内存
            pwText_123[i] = NULL;// 释放内存后，给指针赋值，防止野指针
        }

        if (pwText_abc[i])
        {
            free(pwText_abc[i]);
            pwText_abc[i] = NULL;
        }

        if (pwText_ABC[i])
        {
            free(pwText_ABC[i]);
            pwText_ABC[i] = NULL;
        }

        if (pwText_2aB[i])
        {
            free(pwText_2aB[i]);
            pwText_2aB[i] = NULL;
        }

        if (pwText_123_Dial[i])
        {
            free(pwText_123_Dial[i]);
            pwText_123_Dial[i] = NULL;
        }

        if (pwText_123_IP[i])
        {
            free(pwText_123_IP[i]);
            pwText_123_IP[i] = NULL;
        }

        if (pwText_Abc[i])
        {
            free(pwText_Abc[i]);
            pwText_Abc[i] = NULL;
        }

        if (pwText_hebrew[i])
        {
            free(pwText_hebrew[i]);
            pwText_hebrew[i] = NULL;
        }

        if (pwText_symbol[i])
        {
            free(pwText_symbol[i]);
            pwText_symbol[i] = NULL;
        }

    }
}

//先打开Config 路径下ime文件  如果不存在那么从Factory 路径下拷贝 然后在打开
//参数1 Config 下的IME 路径  参数2 Factory 下的IME 路径  参数3 文件名称
FILE* CIMEProxy::OpenAndCopyFactoryFile(string strConfigIMEPath, string strFactoryIMEPath, string strIMEFileName)
{
    //获取 Config 路径下的 IME 文件名称
    string strConfigIMEFileName = strConfigIMEPath + strIMEFileName;
    //获取 Factory 路径下的 IME 文件名称
    string strFactoryIMEFileName = strFactoryIMEPath + strIMEFileName;

    FILE* fFile = NULL;

    //如果Config 配置下存在 那么直接读Config 路径下的IME 文件就可以了
    if (pathFileExist(strConfigIMEFileName.c_str()))
    {
        fFile = fopen(strConfigIMEFileName.c_str(), "rb");

        return fFile;
    }

    //Config 路径下 IME 文件不存在

#ifdef WIN32
    INPUTMETHOD_INFO("Failed to open ime file\n");
    return FALSE;
#endif

    //如果Factory 路径下的IME文件也不存在 那么直接返回NULL
    if (!pathFileExist(strFactoryIMEFileName.c_str()))
    {
        return NULL;
    }

    //Factory 路径下的IME文件存在 那么先拷贝 到Config 下  然后再打开
    // 拷贝文件并重新打开文件
    char szCommand[256] = {0};
    sprintf(szCommand, "cp %s %s", strFactoryIMEFileName.c_str(), strConfigIMEFileName.c_str());
    system(szCommand);

    //如果Config 路径下的IME 文件存在  直接打开
    if (pathFileExist(strConfigIMEFileName.c_str()))
    {
        fFile = fopen(strConfigIMEFileName.c_str(), "rb");

        return fFile;
    }

    //不存在直接返回NULL

    return NULL;
}

