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

struct MainMCUSendcom { // Size 24
	//Main
	float GoalOpticalTilt = 180;
	float GoalOpticalPan = 180;
	float GoalBodyTilt = 180;
	float GoalBodyPan = 180;
	unsigned char Permission = 0; // Current Permission Status

	//Optical
	unsigned char magnification; // 0 or 1
	unsigned char focus;
	
	//Gun
	unsigned char fire;

	unsigned char dead1 = 0;
	unsigned char dead2 = 0;
	unsigned char dead3 = 0;

	uint8_t start = CHECKER;
};

struct MainMCURecvcom { // Size 40
	float RealOpticalTilt;
	float RealOpticalPan;
	float RealBodyTilt;
	float RealBodyPan;

	float IMUTilt; // nrf
	float IMUPan; // nrf

	float Lidardistance; // Optical
	float GunVoltage; // Gun
	unsigned char SentryPermission; // Request nrf
	unsigned char fire;	// nrf
	unsigned char RGBmagnification; // Optical
	unsigned char remaining_bullets; // Gun
	unsigned char dead1 = 0;
	unsigned char dead2 = 0;
	unsigned char dead3 = 0;

	uint8_t start = CHECKER;
};