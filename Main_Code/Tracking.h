#pragma once
#include "Struct.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>
#include <map>
#include <array>

#define IRCameraToLidar 56.748

class Tracker {
	//camera parameter
private:
	cv::Point Fov;
	cv::Point Resolution;

private:
	unsigned char threshold;
	const unsigned char Maxthreshold;
	const unsigned char Minthreshold;
	std::array<cv::Point, 4> target;
	cv::Mat result;
	bool visualflag;
	int IRDistancePoint;

	cv::Point Oncamera;
public:
	void setVisual(bool flag);
	int distancePoint(int distance);
public:
	Tracker(bool visual, unsigned char max, unsigned char min);
	double Getdistnace(int* p);
	std::pair<bool, cv::Point> IRCatching(cv::Mat frame);
public:
	cv::Point SetIrCoordinate();
	cv::Point SetIrCoordinate(int distance);
	std::pair<double, double> SetPolarCoordinate(cv::Point xy);
	std::pair<bool, std::pair<double, double>> SetPolarTracking(cv::VideoCapture& cap);
	std::pair<bool, std::pair<double, double>> SetPolarTracking(cv::VideoCapture& cap, int distance);
};