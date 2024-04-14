#include "Server.h"


//int init(uint16_t port)
//Return Values On a successful connection, returns SUCCESS.
//If error was encountered when binding the socket, returns BIND_ERROR.
//If error was encountered when creating a socket or listening, returns SETUP_ERROR.
//If error appeared during accept and was not caused by shutdown, returns CONNECT_ERROR.
//If error appeared during accept and WAS caused by shutdown, returns SHUTDOWN.
int Server::init(uint16_t port)
{

	//create the socket 

	
	listeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//If error was encountered when creating a socket or listening, returns SETUP_ERROR.
	if (listeningSocket < 0)
	{
		return SETUP_ERROR;
	}

	//Bind
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	serverAddr.sin_port = htons(port);
	int result = bind(listeningSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	
	//If error was encountered when binding the socket, returns BIND_ERROR.
	if (result == SOCKET_ERROR)
	{

		
		return BIND_ERROR;
	}

	// now we listen 
	result = listen(listeningSocket, 1);
	if (result == SOCKET_ERROR)
	{
		return SETUP_ERROR;
	}


	ComSocket = accept(listeningSocket, NULL, NULL);
	//If error appeared during accept and was not caused by shutdown, returns CONNECT_ERROR.
	if (ComSocket == INVALID_SOCKET)
	{
		return CONNECT_ERROR;
	}

	return SUCCESS;
	//If error appeared during accept and WAS caused by shutdown, returns SHUTDOWN.
	
}


//int readMessage(char* buffer, int32_t size)
//On a successful receive and write to buffer, returns SUCCESS.
//If error appeared during receipt and was not caused by shutdown, returns DISCONNECT.
//If error appeared during receipt and WAS caused by shutdown, returns SHUTDOWN.
//If the message is longer than size, returns PARAMETER_ERROR.
int Server::readMessage(char* buffer, int32_t size)
{
	
	char temp = 0;
	 int result = tcp_recv_whole(ComSocket, (char*)&temp, 1);
	if ((result == SOCKET_ERROR) || (result == 0))
	{
		int error = WSAGetLastError();
		return DISCONNECT;

	}
	

	
	result = tcp_recv_whole(ComSocket,buffer, temp);
	if ((result == SOCKET_ERROR) || (result == 0))
	{
		int error = WSAGetLastError();
		return DISCONNECT;

	}

	return SUCCESS;
}


//int sendMessage(char* data, int32_t length)
//On a successful write to stream, returns SUCCESS.
//If error appeared during send and was not caused by shutdown, returns DISCONNECT.
//If error appeared during send and WAS caused by shutdown, returns SHUTDOWN.
//If length is less than 0 or greater than 255, returns PARAMETER_ERROR.
int Server::sendMessage(char* data, int32_t length)
{
	
	int result = tcp_send_whole(ComSocket, (char*)&length, 1);
	if ((result == SOCKET_ERROR) || (result == 0))
	{
		int error = WSAGetLastError();
		
		return DISCONNECT;

	}


	result = tcp_send_whole(ComSocket, data, length);
	if ((result == SOCKET_ERROR) || (result == 0))
	{
		int error = WSAGetLastError();
		return DISCONNECT;

	}
	return SUCCESS;
}


int Server::tcp_recv_whole(SOCKET s, char* buf, int len)
{
	int total = 0;

	do
	{
		int ret = recv(s, buf + total, len - total, 0);
		if (ret < 1)
			return ret;
		else
			total += ret;

	} while (total < len);

	return total;
}

int Server::tcp_send_whole(SOCKET skSocket, const char* data, uint16_t length)
{
	int result;
	int bytesSent = 0;

	while (bytesSent < length)
	{
		result = send(skSocket, (const char*)data + bytesSent, length - bytesSent, 0);

		if (result <= 0)
			return result;

		bytesSent += result;
	}

	return bytesSent;
}


//no returns 
void Server::stop()
{
	shutdown(listeningSocket, SD_BOTH);
	closesocket(listeningSocket);

	shutdown(ComSocket, SD_BOTH);
	closesocket(ComSocket);

}