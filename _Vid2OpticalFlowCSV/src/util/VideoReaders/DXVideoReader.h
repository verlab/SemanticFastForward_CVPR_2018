#pragma once

#include "AbstractVideoReader.h"
#ifndef NO_DX
class DXVideoReader : public AbstractVideoReader
{
public:
	~DXVideoReader(void);

	bool Open(const char* filename);
	int GetTotalFrames();
	bool SetFrameNum(int fnum);
	IplImage* GetFrame();
	int GetFPS();

private:
	void* _videoReader;
	IplImage* _frameBuffer;
	char* _dataBuffer;
	static bool WIN_COM_INITIALIZED;
	int _numFrames;
	double _frameRate;
	double _videoLength;
	int _nextFrame2Read;
};
#endif