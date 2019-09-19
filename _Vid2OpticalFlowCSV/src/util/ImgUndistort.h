#pragma once

//based upon parameters from http://www.htw-mechlab.de/index.php/undistortion-der-gopro-hd-hero2/

class ImgUndistort
{
public:
	ImgUndistort(int width, int height);
	~ImgUndistort(void);

	IplImage* Undistort(IplImage* inp);

private:
	IplImage* map1, *map2;
	IplImage* out_img;
};
