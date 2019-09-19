#include "stdafx.h"
#include "LKTracker.h"
using namespace cv;

const double LKTracker::EPSILON = 0.01f;
int LKTracker::TRACK_WIN_WIDTH = 32;
int LKTracker::TRACK_WIN_HEIGHT = 16;

LKTracker::LKTracker(IplImage* img)
:_highestLevel2Track(2)
{
	_numChannels = img->nChannels;

    CvSize imgsize= cvGetSize(img);
	int size = MIN(imgsize.width, imgsize.height);
	int k = cvRound(log(size*1.0)/log(2.0));
	_levels = k-4; //min pyramid level is of size 64 for levels=k-5

	//_levels = 1;

	if(_levels < _highestLevel2Track)
		_levels=_highestLevel2Track;

	_curPyr = std::vector<IplImage*>(_levels);
	_prevPyr = std::vector<IplImage*>(_levels);
	_xGradientPyr = std::vector<IplImage*>(_levels);
	_yGradientPyr = std::vector<IplImage*>(_levels);
	int w = imgsize.width, h = imgsize.height;
	for(int i=0; i<_levels; ++i, w/=2,h/=2){
		_curPyr[i] = cvCreateImage(cvSize(w,h), 8, _numChannels);
		if(i >= _highestLevel2Track)
			_prevPyr[i] = cvCreateImage(cvSize(w,h), 8, _numChannels);
		else
			_prevPyr[i] = _curPyr[i];
		if(i >= _highestLevel2Track){
			_xGradientPyr[i] = cvCreateImage(cvSize(w,h), IPL_DEPTH_8S, _numChannels);
			_yGradientPyr[i] = cvCreateImage(cvSize(w,h), IPL_DEPTH_8S, _numChannels);
		}else{ 
			_xGradientPyr[i] = NULL;
			_yGradientPyr[i] = NULL;
		}
	}

	cvCopy(img, _prevPyr[0]);
	for(int i=1; i<_levels; ++i)
		PyrDown(_prevPyr[i-1], _prevPyr[i]);
	for(int i=_highestLevel2Track; i<_levels; ++i)
		SetGradientImg(_prevPyr[i], _xGradientPyr[i], _yGradientPyr[i]);    
}

LKTracker::~LKTracker(void)
{
	for(int i=0; i<_levels; ++i){
		cvReleaseImage(&_curPyr[i]);
		if(i >= _highestLevel2Track)
			cvReleaseImage(&_prevPyr[i]);
	}
}

void LKTracker::SwapModel(IplImage* cur_img)
{
	std::vector<IplImage*> temp = _prevPyr;
	_prevPyr = _curPyr;
	_curPyr = temp;

	for(int i=_highestLevel2Track; i<_levels; ++i)
		SetGradientImg(_prevPyr[i], _xGradientPyr[i], _yGradientPyr[i]);
}

void LKTracker::ComputeTrackWindows(int width, int height, int sample_x, int sample_y, std::vector<TrackPoint> &track_points, IplImage* mask)
{
    for(int i=0; i<width ; i+=sample_x)
        for(int j=0; j<height; j+=sample_y)
        {
			if(mask && (cvGetReal2D(mask, j, i) < 150))
				continue;
			TrackPoint tp; tp._trackingValid=true;
			tp._curLocation = Point2d((double)i, (double)j);
            track_points.push_back(tp);
        }
}

