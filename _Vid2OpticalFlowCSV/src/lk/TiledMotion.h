#pragma once

#include "LKTracker.h"
#include "ImageStats.h"

class TiledMotion
{
public:
	TiledMotion(void);
	~TiledMotion(void);

	void FindTrajectories(int frame_index, IplImage* frame, FILE* traj_file);

private:
	IplImage* _prevImg, *_curImg;
	LKTracker* _tracker;
	int _frameIndex;
	ImageStats* _stats;
};
