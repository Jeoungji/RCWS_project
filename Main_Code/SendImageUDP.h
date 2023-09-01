#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <system_error>
#include <string>
#include <iostream>
#include <vector>
#include "opencv2/opencv.hpp"
#include "Struct.h"
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)


#define SENDBUFSIZE 40960
#define SPLITIMG 7
#define BUFSIZE (SENDBUFSIZE * SPLITIMG) 

class WSASession
{
public:
	WSASession();
	~WSASession();
private:
	WSAData data;
};

class TCP {
private:
    WSADATA wsaData;
    sockaddr_in addr;
    sockaddr_in partner_addr;
    SOCKET sock;
    SOCKET partner_sock;
	//std::vector<char> Sbuf;
	//std::vector<char> Rbuf;

    char buf[30];
public:
    TCP(const unsigned int port);
	TCP(const std::string& server_ip, const unsigned int port);
	~TCP();
	sockaddr_in getPartnerAddr();
	int Recv(TcpRecvCommand& command);
	int Recv(TcpSendCommand& command);
	void Send(TcpSendCommand& command);
	void Send(TcpRecvCommand& command);
};

class UDPSocket_Image
{
private:
	sockaddr_in add;
	SOCKET sock;
	FILE* fp;
	cv::Size size;
	std::vector<char> buf;
	cv::Mat lastimage;
public:
	UDPSocket_Image(const std::string& address, unsigned short port, cv::Size img);
	UDPSocket_Image();
	~UDPSocket_Image();
	void SendTo(cv::Mat& image, int flags = 0);
	void RecvFromImg(cv::Mat& image, int flags = 0);
	void SendTo(const std::string& address, unsigned short port, const char* buffer, int len, int flags = 0);
	void SendTo(sockaddr_in& address, const char* buffer, int len, int flags = 0);
	sockaddr_in RecvFrom(char* buffer, int len, int flags = 0);
	void Setting(const std::string& address, unsigned short port, cv::Size img);
	void Bind(unsigned short port);
};