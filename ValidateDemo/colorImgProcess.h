#ifndef COLORIMGPROCESS_H_
#define COLORIMGPROCESS_H_
#include"ordImgProcLib.h"

class ColorImgProcess{
private:
	int channels[3];
	int histSize[3];
	const float* ranges[3];
	float hRanges[2];

public:
	ColorImgProcess(){
		histSize[0] = 255;
		histSize[1] = 255;
		histSize[2] = 255;

		hRanges[0] = 0.0;
		hRanges[1] = 255.0;

		ranges[0] = hRanges;
		ranges[1] = hRanges;
		ranges[2] = hRanges;
	}

	Mat colorReduce(Mat img, int div=16){
		int n = static_cast<int>(log(static_cast<double>(div)) / log(2.0));
		// mask used to round the pixel value
		uchar mask = 0xFF << n; // e.g. for div=16, mask= 0xF0

		for (int i = 0; i < img.rows; i++){
			Vec3b* data = img.ptr<Vec3b>(i);
			for (int j = 0; j < img.cols; j++){
				(*(data + j))[0] = ((*(data + j))[0] & mask) + div / 2;
				(*(data + j))[1] = ((*(data + j))[1] & mask) + div / 2;
				(*(data + j))[2] = ((*(data + j))[2] & mask) + div / 2;
			}
		}

		return img;
	}

	MatND getHistgram(Mat img){
		MatND hist;

		calcHist(&img,
			1, // histogram of 1 image only
			channels, // the channel used
			cv::Mat(), // no mask is used
			hist, // the resulting histogram
			3, // it is a 3D histogram
			histSize, // number of bins
			ranges // pixel value range
			);

		return hist;
	}

	SparseMat getSparseHistogram(const Mat &image) {
		SparseMat hist(3, histSize, CV_32F);

		calcHist(&image,
			1, // histogram of 1 image only
			channels, // the channel used
			cv::Mat(), // no mask is used
			hist, // the resulting histogram
			3, // it is a 3D histogram
			histSize, // number of bins
			ranges // pixel value range
			);

		return hist;
	}


	// Computes the 1D Hue histogram with a mask.
	// BGR source image is converted to HSV
	MatND getHueHistogram(const Mat image) {
		MatND hist;

		// Convert to Lab color space
		Mat hsv;
		cvtColor(image, hsv, CV_BGR2HSV);

		// Prepare arguments for a 1D hue histogram
		hRanges[0] = 0.0;
		hRanges[1] = 180.0;
		channels[0] = 0; // the hue channel 

		// Compute histogram
		cv::calcHist(&hsv,
			1,			// histogram of 1 image only
			channels,	// the channel used
			Mat(),	// no mask is used
			hist,		// the resulting histogram
			1,			// it is a 1D histogram
			histSize,	// number of bins
			ranges		// pixel value range
			);

		return hist;
	}

	Mat findByHueHistBackProj(Mat roi, Mat img){
		MatND roiHist = getHueHistogram(roi);

		Mat result;
		calcBackProject(&img, 1, channels, roiHist, result, ranges, 255.0);

		return result;
	}
};


#endif