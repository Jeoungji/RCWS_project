

#include "SendImageUDP.h"
#include "Struct.h"
#include "Serial.h"
#include "RCWS.h"
#include <chrono>
#include <thread>
#include <mutex>
#include <queue>

#define DATARECIBEDPER 3

//#define SERIAL

std::mutex mut;
bool isRunning = false;
bool isConnectedServer = false;

std::queue<TcpRecvCommand> tcpRecvcom;
std::queue<TcpSendCommand> tcpSendcom;

std::queue<MainMCUSendcom> MainSendcom;
std::queue<MainMCURecvcom> MainRecvcom;

RCWS rcws;

void imageTread(
    UDPSocket_Image& Socket, cv::VideoCapture& cap)
{
    cv::Mat image;

    while (isConnectedServer) {

        
        
        auto start = std::chrono::steady_clock::now();
        try
        {
            cap.read(image);
            if (image.empty()) continue;
            int num = rcws.GetDistance();
            int d = rcws.distancePoint();
            std::cout << d << std::endl;
            //std::cout << "cols " << image.cols << "  rows : " << image.rows << std::endl;
            cv::line(image, cv::Point(WIDTH / 2, 0), cv::Point(WIDTH / 2, HEIGHT), cv::Scalar(255, 255, 255));
            cv::circle(image, cv::Point(d, HEIGHT / 2), 5, cv::Scalar(0, 0, 255), -1, cv::LINE_AA);
            Socket.SendTo(image);
        }
        catch (std::exception& ex)
        {
            std::cout << ex.what();
        }
        catch (std::runtime_error& e)
        {
            std::cout << e.what();
        }
        auto finish = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = finish - start;
        //std::cout << "Image Sending Time in seconds : " << duration.count() << std::endl;
        cv::waitKey(100);
    }
}

void Commu(
    TCP& com, Serial& MainMCU)
{
    MainMCURecvcom localMainrecvcom;
    MainMCUSendcom localMainsendcom;
    int count = 0;

    while (isConnectedServer) {
        mut.lock();
        try {
            TcpRecvCommand recv;
            com.Recv(recv);
            std::cout << recv.Pan << std::endl;
            tcpRecvcom.push(recv);
        }
        catch (std::exception& e) {
        }
        mut.unlock();

        mut.lock();
        if (!tcpSendcom.empty()) {
            try {
                com.Send(tcpSendcom.front());
                tcpSendcom.pop();
            }
            catch (std::exception& e) {
                std::cout << e.what() << std::endl;
                isConnectedServer = false;
            }
        }
        mut.unlock();

        if (!MainMCU.IsConnected()) isRunning = false;
        int a = MainMCU.ReadData(localMainrecvcom);
        if (a) {
            MainRecvcom.push(localMainrecvcom);
        }
        if (!MainSendcom.empty()) {
            localMainsendcom = MainSendcom.front();
            MainSendcom.pop();
            MainMCU.WriteData(localMainsendcom);
        }
    }
}

int main()
{
    std::cout << "RCWS Operate..." << std::endl;
    isRunning = true;

    Serial MainMCU(6, "Arduino DUE");    // adjust as needed

    if (MainMCU.IsConnected()) {
        std::cout << "[INFO] " << MainMCU.PortName << " Connetcted" << std::endl;
        if (!MainMCU.Checking(sizeof(MainMCURecvcom))) exit(0);
    }
    else exit(0);

    cv::VideoCapture RGBCamera(1);
    if (!RGBCamera.isOpened()) {
        std::cout << "[ERROR] RGBCamera Open failed" << std::endl;
        isRunning = false;
        exit(1);
    }
    else std::cout << "[INFO] RGBCamera Open Success" << std::endl;
    RGBCamera.set(cv::CAP_PROP_FRAME_WIDTH, WIDTH);
    RGBCamera.set(cv::CAP_PROP_FRAME_HEIGHT, HEIGHT);

    cv::VideoCapture IRCamera(2);
    if (!RGBCamera.isOpened()) {

        std::cout << "[ERROR] IRCamera Open failed" << std::endl;
        isRunning = false;
        exit(1);
    }
    else std::cout << "[INFO] IRCamera Open Success" << std::endl;

    WSASession Session;
    TCP com(PORT+1);
    sockaddr_in client = com.getPartnerAddr();
    std::string clientIP = inet_ntoa(client.sin_addr);

    UDPSocket_Image Socket(clientIP, PORT, cv::Size(WIDTH, HEIGHT));

    std::jthread SendingImage(imageTread, 
        std::ref(Socket), std::ref(RGBCamera));

    std::jthread Communication(Commu, std::ref(com), std::ref(MainMCU));

    isConnectedServer = true;
 
    cv::waitKey(1000);

    while (isRunning) {
        // 상황실 데이터 수신
        rcws.SetTCPcom(tcpRecvcom);
        
        // MainMCU 데이터 수신
        rcws.SetMainMCUcom(MainRecvcom);

        //std::cout << rcws.GetGoalTurretAnglePan() << std::endl;
        //std::cout << MainRecvcom.front().RealOpticalTilt << " " <<
        //    MainRecvcom.front() .RealBodyPan << " " <<
        //    MainRecvcom.front() .RealBodyTilt << " " <<
        //    MainRecvcom.front() .RealBodyPan << " " <<
        //    MainRecvcom.front() .IMUTilt << " " <<
        //    MainRecvcom.front() .IMUPan << " " <<
        //    MainRecvcom.front() .Lidardistance << " " <<
        //    MainRecvcom.front() .GunVoltage << " " <<
        //    (int)MainRecvcom.front() .SentryPermission << " " <<
        //    (int)MainRecvcom.front() .fire << " " <<
        //    (int)MainRecvcom.front() .RGBmagnification << " "<<
        //    (int)MainRecvcom.front() .remaining_bullets << std::endl;

        switch (rcws.GetPermission()) {
        case 0: // 0번_기본 상황실 제어
        case 2: // 2번_강제 상황실 제어

            break;
        case 1: // 1번_초병 제어

            break;

        default:
            std::cout << "[WARNNING] Permmission Error" << std::endl;
        }

        //rcws.GetTCPcom(tcpSendcom);
        //rcws.GetMainMCUcom(MainSendcom);

    }

    return 0;
}
