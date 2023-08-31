#pragma once
#include <iostream>
#define CHECKER 65

typedef struct TcpSendCommand {  // Size 
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

typedef struct TcpRecvCommand { // Size 
	int Tilt;
	int Pan;
	unsigned char Permission; // Forced Permission Request
	unsigned char Fire;
	unsigned char TakeAim;
	unsigned char Magnification;
};

typedef struct MainMCUSendcom { // Size 20
	float GoalOpticalTilt = 180;
	float GoalOpticalPan = 180;
	float GoalBodyTilt = 180;
	float GoalBodyPan = 180;
	unsigned char Permission = 0; // Current Permission Status
	unsigned char dead1 = NULL; // Dead space for size
	unsigned char dead2 = NULL; // Dead space for size
	uint8_t start = CHECKER;
};

typedef struct MainMCURecvcom { // Size 28
	float RealOpticalTilt;
	float RealOpticalPan;
	float RealBodyTilt;
	float RealBodyPan;

	float IMUTilt;
	float IMUPan;
	unsigned char SentryPermission; // Request
	unsigned char fire;
	unsigned char dead = NULL; // Dead space for size
	uint8_t start = CHECKER;
};

typedef struct OpticalMCUSendcom { // Size 3
	unsigned char magnification; // 0 or 1
	unsigned char focus;
	uint8_t start = CHECKER;
};

typedef struct OpticalMCURecvcom { // Size 8
	float distance;
	unsigned char magnification;
	unsigned char dead2 = NULL; // Dead space for size
	unsigned char dead3 = NULL; // Dead space for size
	uint8_t start = CHECKER;
};

typedef struct GunMCUSendcom { // Size 2
	unsigned char fire;
	uint8_t start = CHECKER;
};

typedef struct GunMCURecvcom { // Size 8
	float voltage;
	unsigned char remaining_bullets;
	unsigned char dead1 = NULL; // Dead space for size
	unsigned char dead2 = NULL; // Dead space for size
	uint8_t start = CHECKER;
};