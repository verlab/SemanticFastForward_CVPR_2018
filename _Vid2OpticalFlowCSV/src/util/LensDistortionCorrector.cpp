#include "stdafx.h"
#include "LensDistortionCorrector.h"
//#include "QTVideoReader.h"
using namespace std;
using namespace cv;

LensDistortionCorrector::LensDistortionCorrector(void)
{
}

LensDistortionCorrector::~LensDistortionCorrector(void)
{
}

void LensDistortionCorrector::Undistort(Mat& src, Mat& dst)
{
	cv::undistort(src, dst, _cameraMatrix, _distortionCoeffs);
}

void LensDistortionCorrector::DumpCorrectionParams(const char* filename)
{
	FILE* f = fopen(filename, "w");

	for(int j=0; j<3; ++j)
		for(int i=0; i<3; ++i)
			fprintf(f, "%f ", _cameraMatrix.at<double>(j,i));
	fprintf(f, "\n");

	for(int i=0; i<5; ++i)
		fprintf(f, "%f ", _distortionCoeffs.at<double>(i,0));
	fprintf(f, "\n");

	fflush(f);
	fclose(f);
}

void LensDistortionCorrector::Calibrate(const char* dump_file)
{
	_cameraMatrix = Mat::eye(3, 3, CV_64F);
	_distortionCoeffs = Mat::zeros(5, 1, CV_64F);

	FILE* f = fopen(dump_file, "r");

	for(int j=0; j<3; ++j)
		for(int i=0; i<3; ++i)
		{
			double val;
			fscanf(f, "%lf", &val);
			_cameraMatrix.at<double>(j,i) = val;
		}

	for(int i=0; i<5; ++i)
	{
		double val;
		fscanf(f, "%lf", &val);
		_distortionCoeffs.at<double>(i,0) = val;
	}

	fclose(f);
}

void LensDistortionCorrector::Calibrate(const char* formatted_string, int start_frame, int end_frame, int interior_corner_cols, int interior_corner_rows, const char* dump_file)
{
	_boardSize = cv::Size(interior_corner_cols,interior_corner_rows); //interior number of corners

	for(int i=start_frame; i<=end_frame; ++i)
	{
		PRINTF("Processing frame %d\n", i);

		char filename[100];
		sprintf(filename, formatted_string, i);
		Mat frame = imread(string(filename));
		FindCheckerboardCorners(frame);
	}

	FindCameraParameters();

	cout << endl << _cameraMatrix << endl << endl;
	cout << endl << _distortionCoeffs << endl << endl;

	DumpCorrectionParams(dump_file);

	Calibrate(dump_file);

	cout << endl << _cameraMatrix << endl << endl;
	cout << endl << _distortionCoeffs << endl << endl;

	for(int i=start_frame; i<=end_frame; ++i)
	{
		char filename[100];
		sprintf(filename, formatted_string, i);
		Mat frame = imread(string(filename));
		Mat und;
		Undistort(frame, und);

		imshow("orig", frame);
		imshow("und",  und);
		waitKey(0);
	}
}

double LensDistortionCorrector::computeReprojectionErrors(const vector<vector<Point3f> >& objectPoints,
														  const vector<Mat>& rvecs, const vector<Mat>& tvecs,
														  vector<double>& perViewErrors)
{
	vector<Point2f> imagePoints2;
	int i, totalPoints = 0;
	double totalErr = 0, err;
	perViewErrors.resize(objectPoints.size());

	for( i = 0; i < (int)objectPoints.size(); i++ )
	{
		projectPoints(Mat(objectPoints[i]), rvecs[i], tvecs[i],
			_cameraMatrix, _distortionCoeffs, imagePoints2);
		err = norm(Mat(_checkboardPoints[i]), Mat(imagePoints2), CV_L2);
		int n = (int)objectPoints[i].size();
		perViewErrors[i] = (double)std::sqrt(err*err/n);
		totalErr += err*err;
		totalPoints += n;
	}

	return std::sqrt(totalErr/totalPoints);
}

