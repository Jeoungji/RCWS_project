#pragma once
#include <iostream>
#define CHECKER 65

struct TcpSendCommand {  // Size 
	float OpticalTilt;
	float OpticalPan;
	float BodyTilt;
	float BodyPan;
	int distance;
	int pointdistance;
	unsigned char Permission; // Current Permission Status
	unsigned char TakeAim;
	unsigned char Remaining_bullets;
	unsigned char Magnification;
	unsigned char Fire;
};

struct TcpRecvCommand { // Size 
	int Tilt;
	int Pan;
	unsigned char Permission; // Forced Permission Request
	unsigned char Fire;
	unsigned char TakeAim;
	unsigned char Magnification;
};

struct MainMCUSendcom { // Size 20
	//Main
	float GoalOpticalTilt = 180;
	float GoalOpticalPan = 180;
	float GoalBodyTilt = 180;
	float GoalBodyPan = 180;
	unsigned char Permission = 0; // Current Permission Status
	unsigned char dead1 = NULL; // Dead space for size
	unsigned char dead2 = NULL; // Dead space for size

	//Optical
	unsigned char magnification; // 0 or 1
	unsigned char focus;
	
	//Gun
	unsigned char fire;

	uint8_t start = CHECKER;
};

struct MainMCURecvcom { // Size 28
	//Main
	float RealOpticalTilt;
	float RealOpticalPan;
	float RealBodyTilt;
	float RealBodyPan;

	float IMUTilt;
	float IMUPan;
	unsigned char SentryPermission; // Request
	unsigned char fire;
	unsigned char dead = NULL; // Dead space for size

	//Optical
	float distance;
	unsigned char magnification;
	unsigned char dead2 = NULL; // Dead space for size
	unsigned char dead3 = NULL; // Dead space for size

	//Gun
	float voltage;
	unsigned char remaining_bullets;
	unsigned char dead1 = NULL; // Dead space for size
	unsigned char dead2 = NULL; // Dead space for size

	uint8_t start = CHECKER;
};