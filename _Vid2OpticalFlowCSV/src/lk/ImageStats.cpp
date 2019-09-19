#include "stdafx.h"
#include "ImageStats.h"
using namespace cv;

ImageStats::ImageStats(int track_width, int track_height)
:_blockWidth(track_width), _blockHeight(track_height)
{
	if(OUT_GFTT_COUNT)
		_detector = new cv::GFTTDetector((PROCESSING_WIDTH*PROCESSING_HEIGHT)/100, 0.01);
}

ImageStats::~ImageStats(void)
{
}

void ImageStats::EvaluateStats(IplImage* img)
{
	Mat frame(img);

	if(OUT_CANNY_COUNT)
	{
		cvtColor(frame, _grayImg, CV_BGR2GRAY );

		/// Reduce noise with a kernel 3x3
		blur(_grayImg, _cannyEdges, Size(3,3));

		int edgeThresh = 1;
		int lowThreshold = 100;
		int const max_lowThreshold = 100;
		int ratio = 3;
		int kernel_size = 3;

		cv::Canny(_cannyEdges, _cannyEdges, lowThreshold, lowThreshold*ratio, kernel_size );
		cv::threshold(_cannyEdges, _cannyEdges, 0, 1, CV_THRESH_BINARY);
		cv::integral(_cannyEdges, _cannyIntegralImg);

		//Mat disp_img;
		//frame.copyTo( disp_img, _cannyEdges);
		//imshow("canny", disp_img);
		//imshow("cannyIntegralImg", _cannyIntegralImg);
		//waitKey(0);
	}

	if(OUT_GFTT_COUNT)
	{
		if(_gfttImg.rows == 0)
			_gfttImg = cv::Mat(frame.rows, frame.cols, CV_8UC1);
		_gfttImg = Scalar(0);
		std::vector<cv::KeyPoint> kp;
		_detector->detect(frame, kp);
		int size = (int)kp.size();
		for(int i=0; i<size; ++i)
		{
			KeyPoint& k = kp[i];
			Point2d p = k.pt;
			_gfttImg.at<char>(cvRound(p.y), cvRound(p.x)) = 1;
		}

		cv::integral(_gfttImg, _gfttIntegralImg);

		//cv::Mat disp_img;
		//_gfttImg.convertTo(disp_img, CV_8UC1, 255);
		//imshow("gftt", disp_img);
		//imshow("gftt-integral", _gfttIntegralImg);
		//waitKey(0);
	}
}

double ImageStats::GetCannyEdgeCount(cv::Point2d window_center)
{
	return GetRegionSum(window_center, _cannyIntegralImg);
}

double ImageStats::GetGFTTCount(cv::Point2d window_center)
{
	return GetRegionSum(window_center, _gfttIntegralImg);
}

double ImageStats::GetRegionSum(cv::Point2d window_center, cv::Mat& img)
{
	int start_x = cvRound(window_center.x-_blockWidth) - 1;
	int end_x = cvRound(window_center.x+_blockWidth);
	int start_y = cvRound(window_center.y-_blockHeight) - 1;
	int end_y = cvRound(window_center.y+_blockHeight);

	start_x = MAX(start_x, 0);
	start_y = MAX(start_y, 0);
	end_x = MIN(end_x, img.cols-1);
	end_y = MIN(end_y, img.rows-1);

	int tl = img.at<int>(start_y, start_x);
	int tr = img.at<int>(start_y, end_x);
	int bl = img.at<int>(end_y, start_x);
	int br = img.at<int>(end_y, end_x);

	int val = br + tl - tr - bl;

	//if(_regionCountImg.rows == 0){
	//	_regionCountImg = cv::Mat(img.rows, img.cols, CV_8UC1); 
	//	_regionCountImg = Scalar(0);
	//}
	//cv::Rect r(start_x, start_y, end_x-start_x+1, end_y-start_y+1);
	//if(val > 255)
	//	_regionCountImg(r) = Scalar(255);
	//else
	//	_regionCountImg(r) = Scalar(val);
	//imshow("region_count", _regionCountImg);
	//waitKey(0);

	return (double)val;
}

