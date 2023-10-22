#pragma once

// WinSock2 Windows Sockets
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include <string>
#include <iostream>

#include "buffer.h"

struct PacketHeader
{
	uint32_t packetSize;
	uint32_t messageType;
};

struct ChatMessage
{
	PacketHeader header;
	uint32_t messageLength;
	std::string message;
};

// Need to link Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

// First, make it work (messy), then organize

#define DEFAULT_PORT "8412"

int main(int arg, char** argv)
{
	// Initialize WinSock
	WSADATA wsaData;
	int result;

	// Set version 2.2 with MAKEWORD(2,2)
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		printf("WSAStartup failed with error %d\n", result);
		return 1;
	}
	printf("WSAStartup successfully!\n");

	struct addrinfo* info = nullptr;
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));	// ensure we don't have garbage data 
	hints.ai_family = AF_INET;			// IPv4
	hints.ai_socktype = SOCK_STREAM;	// Stream
	hints.ai_protocol = IPPROTO_TCP;	// TCP
	hints.ai_flags = AI_PASSIVE;


	result = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &info);
	if (result != 0) {
		printf("getaddrinfo failed with error %d\n", result);
		WSACleanup();
		return 1;
	}
	printf("getaddrinfo successfully!\n");

	// Socket
	SOCKET serverSocket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
	if (serverSocket == INVALID_SOCKET) {
		printf("socket failed with error %d\n", WSAGetLastError());
		freeaddrinfo(info);
		WSACleanup();
		return 1;
	}
	printf("socket created successfully!\n");

	// Connect
	result = connect(serverSocket, info->ai_addr, (int)info->ai_addrlen);
	if (serverSocket == INVALID_SOCKET) {
		printf("connect failed with error %d\n", WSAGetLastError());
		closesocket(serverSocket);
		freeaddrinfo(info);
		WSACleanup();
		return 1;
	}
	printf("Connect to the server successfully!\n");
	
	bool cont = true;
	
	while (true)
	{
		ChatMessage message;
		std::string tempMessage;
		printf("Send Chat (#leave to leave room): ");
		std::getline(std::cin, tempMessage);
		if (tempMessage == "#leave")
		{
			tempMessage = "[has left room]";
			cont = false;
		}
		message.message = tempMessage;
		message.messageLength = message.message.length();
		message.header.messageType = 1;// Can use an enum to determine this
		message.header.packetSize =
			message.message.length()				// 5 'hello' has 5 bytes in it
			+ sizeof(message.messageLength)			// 4, uint32_t is 4 bytes
			+ sizeof(message.header.messageType)	// 4, uint32_t is 4 bytes
			+ sizeof(message.header.packetSize);	// 4, uint32_t is 4 bytes
		// 5 + 4 + 4 + 4 = 17

		const int bufSize = 512;
		Buffer buffer(bufSize);

		// Write our packet to the buffer
		buffer.WriteUInt32LE(message.header.packetSize);	// should be 17
		buffer.WriteUInt32LE(message.header.messageType);	// 1
		buffer.WriteUInt32LE(message.messageLength);		// 5
		buffer.WriteString(message.message);				// hello

		// Write
		result = send(serverSocket, (const char*)(&buffer.m_BufferData[0]), message.header.packetSize, 0);
		if (result == SOCKET_ERROR) {
			printf("send failed with error %d\n", WSAGetLastError());
			closesocket(serverSocket);
			freeaddrinfo(info);
			WSACleanup();
			return 1;
		}
		//// Write
		//result = send(serverSocket, (const char*)(&buffer.m_BufferData[0]), message.header.packetSize, 0);
		//if (result == SOCKET_ERROR) {
		//	printf("send failed with error %d\n", WSAGetLastError());
		//	closesocket(serverSocket);
		//	freeaddrinfo(info);
		//	WSACleanup();
		//	return 1;
		//}
		//// Write
		//result = send(serverSocket, (const char*)(&buffer.m_BufferData[0]), message.header.packetSize, 0);
		//if (result == SOCKET_ERROR) {
		//	printf("send failed with error %d\n", WSAGetLastError());
		//	closesocket(serverSocket);
		//	freeaddrinfo(info);
		//	WSACleanup();
		//	return 1;
		//}

		// Read
		result = recv(serverSocket, (char*)(&buffer.m_BufferData[0]), 512, 0);
		if (result == SOCKET_ERROR) {
			printf("recv failed with error %d\n", WSAGetLastError());
			closesocket(serverSocket);
			freeaddrinfo(info);
			WSACleanup();
			return 1;
		}
		//printf("Received %d bytes from the client!\n", result);

				// We must receive 4 bytes before we know how long the packet actually is
				// We must receive the entire packet before we can handle the message.
				// Our protocol says we have a HEADER[pktsize, messagetype];
		uint32_t packetSize = buffer.ReadUInt32LE(); 
		//printf("%d", packetSize);
		uint32_t messageType = buffer.ReadUInt32LE(); 
		//printf("%d", messageType);

		if (buffer.m_BufferData.size() >= packetSize) 
		{ 
			// We can finally handle our message
		}

		if (messageType == 1) 
		{
			// We know this is a ChatMessage
			uint32_t messageLength = buffer.ReadUInt32LE(); 
			std::string msg = buffer.ReadString(messageLength); 
			 
			// Must use .c_str() if printing from a std::string, to return as a 'const char*'
			printf("%s\n",
				1, msg.c_str()); 
		}
		else if (messageType == 2)
		{
			// this is a connection message
			uint32_t messageLength = buffer.ReadUInt32LE();
			std::string msg = buffer.ReadString(messageLength);

			printf("%s", msg.c_str());

		}

		system("Pause"); // Force the user to press enter to continue;

		//printf("Server sent: %s\n", buffer);
	}

	// Close
	freeaddrinfo(info); 
	closesocket(serverSocket); 
	WSACleanup();

	return 0;
}