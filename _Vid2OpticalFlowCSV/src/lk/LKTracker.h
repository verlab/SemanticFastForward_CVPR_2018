#pragma once

#include "TrackPoint.h"

class LKTracker
{
public:
	LKTracker(IplImage* img);
	~LKTracker(void);

	void SwapModel(IplImage* cur_img);
	void Track(IplImage* color_img, std::vector<TrackPoint> &track_points);
	static void ComputeTrackWindows(int width, int height, int sample_x, int sample_y, std::vector<TrackPoint> &track_points, IplImage* mask=NULL);

	static int TRACK_WIN_WIDTH;
	static int TRACK_WIN_HEIGHT;

protected:
	void PyrDown(IplImage* src, IplImage* dst);
	void SetGradientImg(IplImage* cur_img, IplImage* x_gradient_img, IplImage* y_gradient_img);
	void ShowGradientImg();

	void ComputeAandBMatrices(double& tx, double& ty, CvRect win);
	void ComputeOnlyBMatrix(double& tx, double& ty, CvRect win);
	bool ComputeAInverse();
	bool UpdateMotion(double& tx, double& ty);

protected:
	std::vector<IplImage*> _curPyr, _prevPyr, _xGradientPyr, _yGradientPyr;
	int _levels, _highestLevel2Track;
	static const int FRAME_MARGIN = 1;
	static const int REG_MAX_ITER = 20;
	static const double EPSILON;
	IplImage* _curImg, *_prevImg, *_xGradientImg, *_yGradientImg;
    //std::vector<CvRect> _trackWindows;
	double _aMat[4], _aInv[4], _bMat[2];
	int _numChannels;
};

