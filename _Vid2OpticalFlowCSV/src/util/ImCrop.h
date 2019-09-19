#pragma once

class ImCropGUI
{
public:
	ImCropGUI(float scale = 0.5f, const char* win_title=NULL);
	~ImCropGUI();

	cv::Rect GetCropRect(cv::Mat img);

	void MouseCallBack(int event, int x, int y, int flags);

private:
	cv::Mat _img, _dispImg;
	std::string _winTitle;
	cv::Rect _cropRect;
	bool _startDrawing;
	float _scale;
};

cv::Rect ImCrop(cv::Mat img, float scale = 0.5f, const char* win_title=NULL);


