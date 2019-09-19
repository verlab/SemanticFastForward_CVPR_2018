#include "stdafx.h"
#include "ConfigReader.h"
#include "util/paramio/ParamIO.h"

void ConfigReader::Read(const char* filename)
{
	ParamIO reader;
	reader.readFile(filename);

	reader.read<int>("TRAJ_FINDER:NUM_BLOCKS_X", NUM_BLOCKS_X, NUM_BLOCKS_X);
	reader.read<int>("TRAJ_FINDER:NUM_BLOCKS_Y", NUM_BLOCKS_Y, NUM_BLOCKS_Y);
	reader.read<double>("TRAJ_FINDER:BLOCK_WIDTH_IMG_WIDTH_RATIO", BLOCK_WIDTH_IMG_WIDTH_RATIO, (double)NUM_BLOCKS_X);
	reader.read<double>("TRAJ_FINDER:BLOCK_HEIGHT_IMG_HEIGHT_RATIO", BLOCK_HEIGHT_IMG_HEIGHT_RATIO, (double)NUM_BLOCKS_Y);

	reader.read<int>("TRAJ_FINDER:START_FRAME", START_FRAME, START_FRAME);
	reader.read<int>("TRAJ_FINDER:END_FRAME", END_FRAME, END_FRAME);
	reader.read<int>("TRAJ_FINDER:FRAME_SKIP", FRAME_SKIP, FRAME_SKIP);
	reader.read<double>("TRAJ_FINDER:LK_FRAME_RESCALE", LK_FRAME_RESCALE, LK_FRAME_RESCALE);
	reader.read<bool>("TRAJ_FINDER:OUT_CANNY_COUNT", OUT_CANNY_COUNT, OUT_CANNY_COUNT);
	reader.read<bool>("TRAJ_FINDER:OUT_GFTT_COUNT", OUT_GFTT_COUNT, OUT_GFTT_COUNT);
	reader.read<bool>("TRAJ_FINDER:OUT_BACK_WARP_ERROR", OUT_BACK_WARP_ERROR, OUT_BACK_WARP_ERROR);

	reader.read<bool>("TRAJ_FINDER:DISPLAY_LK_OUTPUT", DISPLAY_LK_OUTPUT, DISPLAY_LK_OUTPUT);
}