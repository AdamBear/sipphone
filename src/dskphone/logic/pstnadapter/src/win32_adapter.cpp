#include "win32_adapter.h"

#if WIN32

void COMMONUNITS_WIN32_LOG(const char * szMessage)
{
    DWORD dwLastError = GetLastError();
    LPSTR szError = nullptr;
    size_t messageSize = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                                       FORMAT_MESSAGE_IGNORE_INSERTS,
                                       NULL, dwLastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&szError, 0, NULL);
    COMMONUNITS_WARN("%s : [%d]%s", szMessage, dwLastError, szError);
    LocalFree(szError);
}

#define IO_TIMEOUT 1000 // IO异步操作超时时间(ms)
#define IO_RETRY_TIMES 10 // IO读取重试次数
#define IO_RETRY_INTERVAL 5 // IO读取重试间隔(ms)
size_t read(int fd, char * buf, size_t count)
{
    ZeroMemory(buf, count);
    DWORD nRead = 0, nOffset = 0;
    OVERLAPPED oOverlapped;
    ZeroMemory(&oOverlapped, sizeof(OVERLAPPED));
    oOverlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    for (size_t i = 0; i < IO_RETRY_TIMES; ++i)
    {
        if (!ReadFile((HANDLE)fd, buf + nOffset, count - nOffset, &nRead, &oOverlapped))
        {
            if (GetLastError() != ERROR_IO_PENDING
                    || WaitForSingleObject(oOverlapped.hEvent, IO_TIMEOUT) != WAIT_OBJECT_0)
            {
                continue;
            }
            if (HasOverlappedIoCompleted(&oOverlapped))
            {
                nRead = oOverlapped.InternalHigh;
            }
        }
        nOffset += nRead;
        if (nRead > 0 && strchr(buf, '\n'))
        {
            CloseHandle(oOverlapped.hEvent);
            return nOffset;
        }
        Sleep(IO_RETRY_INTERVAL);
    }
    // 操作失败，使用GetLastError获取失败信息
    COMMONUNITS_WIN32_LOG("Read from PSTN Box fail");

    CancelIoEx((HANDLE)fd, &oOverlapped);
    CloseHandle(oOverlapped.hEvent);
    return nOffset;
}

int write(int fd, const char * line, long size)
{
    //for (size_t i = 0; i < size; ++i)
    //{
    //  COMMONUNITS_DEBUG("write to #%d: 0x%p[%d]=0x%02X", fd, line, i, line[i]);
    //}

    DWORD nSent = 0;
    OVERLAPPED oOverlapped;
    ZeroMemory(&oOverlapped, sizeof(oOverlapped));
    oOverlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (WriteFile((HANDLE)fd, line, size, &nSent, &oOverlapped))
    {
        CloseHandle(oOverlapped.hEvent);
        return nSent;
    }
    if (GetLastError() == ERROR_IO_PENDING)
    {
        if (WaitForSingleObject(oOverlapped.hEvent, IO_TIMEOUT) == WAIT_OBJECT_0)
        {
            size_t nResult = oOverlapped.InternalHigh;
            CloseHandle(oOverlapped.hEvent);
            return nResult;
        }
        COMMONUNITS_WARN("Send to PSTN Box timeout!");
    }
    // 操作失败，使用GetLastError获取失败信息
    COMMONUNITS_WIN32_LOG("Send to PSTN Box fail");

    CancelIoEx((HANDLE)fd, &oOverlapped);
    CloseHandle(oOverlapped.hEvent);
    return -1;
}

#endif // WIN32