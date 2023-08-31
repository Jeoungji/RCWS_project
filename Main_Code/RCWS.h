#pragma once
#include "SendImageUDP.h"
#include "Serial.h"
#include <chrono>
#include <thread>
#include <mutex>
#include <queue>
#include "Tracking.h"

#define WIDTH 1280
#define HEIGHT 720
#define PORT  9000

#define CameraToLidar 48.748
#define pi 3.14159265359

class RCWS {
// Hardware limit
private:
	// center is 0degree
	const float AnglelimitPan[2] = { -100, 100 };
	const float AnglelimitTilt[2] = { -20, 80 };

	const float OpticalAnglelimitPan[2] = { -30, 30 };
	const float OpticalAnglelimitTilt[2] = { -30, 30 };

	const float TurnAngleOptical = 0.1; // degree
	const float TurnAngleTurret = 0.4; // degree

	float RGBCameraAngle;
	int RGBDistancePoint; // pixel

	float IRCameraAngle;
	int IRDistancePoint; // pixel

// Goal Status
private:
	float RefCoordinate;
	std::array<float, 2> GoalTurretAngle; // Pan, Tilt
	std::array<float, 2> GoalOpticalAngle; // Pan, Tilt
	std::array<float, 2> TCPOpticalAngle; // Pan, Tilt
	bool SentryRequest; // a request for a sentry's authority
	bool AimRequest = false;
	bool FireRequest = false;
	unsigned char MagnificationRequest;


// Current Status
private:
	std::array<float, 2> RealTurretAngle; // MainMCU feedback turret angle
	std::array<float, 2> RealOpticalAngle; // MainMCU feedback optical angle
	int distance; // (mm) Distance measured by lidar
	std::array<float, 2> IMUdata; // Pan, Tilt
	std::pair<bool, std::array<float, 2>> IRLocation;
	short Permission; // 0:Server, 1:Sentry, 2:Forced Server
	bool RealAim = false;
	bool RealFire = false;
	unsigned char RealMagnification;
	// flag
	bool isRunning = false;
	bool isdatatcp = false;

private:
	Tracker traking;
public:
	RCWS();
	int distancePoint();

	void rcwsOpticalTurn();
	void rcwsTurretTurn();
public:
	int GetDistance() { return distance; }
	const short GetPermission() { return Permission; }

	void GetTCPcom(std::queue<TcpSendCommand>& send);
	void GetMainMCUcom(std::queue<MainMCUSendcom>& send);
	void GetOpticalMCUcom(std::queue<OpticalMCUSendcom>& send);
	void GetGunMCUcom(std::queue<GunMCUSendcom>& send);

public:
	void SetTCPcom(std::queue<TcpRecvCommand> &recv);
	void SetMainMCUcom(std::queue<MainMCURecvcom>& recv);
	void SetOpticalMCUcom(std::queue<OpticalMCURecvcom>& recv);


	void SetDistance(int dis)  { distance = dis; }
	void SetRGBCameraAngle(float Angle) { RGBCameraAngle = Angle; }
};