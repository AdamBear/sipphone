#include "common.h"
#include "service_cfgtools.h"

std::string path_get_encrypt(const std::string & path)
{
    // get encrypt file name by path
    char buffer[BUFFER_SIZE] = { 0 };
    if (cfg_get_sfile_name(path.c_str(), buffer, BUFFER_SIZE) == 0)
    {
        return buffer;
    }
    return path;
}

bool path_file_exist(const std::string & path)
{
    return path_rw_access(path) || path_rw_access(path_get_encrypt(path));
}

//路径映射文件路径
#ifdef WIN32
#pragma data_seg("ShareMem")
static char res_map_path[MAX_PATH + 1] = {0};
static char deviceName[MAX_DEVICE_NAME + 1] = {0};
#pragma data_seg()
#pragma comment(linker, "/SECTION:ShareMem,RWS")
#endif

void setResPathMapFile(const std::string & path)
{
    strncpy(res_map_path, path.c_str(), min(path.size(), MAX_PATH));
}
void setPhoneDeviceName(const char * devName)
{
    strncpy(deviceName, devName, min(strlen(devName), MAX_DEVICE_NAME));
}

bool path_is_encrypt(const std::string & path, std::string & out)
{
    if (path.empty())
    {
        return false;
    }


    out = path;

    typedef std::map<std::string, std::string> MapCfg;
    static MapCfg map_cfgFile;
    static bool s_bInited = false;
    if (!s_bInited)
    {
        s_bInited = true;
        std::ifstream ifs(res_map_path);
        if (ifs)
        {
            std::vector<std::string> vec;
            std::string lineData; //暂存获取到的一行数据
            while (!ifs.eof())
            {
                getline(ifs, lineData);
                trim_both(lineData);
                if (lineData.size() > 0)
                {
                    if (lineData[0] == ';') //注释
                    {
                        continue;
                    }
                    vec.push_back(lineData);
                }
            }
            std::vector<std::string>::iterator it = vec.begin();
            for (; it != vec.end(); it++)
            {
                std::vector<std::string> vecEx;
                parseString(vecEx, *it, "||");

                if (vecEx.size() >= 2)
                {
                    std::string src = path_canonicalize(vecEx[0]);
                    trim_both(src);
                    std::string dst = path_canonicalize(vecEx[1]);
                    if (vecEx.size() >= 3)
                    {
                        if (vecEx[2].size())
                        {
                            std::vector<std::string> devEx;
                            parseString(devEx, vecEx[2], ",");
                            for (int index = 0; index < devEx.size(); index++)
                            {
                                std::string srcTemp = src;
                                trim_both(devEx[index]);
                                srcTemp.append(devEx[index].c_str());
                                map_cfgFile[srcTemp] = dst;
                            }
                        }
                        else
                        {
                            map_cfgFile[src] = dst;
                        }
                    }
                    else
                    {
                        map_cfgFile[src] = dst;
                    }
                }
            }
        }
        //首次读取映射文件时对当前路径进行一次映射确认

        std::string path_temp = path_canonicalize(path);
        path_temp.append(deviceName);
        MapCfg::iterator it = map_cfgFile.find(path_temp);
        if (it != map_cfgFile.end())
        {
            out = it->second;
            if (path_rw_access(out.c_str()))
            {
                return true;
            }
        }
        else
        {
            MapCfg::iterator it = map_cfgFile.find(path_canonicalize(path));
            if (it != map_cfgFile.end())
            {
                out = it->second;
                if (path_rw_access(out.c_str()))
                {
                    return true;
                }
            }
        }
    }
    else
    {
        MapCfg::iterator it = map_cfgFile.find(path_canonicalize(path + deviceName));
        if (it != map_cfgFile.end())
        {
            out = it->second;
            if (path_rw_access(out.c_str()))
            {
                return true;
            }
        }
        else
        {
            MapCfg::iterator it = map_cfgFile.find(path_canonicalize(path));
            if (it != map_cfgFile.end())
            {
                out = it->second;
                if (path_rw_access(out.c_str()))
                {
                    return true;
                }
            }
        }
    }

    // get the std file path, use to get the real file path
    std::string str_org_path = path_canonicalize(path);
    std::string str_new_path = path_get_encrypt(str_org_path);

    if (str_new_path.empty())
    {
        return false;
    }

    // get private path
    if (path_rw_access(str_new_path.c_str()))
    {
        out = str_new_path;
        return true;
    }

    // get current path
    char current_path[BUFFER_SIZE] = {0};
    strcpy(current_path, path.c_str());
    PathRemoveFileSpec(current_path);
    PathAppend(current_path, str_new_path.c_str());
    if (path_rw_access(current_path))
    {
        out = path_canonicalize(current_path);
        return true;
    }

    return false;
}

bool decrypt_buf(const std::string & data, std::string & result)
{
    if (data.empty())
    {
        return false;
    }

    // 2 Ratio for prepare
    size_t len = data.size() * 2;
    char * buffer = (char *)::GlobalAlloc(GPTR, len);
    if (!buffer)
    {
        return false;
    }

    int real_size = cfg_sfile_buf_decrypt2buf(data.c_str(), data.size(), buffer, len);

    // decrypted file succed
    if (real_size >= 0)
    {
        result.assign(buffer, min(len, real_size));
    }

    ::GlobalFree(buffer);
    return real_size >= 0;
}

bool encrypt_buf(const std::string & data, std::string & result)
{
    if (data.empty())
    {
        return false;
    }

    size_t len = data.size() * 2;
    char * buffer = (char *)::GlobalAlloc(GPTR, len);
    if (!buffer)
    {
        return false;
    }

    int real_size = cfg_sfile_buf_encrypt2buf(data.c_str(), data.size(), buffer, len);

    // encrypt file succed
    if (real_size >= 0)
    {
        result.assign(buffer, min(len, real_size));
    }

    ::GlobalFree(buffer);
    return real_size >= 0;
}

bool dump_encrypt_file(const std::string & path, std::string & data)
{
    std::string encrypt_path;
    path_is_encrypt(path, encrypt_path);

    if (!dump_file_imp(encrypt_path, data))
    {
        return false;
    }
    decrypt_buf(data, data);
    return true;
}

bool write_encrypt_file(const std::string & path, const std::string & data)
{
    std::string encrypt_path;
    path_is_encrypt(path, encrypt_path);

    std::string tmp(data);
    encrypt_buf(tmp, tmp);
    return write_file_imp(encrypt_path, tmp);
}

bool path_copy_encrypt_file(const std::string & src, const std::string & to)
{
    std::string data;
    return dump_encrypt_file(src, data) && write_encrypt_file(to, data);
}
