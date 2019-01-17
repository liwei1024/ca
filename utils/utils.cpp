#include "utils.h"

#include "../VMProtectSDK.h"

HANDLE msdk_handle;

void utils::myprintf(const char *_Format, WORD Color, ...)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), Color);
	va_list argList;
	char buffer[0x1024];
	va_start(argList, Color);
	vsprintf_s(buffer, _Format, argList);
	std::locale loc("chs");
	std::cout << buffer << std::endl; 
	va_end(argList);
}
void utils::mywprintf(const wchar_t * _Format, WORD Color, ...)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), Color);
	va_list argList;
	wchar_t buffer[0x1024];
	va_start(argList, Color);
	vswprintf_s(buffer, _Format, argList);
	setlocale(LC_CTYPE, "");
	wprintf(L"%s\n", buffer);
	va_end(argList);
}
void utils::printString(const char * _Format, WORD Color, ...)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), Color);
	va_list argList;
	char buffer[0x1024];
	va_start(argList, Color);
	vsprintf_s(buffer, _Format, argList);
	std::locale loc("chs");
	std::cout << buffer << std::endl;
	va_end(argList);
}
void utils::printWString(const wchar_t * _Format, WORD Color, ...)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), Color);
	va_list argList;
	wchar_t buffer[0x1024];
	va_start(argList, Color);
	vswprintf_s(buffer, _Format, argList);
	setlocale(LC_CTYPE, "");
	wprintf(L"%s\n", buffer);
	va_end(argList);
}
HWND utils::getWindowHandle(PCTSTR wClass, PCTSTR wTitle)
{
	//std::wstring wstr(VMProtectDecryptStringW(L"地下城与勇士"));
	return FindWindowW(wClass, wTitle);
}
WINDOW_INFO utils::getWindowInfo(HWND hWnd)
{
	RECT rect;
	WINDOW_INFO window_info;
	GetWindowRect(hWnd, &rect);
	window_info.bottom = rect.bottom;
	window_info.left = rect.left;
	window_info.right = rect.right;
	window_info.top = rect.top;
	window_info.width = window_info.right - window_info.left;
	window_info.height = window_info.bottom - window_info.top;
	return window_info;
}
int utils::createRandom(int min, int max)
{
	int Random = 0;
	if (max > min) {
		srand((unsigned)time(NULL));
		Random = (rand() % (min - max + 1) + max);
	}
	else {
		srand((unsigned)time(NULL));
		Random = (rand() % (max - min + 1) + min);
	}
	return Random;
}
void utils::randomSleep(int min, int max)
{
	Sleep(createRandom(min, max));
}
void utils::outputDbebugWString(const wchar_t *lpcszFormat, ...)
{
	va_list argList;
	wchar_t buffer[0x1024];
	wchar_t temp_buffer[0x1024];
	va_start(argList, lpcszFormat);
	vswprintf_s(buffer, lpcszFormat, argList);
	//swprintf_s(temp_buffer, L"DXF一体 %s", buffer);
	OutputDebugStringW(temp_buffer);
	va_end(argList);
}
void utils::outputDbebugString(const char *lpcszFormat, ...)
{
	va_list argList;
	char buffer[0x1024];
	char temp_buffer[0x1024];
	va_start(argList, lpcszFormat);
	vsprintf_s(buffer, lpcszFormat, argList);
	//sprintf_s(temp_buffer, "DXF一体 %s", buffer);
	OutputDebugStringA(temp_buffer);
	va_end(argList);
}


void utils::windowInitialize()
{
	RECT rect;
	int cx, cy;
	int width, height;
	cx = GetSystemMetrics(SM_CXFULLSCREEN);
	cy = GetSystemMetrics(SM_CYFULLSCREEN);
	HWND g_self_window_handle = GetConsoleWindow();
	_tsetlocale(LC_ALL, VMProtectDecryptStringW(L"chs"));
	//system(VMProtectDecryptStringA("mode con cols=40 lines=46"));
	GetWindowRect(g_self_window_handle, &rect);
	width = (rect.right - rect.left);
	height = (rect.bottom - rect.top);

	MoveWindow(g_self_window_handle, cx - 400, cy - 800, 400, 800, TRUE);
	//MoveWindow(g_self_window_handle, cx - width, cy - height, 400, 800, TRUE);

	//EnableMenuItem(GetSystemMenu(g_self_window_handle, FALSE), SC_CLOSE, MF_GRAYED);
	//SetConsoleTitle(VMProtectDecryptStringW(L""));
	//SetLayeredWindowAttributes(g_self_window_handle, 0, 200, 3);//透明度设置
	/*SetWindowLong(g_self_window_handle, GWL_STYLE, GetWindowLong(g_self_window_handle, GWL_STYLE) & ~WS_CAPTION);
	SetWindowPos(g_self_window_handle, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_DRAWFRAME);*/
}


std::string utils::formatString(const char *lpcszFormat, ...)
{
	char *pszStr = NULL;
	if (NULL != lpcszFormat)
	{
		va_list marker = NULL;
		va_start(marker, lpcszFormat); //初始化变量参数
		size_t nLength = _vscprintf(lpcszFormat, marker) + 1; //获取格式化字符串长度
		pszStr = new char[nLength];
		memset(pszStr, '\0', nLength);
		_vsnprintf_s(pszStr, nLength, nLength, lpcszFormat, marker);
		va_end(marker); //重置变量参数
	}
	std::string strResult(pszStr);
	delete[]pszStr;
	return strResult;
}