void LKTracker::SetGradientImg(IplImage* cur_img, IplImage* x_gradient_img, IplImage* y_gradient_img)
{
	const int min_x = FRAME_MARGIN, max_x = cur_img->width-FRAME_MARGIN;
	const int min_y = FRAME_MARGIN, max_y = cur_img->height-FRAME_MARGIN;

	for(int j=min_y; j<max_y; ++j)
	{
		//char* x_grad = (char*)cvPtr2D(x_gradient_img, j, min_x*_numChannels);
		//char* y_grad = (char*)cvPtr2D(y_gradient_img, j, min_x*_numChannels);
		//uchar* src_ptr_prev = cvPtr2D(cur_img, j-1, min_x*_numChannels);
		//uchar* src_ptr_cur = cvPtr2D(cur_img, j, min_x*_numChannels);
		//uchar* src_ptr_next = cvPtr2D(cur_img, j+1, min_x*_numChannels);
		char* x_grad = (char*)cvPtr2D(x_gradient_img, j, min_x);
		char* y_grad = (char*)cvPtr2D(y_gradient_img, j, min_x);
		uchar* src_ptr_prev = cvPtr2D(cur_img, j - 1, min_x);
		uchar* src_ptr_cur = cvPtr2D(cur_img, j, min_x);
		uchar* src_ptr_next = cvPtr2D(cur_img, j + 1, min_x);
		for(int i=min_x; i<max_x; ++i)
		{
			for(int c=0; c<_numChannels; ++c, ++x_grad, ++y_grad, ++src_ptr_prev, ++src_ptr_cur, ++src_ptr_next)
			{
				*x_grad = (src_ptr_cur[_numChannels] - src_ptr_cur[-_numChannels])/2;
				*y_grad = (*src_ptr_next - *src_ptr_prev)/2;
			}
		}
	}
}

void LKTracker::PyrDown(IplImage* src, IplImage* dst)
{
	const int dst_width = dst->width;
	const int dst_height = dst->height;
	const int last_col = dst_width-1;

	const int second_pix = _numChannels;
	const int third_pix = 2*_numChannels;

	const int dst_step = _numChannels;
	const int src_step = 2*_numChannels;

	bool src_height_odd = (src->height % 2) != 0;
	bool src_width_odd = (src->width % 2) != 0;

	for(int row=0; row<dst_height; row++)
	{
		uchar* src_ptr1 = (uchar*)cvPtr2D(src, row*2, 0);
		uchar* src_ptr2 = (uchar*)cvPtr2D(src, row*2+1, 0);
		uchar* src_ptr3 = NULL;
		if(src_height_odd)
			src_ptr3 = (uchar*)cvPtr2D(src, row*2+2, 0);
		else{
			if((row+1)<dst_height)
				src_ptr3 = (uchar*)cvPtr2D(src, row*2+2, 0);
			else
				src_ptr3 = (uchar*)cvPtr2D(src, row*2+1, 0);
		}

		uchar* dst_ptr = (uchar*)cvPtr2D(dst, row, 0);

		for (int col=0; col<last_col; col++)
		{
			for(int c=0; c<_numChannels; ++c)
			{
				int val = (src_ptr1[0] + 2*src_ptr1[second_pix] + src_ptr1[third_pix]) +
						  (src_ptr2[0] + 2*src_ptr2[second_pix] + src_ptr2[third_pix]) * 2 +
						  (src_ptr3[0] + 2*src_ptr3[second_pix] + src_ptr3[third_pix]);
				*dst_ptr = (val/16);

				src_ptr1++;	src_ptr2++;	src_ptr3++;
				dst_ptr++;
			}
			src_ptr1+=_numChannels;	src_ptr2+=_numChannels;	src_ptr3+=_numChannels;
		}

		// Handle the last column
		if(!src_width_odd){
			for(int c=0; c<_numChannels; ++c)
			{
				int val = (src_ptr1[0] + 2*src_ptr1[second_pix] + src_ptr1[second_pix]) +
					(src_ptr2[0] + 2*src_ptr2[second_pix] + src_ptr2[second_pix]) * 2 +
					(src_ptr3[0] + 2*src_ptr3[second_pix] + src_ptr3[second_pix]);
				*dst_ptr = (val/16);

				src_ptr1++;	src_ptr2++;	src_ptr3++;
				dst_ptr++;
			}
		}else{
			for(int c=0; c<_numChannels; ++c)
			{
				int val = (src_ptr1[0] + 2*src_ptr1[second_pix] + src_ptr1[third_pix]) +
					(src_ptr2[0] + 2*src_ptr2[second_pix] + src_ptr2[third_pix]) * 2 +
					(src_ptr3[0] + 2*src_ptr3[second_pix] + src_ptr3[third_pix]);
				*dst_ptr = (val/16);

				src_ptr1++;	src_ptr2++;	src_ptr3++;
				dst_ptr++;
			}
		}
	}
}

