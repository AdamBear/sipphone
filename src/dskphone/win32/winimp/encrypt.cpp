#include "common.h"
#include "sha256.h"
#include "compress.h"
#include <wincrypt.h>

#if 0
#   define ENCRYPT_ALGORITHM    CALG_RC2
#   define ENCRYPT_BLOCK_SIZE   8
#else
#   define ENCRYPT_ALGORITHM    CALG_RC4
#   define ENCRYPT_BLOCK_SIZE   1
#endif

std::string WAPIGetUUID()
{
    UUID uuidRoute;
    RPC_STATUS st;
    std::string strUUID;

    st = UuidCreate(&uuidRoute);
    if (st == RPC_S_OK)
    {
        unsigned char * pszGuid;
        UuidToString(&uuidRoute, &pszGuid);
        strUUID = (char *)pszGuid;
        RpcStringFree(&pszGuid);
    }

    return strUUID;
}

std::string WAPISha256Imp(const std::string & data)
{
    sha256_state stats;
    memset(&stats, 0, sizeof(sha256_state));
    sha256_init(&stats);
    sha256_update(&stats, (const unsigned char *)data.c_str(), data.size());
    const int buffer_size = 32;
    unsigned char buffer[buffer_size] = {0};
    sha256_final(&stats, buffer);
    return std::string((char *)buffer, buffer_size);
}

std::string WAPISha256Hex(const std::string & data)
{
    return WAPIHash2Hex(WAPISha256Imp(data));
}

std::string WAPIHash2Hex(const std::string & data)
{
    std::string result;
    result.resize(data.size() * 2);

    LPCSTR lpHex = "0123456789abcdef";
    for (int i = 0; i < data.size(); ++i)
    {
        result[i * 2] = lpHex[(data[i] >> 4) & 0x0F];
        result[i * 2 + 1] = lpHex[data[i] & 0x0F];
    }

    return result;
}

std::string WAPIHashGen(const std::string & data, int need_length)
{
    std::string result;
    int flag = 0;

    while (result.size() < (need_length / 2))
    {
        int hash = 0;
        // 两种hash算法交替进行
        if (flag++ % 2)
        {
            hash = FNVHash1(data + result);
        }
        else
        {
            hash = RSHash(data + result);
        }
        result.append((char *)&hash, sizeof(int));
    }

    return WAPIHash2Hex(result);
}

int RSHash(const std::string & data)
{
    int b    = 378551;
    int a    = 63689;
    int hash = 0;

    for (int i = 0; i < data.size(); i++)
    {
        hash = hash * a + data[i];
        a    = a * b;
    }

    return (hash & 0x7FFFFFFF);
}

int FNVHash1(const std::string & data)
{
    int p = 16777619;
    int hash = 2166136261L;
    for (int i = 0; i < data.size(); i++)
    {
        hash = (hash ^ data[i]) * p;
    }
    hash += hash << 13;
    hash ^= hash >> 7;
    hash += hash << 3;
    hash ^= hash >> 17;
    hash += hash << 5;
    return hash;
}

int WAPIDecodeFileInfo(const std::string & data, std::string & key, size_t & size)
{
    LPCSTR lpData = data.c_str();
    // restore file size
    int head = sizeof(size_t);
    if (data.size() < head)
    {
        return -1;
    }
    size = *((size_t *)lpData);
    lpData += sizeof(size_t);

    // restore key length info
    head += sizeof(size_t);
    if (data.size() < head)
    {
        return -1;
    }
    size_t key_size = *((size_t *)lpData);
    lpData += sizeof(size_t);

    if (key_size > 0)
    {
        // restore key info
        head += key_size;
        if (data.size() < head)
        {
            return -1;
        }
        key.assign(lpData, key_size);
        lpData += key_size;
    }

    return head;
}

int WAPIEncodeFileInfo(const std::string & data, const std::string & key, std::string & result)
{
    // store file size
    size_t size = data.size();
    result.assign((char *)&size, sizeof(size_t));

    // store key info
    size = key.size();
    result.append((char *)&size, sizeof(size_t));

    // store key self
    if (size > 0)
    {
        result.append(key.c_str(), key.size());
    }
    return size + 2 * sizeof(size_t);
}

bool WAPICompress(const std::string & data, std::string & result)
{
    if (data.size() <= 48)
    {
        result = data;
        return true;
    }

    char * buffer = (char *)::GlobalAlloc(GPTR, max(data.size() * 2, 128));
    if (buffer == NULL)
    {
        return false;
    }

    int iRet = compress((void *)data.c_str(), data.size(), buffer);
    std::string out(buffer, iRet);
    // code file info
    WAPIEncodeFileInfo(data, WAPISha256Imp(out), result);
    // store compress data
    result.append(out.c_str(), out.size());
    ::GlobalFree(buffer);
    return iRet > 0;
}

bool WAPIUnCompress(const std::string & data, std::string & result)
{
    if (data.size() <= 48)
    {
        result = data;
        return true;
    }

    size_t size = 0;
    std::string key;
    // uncode file info
    int head = WAPIDecodeFileInfo(data, key, size);
    if (head <= 0)
    {
        return false;
    }

    char * buffer = (char *)::GlobalAlloc(GPTR, size);
    if (buffer == NULL)
    {
        return false;
    }

    std::string input(data.c_str() + head, data.size() - head);
    // sha256 check data
    if (WAPISha256Imp(input) != key)
    {
        return false;
    }

    int iRet = decompress((void *)input.c_str(), input.size(), buffer);
    result.assign(buffer, iRet);
    ::GlobalFree(buffer);
    return iRet > 0;
}

