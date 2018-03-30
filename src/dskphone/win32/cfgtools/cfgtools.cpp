#include <ETLLib.hpp>
#include <xmlparser/pugixml.hpp>
#include <iostream>
#include <winimp/winimp.h>
#include "transformatecode.h"
using namespace std;
using namespace pugi;

#define SWITCH_CASE(name, value) case name: value = #name; break;
#define LOG_TRACE(fmt, ...)  do { innerTrace(formatString(fmt "\n", ##__VA_ARGS__)); } while (0);

static void innerTrace(const string & str)
{
    cout << str;
    //fprintf(stdout, "%s", str.c_str());
    OutputDebugString(str.c_str());
};

struct mapItemInfo
{
    string strDefValue;
    string strM7;
    string strReg;
};

typedef map<string, mapItemInfo>    mapCfgValue;
typedef vector<string>              vecStrValue;
typedef istream_iterator<string>    ISTSTR;
typedef ostream_iterator<string>    OSTSTR;

static void XmlNode2Map(LPCSTR lpFileName, const xml_node & node, mapCfgValue & mapComon)
{
    if (node)
    {
        xml_node nodeChild = node.child("item");
        while (nodeChild)
        {
            mapItemInfo info;
            string strName = nodeChild.attribute("name").as_string();
            info.strDefValue = nodeChild.attribute("defvalue").as_string();
            info.strM7 = nodeChild.attribute("M7").as_string();
            info.strReg = nodeChild.attribute("reg").as_string();

            if (mapComon.find(strName) == mapComon.end())
            {
                mapComon[strName] = info;
            }
            else
            {
                LOG_TRACE("file [%s] sec [%s] item [%s] twice", lpFileName, node.name(), strName.c_str());
            }

            nodeChild = nodeChild.next_sibling();
        }
    }
}

static void ParseXmlFile(LPCSTR lpFileName, mapCfgValue & mapComon, mapCfgValue & mapAcc)
{
    if (textEMPTY(lpFileName))
    {
        return;
    }

    xml_document doc;
    if (doc.load_file(lpFileName))
    {
        xml_node nodeRoot = doc.child("IniConfig");
        if (nodeRoot)
        {
            XmlNode2Map(lpFileName, nodeRoot.child("Common"), mapComon);
            XmlNode2Map(lpFileName, nodeRoot.child("Account"), mapAcc);
        }
    }
}

static void Map2XmlNode(xml_node & node, const mapCfgValue & mapComon)
{
    for (mapCfgValue::const_iterator it = mapComon.begin(); it != mapComon.end(); ++it)
    {
        const string & name = it->first;
        const mapItemInfo & item = it->second;

        if (name.empty())
        {
            continue;
        }

        xml_node nodeChild = node.append_child(node_element);
        nodeChild.set_name("item");
        nodeChild.append_attribute("name", name.c_str());
        nodeChild.append_attribute("defvalue", item.strDefValue.c_str());
        nodeChild.append_attribute("M7", item.strM7.c_str());

        if (!item.strReg.empty())
        {
            nodeChild.append_attribute("reg", item.strReg.c_str());
        }
    }
}

static void ToXmlFile(LPCSTR lpFileName, const mapCfgValue & mapComon, const mapCfgValue & mapAcc)
{
    if (textEMPTY(lpFileName))
    {
        return;
    }

    xml_document doc;

    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    xml_node nodeRoot = doc.append_child(node_element);
    nodeRoot.set_name("IniConfig");

    xml_node nodeCommon = nodeRoot.append_child(node_element);
    nodeCommon.set_name("Common");

    Map2XmlNode(nodeCommon, mapComon);

    xml_node nodeAcc = nodeRoot.append_child(node_element);
    nodeAcc.set_name("Account");

    Map2XmlNode(nodeAcc, mapAcc);

    doc.save_file(lpFileName);
}

static void MapCombine(mapCfgValue & mapHost, mapCfgValue & mapSub)
{
    for (mapCfgValue::iterator it = mapSub.begin(); it != mapSub.end(); ++it)
        if (mapHost.find(it->first) == mapHost.end())
        {
            mapHost[it->first] = it->second;
        }
        else
        {
            LOG_TRACE("combine same item in host [%s]", it->first.c_str());
        }
}

