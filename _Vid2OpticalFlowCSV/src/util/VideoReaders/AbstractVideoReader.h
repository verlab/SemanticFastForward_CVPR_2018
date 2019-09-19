#pragma once

class AbstractVideoReader
{
public:
	virtual ~AbstractVideoReader() {}
	virtual bool Open(const char* filename) = 0;
	virtual int GetTotalFrames() = 0;
	virtual bool SetFrameNum(int fnum) = 0;
	virtual IplImage* GetFrame() = 0;
	virtual int GetFPS() = 0;

	void operator >> (cv::Mat& rhs) {
		IplImage* img = GetFrame();
		rhs = cv::Mat(img);
	}

	enum ReaderTypeEnum {READER_NONE=0, READER_QT, READER_DX, READER_CV};

	static AbstractVideoReader* Create(ReaderTypeEnum type);	
};
