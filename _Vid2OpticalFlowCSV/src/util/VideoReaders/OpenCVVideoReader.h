#pragma once

#include "AbstractVideoReader.h"

class OpenCVVideoReader : public AbstractVideoReader
{
public:
	OpenCVVideoReader(void);
	~OpenCVVideoReader(void);

	bool Open(const char* filename){
		_vc.open(filename); 
		if(_vc.isOpened())
			return true;
		else
			return false;
	}
	int GetTotalFrames()
	{
		return cvRound(_vc.get(CV_CAP_PROP_FRAME_COUNT));
	}
	int GetFPS()
	{
		return cvRound(_vc.get(CV_CAP_PROP_FPS));
	}
	bool SetFrameNum(int fnum)
	{
		return _vc.set(CV_CAP_PROP_POS_FRAMES, fnum);
	}

	IplImage* GetFrame()
	{
		_vc >> _frame;
		_iplImg = _frame;
		return &_iplImg;
	}

private:
	cv::VideoCapture _vc;
	cv::Mat _frame;
	IplImage _iplImg;
};
