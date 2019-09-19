#include "stdafx.h"
#include "DXVideoReader.h"

#ifndef NO_DX

#include <Qedit.h>
#include <atlstr.h>
#include <DShow.h>

bool DXVideoReader::WIN_COM_INITIALIZED = false;

bool DXVideoReader::Open(const char* filename)
{
	_videoReader = NULL;
	_frameBuffer = NULL;
	_dataBuffer = NULL;
	_numFrames = -1;
	_nextFrame2Read = 0;

	HRESULT hr;

	if(!WIN_COM_INITIALIZED){
		hr = CoInitialize(NULL);
		if (FAILED(hr)) {
			_numFrames = -1;
			_videoReader = NULL;
			return false;
		}
		WIN_COM_INITIALIZED = true;
	}

	hr = CoCreateInstance(CLSID_MediaDet, NULL, CLSCTX_INPROC_SERVER,IID_IMediaDet, &_videoReader);
	if (FAILED(hr)) {
		_videoReader = NULL;
		return false;
	}

	IMediaDet* pDet = (IMediaDet*)_videoReader;
	CString str(filename);
	hr = pDet->put_Filename((BSTR)str.GetBuffer());
	if (FAILED(hr)) 
		return false;

	long num_streams;
	hr = pDet->get_OutputStreams(&num_streams);
	if (FAILED(hr)) 
		return false;

	AM_MEDIA_TYPE media_type;
	long i;
	for(i=0; i<num_streams; ++i)
	{
		hr = pDet->put_CurrentStream(i);
		if (FAILED(hr)) {
			_numFrames = -1;
			return false;
		}
		hr = pDet->get_StreamMediaType(&media_type);
		if (FAILED(hr)) {
			_numFrames = -1;
			return false;
		}
		if(media_type.majortype == MEDIATYPE_Video)
			break;
	}
	if(i == num_streams)
		return false;

	int width, height;
	if (media_type.formattype == FORMAT_VideoInfo)
	{
		// Check the buffer size.
		if (media_type.cbFormat >= sizeof(VIDEOINFOHEADER))
		{
			VIDEOINFOHEADER *pVih = reinterpret_cast<VIDEOINFOHEADER*>(media_type.pbFormat);
			width = pVih->bmiHeader.biWidth;
			height = pVih->bmiHeader.biHeight;
		}
	}

	hr = pDet->get_StreamLength(&_videoLength);
	if (FAILED(hr)) 
		return false;

	hr = pDet->get_FrameRate(&_frameRate);
	if (FAILED(hr)) 
		return false;

	_frameBuffer = cvCreateImageHeader(cvSize(width, height), IPL_DEPTH_8U, 3);

	long size;
	hr = pDet->GetBitmapBits(0, &size, 0, width, height);
	if (FAILED(hr)) 
		return false;

	_dataBuffer = new char[size];
	BITMAPINFOHEADER *bmih = (BITMAPINFOHEADER*)_dataBuffer;
	char *imgData = _dataBuffer + sizeof(BITMAPINFOHEADER);

	_frameBuffer->imageData = imgData;

	_numFrames = cvRound(_videoLength*_frameRate);

	return true;
}

DXVideoReader::~DXVideoReader(void)
{
	if(_videoReader){
		IMediaDet* pDet = (IMediaDet*)_videoReader;
		pDet->Release();
	}
	if(_frameBuffer)
		cvReleaseImageHeader(&_frameBuffer);
	if(_dataBuffer)
		delete[] _dataBuffer;

	if(WIN_COM_INITIALIZED){
		CoUninitialize();
		WIN_COM_INITIALIZED = false;
	}
}

int DXVideoReader::GetTotalFrames()
{
	return _numFrames;
}

int DXVideoReader::GetFPS()
{
	return cvRound(_frameRate);
}

bool DXVideoReader::SetFrameNum(int frame_num)
{
	_nextFrame2Read = frame_num;
	return true;
}

IplImage* DXVideoReader::GetFrame()
{
	double time = (_videoLength*_nextFrame2Read)/(double)_numFrames;

	IMediaDet* pDet = (IMediaDet*)_videoReader;
	HRESULT hr = pDet->GetBitmapBits(time, 0, _dataBuffer, _frameBuffer->width, _frameBuffer->height);
	if (FAILED(hr)) 
		return false;

	++_nextFrame2Read;
	cvFlip(_frameBuffer, _frameBuffer);

	return _frameBuffer;
}


#endif