#include "stdafx.h"
#include "TrajFinder.h"
#include "util/LensDistortionCorrector.h"
using namespace cv;

//int Trajectory::LAST_TID = 0;

TrajFinder::TrajFinder(void)
:_frameNum(-1)
{
	//fDetector = new cv::FastFeatureDetector(70);
	//fDetector = new cv::ORB();
	//fDetector = new SurfFeatureDetector(1000);//cv::FeatureDetector::create("SURF");
	fDetector = new cv::SiftFeatureDetector();
	if(fDetector == NULL){
		PRINTF("Detector not found\n");
		exit(1);
	}

	descExtractor = DescriptorExtractor::create("SIFT");
	//descExtractor = DescriptorExtractor::create("BRIEF");
	if(descExtractor  == NULL){
		PRINTF("Desc extractor not found\n");
		exit(1);
	}

	descMatcher = new BFMatcher(NORM_L2,true);
	if(descMatcher == NULL){
		PRINTF("Desc matcher not found\n");
		exit(1);
	}
}

TrajFinder::~TrajFinder(void)
{
}

bool TrajFinder::ValidateMatch(vector<DMatch>& match_vec)
{
	if (match_vec.empty())
		return false;

	if (match_vec.size()>1)
	{
		DMatch& match_1st = match_vec.at(0);
		DMatch& match_2nd = match_vec.at(1);

		if (match_1st.distance!=0)
		{
			double ratio = match_2nd.distance / match_1st.distance;
			if (ratio < 2.0)
				return false;
		} 
	} 

	//int prev_index = match_1st.trainIdx;
	//int cur_index = match_1st.queryIdx;

	//Point2d prev_pt = prevFrameKeyPoints.at(prev_index).pt;
	//Point2d cur_pt = curFrameKeyPoints.at(cur_index).pt;

	//if (norm(prev_pt-cur_pt) > 30)
	//	return false;

	return true;
}

//void TrajFinder::CleanTrajectories()
//{
//	std::vector<Trajectory*> new_t_traj;
//
//	int size = (int)_tempTrajs.size();
//	for(int i=0; i<size; ++i)
//	{
//		Trajectory* tr = _tempTrajs[i];
//		if(tr._frameNums.back() == _frameNum){
//			new_t_traj.push_back(tr);
//			continue;
//		}
//
//		if(tr._detections.size() > 5)
//		{
//			_permTrajs.push_back(tr);
//			continue;
//		}
//
//		delete tr;
//	}
//
//	_tempTrajs = new_t_traj;
//}
//
void TrajFinder::MatchPoints()
{
	vector<cv::KeyPoint> curFrameKeyPoints;
	cv::Mat curFrameDescriptors;

	Mat curFrame(_curFrame);

	curFrameKeyPoints.clear();
	fDetector->detect(curFrame, curFrameKeyPoints);		

	descExtractor->compute(curFrame, curFrameKeyPoints, curFrameDescriptors);

	if(prevFrameKeyPoints.empty())
	{
		prevFrameKeyPoints = curFrameKeyPoints;
		prevFrameDescriptors = curFrameDescriptors.clone();

		//for(int i=0; i<(int)prevFrameKeyPoints.size(); ++i)
		//{
		//	Trajectory* tr = new Trajectory(_frameNum, prevFrameKeyPoints[i].pt);
		//	_tempTrajs.push_back(tr);
		//}

		return;
	}

	vector< vector<cv::DMatch> > matches;
	// Match the feature points
	matches.clear();
	descMatcher->radiusMatch(curFrameDescriptors, prevFrameDescriptors,matches,3000.0);

	//std::vector<Trajectory*> cur_frame_trajs;

	// Match the points
	int size = (int)matches.size();
	prevFrameMatchedPoints.clear();
	curFrameMatchedPoints.clear();
	for(int j=0; j<size; ++j)
	{
		/* For 'regular' match:
		DMatch& match = matches.at(i);
		int prev_index = match.trainIdx;
		int cur_index = match.queryIdx;
		prevFrameMatchedPoints.push_back(prevFrameKeyPoints.at(prev_index).pt);
		curFrameMatchedPoints.push_back(curFrameKeyPoints.at(cur_index).pt);
		*/

		// For KNN or Radius match:
		vector<DMatch>& match_vec = matches.at(j);

		if(!ValidateMatch(match_vec))
		{
			//Point2d cur_pt = curFrameKeyPoints.at(j).pt;
			//Trajectory* tr = new Trajectory(_frameNum, cur_pt);
			//_tempTrajs.push_back(tr);
			//cur_frame_trajs.push_back(tr);
			continue;
		}

		DMatch& match_1st = match_vec.at(0);

		int prev_index = match_1st.trainIdx;
		int cur_index = match_1st.queryIdx;

		Point2d prev_pt = prevFrameKeyPoints.at(prev_index).pt;
		Point2d cur_pt = curFrameKeyPoints.at(cur_index).pt;

		prevFrameMatchedPoints.push_back(prev_pt);
		curFrameMatchedPoints.push_back(cur_pt);

		//Trajectory* prev_traj = _prevFrameTrajs[prev_index];
		//prev_traj->_frameNums.push_back(_frameNum);
		//prev_traj->_detections.push_back(cur_pt);
		//cur_frame_trajs.push_back(prev_traj);
	}

	PRINTF("matched %d points out of %d detected.\n",prevFrameMatchedPoints.size(),curFrameKeyPoints.size());

	prevFrameKeyPoints = curFrameKeyPoints;
	prevFrameDescriptors = curFrameDescriptors.clone();
	//_prevFrameTrajs = cur_frame_trajs;
}

