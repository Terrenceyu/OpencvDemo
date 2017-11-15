#if 0


using namespace std;

LineDetect::LineDetect(){
}

LineDetect::LineDetect(Mat img){
	this->img = img;
}

LineDetect::~LineDetect(){
}

/*检测图片的边缘 - 由边缘图像检测直线 - 选取部分直线坐标做直线拟合*/
void LineDetect::detect(){
	Mat I;
	cvtColor(img, I, CV_BGR2GRAY);
	
	Mat edges;
	//如果一个像素的梯度大与上限值，则被认为是边缘像素，如果小于下限阈值，则被抛弃。
	//如果该点的梯度在两者之间则当这个点与高于上限值的像素点连接时我们才保留，否则删除。
	Canny(I, edges, 125, 350);
	threshold(edges, edges, 128, 255, THRESH_BINARY);
	//imshow("edges", edges);

	// 检测直线，最小投票为90，线条不短于p7，间隙不小于p8
	vector<Vec4i> lines; //起点：x1,y1; 终点：x2,y2
	HoughLinesP(edges, lines, 1, CV_PI / 180, 80, 150, 50);
	cout << "共检测到" << lines.size() << "条直线\n";

	//画出直线
	Mat lineD = Mat::zeros(img.size(), CV_8UC1);
	double A, B, C;
	
	int cnt = 0;
	for (size_t i = 0; i < lines.size(); i++){ 
		Vec4i l = lines[i];

		//cout << l[0] << ", " << l[1] << "\n";
		//line(lineD, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 255, 255), 1, CV_AA);

		double a = (l[3] - l[1]) / (l[2] - l[0]);
		double b = (l[3] - l[1]) % (l[2] - l[0]);
		if ((a == 0) && (b != 0)){ // 0<k<45°
			if (l[1] <= 280 && l[1] >= 200){ //选取部分直线
				line(lineD, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 255, 255), 1, CV_AA);
				cnt++;
			}	
		}

		//求出直线方程 ： Ax+By+C = 0
		/*A = l[3] - l[1];
		B = l[0] - l[2];
		C = l[1]*l[2] - l[0]*l[3];*/
	}

	cout << "符合条件的直线有" << cnt << "条\n";
	

	bitwise_and(edges, lineD, lineD);//将轮廓与白线按位与做拟合数据集
	imshow("lineD", lineD);

	//把点集中的点插入到向量中  
	vector<Point> points;
	//遍历每个像素  
	for (int y = 0; y < lineD.rows; y++)
	{
		uchar* rowPtr = lineD.ptr<uchar>(y);
		for (int x = 0; x < lineD.cols; x++)
		{
			if (rowPtr[x])
			{
				points.push_back(Point(x, y));
			}
		}
	}

	//储存拟合直线的容器  
	Vec4f line;
	//line前两个值 给出的是直线的方向的单位向量，后两个值给出的是该直线通过的一个点
	fitLine(Mat(points), line, CV_DIST_L2, 0, 0.01, 0.01);
	//cout << "line: (" << line[0] << "," << line[1] << ")(" << line[2] << "," << line[3] << ")\n";


	Mat res = Mat::zeros(lineD.size(), CV_8UC1);
	int x0 = line[2];
	int y0 = line[3];

	int x1 = x0 - 200 * line[0];
	int y1 = y0 - 200 * line[1];
	//画出拟合直线
	cv::line(res, Point(x0, y0), Point(x1, y1), Scalar(255,255,255), 1);

	//求出直线方程 ： Ax+By+C = 0
	A = y1 - y0;
	B = x0 - x1;
	C = y0*x1 - x0*y1;
	//计算距离
	int potX = 3; int potY = 141;
	int dis = abs(A * potX + B * potY + C) / sqrt(A * A + B * B);
	cout << "点("<< potX << ", " << potY  << ")" << "到直线的距离为：" << dis;

	imshow("res", res);
	waitKey(0);
}

#endif