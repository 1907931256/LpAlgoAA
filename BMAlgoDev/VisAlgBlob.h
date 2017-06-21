#pragma once
#include "ViType.h"
#include <ipp.h>
#include <mkl.h>

class CVisAlgBlob
{
public:
	CVisAlgBlob();
	~CVisAlgBlob();

	int BlobLabelMarker(IMG_UBBUF src, int &markersNum);
	int BlobAreas(IMG_UBBUF src);
	int GetMaxAreaBlob(IMG_UBBUF src, int &maxBlobArea);

private:
	int m_minLabel;
	int m_maxLabel;
	int m_maxAreaBlobIndex;
	int m_minAreaBlobIndex;
	int m_pBlobAreas[256];
};

