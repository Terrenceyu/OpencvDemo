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
					if (flag){//第一个点
						lP.x = j;
						lP.y = i;

						flag = false;
					}
					
					else{
						if (getEucliDistance(lP.x, lP.y, j, i) < 100){//属于同一类的点集
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


	//获取最大轮廓的序号
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

		return area_max_contour;//返回contours序号
	}

public:
	contourDetect(){	
	}

	Mat detectLines(Mat img){	
		if (img.channels() == 3){
			cvtColor(img, img, CV_BGR2GRAY);
		}

		Mat edges;
		//如果一个像素的梯度大与上限值，则被认为是边缘像素，如果小于下限阈值，则被抛弃。
		//如果该点的梯度在两者之间则当这个点与高于上限值的像素点连接时我们才保留，否则删除。
		Canny(img, edges, 125, 350);
		threshold(edges, edges, 128, 255, THRESH_BINARY);

		// 检测直线，最小投票为90，线条不短于p7，间隙不小于p8
		vector<Vec4i> lines;//起点：x1,y1; 终点：x2,y2
		HoughLinesP(edges, lines, 1, CV_PI / 180, 80, 150, 50);
		cout << "共检测到" << lines.size() << "条直线\n";

		//画出检测到的直线
		int cnt=0;//用作计数符合条件的直线
		Mat res(img.size(), CV_8UC1);
		for (int i = 0; i < lines.size(); i++){
			Vec4i l = lines[i];
			double a = (l[3] - l[1]) / (l[2] - l[0]); 
			double b = (l[3] - l[1]) % (l[2] - l[0]);

		
			if (getEucliDistance(l[0], l[1], l[2], l[3]) > 200){ //线长限定		
				if ( a == 0 ){ //线斜率限定
					line(res, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 255, 255), 1, CV_AA);
					cnt++;
				}
			}
		}

		cout << "符合条件的直线有" << cnt << "条";
		threshold(res, res, 210, 255, THRESH_BINARY);
		bitwise_and(edges, res, res);


		return res;
	}

	//返回拟合直线的两个端点
	vector<Point> fitLinesPoints(Mat& res){
		vector<Point> lPoints = selectPoints(res);
		//画出点集
		Mat img(res.size(), CV_8UC3);

		for (int i = 0; i < lPoints.size(); i++){
			Point p = lPoints[i];
			circle(img, p, 1, Scalar(255,0,0));	
		}

		

		//拟合直线
		Vec4f lineParam;
		//CV_DIST_L2为最小二乘法
		fitLine(lPoints, lineParam, CV_DIST_L2, 0, 1e-2, 1e-2);

		//画出直线
		//获取点斜式的点和斜率  
		Point point0;
		point0.x = lineParam[2];
		point0.y = lineParam[3];

		double k = lineParam[1] / lineParam[0];

		//计算直线的端点(y = k(x - x0) + y0)  
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