static void MapCompare(mapCfgValue & mapHost, mapCfgValue & mapSub)
{
    for (mapCfgValue::iterator it = mapSub.begin(); it != mapSub.end();)
    {
        if (mapHost.find(it->first) != mapHost.end())
        {
            LOG_TRACE("compare same item in host [%s]", it->first.c_str());
            mapSub.erase(it++);
        }
        else
        {
            ++it;
        }
    }
}
/*******************************************************************
** 函数描述:   转化指定路径下指定后缀名文件的编码格式
** 参数：      strDirectory 待查找目录
               fromCode 需转码的文件编码格式
               toCode 需求的文件编码格式
** 输出：
** 返回:
********************************************************************/
static void TransformateCode(const yl::string & strDirectory, const char * fromCode,
                             const char * toCode)
{
    std::vector<std::string> plist;
    ls_path(strDirectory.c_str(), plist, "*.h;*.c;*.cpp;*.hpp;*.cxx;*.cc", true, true);
    std::vector<std::string>::iterator itFind = plist.begin();
    for (; itFind != plist.end(); itFind++)
    {
        std::string fileName = *itFind;
        convertCode_func(fileName.c_str(), fromCode, toCode);
    }
}

static void appendBOM_toUTF8File(const yl::string & strDirectory)
{
    std::vector<std::string> plist;
    ls_path(strDirectory.c_str(), plist, "*.h;*.c;*.cpp;*.hpp;*.cxx;*.cc", true, true);
    std::vector<std::string>::iterator itFind = plist.begin();
    for (; itFind != plist.end(); itFind++)
    {
        std::string fileName = *itFind;
        convertUTF8_BOM(fileName.c_str());
    }
}

