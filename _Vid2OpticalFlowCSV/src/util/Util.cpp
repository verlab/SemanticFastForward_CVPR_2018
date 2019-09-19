#include "stdafx.h"
#include "Util.h"
//#include <Windows.h>
#include <sys/stat.h>
#include <opencv2/nonfree/nonfree.hpp>
using namespace cv;
using namespace std;

std::vector<Point2d> Util::AddMorePointsInVicinity(std::vector<cv::Point2d>& inp, double radius, int num_points, int64 key)
{
	Point2d min_p = Util::Min<Point2d>(inp);
	Point2d max_p = Util::Max<Point2d>(inp);
	double range_p = MIN(fabs(max_p.x-min_p.x), fabs(max_p.y-min_p.y));
	radius = radius*range_p;

	vector<Point2d> out_vec;

	CvRNG rng(key);
	int size = (int)inp.size();
	for(int i=0; i<size; ++i)
	{
		Point2d& pt = inp.at(i);
		out_vec.push_back(pt);
		for(int j=0; j<num_points; ++j)
		{
			double x = pt.x + (double)((cvRandReal(&rng)-0.5)*radius);
			double y = pt.y + (double)((cvRandReal(&rng)-0.5)*radius);
			out_vec.push_back(Point2d(x,y));
		}
	}	
	return out_vec;
}

double Util::Round(double f)
{
	return cvRound(f*1000)/1000.0f;
}

void Util::Perturb(std::vector<cv::Point2d>& inp, double amplitude, int64 key)
{
	Point2d min_p = Util::Min<Point2d>(inp);
	Point2d max_p = Util::Max<Point2d>(inp);
	double range_p = MIN(fabs(max_p.x-min_p.x), fabs(max_p.y-min_p.y));
	amplitude = amplitude*range_p;

	if(key == -1)
		key = cvGetTickCount();
	CvRNG rng(key);
	int size = (int)inp.size();
	for(int i=0; i<size; ++i)
	{
		Point2d& pt = inp.at(i);
		pt.x += (double)((cvRandReal(&rng)-0.5)*amplitude);
		pt.y += (double)((cvRandReal(&rng)-0.5)*amplitude);
	}
}

std::vector<cv::Point2d> Util::Shift(std::vector<cv::Point2d>& inp, Point2d offset)
{
	std::vector<cv::Point2d> ret_vec;
	int size = (int)inp.size();
	for(int i=0; i<size; ++i)
		ret_vec.push_back(inp[i]-offset);
	return ret_vec;
}

bool Util::FileExists(const char *filename ) {
	struct stat buffer ;
	if ( stat( filename, &buffer )  != 0)
		return false ;
	return true ;
}

/*void Util::GetDirectoryListing(std::string directory, std::string ext_to_match, std::vector<std::string>& listing)
{
	std::string sFileName;
	WIN32_FIND_DATA findData;
	HANDLE hFind=FindFirstFile((directory+"\\*.*").c_str(),&findData);
	if (hFind == INVALID_HANDLE_VALUE) 
		return;

	do 
	{
		sFileName = std::string(findData.cFileName);
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
			continue;

		std::string ext = GetFileExt(sFileName);
		if(ext.compare(ext_to_match) == 0)
			listing.push_back(sFileName);	
	}
	while (FindNextFile(hFind, &findData));
}*/

std::string Util::GetFileExt(std::string& filename)
{
	static std::string delimiters(".");
	std::vector<std::string> tokens = Tokenize(filename, delimiters);
	return tokens.back();

}

std::vector<std::string> Util::Tokenize(const std::string& str,const std::string& delimiters)
{
	vector<string> tokens;
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	string::size_type pos = str.find_first_of(delimiters, lastPos);
	while (string::npos != pos || string::npos != lastPos){
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		lastPos = str.find_first_not_of(delimiters, pos);
		pos = str.find_first_of(delimiters, lastPos);
	}
	return tokens;
}



