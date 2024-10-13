#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

// UDP client is not binding to server (refers to connect() in TCP), 
// server address need to be specified in sendto()
// 
// int sendto(
//	// Socket descriptor
//   _In_       SOCKET s,
// 
//	// A pointer to a buffer containing the data to be transmitted		
//   _In_ const char   *buf,	
// 
//	// The length of the buffer
//   _In_       int    len,
// 
//	// Flags to modify the behavior of sendto()
//   _In_       int    flags,
// 
//  // A pointer to a sockaddr structure that contains the destination address
//   _In_ const struct sockaddr *to,
// 
// // The length of the sockaddr structure, which is sizeof(sockaddr) for IPv4
//   _In_       int    tolen
// );

int main()
{
	// Basic setup for sending/receiving data using Winsock library.

	// Store the return value of Winsock functions.
	int iResult{ 0 };

	// WSADATA contains information about the Windows Sockets implementation.
	WSADATA wsaData;

	// SOCKET is a socket descriptor for sending data.
	SOCKET SendSocket;

	// sockaddr_in defines an IPv4 address.
	sockaddr_in RecvAddr;

	// Port number for the server.
	unsigned short Port = 27015;

	// Buffer to store incoming data.
	char SendBuf[1024];
	int BufLen = 1024;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		wprintf(L"WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	// Create a socket for sending data
	SendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (SendSocket == INVALID_SOCKET)
	{
		wprintf(L"socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// Set up the RecvAddr structure with the IP address of
	// the receiver and the specified port number.
	RecvAddr.sin_family = AF_INET;
	// htons() stands for host to network short.
	RecvAddr.sin_port = htons(Port);

	// Prompt the user to enter the IP address of the receiver.
	char ipAddr[INET_ADDRSTRLEN];
	while (1) {
		printf("Enter the IP address of the receiver: ");
		fgets(ipAddr, sizeof(ipAddr), stdin);
		ipAddr[strcspn(ipAddr, "\n")] = 0; // Remove newline character

		// Convert the IP address from text to binary form
		iResult = inet_pton(AF_INET, ipAddr, (void*)&RecvAddr.sin_addr.S_un.S_addr);
		if (iResult == 1) {
			break; // Valid IP address
		}
		else {
			if (iResult == 0) {
				printf("Invalid IP address format. Please try again.\n");
			}
			else {
				printf("inet_pton failed with error: %d\n", WSAGetLastError());
			}
		}
	}

	// inet_pton() stands for Internet Protocol Address, Presentation to Network.
	// It converts an IPv4 address from text to binary form.
	// inet_pton(AF_INET, "<IPv4>", (void*)&RecvAddr.sin_addr.S_un.S_addr);


	while (1)
	{
		wprintf(L"Enter a message to send (type 'exit' to quit): ");
		fgets(SendBuf, BufLen, stdin);

		// Remove the newline character from the input buffer.
		SendBuf[strcspn(SendBuf, "\n")] = 0;

		if (strcmp(SendBuf, "exit") == 0)
		{
			break;
		}
		
		// Send a datagram to the receiver
		iResult = sendto(SendSocket, SendBuf, (int)strlen(SendBuf), 0, (SOCKADDR*)&RecvAddr, sizeof(RecvAddr));
		if (iResult == SOCKET_ERROR)
		{
			wprintf(L"sendto failed with error: %d\n", WSAGetLastError());
			closesocket(SendSocket);
			WSACleanup();
			return 1;
		}
	}

	// When the application is finished sending, close the socket.
	wprintf(L"Finished sending. Closing socket.\n");
	iResult = closesocket(SendSocket);
	if (iResult == SOCKET_ERROR)
	{
		wprintf(L"closesocket failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// Clean up and exit
	wprintf(L"Exiting.\n");
	WSACleanup();
	
	system("pause");
	return 0;
}