cv::Mat ImageStats::GetCroppedImg(cv::Mat& frame, cv::Point2d window_center)
{
	double start_x = window_center.x-_blockWidth;
	double start_y = window_center.y-_blockHeight;

	start_x = MAX(start_x, 0);
	start_y = MAX(start_y, 0);

	Mat cropped(2*_blockHeight, 2*_blockHeight, CV_8UC3);

	Mat warp_mat( 2, 3, CV_64FC1 );
	warp_mat.at<double>(0,0) = 1; warp_mat.at<double>(0,1) = 0; warp_mat.at<double>(0,2) = -start_x;  
	warp_mat.at<double>(1,0) = 0; warp_mat.at<double>(1,1) = 1; warp_mat.at<double>(1,2) = -start_y;  

	cv::warpAffine(frame, cropped, warp_mat, cropped.size() );

	//imshow("orig", frame);
	//imshow("cropped", cropped);
	//waitKey(0);

	return cropped;
}

double ImageStats::GetWarpingErr(IplImage* prev_img, IplImage* cur_img, cv::Point2d prev_loc, cv::Point2d cur_loc)
{
	Mat prev_frame(prev_img);
	Mat cur_frame(cur_img);

	Mat prev_cropped = GetCroppedImg(prev_frame, prev_loc);
	Mat cur_cropped = GetCroppedImg(cur_frame, cur_loc);

	blur(prev_cropped, prev_cropped, Size(3,3));
	blur(cur_cropped, cur_cropped, Size(3,3));

	int width = 2*_blockWidth;
	int height = 2*_blockHeight;

	Vec3f avg_abs_diff(0,0,0),avg_pix(0,0,0); int num_points=0;

	for(int j=0; j<height; ++j)
	{
		uchar* prev_ptr = prev_cropped.ptr(j, 0);
		uchar* cur_ptr = cur_cropped.ptr(j, 0);
		for(int i=0; i<width; ++i, prev_ptr += 3, cur_ptr += 3)
		{
			if(cur_ptr[0] == 0 && cur_ptr[1] == 0 && cur_ptr[2] == 0)
				continue;
			if(prev_ptr[0] == 0 && prev_ptr[1] == 0 && prev_ptr[2] == 0)
				continue;

			for(int k=0; k<3; ++k)
			{
				avg_abs_diff[k] += abs(cur_ptr[k]-prev_ptr[k]);
				avg_pix[k] += prev_ptr[k];
			}
			++num_points;
		}
	}

	if(num_points == 0)
		return -1;

	avg_abs_diff /= num_points;
	avg_pix /= num_points;

	double err = 0;
	for(int k=0; k<3; ++k)
	{
		err += avg_abs_diff[k]/(avg_pix[k]+1);
	}
	err *= 255;

	//int start_x = cvRound(prev_loc.x-_blockWidth);
	//int end_x = cvRound(prev_loc.x+_blockWidth);
	//int start_y = cvRound(prev_loc.y-_blockHeight);
	//int end_y = cvRound(prev_loc.y+_blockHeight);

	//start_x = MAX(start_x, 0);
	//start_y = MAX(start_y, 0);
	//end_x = MIN(end_x, prev_frame.cols-1);
	//end_y = MIN(end_y, prev_frame.rows-1);

	//if(_regionCountImg.rows == 0){
	//	_regionCountImg = cv::Mat(prev_frame.rows, prev_frame.cols, CV_8UC1); 
	//	_regionCountImg = Scalar(0);
	//}
	//cv::Rect r(start_x, start_y, end_x-start_x+1, end_y-start_y+1);
	//if(err > 255)
	//	_regionCountImg(r) = Scalar(255);
	//else
	//	_regionCountImg(r) = Scalar(err);
	//imshow("region_count", _regionCountImg);
	//waitKey(0);
	
	return err;
}
