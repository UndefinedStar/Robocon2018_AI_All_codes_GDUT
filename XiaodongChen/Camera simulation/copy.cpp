#define pi 3.1415926
#define RollRadius 0.4
#define RollDivide 30//度数

#include<opencv2\opencv.hpp>
#include<iostream>
#include<math.h>
#include<opencv2\highgui.hpp>
#include<opencv2\core.hpp>
#include<opencv2\imgproc.hpp>

using namespace std;
using namespace cv;

double X = 0, Y = 0;
double Xt = 0, Yt = 0, Zt = 0;
void On_mouse(int event, int x, int y, int flags, void*);

int main()
{
	//读入地图
	Mat map;
	map = imread("map.png", 1);

	//最终结果
	vector<Point2d> uvRedRoll;
	vector<Point2d> uvGoldRoll;
	vector<Point2d> uvBlueRoll;

	//各个坐标的变量
	double Xw = 0, Yw = 0, Zw = 0;
	double Xc = 0, Yc = 0, Zc = 0;
	double x = 0, y = 0;
	double u = 0, v = 0;

	//外参T,R
	Point3d RedPoint = { 3.825,7.085,2.4 };//世界坐标系下的三个圆环圆心的三维坐标//X Y Z
	Point3d GoldPoint = { 7.085,7.085,3.4 };//单位是：m
	Point3d BluePoint = { 10.345,7.085,2.4 };
	vector<Point3d> RedRoll;
	vector<Point3d> GoldRoll;
	vector<Point3d> BlueRoll;
	//计算圆环的各个点的世界坐标
	for (int i = 0;i < 360;i = i + RollDivide)//红圈
	{
		double flat_angle = pi*i / 180;
		double flat_x = RollRadius*cos(flat_angle) + RedPoint.x;
		double flat_y = RedPoint.y;
		double flat_z = RollRadius*sin(flat_angle) + RedPoint.z;
		RedRoll.push_back(Point3d(flat_x, flat_y, flat_z));
	}
	for (int i = 0;i < 360;i = i + RollDivide)//金圈
	{
		double flat_angle = pi*i / 180;
		double flat_x = RollRadius*cos(flat_angle) + GoldPoint.x;
		double flat_y = GoldPoint.y;
		double flat_z = RollRadius*sin(flat_angle) + GoldPoint.z;
		GoldRoll.push_back(Point3d(flat_x, flat_y, flat_z));
	}
	for (int i = 0;i < 360;i = i + RollDivide)//蓝圈
	{
		double flat_angle = pi*i / 180;
		double flat_x = RollRadius*cos(flat_angle) + BluePoint.x;
		double flat_y = BluePoint.y;
		double flat_z = RollRadius*sin(flat_angle) + BluePoint.z;
		BlueRoll.push_back(Point3d(flat_x, flat_y, flat_z));
	}
	double RotateAngle = 0;
	cout << "输入相机的仰角 " << endl;
	cin >> RotateAngle;
	double Rx[3][3] = { { 1,0,0 },{ 0,cos(pi*(RotateAngle - 90) / 180),sin(pi*(RotateAngle - 90) / 180) },{ 0,-sin(pi*((RotateAngle - 90) / 180)),cos(pi*((RotateAngle - 90) / 180)) } };
	Mat R(3, 3, CV_64FC1, Rx);//旋转矩阵
	cout << "输入相机在世界坐标系下的高度 " << endl;
	cout << "Zt " << endl;
	cin >> Zt;
	cin >> Xt;
	cin >> Yt;
	namedWindow("map", WINDOW_NORMAL);
	setMouseCallback("map", On_mouse, 0);
	imshow("map", map);
	Mat img = Mat::zeros(1080, 1920, CV_8UC3);
	//Xt = 14.17 * X / map.cols;
	//Yt = 14.17 * Y / map.rows;
	double T1[3][1] = { { -Xt },{ -Yt },{ -Zt } };
	Mat T(3, 1, CV_64FC1, T1);//平移矩阵

							  //内参f,dy,dx,cy,cx
	double f = 28.0;//单位是：mm//焦距
	double p = 35.0;//单位是：mm//画幅
	int ImageSizeRows = 1080;//分辨率
	int ImageSizeCols = 1920;
	double dy = 0, dx = 0;
	double cx = 0, cy = 0;
	cx = ImageSizeCols / 2;//理想情况下
	cy = ImageSizeRows / 2;
	double flat = 0;
	flat = sqrt((p*p) / 337);
	dx = (16 * flat) / ImageSizeCols;
	dy = (9 * flat) / ImageSizeRows;
	double InsideValue[3][3] = { { f / dx,0,cx },{ 0,f / dy,cy },{ 0,0,1 } };
	Mat Inside(3, 3, CV_64FC1, InsideValue);//内参数矩阵
	for (int i = 0;i < RedRoll.size();i++)
	{
		Xw = RedRoll.at(i).x;
		Yw = RedRoll.at(i).y;
		Zw = RedRoll.at(i).z;
		double World1[3][1] = { { Xw },{ Yw },{ Zw } };
		Mat World(3, 1, CV_64FC1, World1);
		double Cam1[3][1] = { { Xc },{ Yc },{ Zc } };
		Mat Cam(3, 1, CV_64FC1, Cam1);
		Cam = R*(World + T);
		double Pix1[3][1] = { { u },{ v },{ 1 } };
		Mat Pix(3, 1, CV_64FC1, Pix1);
		Pix = Inside*Cam / Cam.at<double>(2, 0);
		uvRedRoll.push_back(Point2d(Pix.at<double>(0, 0), Pix.at<double>(1, 0)));
	}
	for (int i = 0;i < GoldRoll.size();i++)
	{
		Xw = GoldRoll.at(i).x;
		Yw = GoldRoll.at(i).y;
		Zw = GoldRoll.at(i).z;
		double World1[3][1] = { { Xw },{ Yw },{ Zw } };
		Mat World(3, 1, CV_64FC1, World1);
		double Cam1[3][1] = { { Xc },{ Yc },{ Zc } };
		Mat Cam(3, 1, CV_64FC1, Cam1);
		Cam = R*(World + T);
		double Pix1[3][1] = { { u },{ v },{ 1 } };
		Mat Pix(3, 1, CV_64FC1, Pix1);
		Pix = Inside*Cam / Cam.at<double>(2, 0);
		uvGoldRoll.push_back(Point2d(Pix.at<double>(0, 0), Pix.at<double>(1, 0)));
	}
	for (int i = 0;i < BlueRoll.size();i++)
	{
		Xw = BlueRoll.at(i).x;
		Yw = BlueRoll.at(i).y;
		Zw = BlueRoll.at(i).z;
		double World1[3][1] = { { Xw },{ Yw },{ Zw } };
		Mat World(3, 1, CV_64FC1, World1);
		double Cam1[3][1] = { { Xc },{ Yc },{ Zc } };
		Mat Cam(3, 1, CV_64FC1, Cam1);
		Cam = R*(World + T);
		double Pix1[3][1] = { { u },{ v },{ 1 } };
		Mat Pix(3, 1, CV_64FC1, Pix1);
		Pix = Inside*Cam / Cam.at<double>(2, 0);
		uvBlueRoll.push_back(Point2d(Pix.at<double>(0, 0), Pix.at<double>(1, 0)));
	}

	//显示
	namedWindow("img", WINDOW_NORMAL);
	for (int i = 0;i < uvRedRoll.size();i++)
	{
		if (uvRedRoll.at(i).x >= 0 && uvRedRoll.at(i).y >= 0 && uvRedRoll.at(i).y <= 1080 && uvRedRoll.at(i).x <= 1920)
		{
			circle(img, Point2d(uvRedRoll.at(i).x, uvRedRoll.at(i).y), 2, Scalar(0, 0, 255), 2, 8);
			cout << Point(uvRedRoll.at(i).x, uvRedRoll.at(i).y) << endl;
		}
	}
	for (int i = 0;i < uvGoldRoll.size();i++)
	{
		if (uvGoldRoll.at(i).x >= 0 && uvGoldRoll.at(i).y >= 0 && uvGoldRoll.at(i).y <= 1080 && uvGoldRoll.at(i).x <= 1920)
		{
			circle(img, Point2d(uvGoldRoll.at(i).x, uvGoldRoll.at(i).y), 2, Scalar(255, 255, 0), 2, 8);
			cout << Point(uvGoldRoll.at(i).x, uvGoldRoll.at(i).y) << endl;
		}
	}
	for (int i = 0;i < uvBlueRoll.size();i++)
	{
		if (uvBlueRoll.at(i).x >= 0 && uvBlueRoll.at(i).y >= 0 && uvBlueRoll.at(i).y <= 1080 && uvBlueRoll.at(i).x <= 1920)
		{
			circle(img, Point2d(uvBlueRoll.at(i).x, uvBlueRoll.at(i).y), 2, Scalar(255, 255, 0), 2, 8);
			cout << Point(uvBlueRoll.at(i).x, uvBlueRoll.at(i).y) << endl;
		}
	}

	imshow("img", img);
	waitKey(30);


	system("pause");
	return 0;
}

void On_mouse(int event, int x, int y, int flags, void*)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		cout << "Point " << Point(x, y) << endl;
		Y = y;
		X = x;
	}
	if (event == EVENT_RBUTTONDOWN)
	{

	}
}