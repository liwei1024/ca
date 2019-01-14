#include "pch.h"

#define SLEEP_TIME 5000 //间隔时间

#define FILE_PATH "C:\\log.txt" //信息输出文件

bool brun = false;

SERVICE_STATUS servicestatus;

SERVICE_STATUS_HANDLE hstatus;

int WriteToLog(char* str);

void WINAPI ServiceMain(int argc, char** argv);

void WINAPI CtrlHandler(DWORD request);

int InitService();




int WriteToLog(char* str)

{

	FILE* pfile;

	fopen_s(&pfile, FILE_PATH, "a+");

	if (pfile == NULL)

	{
		return -1;
	}

	fprintf_s(pfile, "%s\n", str);

	fclose(pfile);

	return 0;

}

VOID ThreadMessage(
	MSG* msg
) {
	WriteToLog((char*)"ThreadMessage");
}

void WINAPI ServiceMain(int argc, char** argv)
{

	servicestatus.dwServiceType = SERVICE_WIN32;

	servicestatus.dwCurrentState = SERVICE_START_PENDING;

	servicestatus.dwControlsAccepted = SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_STOP;//在本例中只接受系统关机和停止服务两种控制命令

	servicestatus.dwWin32ExitCode = 0;

	servicestatus.dwServiceSpecificExitCode = 0;

	servicestatus.dwCheckPoint = 0;

	servicestatus.dwWaitHint = 0;

	hstatus = ::RegisterServiceCtrlHandler(L"testservice", CtrlHandler);

	if (hstatus == 0)
	{

		WriteToLog((char*)"RegisterServiceCtrlHandler failed");

		return;

	}

	WriteToLog((char*)"RegisterServiceCtrlHandler success");

	//向SCM 报告运行状态

	servicestatus.dwCurrentState = SERVICE_RUNNING;

	SetServiceStatus(hstatus, &servicestatus);

	MessageBox(NULL,L"Services",L"Services",MB_OK);

	char buffer[1024] = { 0 };
	sprintf_s(buffer,"CurrentThreadId->:%d",GetCurrentThreadId());
	WriteToLog(buffer);
	//在此处添加你自己希望服务做的工作，在这里我做的工作是获得当前可用的物理和虚拟内存信息
	MSG msg = { 0 };//消息指针
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);//等待信号
		DispatchMessage(&msg);//处理信号
		ThreadMessage(&msg);//线程消息处理
	}
	//DWORD dwThread = GetWindowThreadProcessId(GetForegroundWindow(), NULL);

	//// 将前台窗口线程贴附到当前线程（也就是程序A中的调用线程）
	//AttachThreadInput(dwThread, GetCurrentThreadId(), TRUE);
	//AttachThreadlnput();
	/*char buffer[1024] = {0};
	while (true)
	{
		if (GetKeyState(VK_HOME) != 0)
		{
			sprintf_s(buffer, "GetKeyState %d", GetKeyState(VK_HOME));
			WriteToLog(buffer);
		}
		if (GetAsyncKeyState(VK_HOME) != 0)
		{
			sprintf_s(buffer, "GetAsyncKeyState %d", GetAsyncKeyState(VK_HOME));
			WriteToLog(buffer);
		}
		
	}*/
	//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)test, NULL, 0, NULL);
	/*brun = true;

	MEMORYSTATUS memstatus;    

	char str[100];

	memset(str, '\0', 100);

	while (brun)
	{
		GlobalMemoryStatus(&memstatus);
		int availmb = int(memstatus.dwAvailPhys / 1024 / 1024);
		sprintf_s(str, 100, "available memory is %dMB", availmb);
		WriteToLog(str);
		Sleep(SLEEP_TIME);
	}
	char logStr[] = "service stopped";
	WriteToLog(logStr);*/
}


void WINAPI CtrlHandler(DWORD request)
{

    switch (request)
    {
    case SERVICE_CONTROL_STOP:

        brun=false;

        servicestatus.dwCurrentState = SERVICE_STOPPED;

        break;

    case SERVICE_CONTROL_SHUTDOWN:

        brun=false;

        servicestatus.dwCurrentState = SERVICE_STOPPED;

        break;

    default:

        break;

    }

    SetServiceStatus (hstatus, &servicestatus);
}

int main()
{
	
    SERVICE_TABLE_ENTRY entrytable[2];

    entrytable[0].lpServiceName=LPWSTR("testservice");

    entrytable[0].lpServiceProc=(LPSERVICE_MAIN_FUNCTION)ServiceMain;

    entrytable[1].lpServiceName=NULL;

    entrytable[1].lpServiceProc=NULL;

    StartServiceCtrlDispatcher(entrytable);
	
	return 0;
}
