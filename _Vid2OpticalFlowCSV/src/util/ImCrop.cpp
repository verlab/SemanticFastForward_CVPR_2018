#include "stdafx.h"
#include "ImCrop.h"

void ImCropMouseCallBackFunction(int event, int x, int y, int flags, void* userdata)
{
	ImCropGUI* obj = (ImCropGUI*)userdata;
	obj->MouseCallBack(event, x, y, flags);
}

ImCropGUI::ImCropGUI(float scale/* = 1*/, const char* win_title/*=NULL*/)
:_scale(scale)
{
	if(win_title == NULL)
		_winTitle = std::string("ImCrop");
	else
		_winTitle = std::string(win_title);

	//Create a window
	namedWindow(_winTitle, 1);

	//set the callback function for any mouse event
	setMouseCallback(_winTitle, ImCropMouseCallBackFunction, this);
}

cv::Rect ImCropGUI::GetCropRect(cv::Mat img)
{
	_cropRect = cv::Rect(-1,-1,-1,-1);
	_startDrawing = false;
	cv::resize(img, _img, cv::Size(cvRound(img.cols*_scale), cvRound(img.rows*_scale)));
	_dispImg = _img.clone();
	while(1)
	{
		imshow(_winTitle, _dispImg);
		_img.copyTo(_dispImg);
		cv::rectangle(_dispImg, _cropRect, cv::Scalar(0,0,255), 2);
		int key = waitKey(10);
		if(key != -1)
			break;
	}
	cv::destroyWindow(_winTitle);
	return cv::Rect(cvRound(_cropRect.x/_scale), cvRound(_cropRect.y/_scale), cvRound(_cropRect.width/_scale), cvRound(_cropRect.height/_scale));
}

ImCropGUI::~ImCropGUI(void)
{
}

void ImCropGUI::MouseCallBack(int event, int x, int y, int flags)
{
	if  ( event == EVENT_LBUTTONDOWN )
	{
		//cout << "EVENT_LBUTTONDOWN - position (" << x << ", " << y << ")" << endl;
		if(!_startDrawing){
			_cropRect.x = x;
			_cropRect.y = y;
			_cropRect.width = 1;
			_cropRect.height = 1;
			_startDrawing = true;
		}
	}
	else if  ( event == EVENT_MOUSEMOVE)
	{
		if(_startDrawing){
			_cropRect.width = x-_cropRect.x;
			_cropRect.height = y-_cropRect.y;
		}
	}else if(event == EVENT_LBUTTONUP){
		if(_startDrawing){
			std::cout << "ImCrop: Rectangle = " << _cropRect << std::endl;
			_startDrawing = false;
		}
	}
}

cv::Rect ImCrop(cv::Mat img, float scale/* = 1*/, const char* win_title/*=NULL*/)
{
	ImCropGUI icg(scale, win_title);
	return icg.GetCropRect(img);
}
