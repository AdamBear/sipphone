#include "common.h"

bool dump_file_imp(const std::string & fileName, std::string & data)
{
    return dump_file_imp(fileName, data, 0, 0) > 0;
}

size_t dump_file_imp(const std::string & fileName, std::string & data, size_t from, size_t size)
{
    FILE * file = fopen(fileName.c_str(), "rb");
    if (file)
    {
        long len = get_file_size(file);
        if (from >= 0)
        {
            len -= from;
            if (size > 0)
            {
                len = min(len, size);
            }
        }
        else
        {
            from = 0;
        }

        if (len <= 0 || fseek(file, from, SEEK_SET) < 0)
        {
            fclose(file);
            return 0;
        }

        char * buffer = (char *)::GlobalAlloc(GPTR, len);
        if (!buffer)
        {
            return 0;
        }

        size_t read = fread(buffer, sizeof(std::string::value_type), len, file);
        fclose(file);
        data.assign(buffer, min(read, len));
        ::GlobalFree(buffer);
        return read;
    }

    return 0;
}

size_t strip_encode_bom(std::string & data)
{
    size_t bom = bomLength((const unsigned char *)data.c_str(), data.size());
    if (bom > 0)
    {
        data.erase(0, bom);
    }
    return bom;
}

bool dump_utf16_file_imp(const std::string & fileName, std::wstring & data)
{
    std::string u8data;
    if (!dump_file_imp(fileName, u8data))
    {
        return false;
    }

    UniMode mode = DetermineCharEncoding(u8data.c_str(), u8data.size());
    bool bLE = mode == uni16LE_NoBOM || mode == uni16LE;
    bool bBE = mode == uni16BE_NoBOM || mode == uni16BE;
    bool bBom = mode == uni16LE || mode == uni16BE;

    if (!bLE && !bBE)
    {
        return false;
    }

    std::string::size_type pos = bBom ? 2 : 0;
    std::string::size_type base = 0;
    int count = (u8data.size() / 2) - (pos / 2);
    std::wstring result(count, L'\0');

    while (count-- > 0)
    {
        result[base++] = bLE ?
                         MAKEWORD(u8data[pos], u8data[pos + 1]) : MAKEWORD(u8data[pos + 1], u8data[pos]);
        pos += 2;
    }

    data.swap(result);
    return true;
}

bool write_file_imp(const std::string & fileName, const std::string & data)
{
    FILE * file = fopen(fileName.c_str(), "wb");
    if (file)
    {
        fwrite(data.c_str(), sizeof(std::string::value_type), data.size(), file);
        fclose(file);
        return true;
    }
    return false;
}

long get_file_size(FILE * file)
{
    if (!file)
    {
        return -1;
    }

    long curpos = ftell(file);
    fseek(file, 0L, SEEK_END);
    long size = ftell(file);
    fseek(file, curpos, SEEK_SET);
    return size;
}

long get_file_size(const std::string & path)
{
    FILE * file = fopen(path.c_str(), "rb");
    if (file)
    {
        long size = get_file_size(file);
        fclose(file);
        return size;
    }

    return -1;
}

bool path_rw_access(const std::string & path)
{
    return _access(path.c_str(), 06) == 0;
}

void path_replace_spec(std::string & str, const std::string & from, const std::string & to)
{
    replaceString(str, from, to);
}

bool path_match_spec(LPCSTR path, LPCSTR filter)
{
    if (!path || !filter)
    {
        return false;
    }

    if (strcmp(path, filter) == 0 || strcmp(filter, "") == 0)
    {
        return true;
    }

    std::vector<std::string> vec;
    if (parseString(vec, filter, ";,"))
        for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); ++it)
            if (PathMatchSpec(path, it->c_str()))
            {
                return true;
            }
    return false;
}

std::string path_canonicalize(const std::string & path)
{
    if (path.empty())
    {
        return "";
    }

    std::string str = path;
    // replace splash to "\\"
    path_replace_spec(str, "/", "\\");
    path_replace_spec(str, "\\\\", "\\");
    path_replace_spec(str, "\\.\\", "\\");

    char buffer[BUFFER_SIZE] = {0};
    strcpy(buffer, str.c_str());
    PathRemoveBlanks(buffer);
    PathMakePretty(buffer);
    PathUndecorate(buffer);
    return buffer;
}

std::string path_append_sub(const std::string & path, const std::string & sub)
{
    std::string str = path_canonicalize(path);
    char buffer[BUFFER_SIZE] = { 0 };
    strcpy(buffer, str.c_str());
    PathRemoveFileSpec(buffer);
    PathAppend(buffer, sub.c_str());

    char buffer_name[BUFFER_SIZE] = { 0 };
    strcpy(buffer_name, str.c_str());
    PathStripPath(buffer_name);
    PathAppend(buffer, buffer_name);

    return buffer;
}