void LKTracker::ShowGradientImg()
{
	IplImage* gx = cvCreateImage(cvGetSize(_xGradientImg), 8, _numChannels);
	IplImage* gy = cvCreateImage(cvGetSize(_yGradientImg), 8, _numChannels);
	for(int j=0; j<_xGradientImg->height; ++j)
	{
		char* src_gx_ptr = (char*)cvPtr2D(_xGradientImg, j, 0);
		char* src_gy_ptr = (char*)cvPtr2D(_yGradientImg, j, 0);
		uchar* gx_ptr = cvPtr2D(gx, j, 0);
		uchar* gy_ptr = cvPtr2D(gy, j, 0);
		for(int i=0; i<_xGradientImg->widthStep; ++i)
		{
			gx_ptr[i] = abs(src_gx_ptr[i]);
			gy_ptr[i] = abs(src_gy_ptr[i]);
		}
	}
	cvShowImage("gx", gx);
	cvShowImage("gy", gy);
	cvReleaseImage(&gx);
	cvReleaseImage(&gy);
}

void LKTracker::Track(IplImage* gray_img, std::vector<TrackPoint> &track_points)
{
	cvCopy(gray_img, _curPyr[0]);
	for(int levels=1; levels<_levels; ++levels)
		PyrDown(_curPyr[levels-1], _curPyr[levels]);
       
	for(int win=0; win<(int)track_points.size(); win++)
	{
		TrackPoint& tp = track_points[win];
		tp._tx = tp._ty = 0;
		if(tp._curLocation.x<5 || tp._curLocation.x > gray_img->width-5 || tp._curLocation.y<5 || tp._curLocation.y > gray_img->height-5 )
			tp._trackingValid = false;
	}
    
	for(int levels=_levels-1; levels>=_highestLevel2Track; --levels)
	{
		_curImg = _curPyr[levels]; _prevImg = _prevPyr[levels]; 
		_xGradientImg = _xGradientPyr[levels]; _yGradientImg = _yGradientPyr[levels];
   
        double scale = (double)_curImg->width / (double)gray_img->width;
        int win_width = (int)MAX(TRACK_WIN_WIDTH*scale , 5);
        int win_height = (int)MAX(TRACK_WIN_HEIGHT*scale , 5);
        
		//cvShowImage("prev", _prevImg);
		//cvShowImage("cur", _curImg);
		//ShowGradientImg();
		//cvWaitKey(0);
        
        //PRINT("Level %d\n",levels);
        for(int win=0; win<(int)track_points.size(); win++)
        {
            if(track_points[win]._trackingValid == false)
                continue;
            track_points[win]._trackingValid = false;
            CvRect scaled_window =cvRect(cvFloor(track_points[win]._curLocation.x*scale)-win_width, 
                                         cvFloor(track_points[win]._curLocation.y*scale)-win_height,
                                         win_width*2+1, win_height*2+1);
            
            for(int iter=0; iter<REG_MAX_ITER; ++iter)
            {
                if(iter == 0)
                {
                    ComputeAandBMatrices(track_points[win]._tx, track_points[win]._ty, scaled_window);
                    bool inv_success = ComputeAInverse();
                    if(!inv_success)
                        break;
                }else
                    ComputeOnlyBMatrix(track_points[win]._tx, track_points[win]._ty, scaled_window);
                
                track_points[win]._trackingValid = UpdateMotion(track_points[win]._tx, track_points[win]._ty);
                //PRINT("Iter %d, %f, %f\n",iter, m[2], m[5]);
                if(track_points[win]._trackingValid)
                {
                    break;
                }
            }
            if(levels != 0){
                double scale = (double)_prevPyr[levels-1]->width/(double)_prevPyr[levels]->width;
                track_points[win]._tx *= scale;
				track_points[win]._ty *= scale;
            }
            //PRINT("- %.3f, %.3f\n", *x, *y);        
        }
	}
	
    for(int win=0; win<(int)track_points.size(); win++)
    {
        for(int levels=_highestLevel2Track-1; levels > 0; --levels)
        {
            double scale = (double)_prevPyr[levels-1]->width/(double)_prevPyr[levels]->width;
			track_points[win]._tx *= scale;
			track_points[win]._ty *= scale;
        }
        //to make it consistent with the API output
        track_points[win]._tx = -track_points[win]._tx;
		track_points[win]._ty = -track_points[win]._ty;
        track_points[win]._newLocation.x = track_points[win]._curLocation.x + track_points[win]._tx;
        track_points[win]._newLocation.y = track_points[win]._curLocation.y + track_points[win]._ty;
    }
}