void TrajFinder::DumpMatchedPoints()
{
	// Dump the matches to a file.
	char outfname[1000];
	sprintf(outfname,"tracks/frame_%d.ft",_frameNum);
	FILE *output_file = fopen(outfname,"wt");
	int size = curFrameMatchedPoints.size();
	for(int j=0; j<size; ++j)
	{
		fprintf(output_file,"%.02f %.02f %.02f %.02f\n",curFrameMatchedPoints[j].x,curFrameMatchedPoints[j].y,
			prevFrameMatchedPoints[j].x,prevFrameMatchedPoints[j].y);
	}
	fclose(output_file);
}

void TrajFinder::ShowMatchedPoints()
{
	double img_scale = 1.0;

	Mat curFrame(_curFrame);
	Mat disp_mat;

	cv::resize(curFrame, disp_mat, Size(0,0), 1/img_scale, 1/img_scale);
	//disp_mat=Scalar(255,255,255);

	int size = curFrameMatchedPoints.size();
	for(int j=0; j<size; ++j)
	{
		Point pt1(cvRound(curFrameMatchedPoints[j].x/img_scale), cvRound(curFrameMatchedPoints[j].y/img_scale));
		Point pt2(cvRound(prevFrameMatchedPoints[j].x/img_scale), cvRound(prevFrameMatchedPoints[j].y/img_scale));

		cv::circle(disp_mat, pt1, 3, Scalar(0, 0, 255));
		cv::line(disp_mat, pt1, pt2, Scalar(255, 0, 0));
	}
	if(!curFrameMatchedPoints.empty()){
		cv::imshow("Current Frame",disp_mat);
		imwrite("matched_points.png", disp_mat);
		cv::waitKey(0);
	}
}

void TrajFinder::FindTrajectories(int frame_num, IplImage* cur_img)
{	
	_frameNum = frame_num;

#if 0
	LensDistortionCorrector ldc;
	ldc.Calibrate("hero3-und-params.txt");
	Mat src(cur_img); Mat corrected;
	ldc.Undistort(src, corrected);
	IplImage corrected_ipl = corrected;
	_curFrame = &corrected_ipl;
	cvShowImage("traj finder inp frame", _curFrame);
	cvWaitKey(0);
#else
	_curFrame = cur_img;
#endif

	MatchPoints();
	
	//CleanTrajectories();

	DumpMatchedPoints();

	ShowMatchedPoints();
}
