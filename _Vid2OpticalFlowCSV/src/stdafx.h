#pragma once

#pragma warning (disable : 4996)

#include <stdio.h>
#include <string>
#include <vector>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#define _USE_MATH_DEFINES
#include <math.h>
#ifndef MIN
#define MIN(X,Y)	((X)<(Y)?(X):(Y))
#endif

#define SQR(X) ((X)*(X))
#define NO_DX

#define PRINT_ENABLED 1

#define XPRINTF(p,...)   {  if((p) == 0x01) printf(__VA_ARGS__); else if((p) == 0x10) { if(STATE_FILE) fprintf(STATE_FILE, __VA_ARGS__); } else { printf(__VA_ARGS__); if(STATE_FILE) fprintf(STATE_FILE, __VA_ARGS__); } } 
#define PRINTF(...)    { if(PRINT_ENABLED) XPRINTF(3,__VA_ARGS__);}
#define PRINTLN     PRINTF("\n")

extern FILE *STATE_FILE;

extern int NUM_BLOCKS_X;
extern int NUM_BLOCKS_Y;
extern double BLOCK_WIDTH_IMG_WIDTH_RATIO;
extern double BLOCK_HEIGHT_IMG_HEIGHT_RATIO;

extern int START_FRAME;
extern int END_FRAME;
extern int FRAME_SKIP;
extern double LK_FRAME_RESCALE;

extern bool OUT_CANNY_COUNT;
extern bool OUT_GFTT_COUNT;
extern bool OUT_BACK_WARP_ERROR;
extern bool DISPLAY_LK_OUTPUT;

extern float SQAURED_MIN_DIST_FOR_NEW_POINTS;

extern int IMG_WIDTH, IMG_HEIGHT, PROCESSING_WIDTH, PROCESSING_HEIGHT;
extern int64 RAND_KEY;
using namespace std;
using namespace cv;
