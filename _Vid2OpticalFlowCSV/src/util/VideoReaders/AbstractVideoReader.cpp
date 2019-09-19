#include "stdafx.h"
#include "AbstractVideoReader.h"
#include "DXVideoReader.h"
#include "OpenCVVideoReader.h"
#undef CV_INLINE
//#include "QTVideoReader.h"

AbstractVideoReader* AbstractVideoReader::Create(ReaderTypeEnum type)
{
	switch(type)
	{
	case READER_CV: return new OpenCVVideoReader();
#ifndef NO_DX
	case READER_DX: return new DXVideoReader();
#endif
	//case READER_QT: return new QTVideoReader(); 
	default: printf("Unknown reader type\n"); return NULL;
	}
	return NULL;
}