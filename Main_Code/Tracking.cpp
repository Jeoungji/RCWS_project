#include "Tracking.h"
#define pi 3.14159265359

void Tracker::setVisual(bool flag) {
	visualflag = flag;
}

Tracker::Tracker(bool visual = false, unsigned char max = 243, unsigned char min = 200)
	: Maxthreshold(max), Minthreshold(min), threshold(240), visualflag(false),
	Fov(100, 138), Resolution(640, 480)
{}

double Tracker::Getdistnace(int* p) {
	return std::sqrt(std::pow(target[3].x - (p[0] + p[2] / 2), 2)
		+ std::pow(target[3].y - (p[1] + p[3] / 2), 2));
}

std::pair<bool, cv::Point> Tracker::IRCatching(cv::Mat frame) {
	cv::Mat gray, bin, dst, labels, stats, centriods;
	bool detact = false;

	frame.copyTo(dst);

	cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

	cv::threshold(gray, bin, threshold, 255, cv::THRESH_BINARY);

	int cnt = cv::connectedComponentsWithStats(bin, labels, stats, centriods);

	if (cnt > 1) { // 배경을 제외한 대상이 인식된 경우

		std::map <int, int*, std::greater<int>> maxpoint;
		std::map <double, int*> shortpoint;

		for (int i = 1; i < cnt; i++) { // 배경을 제외한 대상의 데이터 검사
			int* p = stats.ptr<int>(i);

			if (p[4] < 50 || p[4] > 1000) continue;	// 크기가 너무 작거나 큰 경우 제외

			maxpoint.emplace(std::pair<int, int*>(p[4], p));

			// 이전 대상과의 거리 계산
			shortpoint.emplace(std::pair<double, int*>(Getdistnace(p), p));
		}

		if (maxpoint.size()) {
			detact = true;
			int* p_M = maxpoint.begin()->second;
			if (visualflag) cv::rectangle(dst, cv::Rect(p_M[0], p_M[1], p_M[2], p_M[3]), cv::Scalar(0, 255, 255), 2);

			int* p_S = shortpoint.begin()->second;
			if (visualflag) cv::rectangle(dst, cv::Rect(p_S[0], p_S[1], p_S[2], p_S[3]), cv::Scalar(255, 100, 255), 2);


			if (p_S[4] * 4 < p_M[4]) {
				if (visualflag) cv::rectangle(dst, cv::Rect(p_M[0], p_M[1], p_M[2], p_M[3]), cv::Scalar(255, 255, 255), 1);
				target[3].x = p_M[0] + p_M[2] / 2;
				target[3].y = p_M[1] + p_M[3] / 2;
			}
			else {
				if (visualflag) cv::rectangle(dst, cv::Rect(p_S[0], p_S[1], p_S[2], p_S[3]), cv::Scalar(255, 255, 255), 1);
				target[3].x = p_S[0] + p_S[2] / 2;
				target[3].y = p_S[1] + p_S[3] / 2;
			}

			if (maxpoint.begin()->first > 500 && threshold < 243) threshold++;
			if (maxpoint.begin()->first < 60 && threshold > 230) threshold--;
		}
	}

	if (cnt > 2 && threshold < 243) threshold++;
	if (cnt < 2 && threshold > 230) threshold--;

	//vector 기반 방향 추적이 필요한 경우 추가 예정
	/*for (int i = 0; i < 3; i++)
		target[i] = target[i + 1];*/
	dst.copyTo(result);
	
	std::pair<bool, cv::Point> retu = { detact, target[3] };
	return retu;
}

int Tracker::distancePoint(int distance) {
	float half_angle_radian = tan(Fov.x / 2 * pi / 180);
	float x = IRCameraToLidar / half_angle_radian;
	std::cout << "d" << std::endl;
	if (distance < x) {
		IRDistancePoint = Resolution.x;
		return 0;
	}

	float alpha = (distance - x) * half_angle_radian;

	float Width = distance * half_angle_radian;

	float pointdistance = Width / (Resolution.x / 2);

	IRDistancePoint = Resolution.x - alpha / pointdistance;

	//std::cout << "distance : " << distance << "  frame : " << frame.rows << "  pointdistance : " << pointdistance << "  DistancePoint : " << DistancePoint << std::endl;
	return IRDistancePoint;
}

cv::Point Tracker::SetIrCoordinate() {
	cv::Point xy; // 카메라좌표로 저장
	xy.x = target[3].x - Resolution.x / 2;
	xy.y = -target[3].y + Resolution.y / 2;
	return xy;
}

cv::Point Tracker::SetIrCoordinate(int distance) {
	cv::Point xy; // 카메라좌표로 저장

	xy.x = target[3].x - distancePoint(distance);
	xy.y = -target[3].y + Resolution.y / 2;
	return xy;
}

std::pair<double, double> Tracker::SetPolarCoordinate(cv::Point xy) {
	std::pair<double, double> theta;
	theta.first = xy.x * 0.215625;
	theta.second = xy.y * 0.20833333;
	return theta;
}

std::pair<bool, std::pair<double, double>> Tracker::SetPolarTracking(cv::VideoCapture& cap) {
	std::pair<bool, cv::Point> ou;
	std::pair<bool, std::pair<double, double>> retu;

	cv::Mat frame;
	cap.read(frame);
	ou = IRCatching(frame);
	
	if (ou.first) {
		cv::Point m;
		m = SetIrCoordinate();
		retu.second = SetPolarCoordinate(m);
		retu.first = true;
	}
	else {
		retu.second.first = 0;
		retu.second.second = 0;
		retu.first = false;
	}
	if (visualflag) cv::imshow("frame", result);
	return retu;
}

std::pair<bool, std::pair<double, double>> Tracker::SetPolarTracking(cv::VideoCapture &cap, int distance) {
	std::pair<bool, cv::Point> ou;
	std::pair<bool, std::pair<double, double>> retu;

	cv::Mat frame;
	cap.read(frame);
	ou = IRCatching(frame);

	if (ou.first) {
		cv::Point m;
		m = SetIrCoordinate(distance);
		retu.second = SetPolarCoordinate(m);
		retu.first = true;
	}
	else {
		retu.second.first = 0;
		retu.second.second = 0;
		retu.first = false;
	}
	if (visualflag) {
		cv::line(result, cv::Point(Resolution.x / 2, 0), cv::Point(Resolution.x / 2, Resolution.y), cv::Scalar(255, 255, 255));
		cv::circle(result, cv::Point(IRDistancePoint, Resolution.y / 2), 5, cv::Scalar(0, 0, 255), -1, cv::LINE_AA);
		std::cout << IRDistancePoint << std::endl;
		cv::imshow("frame", result);
	}
	return retu;
}

//int main() {
//	cv::VideoCapture cap(0);
//	
//	Tracker Ircamera(true);
//	Ircamera.setVisual(true);
//	std::pair<bool, std::pair<double, double>> theta;
//	int num = 1000;
//
//	while (1) {
//		num = num + 10;
//		if (num > 30000) num = 0;
//		std::cout << num << std::endl;
//		theta = Ircamera.SetPolarTracking(cap, num);
//		if (theta.first) {
//			std::cout << theta.second.first << "   " << theta.second.second << std::endl;
//		}
//		cv::waitKey(10);
//	}
//}