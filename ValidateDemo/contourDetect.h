# ifndef LINEDETECT_H_
#define LINEDETECT_H_

#include "ordImgProcLib.h"

class contourDetect
{
private:
	float getEucliDistance(int x1, int y1, int x2, int y2){
		return (float)sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
	}

	
	vector<Point> selectPoints(Mat img){
		vector<Point> pts;
		Point lP; 
		bool flag = true;

		for (int i = 0; i < img.rows; i++){//y
			uchar* data = img.ptr<uchar>(i);
			for (int j = 0; j < img.cols; j++){//x
				if (*(data+j) == 255){
					if (flag){//��һ����
						lP.x = j;
						lP.y = i;

						flag = false;
					}
					
					else{
						if (getEucliDistance(lP.x, lP.y, j, i) < 100){//����ͬһ��ĵ㼯
							pts.push_back(Point(j,i));
							
							lP.x = j;
							lP.y = i;
						}

						else{
							break;
						}
					}					
				}	
			}
		}

		return pts;
	}


	//��ȡ������������
	int getAreaMaxContour(vector<vector<Point>> contours)
	{
		double contour_area_temp = 0, contour_area_max = 0;
		int area_max_contour = 0;
		int c;

		vector<vector<Point>>::const_iterator itContours = contours.begin();
		for (c = 0; itContours != contours.end(); ++itContours){
			if (itContours->size() >= 50 && itContours->size() <= 500){
				contour_area_temp = contourArea(contours[c]);
				if (contour_area_temp > contour_area_max){
					contour_area_max = contour_area_temp;
					area_max_contour = c;
				}
			}

			c++;
		}

		return area_max_contour;//����contours���
	}

public:
	contourDetect(){	
	}

	Mat detectLines(Mat img){	
		if (img.channels() == 3){
			cvtColor(img, img, CV_BGR2GRAY);
		}

		Mat edges;
		//���һ�����ص��ݶȴ�������ֵ������Ϊ�Ǳ�Ե���أ����С��������ֵ����������
		//����õ���ݶ�������֮������������������ֵ�����ص�����ʱ���ǲű���������ɾ����
		Canny(img, edges, 125, 350);
		threshold(edges, edges, 128, 255, THRESH_BINARY);

		// ���ֱ�ߣ���СͶƱΪ90������������p7����϶��С��p8
		vector<Vec4i> lines;//��㣺x1,y1; �յ㣺x2,y2
		HoughLinesP(edges, lines, 1, CV_PI / 180, 80, 150, 50);
		cout << "����⵽" << lines.size() << "��ֱ��\n";

		//������⵽��ֱ��
		int cnt=0;//������������������ֱ��
		Mat res(img.size(), CV_8UC1);
		for (int i = 0; i < lines.size(); i++){
			Vec4i l = lines[i];
			double a = (l[3] - l[1]) / (l[2] - l[0]); 
			double b = (l[3] - l[1]) % (l[2] - l[0]);

		
			if (getEucliDistance(l[0], l[1], l[2], l[3]) > 200){ //�߳��޶�		
				if ( a == 0 ){ //��б���޶�
					line(res, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 255, 255), 1, CV_AA);
					cnt++;
				}
			}
		}

		cout << "����������ֱ����" << cnt << "��";
		threshold(res, res, 210, 255, THRESH_BINARY);
		bitwise_and(edges, res, res);


		return res;
	}

	//�������ֱ�ߵ������˵�
	vector<Point> fitLinesPoints(Mat& res){
		vector<Point> lPoints = selectPoints(res);
		//�����㼯
		Mat img(res.size(), CV_8UC3);

		for (int i = 0; i < lPoints.size(); i++){
			Point p = lPoints[i];
			circle(img, p, 1, Scalar(255,0,0));	
		}

		

		//���ֱ��
		Vec4f lineParam;
		//CV_DIST_L2Ϊ��С���˷�
		fitLine(lPoints, lineParam, CV_DIST_L2, 0, 1e-2, 1e-2);

		//����ֱ��
		//��ȡ��бʽ�ĵ��б��  
		Point point0;
		point0.x = lineParam[2];
		point0.y = lineParam[3];

		double k = lineParam[1] / lineParam[0];

		//����ֱ�ߵĶ˵�(y = k(x - x0) + y0)  
		Point point1, point2;
		point1.x = 0;
		point1.y = k * (0 - point0.x) + point0.y;
		point2.x = 640;
		point2.y = k * (640 - point0.x) + point0.y;

		vector<Point> pVec;
		pVec.push_back(point1);
		pVec.push_back(point2);

		cv::line(img, point1, point2, cv::Scalar(0, 0, 255), 2, 8, 0);
		imshow("Classify Points", img);

		return pVec; 
	}

	void hullSubContour(Mat& img){
		cvtColor(img, img, CV_BGR2GRAY);
		threshold(img, img, 143, 255, THRESH_BINARY);
		//imshow("img", img);

		vector<vector<Point> > contours;
		int nMaxCon;
		vector<Vec4i> hierarchy;
		findContours(img, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
		nMaxCon = getAreaMaxContour(contours);

	
		drawContours(img, contours, nMaxCon, Scalar(255, 255, 255), -1, 8);
		imshow("img", img);
	}
};

#endif