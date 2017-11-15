#ifndef TUBERENOISE_H_
#define TUBERENOISE_H_
#include "ordImgProcLib.h"

class Tube{
private:

public:
	Mat denoise(Mat& img){
		Mat res;

		if (img.channels() == 3)
			cvtColor(img, img, COLOR_BGR2GRAY);//ͼ��ҶȻ�  

		threshold(img, img, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);//��� 
		
		img.copyTo(res);

		//��ͨ��ѡ��
		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;

		findContours(img, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
		int c;

		vector<Point> targetPoint;
		vector<vector<Point>>::const_iterator itContours = contours.begin();
		for (c = 0; itContours != contours.end(); ++itContours){
			if (itContours->size() <= 10){//΢������
				
			}

			//if (&& itContours->size() <= 25 ){//����޶�
			//	if (itContours->at(c).y > img.rows/2){//�������ϰ����Ĳ�������С����
			//		drawContours(res, contours, c, Scalar(255), -1, 8);
			//		//��¼Ŀ������(������С��������)��λ������
			//		targetPoint.push_back(itContours->at(c));
			//	}
			//}

			c++;
		}

	
		for (int i = 0; i < targetPoint.size(); i++){
			cout << targetPoint.at(i).x << ":" << targetPoint.at(i).y << "\n";
		}
	
		return res;
	}
};


#endif