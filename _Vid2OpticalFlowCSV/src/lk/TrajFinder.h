#pragma once

using namespace cv;

//class Trajectory
//{
//public:
//	static int LAST_TID;
//	Trajectory(int f, Point2d p){ _frameNums.push_back(f); _detections.push_back(p); _trID = LAST_TID++; }
//
//	int _trID;
//	std::vector<int> _frameNums;
//	std::vector<cv::Point2d> _detections;
//};

class TrajFinder
{
public:
	TrajFinder(void);
	~TrajFinder(void);
	void FindTrajectories(int frame_num, IplImage* cur_img);

public:
	vector<cv::Point2d> curFrameMatchedPoints, prevFrameMatchedPoints;

	//std::vector<Trajectory*> _tempTrajs, _permTrajs;

private:
	void MatchPoints();
	bool ValidateMatch(vector<cv::DMatch>& match_vec);
	void DumpMatchedPoints();
	void ShowMatchedPoints();

	//void CleanTrajectories();

private:
	int _frameNum;
	cv::Ptr<cv::FeatureDetector> fDetector;
	cv::Ptr<cv::DescriptorExtractor> descExtractor;
	cv::Ptr<cv::DescriptorMatcher> descMatcher;

	IplImage* _curFrame;
	vector<cv::KeyPoint> prevFrameKeyPoints;
	cv::Mat prevFrameDescriptors;
	//std::vector<Trajectory*> _prevFrameTrajs;
};
