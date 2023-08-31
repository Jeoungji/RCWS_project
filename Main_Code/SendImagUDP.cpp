#include "SendImageUDP.h"



WSASession::WSASession()
{
	int ret = WSAStartup(MAKEWORD(2, 2), &data);
	if (ret != 0)
		throw std::system_error(WSAGetLastError(), std::system_category(), "WSAStartup Failed");
}
WSASession::~WSASession()
{
	WSACleanup();
}

/* RCWS 생성자 */
TCP::TCP(const unsigned int port) 
{

	WSAStartup(MAKEWORD(2, 2), &wsaData);
	// 소켓 만들기
	sock = socket(AF_INET, SOCK_STREAM, 0);
	
	// 소켓 설정
	ZeroMemory(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	std::cout << "[INFO] TCP My IP : " << inet_ntoa(addr.sin_addr) << " PORT : " << addr.sin_port << std::endl;

	bind(sock, (SOCKADDR*)&addr, sizeof(addr));

	DWORD recvTimeout = 20;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&recvTimeout, sizeof(recvTimeout));
	listen(sock, 2);
	std::cout << "[INFO] Listenning..." << std::endl;
	// 클라이언트 연결 요청 수락

	ZeroMemory(&partner_addr, sizeof(partner_addr));
	int nlen = sizeof(partner_addr);
	partner_sock = accept(sock, (SOCKADDR*)&partner_addr, &nlen);
	// TCP 연결 수립
	std::cout << "[INFO] Client Accepted" << std::endl;
	std::cout << "[INFO] TCP Client IP : " << inet_ntoa(partner_addr.sin_addr) << " PORT : " << partner_addr.sin_port << std::endl;
}
/* 시스템용 생성자 */
TCP::TCP(const std::string& server_ip, const unsigned int port) 
{
	// WSA  초기화
	int a = WSAStartup(MAKEWORD(2, 2), &wsaData);
	// 소켓 만들기
	partner_sock = socket(AF_INET, SOCK_STREAM, 0);
	// 소켓 설정
	ZeroMemory(&addr, sizeof(addr));
	partner_addr.sin_family = AF_INET;
	partner_addr.sin_addr.s_addr = inet_addr(server_ip.c_str());
	partner_addr.sin_port = htons(port);
	std::cout << "[INFO] TCP Server IP : " << inet_ntoa(partner_addr.sin_addr) << " PORT : " << partner_addr.sin_port << std::endl;
	// TCP 연결 시도
	connect(partner_sock, (SOCKADDR*)&partner_addr, sizeof(partner_addr));
}
TCP::~TCP() {
	closesocket(sock);
	closesocket(partner_sock);
	WSACleanup();
}
sockaddr_in TCP::getPartnerAddr() {
	return partner_addr;
}
int TCP::Recv(TcpRecvCommand& command) {
	sockaddr_in from;
	int size = sizeof(from);
	ZeroMemory(buf, NULL);
	int ret = recvfrom(partner_sock, buf, sizeof(command), 0, reinterpret_cast<SOCKADDR*>(&from), &size);
	if (ret < 0)
		throw std::system_error(WSAGetLastError(), std::system_category(), "recvfrom failed");
	memcpy(&command, buf, sizeof(command));
	return ret;
}
int TCP::Recv(TcpSendCommand& command) {
	sockaddr_in from;
	int size = sizeof(from);
	ZeroMemory(buf, NULL);
	int ret = recvfrom(partner_sock, buf,sizeof(command), 0, reinterpret_cast<SOCKADDR*>(&from), &size);
	if (ret < 0)
		throw std::system_error(WSAGetLastError(), std::system_category(), "recvfrom failed");
	memcpy(&command, buf, sizeof(command));
	return ret;
}
void TCP::Send(TcpSendCommand& command) {
	ZeroMemory(buf, NULL);
	memcpy(buf, &command, sizeof(command));
	int ret = sendto(partner_sock, buf, sizeof(command), 0, reinterpret_cast<SOCKADDR*>(&partner_addr), sizeof(partner_addr));
	if (ret < 0)
		throw std::system_error(WSAGetLastError(), std::system_category(), "sendto failed");
}
void TCP::Send(TcpRecvCommand& command) {
	ZeroMemory(buf, NULL);
	memcpy(buf, &command, sizeof(command));
	int ret = sendto(partner_sock, buf, sizeof(command), 0, reinterpret_cast<SOCKADDR*>(&partner_addr), sizeof(partner_addr));
	if (ret < 0)
		throw std::system_error(WSAGetLastError(), std::system_category(), "sendto failed");
}