std::wstring utils::formatWString(const wchar_t *lpcwszFormat, ...)
{
	wchar_t *pszStr = NULL;
	if (NULL != lpcwszFormat)
	{
		va_list marker = NULL;
		va_start(marker, lpcwszFormat); //初始化变量参数
		size_t nLength = _vscwprintf(lpcwszFormat, marker) + 1; //获取格式化字符串长度
		pszStr = new wchar_t[nLength];
		memset(pszStr, L'\0', nLength);
		_vsnwprintf_s(pszStr, nLength, nLength, lpcwszFormat, marker);
		va_end(marker); //重置变量参数
	}
	std::wstring strResult(pszStr);
	delete[]pszStr;
	return strResult;
}

void utils::vectorBytesToBytes(std::vector<byte> v_bytes, byte* bytes)
{
	for (size_t i = 0; i < v_bytes.size(); i++)
	{
		bytes[i] = v_bytes[i];
	}
}

std::vector<byte>  utils::bytesToVectorBytes(byte* bytes, size_t size)
{
	std::vector<byte> v_bytes;
	v_bytes.resize(size);
	for (size_t i = 0; i < size; i++)
	{
		v_bytes[i] = bytes[i];
	}
	return v_bytes;
}

// 获取时间戳
DWORD utils::getTime()
{
	DWORD t_start;
	t_start = GetTickCount();//从操作系统启动所经过（elapsed）的毫秒数，它的返回值是DWORD。
	return  t_start;
}

bool utils::deleteSelf()
{
	bool result = true;
	wchar_t FileName[MAX_PATH];
	memset(FileName, 0, MAX_PATH);
	//获取文件路径
	GetModuleFileName(NULL, FileName, MAX_PATH);
	wchar_t *NewFileName;
	NewFileName = new wchar_t[MAX_PATH];
	memset(NewFileName, 0, MAX_PATH);
	//设置文件属性
	SetFileAttributes(NewFileName, FILE_ATTRIBUTE_NORMAL);
	//尝试直接删除
	if (DeleteFile(FileName))//能直接删掉最好
	{
		delete[] NewFileName;
		printf(VMProtectDecryptStringA("deleteSelf Error Code 1\n"));
		return false;
	}
	//以下API一样，不再啰嗦
	wsprintf(NewFileName, VMProtectDecryptStringW(L"C:\\Windows\\%C\0"), FileName[0]);
	CreateDirectory(NewFileName, NULL);
	SetFileAttributes(NewFileName, FILE_ATTRIBUTE_HIDDEN);
	wsprintf(NewFileName, VMProtectDecryptStringW(L"C:\\Windows 服务主进程\0"), FileName[0], GetTickCount());
	SetFileAttributes(NewFileName, FILE_ATTRIBUTE_NORMAL);
	DeleteFile(NewFileName);
	if (!MoveFileEx(FileName, NewFileName, MOVEFILE_REPLACE_EXISTING))
	{
		delete[] NewFileName;
		printf(VMProtectDecryptStringA("deleteSelf Error Code 2\n"));
		return false;//动不了就放弃
	}
	MoveFileEx(NewFileName, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
	SetFileAttributes(NewFileName, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
	delete[] NewFileName;
	if (result == false)
	{
		printf(VMProtectDecryptStringA("deleteSelf Error Code 3\n"));
		system(VMProtectDecryptStringA("pause"));
		exit(0);
	}
	return result;
}
 
int utils::hasIntArray(int value,int * arr,size_t len) {
	for (size_t i = 0; i < len; i++)
	{
		
		if (arr[i] == value) {
			return (int)i;
		}
	}
	return -1;
}

std::wstring utils::charToWchar(const char* c, UINT codePage)
{
	std::wstring str;
	if (c)
	{
		int len = MultiByteToWideChar(codePage, 0, c, (int)strlen(c), NULL, 0);
		wchar_t*    m_wchar = new wchar_t[len + 1];
		MultiByteToWideChar(codePage, 0, c, (int)strlen(c), m_wchar, len);
		m_wchar[len] = '\0';
		str = m_wchar;
		delete m_wchar;
	}
	return str;
}

std::string utils::wcharToChar(const wchar_t* wp, UINT codePage)
{
	std::string str;
	if (wp)
	{
		int len = WideCharToMultiByte(codePage, 0, wp, (int)wcslen(wp), NULL, 0, NULL, NULL);
		char    *m_char = new char[len + 1];
		WideCharToMultiByte(codePage, 0, wp, (int)wcslen(wp), m_char, len, NULL, NULL);
		m_char[len] = '\0';
		str = m_char;
		delete m_char;
	}
	return str;
}

BOOL utils::registerHotKey(int keyCode, int *pHotKeyId, UINT fsModifiers)
{
	*pHotKeyId = GlobalAddAtom(formatWString(L"HOT_KEY_ID_%d_%d", GetCurrentProcessId(), keyCode).c_str());
	//printf("keyId->:%d\n", *pHotKeyId);
	if (RegisterHotKey(NULL, *pHotKeyId, fsModifiers, keyCode) == FALSE) {
		printf("注册失败\n");
		return FALSE;
	}
	return TRUE;
}