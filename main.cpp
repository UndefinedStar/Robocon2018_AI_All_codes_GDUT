#include<iostream>
#include<fstream>
#include<opencv2\core.hpp>
#include<highgui.hpp>
#include<imgproc.hpp>
#include<sl/Camera.hpp>
#include<io.h>

void On_mouse1(int event, int x, int y, int flags, void*);
void On_mouse2(int event, int x, int y, int flags, void*);
sl::Camera zed;
sl::Mat slpicPoint;
cv::Point3d pointthree;
std::ofstream outfile;
int currentFrameNum = 0;
short Num = 1;
std::string filename4pic;
std::string filename4picDepth;
cv::Mat pic_flat, pic_out;
cv::Mat depth_flat, depth_out;

int main()
{
	cv::namedWindow("cvpicDepth", cv::WINDOW_NORMAL);
	cv::namedWindow("cvpic", cv::WINDOW_NORMAL);

	std::string filename;
	std::cout << "输入svo文件名称（不用加后缀名）" << std::endl;
	std::cin >> filename;
	sl::InitParameters initVideo(sl::RESOLUTION_HD1080);
	initVideo.svo_input_filename = filename.append(".svo").c_str();
	zed.open(initVideo);
	while (zed.grab() != sl::SUCCESS);

	//程序说明
	std::cout << "暂停-p " << "退出-q" << std::endl;
	std::cout << "鼠标左键双击窗口截图" << std::endl;
	std::cout << "如果是下一段运动，请在深度图下用鼠标右键点击任意位置，程序将记录下一条曲线" << std::endl;

	//打开excel文件
	std::string excelFilename;
	std::cout << "请输入要创建的excel文件名称（不用加后缀名） " << std::endl;
	std::cin >> excelFilename;
	outfile.open(excelFilename.append(".csv"), std::ios::trunc);

	int videoHeight = 0, videoWidth = 0;
	int videoFPS = 0;
	unsigned int videoFrameDrop = 0;
	videoHeight = zed.getResolution().height;//高
	videoWidth = zed.getResolution().width;//宽
	videoFPS = zed.getCameraFPS();//帧率
	videoFrameDrop = zed.getFrameDroppedCount();//掉帧数
	outfile << "相机参数" << std::endl;
	outfile << "Height : " << " , " << videoHeight << " , " << "Width : " << " , " << videoWidth << " , " << "FPS : " << " , " << videoFPS << " , " << "DropFrameNum : " << " , " << videoFrameDrop << std::endl;
	sl::CalibrationParameters calib_parameters;
	calib_parameters = zed.getCameraInformation().calibration_parameters_raw;//标定参数//内参//做成数组输出
	outfile << "相机内参" << std::endl;
	outfile << "左相机内参" << std::endl;
	outfile << "cx" << " , " << calib_parameters.left_cam.cx << " , " << "cy" << " , " << calib_parameters.left_cam.cy;
	outfile << "fx" << " , " << calib_parameters.left_cam.fx << " , " << "fy" << " , " << calib_parameters.left_cam.fy;
	outfile << std::endl;
	outfile << "右相机内参" << std::endl;
	outfile << "cx" << " , " << calib_parameters.right_cam.cx << " , " << "cy" << " , " << calib_parameters.right_cam.cy;
	outfile << "fx" << " , " << calib_parameters.right_cam.fx << " , " << "fy" << " , " << calib_parameters.right_cam.fy;
	outfile << std::endl;
	outfile << "三维点数据" << " x " << " , " << " y " << " , " << " z " << " , " << std::endl;
	outfile << "第" << Num << "条" << std::endl;

	sl::Mat slpic;
	sl::Mat slpicDepth;
	bool quit = true;
	uint64_t timestamps = 0;
	cv::setMouseCallback("cvpicDepth", On_mouse1, 0);
	cv::setMouseCallback("cvpic", On_mouse2, 0);

	while (quit)
	{
		while (zed.grab() != sl::SUCCESS);
		zed.retrieveMeasure(slpicPoint, sl::MEASURE_XYZBGRA);
		zed.retrieveImage(slpic, sl::VIEW_LEFT);
		cv::Mat cvpic(videoHeight, videoWidth, CV_8UC4, slpic.getPtr<sl::uchar1>(sl::MEM_CPU));
		zed.retrieveImage(slpicDepth, sl::VIEW_DEPTH);
		cv::Mat cvpicDepth(videoHeight, videoWidth, CV_8UC4, slpicDepth.getPtr<sl::uchar1>(sl::MEM_CPU));
		timestamps = zed.getCameraTimestamp();//时间戳
		currentFrameNum++;//当前帧

		cv::transpose(cvpic, pic_flat);
		cv::flip(pic_flat, pic_out, 1);
		cv::imshow("cvpic", pic_out);//输出彩色图
		cv::transpose(cvpicDepth, depth_flat);
		cv::flip(depth_flat, depth_out, 1);
		cv::imshow("cvpicDepth", depth_out);//输出深度图

	    int c = cv::waitKey(30);
		if (c == 112)//p = pause
		{
			cv::waitKey(0);
		}
		if (c == 113)//q = quit
			quit = !quit;
		}
	outfile.close();
	system("pause");
	return 0;
}

void On_mouse1(int event, int x, int y, int flags, void*)
{
	if (event == cv::EVENT_LBUTTONDOWN)
	{
		sl::float4 value;
		slpicPoint.getValue(x, y, &value);//三维坐标点
		pointthree.x = value.x;
		pointthree.y = value.y;
		pointthree.z = value.z;
		std::cout << pointthree << std::endl;
		outfile << "第 " << currentFrameNum << " 帧" << " , " << pointthree << " , ";
	}
	if (event == cv::EVENT_RBUTTONDOWN)
	{
		outfile << std::endl;
		++Num;
		outfile << "第" << Num << "条" << std::endl;
	}
	if (event == cv::EVENT_LBUTTONDBLCLK)
	{
		filename4picDepth = std::to_string(currentFrameNum);
		filename4picDepth.append("depthframe").append(".png");//保存的文件的后缀名
		cv::imwrite(filename4picDepth, depth_out);
	}
}
void On_mouse2(int event, int x, int y, int flags, void*)
{
	if (event == cv::EVENT_LBUTTONDBLCLK)
	{
		filename4pic = std::to_string(currentFrameNum);
		filename4pic.append("colorframe").append(".png");//保存的文件的后缀名
		cv::imwrite(filename4pic, pic_out);
	}
}