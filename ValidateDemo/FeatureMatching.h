#ifndef  FEATUREMATCHING_H_
#define FEATUREMATCHING_H_

#include<opencv2/highgui/highgui.hpp>  
using namespace cv;

class FeatureMatching{

public:
	FeatureMatching();
	FeatureMatching(Mat img1, Mat img2);

	virtual ~FeatureMatching();

	void matching();

private:
    Mat img1; 
	Mat img2;
};

#endif