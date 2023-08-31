#include "RCWS.h"
#include <chrono>
#include <thread>
#include <mutex>
#include <queue>


RCWS::RCWS()
	:traking(false, 243,200)
{}

int RCWS::distancePoint() {
	float half_angle_radian = tan(RGBCameraAngle / 2 * pi / 180);
	float x = CameraToLidar / half_angle_radian;

	if (distance < x) {
		RGBDistancePoint = 0;
		return 0;
	}

	float alpha = (distance - x) * half_angle_radian;

	float Width = distance * half_angle_radian;

	float pointdistance = Width / (WIDTH / 2);

	RGBDistancePoint = alpha / pointdistance;

	//std::cout << "distance : " << distance << "  frame : " << frame.rows << "  pointdistance : " << pointdistance << "  DistancePoint : " << DistancePoint << std::endl;
	return RGBDistancePoint;
}


void RCWS::SetTCPcom(std::queue<TcpRecvCommand>& recv) {
	if (recv.empty()) return;

	if (recv.front().Permission == 1) {
		Permission = 2;
	}

	MagnificationRequest = recv.front().Magnification;

	if (Permission != 1) { // Do not update data when sentry permission status
		FireRequest = recv.front().Fire;
		TCPOpticalAngle[0] = TurnAngleOptical * recv.front().Pan;
		TCPOpticalAngle[1] = TurnAngleOptical * recv.front().Tilt;
		AimRequest = recv.front().TakeAim;
	}
	recv.pop();
}

void RCWS::SetMainMCUcom(std::queue<MainMCURecvcom>& recv) {
	if (recv.empty()) return;
	if (Permission != 2) { // If you do not have server forced permission
		Permission = recv.front().SentryPermission;
	} 

	IMUdata[0] = recv.front().IMUPan;
	IMUdata[1] = recv.front().IMUTilt;
	
	RealTurretAngle[0] = recv.front().RealBodyPan;
	RealTurretAngle[1] = recv.front().RealBodyTilt;
	RealOpticalAngle[0] = recv.front().RealOpticalPan;
	RealOpticalAngle[1] = recv.front().RealOpticalTilt;
	
	if (Permission == 1)
		FireRequest = recv.front().fire;

	recv.pop();
}


void RCWS::rcwsTurretTurn() {

}

void RCWS::GetTCPcom(std::queue<TcpSendCommand>& send) {
	TcpSendCommand com;
	com.BodyPan = RealTurretAngle[0];
	com.BodyTilt = RealTurretAngle[1];
	com.OpticalPan = RealOpticalAngle[0];
	com.OpticalTilt = RealOpticalAngle[1];
	com.Permission = Permission;
	com.distance = distance; //(mm)
	com.pointdistance = RGBDistancePoint;
	com.TakeAim = RealAim;
	com.Fire = RealFire;
	com.Magnification = RealMagnification;
	send.push(com);
}

void RCWS::GetMainMCUcom(std::queue<MainMCUSendcom>& send) {
	MainMCUSendcom com;
	com.GoalBodyPan = GoalTurretAngle[0];
	com.GoalBodyTilt = GoalTurretAngle[1];
	com.GoalOpticalPan = GoalOpticalAngle[0];
	com.GoalOpticalTilt = GoalOpticalAngle[1];
	com.Permission = Permission;
	send.push(com);
}