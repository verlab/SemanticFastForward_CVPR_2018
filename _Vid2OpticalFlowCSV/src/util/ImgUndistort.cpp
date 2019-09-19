#include "stdafx.h"
#include "ImgUndistort.h"

ImgUndistort::ImgUndistort(int width, int height)
{
	double cm_arr[9] = {469.96f, 0.0, 640.0f, 0.0, 467.68f, 360.0f, 0.0, 0.0, 1.0f};
	CvMat camera_matrix = cvMat(3, 3, CV_64FC1, cm_arr);

	double dist_arr[5] = {-0.18957f, 0.037319f, 0.0, 0.0, -0.00337f};
	CvMat dist_coeffs = cvMat(1, 5, CV_64FC1, dist_arr);

	map1 = cvCreateImage(cvSize(width, height), IPL_DEPTH_32F, 1);
	map2 = cvCreateImage(cvSize(width, height), IPL_DEPTH_32F, 1);
	cvInitUndistortMap(&camera_matrix, &dist_coeffs, map1, map2);

	out_img = cvCreateImage(cvSize(width, height), 8, 3);
}

ImgUndistort::~ImgUndistort(void)
{
	cvReleaseImage(&map1);
	cvReleaseImage(&map2);
	cvReleaseImage(&out_img);
}

IplImage* ImgUndistort::Undistort(IplImage* inp)
{
	cvRemap(inp, out_img, map1, map2);
	return out_img;
}