bool LensDistortionCorrector::FindCameraParameters()
{
	vector<Mat> rvecs, tvecs;
	_cameraMatrix = Mat::eye(3, 3, CV_64F);
	_distortionCoeffs = Mat::zeros(5, 1, CV_64F);

	static const int squareSize = 100;
	vector<vector<Point3f> > corners(1);
	for( int i = 0; i < _boardSize.height; i++ )
		for( int j = 0; j < _boardSize.width; j++ )
			corners[0].push_back(Point3d(double(j*squareSize),double(i*squareSize), 0));

	corners.resize(_checkboardPoints.size(),corners[0]);

	double rms = calibrateCamera(corners, _checkboardPoints, _imgSize, _cameraMatrix,
		_distortionCoeffs, rvecs, tvecs, CV_CALIB_FIX_K4|CV_CALIB_FIX_K5);
	//printf("RMS error reported by calibrateCamera: %g\n", rms);

	bool ok = checkRange(_cameraMatrix) && checkRange(_distortionCoeffs);

	vector<double> reprojErrs;
	double totalAvgErr = computeReprojectionErrors(corners, rvecs, tvecs, reprojErrs);

	return ok;
}

void LensDistortionCorrector::FindCheckerboardCorners(cv::Mat& frame)
{
	PRINTF("Finding chessboard corners for frame\n");

	_imgSize = cv::Size(frame.cols, frame.rows);

	Mat gray;
	cvtColor(frame, gray, CV_BGR2GRAY);

	vector<Point2f> corners; //this will be filled by the detected corners
	//CALIB_CB_FAST_CHECK saves a lot of time on images
	//that do not contain any chessboard corners
	bool patternfound = findChessboardCorners(gray, _boardSize, corners,CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE + CALIB_CB_FAST_CHECK);

	if(!patternfound){
		printf("Opencv findChessboardCorners returned false. Did not find chess board corners\n");
		return;
	}

	cornerSubPix(gray, corners, cv::Size(11, 11), cv::Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));

	//drawChessboardCorners(gray, _boardSize, Mat(corners), patternfound);
	//imshow("corners", gray);
	//waitKey(0);

	//vector<Point2d> corners_d;
	//for(unsigned int i=0; i<corners.size(); ++i)
	//	corners_d.push_back(corners[i]);
	//
	//_checkboardPoints.push_back(corners_d);
	_checkboardPoints.push_back(corners);
}

void LensDistortionCorrector::Calibrate(const char* video_file, int interior_corner_cols, int interior_corner_rows, const char* dump_file)
{
	_checkboardPoints.clear();
	_boardSize = cv::Size(interior_corner_cols,interior_corner_rows); //interior number of corners

	VideoCapture vc;
	vc.open(video_file);

	int frame_count = cvRound(vc.get(CV_CAP_PROP_FRAME_COUNT));

	PRINTF("Num frames %d\n", frame_count);

	Mat frame;

	int captured_frame = 0;

	bool corrected = false;

	int i = 0;
	while(true)
	{
		PRINTF("Displaying Frame %d of %d\n", i, frame_count);

		vc.set(CV_CAP_PROP_POS_FRAMES, i);
		vc >> frame;

		if(corrected)
		{
			Mat dst; 
			Undistort(frame, dst);

			cv::Mat dst_2_show;
			cv::resize(dst, dst_2_show, cv::Size(dst.cols/2, dst.rows/2));
			imshow("und", dst_2_show);

			//imshow("und", dst);
		}

		cv::Mat frame_2_show;
		cv::resize(frame, frame_2_show, cv::Size(frame.cols/2, frame.rows/2));
		imshow("frame", frame_2_show);
		int k = waitKey(0);

		if(k == 'c' && !corrected)
		{
			PRINTF("Grabbing frame %d for calibration\n", i);
			FindCheckerboardCorners(frame);
			//char filename[100];
			//sprintf(filename, "calibration_frame_%d.png", captured_frame);
			//cvSaveImage(filename, frame);
			//++captured_frame;
			continue;
		} 
		if(k == 'n')
			++i;
		else if(k == 'p')
			--i;
		else if(k >= '1' && k <= '9')
			i += k - '0';
		else if(k == 'x')
		{
			if(corrected)
				break;

			FindCameraParameters();
			DumpCorrectionParams(dump_file);
			corrected = true;			
		}
	
		if(i < 0) i= 0;
		if(i >= frame_count) i = frame_count-1;
	}
}