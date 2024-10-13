#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib, "WS2_32")

// InetPton, in ANSI is inet_pton. To convert an IP address from text to binary.
// INT WSAAPI InetPton(
//   _In_  INT   Family,
//   _In_  PCTSTR pszAddrString,
//   _Out_ PVOID pAddrBuf
// );
// Family: AF_INET or AF_INET6
// pszAddrString: A pointer to the textual IP address to convert.
// pAddrBuf: A pointer to a buffer that receives the address in network byte order.

// Server side must in listening mode (accept)
// 
// INT WSAAPI connect(
//   _In_ SOCKET         s,
//   _In_ const sockaddr *name,
//   _In_ int            namelen
// );

int __cdecl main(int argc, char** argv)
{
	// Initialize 
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	SOCKET s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Get server location
	SOCKADDR_IN sockAddr{};
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(827);
	inet_pton(AF_INET, "127.0.0.1", (void*)&sockAddr.sin_addr.S_un.S_addr);

	// Connect server
	connect(s, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));

	// Send message
	char szMessage[] = "Give me money";
	send(s, szMessage, strlen(szMessage) + sizeof(char), 0);

	// Receive message
	char szBuffer[MAXBYTE] = { 0 };
	recv(s, szBuffer, MAXBYTE, NULL);
	printf("szBuffer = %s \r\n", szBuffer);

	// Close connection
	shutdown(s, 0);
	closesocket(s);
	WSACleanup();

	system("pause");
	return 0;
}
