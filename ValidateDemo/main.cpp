#include<iostream>  

#include "FeatureMatching.h"
#include "contourDetect.h"
#include "bgSubtractor.h"
#include "histProcess.h"
#include "colorImgProcess.h"
#include "tubeDenoise.h"
//0: THRESH_BINARY  当前点值大于阈值时，取Maxval, 也就是第四个参数，下面再不说明，否则设置为0
//1 : THRESH_BINARY_INV 当前点值大于阈值时，设置为0，否则设置为Maxval
//2 : THRESH_TRUNC 当前点值大于阈值时，设置为阈值，否则不改变
//3 : THRESH_TOZERO 当前点值大于阈值时，不改变，否则设置为0
//4 : THRESH_TOZERO_INV  当前点值大于阈值时，设置为0，否则不改变

#if 0

int hullSubContourDemo(Mat img){
	//change the image to binary by setting a threshold  
	threshold(img, img, 120, 255, THRESH_BINARY);
	//imshow("img", img);

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(img, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
	if (!contours.size()){
		return -1;
	}

	//画出所有的轮廓
	Mat resultImage = Mat::zeros(img.size(), CV_8U);
	
	//drawContours(resultImage, contours, -1, Scalar(255, 0, 0));
	//imshow("img", resultImage);


	int nMaxCon;
	nMaxCon = GetAreaMaxContour(contours);
	if (nMaxCon < 0)
	{
		return -1;
	}

	//画出最大轮廓 
	drawContours(resultImage, contours, nMaxCon, Scalar(255, 255, 255), -1, 8);
	//imshow("img", resultImage);

	//逆时针找到最大轮廓点的凹包
	vector<vector<Point>>hull(1);
	convexHull(Mat(contours[nMaxCon]), hull[0], false);

	//画出凹包
	Mat Hull = Mat::zeros(img.size(), CV_8UC1);
	drawContours(Hull, hull, 0, Scalar(125), -1, 8, vector<Vec4i>(), 0, Point());
	//imshow("img", Hull);


	//计算轮廓与凹包差值
	Mat Diff = Mat::zeros(img.size(), CV_8UC1);
	Diff = Hull - resultImage;
	//imshow("img", Diff);

	//差值像素区域开运算处理
	Mat Out = Mat::zeros(img.size(), CV_8UC1);
	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
	morphologyEx(Diff, Out, MORPH_OPEN, element);
	imshow("img", Out);

	

	return 0;
}



struct outVal{
	int out1;
	int out2;
	int out3;
};



//获取最大轮廓的椭圆逼近的 圆心坐标和(Ellipse.size.height + Ellipse.size.width) / 4;
int contoursFitEllipseDemo(Mat img, outVal o1){
	Mat out = Mat::zeros(img.size(), CV_8UC1);

	threshold(img, out, 120, 255, THRESH_BINARY);

	vector<vector<Point> > contours;
	int nMaxCon;
	vector<Vec4i> hierarchy;

	//寻找轮廓
	findContours(out, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	if (!contours.size())
		return -1;

	//找面积最大的轮廓
	nMaxCon = GetAreaMaxContour(contours);

	//画出最大轮廓 
	Mat resultImage = Mat::zeros(img.size(), CV_8U);
	//drawContours(resultImage, contours, nMaxCon, Scalar(255, 255, 255), -1, 8);
	//imshow("img", resultImage);

	//最大轮廓的椭圆逼近
	RotatedRect Ellipse = fitEllipse(contours[nMaxCon]);
	ellipse(resultImage, Ellipse, cvScalar(255, 0, 0, 0));
	imshow("img", resultImage);

	o1.out1 = Ellipse.center.x;
	o1.out2 = Ellipse.center.y;
	o1.out3 = (Ellipse.size.height + Ellipse.size.width) / 4;

	cout << "out1:" <<  o1.out1 << " out2:" << o1.out2 << " out3:" << o1.out3;

	return 0;
}

#endif


void vectorDemo(){
	vector<vector<Point>> ptsVec;

	int cnt = 0;

	while (cnt <= 3)
	{
		vector<Point> pts;
		for (int i = 0; i < 20; i++){
			pts.push_back(Point(i, i+1));
		}

		ptsVec.push_back(pts);

		cnt++;
	}

	for (size_t i = 0; i < ptsVec.size(); i++){
		vector<Point> pts = ptsVec[i];

		for (size_t j = 0; j < pts.size(); j++){
			Point p = pts[j];

			cout << p.x << ", " << p.y << "\n";
		}
	}

	system("pause");
}

int main()
{
	int ret = 0; 
	/*
	Mat Img = imread("D://imgData/gear.jpg", 0);
	if (!Img.data){ 
		cout << "Can't find image!"; return -1; 
	}

	
	//ret = hullSubContourDemo(Img);

	//outVal o1 = {0,0,0};
	//ret = contoursFitEllipseDemo(Img, o1);

	
	//自适应阈值二值化函数测试
	Mat Out = Mat::zeros(Img.size(), CV_8UC1);;
	//p4: CV_ADAPTIVE_THRESH_MEAN_C 或 CV_ADAPTIVE_THRESH_GAUSSIAN_C
	//p5: CV_THRESH_BINARY,CV_THRESH_BINARY_INV
	int blockSize = 15;//block_size用来计算阈值的象素邻域大小: 3, 5, 7, ...
	int constValue = 7;//与方法有关的参数。对方法 CV_ADAPTIVE_THRESH_MEAN_C 和 CV_ADAPTIVE_THRESH_GAUSSIAN_C， 它是一个从均值或加权均值提取的常数（见讨论）, 尽管它可以是负数。

	adaptiveThreshold(Img, Out, 255, ADAPTIVE_THRESH_GAUSSIAN_C, ADAPTIVE_THRESH_GAUSSIAN_C, blockSize, constValue);
	//imshow("img", Out);
	*/

	//特征匹配 SURF
	//Mat img1 = imread("D://imgData/gear_01.jpg");
	//Mat img2 = imread("D://imgData/gear_02.jpg");

	//FeatureMatching fm(img1, img2);
	//fm.matching();

	
	//Mat img = imread("E://imgData/gear.jpg");
	//contourDetect ld;
	//直线检测和拟合
	//Mat res = ld.detectLines(img);
	//ld.fitLinesPoints(res);

	//hull - contour
	//ld.hullSubContour(img);

	//背景分割器
	/*bgSubtractor bs;
	VideoCapture video("E:/imgData/video/traffic.flv");
	bs.videoStractorByMog2(video);*/

	//直方图处理
	/*
	Mat img = imread("E://imgData/sky.jpg");
	
	
	ColorImgProcess cip;
	HistProcess hp;
	Mat dst;
	//dst = hp.histEqualization(img);
	//dst = hp.getHistograyImg(img);
	//imshow("dst", dst);
	dst = cip.colorReduce(img);
	rectangle(dst, Rect(0, 0, 15, 20), Scalar(255), 1, 1, 0);
	imshow("dst", dst);
	dst = cip.findByHueHistBackProj(dst(Rect(0,0,15,20)), dst);

	imshow("img", dst);
	*/

	//数码管去噪声
	Mat img = imread("E://imgData/tube.png");
	Tube tube;

	Mat res = tube.denoise(img);

	imshow("img", img);
	imshow("res", res);

	waitKey(0);
	return ret;
}

