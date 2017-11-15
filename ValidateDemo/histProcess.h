#ifndef HISTPROCESS_H_
#define HISTPROCESS_H_

#include "ordImgProcLib.h"

class HistProcess{
private:
	int histSize[1];
	float hRanges[2];
	const float* ranges[1];
	int channels[1];

	//Apply look-up tables 查找表
	//lookup[] = [0,0,0,''',100,100,100,''',255,255,255]
	//当把此表格应用到图片时，图片0-100灰度的像素灰度就变成0，101-200的变成100,201-255的就变成255
	Mat applyLookUp(const Mat& image, // input image
		const Mat& lookup) { // 1x256 uchar matrix
		// the output image
		Mat result;
		// apply lookup table
		LUT(image, lookup, result);

		return result;
	}


public:
	HistProcess(){
		histSize[0] = 256;

		hRanges[0] = 0.0;
		hRanges[1] = 255.0;

		ranges[0] = hRanges;

		channels[0] = 0;

	}
	
	MatND getHistogram(const Mat& img){
		MatND hist;

		calcHist(&img, 1, // histogram from 1 image only
			channels, Mat(),// no mask is used
			hist, 1, // it is a 1D histogram
			histSize, ranges);

		return hist;
	}

	Mat getHistograyImg(Mat& img){
		MatND hist = getHistogram(img);

		// Get min and max bin values
		double maxVal = 0;
		double minVal = 0;
		minMaxLoc(hist, &minVal, &maxVal, 0, 0);
		// Image on which to display histogram
		Mat histImg(histSize[0], histSize[0], CV_8U, Scalar(255));

		// set highest point at 90% of nbins
		int hpt = static_cast<int>(0.9*histSize[0]);

		// Draw a vertical line for each bin
		for (int h = 0; h < histSize[0]; h++) {
			float binVal = hist.at<float>(h);
			int intensity = static_cast<int>(binVal*hpt / maxVal);

			// This function draws a line between 2 points
			cv::line(histImg, cv::Point(h, histSize[0]),
				cv::Point(h, histSize[0] - intensity),
				cv::Scalar::all(0));
		}

		return histImg;
	}

	//直方图均衡化
	Mat histEqualization(Mat& img){
		if (img.channels() == 3)
			cvtColor(img, img, CV_BGR2GRAY);

		Mat dst(img.size(), CV_8UC1);
		equalizeHist(img, dst);

		return dst;
	}

	//灰度拉伸
	Mat stretch(const Mat &image, int minValue) {
		// Compute histogram first
		MatND hist = getHistogram(image);

		// find left extremity of the histogram
		int imin = 0;
		for (; imin < histSize[0]; imin++){
			if (hist.at<float>(imin) > minValue){
				break;
			}
		}


		// find right extremity of the histogram
		int imax = histSize[0] - 1;
		for (; imax >= 0; imax--){
			if (hist.at<float>(imax) > minValue){
				break;
			}
		}

		std::cout << imin << "---" << imax;
		// Create lookup table
		int dim(256);
		Mat lookup(1, &dim, CV_8U);

		// Build lookup table
		for (int i = 0; i < 256; i++){
			if (i < imin){
				lookup.at<uchar>(i) = 0;
			}
			else if (i > imax){
				lookup.at<uchar>(i) = 255;
			}
			else{
				lookup.at<uchar>(i) = static_cast<uchar>(
					255.0*(i - imin) / (imax - imin) + 0.5);
			}
		}

		// Apply lookup table
		Mat result;
		result = applyLookUp(image, lookup);

		return result;
	}

	//根据roi灰度直方图反投影查找相似区域
	Mat findAreaByHistBackProj(const Mat roi, const Mat img){
		Mat result;

		//计算灰度图像直方图
		MatND hist = getHistogram(roi);
		//归一化
		normalize(hist, hist, 1.0);

		calcBackProject(&img, 1, channels, hist, result, ranges, 255.0);

		// 亮起的区域是目标区域的概率更大(255),而更暗的区域则表示更低的概率(0)
		threshold(result, result, 0.01*255, 255, THRESH_BINARY);

		return result;
	}
};

#endif