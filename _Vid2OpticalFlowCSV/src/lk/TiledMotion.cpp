#include "stdafx.h"
#include "TiledMotion.h"
#include "LKTracker.h"
using namespace cv;
TiledMotion::TiledMotion(void)
{
	_prevImg = _curImg = NULL;
	_tracker = NULL;
	_stats = NULL;
}

TiledMotion::~TiledMotion(void)
{
	if(!_stats)
		delete _stats;

	if(!_prevImg)
		return;

	cvReleaseImage(&_prevImg);
	cvReleaseImage(&_curImg);
	delete _tracker;
}

void TiledMotion::FindTrajectories(int frame_index, IplImage* frame, FILE* traj_file)
{
	_frameIndex = frame_index;

	if(_prevImg == NULL){
		_prevImg = cvCloneImage(frame);
		_curImg = cvCloneImage(frame);
		_tracker = new LKTracker(_prevImg);
		return;
	}else{
		cvCopy(_curImg, _prevImg);
		cvCopy(frame, _curImg);
	}

	int cell_width = cvRound(frame->width/BLOCK_WIDTH_IMG_WIDTH_RATIO);
	int cell_height = cvRound(frame->height/BLOCK_HEIGHT_IMG_HEIGHT_RATIO);
	LKTracker::TRACK_WIN_WIDTH = cell_width/2;
	LKTracker::TRACK_WIN_HEIGHT = cell_height/2;

	if(_stats == NULL)
		_stats = new ImageStats(LKTracker::TRACK_WIN_WIDTH, LKTracker::TRACK_WIN_HEIGHT);

	int inter_block_dist_x = (PROCESSING_WIDTH - cell_width)/(NUM_BLOCKS_X-1);
	int inter_block_dist_y = (PROCESSING_HEIGHT - cell_height)/(NUM_BLOCKS_Y-1);
	int offset_x = (PROCESSING_WIDTH - (inter_block_dist_x*(NUM_BLOCKS_X-1)))/2;
	int offset_y = (PROCESSING_HEIGHT - (inter_block_dist_y*(NUM_BLOCKS_Y-1)))/2;

	vector<TrackPoint> tp_vec;

	for(int j=0; j<NUM_BLOCKS_Y; ++j)
	{
		for(int i=0; i<NUM_BLOCKS_X ; ++i)
		{
			int cx = i*inter_block_dist_x + offset_x;
			int cy = j*inter_block_dist_y + offset_y;

			TrackPoint tp; tp._trackingValid=true;
			tp._curLocation = Point2d((double)cx, (double)cy);
			tp_vec.push_back(tp);
		}
	}

	_tracker->Track(frame, tp_vec);

	_stats->EvaluateStats(_prevImg);

	if(traj_file)
	{
		int size = (int)tp_vec.size();
		for(int i=0; i<size; ++i)
		{
			if(i != 0)
				fprintf(traj_file, ", ");

			TrackPoint& tp = tp_vec[i];
			if(tp._trackingValid)
				fprintf(traj_file, "1");
			else
				fprintf(traj_file, "0");
			Point2d diff = tp._newLocation-tp._curLocation;
			fprintf(traj_file, ", %f, %f", diff.x, diff.y);

			double canny_count = -1, gftt_count = -1, warp_err = -1;
			if(OUT_CANNY_COUNT)
				canny_count = _stats->GetCannyEdgeCount(tp._curLocation);
			if(OUT_GFTT_COUNT)
				gftt_count = _stats->GetGFTTCount(tp._curLocation);
			if(OUT_BACK_WARP_ERROR && tp._trackingValid)
				warp_err = _stats->GetWarpingErr(_prevImg, _curImg, tp._curLocation, tp._newLocation);

			fprintf(traj_file, ", %f", canny_count);
			fprintf(traj_file, ", %f", gftt_count);
			fprintf(traj_file, ", %f", warp_err);
		}
		fprintf(traj_file, "\n");
	}

	if(DISPLAY_LK_OUTPUT)
	{
		Mat disp_mat(_prevImg, true);
		Point pt1, pt2;
		for(int j=0, counter=0; j<NUM_BLOCKS_Y; ++j)
		{
			for(int i=0; i<NUM_BLOCKS_X; ++i, ++counter)
			{
				TrackPoint& tp = tp_vec[counter];

				pt1 = pt2 = tp._curLocation;

				if(tp._trackingValid)
				{
					pt2 = tp._newLocation;
					circle(disp_mat, pt1, 2, Scalar(255, 0, 0), 2);
					line(disp_mat, pt1, pt2, Scalar(0, 255, 255), 2);
				}else
				{
					circle(disp_mat, pt1, 2, Scalar(0, 0, 255), 2);
				}

			}
		}
		imshow("flow", disp_mat);
	}

	_tracker->SwapModel(frame);
}
