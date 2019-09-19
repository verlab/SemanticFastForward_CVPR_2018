#pragma once

class LensDistortionCorrector
{
public:
	LensDistortionCorrector(void);
	~LensDistortionCorrector(void);

	void Calibrate(const char* dump_file);
	void Calibrate(const char* video_file, int interior_corner_cols, int interior_corner_rows, const char* dump_file);
	void Calibrate(const char* formatted_string, int start_frame, int end_frame, int interior_corner_cols, int interior_corner_rows, const char* dump_file);
	void Undistort(cv::Mat& src, cv::Mat& dst);

private:
	void FindCheckerboardCorners(cv::Mat& frame);
	bool FindCameraParameters();
	double computeReprojectionErrors(const vector<vector<cv::Point3f> >& objectPoints,
									 const vector<cv::Mat>& rvecs, const vector<cv::Mat>& tvecs,
									 vector<double>& perViewErrors );
	void DumpCorrectionParams(const char* filename);

private:
	vector< vector<cv::Point2f> > _checkboardPoints;
	cv::Size _boardSize; //interior number of corners
	cv::Size _imgSize;
	cv::Mat _cameraMatrix, _distortionCoeffs;
};
