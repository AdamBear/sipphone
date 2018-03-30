//只针对Windows平台有用
#ifdef WIN32
#include <assert.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windef.h>
#include <winnt.h>
#include <windows.h>
#include <wininet.h>

//#include<winbase.h>
typedef struct {
	/* name of current directory entry (a multi-byte character string) */
	char d_name[MAX_PATH + 1];

	/* file attributes */
	WIN32_FIND_DATA data;
} dirent;

typedef struct {
	/* current directory entry */
	dirent current;

	/* is there an un-processed entry in current? */
	int cached;

	/* file search handle */
	HANDLE search_handle;

	/* search pattern (3 = zero terminator + pattern "\\*") */
	TCHAR patt[MAX_PATH + 3];
} DIR;


DIR *opendir (const char *dirname);
struct dirent *readdir (DIR *dirp);
int closedir (DIR *dirp);
#endif
