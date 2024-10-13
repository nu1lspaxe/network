
#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>

// Import library of winsocket2.2 -> "WS2_32", "WS2_32.lib", "ws2_32", "ws2_32.lib"
#pragma comment(lib, "WS2_32")

// __stdcall is convension for WinAPI, while __cdecl is default for C/C++

// int WSAStartup (
//		WORD wVersionRequired,
//		LPWSADATA lpWSAData
// );
// 
// wVersionRequired: used version of winsock, lpWSAData: pointer to WSADATA

// SOCKET WSAAPI socket (
//		int af,
//		int type,
//		int protocol
// );
//
// af: communication protocol, type: socket type, protocol: protocol

int __cdecl main(void)
{
	/* Initialize */
	// WSADATA contains information about Windows socket
	WSADATA wsaData;
	// Initialize Windows Socket, use winsock2.2
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	// Connect to WSADATA
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	// Set details of socket
	sockaddr_in sockaddr{};
	// AF_INET: TCP/IP
	sockaddr.sin_family = AF_INET;
	// If set to 0, the system will assign a port number (1024~5000)
	sockaddr.sin_port = htons(827);
	// INADDR_ANY: computer current IP
	sockaddr.sin_addr.S_un.S_addr = INADDR_ANY;

	// Bind port 
	bind(s, (SOCKADDR*)&sockaddr, sizeof(SOCKADDR));
	// Set the maximum length of peading/waiting connection
	listen(s, 1);
	SOCKADDR clientAddr{};
	int nSize = sizeof(SOCKADDR);
	SOCKET clientSock;

	// Get client-connected socket
	clientSock = accept(s, (SOCKADDR*)&clientAddr, &nSize);
	char inbuf[1024];

	// Receive data
	recv(clientSock, inbuf, sizeof(inbuf), NULL);
	// '\r\n' is '\n' in winodws
	printf("Receive: %s\r\n", inbuf);
	char outbuf[] = "Helloooo Networb 0w0";
	
	// Send data;
	send(clientSock, outbuf, strlen(outbuf) + sizeof(char), NULL);

	// Close the connection with client
	// If no shutdown, it will timeout()
	// shutdown(SOCKET, int) -> int: 0: stop receiving, 1: stop sending, 2: stop both
	shutdown(clientSock, 0);
	closesocket(clientSock);

	// Close server
	closesocket(s);
	// Stop using Winsock 2 DLL (WS2_32.dll)
	WSACleanup();

	system("pause");
	return 0;
}