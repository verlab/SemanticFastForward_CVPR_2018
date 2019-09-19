#pragma once

class ImageStats
{
public:
	ImageStats(int track_width, int track_height);
	~ImageStats(void);

	void EvaluateStats(IplImage* img);

	double GetCannyEdgeCount(cv::Point2d window_center);
	double GetGFTTCount(cv::Point2d window_center);
	double GetWarpingErr(IplImage* prev_img, IplImage* cur_img, cv::Point2d prev_loc, cv::Point2d cur_loc);

private:
	double GetRegionSum(cv::Point2d window_center, cv::Mat& img);
	cv::Mat GetCroppedImg(cv::Mat& frame, cv::Point2d window_center);

private:
	int _blockWidth, _blockHeight;
	cv::Mat _grayImg, _cannyEdges, _cannyIntegralImg;
	cv::Mat _regionCountImg;
	cv::Ptr<cv::FeatureDetector> _detector;
	cv::Mat _gfttImg, _gfttIntegralImg;
};