/*void Util::FindFrameRange(std::string directory,std::string frame_file_pattern,int &first_frame,int &last_frame)
{
	std::string ext = GetFileExt(frame_file_pattern);
	std::vector<std::string> listing;
	GetDirectoryListing(directory, ext, listing);
	first_frame = INT_MAX;
	last_frame = -1;

	for (int i=0; i<(int)listing.size(); i++)
	{
		std::string curfile = listing[i];
		int framenum;
		int res = sscanf(curfile.c_str(),frame_file_pattern.c_str(),&framenum);

		if (res>0)
		{
			if (framenum<first_frame) first_frame = framenum;
			if (framenum>last_frame) last_frame = framenum;
		}
	}

}*/

cv::Mat Util::FindHomography_SIFT(cv::Mat& first, cv::Mat& second, cv::Mat& warped_second,
								  double min_trans_x, double min_trans_y, double max_trans_x, double max_trans_y)
{
	//-- Step 1: Detect the keypoints using SURF Detector
	int minHessian = 400;

	SiftFeatureDetector detector( minHessian );

	std::vector<KeyPoint> keypoints_second, keypoints_first;

	detector.detect( second, keypoints_second );
	detector.detect( first, keypoints_first );

	//-- Step 2: Calculate descriptors (feature vectors)
	SiftDescriptorExtractor extractor;

	Mat descriptors_second, descriptors_first;

	extractor.compute( second, keypoints_second, descriptors_second );
	extractor.compute( first, keypoints_first, descriptors_first );

	//-- Step 3: Matching descriptor vectors using FLANN matcher
	FlannBasedMatcher matcher;
	std::vector< DMatch > matches;
	matcher.match( descriptors_second, descriptors_first, matches );

	//double max_dist = 0; double min_dist = 50;//100;

	////-- Quick calculation of max and min distances between keypoints
	//for( int i = 0; i < descriptors_object.rows; i++ )
	//{ double dist = matches[i].distance;
	//if( dist < min_dist ) min_dist = dist;
	//if( dist > max_dist ) max_dist = dist;
	//}

	//printf("-- Max dist : %f \n", max_dist );
	//printf("-- Min dist : %f \n", min_dist );

	//-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
	std::vector< DMatch > good_matches;

	for( int i = 0; i < descriptors_second.rows; i++ )
	{ 
		//if( matches[i].distance < 3*min_dist )
		{ 
			DMatch& match = matches[i];
			Point2f& pt_f1 = keypoints_first[match.trainIdx].pt;
			Point2f& pt_f2 = keypoints_second[match.queryIdx].pt;
			Point2f diff = pt_f1-pt_f2;
			if(fabs(diff.x) < min_trans_x || fabs(diff.x) > max_trans_x)
				continue;
			if(fabs(diff.y) < min_trans_y || fabs(diff.y) > max_trans_y)
				continue;

			good_matches.push_back( matches[i]); 
		}
	}

	//Mat img_matches;
	//drawMatches( next, keypoints_object, cur, keypoints_scene,
	//	good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
	//	vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
	//imshow("matched", img_matches);
	//waitKey(0);

	//-- Localize the object
	std::vector<Point2f> matched_points_second;
	std::vector<Point2f> matched_points_first;

	for(unsigned int i = 0; i < good_matches.size(); i++ )
	{
		//-- Get the keypoints from the good matches
		matched_points_second.push_back( keypoints_second[ good_matches[i].queryIdx ].pt );
		matched_points_first.push_back( keypoints_first[ good_matches[i].trainIdx ].pt );
	}

	Mat second2first = findHomography( matched_points_first, matched_points_second, CV_RANSAC );

	printf("Matched %ld features\n", matched_points_second.size());
	cout << second2first << endl << endl;

	if(matched_points_second.size() < 8){
		printf("not enough features match\n");
		exit(1);
	}

	warpPerspective(second, warped_second, second2first.inv(), Size(second.cols, second.rows), INTER_CUBIC);
	//warpPerspective(next, warped_next, H, Size(next.cols, next.rows), INTER_LINEAR);
	//warpPerspective(next, warped_next, H, Size(next.cols, next.rows), INTER_NEAREST);

	//waitKey(0);

	return second2first;
}
