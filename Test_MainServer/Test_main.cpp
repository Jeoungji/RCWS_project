//#include "ImageThread.h"

#include <chrono>
#include <thread>
#include <mutex>
#include <queue>
#include "SendImageUDP.h"

#define WIDTH 1080
#define HEIGHT 720
#define PORT  9000
//#define SERVER_IP "192.168.0.2"
#define SERVER_IP "192.168.0.53"
//#define SERVER_IP "127.0.0.1"

std::queue<cv::Mat> buffer;
std::mutex mutex;
bool isRunning;

void fn(UDPSocket_Image& Socket) {
	cv::Mat image;
	while (1) {
		try
		{
			Socket.RecvFromImg(image);
			if (image.empty()) { continue; }
			else { 
				cv::imshow("recv_image", image);
				isRunning = true;
			}
		}
		catch (std::system_error& e)
		{
			std::cout << e.what();
		}

		cv::waitKey(20);
	}
}

int main()
{
	std::cout << "Test Controler Operate..." << std::endl;
	isRunning = false;
	WSASession Session;

	std::string serip = SERVER_IP;
	TCP sock(serip, 9001);

	UDPSocket_Image Socket;
	Socket.Bind(PORT);
	
	std::jthread t1(fn, std::ref(Socket));
	std::cout << "Waiting..." << std::endl;
	int count = 0;

	TcpSendCommand recv;
	TcpRecvCommand send = { 11,12,13,14 };

	while (1) {
		try {
			sock.Recv(recv);
			std::cout << "Tilt : " << (int)recv.BodyTilt << "  Pan : " << (int)recv.BodyPan
				<< "  OpticalTilt : " << (int)recv.OpticalTilt << "  OpticalPan : " << (int)recv.OpticalPan
				<< "  Permission : " << (int)recv.Permission << "  Pointdistance : " << (int)recv.pointdistance << std::endl;
		}
		catch (std::exception& e) {
		}


		try {
			sock.Send(send);
			
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}

		


	}
	std::cout << std::endl << "Done" << std::endl;
}