bool LKTracker::UpdateMotion(double& tx, double& ty)
{
	double dx = _aInv[0]*_bMat[0] + _aInv[1]*_bMat[1];
	double dy = _aInv[2]*_bMat[0] + _aInv[3]*_bMat[1];

	tx += dx;
	ty += dy;

	bool converged = ((fabs(dx)+fabs(dy)) <  EPSILON);

	return converged;
}

bool LKTracker::ComputeAInverse()
{
	double det = (_aMat[0] * _aMat[3]) - (_aMat[1] * _aMat[2]);
	if(fabs(det) < 0.1f)
		return false;

	double det_inv = 1.0f/det;
	_aInv[3] = _aMat[0] *  det_inv;
	_aInv[1] = _aMat[1] * -det_inv;
	_aInv[2] = _aMat[2] * -det_inv;
	_aInv[0] = _aMat[3] *  det_inv;

	return true;
}

void LKTracker::ComputeAandBMatrices(double& tx_cur2prev, double& ty_cur2prev, CvRect win)
{
	memset(_bMat, 0, 2*sizeof(double));
	memset(_aMat, 0, 4*sizeof(double));

	double current_mot_x = -tx_cur2prev;
	double current_mot_y = -ty_cur2prev;

	const int mot_x_int = cvFloor(current_mot_x);
	const int mot_y_int = cvFloor(current_mot_y);

	const double wx = current_mot_x - mot_x_int;
	const double wy = current_mot_y - mot_y_int;

	const double w1 = (1.0f-wx)*(1.0f-wy);
	const double w2 = (wx)*(1.0f-wy);
	const double w3 = (wx)*(wy);
	const double w4 = (1.0f-wx)*(wy);

	int min_x = MAX(FRAME_MARGIN, FRAME_MARGIN-cvCeil(current_mot_x));
	int min_y = MAX(FRAME_MARGIN, FRAME_MARGIN-cvCeil(current_mot_y));
	int max_x = MIN(_curImg->width-FRAME_MARGIN, _curImg->width-FRAME_MARGIN-cvCeil(current_mot_x));
	int max_y = MIN(_curImg->height-FRAME_MARGIN, _curImg->height-FRAME_MARGIN-cvCeil(current_mot_y));
    min_x=MAX(min_x, win.x);
    min_y=MAX(min_y, win.y);
    max_x=MIN(max_x, win.x+win.width);
    max_y=MIN(max_y, win.y+win.height);

    if( (min_x>=max_x) || (min_y>=max_y) )
    {
//        PRINT("Motion too high for current window\n");
        return;
    }
    
	for (int y=min_y; y < max_y; y++)
	{
		char* xGradientPtr = (char*)cvPtr2D(_xGradientImg, y, min_x);
		char* yGradientPtr = (char*)cvPtr2D(_yGradientImg, y, min_x);
		uchar* prevImgPtr = cvPtr2D(_prevImg, y, min_x);
		uchar* curImgPtr1 = cvPtr2D(_curImg, y+mot_y_int, min_x+mot_x_int);
		uchar* curImgPtr2 = cvPtr2D(_curImg, y+mot_y_int+1, min_x+mot_x_int);

		for (int x=min_x; x<max_x; x++)
		{
			for(int c=0; c<_numChannels; ++c, prevImgPtr++, xGradientPtr++, yGradientPtr++, curImgPtr1++, curImgPtr2++)
			{
				double curPix = w1*curImgPtr1[0] + w2*curImgPtr1[_numChannels] + w3*curImgPtr2[_numChannels] + w4*curImgPtr2[0];
				double it = *prevImgPtr - curPix;				

				_bMat[0] -= it * (*xGradientPtr);
				_bMat[1] -= it * (*yGradientPtr);

				_aMat[0] += (*xGradientPtr) * (*xGradientPtr);
				_aMat[1] += (*xGradientPtr) * (*yGradientPtr);
				_aMat[3] += (*yGradientPtr) * (*yGradientPtr);
			}
		}	
	}
	_aMat[2] = _aMat[1];
}