bool WAPICompressFile(const std::string & from, const std::string & to)
{
    if (from.empty() || to.empty())
    {
        return false;
    }

    std::string data, result;
    return dump_file_imp(from, data) && WAPICompress(data, result) && write_file_imp(to, result);
}

bool WAPIUnCompressFile(const std::string & from, const std::string & to)
{
    if (from.empty() || to.empty())
    {
        return false;
    }

    std::string data, result;
    return dump_file_imp(from, data) && WAPIUnCompress(data, result) && write_file_imp(to, result);
}

bool WAPIEncryptBuffer(const std::string & data, std::string & result, LPSTR szPassword /*= NULL*/)
{
    HCRYPTPROV hProv = 0;
    HCRYPTKEY hKey = 0;

    if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, 0))
    {
        return false;
    }

    std::string key;
    if (szPassword == NULL)
    {
        HCRYPTKEY hXchgKey = 0;
        DWORD dwKeyBlobLen = 0;

        CryptGenKey(hProv, ENCRYPT_ALGORITHM, CRYPT_EXPORTABLE, &hKey);
        CryptGetUserKey(hProv, AT_KEYEXCHANGE, &hXchgKey);
        CryptExportKey(hKey, hXchgKey, SIMPLEBLOB, 0, NULL, &dwKeyBlobLen);
        char * buffer = (char *)::GlobalAlloc(GPTR, dwKeyBlobLen);
        if (buffer == NULL)
        {
            return false;
        }

        CryptExportKey(hKey, hXchgKey, SIMPLEBLOB, 0, (BYTE *)buffer, &dwKeyBlobLen);
        CryptDestroyKey(hXchgKey);
        hXchgKey = 0;

        key.append(buffer, dwKeyBlobLen);
        ::GlobalFree(buffer);
    }
    else
    {
        HCRYPTHASH hHash = 0;
        CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash);
        CryptHashData(hHash, (PBYTE)szPassword, strlen(szPassword), 0);
        CryptDeriveKey(hProv, ENCRYPT_ALGORITHM, hHash, 0, &hKey);
        CryptDestroyHash(hHash);
        hHash = 0;
    }

    DWORD dwCount = data.size();
    DWORD dwBuffSize = dwCount + ENCRYPT_BLOCK_SIZE;
    char * buffer = (char *)::GlobalAlloc(GPTR, dwBuffSize);
    if (buffer == NULL)
    {
        return false;
    }

    memcpy(buffer, data.c_str(), data.size());
    bool bRet = CryptEncrypt(hKey, 0, 1, 0, (PBYTE)buffer, &dwCount, dwBuffSize);
    WAPIEncodeFileInfo(data, key, result);
    result.append(buffer, dwCount);
    ::GlobalFree(buffer);

    return bRet;
}

bool WAPIDecryptBuffer(const std::string & data, std::string & result, LPSTR szPassword /*= NULL*/)
{
    HCRYPTPROV hProv = 0;
    HCRYPTKEY hKey = 0;

    if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_FULL, 0))
    {
        return false;
    }

    std::string key;
    size_t size = 0;
    int head = WAPIDecodeFileInfo(data, key, size);
    if (head < 0)
    {
        return false;
    }

    if (szPassword == NULL)
    {
        CryptImportKey(hProv, (BYTE *)key.c_str(), key.size(), 0, 0, &hKey);
    }
    else
    {
        HCRYPTHASH hHash = 0;
        CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash);
        CryptHashData(hHash, (PBYTE)szPassword, strlen(szPassword), 0);
        CryptDeriveKey(hProv, ENCRYPT_ALGORITHM, hHash, 0, &hKey);
        CryptDestroyHash(hHash);
        hHash = 0;
    }

    char * buffer = (char *)::GlobalAlloc(GPTR, size + ENCRYPT_BLOCK_SIZE);
    if (buffer == NULL)
    {
        return false;
    }

    DWORD dwCount = data.size() - head;
    memcpy(buffer, data.c_str() + head, dwCount);
    bool bRet = CryptDecrypt(hKey, 0, 1, 0, (PBYTE)buffer, &dwCount);
    result.append(buffer, dwCount);
    ::GlobalFree(buffer);

    return bRet;
}

int WAPIRestoreBuferSize(const std::string & data)
{
    return data.size() < sizeof(size_t) ? -1 : *((size_t *)data.c_str());
}

bool WAPIEncryptFile(LPSTR szSource, LPSTR szDestination, LPSTR szPassword)
{
    if (szSource == NULL || szDestination == NULL)
    {
        return false;
    }

    std::string data, result;
    return dump_file_imp(szSource, data) && WAPIEncryptBuffer(data, result, szPassword)
           && write_file_imp(szDestination, result);
}

bool WAPIDecryptFile(LPSTR szSource, LPSTR szDestination, LPSTR szPassword)
{
    if (szSource == NULL || szDestination == NULL)
    {
        return false;
    }

    std::string data, result;
    return dump_file_imp(szSource, data) && WAPIDecryptBuffer(data, result, szPassword)
           && write_file_imp(szDestination, result);
}