int main(int argc, char * argv[])
{
    try
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
        // 命令行参数
        static char * const short_options = "a:c:d:e:fg:i:j:k:l::m:n::o:p:q:s:t:uvw:x:z:?h:";
        static const struct option long_options[] =
        {
            // param
            {  "sha256",     1,   NULL,   'a'  },
            {  "clean",      1,   NULL,   'c'  },
            {  "decrypt",    1,   NULL,   'd'  },
            {  "encrypt",    1,   NULL,   'e'  },
            {  "file",       0,   NULL,   'f'  },
            {  "merge",      1,   NULL,   'g'  },
            {  "input",      1,   NULL,   'i'  },
            {  "encode",     1,   NULL,   'j'  },
            {  "ls",         2,   NULL,   'l'  },
            {  "mask",       1,   NULL,   'm'  },
            {  "systeminfo", 2,   NULL,   'n'  },
            {  "output",     1,   NULL,   'o'  },
            {  "port",       1,   NULL,   'p'  },
            {  "process",    1,   NULL,   'q'  },
            {  "strip",      1,   NULL,   's'  },
            {  "delete",     1,   NULL,   't'  },
            {  "uuid",       0,   NULL,   'u'  },
            {  "remove",     0,   NULL,   'v'  },
            {  "decode",     1,   NULL,   'w'  },
            {  "uncpmoress", 1,   NULL,   'x'  },
            {  "compress",   1,   NULL,   'z'  },
            {  "transcode",  1,   NULL,   'k'  },
            {  "help",       0,   NULL,   '?'  },
            {  "addbom",     0,   NULL,   'h' },
            {  0,            0,   NULL,    0   }
        };

        int ch = 0;
        string input, output, mask;
        bool bRemove = false, bFile = false;

        while ((ch = getopt_long(argc, argv, short_options, long_options, NULL)) != -1)
        {
            switch (ch)
            {
            case 's':
                {
                    remove_empty_directory(optarg);
                }
                break;
            case 'c':
                {
                    delete_files_in_directory(optarg, true, true);
                }
                break;
            case 't':
                {
                    if (textEMPTY(optarg) || mask.empty())
                    {
                        return -1;
                    }

                    delete_files_in_directory(optarg, mask.c_str(), true);
                    remove_empty_directory(optarg);
                }
                break;
            // 扫描文件
            case 'l':
                {
                    if (mask.empty() || output.empty())
                    {
                        return -1;
                    }

                    vecStrValue vec;
                    ls_path(input.c_str(), vec, mask.c_str(), true, true);

                    if (!output.empty())
                    {
                        ofstream ofs(output.c_str());
                        if (!ofs)
                        {
                            return -1;
                        }

                        copy(vec.begin(), vec.end(), OSTSTR(ofs, "\n"));
                        copy(vec.begin(), vec.end(), OSTSTR(cout, "\n"));
                        ofs.close();
                    }

                    if (optarg)
                    {
                        ofstream ofs(optarg);
                        if (!ofs)
                        {
                            return -1;
                        }

                        transform(vec.begin(), vec.end(), vec.begin(), path_get_encrypt);
                        copy(vec.begin(), vec.end(), OSTSTR(ofs, "\n"));
                        copy(vec.begin(), vec.end(), OSTSTR(cout, "\n"));
                        ofs.close();
                    }
                }
                break;
            // 加密
            case 'e':
                {
                    if (bFile)
                    {
                        string name = path_get_encrypt(optarg);
                        path_copy_encrypt_file(optarg, name);
                        LOG_TRACE("%s", name.c_str());

                        if (bRemove)
                        {
                            unlink(optarg);
                        }
                    }
                    else
                    {
                        ifstream ifs(optarg);
                        if (!ifs)
                        {
                            return -1;
                        }

                        vecStrValue vec;
                        copy(ISTSTR(ifs), ISTSTR(), back_inserter(vec));
                        copy(vec.begin(), vec.end(), OSTSTR(cout, "\n"));

                        vecStrValue::iterator it = vec.begin();
                        for (; it != vec.end(); ++it)
                        {
                            path_copy_encrypt_file(*it, path_get_encrypt(*it));

                            if (bRemove)
                            {
                                unlink(it->c_str());
                            }
                        }
                    }
                }
                break;
            // 加密文件名
            case 'w':
                {
                    if (bFile)
                    {
                        string name = path_get_encrypt(optarg);
                        LOG_TRACE("%s", name.c_str());
                    }
                    else
                    {
                        ifstream ifs(optarg);
                        if (!ifs)
                        {
                            return -1;
                        }

                        vecStrValue vec;
                        copy(ISTSTR(ifs), ISTSTR(), back_inserter(vec));
                        copy(vec.begin(), vec.end(), OSTSTR(cout, "\n"));

                        vecStrValue::iterator it = vec.begin();
                        for (; it != vec.end(); ++it)
                        {
                            string name = path_get_encrypt(*it);
                            LOG_TRACE("%s", name.c_str());
                        }
                    }
                }
                break;
            // 解密
            case 'd':
                {
                    if (bFile)
                    {
                        string data;
                        if (dump_encrypt_file(optarg, data))
                        {
                            write_file_imp(optarg, data);
                        }

                        if (bRemove)
                        {
                            string name = path_get_encrypt(optarg);
                            unlink(name.c_str());
                        }
                    }
                    else
                    {
                        ifstream ifs(optarg);
                        if (!ifs)
                        {
                            return -1;
                        }

                        vecStrValue vec;
                        copy(ISTSTR(ifs), ISTSTR(), back_inserter(vec));
                        copy(vec.begin(), vec.end(), OSTSTR(cout, "\n"));

                        vecStrValue::iterator it = vec.begin();
                        for (; it != vec.end(); ++it)
                        {
                            string data;
                            if (dump_encrypt_file(*it, data))
                            {
                                write_file_imp(*it, data);
                            }

                            if (bRemove)
                            {
                                string name = path_get_encrypt(*it);
                                unlink(name.c_str());
                            }
                        }
                    }
                }
                break;
            // 合并两个cfgmapper.xml文件，同时排序
            case 'g':
                {
                    if (input.empty() || output.empty())
                    {
                        return -1;
                    }

                    ofstream ofs("cfgmapper.txt");
                    streambuf * rdbuf_bak = cout.rdbuf(ofs.rdbuf());

                    mapCfgValue mapHost, mapSub, mapAccHost, mapAccSub;

                    ParseXmlFile(input.c_str(), mapHost, mapAccHost);
                    ParseXmlFile(optarg, mapSub, mapAccSub);

                    MapCompare(mapAccHost, mapHost);
                    MapCompare(mapAccSub, mapSub);

                    MapCombine(mapHost, mapSub);
                    MapCombine(mapAccHost, mapAccSub);

                    MapCompare(mapAccHost, mapHost);

                    ToXmlFile(output.c_str(), mapHost, mapAccHost);

                    cout.rdbuf(rdbuf_bak);
                    ofs.close();
                }
                break;
            // 检测端口
            case 'p':
                {
                    if (!is_all<LPCSTR>(optarg, optarg + strlen(optarg), isdigit))
                    {
                        return -1;
                    }

                    DWORD dw = 0;
                    WAPIGetProcessIDByLocalPort(atoi(optarg), dw);
                    LOG_TRACE("%d", dw);
                    return (int)dw;
                }
                break;
            // 获取进程
            case 'q':
                {
                    DWORD dw = 0;
                    WAPIGetProcessID(optarg, dw);
                    LOG_TRACE("%d", dw);
                    return (int)dw;
                }
                break;
            // 压缩
            case 'z':
                {
                    WAPICompressFile(input, optarg);
                }
                break;
            // 解压缩
            case 'x':
                {
                    WAPIUnCompressFile(input, optarg);
                }
                break;
            // 系统概况
            case 'n':
                {
                    string data = WAPIGetSystemInfo();
                    if (bFile)
                    {
                        ofstream ofs(optarg);
                        if (ofs)
                        {
                            ofs << data << endl;
                            ofs.close();
                        }
                    }
                    else
                    {
                        LOG_TRACE("%s", data.c_str());
                    }
                }
                break;
            // sha256
            case 'a':
                {
                    string data = optarg;
                    if (bFile)
                    {
                        dump_file_imp(optarg, data);
                    }
                    else
                    {
                        trim_both(data, "\" \r\n\t");
                    }

                    data = WAPISha256Hex(data);
                    LOG_TRACE("%s", data.c_str());
                }
                break;
            // dedect encode
            case 'j':
                {
                    UniMode mode = DetermineFileEncoding(optarg);
                    LPCSTR lpmode = NULL;
                    switch (mode)
                    {
                        SWITCH_CASE(uni8Bit, lpmode);
                        SWITCH_CASE(uniUTF8, lpmode);
                        SWITCH_CASE(uni16BE, lpmode);
                        SWITCH_CASE(uni16LE, lpmode);
                        SWITCH_CASE(uniCookie, lpmode);
                        SWITCH_CASE(uni7Bit, lpmode);
                        SWITCH_CASE(uni16BE_NoBOM, lpmode);
                        SWITCH_CASE(uni16LE_NoBOM, lpmode);
                    default:
                        lpmode = "uniUnkown";
                    }
                    LOG_TRACE("%s", lpmode);
                }
                break;
            case 'u':
                {
                    LOG_TRACE("%s", WAPIGetUUID().c_str());
                }
                break;
            case 'i':
                {
                    input = optarg;
                }
                break;
            case 'o':
                {
                    output = optarg;
                }
                break;
            case 'm':
                {
                    mask = optarg;
                }
                break;
            case 'f':
                {
                    bFile = true;
                }
                break;
            case 'v':
                {
                    bRemove = true;
                }
                break;
            case  'h':
                {
                    std::string path(optarg);
                    appendBOM_toUTF8File(optarg);
                }
                break;
            case 'k':
                {
                    std::string path(optarg);
                    TransformateCode(optarg, input.c_str(), output.c_str());
                }
                break;
            case '?':
                {
                    // help or test
                }
                break;
            default:
                break;
            }
        }

        return 0;
    }
    catch (exception & e)
    {
        LOG_TRACE("%s", e.what());
    }

    return -1;
}