UDPSocket_Image::UDPSocket_Image(const std::string& address, unsigned short port, cv::Size img)
	: size(img), fp(NULL), buf(BUFSIZE, NULL)
{
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET)
		throw std::system_error(WSAGetLastError(), std::system_category(), "Error opening UDPsocket");
	ZeroMemory(&add, sizeof(add));
	add.sin_family = AF_INET;
	add.sin_addr.s_addr = inet_addr(address.c_str());
	add.sin_port = htons(port);
	std::cout << "[INFO] UDP Client IP : " << inet_ntoa(add.sin_addr) << " PORT : " << add.sin_port << std::endl;
}
UDPSocket_Image::UDPSocket_Image()
	: size(NULL, NULL), fp(NULL), buf(BUFSIZE, NULL)
{
	ZeroMemory(&add, sizeof(add));
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET)
		throw std::system_error(WSAGetLastError(), std::system_category(), "Error opening UDPsocket");
}
void UDPSocket_Image::Setting(const std::string& address, unsigned short port, cv::Size img) {
	size = img;
	ZeroMemory(&add, sizeof(add));
	add.sin_family = AF_INET;
	add.sin_addr.s_addr = inet_addr(address.c_str());
	add.sin_port = htons(port);
	std::cout << "[INFO] UDP Client IP : " << inet_ntoa(add.sin_addr) << " PORT : " << add.sin_port << std::endl;
}
void UDPSocket_Image::Bind(unsigned short port)
{
	sockaddr_in add;
	add.sin_family = AF_INET;
	add.sin_addr.s_addr = htonl(INADDR_ANY);
	add.sin_port = htons(port);
	std::cout << "[INFO] UDP Server IP : " << inet_ntoa(add.sin_addr) << " PORT : " << add.sin_port << std::endl;
	int ret = bind(sock, reinterpret_cast<SOCKADDR*>(&add), sizeof(add));
	if (ret < 0)
		throw std::system_error(WSAGetLastError(), std::system_category(), "Bind failed");
}
UDPSocket_Image::~UDPSocket_Image()
{
	closesocket(sock);
	remove("recv.jpg");
	remove("tmp.jpg");
}

void UDPSocket_Image::SendTo(const std::string& address, unsigned short port, const char* buffer, int len, int flags)
{
	sockaddr_in add;
	add.sin_family = AF_INET;
	add.sin_addr.s_addr = inet_addr(address.c_str());
	add.sin_port = htons(port);
	int ret = sendto(sock, buffer, len, flags, reinterpret_cast<SOCKADDR*>(&add), sizeof(add));
	if (ret < 0)
		throw std::system_error(WSAGetLastError(), std::system_category(), "sendto failed");
}
void UDPSocket_Image::SendTo(sockaddr_in& address, const char* buffer, int len, int flags)
{
	int ret = sendto(sock, buffer, len, flags, reinterpret_cast<SOCKADDR*>(&address), sizeof(address));
	if (ret < 0)
		throw std::system_error(WSAGetLastError(), std::system_category(), "sendto failed");
}
sockaddr_in UDPSocket_Image::RecvFrom(char* buffer, int len, int flags)
{
	sockaddr_in from;
	int size = sizeof(from);
	int ret = recvfrom(sock, buffer, len, flags, reinterpret_cast<SOCKADDR*>(&from), &size);
	if (ret < 0)
		throw std::system_error(WSAGetLastError(), std::system_category(), "recvfrom failed");

	// make the buffer zero terminated
	buffer[ret] = 0;
	return from;
}

void UDPSocket_Image::SendTo(cv::Mat& image, int flags) {
	resize(image, image, size);

	cv::imwrite("tmp.jpg", image);

	ZeroMemory(&buf[0], NULL);

	fopen_s(&fp, "tmp.jpg", "rb");

	if (fp == NULL) {
		throw std::runtime_error("[ERROR] Can't find tmp.jpg (fp is NULL)");
		return;
	}
	fread(&buf[0], BUFSIZE, 1, fp);

	std::vector<char> buffer(SENDBUFSIZE);
	for (int i = 0; i < SPLITIMG; i++) {
		memcpy(&buffer[0], &buf[i * SENDBUFSIZE], (buffer.size()) * sizeof(char));
		//std::cout << i << "번째 송신" << std::endl;
		int ret = sendto(sock, &buffer[0], buffer.size() * sizeof(char), flags, reinterpret_cast<SOCKADDR*>(&add), sizeof(add));
		if (ret < 0) {
			throw std::system_error(WSAGetLastError(), std::system_category(), "sendto failed");
			i--;
		}
		else {
			char buffers;
			RecvFrom(&buffers, sizeof(buffers));
		}
	}
	fclose(fp);
	lastimage = image;
}
void UDPSocket_Image::RecvFromImg(cv::Mat& image, int flags) {
	sockaddr_in from;

	int size = sizeof(from);

	ZeroMemory(&buf[0], NULL);

	fopen_s(&fp, "recv.jpg", "wb");

	for (int i = 0; i < SPLITIMG; i++) {
		std::vector<char> buffer(SENDBUFSIZE);
		int ret = recvfrom(sock, &buffer[0], buffer.size() * sizeof(char), flags, reinterpret_cast<SOCKADDR*>(&from), &size);
		if (ret < 0) {
			throw std::system_error(WSAGetLastError(), std::system_category(), "recvfrom failed");
			i--;
		}
		memcpy(&buf[i * SENDBUFSIZE], &buffer[0], (buffer.size()) * sizeof(char));
		char ack = i + 0x30;
		SendTo(from, &ack, sizeof(ack));
	}
	fwrite(&buf[0], BUFSIZE, 1, fp);
	fclose(fp);
	image = cv::imread("recv.jpg");
}