void LKTracker::ComputeOnlyBMatrix(double& tx_cur2prev, double& ty_cur2prev, CvRect win)
{
	memset(_bMat, 0, 2*sizeof(double));

	double current_mot_x = -tx_cur2prev;
	double current_mot_y = -ty_cur2prev;

	const int mot_x_int = cvFloor(current_mot_x);
	const int mot_y_int = cvFloor(current_mot_y);
    
	const double wx = current_mot_x - mot_x_int;
	const double wy = current_mot_y - mot_y_int;
    
	const double w1 = (1.0f-wx)*(1.0f-wy);
	const double w2 = (wx)*(1.0f-wy);
	const double w3 = (wx)*(wy);
	const double w4 = (1.0f-wx)*(wy);
    
	int min_x = MAX(FRAME_MARGIN, FRAME_MARGIN-cvCeil(current_mot_x));
	int min_y = MAX(FRAME_MARGIN, FRAME_MARGIN-cvCeil(current_mot_y));
	int max_x = MIN(_curImg->width-FRAME_MARGIN, _curImg->width-FRAME_MARGIN-cvCeil(current_mot_x));
	int max_y = MIN(_curImg->height-FRAME_MARGIN, _curImg->height-FRAME_MARGIN-cvCeil(current_mot_y));
    min_x=MAX(min_x, win.x);
    min_y=MAX(min_y, win.y);
    max_x=MIN(max_x, win.x+win.width);
    max_y=MIN(max_y, win.y+win.height);
    
    if( (min_x>=max_x) || (min_y>=max_y) )
    {
//        PRINT("Motion too high for current window\n");
        return;
        
    }
     
	for (int y=min_y; y < max_y; y++)
	{
		char* xGradientPtr = (char*)cvPtr2D(_xGradientImg, y, min_x);
		char* yGradientPtr = (char*)cvPtr2D(_yGradientImg, y, min_x);
		uchar* prevImgPtr = cvPtr2D(_prevImg, y, min_x);
		uchar* curImgPtr1 = cvPtr2D(_curImg, y+mot_y_int, min_x+mot_x_int);
		uchar* curImgPtr2 = cvPtr2D(_curImg, y+mot_y_int+1, min_x+mot_x_int);

		for (int x=min_x; x<max_x; x++)
		{
			for(int c=0; c<_numChannels; ++c, prevImgPtr++, xGradientPtr++, yGradientPtr++, curImgPtr1++, curImgPtr2++)
			{
				double curPix = w1*curImgPtr1[0] + w2*curImgPtr1[_numChannels] + w3*curImgPtr2[_numChannels] + w4*curImgPtr2[0];
				double it = *prevImgPtr - curPix;				
				_bMat[0] -= it * (*xGradientPtr);
				_bMat[1] -= it * (*yGradientPtr);
			}
		}	
	}
}