bool delete_files_in_directory(LPCSTR path, bool recursion /*= false*/,
                               bool remove_empty_dir /*= false*/)
{
    if (!path)
    {
        return false;
    }

    char buffer[BUFFER_SIZE] = {0};
    strcpy(buffer, path);
    PathAppend(buffer, "*.*");

    WIN32_FIND_DATA stWFD = {0};
    HANDLE hFind = FindFirstFile(buffer, &stWFD);
    do
    {
        bool is_directory = (stWFD.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
        if ((is_directory && !recursion) || (is_directory && stWFD.cFileName[0] == '.'))
        {
            continue;
        }

        PathRemoveFileSpec(buffer);
        PathAppend(buffer, stWFD.cFileName);

        if (is_directory)
        {
            delete_files_in_directory(buffer, recursion, remove_empty_dir);
            if (remove_empty_dir)
            {
                rmdir(buffer);
            }
        }
        else
        {
            remove(buffer);
        }
    }
    while (FindNextFile(hFind, &stWFD) != 0);

    return FindClose(hFind);
}

bool delete_files_in_directory(LPCSTR path, LPCSTR filter, bool recursion /*= false*/)
{
    if (!path)
    {
        return false;
    }

    char buffer[BUFFER_SIZE] = {0};
    strcpy(buffer, path);
    PathAppend(buffer, "*.*");

    WIN32_FIND_DATA stWFD = {0};
    HANDLE hFind = FindFirstFile(buffer, &stWFD);
    do
    {
        bool is_directory = (stWFD.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
        if ((is_directory && !recursion) || (is_directory && stWFD.cFileName[0] == '.'))
        {
            continue;
        }

        PathRemoveFileSpec(buffer);
        PathAppend(buffer, stWFD.cFileName);

        if (is_directory)
        {
            delete_files_in_directory(buffer, filter, recursion);
        }
        else if (path_match_spec(buffer, filter))
        {
            remove(buffer);
        }
    }
    while (FindNextFile(hFind, &stWFD) != 0);

    return FindClose(hFind);
}

bool remove_empty_directory(LPCSTR path)
{
    if (!path)
    {
        return false;
    }

    char buffer[BUFFER_SIZE] = {0};
    strcpy(buffer, path);
    PathAppend(buffer, "*.*");

    WIN32_FIND_DATA stWFD = {0};
    HANDLE hFind = FindFirstFile(buffer, &stWFD);
    do
    {
        bool is_directory = (stWFD.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
        if (is_directory && stWFD.cFileName[0] == '.')
        {
            continue;
        }

        PathRemoveFileSpec(buffer);
        PathAppend(buffer, stWFD.cFileName);

        if (is_directory)
        {
            remove_empty_directory(buffer);
        }
    }
    while (FindNextFile(hFind, &stWFD) != 0);

    if (PathIsDirectory(path) && path_file_count(path, true) == 0)
    {
        rmdir(path);
    }
    return FindClose(hFind);
}

size_t path_file_count(LPCSTR path, bool recursion/* = false*/)
{
    if (!path)
    {
        return 0;
    }

    size_t size = 0;
    char buffer[BUFFER_SIZE] = {0};
    strcpy(buffer, path);
    PathAppend(buffer, "*.*");

    WIN32_FIND_DATA stWFD = {0};
    HANDLE hFind = FindFirstFile(buffer, &stWFD);
    do
    {
        bool is_directory = (stWFD.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
        if ((is_directory && !recursion) || (is_directory && stWFD.cFileName[0] == '.'))
        {
            continue;
        }

        PathRemoveFileSpec(buffer);
        PathAppend(buffer, stWFD.cFileName);

        if (is_directory)
        {
            size += path_file_count(buffer, recursion);
        }
        else
        {
            ++size;
        }
    }
    while (FindNextFile(hFind, &stWFD) != 0);

    FindClose(hFind);
    return size;
}

bool copy_directory(LPCSTR from, LPCSTR to, bool recursion /*= false*/, bool cover /*= false*/)
{
    if (!from || !to)
    {
        return false;
    }

    char buffer_from[BUFFER_SIZE] = {0};
    strcpy(buffer_from, from);
    PathAppend(buffer_from, "*.*");

    char buffer_to[BUFFER_SIZE] = {0};
    strcpy(buffer_to, to);
    PathAppend(buffer_to, "*.*");

    if (PathIsDirectory(from) && !PathIsDirectory(to))
    {
        mkdir(to);
    }

    WIN32_FIND_DATA stWFD = {0};
    HANDLE hFind = FindFirstFile(buffer_from, &stWFD);
    do
    {
        bool is_directory = (stWFD.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
        if ((is_directory && !recursion) || (is_directory && stWFD.cFileName[0] == '.'))
        {
            continue;
        }

        PathRemoveFileSpec(buffer_from);
        PathAppend(buffer_from, stWFD.cFileName);

        PathRemoveFileSpec(buffer_to);
        PathAppend(buffer_to, stWFD.cFileName);

        if (is_directory)
        {
            copy_directory(buffer_from, buffer_to, recursion, cover);
        }
        else if (cover || !PathFileExists(buffer_to))
        {
            copy_file(buffer_from, buffer_to);
        }
    }
    while (FindNextFile(hFind, &stWFD) != 0);

    return FindClose(hFind);
}

size_t ls_path(LPCSTR path, std::vector<std::string> & vec, LPCSTR filter /*= ""*/,
               bool recursion /*= false*/, bool full_path /*= true*/)
{
    if (!path)
    {
        return false;
    }

    char buffer[BUFFER_SIZE] = {0};
    strcpy(buffer, path);
    PathAppend(buffer, "*.*");

    WIN32_FIND_DATA stWFD = {0};
    HANDLE hFind = FindFirstFile(buffer, &stWFD);
    do
    {
        bool is_directory = (stWFD.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
        if ((is_directory && !recursion) || (is_directory && stWFD.cFileName[0] == '.'))
        {
            continue;
        }

        PathRemoveFileSpec(buffer);
        PathAppend(buffer, stWFD.cFileName);

        if (is_directory)
        {
            ls_path(buffer, vec, filter, recursion, full_path);
        }
        else if (path_match_spec(stWFD.cFileName, filter))
        {
            vec.push_back(full_path ? buffer : stWFD.cFileName);
        }
    }
    while (FindNextFile(hFind, &stWFD) != 0);

    FindClose(hFind);
    return !vec.empty();
}

bool copy_file(LPCSTR from, LPCSTR to)
{
    if (!from || !to)
    {
        return false;
    }

    std::ifstream ifs(from, std::ios::binary);
    std::ofstream ofs(to, std::ios::binary | std::ios::trunc);

    if (!ifs || !ofs)
    {
        return false;
    }

#if 0000
    char buffer[BLOCK_BUFFER_SIZE] = {0};
    while (ifs && ofs)
    {
        ifs.read(buffer, BLOCK_BUFFER_SIZE);
        ofs.write(buffer, ifs.gcount());
    }

    ifs.close();
    ofs.close();
#else
    ofs << ifs.rdbuf();
#endif
    return true;
}

std::string WAPIGetCurrentDir()
{
    char buffer[BUFFER_SIZE] = { 0 };
    ::GetCurrentDirectory(BUFFER_SIZE, buffer);
    return buffer;
}

std::string WAPIGetCurrentFileName()
{
    TCHAR buffer[BUFFER_SIZE] = { 0 };
    ::GetModuleFileName(NULL, buffer, BUFFER_SIZE);

    PathStripPath(buffer);
    return buffer;
}

std::string WAPIGetCurrentFileNameWithoutExtension()
{
    TCHAR buffer[BUFFER_SIZE] = { 0 };
    ::GetModuleFileName(NULL, buffer, BUFFER_SIZE);

    PathStripPath(buffer);
    PathRemoveExtension(buffer);
    return buffer;
}

std::string WAPIDosDevicePath2LogicalPath(LPCSTR path)
{
    std::string strResult;

    // Translate path with device name to drive letters.
    TCHAR szTemp[BUFFER_SIZE] = {0};
    if (path == NULL || !GetLogicalDriveStrings(_countof(szTemp) - 1, szTemp))
    {
        return strResult;
    }

    TCHAR szName[BUFFER_SIZE] = {0};
    TCHAR szDrive[3] = TEXT(" :");
    BOOL bFound = FALSE;
    TCHAR * p = szTemp;

    do
    {
        // Copy the drive letter to the template string
        *szDrive = *p;

        // Look up each device name
        if (QueryDosDevice(szDrive, szName, _countof(szName)))
        {
            UINT uNameLen = (UINT)_tcslen(szName);
            if (uNameLen < BUFFER_SIZE)
            {
                bFound = _tcsnicmp(path, szName, uNameLen) == 0;

                if (bFound)
                {
                    // Reconstruct pszFilename using szTemp
                    // Replace device path with DOS path
                    TCHAR szTempFile[BUFFER_SIZE];
                    _stprintf_s(szTempFile, TEXT("%s%s"), szDrive, path + uNameLen);
                    strResult = szTempFile;
                }
            }
        }

        // Go to the next NULL character.
        while (*p++);
    }
    while (!bFound && *p);   // end of string

    return strResult;
}

UniMode DetermineCharEncoding(const char * buf, size_t len)
{
    return (buf == NULL || len == 0) ? uniUnkown : determineEncoding((unsigned char *)buf, (int)len);
}

UniMode DetermineFileEncoding(const char * path)
{
    return path == NULL ? uniUnkown : determineFileEncode(path);
}

size_t UCS2LengthFromUTF8(const char * s, size_t len)
{
    return UCS2Length(s, len);
}

size_t UTF8LengthFromUCS2(const wchar_t * s, size_t len)
{
    return UTF8Length(s, len);
}
