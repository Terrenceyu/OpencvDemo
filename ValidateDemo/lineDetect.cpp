#if 0


using namespace std;

LineDetect::LineDetect(){
}

LineDetect::LineDetect(Mat img){
	this->img = img;
}

LineDetect::~LineDetect(){
}

/*���ͼƬ�ı�Ե - �ɱ�Եͼ����ֱ�� - ѡȡ����ֱ��������ֱ�����*/
void LineDetect::detect(){
	Mat I;
	cvtColor(img, I, CV_BGR2GRAY);
	
	Mat edges;
	//���һ�����ص��ݶȴ�������ֵ������Ϊ�Ǳ�Ե���أ����С��������ֵ����������
	//����õ���ݶ�������֮������������������ֵ�����ص�����ʱ���ǲű���������ɾ����
	Canny(I, edges, 125, 350);
	threshold(edges, edges, 128, 255, THRESH_BINARY);
	//imshow("edges", edges);

	// ���ֱ�ߣ���СͶƱΪ90������������p7����϶��С��p8
	vector<Vec4i> lines; //��㣺x1,y1; �յ㣺x2,y2
	HoughLinesP(edges, lines, 1, CV_PI / 180, 80, 150, 50);
	cout << "����⵽" << lines.size() << "��ֱ��\n";

	//����ֱ��
	Mat lineD = Mat::zeros(img.size(), CV_8UC1);
	double A, B, C;
	
	int cnt = 0;
	for (size_t i = 0; i < lines.size(); i++){ 
		Vec4i l = lines[i];

		//cout << l[0] << ", " << l[1] << "\n";
		//line(lineD, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 255, 255), 1, CV_AA);

		double a = (l[3] - l[1]) / (l[2] - l[0]);
		double b = (l[3] - l[1]) % (l[2] - l[0]);
		if ((a == 0) && (b != 0)){ // 0<k<45��
			if (l[1] <= 280 && l[1] >= 200){ //ѡȡ����ֱ��
				line(lineD, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 255, 255), 1, CV_AA);
				cnt++;
			}	
		}

		//���ֱ�߷��� �� Ax+By+C = 0
		/*A = l[3] - l[1];
		B = l[0] - l[2];
		C = l[1]*l[2] - l[0]*l[3];*/
	}

	cout << "����������ֱ����" << cnt << "��\n";
	

	bitwise_and(edges, lineD, lineD);//����������߰�λ����������ݼ�
	imshow("lineD", lineD);

	//�ѵ㼯�еĵ���뵽������  
	vector<Point> points;
	//����ÿ������  
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

	//�������ֱ�ߵ�����  
	Vec4f line;
	//lineǰ����ֵ ��������ֱ�ߵķ���ĵ�λ������������ֵ�������Ǹ�ֱ��ͨ����һ����
	fitLine(Mat(points), line, CV_DIST_L2, 0, 0.01, 0.01);
	//cout << "line: (" << line[0] << "," << line[1] << ")(" << line[2] << "," << line[3] << ")\n";


	Mat res = Mat::zeros(lineD.size(), CV_8UC1);
	int x0 = line[2];
	int y0 = line[3];

	int x1 = x0 - 200 * line[0];
	int y1 = y0 - 200 * line[1];
	//�������ֱ��
	cv::line(res, Point(x0, y0), Point(x1, y1), Scalar(255,255,255), 1);

	//���ֱ�߷��� �� Ax+By+C = 0
	A = y1 - y0;
	B = x0 - x1;
	C = y0*x1 - x0*y1;
	//�������
	int potX = 3; int potY = 141;
	int dis = abs(A * potX + B * potY + C) / sqrt(A * A + B * B);
	cout << "��("<< potX << ", " << potY  << ")" << "��ֱ�ߵľ���Ϊ��" << dis;

	imshow("res", res);
	waitKey(0);
}

#endif