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
	std::cout << "����svo�ļ����ƣ����üӺ�׺����" << std::endl;
	std::cin >> filename;
	sl::InitParameters initVideo(sl::RESOLUTION_HD1080);
	initVideo.svo_input_filename = filename.append(".svo").c_str();
	zed.open(initVideo);
	while (zed.grab() != sl::SUCCESS);

	//����˵��
	std::cout << "��ͣ-p " << "�˳�-q" << std::endl;
	std::cout << "������˫�����ڽ�ͼ" << std::endl;
	std::cout << "�������һ���˶����������ͼ��������Ҽ��������λ�ã����򽫼�¼��һ������" << std::endl;

	//��excel�ļ�
	std::string excelFilename;
	std::cout << "������Ҫ������excel�ļ����ƣ����üӺ�׺���� " << std::endl;
	std::cin >> excelFilename;
	outfile.open(excelFilename.append(".csv"), std::ios::trunc);

	int videoHeight = 0, videoWidth = 0;
	int videoFPS = 0;
	unsigned int videoFrameDrop = 0;
	videoHeight = zed.getResolution().height;//��
	videoWidth = zed.getResolution().width;//��
	videoFPS = zed.getCameraFPS();//֡��
	videoFrameDrop = zed.getFrameDroppedCount();//��֡��
	outfile << "�������" << std::endl;
	outfile << "Height : " << " , " << videoHeight << " , " << "Width : " << " , " << videoWidth << " , " << "FPS : " << " , " << videoFPS << " , " << "DropFrameNum : " << " , " << videoFrameDrop << std::endl;
	sl::CalibrationParameters calib_parameters;
	calib_parameters = zed.getCameraInformation().calibration_parameters_raw;//�궨����//�ڲ�//�����������
	outfile << "����ڲ�" << std::endl;
	outfile << "������ڲ�" << std::endl;
	outfile << "cx" << " , " << calib_parameters.left_cam.cx << " , " << "cy" << " , " << calib_parameters.left_cam.cy;
	outfile << "fx" << " , " << calib_parameters.left_cam.fx << " , " << "fy" << " , " << calib_parameters.left_cam.fy;
	outfile << std::endl;
	outfile << "������ڲ�" << std::endl;
	outfile << "cx" << " , " << calib_parameters.right_cam.cx << " , " << "cy" << " , " << calib_parameters.right_cam.cy;
	outfile << "fx" << " , " << calib_parameters.right_cam.fx << " , " << "fy" << " , " << calib_parameters.right_cam.fy;
	outfile << std::endl;
	outfile << "��ά������" << " x " << " , " << " y " << " , " << " z " << " , " << std::endl;
	outfile << "��" << Num << "��" << std::endl;

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
		timestamps = zed.getCameraTimestamp();//ʱ���
		currentFrameNum++;//��ǰ֡

		cv::transpose(cvpic, pic_flat);
		cv::flip(pic_flat, pic_out, 1);
		cv::imshow("cvpic", pic_out);//�����ɫͼ
		cv::transpose(cvpicDepth, depth_flat);
		cv::flip(depth_flat, depth_out, 1);
		cv::imshow("cvpicDepth", depth_out);//������ͼ

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
		slpicPoint.getValue(x, y, &value);//��ά�����
		pointthree.x = value.x;
		pointthree.y = value.y;
		pointthree.z = value.z;
		std::cout << pointthree << std::endl;
		outfile << "�� " << currentFrameNum << " ֡" << " , " << pointthree << " , ";
	}
	if (event == cv::EVENT_RBUTTONDOWN)
	{
		outfile << std::endl;
		++Num;
		outfile << "��" << Num << "��" << std::endl;
	}
	if (event == cv::EVENT_LBUTTONDBLCLK)
	{
		filename4picDepth = std::to_string(currentFrameNum);
		filename4picDepth.append("depthframe").append(".png");//������ļ��ĺ�׺��
		cv::imwrite(filename4picDepth, depth_out);
	}
}
void On_mouse2(int event, int x, int y, int flags, void*)
{
	if (event == cv::EVENT_LBUTTONDBLCLK)
	{
		filename4pic = std::to_string(currentFrameNum);
		filename4pic.append("colorframe").append(".png");//������ļ��ĺ�׺��
		cv::imwrite(filename4pic, pic_out);
	}
}