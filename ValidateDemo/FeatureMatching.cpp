#include<opencv2/highgui/highgui.hpp>  
#include "opencv2/nonfree/nonfree.hpp"    
#include "opencv2/legacy/legacy.hpp"   

#include <iostream>  

#include "FeatureMatching.h"

using namespace cv;
using namespace std;

FeatureMatching::FeatureMatching(){
}

FeatureMatching::FeatureMatching(Mat img1, Mat img2){
	this->img1 = img1;
	this->img2 = img2;
}


FeatureMatching::~FeatureMatching(){
}

void FeatureMatching::matching(){
	Mat image1(img1.size(), CV_8UC3);
	Mat image2(img1.size(), CV_8UC3);

	image1 = img1.clone();
	image2 = img2.clone();

	//��ȡ������
	SurfFeatureDetector surfDetector(3000);  //hessianThreshold,����������ֵ���������޶�������ĸ���   
	vector<KeyPoint> keyPoint1, keyPoint2;
	surfDetector.detect(image1, keyPoint1);
	surfDetector.detect(image2, keyPoint2);

	//����������
	SurfDescriptorExtractor SurfDescriptor(2500);
	Mat imageDesc1, imageDesc2;
	SurfDescriptor.compute(image1, keyPoint1, imageDesc1);
	SurfDescriptor.compute(image2, keyPoint2, imageDesc2);

	//����������
	//drawKeypoints(image1, keyPoint1, image1, Scalar(255, 0, 0), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	//drawKeypoints(image2, keyPoint2, image2, Scalar(255, 0, 0), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	//imshow("KeyPoints of image1", image1);
	//imshow("KeyPoints of image2", image2);

	
	//������ƥ�䲢��ʾƥ����     
	FlannBasedMatcher matcher;
	vector<DMatch> matchePoints;
	matcher.match(imageDesc1, imageDesc2, matchePoints);

	Mat imageOutput = Mat::zeros(img1.size(), CV_8UC3);;
	drawMatches(img1, keyPoint1, img2, keyPoint2, matchePoints, imageOutput, Scalar::all(-1),
		Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	imshow("AllMatching", imageOutput);

	
	//Mat imageOutput;
	//drawMatches(img1, keyPoint1, img2, keyPoint2, matchePoints, imageOutput);
	//imshow("Mathch Points", imageOutput);

	//��ȡǿ������ ,����ƥ�� 
	double minMatch = 1;
	double maxMatch = 0;
	for (int i = 0; i<matchePoints.size(); i++)
	{
		//ƥ��ֵ�����Сֵ��ȡ  
		minMatch = minMatch>matchePoints[i].distance ? matchePoints[i].distance : minMatch;
		maxMatch = maxMatch<matchePoints[i].distance ? matchePoints[i].distance : maxMatch;
	}

	//��ȡ����ǰ�ߵļ�������ƥ����  
	vector<DMatch> goodMatchePoints;
	for (int i = 0; i<matchePoints.size(); i++)
	{
		if (matchePoints[i].distance<minMatch + (maxMatch - minMatch) / 2)
		{
			goodMatchePoints.push_back(matchePoints[i]);
		}
	}

	//��������ƥ��� 
	drawMatches(img1, keyPoint1, img2, keyPoint2, goodMatchePoints, imageOutput, Scalar::all(-1),
		Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	imshow("BestMatching", imageOutput);

}

