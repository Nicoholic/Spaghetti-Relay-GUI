#include "Client.h"



//
//int init(uint16_t port, char* address) 
//On a successful connection, returns SUCCESS.
//If address is not in dotted - quadrant format, returns ADDRESS_ERROR.
//If error was encountered when creating a socket, returns SETUP_ERROR.
//If error appeared during connect and was not caused by shutdown, returns CONNECT_ERROR.
//If error appeared during connect and WAS caused by shutdown, returns SHUTDOWN.
int Client::init(uint16_t port, char* address)
{
    ComSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ComSocket == INVALID_SOCKET)
	{
		return DISCONNECT;
	}
	
	//Connect
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.S_un.S_addr = inet_addr(address);
	serverAddr.sin_port = htons(port);

	 int result = connect(ComSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (result == SOCKET_ERROR)
	{
		return DISCONNECT;
	}
	
	
	return SUCCESS;
}




//int readMessage(char* buffer, int32_t size)
//On a successful receive and write to buffer, returns SUCCESS.
//If error appeared during receipt and was not caused by shutdown, returns DISCONNECT.
//If error appeared during receipt and WAS caused by shutdown, returns SHUTDOWN.
//If the message is longer than size, returns PARAMETER_ERROR
int Client::readMessage(char* buffer, int32_t size)
{
	
	char temp = 0;
	int result = tcp_recv_whole(ComSocket, (char*)&temp,1);
	if ((result == SOCKET_ERROR) || (result == 0))
	{
		int error = WSAGetLastError();
		return DISCONNECT;

	}


	result = tcp_recv_whole(ComSocket, buffer, temp);
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
int Client::sendMessage(char* data, int32_t length)
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
	return  SUCCESS;
}


int Client::tcp_send_whole(SOCKET skSocket, const char* data, uint16_t length)
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

int Client::tcp_recv_whole(SOCKET s, char* buf, int len)
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

// no returns 
void Client::stop()
{
	
	shutdown(ComSocket, SD_BOTH);
	closesocket(ComSocket);
}