#pragma once

#include "AbstractVideoReader.h"

class QTVideoReader : public AbstractVideoReader
{
public:
	~QTVideoReader(void);

	bool Open(const char* filename);
	int GetTotalFrames();
	bool SetFrameNum(int fnum);
	IplImage* GetFrame();
	int GetFPS();

private:
	void MyConvert(IplImage* inp, IplImage* out);

private:
	int _nextFrame2Read;
	void* _videoReader;
	//Movie _videoReader;
	IplImage* _frameBuffer4Ch, *_frameBuffer3Ch;
	static bool QT_INITIALIZED;
	int _numFrames;
	double _frameRate;
	double _videoLength;
	std::vector<long> _framesTime;
	short _fileId;
	//GWorldPtr _graphicsPtr;
	void* _graphicsPtr;
};
