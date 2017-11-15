#ifndef BGSUBTRACTOR_H_
#define BGSUBTRACTOR_H_

#include"ordImgProcLib.h"

class bgSubtractor{
private:
	
public:
	void videoStractorByMog2(VideoCapture& video){
		Mat frame;
		Mat fMask;//ǰ��mask
		Mat th;//��ֵ��ͼ��

		double rate = video.get(CV_CAP_PROP_FPS);
		int delay = 1000 / rate;

		BackgroundSubtractorMOG2 bgMog2(20, 30, true);

		bool stop(false);
		while (!stop){
			if (! video.read(frame)){
				break;
			}

			// press any key to stop  
			if (waitKey(delay) >= 0)
				stop = true;


			bgMog2(frame, fMask, 0.01);
			

			//ǰ�������ֵ�������ǰ�ɫ��0 - 245���ķ�ǰ�����򣨰��������Լ���Ӱ������Ϊ0��ǰ���İ�ɫ��245 - 255������Ϊ255
			threshold(fMask, th, 245, 255, THRESH_BINARY);

			//��ֵ����̬ѧ����


			//Ѱ��������
			vector<vector<Point>> contours;
			vector<Vec4i> hierarchy;
			
			findContours(th, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

			//Ѱ���������������ؾ���
			vector<vector<Point>>::const_iterator itContours = contours.begin();
			for (int c=0; itContours != contours.end(); ++itContours){
				if (itContours->size() >= 100){
					Rect r = boundingRect(contours[c]);
					rectangle(frame, r, Scalar(0,255,0), 2);
				}

				c++;
			}

			imshow("fmask", fMask);
			imshow("frame", frame);
		}

		video.release();
	}
};


#endif