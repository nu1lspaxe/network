#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int __cdecl main(int argc, char** argv)
{
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo* result = NULL, * ptr = NULL, hints;
	char SendBuf[DEFAULT_BUFLEN];
	char RecvBuf[DEFAULT_BUFLEN];
	int iResult;

	// Validate the parameters
	if (argc != 2)
	{
		wprintf(L"usage: %S server-name\n", argv[0]);
		return 1;
	}

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		wprintf(L"WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		wprintf(L"getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET)
		{
			wprintf(L"socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		// Connect to server
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET)
	{
		wprintf(L"Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	do 
	{
		wprintf(L"Enter message (type 'exit' to close): ");
		fgets(SendBuf, sizeof(SendBuf), stdin);

		// Check if the user wants to exit
		if (strncmp(SendBuf, "exit", 4) == 0)
			break;

		// Send message to server
		iResult = send(ConnectSocket, SendBuf, (int)strlen(SendBuf), 0);
		if (iResult == SOCKET_ERROR)
		{
			wprintf(L"send failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}

		// Receive message from server
		iResult = recv(ConnectSocket, RecvBuf, DEFAULT_BUFLEN, 0);
		if (iResult > 0)
		{
			RecvBuf[strcspn(RecvBuf, "\n")] = 0;
			wprintf(L"Message received: %S\n", RecvBuf);
		}
		else if (iResult == 0)
		{
			wprintf(L"Connection closed\n");
		}
		else
		{
			wprintf(L"recv failed with error: %d\n", WSAGetLastError());
		}
	} while (iResult > 0);


	// shutdown the connection since no more data will be sent
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		wprintf(L"shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	// Receive until the peer closes the connection
	do {

		iResult = recv(ConnectSocket, RecvBuf, DEFAULT_BUFLEN, 0);
		if (iResult > 0)
			wprintf(L"Bytes received: %d\n", iResult);
		else if (iResult == 0)
			wprintf(L"Connection closed\n");
		else
			wprintf(L"recv failed with error: %d\n", WSAGetLastError());

	} while (iResult > 0);

	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();

	return 0;
}